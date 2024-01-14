#!/bin/bash

echo "Station 8 is getting set up ..."

modprobe ath5k &

process_id=$!
wait $process_id

ifconfig wlan0 up
iwconfig wlan0 mode Managed essid AP1
ifconfig wlan0 192.168.1.4 netmask 255.255.255.0
route add -net 192.168.2.0 netmask 255.255.255.0 gw 192.168.1.2

echo "Setting up of station 8 is completed!"
