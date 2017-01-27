/*
 *  NetDUMP 
 *  Author:	xdobia11
 *  Prupose: 	An interface to get connections based at host
 *  Licence:	beerware
 */

#include <arpa/inet.h>
#include "structure.h"

// Initialize init module
void n_init();

// load data
int n_load();

// Fill data with a single connection
// Returns 0 if no data is available
int n_getData(data_t* data);

// free data used by module
void n_dtor();

int convertIP(char* src, struct in6_addr *out);
