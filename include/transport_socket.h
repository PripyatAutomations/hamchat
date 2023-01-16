#if	!defined(_socket_h)
#define	_socket_h

class Socket {
   public:
      Socket(int fd);
      Socket(const char *uri);
      ~Socket();
      // sending binary data
      bool Send(void *data, size_t data_sz);
      // sending text data with a \n line termination
      bool Send(const char *msg, ...);
      bool vSend(const char *msg, va_list *ap);
      int fd;
      int port;
      char host[HOST_NAME_MAX+1];
      char uri[HOST_NAME_MAX+16];
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
      char uri[HOST_NAME_MAX+16];
      int port;
      bool	running;	// is it started?
      Socket *sock;
      int	fd;	/* listener fd */
      ev_io io;		// libev stuff
      Listener(const char *addr);
      bool Start();
};

// Set the fd to non-blocking mode
extern int set_nonblocking(int fd);

// this is in socket.cpp and needs to be called from socket_listener.cpp
extern void sock_read_cb(EV_P_ ev_io *w, int revents);

#endif	// !defined(_socket_h) 
