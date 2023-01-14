#include "hamchat.h"
Heartbeat *heartbeat = NULL;

//
// Support for heartbeat networking
//
static void heartbeat_callback(EV_P_ ev_timer *w, int revents) {
   if (heartbeat != NULL) {
      heartbeat->Send();
   }
} 

Heartbeat::Heartbeat() {
   // Start periodic heartbeat broadcasts
   ev_timer_init(&this->heartbeat_timer, heartbeat_callback, cfg->GetDouble("heartbeat.delay", 60), cfg->GetDouble("heartbeat.interval", 600));
   ev_timer_start(main_loop, &heartbeat_timer);
}

Heartbeat::~Heartbeat() {
   // End periodic timer
   ev_timer_stop(main_loop, &this->heartbeat_timer);
}

bool Heartbeat::Send(void) {
   // Check if PTT is disabled first...
   // XXX: We need to get query this from rigctl
   // fire off heartbeat messages
   if (this->buf[0] == '\0') {
      size_t buf_sz = sizeof(this->buf);
      memset(this->buf, 0, buf_sz);
      snprintf(this->buf, buf_sz, "%s@%s/%dW",cfg->Get("station.callsign", "N0CALL"), cfg->Get("station.gridsquare", "FN20"), cfg->GetInt("station.default_txpower", 5));
   }

   Log->Send(LOG_DEBUG, "Sending heartbeat: %s...", this->buf);

   return true;
}
