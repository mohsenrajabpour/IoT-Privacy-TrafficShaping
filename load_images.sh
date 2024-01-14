#!/bin/bash

bold="\e[1m"
inv="\e[7"
red="\e[91m"
green="\e[92m"
yellow="\e[93m"
reset="\e[0m"

source ./config.sh
echo
echo -e "${yellow}This is going to load image 2 on $node4 and $node6, and image 1 on the remaining nodes.${reset}"
echo 
# User Input
read -p 'Image name 1: ' imagename1
read -p 'Image name 2: ' imagename2

omf tell -a offh -t all &
process_id=$!
wait $process_id
echo " === All nodes are off === "

omf load -i $imagename1 -t $node1,$node2,$node3,$node5,$node7,$node8,$node9 &
process_id=$!
wait $process_id
echo " === First Image is loaded === "

omf load -i $imagename2 -t $node4,$node6 &
process_id=$!
wait $process_id
echo " === Second Image is loaded === "

omf tell -a on -t all &
process_id=$!
wait $process_id
echo " === All nodes are on === "

wget -O- http://internal2dmz.orbit-lab.org:5054/instr/setAll?att=0
wget -O status http://internal2dmz.orbit-lab.org:5054/instr/selDevice?switch=1\&port=1
wget -O status http://internal2dmz.orbit-lab.org:5054/instr/selDevice?switch=2\&port=1
wget -O status http://internal2dmz.orbit-lab.org:5054/instr/selDevice?switch=3\&port=1
wget -O status http://internal2dmz.orbit-lab.org:5054/instr/selDevice?switch=4\&port=1
wget -O status http://internal2dmz.orbit-lab.org:5054/instr/selDevice?switch=5\&port=1
wget -O status http://internal2dmz.orbit-lab.org:5054/instr/selDevice?switch=6\&port=1
wget -O status http://internal2dmz.orbit-lab.org:5054/instr/selDevice?switch=7\&port=1
wget -O status http://internal2dmz.orbit-lab.org:5054/instr/selDevice?switch=8\&port=1

now="$(date)"
printf "================= Completed!  %s =================\n" "$now"
