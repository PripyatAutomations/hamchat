#if	!defined(_transport_h)
#define	_transport_h

#include "transport_ip.h"
#include "transport_modem.h"

enum transport_type {
   TRANSPORT_NONE = 0,
   TRANSPORT_AX25,
   TRANSPORT_IP,
   TRANSPORT_KISS,
   TRANSPORT_MODEM
};

// This defines the connection to transport and presents things as a simple in/out data stream
class Transport {
   private:
      Socket *socket;
      Listener *listener;
      Transport_Modem *transport_modem;
   public:
      enum transport_type transport_type;
      Transport(enum transport_type transport_type);
      ~Transport();
};

#endif	// !defined(_transport_h)
