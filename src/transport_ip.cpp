#include "hamchat.h"
#include <netdb.h>
dict *Listeners = NULL;

int set_nonblocking(int fd) {
   int flags;
   flags = fcntl(fd, F_GETFL);
   flags |= O_NONBLOCK;
   return fcntl(fd, F_SETFL, flags);
}

// Receive something off the socket onto the end of the current contents of the Client's recvbuf
void sock_read_cb(EV_P_ ev_io *w, int revents) {
   Client *cptr = NULL;
   size_t nread = 0;
   int read_tries_left = 1;
   char *bp = NULL;

   // find the client structure or bail
   if ((cptr = find_client(w->fd)) == NULL) {
      Log->Send(LOG_CRIT, "<%d> sock_read_cb was called, but no Client * found, kicking!", w->fd);
      ev_io_stop(EV_A_ w);
      close(w->fd);
      return;
   }

   if (cptr->cli_type != CLI_TYPE_LOCAL) {
      Log->Send(LOG_CRIT, "What??? got socket read for non-local client on fd %d: %d", w->fd, cptr->cli_type);
      return;
   }

   if (cptr->sock == NULL) {
      Log->Send(LOG_CRIT, "wtf?! cptr->sock NULL on %d - exiting!", w->fd);
      abort();
   }
   // we shouldn't read into the buffer while parser is running...
   // this shouldn't happen and means someone screwed up a lock
   if (cptr->sock->recvbuf_lock) {
      Log->Send(LOG_DEBUG, "<%d> sock_read_cb called while recvbuf is locked, aborting (fix the bug!)", w->fd);
      abort();
      return;
   }

   // set the read lock
   cptr->sock->recvbuf_lock = true;

   // put outside the loop so we can collect totals
   size_t buf_size = sizeof(cptr->sock->recvbuf),
          buf_free = buf_size,
          buf_used = cptr->sock->recvbuf_offset,
          total_read = 0;

   while (read_tries_left > 0) {
      if (cptr->sock->recvbuf_offset > 0) {
         bp = cptr->sock->recvbuf + cptr->sock->recvbuf_offset;
      } else {
         bp = cptr->sock->recvbuf;
      }

      nread = recv(w->fd, bp, buf_free, 0);
      
//      Log->Send(LOG_DEBUG, "<%d> recv: %x, %lu, %lu: %s", w->fd, bp, buf_free, nread, bp);

      if (nread > 0) {
         // adjust these, so we calculate things correctly below...
         buf_used += nread;
         buf_free -= nread;
         total_read += nread;
         cptr->sock->recvbuf_offset += nread;

         // notify the parser that there's stuff waiting, if we see a whole packet there
         if (strchr(cptr->sock->recvbuf, '\r') != NULL) {
            cptr->sock->read_waiting = true;
         } else if (strchr(cptr->sock->recvbuf, '\n') != NULL) {
            cptr->sock->read_waiting = true;
         } else {
//            Log->Send(LOG_DEBUG, "<%d> read %lu bytes, but no line ending, so not calling parser", cptr->sock->fd, nread);
         }
      } else if (nread == 0) {	// deal with hangups
            if (cptr) {
               Log->Send(LOG_INFO, "<%d> hangup by IRC Client %s (%s)", w->fd, cptr->GetCallsign(), cptr->GetCallsign());
               cptr->SendToCommonChannels(":%s QUIT :Connection reset by peer", cptr->GetCallsign());
               cptr->sock->recvbuf_lock = false;	// unlock this so things can carry on...
               delete cptr;
               return;
            } else {
               Log->Send(LOG_INFO, "<%d> hangup by unknown connection", w->fd);
               ev_io_stop(EV_A_ w);
               close(w->fd);
               continue;
            }
      } else if (errno == EAGAIN) {
           // nothing left to read...
           break;
      }
      read_tries_left--;
//      if (read_tries_left > 0) {
//         Log->Send(LOG_DEBUG, "<%d> read_tries_left: %d", w->fd, read_tries_left);
//      } else if (read_tries_left == -1) {
//         Log->Send(LOG_WARNING, "<%d> read failed", w->fd);
//      }
   }

   Log->Send(LOG_DEBUG, "<%d> COMPLETED recv: buf_used: %lu (%lu new), buf_free: %lu (max: %lu)", w->fd, buf_used, total_read, buf_free, buf_size);
//   Log->Send(LOG_DEBUG, "<%d> buffer: |%s|", w->fd, cptr->sock->recvbuf);

   // unlock the readbuf
   cptr->sock->recvbuf_lock = false;
}

Socket::Socket(int fd) {
   if (fd >= 0) {
      this->fd = fd;
   } else {
      // create an empty socket, for Connect() call later
   }
};


