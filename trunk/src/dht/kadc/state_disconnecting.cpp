#include <memory>

#include "../exception.h"
#include "state_disconnecting.h"
#include "state_disconnected.h"
#include "task_disconnect.h"
#include "node.h"

using namespace std;

namespace dht  {
namespace kadc {

state_disconnecting *state_disconnecting::_instance = NULL;

state_disconnecting *
state_disconnecting::instance() {
	if (!_instance) _instance = new state_disconnecting;
	return _instance;
}

void
state_disconnecting::connect(node *d, notify_handler *n) {
	throw call_error("kadc::connect can't connect until disconnect finished!");
}

void
state_disconnecting::disconnect(node *d, notify_handler *n) {
	throw call_error("kadc::connect already disconnecting!");
}

void
state_disconnecting::prepare_disconnecting(node *d, notify_handler *n,
                                           bool no_observer) {
	if (this->running_tasks_size(d) > 0) {
		this->quit_all_tasks(d);
	} else {
		// Start task that handles disconnect
		KadCcontext              *kccptr = this->kad_context(d);
		node::message_queue_type *msg_q  = this->message_queue(d);
		this->task_add(d, new task_disconnect(msg_q, kccptr));
	}

	if (!no_observer)
		this->attach_observer_messages(d, observer_info(this, n));			
}

int
state_disconnecting::received_message(node *d, message *m, 
                                      const observer_info &oi) {
	switch (m->type()) {
	case node::msg_task_exit:
		if (this->running_tasks_size(d) == 0) {
			prepare_disconnecting(d, oi.handler(), true);
		}
		// Continue observing messages (waiting for msg_disconnect now)
		return 0;
	case node::msg_disconnect:
		// Received when disconnect task is finished
		this->change_state(d, state_disconnected::instance(), 
		                   node::disconnected);
		this->notify(d, m, oi.handler());
		this->kadc_started(d, false);
		
		// Remove this observer
		return 1;
	}
	
	return 0;
}


} // ns kadc
} // ns dht
