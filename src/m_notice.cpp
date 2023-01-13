#include "hamchat.h"
#include "hamchat.h"

// from src/m_privmsg.h
extern bool do_privmsg(Client *cptr, int argc, char **argv, bool is_notice);

bool m_notice(Client *cptr, int argc, char **argv) {
    return do_privmsg(cptr, argc, argv, true);
}
