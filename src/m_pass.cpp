#include "hamchat.h"

bool m_pass(Client *cptr, int argc, char **argv) {
   char *chpass = argv[2];
   size_t chpass_sz = -1;

   if (cptr->conn_state & IRC_STATE_PASS) {
      cptr->Send(":%s 462 %s :You may not reregister", cfg->Get("core.servername", "hamchat.local"), cptr->callsign);
      return false;
   }

   if (chpass == NULL) {
      cptr->Send(":%s 461 %s PASS :Not enough parameters", cfg->Get("core.servername", "hamchat.local"), cptr->callsign);
      return false;
   }

   chpass_sz = strlen(chpass);
   if (chpass_sz > PASS_LEN)
      chpass_sz = PASS_LEN;

   // Store the password in the user struct, until we have a NICK to use with it
   memset(cptr->pass, 0, PASS_LEN);
   memcpy(cptr->pass, chpass, chpass_sz);
   cptr->conn_state |= IRC_STATE_PASS;
   return false;
}
