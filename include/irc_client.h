#if	!defined(_irc_client_h)
#define	_irc_client_h

// connection state, apply as each is recieved
#define	IRC_STATE_NONE	0x0000
#define	IRC_STATE_CAP	0x0001
#define	IRC_STATE_CAP_END 0x0002
#define	IRC_STATE_NICK	0x0004
#define	IRC_STATE_USER	0x0010
#define	IRC_STATE_PASS	0x0010

// flags client must have to be considered 'alive' (able to message, join, etc)
#define	IRC_STATE_ALIVE	(IRC_STATE_NICK|IRC_STATE_USER)
#define	IRC_IsActive(x)		((x)->conn_state & IRC_STATE_ALIVE)
#define	IRC_HasCap(x)		((x)->conn_state & IRC_STATE_CAP)
#define	IRC_DoneCap(x)		((x)->conn_state & (IRC_STATE_CAP|IRC_STATE_CAP_END))

enum irc_cli_type {
   CLI_TYPE_NONE = 0,
   CLI_TYPE_LOCAL,		// real, local user
   CLI_TYPE_REMOTE,		// remote user struct
   CLI_TYPE_SYSTEM,		// builtin 'bot' user
};
typedef enum irc_cli_type irc_cli_type_t;

class Client {
   private:
      char callsign[CALLSIGN_LEN+1]; // amateur callsign
   public:
      irc_cli_type_t cli_type;
      Socket *sock;		// our socket
      bool is_oper;		// is an irc operator?
      char *away_msg;		// away message, if present
      time_t away_since;	// away since?
      time_t connected;		// sign on time
      time_t last_ping;		// last ping sent
      bool   got_pong;		// got pong for last ping?
      time_t last_msg;		// last time a msg was sent by
      int conn_state;		// Connection flags
      char username[USER_LEN+1]; // username sent by client
      char realname[REALNAME_LEN+1]; // 'real' name
      char hostname[HOST_NAME_MAX+1];
      char pass[PASS_LEN];
      Client(irc_cli_type_t type, int fd);		// Constructor
      ~Client();

      bool Send(const char *fmt, va_list ap);
      bool Send(const char *fmt, ...);
      bool Parse(void);
      // send messages to all common channels
      bool SendToCommonChannels(const char *fmt, ...);
      bool send_connect_numerics(void);
      // User Modes & Flags
      bool SetCallsign(const char *callsign);
      const char *GetCallsign(void);
      bool SetMode(const char *modes);
};

///////////////////////////
// list of connected IRC clients
extern dict *Clients_by_fd;
extern dict *Clients_by_callsign;

// Locate the client structure associated with a file descriptor, if possible
extern Client *find_client(int fd);

// Locate the client structure by callsign, if possible
extern Client *find_client(const char *callsign);

#endif	// !defined(_irc_client_h)
