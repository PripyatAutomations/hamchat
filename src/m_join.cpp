#include "hamchat.h"

bool m_join(Client *cptr, int argc, char **argv) {
   char *chnames = argv[2];
   char *chkeys = argv[3];

   if (argc < 1) {
      return false;
   }

   // empty.. go away irssi!
   if (strlen(chnames) <= 0) {
      return false;
   }
// For now, we do not support multiple joins, keys, etc
#if	0
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

   // split up the keys
   int key_count = 0;
   char keys[JOIN_MAX_CHAN][KEY_LEN];
   memset(keys, 0, sizeof(keys));
   p = chkeys;
   i = 0;

   while (p != NULL) {
      if (*p != ',') {
         keys[key_count][i] = *p;
      } else if (*p == ',') {
         key_count++;
         i = 0;
      }
      
      i++; p++;
   }

#endif

//   for (i = 0; i < JOIN_MAX_CHAN; i++) {
      char *chname = chnames;

      if (*chname == '\0') {
         Log->Debug("<%d> sent JOIN<NULL>...why?", cptr->sock->fd);
         cptr->Send(":%s NOTICE * :Why your client send JOIN :<NULL>??", cfg->Get("core.servername", "hamchat.local"));
         return false;
      }

      if (strlen(chname) == 1 && atoi(chname) == '0') {
         // XXX: Part all channels
         Log->Debug("<%d> requested JOIN 0 (leave all channels)", cptr->sock->fd);
         return false;
      }

      // find the channel, if it exists
      Channel *chptr = find_channel(chname);

      // If it doeson't exist
      if (chptr == NULL) {
         // try to make new channel, with no key
         if ((chptr = find_or_add_channel(cptr, chname)) == NULL) {
            Log->Crit("<%d> Couldn't create channel %s", cptr->sock->fd, chname);
            return false;
         }
      }

      if (*chptr->key != '\0') {
         // Validate the key, for now disallow entry with keys
//         if (strcasecmp(chptr->key, keys[i]) != 0) {
            cptr->Send(":%s 475 %s %s :Cannot join channel (+k)", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, chptr->name);
            return false;
//         }
      }

      // too many channels?
      if (count_user_channels(cptr) >= USER_MAX_CHAN) {
         cptr->Send(":%s 405 %s %s :You have joined too many channels", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, chptr->name);
         return false;
      }

      chptr->AddUser(cptr);

      // for now just break out of here, this is to simple life
//      break;
//   }
   return false;
}
