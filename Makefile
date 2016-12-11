
CFLAGS=-g -Wall -Wextra

main: main.o netstat.o

main.o: main.c netstat.h
netstat.o: netstat.c netstat.h

clean:
	rm *.o main
