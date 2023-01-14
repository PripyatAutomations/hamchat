#if	!defined(_config_h)
#define	_config_h

#define	VERSION	"0.0.1"

//////////////
// Defaults //
//////////////
#define	DEFAULT_CALLSIGN "N0CALL"
#define	DEFAULT_GRID	 "AA00aa"
#define	DEFAULT_TXPOWER	 30

#define	CONFIG_FILE "etc/hamchat.cf"

// Maximum rigs configurable
#define	MAX_RIGS	8

// Time To Live for RF packets relayed
#define	DEFAULT_TTL		3
#define	MAX_TTL			4

// IDLE time for whois
#define	IDLE_AFTER		120

// Maidenhead maximum length
#define	GRID_LEN	6

// Maximum length of callsigns allowed
#define	CALLSIGN_LEN	16

// maximum IRC channel name length
#define	CHAN_LEN	24

// topic message length
#define	TOPIC_LEN	400

// username max
#define	USER_LEN	10
// 'real' name max
#define	REALNAME_LEN	24

// channel key length
#define	KEY_LEN		16

// maximum channels per user
#define	USER_MAX_CHAN	64

// maximum away message length
#define	AWAY_LEN	250

// maximum channels in a JOIN
#define	JOIN_MAX_CHAN	16

// password length
#define	PASS_LEN	32

//////
#define LISTEN_BACKLOG	5	// listen(2)

// PING TIME is how often to PING irc clients
#define	IRC_PING_INTERVAL	600

// PONG_TIME is how long to wait for a PONG from client
#define	IRC_PING_TIMEOUT	60

// commands to parse in one run of Client::Parse
#define	MAX_PARSE_CMDS	16

//#define	BUFFER_MAX	16384
#define	BUFFER_MAX	3072

/////////////////

class Config {
   private:
      dict *data;
      FILE *fp;
      char file[PATH_MAX];
   public:
      Config(const char *file);
      ~Config();
      int Add(const char *key, const char *val);
      int Add(const char *key, const char *val, time_t ts);
      int AddBlob(const char *key, const void **ptr);
      int AddBlob(const char *key, const void **ptr, time_t ts);
      const char *Get(const char *key, const char *defval);
      int GetInt(const char *key, const int defval);
      bool GetBool(const char *key, bool defval);
      double GetDouble(const char *key, const double defval);
      void *GetBlob(const char *key, const void **defval);
      int RemoveKey(const char *key);
      bool ParseSection(const char *section);
      int Dump(FILE *out);
};

extern Config *cfg;

#endif	// !defined(_config_h)
