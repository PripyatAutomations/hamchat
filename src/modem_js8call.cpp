#include "hamchat.h"

//
// Here we wrapp around the JS8 binary and try to do stuff
//
///usr/bin/js8 -s JS8Call -w 1 -m 3 -e /usr/bin -a /opt/remotepi/u/radio/.local/share/JS8Call -t /tmp/JS8Call

#define	JS8CALL_SHM	"hcjs8"
#define	TMPDIR		"/tmp/hamchat/js8"

int run_js8(void) {
   const char *args[] = {
      "-e", "/usr/bin",
      "--shmem", JS8CALL_SHM,
      "--patience", "1",		// 0-4
      "--tr-period", "1",
      "--fft-threads", "3",
      "--my-call", cfg->callsign,
      "--my-grid", cfg->gridsquare,
      "--data-path", "./db/js8",
      "--temp-path", TMPDIR
   };
   return 0;
}
