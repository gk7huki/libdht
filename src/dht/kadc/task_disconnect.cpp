#include <memory>

#include <ace/Guard_T.h>

#include "task_disconnect.h"
#include "node.h"

using namespace std;

namespace dht {
namespace kadc {

task_disconnect::task_disconnect(
	node::message_queue_type *q,
	KadCcontext              *kcc) : task("disconnect")
{
	_msg_queue = q;
	_kcc       = kcc;
}

task_disconnect::~task_disconnect() {
}

int 
task_disconnect::svc(void) {
	ACE_TRACE("task_disconnect::svc");
	// Disconnected message and Task exit message
	auto_ptr<message> msg_d(new message(this, node::msg_disconnect));
	auto_ptr<message> msg_e(new message(this, node::msg_task_exit));
	
	ACE_DEBUG((LM_DEBUG, "task_disconnect: calling KadC_stop\n"));
	int kcs = KadC_stop(_kcc);
	if(kcs != KADC_OK) {
		ACE_DEBUG((LM_DEBUG, "task_disconnect: KadC_stop returned " \
		  "error %d:%s %s\n", _kcc->s, _kcc->errmsg1, _kcc->errmsg2));

		msg_d->success(false);
		msg_d->code(0);
		msg_d->string("error stopping KadC engine");
	} else {
		ACE_DEBUG((LM_DEBUG, "kadc_disconnect: success\n"));
		msg_d->success(true);
	}
	
	ACE_DEBUG((LM_DEBUG, "task_disconnect: sending messages\n"));
	
	ACE_Guard<node::message_queue_type> guard_queue(*_msg_queue);	
	_msg_queue->push(msg_d.get()); msg_d.release();
	_msg_queue->push(msg_e.get()); msg_e.release();
	_msg_queue->signal();
	guard_queue.release();

	ACE_DEBUG((LM_DEBUG, "task_disconnect: exiting thread!\n"));
	
	return 0;
}

} // ns kadc
} // ns dht
