#if	!defined(_packet_h)
#define	_packet_h

struct __attribute__ ( (__packed__)) pkt_hdr {
   unsigned short length;
   unsigned short seqno;
   unsigned short src,
                dst;
};

struct __attribute__ ( (__packed__)) Packet {
   // header
   struct pkt_hdr hdr;

   // payload
   void *data;		// pointer to payload

   // checksum
   unsigned short sum;
}; 

#endif	// !defined(_packet_h)
