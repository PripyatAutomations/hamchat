#include "hamchat.h"

bool m_kill(Client *cptr, int argc, char **argv) {
   char *callsign = argv[3];
   char *reason = argv[4];

   if (callsign == NULL || reason == NULL) {
      cptr->Send(":%s 461 %s KILL :Not enough parameters", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign());
   }

   if (!IRC_IsActive(cptr) || !cptr->is_oper) {
      cptr->Send(":%s 481 %s :Permission denied, you are not an IRC operator.", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign());
      Log->Send(LOG_WARNING, "User <%s> tried to /KILL user <%s>, but they are not operator!", cptr->GetCallsign(), callsign);
      return false;
   }

   // Find the client
   Client *target = find_client(callsign);

   if (target == NULL) {
      cptr->Send(":%s 401 %s %s :No such nickname", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign(), callsign);
      return false;
   }

   // Send the KILL to all common users
   cptr->SendToCommonChannels(":%s QUIT :Killed (%s: %s)", target->GetCallsign(), cptr->GetCallsign(), reason);

   // Let the user know they've been killed   
   cptr->Send(":%s KILL %s :%s", cptr->GetCallsign(), target->GetCallsign(), reason);

   // XXX: Send WALLOPS
   Log->Send(LOG_WARNING, "User <%s> /KILLed user <%s> with reason: %s", cptr->GetCallsign(), callsign, reason);

   // Get rid of the client
   delete target;
   return true;
}
