#include <stdio.h>
#include "hosts.h"
#include "netstat.h"
#include <string.h>
#include <stdlib.h>

#include <time.h>

#include "netstat.h"

#ifdef __linux__
#define HOSTSCRIPT "./hosts.sh"
#else
#define HOSTSCRIPT "./hostsBSD.sh"
#endif

int printIP(struct in6_addr addr)
{
/* DEBUG purpose*/
	
	char buff[256];
	inet_ntop(AF_INET6, &addr, buff, 255);
	printf("IP: %s\n",buff);
	return 0;
}

/* Returns 0 on OK*/
int isBlacklisted(char* addr)
{
	if(strcmp(addr,"127.0.0.1") == 0)
		return 1;
	
	if(strcmp(addr,"::1") == 0)
		return 1;
	return 0;
}

int getHosts(struct in6_addr* arrayOfAddresses,int slots)
{
	FILE* h = popen(HOSTSCRIPT,"r");
	if(h)
	{	
		int i = 0;
		while(feof(h) == 0)
		{
			struct in6_addr addr;
			char line[256];
			if(NULL == fgets(line,256,h))
				break;
			/* line end removal*/
			char *pos = strchr(line,'\n');
			if(pos)
				*pos = '\0';
			/* skip black listed addresses*/
			if(isBlacklisted(line))
				continue;
			printf("Host IP: '%s'\n",line);
			if(convertIP(line,&addr) != 1)	
				perror("Convert failed");
			arrayOfAddresses[i] = addr;
			i++;
		}
		pclose(h);
		return i;
	}
	return 0;
}

/*int main()
{
	struct in6_addr adds[10];
	int count = getHosts(adds, 10);
	for(int i = 0; i < count;i++)
		printIP(adds[i]);
}*/
