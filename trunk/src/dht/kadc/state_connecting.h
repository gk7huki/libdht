#ifndef _DHT_KADC_STATE_CONNECTING_H_
#define _DHT_KADC_STATE_CONNECTING_H_

#include "state.h"
#include "observer_message.h"

namespace dht {
namespace kadc {
    class state_connecting : public state,
                             public observer_message {
        static state_connecting *_instance;
    public:
        static state_connecting *instance();
        
        virtual void connect(client *d, notify_handler *n);
        virtual void disconnect(client *d, notify_handler *n);

        void prepare_connecting(client *d, notify_handler *n);
        
        // Observer interface
        virtual int received_message(client *d, message *m, 
                                     const observer_info &oi);
    protected:
        state_connecting() : state("connecting") {}
    };
} // ns kadc
} // ns dht

#endif
