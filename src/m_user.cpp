#include "hamchat.h"

bool m_user(Client *cptr, int argc, char **argv) {
   char *username = argv[2];
   char *realname = argv[5];

   // client has already sent USER command
   if (cptr->conn_state & IRC_STATE_USER) {
      // Command is invalid from a logged in user
      cptr->Send(":%s 462 %s :You may not reregister", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign());
      return false;
   }

   // validate the USER info...
   if (username == NULL || realname == NULL) {
      Log->Send(LOG_WARNING, "<%d> has bogus username '%s' or realname '%s'", (username ? username : "NULL"), (realname ? realname : "NULL"));
   }

   // new user, needs USER state flag added
   cptr->conn_state |= IRC_STATE_USER;

   // username
   memset(cptr->username, 0, sizeof(cptr->username));
   snprintf(cptr->username, sizeof(cptr->username), "%s", username);

   // realname
   memset(cptr->realname, 0, sizeof(cptr->realname));
   snprintf(cptr->realname, sizeof(cptr->realname), "%s", realname);

   // if client has sent NICK already, go ahead and send numerics
   if (cptr->conn_state & IRC_STATE_NICK) {
     Log->Send(LOG_DEBUG, "<%d> got USER after NICK, proceeding", cptr->sock->fd);
     cptr->send_connect_numerics();
   } else {
     Log->Send(LOG_DEBUG, "<%d> got USER, waiting for NICK", cptr->sock->fd);
   }

   // if we mdae i
   Log->Send(LOG_DEBUG, "<%d> client set USER=%s RealName=%s", cptr->sock->fd, cptr->username, cptr->realname);

   return true;
}
