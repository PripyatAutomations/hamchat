#include "hamchat.h"
Heartbeat *heartbeat = NULL;

//
// Support for heartbeat networking
//
static void heartbeat_callback(EV_P_ ev_timer *w, int revents) {
   Rig *rig = rigs[0];

   if (heartbeat != NULL) {
      // XXX: figure out which rig the heartbeats need to go out
      heartbeat->Send(rig);
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

#define	MIN_BUF_SZ	80

bool Heartbeat::SerializePacket(heartbeat_pkt_t *pkt, char *buf, size_t bufsz) {
   if (pkt == NULL || buf == NULL || bufsz < MIN_BUF_SZ) {
      Log->Send(LOG_CRIT, "%s: invalid arguments pkt <%x> buf <%x> bufsz %lu", __FUNCTION__, pkt, buf, bufsz); 
      return false;
   }
   
   memset(buf, 0, bufsz);

   snprintf(buf, bufsz, "HB %s %s@%.03f %lu\n", pkt->tx_call, pkt->tx_grid, pkt->tx_power, pkt->tx_time);
//   Log->Send(LOG_DEBUG, "HeartBeatPacketSerialized: %s", buf);
   return true;
}

bool Heartbeat::Send(Rig *rig) {
   char buf[80];

   // Check if PTT is disabled first...
   // XXX: We need to get query this from rigctl
   // fire off heartbeat messages
//   if (this->buf[0] == '\0') {
//      size_t buf_sz = sizeof(this->buf);
//      memset(this->buf, 0, buf_sz);
//      snprintf(this->buf, buf_sz, "%s@%s/%dW",cfg->Get("station.callsign", "N0CALL"), cfg->Get("station.gridsquare", "FN20"), cfg->GetInt("station.default_txpower", 5));
//   }

   heartbeat_pkt_t *hb = NULL;
// XXX: Send heartbeats for all connected CLI_TYPE_LOCAL (in Clients_by_fd)
//   this->CreatePacket(cptr)

   // for now just send station heartbeat
   hb = this->CreatePacket(NULL, rig);

   if (this->SerializePacket(hb, buf, sizeof(buf)) == true) {
      Log->Send(LOG_DEBUG, "Sending heartbeat: %s...", buf);
   } else {
      Log->Send(LOG_CRIT, "Heartbeat::SerializePacket() returned false, not sending...");
   }

   return true;
}

heartbeat_pkt_t *Heartbeat::CreatePacket(Client *cptr, Rig *rig) {
   heartbeat_pkt_t *hb = (heartbeat_pkt_t *)malloc(sizeof(heartbeat_pkt_t));

   if (hb == NULL) {
      Log->Send(LOG_CRIT, "malloc failed allocating heartbeat message");
      abort();
   }

   memset(hb, 0, sizeof(heartbeat_pkt_t));

   const char *call = NULL;
   if (cptr != NULL) {     // If client set, use it's callsign and our location
      call = cptr->GetCallsign();
   } else {
      call = cfg->Get("station.callsign", NULL);

      if (call == NULL) {
         Log->Send(LOG_CRIT, "Edit your configuration noob!");
         return NULL;
      }
   }
   memcpy(hb->tx_call, call, strlen(call));
   const char *sta_grid = cfg->Get("station.gridsquare", NULL);

   if (sta_grid == NULL) {
      Log->Send(LOG_CRIT, "No station grid configured; edit station.gridsquare and restart!");
      abort();
   }

   // store the grid location of the station
   size_t grid_sz = strlen(sta_grid);
   if (grid_sz > GRID_LEN)
      grid_sz = GRID_LEN;

   memcpy(hb->tx_grid, sta_grid, grid_sz);
   
   // get the TX power station it
   int tmp = -1;
//   hb->tx_power = rig->GetTXPower(&tmp);
   hb->tx_power = cfg->GetDouble("station.default_txpower", 1.0);

   // send our time so others can synchronize
   hb->tx_time = now;

   return hb;
}
