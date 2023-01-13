#include "hamchat.h"

//
// Support for controlling and seeing status of the radios in an irc channel, per radio
//
// Channel names should be of the sort #callsign-radioX such as #N0CALL-radio0 OR #N0CALL-radio1
//
// Rules:
//	Rig channel MUST be created +mM (permanently moderated)
//	Only channel creator should be able to give ops/voice to users
//	No-one can deop/devoice the channel creator
//	Opped users can control all settings, voiced may only cause TX on set frequency.
//	Users should be automatically joined into channels for all THEIR radios.
