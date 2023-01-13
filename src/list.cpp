#include "hamchat.h"

// print out the entire list to the debug log
void llist_dump(llist_t *list) {
   struct llist *lp = list;
   Log->Debug("llist START %x", list);

   while (lp != NULL) {
      if (lp->ptr != NULL) {
         Client *cptr = (Client *)lp->ptr;
         Log->Debug("    * @%x: ptr => %x fd => %d", lp, lp->ptr, cptr->sock->fd);
      }
      lp = lp->next;
   }
   Log->Debug("llist END %x", list);
}

// get a pointer to the end of the list, without modifying it
llist_t *llist_find_end(llist_t *list) {
   llist_t *lp = list;

//   Log->Debug("llist_find_end(%x) starting", list);

   if (list == NULL)
      return NULL;

   do {
      if (lp == NULL)
         break;

      if (lp->next) {
         lp = lp->next;
      } else {
         break;
      }
   } while (lp->next);

//   Log->Debug("llist_find_end(%x): returning %x", list, lp);
   return lp;
}

llist_t *llist_append(llist_t *list, void *ptr) {
   llist_t *lp = (struct llist *)malloc(sizeof(struct llist));

   if (lp == NULL) {
      Log->Debug("llist_append: Couldn't allocate memory");
      shutdown(205);
   }

   lp->ptr = ptr;
   lp->next = NULL;

   //
   if (list == NULL) {
//      Log->Debug("llist_append: - this is first item!");
      list = lp;
      return lp;
   }

   // Now that we've succesfully allocated it, let's see if we're the first entry or not
   llist_t *endp = llist_find_end(list);

   // this is broken
   if (endp == NULL) {
      Log->Debug("llist_append: endp == NULL yet we passed list != NULL check above on %x... bailing!", list);
      shutdown(301);
      return lp;
   } else {
      // noisy but helpful
//      Log->Debug("llist_append: appending to list %x after entry %x", list, endp);
   }

   // append ourself to end of list
   endp->next = lp;
   return lp;
}

// Remove a node from the list.
bool llist_remove(llist_t *node, llist_t *list) {
   // we can be called to just free a single node, without modifying the list too..
   if (list == NULL) {
      // Do stuff
      return true;
   }

   // first node
   if (node == list) {
//      Log->Debug("llist_remove: entry %x is only node in the list", node);
      free(node);
      return true;
   }

   // Remove it from the list
   llist_t *lp = list, *pp = NULL;
   do {
      if (lp == NULL)
         break;

      // is this our node?
      if (lp == node) {
         // is a previous node present?
         if (pp != NULL) {
//            Log->Debug("llist_remove: found entry %x in list %x, removing it", node, list);
            pp->next = lp->next;
            free(node);
            return true;
         }
      }

      // pointer juggling!
      pp = lp;
      lp = lp->next;
   } while(lp != NULL);

//   Log->Debug("llist_remove fell through trying to remove %x from %x", node, list);
   return false;
}
