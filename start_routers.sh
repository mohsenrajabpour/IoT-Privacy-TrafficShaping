#!/bin/bash

d=240 # Microsecond;  250 for LGTV, 240 for wemo. 10000 for nest

s1=1000
s2=200
p=0.9

r=true

#-------------------------------------------------- Config ---------------------------------------------------
echo
cc=0
for ii in {16..21} {21..16}
do
   if [ $ii = 21 ] && [ $cc = 0 ]
   then
       echo -en "\e[48;5;${ii}m Mohsen Rajabpour \e[0m"
       ((cc++))
   elif [ $ii = 21 ] && [ $cc = 1 ]
   then
       echo -en "\e[48;5;${ii}m      WINLAB      \e[0m"
   else
       echo -en "\e[48;5;${ii}m        \e[0m"
   fi
   done
echo

bold="\e[1m"
inv="\e[7"
red="\e[91m"
green="\e[92m"
yellow="\e[93m"
reset="\e[0m"
#----------------------------------------- User Input---------------------------------------------------------
source ./config.sh

flag1=0
flag2=0

while [ $flag1 = 0 ] && [ $flag2 = 0 ]
do 
  read -e -p 'Shaper (DPS_VSVI, DPS_VSCI, VSVI, VSCI): ' Shaper
  read -e -p 'Number of stations: ' NS
# ###
  if [ $NS = 1 ]
  then
      read -e -p 'Sensor(nest, wemo, lgtv): ' SENS
      if [ $SENS != nest ] && [ $SENS != wemo ] && [ $SENS != lgtv ]
      then
          echo -e "${red}Wrong enetry for the name of sensor!${reset}"
          flag2=0
      fi
  fi
# ###
  read -e -p 'how many experiments: ' NE
  read -e -p 'Waiting time after replaying in stations: ' WT

  echo -e "${yellow}1. If this is a new set of experiments: ${green}How_many_archived = 0   First_row = 1 \n${yellow}2. If this is an addition to previous completed set of experiments: ${green}How_many_archived = Number of archived experiments   First_row = 1 \n${yellow}3. If the scheduled experiment stopped in the middle of an ongoing process: ${green}How_many_archived = 0   First_row = Where the process stopped${reset}"
  read -e -p 'How_many_archived: ' HA
  read -e -p 'First_row: ' FR

  echo

# ###
#--------------------------------------------- Input and output sizes --------------------------------------------------------
  if [ $SENS = nest ]
  then
      in_sizes="0 66 156 172 284"
      out_sizes="0 66 156 172 284"
  elif [ $SENS = wemo ]
  then
      out_sizes="0 683 765 870 1009 1200 1460"
      in_sizes="0 54 66 72 90 91 94 114 128 159 168 183 188 202 210 222 294 370 380 424 444 464 496 518 538 592 612 632 666 686 726 740 760 806 814 834 868 888 962 982 1036 1090 1110 1130 1184 1224 1258 1332 1352 1406 1480 1514"
  elif [ $SENS = lgtv ]
  then
      in_sizes="0 66 74 97 192 353 358 583 1101"      #"0 54 107 123 139 171 187 299 315 747 763 1131"
      out_sizes="0 66 74 97 192 353 358 583 1101"     #"0 54 107 123 139 171 187 299 315 747 763 1131"
  fi
# ###

#--------------------------------------------- Shaper --------------------------------------------------------
  if [ $Shaper = vsci ]
  then
    SHT=2
    pmf_file=VSCI_CSCI_$SENS.txt
    flag1=1
  elif [ $Shaper = vsvi ]
  then
    SHT=1
    pmf_file=VSVI_CSVI_$SENS.txt
    flag1=1
  elif [ $Shaper = dps_vsci ] 
  then
    SHT=0
    pmf_file=DPS_VSCI_$SENS.txt
    flag1=1
  elif [ $Shaper = dps_vsvi ] 
  then
    SHT=0
    pmf_file=DPS_VSVI_$SENS.txt
    flag1=1
  else
    echo -e "${red}Wrong enetry as shapers name!${reset}"
    flag1=0
  fi
done
#------------------------------------------ Transfer files to nodes ------------------------------------------
echo -e "${red}+${reset} Loading all nessessary files ..."

        nohup scp ap.click root@$node4:. >/dev/null 2>&1
        nohup scp se.click root@$node6:. >/dev/null 2>&1

