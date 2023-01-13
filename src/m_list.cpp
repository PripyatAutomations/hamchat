#include "hamchat.h"

bool m_list(Client *cptr, int argc, char **argv) {
   if (!IRC_IsActive(cptr)) {
      cptr->Send(":%s 351 %s hamchat-%s %s :e1", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, VERSION, cfg->Get("core.servername", "hamchat.local"));
      return true;
   }
   return false;
}
