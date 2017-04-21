/*
	2016 
	Author: Roman Dobias (xdobia11)
	VUTBr
*/

#include "structure.h"

/* data structure servers as a key handler and raw data are compared*/
/* Returns 1 if both structures are equal*/
int compare_data(data_t* a, data_t* b);

/* A macro-version of compare_data*/
//#define CMP_DATA(a,b) (((a)->addr4 == (b)->addr4 && (a)->port == (b)->port))

typedef struct shash_item{
	data_t data;
	struct shash_item* next;
} shash_item_t; 

typedef struct {
	shash_item_t** pool;
	int pool_size;
} hash_tab_t;


/* Initialize table to default state*/
int hash_tab_init(hash_tab_t* tab, int size);

/* Delete table*/
int hash_tab_dispose(hash_tab_t* tab);

/* Find member*/
shash_item_t*  util_hash_tab_find(hash_tab_t* tab, data_t* key);

/* Find member*/
data_t*  hash_tab_find(hash_tab_t* tab,struct in6_addr srcAddr, int srcPort, struct in6_addr destAddr, int destPort, int protocol);

/* Add a new member*/
void  hash_tab_add(hash_tab_t* tab,struct in6_addr srcAddr, int srcPort, struct in6_addr destAddr, int destPort, int protocol, char* program);

/* Data version of add*/
void  hash_tab_add_dt(hash_tab_t* tab,data_t* dt);

void hash_tab_print(hash_tab_t* tab);

unsigned long hash(data_t* data, int size);
