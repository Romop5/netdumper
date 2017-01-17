// 2016 
// Author: Roman Dobias (xdobia11)
// VUTBr

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "hashtbl.h"
#include <arpa/inet.h>

int compare_data(data_t* a, data_t* b)
{
	if(bcmp(&a->addr,&b->addr,sizeof(struct in6_addr)) == 0 &&
		a->port == b->port && a->protocol == b->protocol)
		return 1;
	return 0;	
}


// Initialize table to default state
int hash_tab_init(hash_tab_t* tab, int size)
{
	tab->pool_size = size;
	tab->pool = malloc(sizeof(shash_item_t*)*size);	
	if(tab->pool)
	{
		for(int i = 0; i < size; i++)
		{
			tab->pool[i] = NULL;
		}
		return 1;
	}
	return 0;
}

// Delete table
int hash_tab_dispose(hash_tab_t* tab)
{
	for(int i = 0; i < tab->pool_size; i++)
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
}

// Find member
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

// Find member
data_t*  hash_tab_find(hash_tab_t* tab,struct in6_addr addr, int port,int proto)
{
	data_t key = {addr, port,proto};
	shash_item_t* ptr = util_hash_tab_find(tab,&key);
	if(ptr)
	{
		return &ptr->data;
	}
	return NULL;
}

// Add a new member
void  hash_tab_add(hash_tab_t* tab,struct in6_addr addr, int port,int proto, char* program)
{
	//hash_tab_print(tab);
	data_t* data = hash_tab_find(tab,addr,port,proto);	
	if(data)
	{
		strcpy(data->program, program);	
		return;
	} else {
		data_t key = {addr,port,0};	
		unsigned long index = hash(&key, sizeof(data_t)) % tab->pool_size;
		shash_item_t* item = malloc(sizeof(shash_item_t));
		if(item)
		{
			item->data.addr = addr;
			item->data.port= port;
			item->data.protocol = proto;
			strcpy(item->data.program,program);
			item->next = tab->pool[index];
			tab->pool[index] = item;
		}
	}
}

void hash_tab_print(hash_tab_t* tab)
{
	printf("Debut table\n");
	for(int i = 0; i < tab->pool_size; i++)
	{
		shash_item_t* ptr = tab->pool[i];
		while(ptr)
		{	
			char ip[256];
			inet_ntop(AF_INET6, &ptr->data.addr, ip, 255);
			printf("[%d-%s] %s:%d\t%s\n",i,"P",ip, ptr->data.port, ptr->data.program);
			ptr = ptr->next;
		}
	}
}

unsigned long hash(data_t* data, int size)
{
        unsigned long hash = 5381;
	uint32_t *padd = (uint32_t*) &data->addr;
	for(int i = 0; i < 4; i++)
		hash ^= (data->port ^ padd[i]);
        return hash;
}
