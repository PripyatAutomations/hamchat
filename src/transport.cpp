#include "hamchat.h"

//
// Here we interface with the various loadable transport modules
//

Transport::Transport(enum transport_type transport_type) {
   this->transport_type = transport_type;
}

Transport::~Transport() {
}
