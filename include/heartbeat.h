#if	!defined(_heartbeat_h)
#define	_heartbeat_h

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
      bool Send(void);
};

extern Heartbeat *heartbeat;

#endif	// !defined(_heartbeat_h)
