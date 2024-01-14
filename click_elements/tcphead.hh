#ifndef CLICK_TCPHEAD_HH
#define CLICK_TCPHEAD_HH
#include <click/element.hh>
#include <click/glue.hh>
#include <clicknet/tcp.h>
CLICK_DECLS


class TCPHead : public Element {
public:
  TCPHead() CLICK_COLD;
  ~TCPHead() CLICK_COLD;

    const char *class_name() const      { return "TCPHead"; }
    const char *port_count() const      { return PORTS_1_1; }
    const char *flags() const           { return "A"; }

    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
    void add_handlers() CLICK_COLD;

    Packet *simple_action(Packet *);
private:
    struct in_addr _saddr;
    struct in_addr _daddr;
    uint16_t _sport;
    uint16_t _dport;
    unsigned int _seqn;
    unsigned int _ackn;
    unsigned char _bits;
    bool _cksum;
    bool _use_dst_anno;
    atomic_uint32_t _id;

    static String read_handler(Element *, void *) CLICK_COLD;
};

CLICK_ENDDECLS
#endif
