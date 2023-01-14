#define	_daemon_cpp
#include "hamchat.h"
int pidfd = -1;

const char *pidfile = NULL;

// Detach from the console and go into background
int daemonize(void) {
   struct stat sb;

   if (pidfile == NULL)
      pidfile = cfg->Get("path.pidfile", "hamchat.pid");

   if (stat(pidfile, &sb) == 0) {
      fprintf(stderr, "pidfile %s already exists, bailing!\n", pidfile);
      _exit(1);
   }

   // are we configured to daemonize?
   if (cfg->GetBool("core.daemonize", false)) {
      printf("going to the background...\n");
      pid_t pid = fork();

      if (pid < 0) {
         fprintf(stderr, "daemonize: Unable to fork(): %d (%s)", errno, strerror(errno));
         exit(EXIT_FAILURE);
      } else if (pid > 0) {
         // parent exiting
         exit(EXIT_SUCCESS);
      }

      // set umask for created files
      umask(0);

      // attempt to fork our own session id
      pid_t sid = setsid();
      if (sid < 0) {
         fprintf(stderr, "daemonize: Unable to create new SID for child process: %d (%s)", errno, strerror(errno));
         exit(EXIT_FAILURE);
      }
   }

   // save pid file
   pidfd = open(pidfile, O_RDWR | O_CREAT | O_SYNC, 0600);
   if (pidfd == -1) {
      fprintf(stderr, "daemonize: opening pid file %s failed: %d (%s)", pidfile, errno, strerror(errno));
      exit(EXIT_FAILURE);
   }

   // try to lock the pid file, so we can ensure only one instance runs
   if (lockf(pidfd, F_TLOCK, 0) != 0) {
      fprintf(stderr, "daemonize: failed to lock pid file %s: %d (%s)", pidfile, errno, strerror(errno));
      unlink(pidfile);
      exit(EXIT_FAILURE);
   }

   // Print the process id to pidfd
   char buf[10];
   memset(buf, 0, 10);
   sprintf(buf, "%d\n", getpid());
   write(pidfd, buf, strlen(buf));

   // only close stdio if daemonizing
   if (cfg->GetBool("core.daemonize", false)) {
      // close stdio
      close(STDIN_FILENO);
      close(STDOUT_FILENO);
      close(STDERR_FILENO);
   }

   return 0;
}

void shutdown(void) {
   close(pidfd);
   pidfd = -1;
   unlink(pidfile);
}

void shutdown(int status) {
   shutdown();
   Log->Send(LOG_CRIT, "shutting down: %d", status);
   exit(status);
}


// Catch signals
static void sighandler(int signum) {
   Log->Send(LOG_CRIT, "caught signal %d...", signum);
   switch(signum) {
      // Convenience signals
      case SIGHUP:
         Log->Send(LOG_INFO, "Reloading!");
         break;
      case SIGUSR1:
         Log->Send(LOG_INFO, "Dumping database to disk");
         break;
      case SIGUSR2:
         Log->Send(LOG_INFO, "Dumping statistics to disk");
         dump_statistics(cfg->Get("path.statsfile", NULL));
         break;
      // Fatal signals
      case SIGINT:
      case SIGTERM:
      case SIGKILL:
         shutdown(signum);
      default:
         Log->Send(LOG_CRIT, "Caught unknown signal %d", signum);
         break;
   }
}

// set up signal handlers
void init_signals(void) {
   // Fatal signals
   signal(SIGINT, sighandler);
   signal(SIGTERM, sighandler);
   signal(SIGKILL, sighandler);
   // User signals
   signal(SIGHUP, sighandler);
   signal(SIGUSR1, sighandler);
   signal(SIGUSR2, sighandler);
}
