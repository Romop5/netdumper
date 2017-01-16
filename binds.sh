#!/bin/bash
# Author: Roman Dobias (xdobia11) 
# Jan 2017
# Note: used at Ubuntu 14.10

# 1. get the list of IP  bindings and skip first 2 lines
netstat -tuapn --numeric-ports 2>/dev/null | tail -n+3 | \
# 2. print out data in CSV format: proto, sourceip:port, program
awk '{prog = $7; if(index($7,"/") == 0  && index($7,"-") == 0) prog = $6;printf "%s,%s,%s\n", $1,$4,prog}'
# 3. alles
