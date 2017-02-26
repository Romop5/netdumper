# Author: Roman Dobias (xdobia11) 
# Feb 2017
# Note: used at Eva.fit.vutbr (FreeBSD)

# 1. get the list of IP  bindings and skip first 2 lines
sockstat 2>/dev/null | tail -n+3 | \
# 2. print out data in CSV format: proto, sourceip:port, program
awk '{
	if ($5 == "stream" || $5 == "dgram") 
		next;
	prog = $2;
	split($6, chars, "");

	for(i = length($6); i > 0; i--)
		if(chars[i] == ":")
		{
			SERVER = substr($6, 1, i-1);
			PORT = substr($6, i+1);
			break;
		}
		if(SERVER == "*")
			SERVER = "0.0.0.0";
	printf "%s,%s,%s,%s\n", $5,SERVER,PORT,prog}'
# 3. alles
