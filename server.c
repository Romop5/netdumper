#include "udp.h"

int main()
{
	int fd = udp_start_server(3009);
	if(fd == -1)
		err(1,"Bad things happened");
	
	printf("Started.\n");
	
	peer_t	peer;
	peer.addr_size = sizeof(struct sockaddr_in);
	while(1 == 1)
	{
		if(udp_hasConnection(fd, &peer) == 0)
		{
			usleep(30000);
			continue;
		}
		printf("We got a broadcast.\n");
		char* chr = "Super\n";
		int res = udp_sendTo(fd, &peer, chr, strlen(chr)+1);
		if(res != 0)
			printf("Dopr.\n");
		printf("Here we go\n");
	}
	udp_close(fd);
}
