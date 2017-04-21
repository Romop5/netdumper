#include "netstat.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <time.h>

FILE* g_data;

#ifdef __linux__
#define SCRIPTNAME "./binds.sh"
#else
#define SCRIPTNAME "./bindsBSD.sh"
#endif

/* Initialize init module*/
void n_init()
{
}

/* load data*/
int n_load()
{
	g_data = popen(SCRIPTNAME,"r");
	return (g_data != 0);
}

int readColumn(char* col)
{
	int c;
	while((c = fgetc(g_data)) != EOF)
	{
		if(c ==  ',' || c == '\n')
			break;
		*col = c; 
		col++;	
	}
	*col = '\0';
	return (c == EOF);
}

/* return 1 on success*/
int convertIP(char* src, struct in6_addr *out)
{
	int family = (strchr(src,":") == NULL)?AF_INET:AF_INET6;
	bzero(out, sizeof(struct in6_addr));
	if(inet_pton(family, src, (struct in_data*) out) == 1)
	{
		uint32_t *data = (uint32_t*) out;
		data[3] = data[0];
		data[0] = 0;
		return 1;
	}
	return inet_pton(AF_INET6, src, out);
}

/* Fill data with a single connection*/
/* Returns 0 if no data is available*/
int n_getData(data_t* data)
{
	enum columnMap{PROTO, SIP,SPORT,DIP,DPORT, PROGRAM};
	data->timestamp = time(NULL);
	if(feof(g_data) > 0) 
		return 0;

	int sum = 0;
	char strips[6][256];
	int i = 0;
	for(i = 0; i < 6; i++)
	{
		sum += readColumn(strips[i]);
	}	
	if(sum != 0)
		return 0;
	/* get them all*/
	/* CVS:*/
	/* PROTO SRC PORT PROGRAM*/
	/* DEBUG*/
	//printf("[%s] %s:%s [%s]\n",strips[0],strips[1],strips[2],strips[3]);

	/* detect PROTO*/
	data->protocol = P_TCP;
	if(strcmp("UDP", strips[PROTO]) == 0 || strcmp("udp", strips[PROTO]) == 0 || strcmp("udp6",strips[PROTO]) == 0 || strcmp("UDP6",strips[PROTO]) == 0)
		data->protocol = P_UDP;

	/* detect SRC*/
	convertIP(strips[SIP],&data->s_addr); 
	convertIP(strips[DIP],&data->d_addr); 
	
	/* DEBUG purpose*/
	
	/*char buff[256];
	inet_ntop(AF_INET6, &data->s_addr, buff, 255);
	printf("Buff: %s\n",buff);

	inet_ntop(AF_INET6, &data->d_addr, buff, 255);
	printf("Buff: %s\n",buff);
	*/
	/* detect PORT*/
	data->s_port = atoi(strips[SPORT]);
	data->d_port = atoi(strips[DPORT]);

	/* PROGRAM*/
	strcpy(data->program, strips[PROGRAM]);	

	return 1;
}

/* free data used by module*/
void n_dtor()
{
	pclose(g_data);
}

/*
int main()
{

	FILE* dataSource = popen("./binds.sh","r");
	if(dataSource)
	{
		int c;
		while((c = getc(dataSource)) != EOF)
		{	
			putchar(c);
		}
	}
	pclose(dataSource);
}*/
