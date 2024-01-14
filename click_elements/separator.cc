//&&&&&&&&&&&&&&&&
#include <numeric>
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <typeinfo>
#include <stdlib.h>
//&&&&&&&&&&&&&&&&&
#include <click/config.h>
#include "separator.hh"
#include <click/glue.hh>
#include <click/args.hh>
#include <click/error.hh>
#include <click/straccum.hh>
#ifdef CLICK_LINUXMODULE
# include <click/cxxprotect.h>
CLICK_CXX_PROTECT
# include <linux/sched.h>
CLICK_CXX_UNPROTECT
# include <click/cxxunprotect.h>
#endif
///&&&&&&&&&&&&&&&&&&&&&&&&&&&
#define DATA_RESPONSE_PACKET 3
using namespace std;
///&&&&&&&&&&&&&&&&&&&&&&&&&&&
CLICK_DECLS

Separator::Separator()
{
}

int
Separator::configure(Vector<String> &conf, ErrorHandler* errh)
{
  bool timestamp = false;
#ifdef CLICK_LINUXMODULE
  bool print_cpu = false;
#endif
  bool print_anno = false, headroom = false, bcontents;
  _active = true;
  String label, contents = "HEX";
  int bytes = 24;

     if (Args(conf, this, errh)
        .read_p("LABEL", label)
        .read_p("MAXLENGTH", bytes)
        .read("LENGTH", Args::deprecated, bytes)
        .read("NBYTES", Args::deprecated, bytes)
        .read("CONTENTS", WordArg(), contents)
        .read("TIMESTAMP", timestamp)
        .read("PRINTANNO", print_anno)
        .read("ACTIVE", _active)
        .read("HEADROOM", headroom)
#if CLICK_LINUXMODULE
        .read("CPU", print_cpu)
#endif
        .complete() < 0)
        return -1;
        //&&&&&
       // if(filename == "NONE" || (size1 == 0 && randomness == false) ){
         //   return(errh->error("File name, correct packet size or randomness needs to be set!"));
        //}
        //&&&&&

    if (BoolArg().parse(contents, bcontents))
      _contents = bcontents;
  else if ((contents = contents.upper()), contents == "NONE")
      _contents = 0;
  else if (contents == "HEX")
      _contents = 1;
  else if (contents == "ASCII")
      _contents = 2;
  else
      return errh->error("bad contents value '%s'; should be 'NONE', 'HEX', or 'ASCII'", contents.c_str());

  _label = label;
  _bytes = bytes;
  _timestamp = timestamp;
  _headroom = headroom;
  _print_anno = print_anno;
#ifdef CLICK_LINUXMODULE
  _cpu = print_cpu;
#endif
  return 0;
}

