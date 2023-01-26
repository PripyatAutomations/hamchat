#if	!defined(_modem_ardop_h)
#define	ARDOP_HOST	"127.0.0.1"
#define	ARDOP_PORT_CMD	8515
#define	ARDOP_PORT_DATA	8516

enum ardop_mode {
   ARDOP_MODE_NONE = 0,
   // ARQ modes
   ARDOP_MODE_ARQ_200MAX,
   ARDOP_MODE_ARQ_500MAX,
   ARDOP_MODE_ARQ_1000MAX,
   ARDOP_MODE_ARQ_2000MAX,
   ARDOP_MODE_ARQ_500FORCED,
   ARDOP_MODE_ARQ_1000FORCED,
   ARDOP_MODE_ARQ_2000FORCED,

   // FEC modes
   ARDOP_MODE_FEC_4FSK_200_50S,
   ARDOP_MODE_FEC_4FSK_200_50,
   ARDOP_MODE_FEC_4PSK_200_100S,
   ARDOP_MODE_FEC_4PSK_200_100,
   ARDOP_MODE_FEC_8PSK_200_100,
   ARDOP_MODE_FEC_4FSK_500_100S,
   ARDOP_MODE_FEC_4FSK_500_100,
   ARDOP_MODE_FEC_4PSK_500_100,
   ARDOP_MODE_FEC_8PSK_500_100,
   ARDOP_MODE_FEC_4PSK_500_167,
   ARDOP_MODE_FEC_8PSK_500_167,
   ARDOP_MODE_FEC_4FSK_1000_100,
   ARDOP_MODE_FEC_4PSK_1000_100,
   ARDOP_MODE_FEC_8PSK_1000_100,
   ARDOP_MODE_FEC_4PSK_1000_167,
   ARDOP_MODE_FEC_8PSK_1000_167,
   ARDOP_MODE_FEC_4FSK_2000_100,
   ARDOP_MODE_FEC_4PSK_2000_100,
   ARDOP_MODE_FEC_8PSK_2000_100,
   ARDOP_MODE_FEC_4PSK_2000_167,
   ARDOP_MODE_FEC_8PSK_2000_167
};
typedef enum ardop_mode ardop_mode_t;

struct ardop_mode_names {
   ardop_mode_t		type;
   const char 		*name;	
};

enum ardop_modem_state {
   MODEM_STATE_OFFLINE = 0,
   MODEM_STATE_DISC,
   MODEM_STATE_ISS,
   MODEM_STATE_IRS,
   MODEM_STATE_FEC_SEND,
   MODEM_STATE_FEC_RECV
};
typedef enum ardop_modem_state ardop_modem_state_t;

#define	ARDOP_HEADER	0xaaaa

struct ardop_proto_pkt {
   u_int16_t	pkt_header;
   u_int8_t	pkt_chan;
   u_int8_t	pkt_opcode;
   // up to 256 bytes - NULL terminated?
   char	        pkt_payload[256];
   // XXX: Tcp doesn't need CRC?
   u_int16_t    pkt_crc;
};

// ARDOP opcodes
#define	ARDOP_OP_DATA		0	// to TNC
#define	ARDOP_OP_CMD		1	// to TNC
#define	ARDOP_OP_SUCCESS	0	// from TNC (no data follows)
#define	ARDOP_OP_SUCCESS_DATA	1	// from TNC (NULL term data follows)
#define	ARDOP_OP_DATA_REPLY	7	// <length byte (len-1)>data follows

// ARDOP message channels
#define	ARDOP_CHAN_COMMAND	0x20	// 32
#define	ARDOP_CHAN_DATA		0x21	// 33
#define	ARDOP_CHAN_DEBUG	0x22	// 34
#define	ARDOP_CHAN_ALL		0xff	// poll all

class Modem_ARDOP {
   private:
      char host[HOST_NAME_MAX];
      int  port_cmd;
      int  port_data;
      Socket *sock_cmd;
      Socket *sock_data;
      ardop_modem_state_t ardop_state;
      // State that we should try to keep in sync with the modem
      ardop_mode_t tnc_mode;
      bool tnc_autobreak;
      bool tnc_busylock;
      char tnc_capture_dev[128];
      bool tnc_running;		// CODEC
      bool tnc_cwid;
      bool tnc_debuglog;
      int  tnc_drive_level;
      bool tnc_fecid;
      bool tnc_fecsend;
      int  tnc_fec_repeats;
      char tnc_gridsquare[6];
      int  tnc_leader;
      bool tnc_listen;
      int  tnc_maxconreq;
      char tnc_myaux[128];
      bool tnc_mycall[CALLSIGN_LEN];
      char tnc_playback_dev[128];
      int  tnc_send_id;
      int  tnc_squelch;
      char tnc_cwid_suffix[12];

   public:
      Modem_ARDOP();
      ~Modem_ARDOP();
      bool Connect(void);
      // Set Modulation Type
      bool SetModulationMode(ardop_mode_t mode);
      // Get Modulation Type
      ardop_mode_t GetModulationMode(void);
      const char *GetModulationModeName(void);
      // initialize the modem
      bool Init(void);
      ////////////////////////////////////////////////
      // interfaces to the software modem (ardop1c) //
      ////////////////////////////////////////////////
      // Poll the TNC for status, etc
      bool TNC_Poll(int channel);
};
extern struct ardop_mode_names ardop_mode_names[];
extern bool start_ardop_modem(void);

#endif	// !defined(_modem_ardop_h)
