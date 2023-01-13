/*
 * Configuration file loader/saver
 *
 * XXX: Make this deal with misformed and invalid files better
 */
#include "hamchat.h"

Config::~Config() {
   dict_free(this->data);
   fclose(this->fp);
}

bool Config::ParseSection(const char *section) {
   int line = 0, errors = 0;
   char buf[768];
   char *end, *skip,
        *key, *val,
        *this_section = 0;

   // rewind configuration file
   fseek(fp, 0, SEEK_SET);

   do {
      memset(buf, 0, sizeof(buf));
      fgets(buf, sizeof(buf) - 1, this->fp);
      line++;

      // delete prior whitespace...
      skip = buf;
      while(*skip == ' ')
        skip++;

      // Delete trailing newlines
      end = buf + strlen(buf);
      do {
        *end = '\0';
        end--;
      } while(*end == '\r' || *end == '\n');

      // did we eat the whole line?
      if ((end - skip) <= 0)
         continue;

      if ((*skip == '/' && *skip+1 == '/') ||		// comments
           *skip == '#' || *skip == ';')
         continue;
      else if (*skip == '[' && *end == ']') {		// section
         this_section = strndup(skip + 1, strlen(skip) - 2);
//         fprintf(stderr, "[Info]: cfg.section.open: '%s' [%lu]\n", this_section, strlen(this_section));
         continue;
      } else if (*skip == '@') {			// preprocessor
         if (strncasecmp(skip + 1, "if ", 3) == 0) {
            /* XXX: finish this */
         } else if (strncasecmp(skip + 1, "endif", 5) == 0) {
            /* XXX: finish this */
         } else if (strncasecmp(skip + 1, "else ", 5) == 0) {
            /* XXX: finish this */
         }
      }

      // Configuration data *MUST* be inside of a section, no exceptions.
      if (!this_section) {
         fprintf(stderr, "[Debug] config %s has line outside section header at line %d: %s\n", this->file, line, buf);
         errors++;
         continue;
      }

      // skip sections not the one we are looking for
      if (strncasecmp(section, this_section, strlen(section)) != 0) {
         // Do nothing
         continue;
      }

      // Handle configuration sections
      if (strncasecmp(this_section, "general", 7) == 0) {
         // Parse configuration line (XXX: GET RID OF STRTOK!)
         key = strtok(skip, "= \n");
         val = strtok(NULL, "= \n");

         // Store value
         this->Add(key, val);
      } else if (strncasecmp(this_section, "listen", 6) == 0) {
         // create a Listener and append it to Listeners
         create_listener(skip);
      } else if (strncasecmp(this_section, "radio", 5) == 0) {
         int rig_id = atoi(this_section+5);
         if (rig_id < 0 || rig_id > max_rigs) {
            Log->Crit("config_parse(radio): rig_id: %d is out of bounds (0 - %d) at %s:%d", rig_id, max_rigs, this->file, line);
            continue;
         }

         // if this is first entry for the rig, create a new rig object
         if (rigs[rig_id] == NULL) {
            rigs[rig_id] = new Rig();
            rigs[rig_id]->id = rig_id;
            Log->Debug("Allocated new Rig for rig_id %d at %x", rigs[rig_id]->id, rigs[rig_id]);
         }

         key = strtok(skip, "= \n");
         val = strtok(NULL, "= \n");

         // extern struct rig_driver_names rig_driver_names[];
         if (strncasecmp(key, "driver", 6) == 0) {
            // match in rig_driver
            int driver_name_ent = sizeof(struct rig_driver_names)/sizeof(rig_driver_names[0]);
            for (int i = 0; i < driver_name_ent; i++) {
               if (strncasecmp(val, rig_driver_names[i].name, strlen(val)) == 0) {
                  Log->Debug("rdn: %x rdd: %i rrid: %d rrd: %x", rig_driver_names[i].name, rig_driver_names[i].driver, rig_id, rigs[rig_id]->driver);
                  rigs[rig_id]->driver = rig_driver_names[i].driver;
                  Log->Info("<radio%d> set driver: %s (%d)", rig_id, rig_driver_names[i].name, rig_driver_names[i].driver);
                  break;
               } else
                  Log->Debug("skipping %s", rig_driver_names[i].name);
            }
            int driver = rigs[rig_id]->driver;
            Log->Info("<radio%d> set driver: %s (%d)", rig_id, rig_driver_names[driver].driver), driver;
         } else if (strncasecmp(key, "model", 5) == 0) {
            rigs[rig_id]->model = atoi(val);
            Log->Info("<radio%d> set model: %d", rig_id, rigs[rig_id]->model);
         } else if (strncasecmp(key, "enabled", 7) == 0) {
            rigs[rig_id]->Connect();
         }
         
      } else if (strncasecmp(this_section, "modules", 8) == 0) {
         fprintf(stderr, "[Debug] Loading module %s\n", skip);

//         if (module_load(skip) != 0)
//            errors++;
      } else {
         fprintf(stderr, "[Debug] Unknown configuration section '%s' parsing '%s' at %s:%d\n", this_section, buf, this->file, line);
         errors++;
      }
   } while (!feof(this->fp));

//   fprintf(stderr, "cfg <%s> loaded with %d errors from %s\n", section, errors, this->file);
   return true;
}

Config::Config(const char *file) {
   this->data = dict_new();
   memset(this->file, 0, PATH_MAX);
   memcpy(this->file, file, strlen(file));

   if (!(this->fp = fopen(file, "r"))) {
      fprintf(stderr, "[Error] %s: Failed loading %s\n", __FUNCTION__, file);
      fclose(this->fp);
      abort();
   }
   // load the general settings early, so life can be good
   this->ParseSection("general");
}

int Config::Add(const char *key, const char *val) {
   return dict_add(this->data, key, val);
}

int Config::Add(const char *key, const char *val, time_t ts) {
   return dict_add_ts(this->data, key, val, ts);
}

int Config::AddBlob(const char *key, const void **ptr) {
   return dict_add_blob(this->data, key, ptr);
}

int Config::AddBlob(const char *key, const void **ptr, time_t ts) {
   return dict_add_blob_ts(this->data, key, ptr, ts);
}

const char *Config::Get(const char *key, const char *defval) {
   return dict_get(this->data, key, defval);
}

int Config::GetInt(const char *key, const int defval) {
   return dict_getInt(this->data, key, defval);
}

bool Config::GetBool(const char *key, bool defval) {
   return dict_getBool(this->data, key, defval);
}

double Config::GetDouble(const char *key, const double defval) {
   return dict_getDouble(this->data, key, defval);
}

void *Config::GetBlob(const char *key, const void **defval) {
   return dict_get_blob(this->data, key, defval);
}

int Config::RemoveKey(const char *key) {
   return dict_del(this->data, key);
}

int Config::Dump(FILE *out) {
//   return dict_dump(this->data, out);
   return -1;
}
