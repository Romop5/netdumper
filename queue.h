#include "structure.h"
typedef struct queue_item{
	data_t data;
	struct queue_item* next;
} queue_item_t; 

typedef struct {
	queue_item_t* first;
	queue_item_t* last;
} queue_t;

void queue_init(queue_t* q);

void queue_append(queue_t* q, data_t save);

void queue_pop(queue_t* q);

data_t* queue_gettop(queue_t* q);

void queue_dispose(queue_t* q);

void queue_print(queue_t* q);

int queue_length(queue_t* q);


