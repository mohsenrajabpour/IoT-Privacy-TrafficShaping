#!/bin/bash
bold="\e[1m"
inv="\e[7"
red="\e[91m"
green="\e[92m"
yellow="\e[93m"
reset="\e[0m"

source ./config.sh
echo
echo -e "${yellow}Setting up nodes (Stations: $node3, $node5, $node8 ;  AP:$node4 ; Server:$node6) ...${reset}"
echo

ssh root@$node4 'bash -s' <run_ap.sh
#ssh root@$node3 'bash -s' <run_station3.sh 
ssh root@$node6 'bash -s' <run_server.sh

ssh root@$node3 'bash -s' <run_station3.sh
ssh root@$node5 'bash -s' <run_station5.sh
#ssh root@$node7 'bash -s' <run_station7.sh
ssh root@$node8 'bash -s' <run_station8.sh

now="$(date)"
printf "================= Completed!  %s =================\n" "$now"
#echo $(date +%s.%N)
echo

