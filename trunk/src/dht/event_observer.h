#ifndef _DHT_EVENT_OBSERVER_H_
#define _DHT_EVENT_OBSERVER_H_

#include "common.h"

namespace dht {
	// Forward declaration of dht node
	class node;
	
	class event_observer {
		class node *_owner;
	public:
		event_observer();
		virtual ~event_observer();
		
		enum {
			mask_state_changed = 0x1,
			
			mask_all           = 0x1,
			mask_last          = 0x2,
		};
				
		virtual int state_changed(int)  { return 0; }
	};		
}

#endif // _DHT_EVENT_OBSERVER_H_
