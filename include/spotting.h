#if	!defined(_spotting_h)
#define	_spotting_h

enum spot_modes {
   SPOT_MODE_NONE = 0,
   SPOT_MODE_FT4,
   SPOT_MODE_FT8,
   SPOT_MODE_JS8,
   SPOT_MODE_PSK31,
   SPOT_MODE_WSPR,
   SPOT_MODE_ARDOP_ARQ,
   SPOT_MODE_ARDOP_FEC,
   SPOT_MODE_BELL_103,
   SPOT_MODE_BELL_202
};
typedef enum spot_modes spot_modes_t;

struct spot_mode_names {
   spot_modes_t mode;
   const char *name;
};

struct spot_data {
   // callsigns
   char rx_callsign[CALLSIGN_LEN];
   char tx_callsign[CALLSIGN_LEN];

   // maidenhead locations
   char rx_gridsquare[GRID_LEN];
   char tx_gridsquare[GRID_LEN];

   int	rx_power;		// received power
   time_t spot_time;		// time when TX was heard
};
typedef struct spot_data spot_data_t;

class Spotting_DB {
    private:
       Database *db;
    public:
       Spotting_DB();
       ~Spotting_DB();
       bool SendReport(spot_data_t *report);
};

class SpotReport {
   private:
      Spotting_DB *spot_db;	// spotting database
   public:
      SpotReport();
      ~SpotReport();
      bool SendReport(spot_data_t *report);
};

extern struct spot_mode_names spot_mode_names[];

#endif	// !defined(_spotting_h)
