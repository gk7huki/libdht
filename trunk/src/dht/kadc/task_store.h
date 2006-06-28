#ifndef _DHT_KADC_TASK_STORE_H_
#define _DHT_KADC_TASK_STORE_H_

#include <string>

#include "task.h"
#include "node.h"

namespace dht {
namespace kadc {
	class task_store : public task {
		node                     *_node;		
		node::message_queue_type *_msg_queue;
		std::string     _index;
		std::string     _value; 
		std::string     _meta;
		notify_handler *_notify;
		KadCcontext    *_kcc;
		
	public:
		task_store(node *n,
		           node::message_queue_type *q,
		           KadCcontext             *kcc,
		           const key   &pkey,
		           const value &pvalue,
		           notify_handler *n);

		virtual ~task_store();
		
		virtual int svc(void);
	};
	
} // ns kadc
} // ns dht

#endif //_DHT_KADC_TASK_STORE_H_
