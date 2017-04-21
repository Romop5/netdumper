#/bin/bash
# Author: xdobia11
# Produces statistics per process/service

# $1 is directory with flows
FILES=`find . $1|egrep "*nfcap*"`

# get a huuuuge output file
CONCT=""
for file in $FILES; do
TMP=`nfdump -r $file -o "fmt:%uname %line /=/\n" | grep "/=/"`
CONCT="$CONCT $TMP"
done;

echo "$CONCT"
# Detect list of services
SERVICES=`echo "$CONCT"| awk '{print $1}' | sort |uniq`

printf "Services and their protocols:\n"
COUNT=`echo "$CONCT" | wc | awk '{print $1}'`
printf "Lines of output: %s\n" "$COUNT"

for serv in $SERVICES; do
SERV_PART=`echo "$CONCT" | grep "$serv"`
COUNT=`echo "$SERV_PART" | wc | awk '{print $1}'`
PROTOCOLS=`echo "$SERV_PART" | awk '{print $5}' | sort | uniq`
printf "%-5d %s\n" "$COUNT" "$serv"
for proto in $PROTOCOLS; do
PROTO_COUNT=`echo "$SERV_PART" | grep "$proto" | wc | awk '{print $1}'`
printf "\t%s %-5d\n" "$proto" "$PROTO_COUNT"
done;
done;
