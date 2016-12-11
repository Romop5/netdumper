#include <stdio.h>
#include <string.h>
#include "netstat.h"

int main()
{
	// initialize netstat scanner
	n_init();	
	
	n_load();

	ndata data;
	while(n_getData(&data) != 0)
	{
		printf("Proto: %d Port: %d Program: %s\n",
			data.proto, data.port, data.program);
	}
	n_dtor();
}
