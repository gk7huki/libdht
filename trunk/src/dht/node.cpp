#include "event_observer_notifier.h"
#include "node.h"

namespace dht {

node::node() : _obs_notifier(new event_observer_notifier)
{}

node::~node()	{
	ACE_DEBUG((LM_DEBUG, "dht::node dtor\n"));
}

const char *
node::in_state_str() {
	return state_str(in_state());
}

int 
node::in_state() {
	return observer_notifier()->last_received_state();
}

const char *
node::state_str(int state) {
	switch (in_state()) {
	case disconnected:  return "disconnected";
	case connecting:    return "connecting";
	case connected:	    return "connected";
	case disconnecting:	return "disconnecting";
	}
	return "<unknown>";
}

void node::observer_attach(event_observer *h,
                            int event_mask)
{
	observer_notifier()->observer_attach(h, event_mask);
}
void node::observer_remove(event_observer *h) {
	observer_notifier()->observer_remove(h);
}
void node::observer_remove(event_observer *h, int event_mask) {
	observer_notifier()->observer_remove(h, event_mask);
}

} // ns dht
