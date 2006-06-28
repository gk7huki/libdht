#ifndef _DHT_KADC_STATE_CONNECTED_H_
#define _DHT_KADC_STATE_CONNECTED_H_

#include "state.h"
#include "observer_message.h"

namespace dht {
namespace kadc {
	class state_connected : public state {
		static state_connected *_instance;
	public:
		static state_connected *instance();
		
		virtual void connect(dht *d, notify_handler *n);
		
		// Observer interface
		virtual int received_message(dht *d, message *m);
	protected:
		state_connected() : state("connected") {}
	};
} // ns kadc
} // ns dht

#endif
