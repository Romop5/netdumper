#/bin/bash
# Author: xdobia11
# Validation script
# $1 nfdump log
# $2 nfcapd log
echo "===FillValid==="
if [ ! -f "$1" ]
then
echo "USAGE: <nfdump-log> <nfcap-log>"
exit 1 
fi

if [ ! -f "$2" ]
then
echo "USAGE: <nfdump-log> <nfcap-log>"
exit 1 
fi;
# search for lines that weren't filled in
LINES=`cat $1 | egrep -v "(\*\*\*|none)" | awk '{printf "(%s.*%s|%s.*%s) \n", $6,$8,$8,$6}'`

CASE_OK=0
CASE_NO=0
#for each line in nfcapd log
for i in $LINES; do 
# verify it wasn't mention in nfaltern log
RESULT=`cat $2 | egrep "$i"`
if [ ! -z "$RESULT" ]
then
	#echo "OK $i"
	CASE_OK=`expr $CASE_OK + 1`
else 
	echo "NO $i"
	CASE_NO=`expr $CASE_NO + 1`
fi
done;

printf "RESULT: OK: %d NO %d\n" "$CASE_OK" "$CASE_NO"


