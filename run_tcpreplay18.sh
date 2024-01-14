#!/bin/bash

# NOTICE: For boosting the performance, node8 is substituted with node3 (IP address)

echo

tshark -w $outfile -F pcap -q &>x.log 2>y.log &
_pid=$!
sleep 5

filename=LGTV.pcap
sIP=192.168.10.120

dIP1=155.33.33.70
dIP2=155.33.33.75
dIP3=23.196.100.151
dIP4=52.88.243.235

tcprewrite --infile=$filename --outfile=tester1.pcap --srcipmap=$sIP:192.168.1.1 --dstipmap=$dIP1:192.168.1.2 --fixcsum
tcprewrite --infile=tester1.pcap --outfile=tester2.pcap --srcipmap=$sIP:192.168.1.1 --dstipmap=$dIP2:192.168.1.2 --fixcsum
tcprewrite --infile=tester2.pcap --outfile=tester3.pcap --srcipmap=$sIP:192.168.1.1 --dstipmap=$dIP3:192.168.1.2 --fixcsum
tcprewrite --infile=tester3.pcap --outfile=tester4.pcap --srcipmap=$sIP:192.168.1.1 --dstipmap=$dIP4:192.168.1.2 --fixcsum

tcprewrite --infile=tester4.pcap --outfile=tester5.pcap --srcipmap=$dIP1:192.168.1.2 --dstipmap=$sIP:192.168.1.1 --fixcsum
tcprewrite --infile=tester5.pcap --outfile=tester6.pcap --srcipmap=$dIP2:192.168.1.2 --dstipmap=$sIP:192.168.1.1 --fixcsum
tcprewrite --infile=tester6.pcap --outfile=tester7.pcap --srcipmap=$dIP3:192.168.1.2 --dstipmap=$sIP:192.168.1.1 --fixcsum
tcprewrite --infile=tester7.pcap --outfile=tester8.pcap --srcipmap=$dIP4:192.168.1.2 --dstipmap=$sIP:192.168.1.1 --fixcsum

tcpprep --auto=first --pcap=$filename --cachefile=cam_in.cache
tcprewrite --infile=tester8.pcap --outfile=tester.pcap --enet-smac=00:15:6d:84:92:cb,00:15:6d:85:e0:c8 --enet-dmac=00:15:6d:85:e0:c8,00:15:6d:84:92:cb --cachefile=cam_in.cache

echo OK! MAC and IP addresses have been changed to MAC and IP addresses of wlan0 of node1-8 and node1-4.

now="$(date)"
printf "================= %s =================\n" "$now"
echo Start_time: $(date +%s.%N)

tcpreplay -i wlan0 tester.pcap
#process_id=$!
#wait $process_id


sleep 0.5
killall -2 tshark

# ### *** For VSVI ***   (Replaying pcap file for the second time to deplete the buffer)
timeout 70 tcpreplay -i wlan0 tester.pcap
# ### *** For VSCI ***
#sleep 40
# ###


click st8.click

echo
now2="$(date)"
printf "................. The process is completed! ................. %s\n" "$now2"
echo
echo

