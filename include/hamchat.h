#if	!defined(_hamchat_h)
#define	_hamchat_h
#define	MAX_CMDS	32	// max commands per module
#include <list>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <syslog.h>
#include <sqlite3.h>
#include <dlfcn.h>
#include <errno.h>
#include <dirent.h>
#include <stdarg.h>
#include <limits.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <locale.h>
#include <time.h>
#include <ev.h>
// These need to be roughly in this order ;)
#include "file.h"
#include "dict.h"
#include "config.h"
#include "logger.h"
#include "list.h"
// we maybe have slight chicken ate the egg problem here...
#include "transport.h"
#include "modem.h"
#include "database.h"
#include "heartbeat.h"
#include "irc.h"
#include "modes.h"
#include "module.h"
#include "motd.h"
#include "packet.h"
#include "spotting.h"
#include "statistics.h"
#include "rig.h"

// src/daemonize.cpp
extern int daemonize(void);
extern void shutdown(void);
extern void shutdown(int fatal);
extern void init_signals(void);

// src/main.cpp
extern time_t now;
extern Database *main_db;
extern time_t now;
extern struct ev_loop *main_loop;
extern Logger *Log;

#endif	// !defined(_hamchat_h)
