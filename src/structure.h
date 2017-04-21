#ifndef STRUCT_H
#define STRUCT_H

#define P_UDP 0
#define P_TCP 1
/* Following headers are essential for FreeBSD */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

#include <netinet/ip.h>

typedef struct {
	/* key*/
	struct in6_addr addr;
	uint16_t port;
	uint16_t protocol;
	/**/
	uint64_t timestamp;		/* Unix timestamp */
	char program[256];
} data_t;

struct query_msg 
{
	uint16_t count;
	uint64_t sequence;
	data_t items[];
};

#endif
