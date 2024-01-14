#!/bin/bash

echo

tshark -w $outfile -F pcap -q &>x.log 2>y.log &
_pid=$!
sleep 5

filename=BelkinWemoSwitch.pcap
sIP=172.24.1.147

dIP1=239.255.255.250
dIP2=54.235.54.58
dIP3=54.235.111.253
dIP4=54.225.138.155
dIP5=23.23.165.206
dIP6=23.23.162.27
dIP7=174.129.249.52
dIP8=172.24.1.1
dIP9=132.163.4.102
dIP10=107.22.249.250


tcprewrite --infile=$filename --outfile=tester1.pcap --srcipmap=$sIP:192.168.1.3 --dstipmap=$dIP1:192.168.1.2 --fixcsum
tcprewrite --infile=tester1.pcap --outfile=tester2.pcap --srcipmap=$sIP:192.168.1.3 --dstipmap=$dIP2:192.168.1.2 --fixcsum
tcprewrite --infile=tester2.pcap --outfile=tester3.pcap --srcipmap=$sIP:192.168.1.3 --dstipmap=$dIP3:192.168.1.2 --fixcsum
tcprewrite --infile=tester3.pcap --outfile=tester4.pcap --srcipmap=$sIP:192.168.1.3 --dstipmap=$dIP4:192.168.1.2 --fixcsum
tcprewrite --infile=tester4.pcap --outfile=tester5.pcap --srcipmap=$sIP:192.168.1.3 --dstipmap=$dIP5:192.168.1.2 --fixcsum
tcprewrite --infile=tester5.pcap --outfile=tester6.pcap --srcipmap=$sIP:192.168.1.3 --dstipmap=$dIP6:192.168.1.2 --fixcsum
tcprewrite --infile=tester6.pcap --outfile=tester7.pcap --srcipmap=$sIP:192.168.1.3 --dstipmap=$dIP7:192.168.1.2 --fixcsum
tcprewrite --infile=tester7.pcap --outfile=tester8.pcap --srcipmap=$sIP:192.168.1.3 --dstipmap=$dIP8:192.168.1.2 --fixcsum
tcprewrite --infile=tester8.pcap --outfile=tester9.pcap --srcipmap=$sIP:192.168.1.3 --dstipmap=$dIP9:192.168.1.2 --fixcsum
tcprewrite --infile=tester9.pcap --outfile=tester10.pcap --srcipmap=$sIP:192.168.1.3 --dstipmap=$dIP10:192.168.1.2 --fixcsum

tcprewrite --infile=tester10.pcap --outfile=tester11.pcap --srcipmap=$dIP1:192.168.1.2 --dstipmap=$sIP:192.168.1.3 --fixcsum
tcprewrite --infile=tester11.pcap --outfile=tester12.pcap --srcipmap=$dIP2:192.168.1.2 --dstipmap=$sIP:192.168.1.3 --fixcsum
tcprewrite --infile=tester12.pcap --outfile=tester13.pcap --srcipmap=$dIP3:192.168.1.2 --dstipmap=$sIP:192.168.1.3 --fixcsum
tcprewrite --infile=tester13.pcap --outfile=tester14.pcap --srcipmap=$dIP4:192.168.1.2 --dstipmap=$sIP:192.168.1.3 --fixcsum
tcprewrite --infile=tester14.pcap --outfile=tester15.pcap --srcipmap=$dIP5:192.168.1.2 --dstipmap=$sIP:192.168.1.3 --fixcsum
tcprewrite --infile=tester15.pcap --outfile=tester16.pcap --srcipmap=$dIP6:192.168.1.2 --dstipmap=$sIP:192.168.1.3 --fixcsum
tcprewrite --infile=tester16.pcap --outfile=tester17.pcap --srcipmap=$dIP7:192.168.1.2 --dstipmap=$sIP:192.168.1.3 --fixcsum
tcprewrite --infile=tester17.pcap --outfile=tester18.pcap --srcipmap=$dIP8:192.168.1.2 --dstipmap=$sIP:192.168.1.3 --fixcsum
tcprewrite --infile=tester18.pcap --outfile=tester19.pcap --srcipmap=$dIP9:192.168.1.2 --dstipmap=$sIP:192.168.1.3 --fixcsum
tcprewrite --infile=tester19.pcap --outfile=tester20.pcap --srcipmap=$dIP10:192.168.1.2 --dstipmap=$sIP:192.168.1.3 --fixcsum

tcpprep --auto=first --pcap=$filename --cachefile=cam_in.cache
tcprewrite --infile=tester20.pcap --outfile=tester.pcap --enet-smac=00:15:6d:84:92:cb,00:15:6d:84:92:cd --enet-dmac=00:15:6d:84:92:cd,00:15:6d:84:92:cb --cachefile=cam_in.cache


echo OK! MAC and IP addresses have been changed to MAC and IP addresses of wlan0 of node1-5 and node1-4.

now="$(date)"
printf "================= %s =================\n" "$now"
echo Start_time: $(date +%s.%N)

tcpreplay -i wlan0 tester.pcap
#process_id=$!
#wait $process_id

sleep 0.5
killall -2 tshark

# ### *** For VSVI ***   (Replaying pcap file for the second time to deplete the buffer)
#timeout 6m tcpreplay -i wlan0 tester.pcap
# ### *** For VSCI ***
sleep 3m
# ###

click st5.click

echo
now2="$(date)"
printf "................. The process is completed! ................. %s\n" "$now2"
echo
echo

