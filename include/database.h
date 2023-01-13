#if	!defined(__db_h)
#define	__db_h

class Database {
   private:
      sqlite3 *db;

   public:
      Database(const char *path, const char *schema);
      bool LoadSchema(const char *path);
};

#endif	// !defined(__db_h)
