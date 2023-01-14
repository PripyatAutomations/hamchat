#include "hamchat.h"

bool m_names(Client *cptr, int argc, char **argv) {
   char *target = argv[2];

   // get rid of invalid requrests
   if (target == NULL || target[0] != '#') {
      cptr->Send(":%s 366 %s %s :End of /NAMES list", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign(), target);
      return false;
   }
   

   Channel *chptr = find_channel(target);

   if (chptr == NULL) {
      cptr->Send(":%s 366 %s %s :End of /NAMES list", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign(), target);
      return false;
   }

   // walk the channels dict and find the desired channel
   const char *key;
   const void *val;
   int    rank = 0;
   time_t ts = 0;

   if (chptr->members == NULL)
      return false;

   while (1) {
      rank = dict_enumerate_blob(chptr->members, rank, &key, &val, &ts);

      if (rank < 0 || val == NULL)
         break;

      // these are =*@
      char privacy = '=';
      irc_channel_member *cm = (irc_channel_member *)val;
      if (cm == NULL) {
         Log->Send(LOG_CRIT, "m_names: cm == NULL for %s", chptr->name);
         break;
      }

      Client *mcptr = cm->cptr;
      if (mcptr == NULL) {
         Log->Send(LOG_CRIT, "m_names: mcptr == NULL for %s", chptr->name);
         break;
      }
      cptr->Send(":%s 353 %s %c %s :%s", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign(), privacy, target, mcptr->GetCallsign());
   }

   cptr->Send(":%s 366 %s %s :End of /NAMES list", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign(), target);
   return true;
}
