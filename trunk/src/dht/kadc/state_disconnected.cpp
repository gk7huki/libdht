#include <memory>

#include "../exception.h"
#include "state_disconnected.h"
#include "state_disconnecting.h"
#include "state_connecting.h"

using namespace std;

namespace dht  {
namespace kadc {

state_disconnected *state_disconnected::_instance = NULL;

state_disconnected *
state_disconnected::instance() {
	if (!_instance) _instance = new state_disconnected;
	return _instance;
}

void
state_disconnected::connect(node *d, notify_handler *n)
{
	state_connecting::instance()->prepare_connecting(d, n);
	this->change_state(d, state_connecting::instance(), node::connecting);
}

void
state_disconnected::disconnect(node *d, notify_handler *n) {
	if (this->kadc_started(d)) {
		this->change_state(d, state_disconnecting::instance(), node::disconnecting);
		state_disconnecting::instance()->prepare_disconnecting(d, n);
	} else if (n) {
		// TODO 
		throw call_error(
			"disconnect called without matching connect with " \
			"handler: not implemented"
		);
	}
}


} // ns kadc
} // ns dht
