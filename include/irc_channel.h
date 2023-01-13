#if	!defined(_irc_channel_h)
#define	_irc_channel_h

class Channel {
   private:
   public:
      Channel(Client *cptr, const char *name);
      ~Channel();
      llist_t *members;			// channel member list
      char name[CHAN_LEN];		// channel name
      char topic[TOPIC_LEN];		// channel topic
      char topic_setby[CALLSIGN_LEN];	// topic set by?
      char key[KEY_LEN];		// key for entry
      time_t topic_time;		// topic set when?
      Client *owner;			// client that created channel
      bool	is_inviteonly;		// mode +i
      bool	is_moderated;		// mode +m
      bool AddUser(Client *cptr);	// remove user from channel
      bool KickUser(Client *cptr, Client *kicker, const char *reason);
      bool RemoveUser(Client *cptr, const char *reason);	// add user tp channel
      bool SendToAllExcept(Client *cptr, const char *msg, va_list ap);
      bool SendToAllExcept(Client *cptr, const char *msg, ...);
      bool SendToAll(const char *msg, ...);

      // get status about users, for use elsewhere...
      bool IsOwner(Client *cptr);	// is cptr the channel owner?
      bool IsMember(Client *cptr);	// is cptr a member of channel?
      bool IsMuted(Client *cptr);	// is cptr muted in channel?
      bool IsOp(Client *cptr);		// is cptr an op in channel?
      bool IsVoice(Client *cptr);	// is cptr voiced in channel?
};

struct irc_channel_member {
   Client *cptr;		// client pointer
   time_t join_time;		// join time
   bool	has_op;			// is an operator
   bool has_voice;		// has voice
   bool is_muted;		// is muted (can't talk even if +o/+v)
};
typedef struct irc_channel_member irc_channel_member;

extern llist_t *Channels;

//////////////////////////
// Find channel by name //
//////////////////////////
// Find channel by name, returning llist pointer
extern llist_t *find_channel_lp(const char *name);

// Find channel by name, returning Channel pointer
extern Channel *find_channel(const char *name);

// Try to add the user to the channel or create it
extern Channel *find_or_add_channel(Client *cptr, const char *name);

// Count channels a user is in
extern int	count_user_channels(Client *cptr);

#endif	// !defined(_irc_channel_h)
