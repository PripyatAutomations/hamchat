#if	!defined(_statistics_h)
#define	_statistics_h

struct Stats {
   time_t birthday;		// when did we start?
   ///////////
   // Users //
   ///////////
   unsigned long total_connections;	// total connections since start
   unsigned long max_clients;		// maxium clients
   unsigned long curr_clients;		// current clients
   unsigned long curr_opers;

   ///////////
   // Chans //
   ///////////
   unsigned long max_channels;		// maximum channels
   unsigned long curr_channels;		// current clients

   /////////////
   // Servers //
   /////////////
   unsigned long max_servers;
   unsigned long curr_servers;
};

extern bool update_statistics(void);
extern bool dump_statistics(const char *path);

#if	!defined(_statistics_cpp)
extern struct Stats stats;
#endif

#endif	// !defined(_statistics_h)
