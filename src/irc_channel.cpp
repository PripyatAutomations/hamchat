#define	 _irc_channel_cpp
#include "hamchat.h"

//bool    Channels_lock;
llist_t *Channels = NULL;

// constructor for a new channel
Channel::Channel(Client *cptr, const char *name) {
   // copy data into object
   snprintf(this->name, CHAN_LEN, "%s", name);
   this->owner = cptr;
   this->members = llist_append(NULL, (void *)cptr);

   // create/append channel list...
   if (Channels == NULL) {
      Channels = llist_append(NULL, (void *)this);
   } else {
      llist_append(Channels, (void *)this);
   }
}

// look a channel up by name and get it's list pointer (so that it could be deleted, or whatever)
llist_t *find_channel_lp(const char *name) {
   llist_t *lp = Channels;

   // list is empty
   if (lp == NULL) {
      Log->Debug("find_channel: Channels list is NULL looking for %s", name);
      return NULL;
   }

   // walk the list
   while (lp != NULL) {
      Channel *chptr;

      // if there's valid data in the pointer...
      if ((lp != NULL) && (lp->ptr != NULL)) {
         // cast it to channel
         chptr = (Channel *)lp->ptr;

         // if there's no name, this isn't it
         if (chptr->name[0] != '\0') {
            continue;
         }
         if (strcasecmp(chptr->name, name) == 0) {
            Log->Debug("found channel %s, returning lp: %x", chptr->name, lp);
            break;
         }
      }
      lp = lp->next;
   }

   return lp;
}

Channel *find_channel(const char *name) {
   Channel *chptr;
   llist_t *lp = find_channel_lp(name);

   if (lp != NULL && lp->ptr != NULL) {
      chptr = (Channel *)lp->ptr;
      return chptr;
   }

   return NULL;
}

Channel *find_or_add_channel(Client *cptr, const char *name) {
   Channel *chptr = NULL;

   if ((chptr = find_channel(name)) == NULL) {
      chptr = new Channel(cptr, name);
      return chptr;
   }

   return chptr;
}

int count_user_channels(Client *cptr) {
   return -1;
}

// Add a user to the channel
bool Channel::AddUser(Client *cptr) {
   irc_channel_member *cm;

   if (cptr == NULL)
      return false;

   if (this->members != NULL) {
      llist_t *lp = this->members;
      do {
         if (lp == NULL)
            break;

         cm = (irc_channel_member *)lp->ptr;
         if (cm->cptr == cptr) {
            // XXX: Send you're already in channel error...
            return false;
         }

         if (lp->next) {
            lp = lp->next;
         } else {
            break;
         }
      } while (lp);
   }

   // if we made it here, add the user
   if ((cm = (irc_channel_member *)malloc(sizeof(irc_channel_member))) == NULL) {
      Log->Crit("Couldn't allocate memory for irc_channel_member adding %s to %s: %d %s", cptr->callsign, this->name, errno, strerror(errno));
      return false;
   }

   // save time they joined
   cm->join_time = now;

   // create channel's list if it doesn't exist yet
   if (this->members == NULL) {
      this->members = llist_append(NULL, (void *)cm);

      // Give them ops
      cm->has_op = true;
   } else {
      llist_append(this->members, (void *)cm);
   }

   // broadcast the join the channel...
   this->SendToAll(":%s!%s@%s JOIN %s", cptr->callsign, cptr->username, cptr->hostname ,this->name);

   // tell the user what's up with topic
   if (this->topic[0] == '\0') {
      cptr->Send(":%s 331 %s %s :No topic is set", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, this->name);
   } else {
      cptr->Send(":%s 332 %s %s :%s", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, this->name, this->topic);
      cptr->Send(":%s 333 %s %s %s :%lu", cfg->Get("core.servername", "hamchat.local"), cptr->callsign, this->name, this->topic_setby, this->topic_time);
   }

   return true;
}


bool Channel::RemoveUser(Client *cptr, const char *reason) {
   if (cptr == NULL)
      return false;

   if (this->members != NULL) {
      llist_t *lp = this->members;
      do {
        irc_channel_member *mp = (irc_channel_member *)lp->ptr;
        Client *mcptr = mp->cptr;

        if (mcptr != NULL && mcptr == cptr) {
           Log->Debug("<%d> removing from channel %s (%s)", cptr->sock->fd, this->name, reason);
           llist_remove(this->members, lp);
           break;
        }
        lp = lp->next;
      } while (lp != NULL);
   } else {
      Log->Debug("got request to remove %s from %s but they aren't a member!", cptr->callsign, this->name);
      return false;
   }
   return true;
}

