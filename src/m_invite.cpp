#include "hamchat.h"

//
// Allow inviting users into a channel, this is probably incomplete but i wanted to get the numerics in here...
bool m_invite(Client *cptr, int argc, char **argv) {
   char *invitee = argv[2];
   char *chname = argv[3];

   // find the channel
   Channel *chptr = find_channel(chname);
   if (chptr == NULL) {
      cptr->Send(":%s 403 %s %s :No such channel", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, chname);
      return false;
   }

   // is sender on channel?
   if (!chptr->IsMember(cptr)) {
      cptr->Send(":%s 442 %s %s :You're not on that channel", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, chname);
      return false;
   }

   // figure out if this sender is an op...
   if (!chptr->IsOp(cptr)) {
      cptr->Send(":%s 482 %s %s :You're not channel operator", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, chname);
      return false;
   }

   // find the invitee
   Client *iptr = find_client(invitee);
   if (iptr == NULL) {
      // user doesn't exist
      cptr->Send("");
      return false;
   }

   // is user already on channel?
   if (chptr->IsMember(iptr)) {
      cptr->Send(":%s 443 %s %s %s", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, iptr->callsign, chname);
      return false;
   }

   // tell the sender the invite is being sent
   cptr->Send(":%s 341 %s %s %s", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, invitee, chname);

   // tell the invitee about this whole ordeal
   iptr->Send(":%s INVITE %s %s", cptr->callsign, iptr->callsign, chname);
   return true;
}
