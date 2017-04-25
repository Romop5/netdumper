#/bin/bash
# Author: xdobia11
# Produces statistics per process/service

CONCT=`cat "$1"`
#echo "$CONCT"
# Detect list of services
SERVICES=`echo "$CONCT"| awk '{print $1}' | sort |uniq`

printf "Services and their protocols:\n"
COUNT=`echo "$CONCT" | wc | awk '{print $1}'`
printf "Lines of output: %s\n" "$COUNT"

for serv in $SERVICES; do
SERV_PART=`echo "$CONCT" | egrep "^$serv "`
COUNT=`echo "$SERV_PART" | wc | awk '{print $1}'`
PROTOCOLS=`echo "$SERV_PART" | awk '{print $5}' | sort | uniq`
printf "%-5d %s\n" "$COUNT" "$serv"
for proto in $PROTOCOLS; do
PROTO_COUNT=`echo "$SERV_PART" | grep " $proto " | wc | awk '{print $1}'`
printf "\t%s %-5d\n" "$proto" "$PROTO_COUNT"
done;
done;
