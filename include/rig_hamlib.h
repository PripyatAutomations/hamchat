#if	!defined(_rig_hamlib_h)
#define	_rig_hamlib_h

#include <hamlib/rig.h>

class Rig_Hamlib {
   private:
      RIG *rig;
//      freq_t *rig_freq;
//      rmode_t rig_rmode;
//      pbwidth_t rig_width;
      vfo_t rig_vfo_a;
      int rig_smeter;
      RigStatus_t *rigstatus;
      ptt_t ptt;

   public:
      Rig_Hamlib(rig_model_t model);
      ~Rig_Hamlib();
      bool PollStatus(void);
      bool GetPTT(void);
      bool SetPTT(bool state);
      bool SetTXPower(int power);
      value_t *GetTXPower(value_t *rv);
      freq_t *GetVFOFreq(vfo_t vfo, freq_t *freq);
      bool SetVFOFreq(vfo_t vfo, freq_t freq);
      bool SetVFOMode(vfo_t vfo, rmode_t mode, pbwidth_t width);
};

//extern Rig_Hamlib *rigctl_hamlib;

#endif	// !defined(_rig_hamlib_h)
