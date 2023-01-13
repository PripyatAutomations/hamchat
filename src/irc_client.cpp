#include "hamchat.h"

// List of our client objects
llist_t *Clients = NULL;

////////////////////////////////////

//////////////////////////////////
// constructor for a new client //
//////////////////////////////////
Client::Client(irc_cli_type_t type, int fd) {
   this->cli_type = type;

   // are we a socket client?
   if (type == CLI_TYPE_LOCAL) {
      this->sock = new Socket(fd);
      this->sock->fd = fd;
      snprintf(this->hostname, HOST_NAME_MAX, "irc.local");

      // set the connection state...
      this->conn_state = IRC_STATE_NONE;
   }

   // set connected time to now
   this->connected = now;
   stats.curr_clients++;

   if (stats.curr_clients > stats.max_clients) {
      stats.max_clients = stats.curr_clients;
      Log->Info("*** New max clients %d (total %lu connections since startup) ***", stats.max_clients, stats.total_connections);
      // XXX: Send wallops to all local users?
   }

   // Send some progress to client
   this->Send("NOTICE AUTH :*** Processing your connection, please wait...");
   this->Send("NOTICE AUTH :*** If not already configured, please send your password using /PASS to continue");
}

////////////////
// Destructor //
////////////////
Client::~Client() {
   Client *cptr = this;

   Log->Debug("<%d> deleting Client", cptr->sock->fd);

   // stop socket event watcher
   ev_io_stop(main_loop, &cptr->sock->io);

   // close the socket
   close(cptr->sock->fd);

   // update stats
   stats.curr_clients--;

   // remove from client list...
   llist_t *lp = Clients;
   do {
     // shouldn't happen...
     if (lp == NULL)
        break;

     if (lp != NULL && lp->ptr != NULL) {
        Client *tcptr = (Client *)lp->ptr;

        // is it a valid file descriptor?
        if (tcptr->sock->fd < 0) {
           Log->Debug("wtf? fd=%d (<0) for client %x", tcptr->sock->fd, tcptr);
           continue;
        }

        // match!
        if (tcptr == cptr) {
           llist_remove(lp, Clients);

           // last item?
           if (Clients == lp)
              Clients = NULL;

           break;
        }
     }
     lp = lp->next;
   } while(lp != NULL);

   // get rid of the socket
   delete this->sock;
   this->sock = NULL;
}
// send a protocol message to the client
bool Client::Send(const char *fmt, ...) {
   va_list ap;
   va_start(ap, fmt);
   return this->Send(fmt, ap);
}

// alternate form
bool Client::Send(const char *fmt, va_list ap) {
   size_t rc = -1;
   // expand the message
   char mybuf[512];
   memset(mybuf, 0, 512);

   vsnprintf(mybuf, sizeof(mybuf), fmt, ap);

   if (this->sock == NULL) {
      Log->Crit("client %s has invalid socket struct (NULL)", (*this->callsign == '\0' ? this->callsign : "*"));
      return false;
   }

   if (this->cli_type == CLI_TYPE_LOCAL) {
      if ((rc = send(this->sock->fd, mybuf, strlen(mybuf), 0)) < 0) {
         Log->Debug("<%d> error writing to socket: %d (%s)", this->sock->fd, errno, strerror(errno));
      } else {
        // if succesfully written, add it to total length
        size_t term = send(this->sock->fd, "\r\n", 2, 0);

        if (term > 0)
          rc += term;

        Log->Debug("<%d> sent %d bytes \"%s\"", this->sock->fd, rc, mybuf);
      }
      return true;
   }
   return false;
}

Client *find_client(const char *callsign) {
   llist_t *lp = Clients;
   Client *cp = NULL;

   if (lp == NULL) {
      Log->Debug("<%s> find_client: Client list is NULL, returning", callsign);
      return NULL;
   }

   do {
      if (lp == NULL) {
         return NULL;
      }

      if (lp->ptr != NULL) {
         cp = (Client *)lp->ptr;

         if (strlen(cp->callsign) <= 0) {
            lp = lp->next;
            continue;
         }

         if (strcasecmp(cp->callsign, callsign) == 0) {
//            Log->Debug("<%s> find_client matched %x <returning cptr=%x>", callsign, lp, cp);
            return cp;
         } else {
//            Log->Debug("<%s> find_client skipping %x (!match: fd=%d)", callsign, lp, cp->sock->fd);
         }
      } else {
         Log->Debug("<%s> NULL in find_client: lp=%x, lp->ptr=%x", callsign, lp, lp->ptr);
      }
      lp = lp->next;
   } while (lp != NULL);

   Log->Debug("<%s> find_client returned no results", callsign);
   return NULL;
}

