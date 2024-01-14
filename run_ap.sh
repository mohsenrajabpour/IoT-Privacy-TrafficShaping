#!/bin/bash

echo "AP is getting set up ..."

modprobe ath9k &

process_id=$!
wait $process_id

ifconfig wlan0 up
hostapd /etc/hostapd/hostapd.conf -B
ifconfig wlan0 192.168.1.2 netmask 255.255.255.0
ifconfig eth0 192.168.2.1 netmask 255.255.255.0

echo "Setting up of AP is completed!"
