#include "hamchat.h"

//
// Here we define the "simple" wire protocol
//
// Messages will be encoded into this format for transports
// supporting "simple" protocol.
//
// Here we produce a simple framed protocol intended to be compatible
// with both multicast (FEC) and connected (ARQ) modes.


bool w_simple_encode_away(Client *cptr, int argc, char **argv) {
   return false;
}

bool w_simple_encode_invite(Client *cptr, int argc, char **argv) {
   return false;
}

bool w_simple_encode_join(Client *cptr, int argc, char **argv) {
   return false;
}

bool w_simple_encode_kick(Client *cptr, int argc, char **argv) {
   return false;
}

bool w_simple_encode_mode(Client *cptr, int argc, char **argv) {
   return false;
}

bool w_simple_encode_nick(Client *cptr, int argc, char **argv) {
   return false;
}

bool w_simple_encode_notice(Client *cptr, int argc, char **argv) {
   return false;
}

bool w_simple_encode_part(Client *cptr, int argc, char **argv) {
   return false;
}

bool w_simple_encode_privmsg(Client *cptr, int argc, char **argv) {
   return false;
}

bool w_simple_encode_quit(Client *cptr, int argc, char **argv) {
   return false;
}

bool w_simple_encode_time(Client *cptr, int argc, char **argv) {
   return false;
}

bool w_simple_encode_topic(Client *cptr, int argc, char **argv) {
   return false;
}

bool w_simple_encode_version(Client *cptr, int argc, char **argv) {
   return false;
}

// These are the only IRC messages that will be propagated
// to other instances...
struct wireproto_cmds proto_cmds_simple[] = {
   { "AWAY", 	w_simple_encode_away },
   { "INVITE",	w_simple_encode_invite },
   { "JOIN",	w_simple_encode_join },
   { "KICK",	w_simple_encode_kick },
   { "MODE",	w_simple_encode_mode },
   { "NICK",	w_simple_encode_nick },
   { "NOTICE",	w_simple_encode_notice },
   { "PART",	w_simple_encode_part },
   { "PRIVMSG",	w_simple_encode_privmsg },
   { "QUIT",	w_simple_encode_quit },
   { "TIME",	w_simple_encode_time },
   { "TOPIC",	w_simple_encode_topic },
   { "VERSION",	w_simple_encode_version },
   { NULL, 	NULL }
};
