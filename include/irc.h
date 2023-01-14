#if	!defined(_irc_h)
#define	_irc_h
#include <stdlib.h>
#include "irc_client.h"
#include "irc_parser.h"
#include "irc_channel.h"

extern bool is_valid_callsign(const char *callsign);

static inline const char *__fd_to_str(int fd) {
    static char itoa_buf[10];
    memset(itoa_buf, 0, 10);
    snprintf(itoa_buf, 10, "%d", fd);
    return itoa_buf;
}

#define ClientId(x)		( ((x)->callsign[0] != '\0') ? (x)->callsign : __fd_to_str((x)-sock->fd) )

#endif	// !defined(_irc_h)
