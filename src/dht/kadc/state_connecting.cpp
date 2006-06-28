#include <memory>

#include "../exception.h"
#include "state_connecting.h"
#include "state_connected.h"
#include "state_disconnected.h"
#include "state_disconnecting.h"
#include "task_connected_detect.h"
#include "node.h"
#include "util.h"

using namespace std;

namespace dht  {
namespace kadc {

state_connecting *state_connecting::_instance = NULL;

state_connecting *
state_connecting::instance() {
	if (!_instance) _instance = new state_connecting;
	return _instance;
}

void
state_connecting::connect(node *d, notify_handler *n) {
	throw call_error("kadc::connect already connecting!");
}

void
state_connecting::disconnect(node *d, notify_handler *n) {
	observer_info oi(this);
	bool found = this->detach_observer_messages(d, &oi);
	if (found && oi.handler()) 
		oi.handler()->failure(0, "aborted by disconnect");
	
	this->change_state(d, state_disconnecting::instance(), node::disconnecting);
	state_disconnecting::instance()->prepare_disconnecting(d, n);
}

void
state_connecting::prepare_connecting(node *d, notify_handler *n) {
	// For now always passive node
	int passive_mode = 1;
	const char *init_file = d->init_file();
	KadCcontext *kcc = this->kad_context(d);
	*kcc = KadC_start((char *)init_file, passive_mode, 0);
	this->kadc_started(d, true);
	
	if(kcc->s != KADC_OK) {
		throw operation_errorf(
			"KadC_start(%s, %d) returned error %d:\n%s %s",
			init_file, passive_mode, kcc->s, kcc->errmsg1, kcc->errmsg2
		);
	}
	
	// Prepare a new task that detects when connected, and notifies
	// the next state.
	node::message_queue_type *msg_queue = this->message_queue(d);
	
	this->task_add(d, new task_connected_detect(msg_queue, kcc));
	this->attach_observer_messages(d, observer_info(this, n));
}

int
state_connecting::received_message(node *d, message *m, 
                                   const observer_info &oi) 
{
	if (m->type() != node::msg_connect) return 0;
		
	if (m->success()) {
		addr_inet_type ext;
		util::kadc_external_address(&ext, this->kad_context(d));
		this->external_addr(d, ext);
		this->change_state(d, state_connected::instance(), node::connected);
	} else {
		this->change_state(d, state_disconnected::instance(), 
		                   node::disconnected);
	}
	this->notify(d, m, oi.handler());
	
	// Remove observer
	return 1;
}


} // ns kadc
} // ns dht
