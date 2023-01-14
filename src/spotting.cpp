#include "hamchat.h"

//
// Support for reporting spots to various networks
//

SpotReport::SpotReport() {
   // we always initialize the the database target
   this->spot_db = new Spotting_DB();

   // XXX: add pskreporter output
   // XXX: add rbn output?
   // XXX: Other aggregators?
}

SpotReport::~SpotReport() {
}

bool SpotReport::SendReport(spot_data_t *report) {
   // if database logging is available, log it...
   if (this->spot_db != NULL)
      this->spot_db->SendReport(report);

   return true;
}