Client *find_client(int fd) {
   llist_t *lp = Clients;
   Client *cp = NULL;

   if (lp == NULL) {
      Log->Debug("<%d> find_client: Client list is NULL, returning", fd);
      return NULL;
   }

   do {
      if (lp == NULL) {
         return NULL;
      }

      if (lp->ptr != NULL) {
         cp = (Client *)lp->ptr;

         if (cp->sock->fd == fd) {
//            Log->Debug("<%d> find_client matched %x <returning cptr=%x>", fd, lp, cp);
            return cp;
         } else {
//            Log->Debug("<%d> find_client skipping %x (!match: fd=%d)", fd, lp, cp->sock->fd);
         }
      } else {
         Log->Debug("<%d> NULL in find_client: lp=%x, lp->ptr=%x", fd, lp, lp->ptr);
      }
      lp = lp->next;
   } while (lp != NULL);

   Log->Debug("<%d> find_client returned no results", fd);
   return NULL;
}

bool Client::send_connect_numerics(void) {
   Client *cptr = this;
   bool login_ok = false;
   char datebuf[64];
   time_t birthdate = stats.birthday;
   locale_t utc_locale;
   setlocale(LC_TIME, "C");
   memset(datebuf, 0, sizeof(datebuf));
   strftime(datebuf, sizeof(datebuf) - 1, "%a %b %d %Y at %H:%M:%S UTC", gmtime(&birthdate));

   // XXX: get the user's key from storage so we can compare it

   // Check if the user's password is matched (yes i know this is a weird place for it, but it ensures they're completed registration steps...)
   if (*this->pass != '\0') {
      // XXX: Look up the user in the database and set login_ok if they're good
      Log->Debug("<%d> validating password...", this->sock->fd);

      // If we made it here and login_ok hasn't been set true, tell them the password is wrong and boot them
      if (!login_ok) {
         // zero out the storage
         memset(this->pass, 0, PASS_LEN);
         Log->Debug("<%d> incorrect password, disconnecting!", this->sock->fd);
         this->Send(":%s 464 %s :Password incorrect", cfg->Get("core.servername", "hamchat.local"), this->callsign);
         this->Send("ERROR :Incorrect user/password");
         delete this;
         return false;
      }
   }
   this->Send(":%s 001 %s :Welcome to the Internet Relay Chat Network %s!", cfg->Get("core.servername", "hamchat.local"), this->callsign, this->callsign);
   this->Send(":%s 002 %s :Your host is %s, running version hamchat %s", cfg->Get("core.servername", "hamchat.local"), this->callsign, cfg->Get("core.servername", "hamchat.local"), VERSION);
   this->Send(":%s 003 %s :This server was created %s", cfg->Get("core.servername", "hamchat.local"), this->callsign, datebuf);
   this->Send(":%s 004 %s %s hamchat-%s :exp", cfg->Get("core.servername", "hamchat.local"), this->callsign, cfg->Get("core.servername", "hamchat.local"), VERSION);
   this->Send(":%s 251 %s :There are %lu users connected to this instance", cfg->Get("core.servername", "hamchat.local"), this->callsign, stats.curr_clients);
   this->Send(":%s 255 %s :I have %lu clients and no linked servers", cfg->Get("core.servername", "hamchat.local"), this->callsign, stats.curr_clients);
   this->Send(":%s 265 %s %lu %d :Current local users %lu, max %lu", cfg->Get("core.servername", "hamchat.local"), this->callsign, stats.curr_clients, stats.max_clients, stats.curr_clients, stats.max_clients);
   this->Send(":%s 266 %s %lu %lu :Current global users %lu, max %lu", cfg->Get("core.servername", "hamchat.local"), this->callsign, stats.curr_clients, stats.max_clients, stats.curr_clients, stats.max_clients);
   this->Send(":%s 250 %s :Highest connection count: %lu (%lu clients) (%lu received)", cfg->Get("core.servername", "hamchat.local"), this->callsign, stats.max_clients, stats.max_clients, stats.max_clients, stats.max_clients);

   // XXX: MOTD
   return true;
}

bool Client::SendToCommonChannels(const char *fmt, ...) {
   Client *cptr = this;
   va_list ap;
   va_start(ap, fmt);

   // XXX: figure out users in common channels and send to them
   llist_t *c = Channels;
   Channel *chptr;
   do {
     if (c == NULL)
        break;

     if (c->ptr == NULL)
        break;

     chptr = (Channel *)c->ptr;
     if (chptr->members == NULL)
        break;

     llist_t *ml = chptr->members;

     Log->Debug("Interating channel members for %s", chptr->name);

     do {
       if (ml == NULL)
          break;

       Client *mcptr = (Client *)ml->ptr;
       if (mcptr == NULL)
          break;

       if (mcptr == this) {
          Log->Debug("%s/%s: sending", chptr->name, mcptr->callsign);
          chptr->SendToAllExcept(NULL, fmt, ap);
       }
       ml = ml->next;
     } while (ml != NULL);
     c = c->next;
   } while (c != NULL);
   // XXX: Send to other servers
   return true;
}

bool Client::SetMode(const char *modes) {
   return false;
}
