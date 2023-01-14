#if	!defined(_logger_h)
#define	_logger_h
/////////////////////////////////////////////
// private bits used only internally
#define LOG_INVALID_STR "*invalid_level*"

enum log_target_type { NONE = 0, LOG_syslog, LOG_stderr, LOG_file, LOG_fifo };
typedef struct {
  // private (do not modified)
  enum log_target_type type;
  FILE *fp;
} LogHndl;
///////////////////////////////////////////

//////////////////////
// Public interface //
//////////////////////
class Logger {
    private:
        FILE *fp;
        LogHndl *log_hndl;

    public:
        Logger(const char *path);
        ~Logger();
        bool Send(int level, const char *msg, va_list);
        bool Send(int level, const char *msg, ...);
};

#endif	// !defined(_logger_h)
