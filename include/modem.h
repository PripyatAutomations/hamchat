#if	!defined(_modem_h)
#define	_modem_h
#include "modem_ardop.h"
extern bool init_modem_thread(void);

enum modem_type_t {
   MODEM_DRIVER_NONE = 0,
   MODEM_DRIVER_ARDOP,
   MODEM_DRIVER_BELL103,
   MODEM_DRIVER_BELL202,
   MODEM_DRIVER_PSK31   
};
typedef enum modem_type_t modem_type_t;

struct modem_type_names {
   modem_type_t         type;
   const char           *name;  
};
typedef struct modem_type_names modem_type_names;

class Modem {
   private:
      modem_type_t modem_type;
      Modem_ARDOP *ardop;
   public:
      Modem(modem_type_t type);
      ~Modem();
};

#endif	// _modem_h
