/* 

 Copyright (c) 2013-2016, Tomas Podermanski
    
 This file is part of libnf.net project.

 Libnf is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 Libnf is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with libnf.  If not, see <http://www.gnu.org/licenses/>.*/



/*  This is an alterned code of lnf_ex12 from libnf project*/

#include "libnf.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define LLUI long long unsigned int

#include "hashtbl.h"
#include "queue.h"
#include "query.h"
#include "structure.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
/*#define _SVID_SOURCE*/

#include <dirent.h>
#include "log.h"
#include "udp.h"

#include "btree.h"
int filter(const struct dirent * file)
{
	char ptr[256];
	strncpy(ptr, file->d_name, 256);
	char* token = NULL;
	token = strtok(ptr, ".");
	if(token == NULL)
		return 0;
	if(strcmp(token, "nfcapd") == 0)
	{
		token = strtok(NULL, ".");
		while(*token != '\0')
			if(isdigit(*(token++)) == 0)
				return 0;
		/*printf("File: %s\n",token);*/
		return 1;
	}
	return 0;
}
/* print files in current directory in reverse order */

/* List nfcapd files*/
/* Returns the count of files*/
int listFiles(char* dir, struct dirent ***files)
{
	int n;
   n = scandir(".", files, filter, NULL);
	if (n < 0)
		perror("scandir");
	return n;
}

void freeFiles(struct dirent **namelist, int n)
{
	while (n--) {
            free(namelist[n]);
        }
        free(namelist);
}


/* This function takes NetFlow dump file with name 'path' and creates a new*/
/* file with process names*/
int alternFile(const char* path, hash_tab_t* processes, queue_t* front)
{
	printf("Alterning file %s\n", path);	
	lnf_file_t *filep_in;
	lnf_file_t *filep_out;
	lnf_rec_t *recp_in;
	lnf_rec_t *recp_out;
	/*lnf_brec1_t brec;*/
	char buf[LNF_REC_RAW_TLV_BUFSIZE];
	size_t size;

	/* append 'out' to alterned file*/
	char *filename_out = malloc(strlen(path)+1+3);
	if(filename_out == NULL)
	{
		perror("malloc failed\n");
		return 0;
	}

	sprintf(filename_out,"out%s", path);
	int i = 0;
	int j = 0;
	int k = 0;
	int updates = 0;
	
	uint64_t flowEndTime;
	if (lnf_open(&filep_in, path, LNF_READ, NULL) != LNF_OK) {
		fprintf(stderr, "Can not open file %s\n", path);
		exit(1);
	}

	if (lnf_open(&filep_out, filename_out, LNF_WRITE, NULL) != LNF_OK) {
		fprintf(stderr, "Can not open file %s\n", filename_out);
		exit(1);
	}

	lnf_rec_init(&recp_in);
	lnf_rec_init(&recp_out);

	/* update front before each file*/
	/*updateFront(front);*/
	/* hash tree key*/
	data_t key,keyB;
	/* for each entry*/
	while (lnf_read(filep_in, recp_in) != LNF_EOF) {

		i++;

		/* try to get a raw copy of data*/
		if (lnf_rec_get_raw(recp_in, LNF_REC_RAW_TLV, buf, sizeof(buf), &size) == LNF_OK) {
			
			uint8_t proto;
			/* determine the time*/
			/*lnf_rec_fget(recp_in, LNF_FLD_BREC1, &brec);*/
			lnf_rec_fget(recp_in, LNF_FLD_LAST,&flowEndTime);
			lnf_rec_fget(recp_in, LNF_FLD_SRCADDR,&key.addr);
			lnf_rec_fget(recp_in, LNF_FLD_DSTADDR,&keyB.addr);
			lnf_rec_fget(recp_in, LNF_FLD_SRCPORT,&key.port);
			lnf_rec_fget(recp_in, LNF_FLD_DSTPORT,&keyB.port);
			lnf_rec_fget(recp_in, LNF_FLD_PROT,&proto);

		
			/* skip IPv6 flow
			if(!IN6_IS_ADDR_V4COMPAT((struct in6_addr *) &key.addr))
			{
				perror("Skipping IPv6\n");
				continue;
			}
*/

			#define UDP_PROTO 17

			key.protocol = P_TCP; 
			if(proto == UDP_PROTO)
				key.protocol = P_UDP; 

			while(queue_length(front) > 0)
			{
				/* get the top of front*/
				data_t* dt = queue_gettop(front);
				if(dt->timestamp < flowEndTime)
				{
					updates++;
					/* update hashtable*/
					hash_tab_add(processes, dt->addr, dt->port,dt->protocol, dt->program);
					queue_pop(front);
					continue;
				} 
				break;
			}
			
			/* default process*/
			char* def= "none";
			data_t* it = hash_tab_find(processes, key.addr, key.port,key.protocol);
			if(!it)
				it = hash_tab_find(processes, keyB.addr, keyB.port,key.protocol);
			if(it)
			{
				LOG("Searching for port %d and proto %d\n", key.port,key.protocol);
				def = it->program;
				LOG("Saving %s process\n",def);
				k++;
			} else {
				struct bnode * serA = getService(key.port);
				struct bnode * serB = getService(keyB.port);
				if(serA || serB)
				{
					if(serA == NULL)	
						serA = serB;
					if(key.protocol == P_UDP)
						def = serA->udp;	
					else
						def = serA->tcp;	
					LOG("Inserting service %s\n",def);	
				}
				k++;
			}
			
			/* now, the data in buf cen be transfered somwhere else */
			
			if ( lnf_rec_set_raw(recp_out, buf, size) == LNF_OK) {
				if(lnf_rec_fset(recp_out, LNF_FLD_USERNAME, def) != LNF_OK)
					printf("ERROR\n");
				if ( lnf_write(filep_out, recp_out) == LNF_OK) {
					j++;
				} else 
					perror("lnf_write failed");
				
			} else 
					perror("lnf_set_raw failed");
		} else 
			perror("lnf_read");

	}

	lnf_rec_free(recp_in);
	lnf_rec_free(recp_out);
	lnf_close(filep_in);
	lnf_close(filep_out);
	char buff[256];

/*	RENAME old file - obsolete
 * 	sprintf(buff,"old%s",path);
	if(rename(path, buff) != 0)
		perror("Rename");
*/
	unlink(path);

	LOG("%d read, %d changed, %d flow stored in %s\n",i,k,j,path);
	printf("%d read, %d changed, %d flow stored in %s\n",i,k,j,path);
	printf("No. of updates: %d\n",updates);
	/*hash_tab_print(processes);*/

	free(filename_out);
	return 0;
}

