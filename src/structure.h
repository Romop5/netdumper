#ifndef STRUCT_H
#define STRUCT_H

#define P_UDP 0
#define P_TCP 1
/* Following headers are essential for FreeBSD */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>

/*data_t stores assocication (sourceAddr,destAddr,proto)->(program,timestamp)*/
typedef struct {
	/*Composed key*/
	struct in6_addr s_addr;		/* source IP*/
	uint16_t s_port;		/* source port*/
	struct in6_addr d_addr;		/* destination IP*/
	uint16_t d_port;		/* destination port*/
	uint16_t protocol;		/* flow protocol (e.g. 17 - UDP)*/
	/* Additional data*/
	uint64_t timestamp;		/* Unix timestamp */
	char program[256];
} data_t;

/* Query msg is used in Tracker-To-Updater communication as a message 
 * with variable count of items*/
struct query_msg 
{
	uint16_t count;
	uint64_t sequence;
	data_t items[];
};

#endif