Packet *
Separator::simple_action(Packet *p)
{
    if (!_active)
        return p;

    int bytes = (_contents ? _bytes : 0);
    if (bytes < 0 || (int) p->length() < bytes)
        bytes = p->length();
    StringAccum sa(_label.length() + 2 // label:
                   + 6          // (processor)
                   + 28         // timestamp:
                   + 9          // length |
                   + (_headroom ? 17 : 0) // (h[headroom] t[tailroom])
                   + Packet::anno_size*2 + 3 // annotations |
                   + 3 * bytes);
    if (sa.out_of_memory()) {
        click_chatter("no memory for Separator");
        return p;
    }

    const char *sep = "";
    if (_label) {
        sa << _label;
        sep = ": ";
    }
#ifdef CLICK_LINUXMODULE
    if (_cpu) {
        click_processor_t my_cpu = click_get_processor();
        sa << '(' << my_cpu << ')';
        click_put_processor();
        sep = ": ";
    }
#endif
    if (_timestamp) {
        sa << sep << p->timestamp_anno();
        sep = ": ";
    }

    // sa.reserve() must return non-null; we checked capacity above
    int len;
    len = sprintf(sa.reserve(11), "%s%4d", sep, p->length());
    sa.adjust_length(len); // can be removed for deleting the length at the beginning

    // headroom and tailroom
    if (_headroom) {
        len = sprintf(sa.reserve(16), " (h%d t%d)", p->headroom(), p->tailroom());
        sa.adjust_length(len);
    }

    if (_print_anno) {
        sa << " | ";
        char *buf = sa.reserve(Packet::anno_size * 2);
        int pos = 0;
        for (unsigned j = 0; j < Packet::anno_size; j++, pos += 2)
            sprintf(buf + pos, "%02x", p->anno_u8(j));
        sa.adjust_length(pos);
    }

    if (bytes) {
        sa << " | ";
        char *buf = sa.data() + sa.length();
        const unsigned char *data = p->data();
        if (_contents == 1) {
            for (int i = 0; i < bytes; i++, data++) {
                if (i && (i % 4) == 0)
                    *buf++ = ' ';
                sprintf(buf, "%02x", *data & 0xff);
                buf += 2;
                //&&&&&&&&&&&&&&&&&&&&&&&&&&&
                // Save in char array
                snprintf(myArray + ca_count, 2, "%c", *data);
                ca_count++;
                sum_bytes++;
                //&&&&&&&&&&&&&&&&&&&&&&&&&&&&
            }

        } else if (_contents == 2) {
            for (int i = 0; i < bytes; i++, data++) {
                if ((i % 8) == 0)
                    *buf++ = ' ';
                if (*data < 32 || *data > 126)
                    *buf++ = '.';
                else
                    *buf++ = *data;
            }
        }
        sa.adjust_length(buf - (sa.data() + sa.length()));
    }

  click_chatter("%s", sa.c_str());
  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
  click_chatter ("--   --   --   --   --   --   --   --   --   -- \n");
  total_bytes += bytes;
  count_received_packets++;
  click_chatter("Packet %d is added to the file and char array! Sum of all received bytes to SERVER: %d", count_received_packets, sum_bytes);

//Reading MAC address of packets' first destination
  if(count_received_packets == 1){
     memcpy(tempArray_first_mac, myArray, 6); //6 for MAC
     char *first_mac_pointer = tempArray_first_mac;
     char *buf_first_mac_pointer = buf_first_mac;
     for(int k = 0; k < 6; k++, first_mac_pointer ++) {
         sprintf(buf_first_mac_pointer , "%02x", *first_mac_pointer  & 0xff);
         buf_first_mac_pointer  += 2;
     }
     click_chatter("Packets' first destination was: %s\n", buf_first_mac);
  }
// SEPARATION PROCESS
  flag = 0;
  while(flag == 0){
//Dealing with less than 18
        if(flag2 == 1){ //flag2 is set whenever total_bytes<18
           int mac_pos = 0;
           memcpy(tempArray_mac, myArray + find_length, 17+6);//17+6 remaining+mac length
          int sum_res = 0;
          sum_res = accumulate(tempArray_mac, tempArray_mac+17+6, sum_res);// Checking if all the 23 are not zeros
          if(sum_res != 0){
             char *mac_pointer = tempArray_mac;
             char buf_mac[47];
             char *buf_mac_pointer = buf_mac;
             for(int k = 0; k < 23; k++, mac_pointer ++) {
                 sprintf(buf_mac_pointer , "%02x", *mac_pointer  & 0xff);
                 buf_mac_pointer  += 2;
             }
             //char mac_check[] = "708bcdbc8037";
             //click_chatter("MAC address: %s ", buf_first_mac);
             click_chatter("Buffer contents for finding MAC: %s\n", buf_mac);
             char * pch = strstr(buf_mac, buf_first_mac);//mac_check);
             mac_pos = (pch - buf_mac)/2;
             click_chatter("MAC position: %d", mac_pos);
             find_length += mac_pos;
             total_bytes -= mac_pos;
           }
           flag2 = 0;
        }
//Finding protocol
        memcpy(tempArray_protocol, myArray + find_length + 12, 2);//12->protocol place
        char *pro_pointer =  tempArray_protocol;
        char buf_pro[3];
        char *buf_pro_pointer =  buf_pro;
       for (int k = 0; k < 2; k++, pro_pointer ++) {
            sprintf(buf_pro_pointer , "%02x", *pro_pointer  & 0xff);
            buf_pro_pointer  += 2;
       }
//Finding length
       memcpy(tempArray_len, myArray + find_length + 16, 2);//16->length palce
       char *len_pointer =  tempArray_len;
       char buf_len[10];
       char *buf_pointer =  buf_len;
       for (int k = 0; k < 2; k++, len_pointer ++) {
            sprintf(buf_pointer , "%02x", *len_pointer  & 0xff);
            buf_pointer  += 2;
       }
//Calculating total length and applying excpetions
      char arp_check[] = "0806";
      if (strcmp(buf_pro, arp_check) == 0 ){
          pac_len = 28; //exceptional case
      }else{
          pac_len = stoi(buf_len, 0, 16);// general case
      }
      char eapol_check[] = "888e";
      if (strcmp(buf_pro, eapol_check) == 0 ){
          find_length += (pac_len + 18);//exceptional case
          size = pac_len + 18;
      }else{
          find_length += (pac_len + 14);//general case
          size = pac_len + 14;
      }
//Printing found  length and protocol
      if (pac_len != 0){
          click_chatter("==> Protocol: %s\n", buf_pro);
          click_chatter("--> Length:   %zd == %s\n", size, buf_len);
      }
//Reconstructing and sending the packets
       if ((size > total_bytes)){
           find_length -= size;
           flag = 1;
           click_chatter("Not enough bytes in the buffer, size > total_bytes");
           break;
       }
       end_point = find_length;
       start_point = end_point - size;
       how_many = end_point - start_point;
       if(pac_len != 0){
           Send_SSP();
           Timestamp now = Timestamp::now();
           if(count_sent_packets == 1){
              temp_time1 = (now).doubleval();
           }
           double time_elapsed = (now).doubleval() - temp_time1;
           click_chatter("Timestamp: %u.%u  -----  Elapsed time: %f", now.sec(), now.usec(), time_elapsed);
           click_chatter("Remaining bytes in buffer after taking out the above packet: %d", total_bytes-how_many);
           click_chatter("=================================================================== \n");
       }
       total_bytes = total_bytes -  how_many;
//Dealing with the case if remaining bytes is  less than 18 bytes
       if(total_bytes < 18){
          flag  = 1;
          flag2 = 1;
          click_chatter("Not enough bytes in the buffer, total_bytes < 18");
          break;
       }
   }

  return 0;
  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//return p;
}

//&&&&&&&&&&&&&&&&&&&&&&
void
Separator::Send_SSP(){
      struct data_response_packet* data_p;
      WritablePacket *pkt = Packet::make(size);
      data_p = reinterpret_cast<struct data_response_packet *> (pkt->data());
      char *payload= (char*)(pkt->data());
      memcpy(tempArray, myArray + start_point, how_many);
      memcpy(payload, tempArray, size);
      memset (tempArray, 0, sizeof tempArray);
      count_sent_packets++;
      click_chatter("Packet %d is reconstructed with a size of  %zd.", count_sent_packets, size);
      output(0).push(pkt);
}
//&&&&&&&&&&&&&&&&&&&&&&

void

Separator::add_handlers()
{
    add_data_handlers("active", Handler::OP_READ | Handler::OP_WRITE | Handler::CHECKBOX | Handler::CALM, &_active);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(Separator)
ELEMENT_MT_SAFE(Separator)
