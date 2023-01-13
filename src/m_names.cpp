#include "hamchat.h"

bool m_names(Client *cptr, int argc, char **argv) {
   char *target = argv[2];

   // get rid of invalid requrests
   if (target == NULL || target[0] != '#') {
      cptr->Send(":%s 366 %s %s :End of /NAMES list", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, target);
      return false;
   }
   

   Channel *chptr = find_channel(target);

   if (chptr == NULL) {
      cptr->Send(":%s 366 %s %s :End of /NAMES list", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, target);
      return false;
   }

   llist_t *cm = chptr->members;

   do {
     if (cm == NULL)
        break;

     char privacy = '=';		// public channel
     Client *mcptr = (Client *)cm->ptr;

     if (mcptr == NULL)
        break;

     cptr->Send(":%s 353 %s %c %s :%s", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, privacy, target, mcptr->callsign);
     cm = cm->next;
   } while(cm != NULL);

   cptr->Send(":%s 366 %s %s :End of /NAMES list", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, target);
   return true;
}
