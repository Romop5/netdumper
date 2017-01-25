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

// Initialize init module
void n_init()
{
}

// load data
int n_load()
{
	g_data = popen("./binds.sh","r");
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

// return 1 on success
int convertIP(char* src, struct in6_addr *out)
{
	bzero(out, sizeof(struct in6_addr));
	if(inet_pton(AF_INET, src, (struct in_data*) out) == 1)
	{
		uint32_t *data = (uint32_t*) out;
		data[3] = data[0];
		data[0] = 0;
		return 1;
	}
	return inet_pton(AF_INET6, src, out);
}

// Fill data with a single connection
// Returns 0 if no data is available
int n_getData(data_t* data)
{
	data->timestamp = time(NULL);
	if(feof(g_data) > 0) 
		return 0;

	int sum = 0;
	char strips[4][256];
	for(int i = 0; i < 4; i++)
	{
		sum += readColumn(strips[i]);
	}	
	if(sum != 0)
		return 0;
	// get them all
	// CVS:
	// PROTO SRC PORT PROGRAM
	// DEBUG
	//printf("[%s] %s:%s [%s]\n",strips[0],strips[1],strips[2],strips[3]);

	// detect PROTO
	data->protocol = P_TCP;
	if(strcmp("UDP", strips[0]) == 0 || strcmp("udp", strips[0]) == 0)
		data->protocol = P_UDP;

	// detect SRC
	convertIP(strips[1],&data->addr); 
	
	// DEBUG purpose
/*	
	char buff[256];
	inet_ntop(AF_INET6, &data->addr, buff, 255);
	printf("Buff: %s\n",buff);
*/
	// detect PORT
	data->port = atoi(strips[2]);

	// PROGRAM
	strcpy(data->program, strips[3]);	

	return 1;
}

// free data used by module
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
