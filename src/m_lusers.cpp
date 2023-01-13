#include "hamchat.h"

bool m_lusers(Client *cptr, int argc, char **argv) {
   cptr->Send(":%s 251 %s :There are %d users and 0 invisible on %d servers", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, stats.curr_clients, stats.curr_servers);
   cptr->Send(":%s 252 %s %d :operator(s) online", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, stats.curr_opers);
//   cptr->Send(":%s 253 %s %d :unknown connections", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, count_all_unknown());
//   cptr->Send(":%s 254 %s %d :channels formed", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, count_all_channels());
   cptr->Send(":%s 255 %s :I have %d clients and %d servers", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, stats.curr_clients, stats.curr_servers);

   return false;
}