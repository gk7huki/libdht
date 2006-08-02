#include "reactor_event_handler.h"
#include "client.h"

namespace dht {
namespace kadc {

reactor_event_handler::reactor_event_handler(client *owner_client) {
	owner(owner_client);
}

reactor_event_handler::~reactor_event_handler() {}

void
reactor_event_handler::owner(client *n) {
	_owner = n;
}

void
reactor_event_handler::signal() {
	ACE_DEBUG((LM_DEBUG, 
	          "%t reactor_event_handler::signal called, owner: %d\n",
	          _owner));
	ACE_DEBUG((LM_DEBUG, 
	          "%t reactor_event_handler::signal called, reactor: %d\n",
	          _owner->reactor()));
	_owner->reactor()->notify(this);
}
	
int
reactor_event_handler::handle_exception(ACE_HANDLE) {
	ACE_DEBUG((LM_DEBUG, "%t reactor_event_handler::handle_exception called\n"));
	_owner->_process_queue();
	return 0;
}

} // ns kadc
} // ns dht

