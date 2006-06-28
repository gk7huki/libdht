#ifndef _DHT_KADC_STATE_DISCONNECTING_H_
#define _DHT_KADC_STATE_DISCONNECTING_H_

#include "state.h"
#include "observer_message.h"

namespace dht {
namespace kadc {
	class state_disconnecting : public state,
	                            public observer_message {
		static state_disconnecting *_instance;
	public:
		static state_disconnecting *instance();
		
		virtual void connect(node *d, notify_handler *n);
		virtual void disconnect(node *d, notify_handler *n);
		
		// Observer interface
		virtual int received_message(node *d, message *m, 
		                             const observer_info &oi);
	protected:
		state_disconnecting() : state("disconnecting") {}
		
	public:
		void prepare_disconnecting(node *d, notify_handler *n,
		                           bool no_observer = false);
	
	};
} // ns kadc
} // ns dht

#endif
