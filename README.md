# Extending NetFlow with process data
## (Projektova prax)
This project is a part of Projektova Prax subject, currently being taken at [Brno University of Technology](http://www.fit.vutbr.cz/).

The aim of the project is to create profiles of computer networks with emphasis on communicating processes, running on certain hosts. The goal is reached by introducing a new system, based on [Cisco NetFlow](https://en.wikipedia.org/wiki/NetFlow), which collects data about processes separetely and use them to extend NetFlow dumps.
## 3rd party software
* [NProbe by Ntop](http://www.ntop.org/products/netflow/nprobe/)
* [nfdump (nf-tools)](http://nfdump.sourceforge.net/)
* [libnf (FIT VUTBR)](https://github.com/VUTBR/libnf)

## Understanding the project
The project is divided into two parts: ProcessTracker and FlowUpdater. 

### ProcessTracker
This daemon provides collecting data about processes at certain host. *ProcessTracker* is using scripts `hosts.sh` and `binds.sh` to scan available network interfaces and to get the names of processes, listening at certain ports, respectively. 

To store and compare process bindings, a *search tree* (implemented in `hashtbl.c`) is used while storing data with key **(IPadd, port, proto)** and **data** (the process name). In addition, a *FIFO* *queue* is used to handle differences in the search tree as events which happen in time. Queue is implemented in `queue.c` 

The main program of *ProcessTracker* is an infinite loop, providing status updates periodically. In an status update, the result of script `bindings.sh` is compared with data stored in the search tree. If any difference is detected in process bindings, a new event is emmited to the queue. 
The rest of status check is reserved for serving clients the data stored in the queue.
Implementation can be found in `daemon.c`

### FlowUpdater

*FlowUpdater* works with outputs of NetFlow collector using 3rd party library *libnf*. Before updating flow records, process data is received by *ProcessTracker* using UDP and data are used to reconstruct the search tree at FlowUpdater side. 
For each NetFlow record in NetFlow dump file, the search tree is accessed with key composed of **source IP, source port and protocol**. If entry with specified key exists, entry's process name is inserted into flow record. Otherwise, a text literal (place holder) is used. Implementation can be found in `nfaltern.c`

### Utils
The implementation is based on specialized modules created for transparency purposes. 
Data structures used to store data are described in `structure.h`
`netstat.h` together with `host.h` provide tools for accessing system network configuration.
`udp.h` defines comfort simple wrapping layer to use UDP server-client model.




## Using this project
**Note**: libnf.a included in main directory was compiled under Linux. 
In order to compile under FreeBSD, compile `libnf` first and copy .a from
`/src/.libs/libnf.a` to compilation directory.


Project has been tested at Ubuntu 16.10 with standard *gcc*.
> `make`

The previous command produces binary files `deamon` and `nfaltern`.

### Running Deamon
Deamon expects port number to be set in arguments. 
> `./deamon PORTNUM`

**Note**: to have an access to all processess, deamon must be run with *root* rights. 

### Running nfaltern
To update files, place nfaltern into directory where *nfdump* dumps its data.
> `./nfaltern IP PORTNUM TIME`

where *IP* and *PORTNUM* stand for IPv4 address and port of deamon and *TIME* is the delta time between two flow updates, multiplied with 10, in seconds.

## Using extended flows 
An example of extended flows can be found in directory `profiles`. For seeing *process* collumn in flows, it's neccessary to run `nfdump` with special command-line argument, specifying output format. *%uname* specifies the process name added into NetFlow flow dumps. 

To see an extended output, use this command
`nfdump -o "fmt: %line %uname" -r <directory-to-extended-dumps>`

![An example output](https://github.com/Romop5/netdumper/doc/console.png)

## What's the licence
See _LICENCE_.
