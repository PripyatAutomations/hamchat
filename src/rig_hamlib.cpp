#include "hamchat_module.h"
#include <hamlib/rig.h>

Rig_Hamlib *rigctl_hamlib = NULL;

// Support controlling the rig via hamlib
//

Rig_Hamlib::Rig_Hamlib(rig_model_t model) {
   // NONE|BUG|ERR|WARN|VERBOSE|TRACE
   rig_set_debug(RIG_DEBUG_WARN);
   rig_load_all_backends();
   // or RIG_MODEL_RPC
   this->rig = rig_init(model);

   if (!this->rig) {
      Log->Crit("Failed initializing rig");
      shutdown(-1);
   }

//   rig.port = RIG_PORT_NETWORK;
//   rig.ptt_type = RIG_PTT_RIG;
//   rig_rmode = RIG_MODE_PKTUSB;
   int rc = rig_open(this->rig);
   if (rc != RIG_OK) {
      Log->Crit("hamlib_open: error %d (%s)", rc, rigerror(rc));
      shutdown(2);
   }
}

Rig_Hamlib::~Rig_Hamlib() {
   rig_close(this->rig);
   rig_cleanup(this->rig);
}

bool Rig_Hamlib::GetPTT(void) {
   int rc = rig_get_ptt(this->rig, RIG_VFO_A, &this->ptt);

   if (rc == RIG_OK)
      Log->Debug("read ptt: %s", (this->ptt ? "on" : "off"));
   else
      Log->Debug("read ptt failed: %d (%s)", rc, rigerror(rc));

   return false;
}

bool Rig_Hamlib::SetPTT(bool state) {
   int rc;

   Log->Info("* Setting PTT to %s", (state ? "true" : "false"));

   if (state) {
      rc = rig_set_ptt(this->rig, RIG_VFO_A, RIG_PTT_ON_DATA);
   } else {
      rc = rig_set_ptt(this->rig, RIG_VFO_A, RIG_PTT_OFF);
   }

   if (rc != RIG_OK ) {
      Log->Warn("rig_set_ptt: error = %s \n", rigerror(rc));
      return false;
   }
   return true;
}

value_t *Rig_Hamlib::GetTXPower(value_t *rv) {
   if (rig_has_set_level(this->rig, RIG_LEVEL_RFPOWER)) {
      rig_get_level(this->rig, this->rig_vfo_a, RIG_LEVEL_RFPOWER, rv);
      return rv;
   }
   return NULL;
}

bool Rig_Hamlib::SetTXPower(int power) {
   return true;
}

// This is called by rig_poller() to get rig status every once in a while */
bool Rig_Hamlib::PollStatus(void) {
   // Poll rig status
   // * POWER OUTPUT
   // * SWR
   // Poll rig configuration
   // * Frequency
   // * Mode
   // * Bandwidth
   return true;
}

freq_t *Rig_Hamlib::GetVFOFreq(vfo_t vfo, freq_t *freq) {
   int rc = rig_get_freq(this->rig, vfo, freq);

   if (rc == RIG_OK)
      return freq;

   return 0;
}


bool Rig_Hamlib::SetVFOFreq(vfo_t vfo, freq_t freq) {
   int rc = rig_set_freq(this->rig, vfo, freq);

   if (rc == RIG_OK)
      return true;

   return false;
}

bool Rig_Hamlib::SetVFOMode(vfo_t vfo, rmode_t mode, pbwidth_t width) {
   int rc = rig_set_mode(this->rig, vfo, mode, width);
   if (rc == RIG_OK)
     return true;

   return false;
}
