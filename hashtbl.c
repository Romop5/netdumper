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
	if(a->addr4 == b->addr4 &&
		a->port == b->port)
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
data_t*  hash_tab_find(hash_tab_t* tab,int addr4, int port,int proto)
{
	data_t key = {addr4, port,""};
	shash_item_t* ptr = util_hash_tab_find(tab,&key);
	if(ptr)
	{
		return &ptr->data;
	}
	return NULL;
}

// Add a new member
void  hash_tab_add(hash_tab_t* tab,int addr4, int port,int proto, char* program)
{
	data_t* data = hash_tab_find(tab,addr4,port,proto);	
	if(data)
	{
		strcpy(data->program, program);	
		return;
	} else {
		data_t key = {addr4,port,0};	
		unsigned long index = hash(&key, sizeof(data_t)) % tab->pool_size;
		shash_item_t* item = malloc(sizeof(shash_item_t));
		if(item)
		{
			item->data.addr4 = addr4;
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
			struct in_addr adr = {ptr->data.addr4};
			char* ip = inet_ntoa(adr);
			printf("[%d] %s:%d\t%s\n",i,ip, ptr->data.port, ptr->data.program);
			ptr = ptr->next;
		}
	}
}

unsigned long hash(data_t* data, int size)
{
        unsigned long hash = 5381;
	hash ^= data->port ^ data->addr4; 
        return hash;
}
