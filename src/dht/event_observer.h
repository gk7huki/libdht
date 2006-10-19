#ifndef _DHT_EVENT_OBSERVER_H_
#define _DHT_EVENT_OBSERVER_H_

#include "common.h"
#include "key.h"
#include "value.h"

namespace dht {
    // Forward declaration of dht client
    class client;
    
    class event_observer {
        class client *_owner;
    public:
        event_observer();
        virtual ~event_observer();
        
        enum {
            mask_state_changed = 1 << 0,
            mask_search_result = 1 << 1,
            mask_last          = 1 << 2,
            mask_all           = 1 << 2 - 1,
        };
        
        virtual int state_changed(int)  { return 0; }
        virtual int search_result(const key &k, const value &v) { return 0; }
    };      
}

#endif // _DHT_EVENT_OBSERVER_H_
