#ifndef _DHT_EVENT_OBSERVER_H_
#define _DHT_EVENT_OBSERVER_H_

#include "common.h"
#include "key.h"
#include "value.h"

namespace dht {
    // Forward declaration of dht client
    class client;

    /**
     * @class event_observer event_observer.h dht/event_observer.h
     * @brief A parent class for event observer
     * 
     * Each event observer must inherit from this class. Appropriate
     * functions will be called when registered events happen.
     */    
    class event_observer {
        // class client *_owner;
    public:
        event_observer();
        virtual ~event_observer();
        
        enum {
            mask_state_changed = 1 << 0,
            mask_search_result = 1 << 1,
            mask_last          = 1 << 2,
            mask_all           = 1 << 2 - 1,
        };
        
        /**
         * @brief Called when state changed
         * @param s new state of the DHT client
         * @see dht::client for the possible values for state
         * 
         * Always return 0.
         */
        virtual int state_changed(int s);

        /**
         * @brief Called when search result obtained
         * @param k the key that was searched
         * @param v the value that was found
         *
         * Return 0 if you want to continue receiving
         * search results. Return 1 if you wish to not
         * receive anymore search results for the
         * search this was an answer to.
         */
        virtual int search_result(const dht::key &k, const dht::value &v);
    };      
}

#endif // _DHT_EVENT_OBSERVER_H_
