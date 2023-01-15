#include "hamchat.h"

//
// Support for 'bot' commands in some channels
//

// Suported channels:
//
//	<CALLSIGN>-radio*	Radio control channels


struct bot_cmds {
   char name[16];
   bool (*bot_cmd)(Client *cptr, const char *buf);
   const char *help;
};
typedef struct bot_cmds bot_cmds_t;

// This is ugly since we cannot modify the input strings....
bool bot_freq(Client *cptr, const char *buf) {
    Rig *rig = NULL;
    if (buf == NULL) {
       // setup for readback frequency of rig 0
       rig = rigs[0];
    } else {
       const char *arg1 = NULL, *arg2 = NULL;
       arg1 = buf;
       arg2 = strchr(arg2, ' ');
       if (arg1 == NULL) {
          // well that's not teh answer...
       }
       size_t arg1_len = arg2 - arg1;

       char tmpbuf[32];
       memset(tmpbuf, 0, 32);

       if (arg1_len > 0 && arg1_len < 32) {
          memcpy(tmpbuf, buf, arg1_len);
       }
       // it's actually next character
       arg2++;
       
    }
    return false;
}

bool bot_mode(Client *cptr, const char *buf) {
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
