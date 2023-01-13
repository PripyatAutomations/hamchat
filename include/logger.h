#if	!defined(_logging_h)
#define	_logging_h

class Logger {
    private:
        FILE *fp;
        int Log(enum LogLevel loglevel, const char *str, va_list);
    public:
        Logger(const char *path);
        ~Logger();
        int Crit(const char *f, ...);
        int Debug(const char *f, ...);
        int Info(const char *f, ...);
        int Warn(const char *f, ...);
};

#if	!defined(_logger_c)
extern Logger *Log;
extern void log_init(const char *path);

#endif

#endif	// !defined(_logging_h)
