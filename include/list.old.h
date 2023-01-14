#if	!defined(_list_h)
#define	_list_h

struct llist {
   void *ptr;
   struct llist *next;
   unsigned short magic;
};

#define	LIST_MAGIC_NONE		0x0cc0
#define	LIST_MAGIC_CLIENT	0xf001
#define	LIST_MAGIC_CHANNEL	0xc002

typedef struct llist llist_t;

#if	!defined(_list_c)
extern void llist_dump(llist_t *list);
extern llist_t *llist_find_end(llist_t *list);
extern llist_t *llist_append(llist_t *list, void *ptr);
extern bool llist_remove(llist_t *node, llist_t *list);

#endif
#endif	// !defined(_list_h)
