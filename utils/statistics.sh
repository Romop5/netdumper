#/bin/bash
# Author: xdobia11
# Produces statistics per process/service

# $1 is directory with flows
if [ -d ]
then
echo "USAGE: <dir-with-nfdumps>"
exit 1
fi

# $1 is directory with flows
FILES=`find . $1|egrep "*nfcap*"`

# get a huuuuge output file
CONCT=""
for file in $FILES; do
TMP=`nfdump -r $file -o "fmt:%uname %line6 /=/\n" | grep "/=/"`
CONCT="$CONCT $TMP"
done;

echo "$CONCT"
# Detect list of services
SERVICES=`echo "$CONCT"| awk '{print $1}' | sort |uniq`

printf "Services and their protocols:\n"
COUNT=`echo "$CONCT" | wc | awk '{print $1}'`
printf "Lines of output: %s\n" "$COUNT"

SUM=0
for serv in $SERVICES; do
SERV_PART=`echo "$CONCT" | grep "$serv"`
COUNT=`echo "$SERV_PART" | wc | awk '{print $1}'`
PROTOCOLS=`echo "$SERV_PART" | awk '{print $5}' | sort | uniq`
printf "%-5d %s\n" "$COUNT" "$serv"
for proto in $PROTOCOLS; do
PROTO_COUNT=`echo "$SERV_PART" | grep " $proto " | wc | awk '{print $1}'`
SUM=`expr $SUM + $PROTO_COUNT`
printf "\t%s %-5d\n" "$proto" "$PROTO_COUNT"
done;
done;
printf "Overall sum: %d\n" $SUM
