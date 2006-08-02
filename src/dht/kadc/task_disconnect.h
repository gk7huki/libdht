#ifndef _DHT_KADC_TASK_DISCONNECT_H_
#define _DHT_KADC_TASK_DISCONNECT_H_

#include "task.h"
#include "client.h"

namespace dht {
namespace kadc {
	class task_disconnect : public task {
		client::message_queue_type *_msg_queue;
		KadCcontext             *_kcc;
	public:
		task_disconnect(client::message_queue_type *q,
		                KadCcontext             *kcc);
		virtual ~task_disconnect();
		
		virtual int svc(void);
	};
	
} // ns kadc
} // ns dht

#endif //_DHT_KADC_TASK_DISCONNECT_H_
