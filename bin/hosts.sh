#!/bin/sh
# Author: Roman Dobias (xdobia11@fit.vutbr.cz)
# Purpose: print out the list of all IPv4/v6 addresses of available interfaces
# Example:
#	127.0.0.1
#	::1

ip addr| grep inet| awk '{print $2}' | cut -d '/' -f 1
