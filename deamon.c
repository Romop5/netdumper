#include "queue.h"
#include "hashtbl.h"
#include "udp.h"


// stores triples (ip, port, program)
hash_tab_t g_binds;

// a queue of triples (ip, port, program) which to be sent out 
queue_t g_outData;


int updatePortBinds()
{
	static int counter = 0;
	// if 1000 ticks has passed since last update
	if(counter++ > 100)
	{
		counter = 0;
		// then update
		// TODO: update hash table entries	
		printf("Update...\n");

		data_t a = {inet_addr("127.0.0.1"),80,P_UDP,"romco"};
		queue_append(&g_outData, a);
	}
}

void sendDataOut(int fd, peer_t* p)
{
	printf("Sending out data...\n");
	int count = queue_length(&g_outData);		
	// create an output buffer for entries with header
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
		// send it out
		udp_sendTo(fd, p, msg, output_len);

		free(msg);
	}
}




int main()
{
	
	queue_init(&g_outData);
	hash_tab_init(&g_binds,100);

	int fd = udp_start_server(3009);
	if(fd == -1)
		err(1,"Bad things happened");
	
	printf("Started.\n");
	
	peer_t	peer;
	peer.addr_size = sizeof(struct sockaddr_in);
	while(1 == 1)
	{
		if(udp_hasConnection(fd, &peer) > 0)
		{
			// send out data
			sendDataOut(fd,&peer);
		}
		// update
		updatePortBinds();
		usleep(30000);
	}
	udp_close(fd);
}
