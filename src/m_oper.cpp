#include "hamchat.h"

bool m_oper(Client *cptr, int argc, char **argv) {
   cptr->Send(":%s 481 %s :Permission denied, you are not an IRC operator.", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign());
   return false;
}
