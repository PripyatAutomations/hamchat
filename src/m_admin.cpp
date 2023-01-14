#include "hamchat.h"

//
// Return admin info about the server
//
// For now, it must be edited in include/config.h
//

bool m_admin(Client *cptr, int argc, char **argv) {
   cptr->Send(":%s 256 %s %s :Administrative info", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign(), cfg->Get("core.servername", "hamchat.local"));
   cptr->Send(":%s 257 %s :%s", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign(), cfg->Get("admin.line1", ""));
   cptr->Send(":%s 258 %s :%s", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign(), cfg->Get("admin.line2", ""));
   cptr->Send(":%s 259 %s :%s", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign(), cfg->Get("admin.line3", ""));
   return true;
}