Socket::Socket(const char *uri) {
   int fd;
   const char *start = uri, *sep;
   enum { UDP = 0, TCP, FIFO } type;
   struct hostent *hostnm;
   struct sockaddr_in server;

   // save the whole URI to the object
   size_t cpysz = strlen(uri);
   if (cpysz > sizeof(this->uri))
      cpysz = sizeof(this->uri);
   memcpy(this->uri, uri, cpysz);

   // parse out protocol
   if (strncasecmp(uri, "tcp://", 6) == 0) {
     type = TCP;
     start = uri + 6;
   } else if (strncasecmp(uri, "udp://", 6) == 0) {
     type = UDP;
     start = uri + 6;
   } else if (strncasecmp(uri, "fifo://", 7) == 0) {
     type = FIFO;
     start = uri + 7;
   } 

   // to do the needful for IP transports
   if (type == TCP || type == UDP) {
      // extract the port
      sep = strchr(start, ':');

      if (sep == NULL) {
         Log->Send(LOG_CRIT, "invalid socket address: %s", uri);
         delete this;
      }

      // seperator is valid!
      if (sep > uri) {
         // extract and save port
         int port;
         port = atoi(sep+1);

         // save hostname
         memset(this->host, 0, HOST_NAME_MAX);
         size_t cpylen = sep - uri;
         if (cpylen > 0)
            memcpy(this->host, uri, cpylen);

         if (port == 0) {
            Log->Send(LOG_CRIT, "parsing listener %s failed: port '%s' invalid (0)", this->host, port);
            shutdown(101);
         }
         this->port = port;
      }

      // resolve the hostname
      hostnm = gethostbyname(this->host);
      if (hostnm == NULL) {
         Log->Send(LOG_CRIT, "couldnt resolve hostname %s", this->host);
      }
   } else {
     Log->Send(LOG_ERR, "* Invalid socket type selected (%d): %s", type, uri);
     delete this;
   }

   if (type == TCP) {
      Log->Send(LOG_INFO, "* Connecting to tcp://%s:%d", host, port);
   } else if (type == UDP) {
      Log->Send(LOG_INFO, "* Connecting to udp://%s:%d", host, port);
   }

   // Create the socket connection
   if (type == TCP)
      this->fd = socket(PF_INET, SOCK_STREAM, 0);
   else
      this->fd = socket(PF_INET, SOCK_DGRAM, 0); 
}

Socket::~Socket() {
   if (this->fd >= 0) {
      close(this->fd);
      this->fd = -1;
   }
}

//////////////////////////////////////////
// A simple socket listener using libev //
//////////////////////////////////////////
static void sock_pending_cb(EV_P_ ev_io *w, int revents) {
   int client_fd = -1;

   while (true) {
      struct sockaddr_in client_addr;
      socklen_t client_len = sizeof(client_addr);

      client_fd = accept(w->fd, (struct sockaddr *)&client_addr, &client_len);

      if (client_fd == -1) {
         if (errno != EAGAIN && errno != EWOULDBLOCK) {
            Log->Send(LOG_WARNING, "<%d> Failed to accept(): %d (%s)", w->fd, errno, strerror(errno));
         }
         break;
      }
      Log->Send(LOG_INFO, "<%d> Accepted new connection on fd <%d>", w->fd, client_fd);

      // update statistics
      stats.total_connections++;

      // create the client object
      Client *cptr = new Client(CLI_TYPE_LOCAL, client_fd);
      set_nonblocking(cptr->sock->fd);
      ev_io_init(&cptr->sock->io, sock_read_cb, cptr->sock->fd, EV_READ);
      ev_io_start(EV_A_ &cptr->sock->io);
   }
}

Listener::Listener(const char *addr) {
   const char *sep, *pp;

   if (Listeners == NULL) {
      Listeners = dict_new();
   }

   if (addr == NULL) {
      this->port = 6660;
   }

   sep = strchr(addr, ':');

   if (sep == NULL) {
      Log->Send(LOG_CRIT, "invalid listener configuration: %s", addr);
      shutdown(100);
   }

   // seperator is valid!
   if (sep > addr) {
      memset(this->host, 0, HOST_NAME_MAX);
      pp = sep + 1;
      int ptmp = atoi(pp);

      if (ptmp == 0) {
         Log->Send(LOG_CRIT, "parsing listener %s failed: port '%s' invalid (0)", addr, port);
         shutdown(101);
      }
      this->port = ptmp;
      size_t cpylen = sep - addr;

      if (cpylen > 0)
         memcpy(this->host, addr, cpylen);
   }

   this->sock = new Socket(socket(PF_INET, SOCK_STREAM, 0));

   Log->Send(LOG_INFO, "Starting listener on %s:%d", this->host, this->port);
   this->running = true;

   // try to set SO_REUSEADDR for quicker restarts
   const int one = 1;

   if (setsockopt(this->sock->fd, SOL_SOCKET, SO_REUSEADDR, &one, sizeof(int)) < 0)
      Log->Send(LOG_CRIT, "setsockopt(SO_REUSEADDR) failed");

   // Convert the host and port into a BSD style sockaddr_in
   struct sockaddr_in sa;
   sa.sin_family = AF_INET;
   sa.sin_port = htons(this->port);

   // allow wildcard listeners
   if (this->host[0] == '*') {
      sa.sin_addr.s_addr = INADDR_ANY;
   } else {
      inet_aton(this->host, &sa.sin_addr);
   }

   if (bind(this->sock->fd, (struct sockaddr *)&sa, sizeof(sa)) != 0) {
      Log->Send(LOG_CRIT, "error binding listener %s:%d: %d (%s)", this->host, this->port, errno, strerror(errno));
      shutdown(120);
   }

   // and start non-blocking listener!
   set_nonblocking(this->sock->fd);
   listen(this->sock->fd, LISTEN_BACKLOG);

   // add it to our listener list
   char buf[16];
   memset(buf, 0, 16);
   snprintf(buf, 15, "%d", this->sock->fd);
   dict_add_blob(Listeners, buf, (void *)this);

   // let libev handle events on the socket   
   ev_io_init(&this->sock->io, sock_pending_cb, this->sock->fd, EV_READ);
   ev_io_start(main_loop, &this->sock->io);
}
