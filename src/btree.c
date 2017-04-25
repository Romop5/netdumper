#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "btree.h"

#define FILENAME "services.txt"

struct bnode* bservices = NULL;

int btree_add(struct bnode** tree, int key, char* tcp, char* udp)
{
	struct bnode **ptr = tree;
	while(*ptr != NULL)
	{
		if((*ptr)->key == key)
		{
			strncpy((*ptr)->tcp, tcp, 100);	
			strncpy((*ptr)->udp, udp, 100);	
			return 0;
		}
		if((*ptr)->key > key)
			ptr = &((*ptr)->left);
		else
			ptr = &((*ptr)->right);
	} 
		
	*ptr = (struct bnode*) malloc(sizeof(struct bnode));
	if(*ptr)
	{
	
		strncpy((*ptr)->tcp, tcp, 100);	
		strncpy((*ptr)->udp, udp, 100);	
		(*ptr)->key = key;
		(*ptr)->left= NULL;
		(*ptr)->right = NULL;
		return 0;
	}
	return -1;
}

struct bnode* btree_get(struct bnode* tree, int key)
{
	while(tree != NULL)
	{
		if(tree->key == key)
			return tree;
		if(tree->key > key)
			return btree_get(tree->left, key);
		return btree_get(tree->right, key);
	}	
	return NULL;
}

void btree_del(struct bnode* tree)
{
	if(tree)
	{
		btree_del(tree->left);
		btree_del(tree->right);
		free(tree);
	}
}


int load_services()
{
	FILE* f = fopen("services.txt", "r");
	if(f)
	{
		char tcp[100],udp[100];
		int port;
		while(!feof(f))
		{
			fscanf(f,"%d %s %s\n",&port,tcp,udp);
			strcat(tcp,"***");
			strcat(udp,"***");
			btree_add(&bservices,port,tcp, udp);
			//printf("%d and %s/%s\n",port, tcp,udp);
		}
	} else {
		return -1;
	}
	fclose(f);
	return 0;
}

void free_services()
{
	btree_del(bservices);
}

struct bnode * getService(int port)
{
	return btree_get(bservices, port);
}
/*
int main()
{
	int result = load_services();
	if(result == 0)
		printf("OK\n");
	else
		printf("FAIL\n");
	btree_del(bservices);
}


*/
