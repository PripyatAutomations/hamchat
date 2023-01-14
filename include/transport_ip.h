#if	!defined(_socket_h)
#define	_socket_h

class Socket {
   public:
      Socket(int fd);
      Socket(const char *uri);
      ~Socket();
      int fd;
      char sendbuf[BUFFER_MAX];
      char recvbuf[BUFFER_MAX];
      bool sendbuf_lock;
      bool recvbuf_lock;
      bool read_waiting;
      size_t recvbuf_offset;
      ev_io io;			// libev bit
};

class Listener {
   private:
   public:
      char host[HOST_NAME_MAX];
      int port;
      bool	running;	// is it started?
      Socket *sock;
      int	fd;	/* listener fd */
      ev_io io;		// libev stuff
      Listener(const char *addr);
      bool Start();
};


//extern llist_t *Listeners;

// create configured listeners...
extern bool create_listener(const char *uri);

// Set the fd to non-blocking mode
extern int set_nonblocking(int fd);

// this is in socket.cpp and needs to be called from socket_listener.cpp
extern void sock_read_cb(EV_P_ ev_io *w, int revents);

#endif	// !defined(_socket_h) 