int updateFiles(hash_tab_t* processes, queue_t* front,int delay)
{
	static int counter = 0;
	if(!(counter++ >= delay))
		return 0;
	counter = 0;

	/* process files*/
	
	printf("Updating files ...\n");
	printf("Count of items in front: %d ...\n",queue_length(front));
	struct dirent **names;
	int n = listFiles(".",&names);
	for(int i = 0; i < n; i++)
		alternFile(names[i]->d_name, processes, front);
	freeFiles(names, n);
	return 1;
}



int main(int argc, char ** argv)
{
	fprintf(stdout,"FlowUpdater\n");
	if(argc < 2)
	{
		fprintf(stderr,"USAGE: UPDATER_PORT [delayTime]\n");
		return 1;
	}

	int serverPort = atoi(argv[1]);

	int delay = 6;
	if(argc >= 3)
		delay = atoi(argv[2]);
	

	if(load_services() != 0)
	{
		printf("Failed to load IANA services. Missing services.txt ?");
		return 1;
	}

	queue_t que;
	hash_tab_t tab;

	queue_init(&que);
	hash_tab_init(&tab,100);
	LOG("Starting FlowUpdater with delay: %d0 seconds\n",delay);
	printf("Starting FlowUpdater with delay: %d0 seconds\n",delay);


	int fd = udp_start_server(serverPort);
	if(fd == -1)
	{
		err(1,"Failed to start an UDP server at port %d\n",serverPort);
		exit(1);	
	}
	
	printf("Started server at port %d.\n", serverPort);

	unsigned int ticks = 0;
	while(1 == 1)
	{
		++ticks;
		updateFront(&que,fd);
		if(ticks > 10)
		{
			updateFiles(&tab,&que,delay);
			ticks = 0;
		}
		sleep(1);
	}
}
