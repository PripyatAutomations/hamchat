#if	!defined(_irc_channel_h)
#define	_irc_channel_h

struct irc_channel_member {
   Client *cptr;		// client pointer
   time_t join_time;		// join time
   bool	has_op;			// is an operator
   bool has_voice;		// has voice
   bool is_muted;		// is muted (can't talk even if +o/+v)
};
typedef struct irc_channel_member irc_channel_member;

class Channel {
   private:
   public:
      // cptr is creator, can be NULL
      Channel(Client *cptr, const char *name);
      ~Channel();
      dict *members;			// channel members dictionary (blob pointers to irc_channel_member *)
      char name[CHAN_LEN];		// channel name
      char topic[TOPIC_LEN];		// channel topic
      char topic_setby[CALLSIGN_LEN];	// topic set by?
      char key[KEY_LEN];		// key for entry
      time_t topic_time;		// topic set when?
      Client *owner;			// client that created channel
      bool	is_inviteonly;		// mode +i
      bool	is_moderated;		// mode +m

      // Administrative Functions
      ///////////////////////////////
      bool AddUser(Client *cptr);	// remove user from channel
      bool KickUser(Client *cptr, Client *kicker, const char *reason);
      bool RemoveUser(Client *cptr, const char *reason);	// add user tp channel

      // Persistence Stuff
      //////////////////////
      bool DumpToDb(Database *db, const char *table);

      // Send messages to channel members
      /////////////////////////////////////
      bool SendToAllExcept(Client *cptr, const char *msg, va_list ap);
      bool SendToAllExcept(Client *cptr, const char *msg, ...);
      bool SendToAll(const char *msg, ...);

      // get status about users, for use elsewhere...
      ////////////////////////////////////////////////
      bool IsOwner(Client *cptr);	// is cptr the channel owner?
      bool IsMember(Client *cptr);	// is cptr a member of channel?
      bool IsMuted(Client *cptr);	// is cptr muted in channel?
      bool IsOp(Client *cptr);		// is cptr an op in channel?
      bool IsVoice(Client *cptr);	// is cptr voiced in channel?
};

//////////////////////////
// Find channel by name //
//////////////////////////
// Find channel by name, returning Channel pointer
extern Channel *find_channel(const char *name);

// Try to add the user to the channel or create it
extern Channel *find_or_add_channel(Client *cptr, const char *name);

// Count channels a user is in
extern int	count_user_channels(Client *cptr);
extern bool load_channels_from_db(Database *db, const char *table);

#endif	// !defined(_irc_channel_h)
