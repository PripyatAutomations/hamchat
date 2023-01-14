#include "hamchat.h"

bool m_die(Client *cptr, int argc, char **argv) {
   if (!cptr->is_oper) {
      cptr->Send(":%s 481 %s :Permission denied, you are not an IRC operator.", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign());
      return false;
   }

   Log->Send(LOG_CRIT, "Got shutdown from %s, goodbye!", cptr->GetCallsign());
   shutdown();
   return true;
}
