#include "hamchat.h"

//
// Support for 'bot' commands in some channels
//

// Suported channels:
//
//	<CALLSIGN>-radio*	Radio control channels


struct bot_cmds {
   char name[16];
   bool (*bot_cmd)(Client *cptr, const char *buf),
   const char *help;
};
typedef struct bot_cmds bot_cmds_t;

bool bot_freq(Client *cptr, const char *buf) {
    return false;
}

bool bot_power(Client *cptr, const char *buf) {
    return false;
}

bool bot_spots(Client *cptr, const char *buf) {
    return false;
}

bool bot_tx(Client *cptr, const char *buf) {
    return false;
}

bool bot_vfo(Client *cptr, const char *buf) {
    return false;
}

bot_cmds_t bot_cmds[] = {
   { "!freq",   bot_freq,  "Set frequency !freq <rig> [freq] | Display frequency !freq [rig]" },
   { "!mode",   bot_mode,  "Set modulation mode !mode <rig> [mode] | Display mode !freq [rig]" },
   { "!power",  bot_power, "Set power !power <rig> [power] | Display power !power [rig]" },
   { "!spots",	bot_spots, "Show recent spots: !spots [callsign] [number]" },
   { "!tx",     bot_tx,    "Transmit a raw message (FT4/FT8/etc)" },
   { "!vfo",    bot_vfo,   "Switch active VFO !vfo [vfo] | Display active VFO !vfo" },
   { NULL, 	NULL,	   NULL }
};
