#if	!defined(_wireproto_h)
#define	_wireproto_h

// this contains a list of IRC command to wire protocol mappings...
// these should be invoked by the IRC message handler m_* calling
// wireproto_send(Client *cptr, int argc, char **argv);
//
struct wireproto_cmds {
   const char *cmd;
   bool       (*func)(Client *cptr, int argc, char **argv);
};

extern bool wireproto_send(Client *cptr, int argc, char **argv);

// our 'simple' protocol, the only one for now... (wireproto_simple.cpp)
extern struct wireproto_cmds proto_cmds_simple[];

#endif	// !defined(_wireproto_h)
