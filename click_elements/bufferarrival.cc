//&&&&&&&&&&&&&&&&
#include <chrono>
#include <vector>
#include <random>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <cstring>

#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string>
#include <typeinfo>
#include <stdlib.h>
//&&&&&&&&&&&&&&&&&
#include <click/config.h>
#include "bufferarrival.hh"
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

BufferArrival::BufferArrival()
//&&&&&
   : _timer(this)
//&&&&&
{
}

int
BufferArrival::configure(Vector<String> &conf, ErrorHandler* errh)
{
  bool timestamp = false;
#ifdef CLICK_LINUXMODULE
  bool print_cpu = false;
#endif
  bool print_anno = false, headroom = false, bcontents;
  _active = true;
  String label, contents = "HEX";
  int bytes = 24;
  //&&
  srand (time(NULL));
  //&&

     if (Args(conf, this, errh)
        //&&&
        .read("OUT_SIZES", out_sizes)
        .read("PMF", pmf)
        .read("DELAY", delay)

        .read_p("FILENAME", FilenameArg(), filename)
        .read("SIZE", size1)
        .read("RANDOMNESS", randomness)
        //&&&
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
        if(filename == "NONE" || (size1 == 0 && randomness == false) ){
            return(errh->error("File name, correct packet size or randomness needs to be set!"));
        }
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
//&&&&&&&&&&&&&&&
  // Vectorizing output sizes
  std::string str_sizes(out_sizes.c_str());
  istringstream is1(str_sizes);
  out_sizes_vector.assign(istream_iterator<double>(is1), istream_iterator<double>());
  // Vectorizing pmf
  std::string str_pmf(pmf.c_str());
  str_pmf.erase (remove(str_pmf.begin (), str_pmf.end (), ';'), str_pmf.end ());
  istringstream is2(str_pmf);
  pmf_vec.assign(istream_iterator<double>(is2), istream_iterator<double>());

//&&&&&&&&&&&&&&&

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
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
int
BufferArrival::initialize(ErrorHandler *) {
        _timer.initialize(this);
        return 0;
}
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&

Packet *
BufferArrival::simple_action(Packet *p)
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
        click_chatter("no memory for BufferArrival");
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
                // Save in a file
                if(filename){
                  FILE *pFile;
                  if((pFile = freopen(filename.c_str(), "a" ,stdout)) == NULL) {
                    printf("Cannot open file.\n");
                    exit(1);
                  }
                  printf("%c", *data);
                  fclose(pFile);
                }
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
  
  click_chatter ("--   --   --   --   --   --   --   --   --   -- ");
  total_bytes += bytes;
  if(count_received_packets == 0){
    //_timer.schedule_after_msec(delay);  //millisecond 
    _timer.schedule_after(Timestamp::make_usec(delay)); //microsecond
  }
  count_received_packets++;
  click_chatter("Packet %d is added to the file and char array! Sum of all bytes: %d",  count_received_packets, sum_bytes);
  click_chatter("Total bytes in buffer (before sending): %d.", total_bytes);

  return 0;
  //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
//return p;
}

//&&&&&&&&&&&&&&&&&&&&&&
int
BufferArrival::Gen_Sizes() {
  unsigned seed = std::chrono::system_clock::now ().time_since_epoch ().count ();
  std::default_random_engine generator (seed);
  std::discrete_distribution <int>distribution (pmf_vec.begin (), pmf_vec.end ());
  int size_index = distribution (generator);
  int gen_size = out_sizes_vector[size_index];
  return gen_size;
}

void
BufferArrival::run_timer(Timer *t){
  if (randomness == true && currenT != count_received_packets){
     size = Gen_Sizes();
  }
  else if (randomness == false){
     size = size1;
  }

  if (size !=0 && currenT != count_received_packets){
     click_chatter("--> Current packaging size is: %zd ", size);
     end_point = sum_bytes;
     start_point = end_point - total_bytes;
     how_many = end_point - start_point;
     if (how_many > size){
        how_many = size;
     }
     Send_SSP();
     total_bytes = total_bytes -  how_many;
     click_chatter("Remaining bytes in buffer after sending: %d", total_bytes);
  }
  currenT = count_received_packets;
  //_timer.reschedule_after_msec(delay);
  _timer.reschedule_after(Timestamp::make_usec(delay));
  time_t rawtime;//show time
  time (&rawtime);
  click_chatter ("===================%s=========================================================================== \n", asctime(localtime (&rawtime)));
}

void
BufferArrival::Send_SSP(){
      struct data_response_packet* data_p;
      WritablePacket *pkt = Packet::make(size);
      data_p = reinterpret_cast<struct data_response_packet *> (pkt->data());
      char *payload= (char*)(pkt->data());
      memcpy(tempArray, myArray + start_point, how_many);
      memcpy(payload, tempArray, size);
      memset (tempArray, 0, sizeof tempArray);
      count_sent_packets++;
      click_chatter("Packet %d is sent to the server with a size of  %zd.", count_sent_packets, size);
      output(0).push(pkt);
}
//&&&&&&&&&&&&&&&&&&&&&&

void

BufferArrival::add_handlers()
{
    add_data_handlers("active", Handler::OP_READ | Handler::OP_WRITE | Handler::CHECKBOX | Handler::CALM, &_active);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(BufferArrival)
ELEMENT_MT_SAFE(BufferArrival)

