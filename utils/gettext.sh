#/bin/bash
# Author: xdobia11
# Get the output of NFDUMP in text format
# Use together with statisticstext.sh

# $1 is directory with flows
FILES=`find . $1|egrep "*nfcap*"`

# get a huuuuge output file
CONCT=""
for file in $FILES; do
TMP=`nfdump -r $file -6 -o "fmt:%uname %line /=/\n" | grep "/=/"`
CONCT="$CONCT $TMP"
done;

echo "$CONCT"
