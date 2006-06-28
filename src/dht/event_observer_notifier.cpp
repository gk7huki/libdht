#include "common.h"
#include "event_observer_notifier.h"
#include "exception.h"
#include "node.h"

namespace dht {

event_observer_notifier::event_observer_notifier() 
  : _last_received_state(node::disconnected) 
{
}

event_observer_notifier::~event_observer_notifier() {}

void
event_observer_notifier::observer_attach(event_observer *h, int event_mask)
{
	int pow = 1;
	while (event_mask) {
		if (event_mask & pow) {
			ACE_DEBUG((LM_DEBUG, "group:: adding observer for %d\n", pow));
			if (_container.count(pow))
				throw call_error("Multiple observers not possible currently, " \
				                 "remove old observer first");
			_container[pow] = h;
			event_mask &= ~pow;
		}
		pow <<= 1;
	}
}

void 
event_observer_notifier::observer_remove(event_observer *h)
{
	observer_remove(h, event_observer::mask_all);
}

void 
event_observer_notifier::observer_remove(event_observer *h, int event_mask)
{
	int pow = 1;
	while (event_mask) {
		if (event_mask & pow) {
			if (_container.count(pow) &&
			    _container[pow] == h)
			{
				ACE_DEBUG((LM_DEBUG, "group:: removing observer for %d\n", pow));
				_container.erase(pow);
			}
			event_mask &= ~pow;
		}
		pow <<= 1;		
	}	
}
				
} // ns dht

