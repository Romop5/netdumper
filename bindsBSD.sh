# Author: Roman Dobias (xdobia11) 
# Feb 2017
# Note: used at Eva.fit.vutbr (FreeBSD)

# 1. get the list of IP  bindings and skip first 2 lines
sockstat 2>/dev/null | tail -n+3 | \
# 2. print out data in CSV format: proto, sourceip:port, program
awk '{
	if (match($5,"(tcp|udp)[46]") == 0)
		next;
	sub("[46]","", $5);
	prog = $2;
	if(prog == "?")
		next;
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
	printf "%s,%s,%s,%s\n", $5,SERVER,PORT,prog}'\
| sort | uniq 
# 3. alles
