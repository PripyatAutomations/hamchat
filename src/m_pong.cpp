#include "hamchat.h"

bool m_pong(Client *cptr, int argc, char **argv) {
   if (cptr->last_ping) {
      // XXX: Compare the ping and make sure it's valid, for now just accept it

      // clear the pending ping timeout and acknowledge it
      cptr->last_ping = now;
      cptr->got_pong = true;

      return true;
   }

   return false;
}
