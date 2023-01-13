   #include "hamchat.h"

//
//
//
// XXX: Add support for logging to a channel
//

Logger *Log;
static enum LogLevel my_loglevel = LOG_DEBUG;

void log_init(const char *path) {
   Log = new Logger(path);
}

Logger::Logger(const char *path) {
   FILE *fp = NULL;

   if ((fp = fopen(path, "w+")) != NULL) {
      this->fp = fp;
       printf("Opened logfile %s!\n", path);
   } else {
      fprintf(stderr, "hamchat: Failed opening log file %s\n", path);
      // disable daemonizing
      cfg->Add("core.daemonize", "false");
   }
}

Logger::~Logger() {
   if (this->fp != NULL) {
      this->Crit("Closing logfile!");
      fflush(this->fp);
      fclose(this->fp);
   }
}

int Logger::Log(enum LogLevel loglevel, const char *str, va_list ap) {
   char logbuf[1024];
   char datebuf[32];
   const char *priority = NULL;

   if (loglevel > my_loglevel) {
      // Don't bother showing it
      return 0;
   }

   // expand out the log message to full text
   memset(logbuf, 0, sizeof(logbuf));
   vsnprintf(logbuf, sizeof(logbuf) - 1, str, ap);

   // create data stamp
   memset(datebuf, 0, sizeof(datebuf));
   strftime(datebuf, sizeof(datebuf) - 1, "%Y-%m-%d %H:%M:%S", localtime(&now));

   // priority labels      
   switch(loglevel) {
      case LOG_CRIT:
         priority = "Crit";
         break;
      case LOG_WARN:
         priority = "Warn";
         break;
      case LOG_INFO:
         priority = "Info";
         break;
      case LOG_DEBUG:
         priority = "Debug";
         break;
      case LOG_NONE:
      default:
         priority = "NONE";
         break;
   }

   // if we aren't in daemon mode, write to stdout
   if (cfg->GetBool("core.daemonize", false) == false) {
      fprintf(stdout, "%s [%s] %s\n", datebuf, priority, logbuf);
      fflush(stdout);
   }

   if (this->fp != NULL) {
      // print timestamp and system
      fprintf(this->fp, "%s [%s] %s\n", datebuf, priority, logbuf);

      // ensure it gets written to disk immediately
      fflush(this->fp);
   }

   return 0;
}

int Logger::Crit(const char *f, ...) {
   va_list ap;

   va_start(ap, f);
   return this->Log(LOG_CRIT, f, ap);
}

int Logger::Debug(const char *f, ...) {
   va_list ap;

   va_start(ap, f);
   return this->Log(LOG_DEBUG, f, ap);
}

int Logger::Info(const char *f, ...) {
   va_list ap;

   va_start(ap, f);
   return this->Log(LOG_INFO, f, ap);
}

int Logger::Warn(const char *f, ...) {
   va_list ap;

   va_start(ap, f);
   return this->Log(LOG_WARN, f, ap);
}
