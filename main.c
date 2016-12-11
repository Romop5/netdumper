#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "netstat.h"


ndata tcpData[65356];
ndata udpData[65356];

void processData(ndata* data)
{
	ndata* array = tcpData;
	if(data->proto== UDP)
		array = udpData;
	if(strcmp(array[data->port].program, data->program) != 0)
	{
		strcpy(array[data->port].program, data->program);
		printf("'%lu' '%s' '%d' '%s' \n",time(NULL), (data->proto)?"UDP":"TCP",data->port, data->program);
	}
}

int main(int argc, char** argv)
{
	if(argc < 2)
	{
		fprintf(stderr,"USAGE: FILE_TO_STORE_DUMPS");
		return 1;
	}
	// set data to null

	// initialize netstat scanner
	n_init();	
	while(1 == 1)
	{
		n_load();

		ndata data;
		while(n_getData(&data) != 0)
		{
			processData(&data);
		}
		n_dtor();
		sleep(1);
	}
	return 0;
}
