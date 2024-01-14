#!/bin/bash

echo


filename=BelkinWemoSwitch.pcap
sIP=172.24.1.147
#dIP=

tcprewrite --infile=$filename --outfile=tester1.pcap --srcipmap=$sIP:192.168.1.3 --dstipmap=$sIP:192.168.1.3 --fixcsum

tcprewrite --infile=tester1.pcap --outfile=tester2.pcap --srcipmap=23.23.162.27:192.168.1.2 --dstipmap=23.23.162.27:192.168.1.2 --fixcsum
tcprewrite --infile=tester2.pcap --outfile=tester3.pcap --srcipmap=23.23.165.206:192.168.1.2 --dstipmap=23.23.165.206:192.168.1.2 --fixcsum
tcprewrite --infile=tester3.pcap --outfile=tester4.pcap --srcipmap=54.225.138.155:192.168.1.2 --dstipmap=54.225.138.155:192.168.1.2 --fixcsum
tcprewrite --infile=tester4.pcap --outfile=tester5.pcap --srcipmap=54.235.54.58:192.168.1.2 --dstipmap=54.235.54.58:192.168.1.2 --fixcsum
tcprewrite --infile=tester5.pcap --outfile=tester6.pcap --srcipmap=54.235.111.253:192.168.1.2 --dstipmap=54.235.111.253:192.168.1.2 --fixcsum
tcprewrite --infile=tester6.pcap --outfile=tester7.pcap --srcipmap=107.22.249.250:192.168.1.2 --dstipmap=107.22.249.250:192.168.1.2 --fixcsum
tcprewrite --infile=tester7.pcap --outfile=tester8.pcap --srcipmap=132.163.4.102:192.168.1.2 --dstipmap=132.163.4.102:192.168.1.2 --fixcsum
tcprewrite --infile=tester8.pcap --outfile=tester9.pcap --srcipmap=172.24.1.1:192.168.1.2 --dstipmap=172.24.1.1:192.168.1.2 --fixcsum
tcprewrite --infile=tester9.pcap --outfile=tester10.pcap --srcipmap=174.129.249.52:192.168.1.2 --dstipmap=174.129.249.52:192.168.1.2 --fixcsum


tcpprep --auto=first --pcap=$filename --cachefile=cam_in.cache
tcprewrite --infile=tester10.pcap --outfile=tester.pcap --enet-smac=00:15:6d:84:92:cb,00:60:b3:25:c0:37 --enet-dmac=00:60:b3:25:c0:37,00:15:6d:84:92:cb --cachefile=cam_in.cache

echo OK! MAC and IP addresses have been changed to MAC and IP addresses of wlan0 of node1-7 and node1-4.

now="$(date)"
printf "================= %s =================\n" "$now"
echo Start_time: $(date +%s.%N)

tcpreplay -i wlan0 tester.pcap
process_id=$!
wait $process_id

# ### *** For VSVI ***   (Replaying pcap file for the second time to deplete the buffer)
#timeout 6m tcpreplay -i wlan0 tester.pcap
# ### *** For VSCI ***
sleep 3m
# ###

click st7.click

echo
now2="$(date)"
printf "................. The process is completed! ................. %s\n" "$now2"
echo
echo

