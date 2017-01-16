/*
	Simple interface for UDP communication
*/

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <ctype.h>
#include <err.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#define PORT 3009		// server port number
#define BUFFER	(1024)		// length of the receiving buffer

typedef struct sockaddr * s_peer;

typedef struct peer
{
	struct sockaddr_in addr;
	int addr_size;
} peer_t;

// Start a non-blocking server
// Returns FD or -1
int udp_start_server(const int port);

// Returns 1 if there is a connection, otherwise 0
int udp_hasConnection(int fd, peer_t* client);

// Returns 1 on success, otherwise 0
int udp_sendTo(int fd, const peer_t* client, char *msg, const int len);
int udp_start_client(const char *addr, const int port,peer_t* out);
int udp_hasData(int fd, char *data, int len,peer_t* peer);
int udp_close(int fd);
