#ifndef _DHT_KADC_REACTOR_EVENT_HANDLER_H_
#define _DHT_KADC_REACTOR_EVENT_HANDLER_H_

#include <ace/Event_Handler.h>

namespace dht {
namespace kadc {

// Forward declaration
class node;

class reactor_event_handler : public ACE_Event_Handler {
	class node *_owner;
	
public:
	reactor_event_handler(class node *owner_node);
	virtual ~reactor_event_handler();
	
	void owner(class node *n);
	
	// Called when the node's message queue might have something in it.
	// Wakes up the reactor so that this class will be called again from
	// the reactor thread.
	void signal();
	
	virtual int handle_exception(ACE_HANDLE);
};

} // ns kadc
} // ns dht

#endif //_DHT_KADC_REACTOR_EVENT_HANDLER_H_
