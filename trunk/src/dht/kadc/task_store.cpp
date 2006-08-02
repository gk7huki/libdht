#include <memory>

#include <ace/Guard_T.h>

#include "../exception.h"
#include "task_store.h"
#include "client.h"
#include "util.h"

using namespace std;

namespace dht {
namespace kadc {

task_store::task_store(client *n,
                       client::message_queue_type *q,
                       KadCcontext             *kcc,
                       const key               &pkey,
                       const value             &pvalue,
                       notify_handler          *h) : task("store")
{
	if (!pkey.allow_hash_transform() && pkey.size() > 16)
		throw call_errorf(
			"dht::kadc::store too big a key given " \
			"max key length 16 bytes, given key is %d bytes",
			pkey.size());
	if (!pvalue.allow_hash_transform() && pvalue.size() > 16)
		throw call_errorf(
			"dht::kadc::store too big a value given " \
			"max value length 16 bytes, given value is %d bytes",
			pvalue.size()
		);

	util::kadc_hash(&_index, pkey.data(), pkey.size(), 
	                pkey.allow_hash_transform());
	util::kadc_hash(&_value, pvalue.data(), pvalue.size(), 
	                pvalue.allow_hash_transform());
	util::kadc_meta(&_meta, pvalue.meta());
	
	_client      = n;
	_msg_queue = q;
	_kcc       = kcc;
	_notify    = h;
}

task_store::~task_store() {
}

int 
task_store::svc(void) {
	ACE_TRACE("task_store::svc");
	// Publish message and Task exit message
	auto_ptr<message> msg_p(new message(this, client::msg_store));
	auto_ptr<message> msg_e(new message(this, client::msg_task_exit));
	
	msg_p->handler(_notify);
	
	size_t threads   = _client->store_threads();
	size_t duration  = _client->store_duration();	
	// For some reason KadC_republish does not set the defaults,
	// so do it manually here
	if (threads == 0)  threads = 5;   // 5 threads by default
	if (duration == 0) duration = 15; // 15 secs by default

	ACE_DEBUG((LM_DEBUG, "task_store: store index/value: %s/%s, " 
	                     "threads/duration: %d/%d\n",
	                     _index.c_str(), _value.c_str(),
	                     threads, duration));
	
	int kcs = KadC_republish(_kcc, 
	                         _index.c_str(), 
	                         _value.c_str(), 
	                         // "",
	                         _meta.c_str(),
	                         threads, duration);
	                         
	if (kcs == -1) {
		ACE_DEBUG((LM_DEBUG, "task_store: KadC_republish returned error\n"));

		msg_p->success(false);
		msg_p->code(0);
		msg_p->string("error publishing");
	} else if (kcs == 0) {	
		ACE_DEBUG((LM_DEBUG, "task_store: KadC_republish returned 0 nodes\n"));

		msg_p->success(false);
		msg_p->code(0);
		msg_p->string("0 nodes accepted the stored key/value");
		
	} else {
		ACE_DEBUG((LM_DEBUG, "task_store: store success, number of peer " \
		                     "nodes where value was stored: %d\n", kcs));
		msg_p->success(true);
	}
	
	ACE_DEBUG((LM_DEBUG, "task_store: sending messages\n"));
	
	ACE_Guard<client::message_queue_type> guard_queue(*_msg_queue);	
	_msg_queue->push(msg_p.get()); msg_p.release();
	_msg_queue->push(msg_e.get()); msg_e.release();
	_msg_queue->signal();
	guard_queue.release();

	ACE_DEBUG((LM_DEBUG, "task_store: exiting thread\n"));
	
	return 0;
}

} // ns kadc
} // ns dht
