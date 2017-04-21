/* 	
   	Year:	2016 
	Author: Roman Dobias (xdobia11)
 	VUTBr
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtbl.h"
#include <arpa/inet.h>
#include <strings.h>

int compare_data(data_t* a, data_t* b)
{
	if(bcmp(&a->s_addr,&b->s_addr,sizeof(struct in6_addr)) == 0 &&
		a->s_port == b->s_port && a->d_port == b->d_port &&
		a->protocol == b->protocol &&
		bcmp(&a->d_addr,&b->d_addr,sizeof(struct in6_addr))== 0 )
		return 1;
	return 0;	
}


/* Initialize table to default state*/
int hash_tab_init(hash_tab_t* tab, int size)
{
	tab->pool_size = size;
	tab->pool = malloc(sizeof(shash_item_t*)*size);	
	if(tab->pool)
	{
		int i = 0;
		for(i = 0; i < size; i++)
		{
			tab->pool[i] = NULL;
		}
		return 1;
	}
	return 0;
}

/* Delete table*/
int hash_tab_dispose(hash_tab_t* tab)
{
	int i = 0;
	for(i = 0; i < tab->pool_size; i++)
	{
		shash_item_t* next = tab->pool[i],*current = NULL;
		while(next)
		{
			current = next;
			next = next->next;
			free(current);
		}
	}
	free(tab->pool);
	tab->pool = NULL;
	tab->pool_size = 0;
	return 0;
}

/* Find member*/
shash_item_t*  util_hash_tab_find(hash_tab_t* tab, data_t* key)
{
	unsigned long index = hash( key, sizeof(data_t)) % tab->pool_size;
	
	shash_item_t* next = tab->pool[index];
	while(next)
	{
		if(compare_data(&next->data, key) == 1)
			return next;
		next = next->next;
	}

	return NULL;
}

/* Find member*/
data_t*  hash_tab_find(hash_tab_t* tab,struct in6_addr srcAddr, int srcPort, struct in6_addr destAddr, int destPort, int protocol)
{
	data_t key = {srcAddr, srcPort,destAddr,destPort,protocol};
	shash_item_t* ptr = util_hash_tab_find(tab,&key);
	if(ptr)
	{
		return &ptr->data;
	}
	return NULL;
}

/* Add a new member*/
void  hash_tab_add(hash_tab_t* tab,struct in6_addr srcAddr, int srcPort, struct in6_addr destAddr, int destPort, int protocol, char* program)
{
	data_t* data = hash_tab_find(tab,srcAddr,srcPort,destAddr,destPort,protocol);	
	if(data)
	{
		strcpy(data->program, program);	
		return;
	} else {
		data_t key = {srcAddr,srcPort,destAddr,destPort,protocol,0};	
		unsigned long index = hash(&key, sizeof(data_t)) % tab->pool_size;
		shash_item_t* item = malloc(sizeof(shash_item_t));
		if(item)
		{
			/*item->data.addr = srcAddr;
			item->data.port= srcPort;
			item->data.protocol = proto;
			*/
			memcpy(item, &key,sizeof(key));
			strcpy(item->data.program,program);
			item->next = tab->pool[index];
			tab->pool[index] = item;
		}
	}
}

void hash_tab_print(hash_tab_t* tab)
{
	printf("HASH TABLE DEBUG:\n");
	printf("ID\tADD\t\t\t\tPROGRAM\t\t\tPROTO:\n");
	int i = 0;
	for(i = 0; i < tab->pool_size; i++)
	{
		shash_item_t* ptr = tab->pool[i];
		while(ptr)
		{	
			char ip[256];
			inet_ntop(AF_INET6, &ptr->data.s_addr, ip, 255);
			char ipB[256];
			inet_ntop(AF_INET6, &ptr->data.d_addr, ipB, 255);
			char* proto = (ptr->data.protocol == P_UDP)?"UDP":"TCP";
			printf("[%d] %28s:%-5d - %28s:%-5d\t%-20s\t%s\n",i,ip, ptr->data.s_port, ipB, ptr->data.d_port, ptr->data.program,proto);
			ptr = ptr->next;
		}
	}
}

unsigned long hash(data_t* data, int size)
{
        unsigned long hash = 5381;
	uint32_t *padd = (uint32_t*) &data->s_addr;
	int i = 0;
	for(i = 0; i < 4; i++)
		hash ^= (data->s_port ^ padd[i]);
        return hash;
}
