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
 along with libnf.  If not, see <http://www.gnu.org/licenses/>.

*/

//  This is an alterned code of lnf_ex12 from libnf project

#include "libnf.h"
#include <arpa/inet.h>
#include <sys/socket.h>
#include <unistd.h>

#define LLUI long long unsigned int

#include "hashtbl.h"
#include "queue.h"
#include "structure.h"

// This function takes NetFlow dump file with name 'path' and creates a new
// file with process names
int alternFile(const char* path, hash_tab_t* processes, queue_t* front)
{
	lnf_file_t *filep_in;
	lnf_file_t *filep_out;
	lnf_rec_t *recp_in;
	lnf_rec_t *recp_out;
	char buf[LNF_REC_RAW_TLV_BUFSIZE];
	size_t size;

	// append 'out' to alterned file
	char *filename_out = malloc(strlen(path)+1+3);
	if(filename_out == NULL)
	{
		perror("malloc failed\n");
		return 0;
	}

	sprintf(filename_out,"%sout", path);
	int i = 0;
	int j = 0;
	int c;
	
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

	// update front before each file
	updateFront(front);
	// hash tree key
	data_t key;
	// for each entry
	while (lnf_read(filep_in, recp_in) != LNF_EOF) {

		i++;

		// try to get a raw copy of data
		if (lnf_rec_get_raw(recp_in, LNF_REC_RAW_TLV, buf, sizeof(buf), &size) == LNF_OK) {
			
			int proto;
			lnf_ip_t addr;
			// determine the time
			lnf_rec_fget(recp_in, LNF_FLD_LAST,&flowEndTime);
			lnf_rec_fget(recp_in, LNF_FLD_SRCADDR,&addr);
			lnf_rec_fget(recp_in, LNF_FLD_SRCPORT,&key.port);
			lnf_rec_fget(recp_in, LNF_FLD_PROT,&proto);
		
			// skip IPv6 flow
			if(!IN6_IS_ADDR_V4COMPAT((struct in6_addr *) &addr))
			{
				perror("Skipping IPv6\n");
				continue;
			}

			// copy last 4 bytes of IPv6 address => get IPv4
			key.addr4 = addr.data[3];
	
			#define UDP_PROTO 0x17

			key.protocol = P_TCP; 
			if(proto == UDP_PROTO)
				key.protocol = P_UDP; 

			while(queue_length(front) > 0)
			{
				// get the top of front
				data_t* dt = queue_gettop(front);
				if(dt->timestamp < flowEndTime)
				{
					fprintf(stderr,"Updating hashtable.\n");
					// update hashtable
					hash_tab_add(processes, dt->addr4, dt->port,dt->protocol, dt->program);
					continue;
				} else {
				}
				break;
			}
			
			// default process
			char* def= "NO_DATA";
			shash_item_t* it = hash_tab_find(processes, key.addr4, key.port,key.protocol);
			if(it)
				def = it->data.program;
			
			/* now, the data in buf cen be transfered somwhere else */
			
			if ( lnf_rec_set_raw(recp_out, buf, size) == LNF_OK) {
				if(lnf_rec_fset(recp_out, LNF_FLD_USERNAME, def) != LNF_OK)
					printf("ERROR\n");
				if ( lnf_write(filep_out, recp_out) == LNF_OK) {
					j++;
				}
			}
		}

	}

	lnf_rec_free(recp_in);
	lnf_rec_free(recp_out);
	lnf_close(filep_in);
	lnf_close(filep_out);

	return 0;
}

int main()
{
	queue_t que;
	hash_tab_t tab;
	queue_init(&que);
	hash_tab_init(&tab,100);

	printf("Hello\n");
	alternFile("test-file.tmp",&tab,&que);

	

}
