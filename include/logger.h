#if	!defined(_logger_h)
#define	_logger_h

#define LOG_INVALID_STR "*invalid_level*"

enum log_target_type { NONE = 0, LOG_syslog, LOG_stderr, LOG_file, LOG_fifo };
typedef struct {
  // private (do not modified)
  enum log_target_type type;
  FILE *fp;
} LogHndl;

//extern LogHndl *mainlog;

class Logger {
    private:
        FILE *fp;
        LogHndl *hndl;

    public:
        Logger(const char *path);
        ~Logger();
        bool Send(int level, const char *msg, va_list);
        bool Send(int level, const char *msg, ...);
};

extern Logger *Log;

#endif	// !defined(_logger_h)
