# Author: Roman Dobias (xdobia11) 
# Jan 2017
# Note: used at Ubuntu 14.10

# 1. get the list of IP  bindings and skip first 2 lines
netstat -tuapn --numeric-ports 2>/dev/null | tail -n+3 | \
# 2. print out data in CSV format: proto, sourceip:port, program
awk '{
	prog = $7;
	if(index($7,"/") == 0  && index($7,"-") == 0)
		 prog = $6; 
	split($4, chars, "");

	for(i = length($4); i > 0; i--)
		if(chars[i] == ":")
		{
			SERVER = substr($4, 1, i-1);
			PORT = substr($4, i+1);
			break;
		}
	printf "%s,%s,%s,%s\n", $1,SERVER,PORT,prog}'
# 3. alles
