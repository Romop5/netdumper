#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include "netstat.h"


data_t tcpData[65356];
data_t udpData[65356];

void processData(data_t* data)
{
	data_t* array = tcpData;
	if(data->protocol== P_UDP)
		array = udpData;
	if(strcmp(array[data->port].program, data->program) != 0)
	{
		strcpy(array[data->port].program, data->program);
		printf("'%lu' '%s' '%d' '%s' \n",time(NULL), (data->protocol)?"UDP":"TCP",data->port, data->program);
	}
}

int main(int argc, char** argv)
{
	// initialize netstat scanner
	n_init();	
	while(1 == 1)
	{
		n_load();

		data_t data;
		while(n_getData(&data) != 0)
		{
			processData(&data);
		}
		n_dtor();
		sleep(1);
	}
	return 0;
}
