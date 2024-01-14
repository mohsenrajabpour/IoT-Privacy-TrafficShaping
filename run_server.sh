#!/bin/bash

echo "Server is getting set up ..."

ifconfig eth0 192.168.2.2 netmask 255.255.255.0
route add -net 192.168.1.0 netmask 255.255.255.0 gw 192.168.2.1

echo "Setting up of server is completed!"
