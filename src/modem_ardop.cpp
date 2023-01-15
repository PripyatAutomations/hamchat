#include "hamchat.h"
#include <sys/wait.h>

//
// Transport via ardop soft modems
//
// This talks to the various C implementations of ardop via a socket
//
//
// We may have to change versions of this, if problems occur,
// but this is the most recent C ardop i could find.
//
// You make need to do some configuration to get pulseaudio/pipewire to play nice
// as it uses alsa.
//
// PTT is to be handled by rig_* modules
//

struct ardop_mode_names ardop_mode_names[] = {
  {  ARDOP_MODE_NONE,		   "ardop/node" },
  {  ARDOP_MODE_ARQ_200MAX,	   "ardop/arq:200max" },
  {  ARDOP_MODE_ARQ_500MAX,	   "ardop/arq:500max" },
  {  ARDOP_MODE_ARQ_1000MAX,	   "ardop/arq:1000max" },
  {  ARDOP_MODE_ARQ_2000MAX,	   "ardop/arq:2000max" },
  {  ARDOP_MODE_ARQ_500FORCED,	   "ardop/arq:500forced" },
  {  ARDOP_MODE_ARQ_1000FORCED,	   "ardop/arq:1000forced" },
  {  ARDOP_MODE_ARQ_2000FORCED,	   "ardop/arq:2000forced" },
  {  ARDOP_MODE_FEC_4FSK_200_50S,  "ardop/fec:4fsk_200_50s" },
  {  ARDOP_MODE_FEC_4FSK_200_50,   "ardop/fec:4fsk_200_50" },
  {  ARDOP_MODE_FEC_4PSK_200_100S, "ardop/fec:4psk_200_100s" },
  {  ARDOP_MODE_FEC_4PSK_200_100,  "ardop/fec:4psk_200_100" },
  {  ARDOP_MODE_FEC_8PSK_200_100,  "ardop/fec:8psk_200_100" },
  {  ARDOP_MODE_FEC_4FSK_500_100S, "ardop/fec:4fsk_500_100s" },
  {  ARDOP_MODE_FEC_4FSK_500_100,  "ardop/fec:4fsk_500_100" },
  {  ARDOP_MODE_FEC_4PSK_500_100,  "ardop/fec:4psk_500_100" },
  {  ARDOP_MODE_FEC_8PSK_500_100,  "ardop/fec:8psk_500_100" },
  {  ARDOP_MODE_FEC_4PSK_500_167,  "ardop/fec:4psk_500_167" },
  {  ARDOP_MODE_FEC_8PSK_500_167,  "ardop/fec:8psk_500_167" },
  {  ARDOP_MODE_FEC_4FSK_1000_100, "ardop/fec:4fsk_1000_100" },
  {  ARDOP_MODE_FEC_4PSK_1000_100, "ardop/fec:4psk_1000_100" },
  {  ARDOP_MODE_FEC_8PSK_1000_100, "ardop/fec:8psk_1000_100" },
  {  ARDOP_MODE_FEC_4PSK_1000_167, "ardop/fec:4psk_1000_167" },
  {  ARDOP_MODE_FEC_8PSK_1000_167, "ardop/fec:8psk_1000_167" },
  {  ARDOP_MODE_FEC_4FSK_2000_100, "ardop/fec:4fsk_2000_100" },
  {  ARDOP_MODE_FEC_4PSK_2000_100, "ardop/fec:4psk_2000_100" },
  {  ARDOP_MODE_FEC_8PSK_2000_100, "ardop/fec:8psk_2000_100" },
  {  ARDOP_MODE_FEC_4PSK_2000_167, "ardop/fec:4psk_2000_167" },
  {  ARDOP_MODE_FEC_8PSK_2000_167, "ardop/fec:8psk_2000_167" }
};

Modem_ARDOP::Modem_ARDOP() {
}

Modem_ARDOP::~Modem_ARDOP() {
}

bool Modem_ARDOP::Connect(void) {
//   fd_cmd = conn
   // XXX: These need to be read from config
   this->sock_cmd = new Socket("tcp://localhost:8515");
   this->sock_data = new Socket("tcp://localhost:8516");
   if (this->sock_cmd == NULL || this->sock_data == NULL) {
      Log->Send(LOG_CRIT, "Unable to open connection to ardop modem (cmd: %s, data: %s)", (this->port_cmd ? "true" : "false"), (this->port_data ? "true" : "false"));
      return false;
   }


   return false;
}

bool Modem_ARDOP::SetModulationMode(ardop_mode_t tncmode) {
   size_t max_mode = (sizeof(ardop_mode_names) / sizeof(ardop_mode_names[0]));

   if (tnc_mode < ARDOP_MODE_NONE || tnc_mode > max_mode) {
      Log->Send(LOG_CRIT, "Requested ardop mode %d is not a valid mode (max: %d)", tncmode, max_mode);
      return false;
   }

   Log->Send(LOG_DEBUG, "Setting ARDOP mode to %s (%d)", ardop_mode_names[tncmode], tncmode);
   return true;
}

ardop_mode_t Modem_ARDOP::GetModulationMode(void) {
   return this->tnc_mode;
}

const char *Modem_ARDOP::GetModulationModeName(void) {
   return ardop_mode_names[this->tnc_mode].name;
}

bool start_ardop_modem(void) {
   pid_t childpid;
   int status;
   char *args[2] = { "ardopc", NULL };

   childpid = fork();

   if (childpid >= 0) {
      if (childpid == 0) {
         const char *path = cfg->Get("path.ardop_bin", NULL);

         if (path == NULL) {
            Log->Send(LOG_CRIT, "Unable to spawn ardop modem (path.ardop_bin not confirmed)");
            return false;
         }

         if (execve(path, args, NULL) < 0) {
            Log->Send(LOG_CRIT, "unable to exec ardop modem at %s", path);
            return false;
         }
      } else {
//         wait(&status);
         sleep(1);
         Modem_ARDOP *ardop = new Modem_ARDOP();
         ardop->Connect();
      }
   } else {
      Log->Send(LOG_CRIT, "fork failed");
   }


   return true;
}
