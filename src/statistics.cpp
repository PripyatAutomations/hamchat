#define _statistics_cpp
#include "hamchat.h"

struct Stats stats;

bool update_statistics(void) {
    // do nothing yet
    return true;
}

// dump_statistics(cfg->statsfile);

bool dump_statistics(const char *path) {
    struct stat sb;
    FILE *fp;

    if (stat(path, &sb) == 0) {
       // statistics file already exists, try to unlink it
       if (unlink(path)) {
          Log->Send(LOG_WARNING, "failed removing old statistics file %s: %d (%s)", path, errno, strerror(errno));
          return false;
       }
    }
    if ((fp = fopen(path, "w+")) == NULL) {
       Log->Send(LOG_WARNING, "failed opening statistics file %s: %d (%s)", path, errno, strerror(errno));
       return false;
    }

    // XXX: Dump the stats here
    fprintf(fp, "uptime=%lu\n", (now - stats.birthday));

    // flush it to disk
    fflush(fp);

    // close it
    fclose(fp);
    fp = NULL;
    return true;
}
