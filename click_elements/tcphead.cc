#include <click/config.h>
#include "tcphead.hh"
#include <clicknet/ip.h>
#include <clicknet/tcp.h>
#include <click/args.hh>
#include <click/error.hh>
#include <click/glue.hh>
#include <click/router.hh>
CLICK_DECLS

TCPHead::TCPHead()
: _cksum(true), _use_dst_anno(false)
{
    _id = 0;
#if HAVE_FAST_CHECKSUM && FAST_CHECKSUM_ALIGNED
    _checked_aligned = false;
#endif
}

TCPHead::~TCPHead()
{
}

int
TCPHead::configure(Vector<String> &conf, ErrorHandler *errh)
{
    IPAddress saddr,daddr;
    String daddr_str;
    bool cksum;    
    
    uint16_t sport, dport;
    unsigned char bits;
    unsigned seqn, ackn;
    unsigned int limit = 1;
//    bool stop = false;
    if (Args(conf, this, errh)
        .read_mp("SRC", saddr)
        .read_mp("SPORT", IPPortArg(IP_PROTO_TCP), sport)
        .read_mp("DST", AnyArg(), daddr_str)
        .read_mp("DPORT", IPPortArg(IP_PROTO_TCP), dport)
        .read_mp("SEQNO", seqn)
        .read_mp("ACKNO", ackn)
        .read_mp("FLAGS", bits)
  //      .read_p("COUNT", limit)
//        .read_p("STOP", stop)
        .complete() < 0)
        return -1;


    if (daddr_str.equals("DST_ANNO", 8)) {
	_daddr = IPAddress();
	_use_dst_anno = true;
    } else if (IPAddressArg().parse(daddr_str, _daddr, this))
	_use_dst_anno = false;
    else
	return errh->error("bad DST");


   _saddr = saddr;
   _sport = htons(sport);
   _dport = htons(dport);
  // _cksum = cksum;
   _seqn = seqn;
   _ackn = ackn;
   _bits = bits;

    if (daddr_str.equals("DST_ANNO", 8)) {
	_daddr = IPAddress();
	_use_dst_anno = true;
    } else if (IPAddressArg().parse(daddr_str, _daddr, this))
	_use_dst_anno = false;
    else
	return errh->error("bad DST");
/*
  if (limit > 0) {
    Packet *p = me->make_packet(saddr, daddr, sport, dport, seqn, ackn, bits);
    for (unsigned int i = 0; i < limit; i++)
      me->output(0).push(i + 1 < limit ? p->clone() : p);
  }
  if (stop)
    me->router()->please_stop_driver();*/
  return 0;
}


Packet *
TCPHead::simple_action(Packet *p_in)
{
  WritablePacket *q = p_in->push(sizeof(click_tcp) + sizeof(click_ip));

  //click_chatter("TCP header length:  %d",sizeof(click_tcp) );


  click_ip *ip = reinterpret_cast<click_ip *>(q->data());
  click_tcp *tcp = reinterpret_cast<click_tcp *>(ip + 1);
  #if !HAVE_INDIFFERENT_ALIGNMENT
      assert((uintptr_t)ip % 4 == 0);
  #endif

  // set up IP header
  ip->ip_v = 4;
  ip->ip_hl = sizeof(click_ip) >> 2;
  ip->ip_len = htons(q->length());
  ip->ip_id = htons(_id.fetch_and_add(1));
  ip->ip_p = IP_PROTO_TCP;
  ip->ip_src = _saddr;
  if (_use_dst_anno)
      ip->ip_dst = q->dst_ip_anno();
  else {
      ip->ip_dst = _daddr;
      q->set_dst_ip_anno(IPAddress(_daddr));
  }
  ip->ip_tos = 0;
  ip->ip_off = 0;
  ip->ip_ttl = 250;

  ip->ip_sum = 0;
#if HAVE_FAST_CHECKSUM && FAST_CHECKSUM_ALIGNED
  if (_aligned)
    ip->ip_sum = ip_fast_csum((unsigned char *)ip, sizeof(click_ip) >> 2);
  else
    ip->ip_sum = click_in_cksum((unsigned char *)ip, sizeof(click_ip));
#elif HAVE_FAST_CHECKSUM
  ip->ip_sum = ip_fast_csum((unsigned char *)ip, sizeof(click_ip) >> 2);
#else
  ip->ip_sum = click_in_cksum((unsigned char *)ip, sizeof(click_ip));
#endif

  q->set_ip_header(ip, sizeof(click_ip));

  // TCP fields
  memcpy((void *) &(tcp->th_sport), (void *) &_sport, sizeof(_sport));
  memcpy((void *) &(tcp->th_dport), (void *) &_dport, sizeof(_dport));
  tcp->th_seq = htonl(_seqn);
  tcp->th_ack = htonl(_ackn);
  tcp->th_off = 5;
  tcp->th_flags = _bits;
  tcp->th_win = htons(32120);
  tcp->th_sum = htons(0);
  tcp->th_urp = htons(0);


  uint16_t len = q->length() - sizeof(click_ip) - sizeof(click_tcp);
  _seqn += len;
  // now calculate tcp header cksum
  unsigned csum = click_in_cksum((unsigned char *)tcp, sizeof(click_tcp));
  tcp->th_sum = click_in_cksum_pseudohdr(csum, ip, sizeof(click_tcp));

  return q;
}

//

String TCPHead::read_handler(Element *e, void *thunk)
{
    TCPHead *u = static_cast<TCPHead *>(e);
    switch ((uintptr_t) thunk) {
      case 0:
        return IPAddress(u->_saddr).unparse();
      case 1:
        return String(ntohs(u->_sport));
      case 2:
        return IPAddress(u->_daddr).unparse();
      case 3:
        return String(ntohs(u->_dport));
      default:
        return String();
    }
}
//
void
TCPHead::add_handlers()
{
    add_read_handler("src", read_handler, 0);
    add_write_handler("src", reconfigure_keyword_handler, "0 SRC");
    add_read_handler("sport", read_handler, 1);
    add_write_handler("sport", reconfigure_keyword_handler, "1 SPORT");
    add_read_handler("dst", read_handler, 2);
    add_write_handler("dst", reconfigure_keyword_handler, "2 DST");
    add_read_handler("dport", read_handler, 3);
    add_write_handler("dport", reconfigure_keyword_handler, "3 DPORT");
}

CLICK_ENDDECLS
EXPORT_ELEMENT(TCPHead)
