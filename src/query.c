#include <unistd.h>
#include "udp.h"
#include "structure.h"
#include "queue.h"
#include "log.h"
#include <time.h>



/* Return -1 of failure*/
/* On success, receives structure S_queryData*/
int sockQuery(const char* serverAddr, int port, char* buffer, const int sizeOfBuffer)
{
	/* start a new UDP client*/
	peer_t server;
	int fd = udp_start_client(serverAddr,port,&server);
	if(fd == -1)
		return -1;

	/* send a request to server*/
	char * reqv = "REQ";
	udp_sendTo(fd, &server,reqv,strlen(reqv)+1); 
	
	/* wait for a response*/
	int len = 0;
	for(int i = 0; i < 100; i++)
	{
		if((len = udp_hasData(fd,buffer, sizeOfBuffer, &server)) > 0)
			break;
		usleep(10000);
	} 
	udp_close(fd);
	if(len < 0)
		return -1;
	return len;
}

/* let SERVERADDR and PORT are globals*/
int updateFront(queue_t* front,int fd)
{
	peer_t	peer;
	peer.addr_size = sizeof(struct sockaddr_in);
	if(udp_hasConnection(fd, &peer) > 0)
	{
		struct query_msg * buff = malloc(36000);
		if(!buff)
			return -1;

		int res = udp_hasData(fd,(char*) buff, 36000, &peer);
		//int res = sockQuery(IP, port, (char*)buff, 16000);
		if(res > 0)
		{
			char ip[256];
			udp_getPeerIP(&peer, ip);
			LOG("Update [%u] from %s\n with %d items\n",buff->sequence,ip,buff->count);
			int len = buff->count;
			/* for each item in server delivery: queue up the server front*/
			for(int i = 0; i < len; i++)
			{
				queue_append(front, buff->items[i]);
				/*LOG Changes*/
				time_t tstamp = (time_t) buff->items[i].timestamp;
				char buffer[40];
				struct tm* tm_info;

				tm_info = localtime(&tstamp);

				strftime(buffer, 39, "%Y-%m-%d %H:%M:%S", tm_info);

				char ip[256];
				inet_ntop(AF_INET6, &buff->items[i].s_addr, ip, 255);
				char ipB[256];
				inet_ntop(AF_INET6, &buff->items[i].d_addr, ipB, 255);
				char* proto = (buff->items[i].protocol == P_UDP)?"UDP":"TCP";
				LOG("%28s:%-5d %28s:%-5d\t%-20s\t%s - %s\n",ip, buff->items[i].s_port, ipB,buff->items[i].d_port, buff->items[i].program,proto,buffer);
			}
		}
		free(buff);
	}
	return 0;
}
/*
int main()
{
	struct query_msg *msg = malloc(sizeof(struct query_msg)+sizeof(data_t)*10000);
	int len = sockQuery("127.0.0.1",3009,(char*) msg, 10000*sizeof(data_t));
	printf("Len %d\n",len);
	if(len != -1)
	{
		printf("Query count:%d:\n",msg->count);
		for(int i = 0; i < msg->count; i++)
		{
			struct in_addr a = {msg->items[i].addr4};
			printf("Item: %10s:%d %s:\n",inet_ntoa(a), msg->items[i].port, msg->items[i].program);
		}
	}
	free(msg);
}
*/
