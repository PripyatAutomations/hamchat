#define	_irc_parser_cpp
#include "hamchat.h"
#include "irc_cmd_table.h"

////////////////////////////////////////////////////////////////////
// Called whenever this->read_waiting is noticed by the main loop //
////////////////////////////////////////////////////////////////////
#define	MAX_ARGS	8
#define	ARG_LEN		512

bool Client::Parse(void) {
   int parc = 0;
   char *parv[MAX_ARGS];

   if (this->sock->recvbuf_lock) {
      Log->Debug("<%d> Client::Parse called while recvbuf locked", this->sock->fd);
      return true;
   }
   // take the lock
   this->sock->recvbuf_lock = true;

//   Log->Debug("<%d> Parsing buffer (%lu bytes): |%s|", this->sock->fd, strlen(this->sock->recvbuf), this->sock->recvbuf);

   // clear pointer array
   memset(parv, 0, sizeof(parv));

   // allocate hostname length in case it's a server
   // XXX: we need to unfuck this...
   parv[0] = (char *)malloc(HOST_NAME_MAX);
   parv[1] = (char *)malloc(CMD_LEN);
   memset(parv[0], 0, HOST_NAME_MAX);
   memset(parv[1], 0, CMD_LEN);

   // Start at the top of the socket's receive buffer.
   char *buf_head = this->sock->recvbuf;
   char *buf = buf_head;
   //
   size_t 	bytes_consumed = 0;		// total bytes consumed from buffer
   bool		has_prefix = false;		// has it a prefix?

   int max_cmds = (sizeof(irc_commands) / sizeof(irc_commands[0]));
   struct irc_cmds *cp = NULL;
   bool valid_cmd = false;
   bool allowed_cmd = true;

   ///////////////////////////////////

   // Deal with stray newlines that get into here... >:(
   while (*buf == '\n' || *buf == '\r' || *buf == ' ')
      buf++;

   // client sent prefix
   if (buf[0] == ':') {
      has_prefix = true;
      // skip over it and count towards our consumed bytes
      buf++;
      bytes_consumed++;
   }

   //////////////////////////////
   // Set parv[0]: Sender name //
   //////////////////////////////
   if (has_prefix) {
      char *np = strchr(buf, ' ');
      if (np != NULL) {
         size_t eaten_bytes = (np - buf);
         *np = '\0';

         if (eaten_bytes > 0) {
            bytes_consumed += eaten_bytes;
         } else {
            Log->Debug("maths are hard? parser eaten_bytes = %lu", eaten_bytes);
            return false;
         }
         parv[0] = (char *)malloc(eaten_bytes);
         memset(parv[0], 0, eaten_bytes);
         memcpy(parv[0], buf, eaten_bytes);
         buf += eaten_bytes;
//         Log->Debug("<%d> parsing prefix for message:  %d: eaten_bytes: %lu <%lu total consumed>", this->sock->fd, parc, eaten_bytes, bytes_consumed);
      }
   } else if (this->callsign[0] != '\0') {
      // use our callsign as it's been validated already
      memcpy(parv[0], this->callsign, strlen(this->callsign));
   }
   parc++;

   // parse the rest of the line
   size_t parv_offset = 0;
   bool final_arg = false;

   // XXX: We need to pull out command and figure out max_args on this one...

   while (buf != NULL) {
      // XXX: this is stupid...
      if (parv[parc] == NULL) {
         // we needn't allocate any more than a single irc line, minus what we've already conusmed...
         parv[parc] = (char *)malloc(512 - bytes_consumed);
         memset(parv[parc], 0, (512 - bytes_consumed));
      }

      if (buf == NULL || *buf == '\0')
         break;

      if (*buf == '\r' || *buf == '\n') {
//        bytes_consumed++;
        break;
      } else if (!final_arg && *buf == ' ') {
        parc++;
        parv_offset = 0;
        //
      } else if (!final_arg && *buf == ':') {
        final_arg = true;
      } else { // copy the byte over
        parv[parc][parv_offset] = *buf;
        parv_offset++;
      }      
      buf++;
      bytes_consumed++;
   }
   ///////////////////////////////////

   if (bytes_consumed > 0) {
      buf = (this->sock->recvbuf + bytes_consumed);
   } else {
      Log->Crit("<%d> bytes_consumed < 0: %d", this->sock->fd, bytes_consumed);
   }

//   Log->Debug("<%d> buf: %x <%d> parc: %d // consumed: %lu", this->sock->fd, buf, strlen(buf), parc, bytes_consumed);

   if (parc <= 1 || parv[1] == NULL) {
      Log->Debug("<%d> Empty command", this->sock->fd);
      goto cleanup;
   }


   for (int i = 0; i < max_cmds; i++) {
      valid_cmd = false;
      cp = &irc_commands[i];

      if (cp->cmd_proto != NULL) {
         if (strcasecmp(cp->cmd_proto, parv[1]) == 0) {
            // does the command require the user to be registered?
            if (cp->registered) {
               if (!IRC_IsActive(this)) {
                  Log->Debug("<%d> Got %s from unregistered user", this->sock->fd, parv[1]);
                  this->Send(":%s 451 %s :You have not registered", cfg->Get("core.servername", "hamchat.local"), (*this->callsign != '\0' ? this->callsign : " *"));
                  valid_cmd = true;
                  allowed_cmd = false;
                  break; // command not valid yet
               }
            }

            // throw an error if not enough arguments
            if ((parc - 1) < cp->min_args) {
               this->Send(":%s 461 %s %s :Not enough parameters", cfg->Get("core.servername", "hamchat.local"), this->callsign, parv[1]);
               valid_cmd = false;
               break;
            }

            // throw an error if too many arguments (this is a parser bug.. let's see if it even matters?)
            if ((parc - 1) > cp->max_args) {
               Log->Crit("<%d> too many parameters (%d > %d) for cmd %s in parser", this->sock->fd, parc, cp->max_args, cp->cmd_proto);
               valid_cmd = false;
               break;
            }

            if (cp->cmd_func != NULL) {			// a valid command was found
               Log->Debug("<%d> matched command %s in entry %d [%x] with handler %x", this->sock->fd, parv[1], i, cp, cp->cmd_func);
               valid_cmd = true;
               break;
            } else {					// command lacks a handler - invalid
               Log->Debug("<%d> matched command %s in entry %d [%x] but it has no handler defined", this->sock->fd, parv[1], i, cp);
               valid_cmd = true;
               cp = NULL;
            }
         }
      }
   }

   // no command was found
   if (!valid_cmd) {
      Log->Debug("<%d> no matching command for %s from client", this->sock->fd, parv[1]);
      this->Send(":%s 421 %s %s :Unknown command", cfg->Get("core.servername", "hamchat.local"), this->callsign, parv[1]);
   } else if (allowed_cmd) {
//      Log->Debug("<%d> parc=%d 0=%s 1=%s 2=%s 3=%s 4=%s", this->sock->fd, parc, (parv[0] != NULL ? parv[0] : "NULL"), (parv[1] != NULL ? parv[1] : "NULL"), (parv[2] != NULL ? parv[2] : "NULL"), (parv[3] != NULL ? parv[3] : "NULL"));
      int rv = cp->cmd_func(this, (parc - 1), parv);
      Log->Debug("<%d> command %s gave result %s", this->sock->fd, parv[1], (rv ? "true" : "false"));
   }


   // move the buffer up as needed
   if (bytes_consumed >= BUFFER_MAX) {
      Log->Debug("--- zeroing sockbuf as full contents were consumed ---");
      memset(this->sock->recvbuf, 0, BUFFER_MAX);
   } else {
      char *move_start = this->sock->recvbuf + bytes_consumed;
      size_t move_len = BUFFER_MAX - bytes_consumed;
      Log->Debug("--- memmove recvbuf contents down --- %x -> %x, %lu bytes (freed %lu bytes)", move_start, this->sock->recvbuf, move_len, bytes_consumed);
      void *rc = memmove(this->sock->recvbuf, move_start, move_len);

      if (rc == NULL)
         Log->Debug("res: %d: %d: %s", rc, errno, strerror(errno));
   }

   this->sock->recvbuf_offset = 0;

cleanup:
   // XXX: free all the pointers (yuck)
   for (int i = 0; i < parc; i++) {
      if (parv[i] != NULL) {
         free(parv[i]);
         parv[i] = NULL;
      }
   }

   // unset the lock
   this->sock->recvbuf_lock = false;
   this->sock->read_waiting = false;

   return true;
}
