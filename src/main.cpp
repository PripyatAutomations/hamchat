#define	_main_cpp
#include "hamchat.h"

Config *cfg;

// Our current time
time_t now = time(NULL);

// libev stuff
ev_timer tick_timer,
         service_irc_timer;
struct ev_loop	*main_loop = EV_DEFAULT;

// service all the irc clients
static void service_irc_cb(EV_P_ ev_timer *w, int events) {
#if	0
   llist_t *lp = Clients;
   do {
      if (lp == NULL)
         return;

      if (lp->ptr != NULL) {
         Client *cptr = (Client *)lp->ptr;

         // parse it's commands
         if (cptr->sock->read_waiting) {
            // but only if there's a whole line available...
            if (strchr(cptr->sock->recvbuf, '\n') != NULL) {
               Log(LOG_DEBUG, "<%d> cptr has a new message waiting, parsing", cptr->sock->fd);
               cptr->Parse();
            } else {
               Log(LOG_DEBUG, "<%d> readwaiting but no newline", cptr->sock->fd);
            }
            // Only do this if debugging
            cptr->sock->read_waiting = false;
         }

         // Is it an active user or still logging in? if so, don't bother yet
         if (cptr->conn_state & (IRC_STATE_NICK|IRC_STATE_USER)) {
            // send a ping if none has been sent, or the last one was a long time ago...
            if (cptr->last_ping == 0 || (cptr->got_pong && (now - cptr->last_ping) > IRC_PING_INTERVAL)) {
               cptr->last_ping = now;
               cptr->got_pong = false;
               cptr->Send("PING :%lu", cptr->last_ping);
            } else if (!cptr->got_pong && (now - cptr->last_ping > IRC_PING_TIMEOUT)) {
               // if ping time has been exceeded and we didn't g
               Log(LOG_DEBUG, "<%d> PING timeout %lu", cptr->sock->fd, cptr->last_ping);
               cptr->SendToCommonChannels(":%s QUIT :Ping timeout (%lu) seconds", cptr->callsign, IRC_PING_TIMEOUT);
               delete cptr;
               return;
            }
         }
      } else {
         Log(LOG_CRIT, "invalid llist item %x in Clients list <%x> has NULL ptr", lp, Clients);
      }
      lp = lp->next;
   } while(lp != NULL);
#endif

}

static void tick_cb(EV_P_ ev_timer *w, int revents) {
   now = time(NULL);
}

// Main loop
int main(int argc, char **argv) {
   init_signals();
   stats.birthday = now;

   // XXX: implement commandline parser (src/cmdline.cpp)
   // did we get passed a config file?
   if (argc > 1) {
      cfg = new Config(argv[1]);
   } else {	// nope, used the compiled in path (include/config.h)
      cfg = new Config(CONFIG_FILE);
   }
   daemonize();

   // ensure if we die from a signal that we cleanup a bit (pidfile, etc)
   atexit(shutdown);

   // Bring up logging as early as possible
   Log = new Logger(cfg->Get("path.logfile", "file:///tmp/hamchat.log"));
   Log->Send(LOG_INFO, "hamchat %s starting up. Good luck!", VERSION);

   // Start up listeners
   cfg->ParseSection("listen");

   // Main db
   db = new Database(cfg->Get("path.db_main", "hamchat.db"), cfg->Get("path.db_main_schema", "init.sql"));
   init_modem_thread();

   // this loop will service clients every time it's called
   ev_timer_init(&service_irc_timer, service_irc_cb, 0.1, 0.1);
   ev_timer_start(main_loop, &service_irc_timer);

   // start out periodic timer tick
   ev_timer_init(&tick_timer, tick_cb, 1, 1);
   ev_timer_start(main_loop, &tick_timer);

   // Initialize radios
   cfg->ParseSection("radio");

   // setup heartbeat messages
   heartbeat = new Heartbeat();

   Log->Send(LOG_INFO, "We're up and running with pid %d, good luck and godspeed!", getpid());
   ev_run(main_loop, 0);

   // cleanup and exit gracefully ;)
   shutdown();
   return EXIT_SUCCESS;
}
