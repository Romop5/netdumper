#include "queue.h"
#include "hashtbl.h"
#include "udp.h"
#include "log.h"

#include "netstat.h"
#include "hosts.h"
#include <strings.h>
#include <unistd.h>

/* stores triples (ip, port, program)*/
hash_tab_t g_binds;

/* a queue of triples (ip, port, program) which to be sent out */
queue_t g_outData;

struct in6_addr empty = {0,};

/* machine interfaces*/
struct in6_addr hosts[10];

/* count of machine interfaces*/
int hostsCount;

/*deamon port number*/
unsigned short dPort;

/* Return 1 on update*/
int updateProgram(data_t* data)
{
	/* ignore deamon and its instances */
	if(data->protocol == P_UDP && data->port == dPort)
		return 0; 
	data_t* dt = hash_tab_find(&g_binds, data->addr, data->port, data->protocol);
	if(dt)
	{
		/* if nothing has changed, scan for another*/
		if(strcmp(dt->program, data->program) == 0)
			return 0;
		printf("'%s' vs '%s'\n",dt->program, data->program);
	} 
	
	hash_tab_add(&g_binds, data->addr, data->port, data->protocol, data->program);

	/* append into queue and altern timestamp*/
	/*time_t t = time();*/
	/*data.timestamp = t;*/

	/*Skip 0.0.0.0 address*/
	if(bcmp(&data->addr, &empty, sizeof(struct in6_addr)) != 0)
		queue_append(&g_outData,*data); 

	/*LOG Changes*/
	char ip[256];
	inet_ntop(AF_INET6, &data->addr, ip, 255);
	char* proto = (data->protocol == P_UDP)?"UDP":"TCP";
	LOG("%28s:%-5d\t%-20s\t%s\n",ip, data->port, data->program,proto);
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
				printf("%s\n",data.program);
				for(int i = 0; i < hostsCount; i++)
				{
					data.addr = hosts[i];
					if(updateProgram(&data) == 1)
						i++;
				}
			}
		}
		
	}
	LOG("New queue elements appended: %d\n",i);
	
	/*hash_tab_print(&g_binds);i*/
/*	queue_print(&g_outData);*/
	n_dtor();
	
	return 0;
}

void sendDataOut(int fd, peer_t* p)
{
	static uint32_t seqNum = 0;
	seqNum++;

//	printf("Sending out data...\n");
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
		msg->sequence = seqNum;
		//msg->timestamp = (uint64_t) time(NULL);
		
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
	if(argc < 3)
	{
		fprintf(stderr,"USAGE: <IP> <PORT>\n");
		fprintf(stderr,"\tIP:PORT  = the adress of process collector\n");
		return 1;
	}	
	int port = atoi(argv[2]);
	dPort = port;

	queue_init(&g_outData);
	hash_tab_init(&g_binds,100);
	
	hostsCount = getHosts(hosts,10);

	peer_t server;
	int fd = udp_start_client(argv[1],port,&server);
	if(fd == -1)
	{
		err(1,"Failed to start UDP client\n");
		exit(1);
	}
	printf("Collector address: %s:%d.\n", argv[1],port);
	unsigned int tick = 0;
	while(1 == 1)
	{
		++tick;
		if(tick > 900)
		{
			/* send out data*/
			LOG("Sending update ...\n");	
			sendDataOut(fd,&server);
			tick = 0;
		}
		/* update*/
		updatePortBinds();
		/* sleep 30 ms */
		usleep(10000);
	}
	udp_close(fd);
}
