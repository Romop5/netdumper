#ifndef STRUCT_H
#define STRUCT_H

#define P_UDP 0
#define P_TCP 1

#include <netinet/ip.h>

typedef struct {
	// key
	struct in6_addr addr;
	uint16_t port;
	int protocol;
	//
	uint64_t timestamp;		// Unix timestamp 
	char program[256];
} data_t;

struct query_msg 
{
	int count;
	data_t items[];
};

#endif
