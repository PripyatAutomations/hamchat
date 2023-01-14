#include "hamchat.h"

bool m_stats(Client *cptr, int argc, char **argv) {
   if (!cptr->is_oper) {
      cptr->Send(":%s 481 %s :Permission denied, you are not an IRC operator.", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign());
      return false;
   }

   Log->Send(LOG_CRIT, "Got STATS %s from %s", argv[1], cptr->GetCallsign());
   return true;
}
