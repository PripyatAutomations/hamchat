#if	!defined(_file_h)
#define	_file_h

extern int is_link(const char *path);
extern int is_dir(const char *path);
extern int is_file(const char *path);
extern int is_fifo(const char *path);
#endif // !defined(_file_h)
