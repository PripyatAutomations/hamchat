#include "hamchat.h"

llist_t *Modules = NULL;

Module::Module(const char *path){
   void *h = NULL;

   if ((h = dlopen(path, RTLD_LAZY|RTLD_GLOBAL|RTLD_DEEPBIND)) != NULL) {
      this->hndl = h;
      this->loaded = true;
   } else {
      int dlerrno = errno;
      // Handle errors
      Log->Crit("module::Init couldn't load module %s (%d: %s)", path, dlerrno, dlerror());
   }

   if (Modules == NULL) {
      Modules = llist_append(NULL, (void *)this);
   } else {
      llist_append(Modules, (void *)this);
   }
}

int Module::Unload(void) {
   return 0;
}

// Load all modules
bool load_modules(void) {
   // if it's on the noload list, skip it
   DIR *dh;
   struct dirent *de;
   
   if ((dh = opendir(cfg->modpath))) {
      Log->Crit("load_modules failed opening modpath %s: %d (%s)", cfg->modpath, errno, strerror(errno));
      shutdown(255);
   }

   char modpath[PATH_MAX];

   while ((de = readdir(dh)) != NULL) {
      // skip anything starting with a dot...
      if (de->d_name[0] != '.') {
         memset(modpath, 0, PATH_MAX);
         snprintf(modpath, PATH_MAX - 1, "%s/%s", cfg->modpath, de->d_name);
         Module *mod = new Module(modpath);
      } else {
         Log->Debug("load_modules: skipping %s", modpath);
      }
   }

   // close the directory handle
   closedir(dh);
   return true;
}
