/*
 * Author: xdobia11
 * Purpose: mapping UDP/TCP ports to IANA-registred services
 * Note: file services.txt is used as a text database of services
 */

struct bnode {
	unsigned int key;
	char tcp[100];
	char udp[100];
	struct bnode* left, *right;
};

// Returns 0 on success, -1 on error
int load_services();

void free_services();

struct bnode * getService(int port);

