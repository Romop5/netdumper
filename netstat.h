/*
 *  NetDUMP 
 *  Author:	xdobia11
 *  Prupose: 	An interface to get connections based at host
 *  Licence:	beerware
 */

#include <arpa/inet.h>

enum eProto { TCP, UDP };

// Structure defines exchanged data
typedef struct {
	// IPv4/6 stored in IPv6 format
	struct in6_addr src; 
	// local port (0 - 65k)
	int port;
	// protocol type (TCP/UDP)
	enum eProto proto;
	// pid / program
	char program[255];
} ndata;


// Initialize init module
void n_init();

// load data
int n_load();

// Fill data with a single connection
// Returns 0 if no data is available
int n_getData(ndata* data);

// free data used by module
void n_dtor();
