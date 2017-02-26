CC=gcc
CFLAGS=-g -Wall -g -std=c99
all: test deamon nfaltern

nfaltern: nfaltern.o query.o hashtbl.o queue.o udp.o
	gcc query.o hashtbl.o queue.o udp.o nfaltern.o -L./. -l:./libnf.a -o nfaltern
deamon: deamon.o hashtbl.o queue.o udp.o hosts.o
	gcc netstat.c deamon.o hashtbl.o queue.o udp.o hosts.o -o deamon
test: hashtbl.o queue.o
	gcc test.c hashtbl.o queue.o -o test 

clean: 
	rm deamon nfaltern test *.o
	
###############################################################################
#	OBJECTS
###############################################################################

hashtbl.o: hashtbl.c
queue.o: queue.c queue.h
server: server.c
	gcc -o server server.c

deamon.o: deamon.c queue.h structure.h hashtbl.h udp.h

udp.o: udp.c udp.h
query.o: query.c udp.h structure.h
hosts.o: hosts.c hosts.h

nfaltern.o: nfaltern.c libnf.h hashtbl.h structure.h queue.h query.h