if [ $NS = 1 ]
then
# ###
        if [ $SENS = nest ]
        then
            nohup scp NestCamMotionDetection.pcap run_tcpreplay13.sh st3.click root@$node3:. >/dev/null 2>&1
        elif [ $SENS = wemo ]
        then
#            nohup scp BelkinWemoSwitch.pcap       run_tcpreplay17.sh st7.click root@$node7:. >/dev/null 2>&1
            nohup scp BelkinWemoSwitch.pcap       run_tcpreplay15.sh st5.click root@$node5:. >/dev/null 2>&1
        elif [ $SENS = lgtv ]
        then
            nohup scp LGTV.pcap     run_tcpreplay18.sh st8.click root@$node3:. >/dev/null 2>&1
        fi
# ###
<< 'C1'
elif [ $NS = 2 ]
then
            nohup scp NestCamMotionDetection.pcap run_tcpreplay13.sh st3.click root@$node3:. >/dev/null 2>&1
            #nohup scp BelkinWemoSwitch.pcap       run_tcpreplay17.sh st7.click root@$node7:. >/dev/null 2>&1
            nohup scp BelkinWemoSwitch.pcap       run_tcpreplay15.sh st5.click root@$node5:. >/dev/null 2>&1
else
            nohup scp NestCamMotionDetection.pcap run_tcpreplay13.sh st3.click root@$node3:. >/dev/null 2>&1
            #nohup scp BelkinWemoSwitch.pcap       run_tcpreplay17.sh st7.click root@$node7:. >/dev/null 2>&1
            nohup scp BelkinWemoSwitch.pcap       run_tcpreplay15.sh st5.click root@$node5:. >/dev/null 2>&1
            nohup scp SenseSleepMonitor2.pcap     run_tcpreplay18.sh st8.click root@$node8:. >/dev/null 2>&1
C1
fi
#------------------------------------------------- Main ------------------------------------------------------

first_row=$FR
i=$FR

let end_row=$first_row+$NE-1
sed -n ''$first_row','$end_row' p' $pmf_file | while read line
do
        pmf=${line}
        
        let i1=2*$((i+HA))-1+100
        let i2=2*$((i+HA))+100
        let i3=$((i+HA))

        echo -e "${red}++${reset} Starting all the routers ..."

        ssh -n root@$node4 "nohup click ap.click d=$d s1=$s1 s2=$s2 p=$p r=$r sht=$SHT pmf='$pmf' out_sizes='$out_sizes' in_sizes='$in_sizes' &>LOG4${i1}.log 2>LOG4${i2}.log &"
        ssh -n root@$node6 "nohup click se.click outname=OUT${i3} ns=$NS &>LOG6${i1}.log 2>LOG6${i2}.log &"

        echo -e "${red}+++${reset} Replaying the packets ..."

        if [ $NS = 1 ]
        then
# ###
            if [ $SENS = nest ]
            then
                ssh -n root@$node3 "outfile=Sent3${i3} nohup ./run_tcpreplay13.sh &>LOG3${i1}.log 2>LOG3${i2}.log &"
            elif [ $SENS = wemo ]
            then
                #ssh -n root@$node7 "nohup ./run_tcpreplay17.sh &>LOG7${i1}.log 2>LOG7${i2}.log &"
                ssh -n root@$node5 "outfile=Sent5${i3} nohup ./run_tcpreplay15.sh &>LOG5${i1}.log 2>LOG5${i2}.log &"
            elif [ $SENS = lgtv ]
            then
                ssh -n root@$node3 "outfile=Sent8${i3} nohup ./run_tcpreplay18.sh &>LOG8${i1}.log 2>LOG8${i2}.log &"
            fi
# ###
<< 'C2'
        elif [ $NS = 2 ]
        then
                ssh -n root@$node3 "outfile=Sent3${i3} nohup ./run_tcpreplay13.sh &>LOG3${i1}.log 2>LOG3${i2}.log &"
                #ssh -n root@$node7 "nohup ./run_tcpreplay17.sh &>LOG7${i1}.log 2>LOG7${i2}.log &"
                ssh -n root@$node5 "outfile=Sent5${i3} nohup ./run_tcpreplay15.sh &>LOG5${i1}.log 2>LOG5${i2}.log &"
        else
                ssh -n root@$node3 "outfile=Sent3${i3} nohup ./run_tcpreplay13.sh &>LOG3${i1}.log 2>LOG3${i2}.log &"
                #ssh -n root@$node7 "nohup ./run_tcpreplay17.sh &>LOG7${i1}.log 2>LOG7${i2}.log &"
                ssh -n root@$node5 "outfile=Sent5${i3} nohup ./run_tcpreplay15.sh  &>LOG5${i1}.log 2>LOG5${i2}.log &"
                ssh -n root@$node8 "nohup ./run_tcpreplay18.sh &>LOG8${i1}.log 2>LOG8${i2}.log &"
