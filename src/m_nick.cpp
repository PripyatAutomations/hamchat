#include "hamchat.h"

bool m_nick(Client *cptr, int argc, char **argv) {
   char *new_call = argv[2];
   bool changing_nick = false;

   if (argc >= 1) {
      // Validate it meets callsign character limitations
      if (!is_valid_callsign(new_call)) {
         cptr->Send(":%s 432 %s %s :Invalid callsign format", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, new_call);
         return false;
      }

      // Before we do anything else, let's see if it's already in use...
      Client *owner = find_client(new_call);
      if (owner != NULL) {
         cptr->Send(":%s 433 %s %s :Callsign is already registered", cfg->Get("core.servername", "hamchat.local"), (*cptr->callsign  != '\0' ? cptr->callsign : "*") , new_call);
         return false;
      }

      // do they already have a callsign set?
      if (cptr->conn_state & (IRC_STATE_NICK|IRC_STATE_USER)) {
         Log->Debug("<%d> callsign change %s -> %s", cptr->sock->fd, cptr->callsign, new_call);
         changing_nick = true;
      } else {
         cptr->conn_state |= IRC_STATE_NICK;
         Log->Debug("<%d> client set their callsign to %s", cptr->sock->fd, new_call);
      }


      if (cptr->callsign[0] != '\0' && changing_nick) {
         cptr->SendToCommonChannels(":%s NICK %s", cptr->callsign, new_call);
         cptr->Send(":%s NICK %s", cptr->callsign, new_call);
      } else {	// no old callsign
         // did they become active user?
         if (cptr->conn_state & IRC_STATE_USER) {
           Log->Debug("<%d> got NICK after USER, proceeding", cptr->sock->fd);
           cptr->send_connect_numerics();
         } else {
           Log->Debug("<%d> got NICK, waiting for USER", cptr->sock->fd);
         }
     }
   } else {
      cptr->Send(":%s 431 %s :No callsign given", cfg->Get("core.servername", "hamchat.local"), cptr->callsign);
      return false;
   }      

   // copy it in place
   memset(cptr->callsign, 0, sizeof(cptr->callsign));
   snprintf(cptr->callsign, CALLSIGN_LEN, "%s", new_call);

   return true;
}
