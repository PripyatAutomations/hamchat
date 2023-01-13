#include "hamchat.h"

struct spot_mode_names spot_mode_names[] = {
   { SPOT_MODE_NONE, 		"NONE" },
   { SPOT_MODE_FT4,		"FT4" },
   { SPOT_MODE_FT8,		"FT8" },
   { SPOT_MODE_JS8,		"JS8" },
   { SPOT_MODE_PSK31,		"PSK31" },
   { SPOT_MODE_WSPR,		"WSPR" },
   { SPOT_MODE_ARDOP_ARQ,	"ARDOP_ARQ" },
   { SPOT_MODE_ARDOP_FEC,	"ARDOP_FEC" },
   { SPOT_MODE_BELL_103,	"Bell.103" },
   { SPOT_MODE_BELL_202,	"Bell.202" }
};

//
// Support for storing spots into a database
//

Spotting_DB::Spotting_DB() {
}

Spotting_DB::~Spotting_DB() {
}

bool Spotting_DB::SendReport(spot_data_t *report) {
   if (this->db == NULL)
      return false;

//   this->db->Execute("INSERT INTO spots (rx_call, tx_call, rx_grid, tx_grid, spot_time, mode) VALUES ('%s', '%s', '%s', '%s', '%s');",
//      data->rx_callsign, data->tx_callsign, data->rx_grid, data->tx_grid, data->spot_time, data->mode);

   return true;   
}