C2
        fi

        echo -e "${red}++++${reset} Packets are being sent ..."
        sleep $WT
        echo -e "${red}+++++${reset} Stoping all routers process ..."

        while :
        do
                nohup scp root@$node6:LOG6${i1}.log . >/dev/null 2>&1
                while ! grep "End_Flag" LOG6${i1}.log
                do 
                    echo "End_Flag not found yet!"
                    sleep 10
                    nohup scp root@$node6:LOG6${i1}.log . >/dev/null 2>&1
                done

                EF=$(grep End_Flag LOG6${i1}.log  | cut -f2 -d"=" | tail -1 | awk '{$1=$1};1')
                echo EF: $EF
                if [ $EF = $NS ]
                        then
                        echo -e "${green}Server reconstructed the last packet and is going to stop${reset}"
                        ssh -n root@$node4 "killall -2 click"
                        echo -e "${green}AP has been stopped too${reset}"
                        break
                fi
                echo -e "${yellow}Server and AP are still working${reset} ..."
                sleep 10
        done
#--------------------------------------- Dealing with output files ----------------------------------------
        rm LOG6${i1}.log

        ssh -n root@$node6 "nohup tshark -q -r OUT${i3} -Y '!(ip.src==192.168.1.2)'  -T fields -e frame.time_epoch &>Timestamp6${i3}  &"

        if [ $SENS = nest ]
        then
            ssh -n root@$node3 "nohup tshark -q -r tester.pcap -z endpoints,ipv4 &>Stat3${i3}  &"
            ssh -n root@$node3 "nohup tshark -q -r Sent3${i3} -Y '(ip.dst==192.168.1.2)&&!(icmp)'  -T fields -e frame.time_epoch &>Timestamp3${i3}  &"
        elif [ $SENS = wemo ]
        then
            #ssh -n root@$node7 "nohup tshark -q -r tester.pcap -z endpoints,ipv4 &>Stat${i3}  &"
            ssh -n root@$node5 "nohup tshark -q -r tester.pcap -z endpoints,ipv4 &>Stat5${i3}  &"
            ssh -n root@$node5 "nohup tshark -q -r Sent5${i3} -Y '(ip.dst==192.168.1.2)&&!(icmp)'  -T fields -e frame.time_epoch &>Timestamp5${i3}  &"
        elif [ $SENS = lgtv ]
        then
            ssh -n root@$node3 "nohup tshark -q -r tester.pcap -z endpoints,ipv4 &>Stat8${i3}  &"
            ssh -n root@$node3 "nohup tshark -q -r Sent8${i3} -Y '(ip.dst==192.168.1.2)&&!(icmp)'  -T fields -e frame.time_epoch &>Timestamp8${i3}  &"
        fi

        echo -e "${red}++++++${reset} Transfering required files to Results folder ..."
        nohup scp root@$node6:OUT${i3}      /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1
        nohup scp root@$node6:LOG6${i2}.log /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1
        nohup scp root@$node6:Timestamp6${i3}      /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1

        nohup scp root@$node3:Stat3${i3}     /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1
        nohup scp root@$node3:LOG3${i1}.log /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1
        nohup scp root@$node3:Timestamp3${i3} /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1
        nohup scp root@$node3:Sent3${i3}     /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1

        #nohup scp root@$node7:Stat${i3}     /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1
        #nohup scp root@$node7:LOG7${i1}.log /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1
        nohup scp root@$node5:Stat5${i3}     /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1
        nohup scp root@$node5:LOG5${i1}.log /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1
        nohup scp root@$node5:Timestamp5${i3} /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1
        nohup scp root@$node5:Sent5${i3}     /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1
        
        nohup scp root@$node3:Stat8${i3}     /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1
        nohup scp root@$node3:LOG8${i1}.log /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1
        nohup scp root@$node3:Timestamp8${i3} /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1
        nohup scp root@$node3:Sent8${i3}     /home/mohsen/IoT_Privacy/Results >/dev/null 2>&1

        ((i++))
done

