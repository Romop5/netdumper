#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include "queue.h"
void queue_init(queue_t* q)
{
	q->first = NULL;
	q->last = NULL;	
}

void queue_append(queue_t* q, data_t save)
{
	queue_item_t* it = malloc(sizeof(queue_item_t));
	if(it)
	{
		it->data = save;
		it->next = NULL;
		if(q->last)
		{
			q->last->next = it;	
		} else {
			q->first = it;
		}
		q->last = it;
	}
		
}

void queue_pop(queue_t* q)
{
	if(q->first)
	{
		if(q->first == q->last)
			q->last = NULL;
		queue_item_t* curr = q->first;
		q->first = curr->next;
		free(curr);
	}	
}

data_t* queue_gettop(queue_t* q)
{
	if(q->first)
	return &q->first->data;
}

void queue_dispose(queue_t* q)
{
	queue_item_t* it = q->first, *next = NULL;
	while(it)
	{
		next = it;
		it = next->next;	
		free(next);
	}
	q->first = NULL;
	q->last = NULL;
}

void queue_print(queue_t* q)
{
	queue_item_t* it = q->first;
	
	int i = 0; 
	while(it)
	{
		char ip[256];
		inet_ntop(AF_INET6, &it->data.addr, ip, 256);
		printf("[%2d] %s:%d\n",++i,ip, it->data.port);
		it = it->next;
	}
}

int queue_length(queue_t* q)
{
	queue_item_t* it = q->first;
	int len = 0;
	while(it != NULL)
	{
		len ++;
		it = it->next;
	}
	return len;
}
