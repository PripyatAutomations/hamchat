#include "hamchat.h"
// XXX: Add support for logging to channels

static struct log_levels {
   const char *str;
   int level;
} log_levels[] = {
   { "debug", LOG_DEBUG },
   { "info", LOG_INFO },
   { "notice", LOG_NOTICE },
   { "warning", LOG_WARNING },
   { "error", LOG_ERR },
   { "critical", LOG_CRIT },
   { "alert", LOG_ALERT },
   { "emergency", LOG_EMERG },
   { NULL, -1 },
};

static inline const char *LogName(int level) {
   struct log_levels *lp = log_levels;

   do {
      if (lp->level == level)
         return lp->str;

      lp++;
   } while(lp->str != NULL);

   return LOG_INVALID_STR;
}

static inline const int LogLevel(const char *name) {
   struct log_levels *lp = log_levels;

   do {
      if (strcmp(lp->str, name) == 0)
         return lp->level;

      lp++;
   } while(lp->str != NULL);

   return -1;
}

bool Logger::Send(int level, const char *msg, ...) {
   va_list ap;
   va_start(ap, msg);
   return this->Send(level, msg, ap);
}

bool Logger::Send(int level, const char *msg, va_list ap) {
   char datebuf[32];
   char buf[4096];
   int max;
   FILE *fp = stderr;		// default to stderr til logging is up

   // create data stamp
   memset(datebuf, 0, sizeof(datebuf));
   strftime(datebuf, sizeof(datebuf) - 1, "%Y-%m-%d %H:%M:%S", localtime(&now));

   max = LogLevel(cfg->Get("log.level", "debug"));

   if (max < level)
      return false;


   if (this->log_hndl) {
      if (this->log_hndl->type == LOG_syslog) {
         vsyslog(level, msg, ap);
      } else if (this->log_hndl->type != NONE) {
         memset(buf, 0, 4096);
         vsnprintf(buf, 4095, msg, ap);
      }

      if (this->log_hndl->type != LOG_stderr && this->log_hndl->fp)
         fp = this->log_hndl->fp;
   }

   // if we aren't in daemon mode, write to stdout
   if (cfg->GetBool("core.daemonize", false) == false && fp != stderr) {
      fprintf(stdout, "%s [%s] %s\n", datebuf, LogName(level), buf);
      fflush(stdout);
   }

   fprintf(fp, "%s [%s] %s\n", datebuf, LogName(level), buf);

   // force it to disk
   fflush(fp);

   va_end(ap);
   return true;
}

Logger::Logger(const char *path) {
   this->log_hndl = (LogHndl *)malloc(sizeof(LogHndl));

   if (strcasecmp(path, "syslog") == 0) {
      this->log_hndl->type = LOG_syslog;
      openlog("hamchat", LOG_NDELAY|LOG_PID, LOG_DAEMON);
   } else if (strcasecmp(path, "stderr") == 0) {
      this->log_hndl->type = LOG_stderr;
      this->log_hndl->fp = stderr;
   } else if (strncasecmp(path, "fifo://", 7) == 0) {
      if (is_fifo(path + 7) || is_file(path + 7))
         unlink(path + 7);

      mkfifo(path+7, 0600);

      if (!(this->log_hndl->fp = fopen(path + 7, "w"))) {
         this->Send(LOG_ERR, "Failed opening log fifo '%s' %s (%d)", path+7, errno, strerror(errno));
         this->log_hndl->fp = stderr;
      } else
         this->log_hndl->type = LOG_fifo;
   } else if (strncasecmp(path, "file://", 7) == 0) {
      if (!(this->log_hndl->fp = fopen(path + 7, "w+"))) {
         this->Send(LOG_ERR, "failed opening log file '%s' %s (%d)", path+7, errno, strerror(errno));
         this->log_hndl->fp = stderr;
      } else
         this->log_hndl->type = LOG_file;
   }
}

Logger::~Logger() {
   if (this->log_hndl == NULL)
      return;

   if (this->log_hndl->type == LOG_file || this->log_hndl->type == LOG_fifo) {
      fflush(this->log_hndl->fp);
      fclose(this->log_hndl->fp);
   } else if (this->log_hndl->type == LOG_syslog) {
      closelog();
   }

   free(this->log_hndl);
   this->log_hndl = NULL;
}
