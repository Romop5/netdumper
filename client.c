#include "server.h"

int main()
{
	peer_t server;
	int fd = udp_start_client("127.0.0.1",3009,&server);
	if(fd == -1)
		err(1,"Bad things happened");
	
	printf("Client: Started a client.\n");
	
	//peer.addr_size = sizeof(struct sockaddr_in);

	char recvbuff[16000];
	while(1 == 1)
	{
		int c = getchar();
		if(c == EOF)
			break;
		char msg[] = "ahoj";
		udp_sendTo(fd, &server,msg,strlen(msg)+1); 
		printf("Client: Sended out\n");
		int i = 0;
		for(i = 0; i < 100; i++)
		{
			if(udp_hasData(fd,recvbuff, 16000, &server) > 0)
				break;
			usleep(10000);
		} 
		if(i == 100)
		{
			printf("Client: Connection dropped.");
			break;
		}
		else {
			printf("Client: We got a broadcast.\n");
			printf("Client: Recv Msg: %s\n",recvbuff);
		}
	}
	udp_close(fd);
}
