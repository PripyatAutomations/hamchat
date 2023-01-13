#include "hamchat.h"

//
// Set or clear an away message.
// When users are away, RF auto-replies are disabled
//
bool m_away(Client *cptr, int argc, char **argv) {
   // De-allocate any existing away message
   if (cptr->away_msg != NULL) {
      free(cptr->away_msg);
      cptr->away_msg = NULL;
      cptr->away_since = 0;
    }

   char *target = argv[2];
   // Are we setting away?

   if (argc >= 1) {
      // determine the length
      size_t away_sz = strlen(target);

      // truncate to something sane
      if (away_sz > AWAY_LEN)
         away_sz = AWAY_LEN;

      // set away time
      cptr->away_since = now;

      // allocate a buffer
      cptr->away_msg = (char *)malloc(away_sz);
      // clear it out
      memset(cptr->away_msg, 0, away_sz);
      // and copy in the away message
      memcpy(cptr->away_msg, target, away_sz);
      cptr->Send(":%s 306 %s :You have been marked as being away", cfg->Get("core.servername", "hamchat.local"), cptr->callsign);
   } else { // clearing away
      cptr->Send("%s 305 %s :You are no longer marked as being away.", cfg->Get("core.servername", "hamchat.local"), cptr->callsign);
   }
   return true;
}
