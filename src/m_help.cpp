#include "hamchat.h"

//
// Some day this will display help for the various commands supported
//
// For now it does nothing
bool m_help(Client *cptr, int argc, char **argv) {
   cptr->Send(":%s NOTICE %s :*** HELP is not currently implemented. Sorry! Try #hamchat for help!", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign());
   return true;
}
