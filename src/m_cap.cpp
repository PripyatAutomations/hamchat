#include "hamchat.h"

bool m_cap(Client *cptr, int argc, char **argv) {
#if	0
   if (IRC_IsActive(cptr) && IRC_HasCap(cptr)) {
      cptr->Send(":%s ERROR :Already got CAP", cfg->Get("core.servername", "hamchat.local"));
      return false;
   } else {
      Log->Debug("got CAP %s from %d", argv[2], cptr->sock->fd);
      if ((strncasecmp(argv[2], "LS", 2) == 0) ||
          (strncasecmp(argv[2], "REQ", 3) == 0)) {
         cptr->conn_state |= IRC_STATE_CAP;
      } else if ((strncasecmp(argv[2], "END", 3) == 0)) {
         cptr->conn_state |= IRC_STATE_CAP_END;
      }
   }

   Log->Debug("client on %d sent CAP %s", cptr->sock->fd, argv[2]);
   cptr->Send(":%s CAP * LS :", cfg->Get("core.servername", "hamchat.local"));
#endif
   cptr->Send(":%s 421 %s CAP :Unknown command", cfg->Get("core.servername", "hamchat.local"), (*cptr->callsign != '\0' ? cptr->callsign : "*"));
   return true;
}
