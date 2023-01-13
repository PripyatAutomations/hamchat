#include "hamchat.h"

bool m_time(Client *cptr, int argc, char **argv) {
   char datebuf[32];
   memset(datebuf, 0, sizeof(datebuf));
   strftime(datebuf, sizeof(datebuf) - 1, "%Y-%m-%d %H:%M:%S", localtime(&now));
   cptr->Send(":%s 391 %s %s %lu 0 :%s UTC", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, cfg->Get("core.servername", "hamchat.local"), now, datebuf);
   return true;
}
