#include "hamchat.h"

bool m_part(Client *cptr, int argc, char **argv) {
   char *chnames = argv[2];
   char *part_msg = argv[3];

   if (argc < 2) {
      return false;
   }

   char *p;
   int i = 0;

   // split up the channels
   int chan_count = 0;
   char chans[JOIN_MAX_CHAN][CHAN_LEN];
   memset(chans, 0, sizeof(chans));
   p = chnames;

   while (p != NULL) {
      if (*p != ',') {
         chans[chan_count][i] = *p;
      } else if (*p == ',') {
         chan_count++;
         i = 0;
      }
      
      i++; p++;
   }

   // try to join channels with the keys given
   for (i = 0; i < JOIN_MAX_CHAN; i++) {
      char *chname = chans[i];

      // find the channel, if it exists
      Channel *chptr = find_channel(chname);

      if (chptr == NULL) {
         // no such channel
         Log->Send(LOG_DEBUG, "<%s> sent PART for channel they aren't in (%s)", cptr->GetCallsign(), chname);
      } else {
         return chptr->RemoveUser(cptr, part_msg);
      }
   }
   return false;
}
