#include "hamchat.h"

bool m_ping(Client *cptr, int argc, char **argv) {
   if (argc >= 1) {
      cptr->Send(":%s PONG :%s", cfg->Get("core.servername", "hamchat.local"), argv[2]);
//      Log->Debug("got ping from client on fd %d <%s> reply sent!", cptr->sock->fd, argv[2]);
      return true;
   }

   return false;
}
