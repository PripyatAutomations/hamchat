#include "hamchat.h"

// Here we should load the motd the first time it's requested
// and keep it in memory until shutdown

MOTD *motd;

MOTD::MOTD(const char *path) {
   FILE *fp;
   struct stat sb;

   if (stat(path, &sb) == 0) {
      fp = fopen(path, "r");

      if (!fp) {
         Log->Warn("couldn't open motd file %s: %d: %s", path, errno, strerror(errno));
         return;
      }

      // save the motd path...
      snprintf(this->motd_path, PATH_MAX, "%s", path);
      // copy the stat struct, so we can see if file has changed later...
      memcpy((void *)&this->last_stat, (void *)&stat, sizeof(struct stat));

      while (!feof(fp)) {
         char *buf = (char *)malloc(256);
         memset(buf, 0, 256);

         // read from the file
         if (fgets(buf, 256, fp) != NULL) {
            if (this->motd_text == NULL) {
               this->motd_text = llist_append(NULL, (void *)buf);
            } else {
               llist_append(this->motd_text, (void *)buf);
            }
         } else {
            Log->Debug("Failed loading motd line, trying to continue...");
         }
      }

      // close file handle
      fclose(fp);
   } else {
      Log->Warn("motd file %s not found", path);
   }
}

bool MOTD::Send(Client *cptr) {
   struct stat sb;
   
   if (cptr == NULL)
      return false;

   if (this->motd_text != NULL) {
      cptr->Send(":%s 375 %s :- %s Message of the day -", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, cfg->Get("core.servername", "hamchat.local"));
      llist_t *lp = this->motd_text;
      do {
        if (lp == NULL)
           break;

        if (lp->ptr == NULL)
           continue;

        char *buf = (char *)lp->ptr;
        cptr->Send(":%s 372 %s :%s", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, buf);
        lp = lp->next;
      } while(lp != NULL);
      cptr->Send(":%s 376 %s :End of /MOTD command", cfg->Get("core.servername", "hamchat.local"), cptr->callsign);
   } else {
      cptr->Send(":%s 422 %s :MOTD File is missing", cfg->Get("core.servername", "hamchat.local"), cptr->callsign);
   }
   return true;
}

bool m_motd(Client *cptr, int argc, char **argv) {
   if (motd != NULL)
      motd->Send(cptr);
   return false;
}
