#include "hamchat.h"

bool m_whois(Client *cptr, int argc, char **argv) {
   char *target = NULL, *nick = NULL;

   if (argc >= 3) {
//      target = argv[2];
      nick = argv[3];
   } else {
      nick = argv[2];
   }

   if (nick == NULL) {
      cptr->Send(":%s 431 %s :No callsign given", cfg->Get("core.servername", "hamchat.local"), cptr->callsign);
      return false;
   }

   Client *acptr = find_client(nick);

   // if no such client found
   if (acptr == NULL) {
      // XXX: Implement WHOWAS from the database
      cptr->Send(":%s 401 %s %s :Callsign is not currently reachable", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, nick);
      return false;
   }

   // Start of WHOIS reply
   cptr->Send(":%s 311 %s %s %s %s * :%s", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, acptr->callsign, acptr->username, acptr->hostname,  acptr->realname);
   cptr->Send(":%s 312 %s %s %s.hamchat :%s via hamchat", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, acptr->callsign, acptr->callsign, acptr->callsign);

   if (acptr->is_oper) {
      cptr->Send(":%s 313 %s %s :Is an IRC operator", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, acptr->callsign);
   }

   time_t idle_time = 0;
   if (acptr->last_msg > 0 && (now - acptr->last_msg > IDLE_AFTER)) {
      idle_time = now - acptr->last_msg;
   } else {
      idle_time = 0;
   }
   cptr->Send(":%s 317 %s %s %lu %lu :seconds idle, signon time", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, acptr->callsign, idle_time, acptr->connected);

// ...each channel acptr is in
//   cptr->Send(":%s 319 %s %s :%s", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, acptr->callsign, whois_chan_buf);
// RPL_WHOISSPECIAL
//   cptr->Send(":%s 320 %s %s :Connected via %s <%s>", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, acptr->callsign);

   if (cptr->conn_state & IRC_STATE_PASS) {
      cptr->Send(":%s 330 %s %s %s :is logged in as", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, acptr->callsign, acptr->callsign);
      cptr->Send(":%s 307 %s %s :has logged in for this account", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, acptr->callsign);
   }

   // End of WHOIS reply
   cptr->Send(":%s 318 %s %s :End of /WHOIS", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, acptr->callsign);
   return true;
}
