#include "hamchat.h"

bool m_quit(Client *cptr, int argc, char **argv) {
   char *reason = argv[1];

   if (IRC_IsActive(cptr)) {
      // remove from channels with quit message
   }

   // delete the Client
   Log->Send(LOG_INFO, "<%d> Client %s QUIT", cptr->sock->fd, cptr->GetCallsign());
   delete cptr;
   return true;
}
