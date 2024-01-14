#ifndef CLICK_BUFFERTIMEARRIVAL_HH
#define CLICK_BUFFERTIMEARRIVAL_HH
#include <click/element.hh>
#include <click/string.hh>
//&&&&&
#include <random>
#include <click/timer.hh>
//&&&&&
CLICK_DECLS


class BufferTimeArrival : public Element { public:

    BufferTimeArrival() CLICK_COLD;

    const char *class_name() const              { return "BufferTimeArrival"; }
    const char *port_count() const              { return PORTS_1_1; }

    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
    //&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
    int initialize(ErrorHandler *) CLICK_COLD;
    void run_timer(Timer *t);
    void Send_SSP();
    int Gen_Sizes();
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
    int dim1, dim2, row, current_length;
    std::vector <std::vector <double>>pmf_2Dvec;
    std::vector<int> in_sizes_vector;
    std::vector<int> out_sizes_vector;
    std::vector<double> pmf_vec;
    std::default_random_engine generator;
    std::discrete_distribution<int> distribution;
    String pmf;
    String in_sizes;
    String out_sizes;

    double _Pr;
    bool OneZero = 1;

    bool randomness;
    int currenT = 0;
    char tempArray[5000];
    int end_point;
    int start_point;
    int how_many;
    int sum_bytes = 0;
    int ca_count = 0;
    size_t size, size1, size2;
    int delay;
    Timer _timer;
    int total_bytes = 0;
    char myArray[10000000];
    int count_sent_packets = 0;
    int count_received_packets = 0;
    String filename; //content filename
    Vector<String> datafile;
//&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
};

CLICK_ENDDECLS
#endif

