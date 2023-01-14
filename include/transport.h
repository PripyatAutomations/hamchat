/*
 * Transport layer:
 *	Here we try to blur the difference between a socket/modem/other similar connection.
 *
 * This could be used for the data stream to/from a soft modem, while handling control elsewhere.
 */
#if	!defined(_transport_h)
#define	_transport_h

#include "transport_socket.h"
#include "transport_modem.h"

enum transport_type {
   TRANSPORT_NONE = 0,
   TRANSPORT_SOCKET,
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
