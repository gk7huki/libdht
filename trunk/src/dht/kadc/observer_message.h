#ifndef DHT_KADC_OBSERVER_MESSAGE_H_
#define DHT_KADC_OBSERVER_MESSAGE_H_

#include "client.h"
#include "message.h"
#include "observer_info.h"
#include "../notify_handler.h"

namespace dht {
namespace kadc {

    class observer_message {
    public:     
        virtual int received_message(client *, message *m, 
                                     const observer_info &) = 0;
    };
    
} // ns kadc
} // ns dht

#endif //DHT_KADC_OBSERVER_MESSAGE_H_
