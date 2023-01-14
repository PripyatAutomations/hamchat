#if	!defined(_heartbeat_h)
#define	_heartbeat_h

struct heartbeat_pkt {
   char   tx_call[CALLSIGN_LEN];
   char   tx_grid[GRID_LEN];
   float    tx_power;
   time_t tx_time;
};
typedef struct heartbeat_pkt heartbeat_pkt_t;

class Heartbeat {
   private:
      void *(Callback)();
      char buf[32];
      void *packet;	// pointer to the audio buffer
      time_t	last_sent,
                last_ack;
      ev_timer heartbeat_timer;
   public:	
      Heartbeat();
      ~Heartbeat();
      heartbeat_pkt_t *CreatePacket(Client *cptr, Rig *rig);
      bool SerializePacket(heartbeat_pkt_t *pkt, char *buf, size_t bufsz);
      bool Send(Rig *rig);
};

extern Heartbeat *heartbeat;

#endif	// !defined(_heartbeat_h)
