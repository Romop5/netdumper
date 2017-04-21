CC=gcc
CFLAGS=-O0 -Wall -g -std=c99
all: test deamon nfaltern

nfaltern: btree.o nfaltern.o log.o query.o hashtbl.o queue.o udp.o
	gcc btree.o log.o query.o hashtbl.o queue.o udp.o nfaltern.o -L./. -l:./libnf.a -o nfaltern
deamon: deamon.o netstat.o log.o hashtbl.o queue.o udp.o hosts.o
	gcc log.o netstat.o deamon.o hashtbl.o queue.o udp.o hosts.o -o deamon
test: hashtbl.o queue.o
	gcc test.c hashtbl.o queue.o -o test 

clean: 
	rm -rf deamon nfaltern test *.o
	
###############################################################################
#	OBJECTS
###############################################################################

log.o: log.c log.h
hashtbl.o: hashtbl.c
queue.o: queue.c queue.h
server: server.c
	gcc -o server server.c

deamon.o: deamon.c queue.h structure.h hashtbl.h udp.h

udp.o: udp.c udp.h
query.o: query.c udp.h structure.h
hosts.o: hosts.c hosts.h

nfaltern.o: nfaltern.c libnf.h hashtbl.h structure.h queue.h query.h
btree.o: btree.c btree.h
netstat.o: netstat.c netstat.h structure.h
