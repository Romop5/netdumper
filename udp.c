#include "udp.h"
#include <strings.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/in.h>
int fd_server;
int udp_start_server(const int port)
{
	int fd;
	struct sockaddr_in server;
	server.sin_family = AF_INET;	/* set IPv4 addressing*/
	server.sin_addr.s_addr = htonl(INADDR_ANY);	/* the server listens to any interface*/
	server.sin_port = htons(port);	/* the server listens on this port*/
	if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)	/* create the server UDP socket*/
		return -1;
	if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0)
		return -1;
	if (bind(fd, (struct sockaddr *)&server, sizeof(server)) == -1)	/* binding with the port*/
		return -1;
	return fd;
}

int udp_hasConnection(int fd, peer_t* client)
{
	int buff;
	int res = recvfrom(fd, &buff, sizeof(buff), 0, (struct sockaddr*) &client->addr, &client->addr_size);
	if (res == -1)
		return 0;
	/*printf("Bum\n");*/
	return 1;
}

int udp_sendTo(int fd, const peer_t* client, char *msg, const int len)
{
	/*printf("Len: %d\n",len);*/
	int r;
	r = sendto(fd, msg, len, 0, (struct sockaddr *) &client->addr, client->addr_size);	/* send the answer*/
	/*printf("r: %d\n",r);*/
	if (r == -1 || r != len)
	{
		perror(NULL);
		return 1;
	}
	return 0;
}

int udp_start_client(const char *addr, const int port,peer_t* out)
{
	bzero(out,sizeof(peer_t));
  int fd;
	inet_aton(addr, &out->addr.sin_addr);
   
  if ((fd = socket(AF_INET , SOCK_DGRAM , 0)) == -1)   /*create a client socket*/
	return -1;
  
	if (fcntl(fd, F_SETFL, O_NONBLOCK) != 0)
		return -1;
  out->addr.sin_family = AF_INET;                   
  out->addr.sin_port = htons(port);                 /* set the server port (network byte order)*/
  out->addr_size = sizeof(struct sockaddr_in);

	return fd;
}

int udp_hasData(int fd, char *data, int len,peer_t* peer)
{
	int res = recvfrom(fd, data, len, 0, (struct sockaddr*) &peer->addr, &peer->addr_size);
	if (res == -1)
		return 0;
	/*printf("Client: Bum %d\n",res);*/
	return res;

}

int udp_close(int fd)
{
	close(fd);
	return 0;
}
