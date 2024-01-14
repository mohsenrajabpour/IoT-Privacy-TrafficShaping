#!/bin/bash

echo

tshark -w $outfile -F pcap -q &>x.log 2>y.log &
_pid=$!
sleep 5

filename=NestCamMotionDetection.pcap
sIP=172.24.1.84
dIP=54.86.169.103

tcprewrite --infile=$filename --outfile=tester1.pcap --srcipmap=$sIP:192.168.1.1 --dstipmap=$dIP:192.168.1.2 --fixcsum
tcprewrite --infile=tester1.pcap --outfile=tester2.pcap --srcipmap=$dIP:192.168.1.2 --dstipmap=$sIP:192.168.1.1 --fixcsum
tcpprep --auto=first --pcap=$filename --cachefile=cam_in.cache
tcprewrite --infile=tester2.pcap --outfile=tester.pcap --enet-smac=00:15:6d:84:92:cb,00:15:6d:85:e0:c8 --enet-dmac=00:15:6d:85:e0:c8,00:15:6d:84:92:cb --cachefile=cam_in.cache

echo OK! MAC and IP addresses have been changed to MAC and IP addresses of wlan0 of node1-3 and node1-4.

now="$(date)"
printf "================= %s =================\n" "$now"
echo Start_time: $(date +%s.%N)

tcpreplay -i wlan0 tester.pcap
#process_id=$!
#wait $process_id

sleep 0.5
killall -2 tshark

# ### *** For VSVI ***   (Replaying pcap file for the second time to deplete the buffer)
#timeout 1m tcpreplay -i wlan0 tester.pcap
# ### *** For VSCI ***
sleep 1m
# ###

click st3.click

echo
now2="$(date)"
printf "................. The process is completed! ................. %s\n" "$now2"
echo
echo
