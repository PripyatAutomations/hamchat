#include "hamchat.h"

bool m_version(Client *cptr, int argc, char **argv) {
   cptr->Send(":%s 351 %s hamchat-%s %s :e1", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, VERSION, cfg->Get("core.servername", "hamchat.local"));
   return true;
}
