#include "hashtbl.h"
#include "queue.h"
int main()
{
	hash_tab_t table;
	hash_tab_init(&table, 7);
	hash_tab_add(&table, inet_addr("127.0.01"), 80, P_UDP, "test");	
	hash_tab_add(&table, inet_addr("127.0.01"), 80, P_UDP, "roman");	
	hash_tab_add(&table, inet_addr("127.0.01"), 80, P_UDP, "roman");	
	hash_tab_print(&table);
	hash_tab_dispose(&table);

	printf("Queu\n");
	queue_t q;
	queue_init(&q);
	
	data_t a = {inet_addr("127.0.0.1"),80,0};
	queue_append(&q,a);
	queue_print(&q);
	queue_pop(&q);
	queue_pop(&q);
	queue_pop(&q);
	queue_dispose(&q);

	
}
