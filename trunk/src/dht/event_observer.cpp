#include "event_observer.h"
#include "client.h"

namespace dht {
    event_observer::event_observer() // : _owner(NULL) 
    {
    }
    
    event_observer::~event_observer() {}
    
    int
    event_observer::state_changed(int) { return 0; }

    int 
    event_observer::search_result(const dht::key &k, const dht::value &v) 
    { 
        return 0; 
    }    
}

