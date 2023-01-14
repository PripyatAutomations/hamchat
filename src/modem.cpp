//#include "hamchat.h"
//////////////////////////////////////////////////////////////////////////////
// Here we provide the thread for interfacing with the various soft modems  //
//                                                                          //
// This lets us ensure timing is reliable, in that we won't get momentarily //
// blocked or delayed by less important things like IRC traffics            //
//////////////////////////////////////////////////////////////////////////////
#include <limits.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdint.h>
#include <inttypes.h>
#include "hamchat.h"
static pthread_t modem_thread;
static bool modem_running = false;

///////////////////////////////
// Table of available modems //
///////////////////////////////
// { "ardop", modem_ardop },
// { "bell.103", modem_bell103 },
// { "bell.202", modem_bell202 },
struct modem_type_names modem_drivers[] = {
   { MODEM_DRIVER_NONE,		"NONE" },
   { MODEM_DRIVER_ARDOP,	"ardop" },
   { MODEM_DRIVER_BELL103,	"bell.103" },
   { MODEM_DRIVER_BELL202,	"bell.202" },
   { MODEM_DRIVER_PSK31,	"psk31" }
};

/////////////////////
// Our main thread //
/////////////////////
static void *modem_thread_main(void *arg) {
   Log->Send(LOG_DEBUG, "Greetings from modem_thread!");
   // XXX: Do stuff
   while (true) {
      sleep(30);
   }
}

//////////////////////////////////////////
// Start up our modem thread, if needed //
//////////////////////////////////////////
bool init_modem_thread(void) {
   void *arg = NULL;

   if (!modem_running) {
      pthread_create(&modem_thread, NULL, modem_thread_main, arg);
      modem_running = true;
   } else {
      Log->Send(LOG_WARNING, "modem is already running, not restarting it.");
      return false;
   }

   Log->Send(LOG_INFO, "Succesfully started modem interface thread");
   return true;
}

Modem::Modem(modem_type_t type) {
   this->modem_type = type;

   if (type == MODEM_DRIVER_ARDOP) {
      this->ardop = new Modem_ARDOP();
   }
}

Modem::~Modem() {
}
