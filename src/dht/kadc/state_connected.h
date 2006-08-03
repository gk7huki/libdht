#ifndef _DHT_KADC_STATE_CONNECTED_H_
#define _DHT_KADC_STATE_CONNECTED_H_

#include "state.h"
#include "observer_message.h"

namespace dht {
namespace kadc {
    class state_connected : public state, 
                            public observer_message {
        static state_connected *_instance;
    public:
        static state_connected *instance();
        
        virtual void connect(client *d, notify_handler *n);
        virtual void disconnect(client *d, notify_handler *n);
        virtual void find(client *d,
                          const key      &index,
                          search_handler *handler);     
        virtual void store(client *d,
                           const key      &index,
                           const value    &content,
                           notify_handler *notify = NULL);

        // Observer interface
        virtual int received_message(client *d, message *m, 
                                     const observer_info &oi);
    protected:
        state_connected() : state("connected") {}
    };
} // ns kadc
} // ns dht

#endif
