#if	!defined(_motd_h)
#define	_motd_h
class MOTD {
   private:
      char motd_path[PATH_MAX];
//      llist_t *motd_text;
      struct stat last_stat;
   public:
      MOTD(const char *path);
      bool Send(Client *cptr);
};

extern MOTD *motd;
#endif	// !defined(_motd_h)
