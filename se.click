cl1 :: Classifier(12/0800,-)
cl2 :: Classifier(12/0800 34/11111111111111111111111111111111111111111111111111111111111111111111111111111111,-)
chLen :: CheckLength(64)

FromDevice(eth0)
 -> HostEtherFilter(00:03:1d:0a:3a:4c,DROP_OWN true, DROP_OTHER false)
 -> cl1
      cl1[1] -> Discard;
      cl1[0] -> chLen
                    chLen[0] -> Discard;
                    chLen[1] -> Strip(54)
                             -> cl2
                                  cl2[1] -> Separator(MAXLENGTH -1) -> Print(MAXLENGTH -1) -> ToDump($outname);
                                  cl2[0] -> Print ("LAST_PACKET", MAXLENGTH -1)
                                         -> sx :: Script(TYPE PACKET, init End_Flag 0, set End_Flag $(add $End_Flag 1), print "End_Flag = "$End_Flag, return $End_Flag)
                                             sx[0] -> Discard;
                                             sx[1] -> Script(TYPE PACKET, goto stop $(ne $ns 1), wait 0.5s, stop);
                                             sx[2] -> Script(TYPE PACKET, goto stop $(ne $ns 2), wait 0.5s, stop);
                                             sx[3] -> Script(TYPE PACKET, goto stop $(ne $ns 3), wait 0.5s, stop);
