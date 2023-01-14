#include "hamchat.h"

int is_file(const char *path) {
   struct stat sb;

   if (stat(path, &sb) != 0)
      return 0;

   if (S_ISREG(sb.st_mode)) {
      Log->Send(LOG_DEBUG, "is_file: %s: true", path);
      return 1;
   }

   return 0;
}

int is_dir(const char *path) {
   struct stat sb;

   if (stat(path, &sb) != 0)
      return 0;

   if (S_ISDIR(sb.st_mode))
      return 1;

   return 0;
}

int is_link(const char *path) {
   struct stat sb;

   if (stat(path, &sb) != 0)
      return 0;

   if (S_ISLNK(sb.st_mode))
      return 1;

   return 0;
}

int is_fifo(const char *path) {
   struct stat sb;

   if (stat(path, &sb) != 0)
      return 0;

   if (S_ISFIFO(sb.st_mode))
      return 1;

   return 0;
}
