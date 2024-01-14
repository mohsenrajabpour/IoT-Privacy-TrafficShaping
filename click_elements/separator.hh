#ifndef CLICK_SEPARATOR_HH
#define CLICK_SEPARATOR_HH
#include <click/element.hh>
#include <click/string.hh>
//&&&&&
#include <click/timer.hh>
//&&&&&
CLICK_DECLS


class Separator : public Element { public:

    Separator() CLICK_COLD;

    const char *class_name() const              { return "Separator"; }
    const char *port_count() const              { return PORTS_1_1; }

    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    void Send_SSP();
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    bool can_live_reconfigure() const           { return true; }
    void add_handlers() CLICK_COLD;

    Packet *simple_action(Packet *);

 private:

    String _label;
    int _bytes;         // How many bytes of a packet to print
    bool _active;
    bool _timestamp : 1;
    bool _headroom : 1;
#ifdef CLICK_LINUXMODULE
    bool _cpu : 1;
#endif
    bool _print_anno;
    uint8_t _contents;
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
   char buf_first_mac[13];
   char tempArray_first_mac[13];
   bool flag2 = 0;
   char tempArray_mac[47];

    double temp_time1;
    bool flag;
    int  pac_len = 0;
    int  find_length = 0;
    char tempArray_len[10];
    char tempArray_protocol[10];
    int  how_many = 0;
    char tempArray[5000];
    int  start_point;
    int  end_point;
    int  sum_bytes = 0;
    int  ca_count = 0;
    int  size, size1;
    int  total_bytes = 0;
    char myArray[100000000];
    int  count_sent_packets = 0;
    int  count_received_packets = 0;
    String filename; //content filename
    Vector<String> datafile;
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
};

CLICK_ENDDECLS
#endif
