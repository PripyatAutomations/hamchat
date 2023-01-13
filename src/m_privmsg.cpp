#include "hamchat.h"


bool do_privmsg(Client *cptr, int argc, char **argv, bool is_notice) {
    char *target = argv[2];
    char *msg = argv[3];

    if (target == NULL) {
       cptr->Send(":%s 411 %s :No recipient given", cfg->Get("core.servername", "hamchat.local"), cptr->callsign);
       return false;
    }

    if (msg == NULL) {
       cptr->Send(":%s 412 %s :No text to send", cfg->Get("core.servername", "hamchat.local"), cptr->callsign);
       return false;
    }

    if (target[0] == '#') {  // it's for a channel...
        // XXX: do channel message
        Channel *chptr = find_channel(target);
        bool muted = false;
        bool is_member = true;

        // XXX: check for bans

        // check if user is a member of the channel
        if (chptr->IsMember(cptr))
           is_member = true;

        // if no such channel, muted, or not a member, error
        if (chptr == NULL || muted || !is_member) {
           cptr->Send(":%s 404 %s %s :Cannot send to channel", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, target);
           return false;
        }

        // if we made it this far, send the message
        chptr->SendToAll(":%s!%s@%s %s %s :%s", cptr->callsign, cptr->username, cptr->hostname, (is_notice ? "NOTICE" : "PRIVMSG"), target, msg);
    } else {  // it's for a user
        // destination client (if a user)
        Client *dcptr = NULL;
        
        // couldn't find the target
        if ((dcptr = find_client(target)) == NULL) {
           cptr->Send(":%s 401 %s %s :No such callsign", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, target);
           return false;
        }

        // send away message, if it's to a person
        if (dcptr->away_msg != NULL) {
           cptr->Send(":%s 301 %s %s :%s", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, dcptr->callsign, dcptr->away_msg);
        }

        dcptr->Send(":%s!%s@%s %s %s :%s", cptr->callsign, cptr->username, cptr->hostname, (is_notice ? "NOTICE" : "PRIVMSG"), dcptr->callsign, argv[3]);
    }
    cptr->last_msg = now;
    return true;
}

bool m_privmsg(Client *cptr, int argc, char **argv) {
    return do_privmsg(cptr, argc, argv, false);
}
