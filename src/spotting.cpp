#include "hamchat.h"

//
// Support for reporting spots to various networks
//

SpotReport::SpotReport() {
   this->spot_db = new Spotting_DB();
}

SpotReport::~SpotReport() {
}

bool SpotReport::SendReport(spot_data_t *report) {
   // if database logging is available, log it...
   if (this->spot_db != NULL)
      this->spot_db->SendReport(report);

   return true;
}