#include "queue.h"
#include "hashtbl.h"
#include "udp.h"

#include "netstat.h"
#include "hosts.h"
#include <strings.h>

/* stores triples (ip, port, program)*/
hash_tab_t g_binds;

/* a queue of triples (ip, port, program) which to be sent out */
queue_t g_outData;

struct in6_addr empty = {0,};

/* machine interfaces*/
struct in6_addr hosts[10];

/* count of machine interfaces*/
int hostsCount;

/* Return 1 on update*/
int updateProgram(data_t* data)
{
	data_t* dt = hash_tab_find(&g_binds, data->addr, data->port, data->protocol);
	if(dt)
	{
		/* if nothing has changed, scan for another*/
		if(strcmp(dt->program, data->program) == 0)
			return 0;
	} 
	
	hash_tab_add(&g_binds, data->addr, data->port, data->protocol, data->program);

	/* append into queue and altern timestamp*/
	/*time_t t = time();*/
	/*data.timestamp = t;*/
	queue_append(&g_outData,*data); 
	return 1;
	

}



int updatePortBinds()
{
	static int count = 0; 
	count++;
	if(count < 100)
		return 0;
	count = 0; 
	n_load();
	data_t data; 
	int i = 0;
	while(n_getData(&data) != 0)
	{
		if(data.program[0] == '-')
			continue;

		if(updateProgram(&data) == 1)
		{	
			i++;
			if(bcmp(&data.addr, &empty, sizeof(struct in6_addr)) == 0)
			{
				for(int i = 0; i < hostsCount; i++)
				{
					data.addr = hosts[i];
					if(updateProgram(&data) == 1)
						i++;
				}
			}
		}
		
	}
	printf("New queue elements count %d\n",i);
	
	hash_tab_print(&g_binds);
/*	queue_print(&g_outData);*/
	n_dtor();
	
	return 0;
}

void sendDataOut(int fd, peer_t* p)
{
	printf("Sending out data...\n");
	int count = queue_length(&g_outData);		
	/* message size limitation*/
	if(count > 20)
		count = 20;
	/* create an output buffer for entries with header*/
	int output_len = sizeof(struct query_msg)+sizeof(data_t)*count;
	struct query_msg *msg = malloc(output_len);
	if(msg)
	{
		msg->count = count;	
		for(int i = 0; i < count; i++)
		{
			memcpy(&msg->items[i], queue_gettop(&g_outData),sizeof(data_t));
			queue_pop(&g_outData);
		}
		/* send it out*/
		udp_sendTo(fd, p, (char*) msg, output_len);

		free(msg);
	}
}




int main(int argc, char ** argv)
{
	fprintf(stdout,"ProcessTracker\n");
	if(argc < 2)
	{

		fprintf(stderr,"USAGE: portNum\n");
		fprintf(stderr,"Note: portNum is an UDP port to listed at.\n");
		return 1;
	}	
	int port = atoi(argv[1]);

	queue_init(&g_outData);
	hash_tab_init(&g_binds,100);
	
	hostsCount = getHosts(hosts,10);


	int fd = udp_start_server(port);
	if(fd == -1)
		err(1,"Bad things happened\n");
	
	printf("Started server at port %d.\n", port);

	
	
	peer_t	peer;
	peer.addr_size = sizeof(struct sockaddr_in);
	while(1 == 1)
	{
		if(udp_hasConnection(fd, &peer) > 0)
		{
			/* send out data*/
			sendDataOut(fd,&peer);
		}
		/* update*/
		updatePortBinds();
		usleep(30000);
	}
	udp_close(fd);
}
