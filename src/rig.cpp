#include "hamchat.h"

// see include/config.h for MAX_RIGS (8 by default);
Rig *rigs[MAX_RIGS];
const int max_rigs = MAX_RIGS;

struct rig_driver_name rig_driver_names[] {
  { RIG_DRIVER_NONE, 		"NONE" },
  { RIG_DRIVER_HAMLIB,		"hamlib" },
  { RIG_DRIVER_FLRIG,		"flrig" },
  { RIG_DRIVER_SERIAL_PTT, 	"serial-ptt" },
  { RIG_DRIVER_NONE,		NULL }
};

void Rig::Connect(void) {
   Log->Send(LOG_INFO, "<radio%d>: Connecting to rig...", this->id);
   Log->Send(LOG_INFO, "<radio%d>: driver: %d: model %lu", this->driver, this->model);

   if (this->driver == RIG_DRIVER_HAMLIB) {
      this->hamlib = new Rig_Hamlib(this->model);
   } else {
      Log->Send(LOG_CRIT, "Unable to open rig driver %d, model %d", this->driver, this->model);
   }
}

Rig::Rig() {
}

Rig::~Rig() {
   // Turn off PTT, at unload...
   this->SetPTT(false);

   if (this->driver == RIG_DRIVER_HAMLIB) {
      if (this->hamlib != NULL) {
         delete this->hamlib;
         this->hamlib = NULL;
      }
   } else {
      Log->Send(LOG_WARNING, "Unsupoorted rig driver %d while unloading rig", this->driver);
   }
}

bool Rig::PollStatus(void) {
   if (this->driver == RIG_DRIVER_HAMLIB) {
      if (this->hamlib != NULL) {
         return this->hamlib->PollStatus();
      }
   }
   return false;
}

bool Rig::GetPTT(void) {
   if (this->driver == RIG_DRIVER_HAMLIB) {
      if (this->hamlib != NULL) {
         return this->hamlib->GetPTT();
      }
   }
   return false;
}

bool Rig::SetPTT(bool state) {
   if (this->driver == RIG_DRIVER_HAMLIB) {
      if (this->hamlib != NULL) {
         return this->hamlib->SetPTT(state);
      }
   }
   return false;
}

bool Rig::SetTXPower(int power) {
   if (this->driver == RIG_DRIVER_HAMLIB) {
      if (this->hamlib != NULL) {
         return this->hamlib->SetTXPower(power);
      }
   }
   return false;
}

// XXX: should we be using a float here??
int Rig::GetTXPower(int *power) {
   if (this->driver == RIG_DRIVER_HAMLIB) {
      if (this->hamlib != NULL) {
         value_t tmp;
         this->hamlib->GetTXPower(&tmp);
         return tmp.i;
      }
   } else
      return -1;
   return false;
}

bool Rig::SetVFOFreq(vfo_t vfo, freq_t freq) {
   if (this->driver == RIG_DRIVER_HAMLIB) {
      if (this->hamlib != NULL) {
         int rc = this->hamlib->SetVFOFreq(vfo, freq);
         if (rc == RIG_OK)
            return true;
      }
   }
   return false;
}

bool Rig::SetVFOMode(vfo_t vfo, rmode_t mode, pbwidth_t width) {
   if (this->driver == RIG_DRIVER_HAMLIB) {
      if (this->hamlib != NULL) {
         int rc = this->hamlib->SetVFOMode(vfo, mode, width);
         if (rc == RIG_OK)
            return true;
      }
   }
   return false;
}
