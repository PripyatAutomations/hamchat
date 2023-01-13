// these come from src/m_*.cpp
extern bool m_admin (Client *cptr, int argc, char **argv);
extern bool m_away (Client *cptr, int argc, char **argv);
extern bool m_cap (Client *cptr, int argc, char **argv);
extern bool m_die (Client *cptr, int argc, char **argv);
extern bool m_help (Client *cptr, int argc, char **argv);
extern bool m_info (Client *cptr, int argc, char **argv);
extern bool m_invite (Client *cptr, int argc, char **argv);
extern bool m_join (Client *cptr, int argc, char **argv);
extern bool m_kick (Client *cptr, int argc, char **argv);
extern bool m_kill (Client *cptr, int argc, char **argv);
extern bool m_list (Client *cptr, int argc, char **argv);
extern bool m_lusers (Client *cptr, int argc, char **argv);
extern bool m_mode (Client *cptr, int argc, char **argv);
extern bool m_motd (Client *cptr, int argc, char **argv);
extern bool m_names (Client *cptr, int argc, char **argv);
extern bool m_nick (Client *cptr, int argc, char **argv);
extern bool m_notice (Client *cptr, int argc, char **argv);
extern bool m_oper (Client *cptr, int argc, char **argv);
extern bool m_part (Client *cptr, int argc, char **argv);
extern bool m_pass (Client *cptr, int argc, char **argv);
extern bool m_ping (Client *cptr, int argc, char **argv);
extern bool m_pong (Client *cptr, int argc, char **argv);
extern bool m_privmsg (Client *cptr, int argc, char **argv);
extern bool m_quit (Client *cptr, int argc, char **argv);
extern bool m_rehash (Client *cptr, int argc, char **argv);
extern bool m_stats (Client *cptr, int argc, char **argv);
extern bool m_time (Client *cptr, int argc, char **argv);
extern bool m_topic (Client *cptr, int argc, char **argv);
extern bool m_unknown (Client *cptr, int argc, char **argv);
extern bool m_user (Client *cptr, int argc, char **argv);
extern bool m_userhost (Client *cptr, int argc, char **argv);
extern bool m_version (Client *cptr, int argc, char **argv);
extern bool m_who (Client *cptr, int argc, char **argv);
extern bool m_whois (Client *cptr, int argc, char **argv);
extern bool m_whowas (Client *cptr, int argc, char **argv);

struct irc_cmds irc_commands[] = {
   // Command,  Numeric, MinArgs, MaxArgs, Function, Registered
   { "ADMIN", 	'A', 0, 1, 	&m_admin,	true,		"Shows information about the server administration" },
   { "AWAY", 	'B', 0, 1, 	&m_away,	true,		"Sets or clears an away message. RF auto-replies disabled while away" },
   { "CAP", 	'C', 0, MAXPARA, &m_cap,	false,		"Internal comamnd (IRCv3) NYI" },
   { "DIE", 	'E', 0, 1, 	&m_die,		true,		"Allows the admin to kill the sever (with optional password)" },
   { "HELP",    '\0', 0, 2,	&m_help,	true,		"Display help about commands" },
   { "INFO", 	'F', 0, 0, 	&m_info,	true,		"Displays information about the server and it's developers" },
   { "INVITE", 	'G', 2, 2, 	&m_invite,	true,		"Invite users to a channel (op required)" },
   { "JOIN", 	'I', 1, 2, 	&m_join,	true,		"Join a channel" },
   { "KICK", 	'J', 2, 3, 	&m_kick,	true,		"Remove a user from a channel (op required)" },
   { "KILL",	'K', 1, 2,	&m_kill,	true,		"Remove a user from the server (server admin required)" },
   { "LIST", 	'L', 0, 2, 	&m_list,	true,		"Lists/search available channels" },
   { "LUSERS",	'i', 0, 0,	&m_lusers,	true,		"Display statistics of users/servers/channels" },
   { "MODE", 	'M', 1, MAXPARA, &m_mode,	true,		"Set Channel or User modes" },
   { "MOTD", 	'N', 0, 1, 	&m_motd,	true,		"Display the server Message of The Day" },
   { "NAMES", 	'O', 0, 1, 	&m_names,	true,		"Show the names of the channel membership"  },
   { "NICK", 	'P', 1, 1, 	&m_nick,	false,		"Set or Change your callsign/suffix" },
   { "NOTICE", 	'Q', 2, 2, 	&m_notice,	true,		"Send a NOTICE to user or channel" },
   { "OPER", 	'R', 1, 1, 	&m_oper,	true,		"Login as an administrator" },
   { "PART", 	'S', 0, 1, 	&m_part,	true,		"Leave a channel" },
   { "PASS", 	'T', 0, 1, 	&m_pass,	true,		"Send PASSword during login" },
   { "PING", 	'U', 1, 1, 	&m_ping,	false,		"Request a response (PONG) from another client or server" },
   { "PONG", 	'V', 1, 1, 	&m_pong,	false,		"Message sent in response to a PING from a user or server" },
   { "PRIVMSG", 'W', 2, 3, 	&m_privmsg,	true,		"Send a message to channel or user" },
   { "QUIT", 	'X', 0, 1, 	&m_quit,	false,		"Leave the IRC network" },
   { "REHASH", 	'Y', 0, 1, 	&m_rehash,	true,		"Admin command to reload server configuration" },
   { "STATS", 	'Z', 1, 2, 	&m_stats,	true, 		"Display service statistics" },
   { "TIME", 	'a', 0, 1, 	&m_time,	true,		"Get the current time on server"  },
   { "TOPIC", 	'b', 1, 2, 	&m_topic,	true,		"Get or set the topic of a channel" },
   { "USER", 	'c', 4, 4, 	&m_user,	false,		"Sent during registration to set username/realname" },
   { "USERHOST", 'd', 1, 5, 	&m_userhost,	true,		"Get user's host info" },
   { "VERSION", 'e', 0, 1, 	&m_version,	true,		"Display the version of ther server" },
   { "WHO", 	'f', 1, 1, 	&m_who, 	true,		"Query a list of users matching a hostmask|nick|channel" },
   { "WHOIS", 	'g', 0, 2, 	&m_whois,	true,		"Displays extended information about the user" },
   { "WHOWAS", 	'h', 1, 1, 	&m_whowas,	true,		"Displays information abotu the last time a callsign was heard" },
   { NULL,	'\0', -1, -1, 	NULL,		false,		NULL }
};
