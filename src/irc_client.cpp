#include "hamchat.h"

///////////////////////////////////
// These MUST be kept up to date //
///////////////////////////////////
dict *Clients_by_fd = NULL;
dict *Clients_by_callsign = NULL;

////////////////////////////////////

//////////////////////////////////
// constructor for a new client //
//////////////////////////////////
Client::Client(irc_cli_type_t type, int fd) {
   // Initialize the client storages
   if (Clients_by_fd == NULL)
      Clients_by_fd = dict_new();

   if (Clients_by_callsign == NULL)
      Clients_by_callsign = dict_new();

   this->cli_type = type;

   // are we a socket client?
   if (type == CLI_TYPE_LOCAL) {
      this->sock = new Socket(fd);
      snprintf(this->hostname, HOST_NAME_MAX, "irc.local");

      // set the connection state...
      this->conn_state = IRC_STATE_NONE;

      // Add the client to the _by_fd lookup table
      if (fd) {
         this->sock->fd = fd;
         char buf[12];
         memset(buf, 0, 12);
         snprintf(buf, 11, "%d", fd);
         dict_add_blob(Clients_by_fd, buf, (void *)this);
      }
   }

   // set connected time to now
   this->connected = now;
   stats.curr_clients++;

   if (stats.curr_clients > stats.max_clients) {
      stats.max_clients = stats.curr_clients;
      Log->Send(LOG_INFO, "*** New max clients %d (total %lu connections since startup) ***", stats.max_clients, stats.total_connections);
      // XXX: Send wallops to all local users?
   }

   Log->Send(LOG_NOTICE, "New Client: type=%d", this->cli_type);
   // Send some progress to client
   this->Send("NOTICE AUTH :*** Processing your connection, please wait...");
   this->Send("NOTICE AUTH :*** If not already configured, please send your password using /PASS to continue");
}

////////////////
// Destructor //
////////////////
Client::~Client() {
   Client *cptr = this;

   // is it local?
   if (cptr->cli_type == CLI_TYPE_LOCAL) {
      Log->Send(LOG_DEBUG, "<%d> deleting Client", cptr->sock->fd);

      // stop socket event watcher
      ev_io_stop(main_loop, &cptr->sock->io);

      // delete reference to the socket
      char fdbuf[16];
      memset(fdbuf, 0, 16);
      snprintf(fdbuf, 15, "%d", cptr->sock->fd);
      dict_del(Clients_by_fd, fdbuf);

      // close the socket
      close(cptr->sock->fd);
      // get rid of the socket
      delete this->sock;
   }
   // remove client name reference for all clients
   if (cptr->callsign[0] != '\0') {
      dict_del(Clients_by_callsign, cptr->callsign);
   }

   // update stats
   stats.curr_clients--;

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
      Log->Send(LOG_CRIT, "client %s has invalid socket struct (NULL)", (*this->callsign == '\0' ? this->callsign : "*"));
      return false;
   }

   if (this->cli_type == CLI_TYPE_LOCAL) {
      if ((rc = send(this->sock->fd, mybuf, strlen(mybuf), 0)) < 0) {
         Log->Send(LOG_DEBUG, "<%d> error writing to socket: %d (%s)", this->sock->fd, errno, strerror(errno));
      } else {
        // if succesfully written, add it to total length
        size_t term = send(this->sock->fd, "\r\n", 2, 0);

        if (term > 0)
          rc += term;

        Log->Send(LOG_DEBUG, "<%d> sent %d bytes \"%s\"", this->sock->fd, rc, mybuf);
      }
      return true;
   }
   return false;
}

Client *find_client(const char *callsign) {
   Client *cp = NULL;

   if ((cp = (Client *)dict_get_blob(Clients_by_callsign, callsign, NULL)) == NULL) {
      Log->Send(LOG_DEBUG, "<%s> find_client returned no results", callsign);
      return NULL;
   }
   return cp;
}

Client *find_client(int fd) {
   Client *cp = NULL;

   char buf[16];
   memset(buf, 0, 16);
   snprintf(buf, 15, "%d", fd);

   if ((cp = (Client *)dict_get_blob(Clients_by_fd, buf, NULL)) == NULL) {
      Log->Send(LOG_DEBUG, "<%d> find_client returned no results", fd);
      return NULL;
   }

   return cp;
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
      Log->Send(LOG_DEBUG, "<%d> validating password...", this->sock->fd);

      // If we made it here and login_ok hasn't been set true, tell them the password is wrong and boot them
      if (!login_ok) {
         // zero out the storage
         memset(this->pass, 0, PASS_LEN);
         Log->Send(LOG_DEBUG, "<%d> incorrect password, disconnecting!", this->sock->fd);
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

//   chptr->SendToAllExcept(NULL, fmt, ap);
   // XXX: Send to other servers
   return true;
}

bool Client::SetMode(const char *modes) {
   return false;
}

bool Client::SetCallsign(const char *callsign) {
   // cleanup first, if needed
   if (this->callsign[0] != '\0') {
      dict_del(Clients_by_callsign, this->callsign);
   }

   // copy it in place
   memset(this->callsign, 0, CALLSIGN_LEN);
   snprintf(this->callsign, CALLSIGN_LEN, "%s", callsign);

   // and add to the lookup table
   dict_add_blob(Clients_by_callsign, this->callsign, (void *)this);

   return false;
}

const char *Client::GetCallsign(void) {
   return this->callsign;
}
