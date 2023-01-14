#include "hamchat.h"

// A place to store pointers to all databases
dict *Databases = NULL;

Database::~Database() {
 sqlite3_close(this->db);
}

Database::Database(const char *name) {
   int rc;
   struct stat sb;
   bool need_load_schema = false;

   // XXX: Get schema and db paths from config
   char dbpath[40], schemapath[60];
   memset(dbpath, 0, 40);
   memset(schemapath, 0, 40);
   snprintf(dbpath, 60, "path.db_%s", name);
   snprintf(schemapath, 59, "path.db_%s_schema", name);
   
   const char *path = cfg->Get(dbpath, "NULL");
   const char *schema = cfg->Get(schemapath, "NULL");

   if (path == NULL) {
      Log->Send(LOG_CRIT, "DB<%s> loading database failed", name);
      abort();
   }

   // is this a new database? if so, we need to load schema below
   if (stat(path, &sb) == -1) {
      if (errno == ENOENT) {
         Log->Send(LOG_DEBUG, "DB<%s>: database %s is new, schema_file %s will be loaded, if present", name, path, schema);
         need_load_schema = true;
      }
   }
   
   // try to open the database
   if ((rc = sqlite3_open(path, &this->db))) {
      Log->Send(LOG_CRIT, "DB<%s>: can't open database %s: %s", name, path, sqlite3_errmsg(this->db));
      shutdown(200);
   } else {
      Log->Send(LOG_INFO, "DB<%s>: opened database %s succesfully!", name, path);
   }

   if (need_load_schema) { 
      bool load_schema_res = this->LoadSchema(schema);
      Log->Send(LOG_INFO, "DB<%s>: Loading schema %s %s", name, path, (load_schema_res ? "successful" : "failed"));
   }
}

static int load_schema_callback(void *NA, int argc, char **argv, char **azColName) {
   int i;
   for(i = 0; i<argc; i++) {
      printf("LoadSchema: %s = %s\n", azColName[i], argv[i] ? argv[i] : "NULL");
   }
   printf("\n");
   return 0;
}

bool Database::LoadSchema(const char *path) {
   struct stat sb;

   if (stat(path, &sb) == 0) {
      // Load the schema
      char sql[PATH_MAX+9];
      char *zErrMsg = NULL;
      memset(sql, 0, PATH_MAX+9);
      snprintf(sql, PATH_MAX+9, ".import %s\n", path);
      int rc = sqlite3_exec(db, sql, load_schema_callback, 0, &zErrMsg);

      if (rc != SQLITE_OK) {
         Log->Send(LOG_CRIT, "sql error loading schema %s: %s", path, zErrMsg);
         sqlite3_free(zErrMsg);
         zErrMsg = NULL;
      } else {
         Log->Send(LOG_INFO, "Database initialized successfully, using schema %s", path);
      }
   } else {
      Log->Send(LOG_CRIT, "failed loading database schema %s: %d (%s)", path, errno, strerror(errno));
   }

   return true;
}
