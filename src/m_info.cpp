#include "hamchat.h"

//
// Describe the server and it's software to the user
//
bool m_info(Client *cptr, int argc, char **argv) {
    cptr->Send(":%s 371 %s :Your host is %s running version %s of hamchat", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign(), cfg->Get("core.servername", "hamchat.local"), VERSION);
    cptr->Send(":%s 371 %s :This software was developed mostly from scratch by PripyatAutomations. It would not be possible without the many who came before us!", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign());
    cptr->Send(":%s 371 %s :The following projects saved countless hours of development and have my deepest thanks!", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign());
    cptr->Send(":%s 371 %s :gdb - The GNU Debugger - https://sourceware.org/gdb/", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign());
    cptr->Send(":%s 371 %s :hamlib - A library and daemon for controlling amateur radio transceivers. - https://hamlib.github.io/", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign());
    cptr->Send(":%s 371 %s :libev - A lightweight asynchronous event loop that lets us worry about the data and not the sockets! - https://github.com/enki/libev", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign());
    cptr->Send(":%s 371 %s :sqlite3 - A lightweight database who performs well. Used for storing routing data, channels (config, logs, etc), user data, and other bits - https://www.sqlite.org", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign());
    cptr->Send(":%s 374 %s :End of INFO list", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign());
    return false;
}
