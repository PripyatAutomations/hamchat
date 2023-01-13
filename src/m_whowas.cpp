#include "hamchat.h"

bool m_whowas(Client *cptr, int argc, char **argv) {
   // only opers can see WHOWAS
   if (!cptr->is_oper) {
      cptr->Send(":%s 481 %s :Permission denied, you are not an IRC operator.", cfg->Get("core.servername", "hamchat.local"), cptr->callsign);
      return false;
   }

   return false;
}
