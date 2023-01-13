#include "hamchat.h"

// For interoperability, we restrict callsigns/nicks
bool is_valid_callsign(const char *callsign) {
   const char allowed_chars[] = "0123456789-_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
   size_t call_len = -1;

   if (callsign == NULL)
      return false;

   // first character must be alpha, so bail
   if (!isalpha(callsign[0]))
      return false;

   // minimum of 3 chacters (we need the call_len below anyways)
   if ((call_len = strlen(callsign)) <= 3 || (call_len > CALLSIGN_LEN))
      return false;

   // save the value in case we want to do more checks below
   size_t valid_len = strspn(callsign, allowed_chars);

   // if we made it this far, we can accept the id
   if (valid_len == call_len) {
      return true;      
   }
   return false;
}