bool Channel::KickUser(Client *cptr, Client *kicker, const char *reason) {
   if (cptr == NULL)
      return false;

   this->SendToAllExcept(NULL, ":%s KICK %s %s :%s", (kicker != NULL ? kicker->callsign : cfg->Get("core.servername", "hamchat.local")), this->name, cptr->callsign, reason);
   this->RemoveUser(cptr, reason);
   return true;
}

bool Channel::SendToAll(const char *msg, ...) {
   va_list ap;
   va_start(ap, msg);

   // send it to our generic handler
   this->SendToAllExcept(NULL, msg, ap);
   return true;
}

bool Channel::SendToAllExcept(Client *cptr, const char *msg, ...) {
   va_list ap;
   va_start(ap, msg);

   // send it to our generic handler
   this->SendToAllExcept(cptr, msg, ap);
   return true;
}

bool Channel::SendToAllExcept(Client *cptr, const char *msg, va_list ap) {
   // Implement the actual send to logic
   llist_t *cm = this->members;

   if (cm == NULL) {
      return false;
   }

   do {
     if (cm == NULL)
        break;

     irc_channel_member *mp = (irc_channel_member *)cm->ptr;

     if (mp == NULL || mp->cptr == NULL) {
        Log->Debug("<%s> bailing on null pts", this->name);
        break;
     }
     Client *mcptr = mp->cptr;

     // skip it if it's the Except
     if (mcptr == NULL || mcptr == cptr) {
        Log->Debug("<%s> skipping member %x", this->name, mp);
        continue;
     }

     // if we made it here, send it to this user
     mcptr->Send(msg, ap);

     // continue on with the next member
     cm = cm->next;
   } while (cm != NULL);
   return true;
}

Channel::~Channel() {
}

bool Channel::IsMuted(Client *cptr) {
   llist_t *mp = this->members;

   // can't be an op if channel has no members...
   if (mp == NULL)
      return false;

   do {
      if (mp == NULL || mp->ptr == NULL)
         return false;

      irc_channel_member *mptr = (irc_channel_member *)mp->ptr;

      // we don't care about any user but cptr
      if (mptr->cptr != cptr)
         continue;

      if (mptr->is_muted)
         return true;

      // next!
      mp = mp->next;
   } while(mp != NULL);
   return false;
}

bool Channel::IsOp(Client *cptr) {
   llist_t *mp = this->members;

   // can't be an op if channel has no members...
   if (mp == NULL)
      return false;

   do {
      if (mp == NULL || mp->ptr == NULL)
         return false;

      irc_channel_member *mptr = (irc_channel_member *)mp->ptr;

      // we don't care about any user but cptr
      if (mptr->cptr != cptr)
         continue;

      if (mptr->has_op)
         return true;

      // next!
      mp = mp->next;
   } while(mp != NULL);
   return false;
}

bool Channel::IsVoice(Client *cptr) {
   llist_t *mp = this->members;

   // can't be an op if channel has no members...
   if (mp == NULL)
      return false;

   do {
      if (mp == NULL || mp->ptr == NULL)
         return false;

      irc_channel_member *mptr = (irc_channel_member *)mp->ptr;

      // we don't care about any user but cptr
      if (mptr->cptr != cptr)
         continue;

      if (mptr->has_voice)
         return true;

      // next!
      mp = mp->next;
   } while(mp != NULL);
   return false;
}

bool Channel::IsMember(Client *cptr) {
   llist_t *mp = this->members;

   // can't be an op if channel has no members...
   if (mp == NULL)
      return false;

   do {
      if (mp == NULL || mp->ptr == NULL)
         return false;

      irc_channel_member *mptr = (irc_channel_member *)mp->ptr;

      // we don't care about any user but cptr
      if (mptr->cptr == cptr) {
         return true;
      }

      // next!
      mp = mp->next;
   } while(mp != NULL);
   return false;
}

bool Channel::IsOwner(Client *cptr) {
   if (this->owner == NULL)
      return false;
   if (this->owner == cptr)
      return true;
   return false;
}
