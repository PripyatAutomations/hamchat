#if	!defined(_module_h)
#define	_module_h

enum mod_type {
   MODULE_NONE = 0,		// invalid
   MODULE_IRC,			// irc protocol command
   MODULE_TRANSPORT,		// transport protocol
   MODULE_MODEM,		// softmodem driver
   MODULE_RIG			// rig interface (hamlib/flrig/serial)	
};
typedef enum mod_type mod_type_t;

struct module_info {
   mod_type_t	   mod_type;

   // IRC:
   struct irc_cmds *cmds[MAX_CMDS];	// commands
};
typedef struct module_info module_t;

class Module {
   private:
      void *hndl;
      
   public:
      bool loaded;	// is module loaded successfully?
      Module(const char *path);
      int Unload(void);
};


#endif	// !defined(_module_h)
