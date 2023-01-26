#include "hamchat.h"
//#include <xmlrpc-c/xmlrpc.h>
#include <xmlrpc.h>

//
// Here we provide a connection to fldigi via xml-rpc
//
// This lets you use modes supported by fldigi as transports
//

Modem_FLDIGI::Modem_FLDIGI() {
   xmlrpc_env env;
   xmlrpc_env_init(&env);
   xmlrpc_init(&env);
}

Modem_FLDIGI::~Modem_FLDIGI() {
   xmlrpc_term();
}
