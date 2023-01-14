#if	!defined(_rig_h)
#define	_rig_h

enum rig_driver_t {
   RIG_DRIVER_NONE = 0,
   RIG_DRIVER_HAMLIB,
   RIG_DRIVER_FLRIG,
   RIG_DRIVER_SERIAL_PTT
};

struct rig_driver_name {
   enum rig_driver_t driver;
   const char *name;
};

struct RigStatus {
   bool malloced;		// set to true if allocated internally
   
};
typedef struct RigStatus RigStatus_t;

#define	RIG_DESCR_MAX	128

#include "rig_hamlib.h"

class Rig {
   private:
      bool  ptt_blocked;		// Allows blocking PTT on a device globally
      Channel *chptr;			// channel for control + status
      // driver pointers
      Rig_Hamlib *hamlib;		// if it's a hamlib driver
   public:
      char description[RIG_DESCR_MAX];	// rig description (free form)
      int  id;				// rig ID #
      enum rig_driver_t driver;		// what driver is in use?
      int  model;			// rig model
      Rig();
      ~Rig();
      void Connect(void);
      bool PollStatus(void);
      bool GetPTT(void);
      bool SetPTT(bool state);
      bool SetTXPower(int power);
      int GetTXPower(int *rv);
      bool SetVFOFreq(vfo_t vfo, freq_t freq);
      bool SetVFOMode(vfo_t vfo, rmode_t mode, pbwidth_t width);
};

// from src/rig.cpp
extern struct rig_driver_name rig_driver_names[];
extern Rig *rigs[MAX_RIGS];
extern const int max_rigs;

#endif	// !defined(_rig_h)
