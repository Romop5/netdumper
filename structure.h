#ifndef STRUCT_H
#define STRUCT_H

#define P_UDP 0
#define P_TCP 1

typedef struct {
	// key
	int addr4;
	int port;
	int protocol;
	//
	int timestamp;		// Unix timestamp 
	char program[256];
} data_t;

struct query_msg 
{
	int count;
	data_t items[];
};

#endif
