#include "hamchat.h"
Database *db = NULL;

Database::~Database() {
 sqlite3_close(this->db);
}

Database::Database(const char *path, const char *schema) {
   int rc;
   struct stat sb;
   bool need_load_schema = false;

   // is this a new database? if so, we need to load schema below
   if (stat(path, &sb) == -1) {
      if (errno == ENOENT) {
         Log->Debug("database %s is new, schema_file %s will be loaded, if present", path, schema);
         need_load_schema = true;
      }
   }
   
   // try to open the database
   if ((rc = sqlite3_open(path, &this->db))) {
      Log->Crit("DB: can't open database %s: %s", path, sqlite3_errmsg(this->db));
      shutdown(200);
   } else {
      Log->Info("DB: opened database %s succesfully!", path);
   }

   if (need_load_schema) { 
      bool load_schema_res = this->LoadSchema(schema);
      Log->Info("Loading schema %s %s", path, (load_schema_res ? "successful" : "failed"));
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
         Log->Crit("sql error loading schema %s: %s", path, zErrMsg);
         sqlite3_free(zErrMsg);
         zErrMsg = NULL;
      } else {
         Log->Info("Database initialized successfully, using schema %s", path);
      }
   } else {
      Log->Crit("failed loading database schema %s: %d (%s)", path, errno, strerror(errno));
   }

   return true;
}
