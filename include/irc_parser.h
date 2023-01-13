#if	!defined(_irc_parser_h)
#define _irc_parser_h

#define	CMD_LEN	32

#define IRC_PARSER_RAW_BUFFER_SIZE 512

class IRCParser {
};

struct irc_cmds {
    const char *cmd_proto;	// protcol (IRC) message
    const unsigned short cmd_byte; // bytecode representation
    int min_args,		// minimum args, else error
       max_args;		// maximum args, else error
    bool (*cmd_func)(Client *cptr, int argc, char **argv);
    bool registered;		// only available to fully logged in users
    const char *help;		// help message
};
typedef struct irc_cmds irc_cmd_t;

////// from ratbox
/* MessageHandler */
typedef enum HandlerType
{
	UNREGISTERED_HANDLER,
	CLIENT_HANDLER,
	RCLIENT_HANDLER,
	SERVER_HANDLER,
	ENCAP_HANDLER,
	OPER_HANDLER,
	LAST_HANDLER_TYPE
}
HandlerType;

/* struct Client* client_p   - connection message originated from
 * struct Client* source_p   - source of message, may be different from client_p
 * int            parc   - parameter count
 * char*          parv[] - parameter vector
 */
typedef int (*MessageHandler) (struct Client *, struct Client *, int, const char *[]);

struct MessageEntry
{
	MessageHandler handler;
	int min_para;
};

/* Message table structure */
struct Message
{
	const char *cmd;
	unsigned int count;	/* number of times command used */
	unsigned int rcount;	/* number of times command used by server */
	unsigned long bytes;	/* bytes received for this message */
	unsigned int flags;	/* bit 0 set means that this command is allowed
				 * to be used only on the average of once per 2
				 * seconds -SRB
				 */
	/* handlers:
	 * UNREGISTERED, CLIENT, RCLIENT, SERVER, OPER, LAST
	 */
	struct MessageEntry handlers[LAST_HANDLER_TYPE];
};

#define MFLG_SLOW	0x01	/* executed roughly once per 2s */
#define MFLG_UNREG	0x02	/* available to unregistered clients */

#define MAX_MSG_HASH 512	/* don't change this unless you know what you are doing */
#define MAXPARA    15

struct MessageHash
{
	char *cmd;
	struct Message *msg;
	struct MessageHash *next;
};

extern struct MessageHash *msg_hash_table[];
void parse(struct Client *, char *, char *);
int string_to_array(char *string, char *parv[]);
/*
 * m_functions execute protocol messages on this server:
 * int m_func(struct Client* client_p, struct Client* source_p, int parc, char* parv[]);
 *
 *    client_p    is always NON-NULL, pointing to a *LOCAL* client
 *            structure (with an open socket connected!). This
 *            identifies the physical socket where the message
 *            originated (or which caused the m_function to be
 *            executed--some m_functions may call others...).
 *
 *    source_p    is the source of the message, defined by the
 *            prefix part of the message if present. If not
 *            or prefix not found, then source_p==client_p.
 *
 *            (!IsServer(client_p)) => (client_p == source_p), because
 *            prefixes are taken *only* from servers...
 *
 *            (IsServer(client_p))
 *                    (source_p == client_p) => the message didn't
 *                    have the prefix.
 *
 *                    (source_p != client_p && IsServer(source_p) means
 *                    the prefix specified servername. (?)
 *
 *                    (source_p != client_p && !IsServer(source_p) means
 *                    that message originated from a remote
 *                    user (not local).
 *
 *
 *            combining
 *
 *            (!IsServer(source_p)) means that, source_p can safely
 *            taken as defining the target structure of the
 *            message in this server.
 *
 *    *Always* true (if 'parse' and others are working correct):
 *
 *    1)      source_p->from == client_p  (note: client_p->from == client_p)
 *
 *    2)      MyConnect(source_p) <=> source_p == client_p (e.g. source_p
 *            *cannot* be a local connection, unless it's
 *            actually client_p!). [MyConnect(x) should probably
 *            be defined as (x == x->from) --msa ]
 *
 *    parc    number of variable parameter strings (if zero,
 *            parv is allowed to be NULL)
 *
 *    parv    a NULL terminated list of parameter pointers,
 *
 *                    parv[0], sender (prefix string), if not present
 *                            this points to an empty string.
 *                    parv[1]...parv[parc-1]
 *                            pointers to additional parameters
 *                    parv[parc] == NULL, *always*
 *
 *            note:   it is guaranteed that parv[0]..parv[parc-1] are all
 *                    non-NULL pointers.
 */

#endif
