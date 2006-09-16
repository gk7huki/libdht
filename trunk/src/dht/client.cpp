#include "event_observer_notifier.h"
#include "client.h"

namespace dht {

client::client() : _obs_notifier(new event_observer_notifier)
{}

client::~client()   {
    ACE_DEBUG((LM_DEBUG, "dht::client dtor\n"));
}

const char *
client::in_state_str() {
    return state_str(in_state());
}

int 
client::in_state() {
    return observer_notifier()->last_received_state();
}

const char *
client::state_str(int state) {
    switch (in_state()) {
    case disconnected:  return "disconnected";
    case connecting:    return "connecting";
    case connected:     return "connected";
    case disconnecting: return "disconnecting";
    }
    return "<unknown>";
}

void client::observer_attach(event_observer *h,
                            int event_mask)
{
    observer_notifier()->observer_attach(h, event_mask);
}
void client::observer_remove(event_observer *h) {
    observer_notifier()->observer_remove(h);
}
void client::observer_remove(event_observer *h, int event_mask) {
    observer_notifier()->observer_remove(h, event_mask);
}

} // ns dht
