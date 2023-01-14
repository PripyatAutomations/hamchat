#define	 _irc_channel_cpp
#include "hamchat.h"
////////////////////////////////////////////////////////////
// In memory representation of all of our active channels //
////////////////////////////////////////////////////////////
// Here we store a dictionary of all the channels known to us
dict *Channels = NULL;

// Someday we'll implement dumping the channel db to sql (and loading from it!)
bool Channel::DumpToDb(Database *db, const char *section) {
   struct stat sb;
   FILE *fp;
   const char *path = cfg->Get("path.channel_dump", NULL);
   bool keep_old_chan_stats = cfg->GetBool("chan.dump.keep_old", false);

   if (path == NULL)
      return true;

   if (!keep_old_chan_stats && stat(path, &sb) == 0) {
      // statistics file already exists, try to unlink it
      if (unlink(path)) {
         Log->Send(LOG_WARNING, "failed removing old channels dump file %s: %d (%s)", path, errno, strerror(errno));
         return false;
      }
   }

   if ((fp = fopen(path, "a+")) == NULL) {
      Log->Send(LOG_WARNING, "failed opening channels dump file %s: %d (%s)", path, errno, strerror(errno));
      return false;
   }

   // from dict_dump();
   const char *key;
   const void *val;
   int    rank = 0;
   int    errors = 0;
   time_t ts = 0;

   if (!Channels || !fp)
      return false;

   while (true) {
      rank = dict_enumerate_blob(Channels, rank, &key, &val, &ts);

      if (rank < 0)
         break;

      Channel *chptr = (Channel *)val;
      // emit output here in CSV format
      fprintf(fp, "%s\n", chptr->name);
   }

   // XXX: Import the CSV into the sqlite db as needed
   // XXX: fseek(fp, 0, SEEK_SET);
   // XXX: scan into sqlite   
   fflush(fp);
   fclose(fp);
   return true;
}

bool load_channels_from_db(Database *db, const char *table) {
   return false;
}

Channel::Channel(Client *cptr, const char *name) {
   if (Channels == NULL)
      Channels = dict_new();

   snprintf(this->name, CHAN_LEN, "%s", name);
   this->owner = cptr;
   this->members = dict_new();

   // Add ourself to the Channels list
   dict_add_blob(Channels, this->name, (void *)this);
}

Channel::~Channel() {
   dict_free(this->members);
   // Remove ourself from the Channels list
   dict_del(Channels, this->name);
}

// Find a channel by name
Channel *find_channel(const char *name) {
   void *ptr = NULL;
   Channel *chptr = NULL;

   if ((ptr = dict_get_blob(Channels, name, NULL)) != NULL) {
      chptr = (Channel *)ptr;
      Log->Send(LOG_DEBUG, "found channel %s, returning chptr: %x", chptr->name, chptr);
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

   void *vp;
   if ((vp = dict_get_blob(this->members, cptr->GetCallsign(), NULL)) != NULL) {
      cm = (irc_channel_member *)vp;

      if (strcasecmp(cm->cptr->GetCallsign(), cptr->GetCallsign()) == 0) {
         Log->Send(LOG_DEBUG, "user %s is already in channel %s", cptr->GetCallsign(), this->name);
         // XXX: Send already in channel numeric
         return false;
      }
   }

   // if we made it here, add the user
   if ((cm = (irc_channel_member *)malloc(sizeof(irc_channel_member))) == NULL) {
      Log->Send(LOG_CRIT, "Couldn't allocate memory for irc_channel_member adding %s to %s: %d %s", cptr->GetCallsign(), this->name, errno, strerror(errno));
      return false;
   }

   // save time they joined
   cm->join_time = now;

   // add to member list
   dict_add_blob(this->members, cptr->GetCallsign(), cm);

   // broadcast the join the channel...
   this->SendToAll(":%s!%s@%s JOIN %s", cptr->GetCallsign(), cptr->username, cptr->hostname ,this->name);

   // tell the user what's up with topic
   if (this->topic[0] == '\0') {
      cptr->Send(":%s 331 %s %s :No topic is set", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign(), this->name);
   } else {
      cptr->Send(":%s 332 %s %s :%s", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign(), this->name, this->topic);
      cptr->Send(":%s 333 %s %s %s :%lu", cfg->Get("core.servername", "hamchat.local"), cptr->GetCallsign(), this->name, this->topic_setby, this->topic_time);
   }
   return true;
}


bool Channel::RemoveUser(Client *cptr, const char *reason) {
   if (cptr == NULL)
      return false;

   dict_del(this->members, cptr->GetCallsign());
   Log->Send(LOG_DEBUG, "<%d> removing from channel %s (%s)", cptr->sock->fd, this->name, reason);

   return true;
}

bool Channel::KickUser(Client *cptr, Client *kicker, const char *reason) {
   if (cptr == NULL)
      return false;

   this->SendToAllExcept(NULL, ":%s KICK %s %s :%s", (kicker != NULL ? kicker->GetCallsign() : cfg->Get("core.servername", "hamchat.local")), this->name, cptr->GetCallsign(), reason);
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
   // XXX: Iterate over the whole this->members dictionary
//     mcptr->Send(msg, ap);
   return true;
}


bool Channel::IsMuted(Client *cptr) {
   irc_channel_member *mp = NULL;

   if ((mp = (irc_channel_member *)dict_get_blob(this->members, cptr->GetCallsign(), NULL)) == NULL) {
      return false;
   }

   if (mp->is_muted)
      return true;

   return false;
}

bool Channel::IsOp(Client *cptr) {
   irc_channel_member *mp = NULL;

   if ((mp = (irc_channel_member *)dict_get_blob(this->members, cptr->GetCallsign(), NULL)) == NULL) {
      return false;
   }

   if (mp->has_op)
      return true;

   return false;
}

bool Channel::IsVoice(Client *cptr) {
   irc_channel_member *mp = NULL;

   if ((mp = (irc_channel_member *)dict_get_blob(this->members, cptr->GetCallsign(), NULL)) == NULL) {
      return false;
   }

   if (mp->has_voice)
      return true;

   return false;
}

bool Channel::IsMember(Client *cptr) {
   irc_channel_member *mp = NULL;

   if ((mp = (irc_channel_member *)dict_get_blob(this->members, cptr->GetCallsign(), NULL)) == NULL) {
      return false;
   }

   return true;
}

bool Channel::IsOwner(Client *cptr) {
   if (this->owner == NULL)
      return false;
   if (this->owner == cptr)
      return true;
   return false;
}
