#ifndef DHT_EVENT_OBSERVER_NOTIFIER_H_
#define DHT_EVENT_OBSERVER_NOTIFIER_H_

#include <map>
#include <list>

#include "event_observer.h"
#include "key.h"
#include "value.h"

namespace dht {
    class event_observer_notifier {
        // typedef std::list<event_observer *>        _observer_list_type;
        typedef std::map<int, event_observer *> _observer_map_type;
        _observer_map_type                      _container;
        
        int _last_received_state;
        
        inline event_observer *_find_obs(int ev_type);
    public:
        event_observer_notifier();
        virtual ~event_observer_notifier();

        void observer_attach(event_observer *h, int event_mask);
        void observer_remove(event_observer *h);
        void observer_remove(event_observer *h, int event_mask);
        
        // So far only state changes, TODO others too
        inline int state_changed(int state);
        inline int search_result(const key &k, const value &v);
        inline int last_received_state() const;
    };

    inline event_observer *
    event_observer_notifier::_find_obs(int ev_type) {
        _observer_map_type::iterator i = _container.find(ev_type);
        if (i != _container.end()) return i->second;
        return NULL;
    }

    inline int 
    event_observer_notifier::state_changed(int state) {
        _last_received_state = state;
        event_observer *o = _find_obs(event_observer::mask_state_changed);
        return (o ? o->state_changed(state) : 0);
    }

    inline int 
    event_observer_notifier::search_result(const key &k, const value &v) {
        event_observer *o = _find_obs(event_observer::mask_search_result);
        return (o ? o->search_result(k, v) : 0);
    }

    inline int
    event_observer_notifier::last_received_state() const {
        return _last_received_state;
    }
} // ns dht

#endif // DHT_EVENT_OBSERVER_NOTIFIER_H_
