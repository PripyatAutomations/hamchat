#include "hamchat.h"

bool m_unknown(Client *cptr, int argc, char **argv) {
   if (cptr != NULL) {
      // XXX: use the correct numeric here
      cptr->Send(":%s 421 %s %s :Invalid command", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign(), argv[1]);
   }
   Log->Send(LOG_DEBUG, "unknown command from %d: %s", cptr->sock->fd, argv[1]);
   return false;
}
