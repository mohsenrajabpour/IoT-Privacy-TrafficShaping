
c  :: Classifier(12/0800 34/11111111111111111111111111111111111111111111111111111111111111111111111111111111, 12/0800, -)
sw :: Switch($sht)
T  :: Print(MAXLENGTH -1) -> TCPHead(192.168.2.1, 443, 192.168.2.2, 57396, SEQNO 1,ACKNO 1, FLAGS 0x018) -> EtherEncap(0x0800, d8:50:e6:d4:f5:2d, 70:8b:cd:bc:80:37)-> Queue -> ToDevice(eth0)

FromDevice(wlan0)
 -> c
     c[2] -> Discard;
     c[0] -> T;
     c[1] -> sw
               sw[0] -> BufferTimeArrival(MAXLENGTH -1, FILENAME out, DELAY $d, SIZE1 $s1, SIZE2 $s2, PROB $p, RANDOMNESS $r, PMF "$pmf", OUT_SIZES "$out_sizes", IN_SIZES "$in_sizes") -> T
               sw[1] -> BufferArrival(MAXLENGTH -1, FILENAME out, DELAY $d, SIZE $s1, RANDOMNESS $r, PMF "$pmf", OUT_SIZES "$out_sizes") -> T
               sw[2] -> BufferTime(MAXLENGTH -1, FILENAME out, DELAY $d, SIZE $s1, RANDOMNESS $r, PMF "$pmf", OUT_SIZES "$out_sizes") -> T
