#!/bin/bash 
# Author: Roman Dobias

# Find all IPv4/IPv6 addresses at the machine
ip address | grep inet | awk '{print $2}' | cut -d '/' -f 1
