#ifndef _DHT_KADC_STATE_DISCONNECTED_H_
#define _DHT_KADC_STATE_DISCONNECTED_H_

#include "state.h"

namespace dht {
namespace kadc {
	class state_disconnected : public state {
		static state_disconnected *_instance;
	public:
		static state_disconnected *instance();
		
		virtual void connect(class client *d, notify_handler *n);
		virtual void disconnect(client *d, notify_handler *n);
	protected:
		state_disconnected() : state("disconnected") {}
	};
} // ns kadc
} // ns dht

#endif
