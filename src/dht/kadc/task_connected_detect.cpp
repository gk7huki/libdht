#include <memory>

#include <ace/Guard_T.h>

#include "task_connected_detect.h"
#include "client.h"

using namespace std;

namespace dht {
namespace kadc {

task_connected_detect::task_connected_detect(
	client::message_queue_type *q,
	KadCcontext             *kcc) : task("connected_detect")
{
	_msg_queue = q;
	_kcc       = kcc;
	
	// 0.5 second poll interval
	// 2m00s connection timeout
	// 0m10s client timeout, which is started when first node is
	//       contacted
	_poll_interval = time_value_type(0,  5 * 100000);
	_conn_timeout  = time_value_type(120);
	_node_timeout  = time_value_type(10);
	
	_info_debug_interval = time_value_type(10);
}

task_connected_detect::~task_connected_detect() {
}

int 
task_connected_detect::svc(void) {
	ACE_TRACE("task_connected_detect::svc");
	// Connected message and Task exit message
	auto_ptr<message> msg_c(new message(this, client::msg_connect));
	auto_ptr<message> msg_e(new message(this, client::msg_task_exit));

	ACE_DEBUG((LM_DEBUG, "task_connected_detect: polling when connected\n"));
	ACE_Guard<task_connected_detect> guard(*this);
	
	int last_fwstatus = 0;
	int last_nknodes  = 0;

	// Set absolute timeout
	_abs_conn_timeout    = ACE_OS::gettimeofday() + _conn_timeout;
	_abs_next_info_debug = ACE_OS::gettimeofday() + _info_debug_interval;
	// this->acquire();
	while (1) {
		if (this->quit()) {
			ACE_DEBUG((LM_DEBUG, "task_connected_detect: quit detected\n"));
			msg_c->success(false);
			// TODO maybe a proper code
			msg_c->code(0);
			msg_c->string("Connecting aborted");
			break;
		}
		int fwstatus = KadC_getfwstatus(_kcc);
		int nknodes  = KadC_getnknodes(_kcc);
		int ncontact = KadC_getncontacts(_kcc);
		
		if (_abs_next_info_debug < ACE_OS::gettimeofday()) {
			_abs_next_info_debug = ACE_OS::gettimeofday() + _info_debug_interval;
			ACE_DEBUG((LM_INFO, 
			  "task_connected_detect: firewall status %d, nodes %d, contacts %d\n",
			  fwstatus, nknodes, ncontact));
		}
					
		if (fwstatus != last_fwstatus) {
			ACE_DEBUG((LM_DEBUG, "task_connected_detect: firewall status detected " \
			          "(%d)\n", fwstatus));
			last_fwstatus = fwstatus;
		}
		if (nknodes != last_nknodes) {
			ACE_DEBUG((LM_DEBUG, "task_connected_detect: nodes/contacts %d/%d\n",
			           nknodes, ncontact));
			last_nknodes = nknodes;
		}
		
		// TODO configurable? Timeout for connect? Timeout if nodes don't
		// grow beyond this limit?
		// Make this reasonably big so that usually KadC library gets
		// at least node timeout amount of time to get ready for 
		// finds/stores.
		if (nknodes >= 20) {
			ACE_DEBUG((LM_DEBUG, "task_connected_detect: connection detected\n"));
			
			msg_c->success(true);			
			break;
		}

		// Timeout handling
		if (_has_timeouted(msg_c.get(), fwstatus, nknodes))
			break;
			
		// ACE_DEBUG((LM_DEBUG, "task_connected_detect: sleeping on cond var\n"));
		this->wait(_poll_interval);
	}
	
	guard.release();
	
	ACE_DEBUG((LM_DEBUG, "task_connected_detect: sending messages\n"));
	
	ACE_Guard<client::message_queue_type> guard_queue(*_msg_queue);	
	_msg_queue->push(msg_c.get()); msg_c.release();
	_msg_queue->push(msg_e.get()); msg_e.release();
	_msg_queue->signal();
	guard_queue.release();

	ACE_DEBUG((LM_DEBUG, "task_connected_detect: exiting thread\n"));
	
	return 0;
}

bool
task_connected_detect::_has_timeouted(message *msg_c, int fwstatus, int nknodes)
{
	time_value_type now = ACE_OS::gettimeofday();
	
	if (_abs_conn_timeout < now) {
		ACE_DEBUG((LM_DEBUG, "task_connected_detect: timeout\n"));
		// If there are at least some nodes, then return with success
		if (nknodes > 0) {
			ACE_DEBUG((LM_DEBUG, 
			  "task_connected_detect: timeout reached but %d nodes has been " \
			  "found, returning success\n", nknodes));
			msg_c->success(true);			
			return true;
		} else {
			ACE_DEBUG((LM_DEBUG, "task_connected_detect: connection timeout " \
			  "0 contact nodes found\n"));
			msg_c->success(false);
			// TODO maybe a proper code
			msg_c->code(0);
			msg_c->string("Connecting timeouted");
			return true;
		}
	}
	
	if (nknodes > 0 && _abs_node_timeout == time_value_type::zero) {
		ACE_DEBUG((LM_DEBUG, "task_connected_detect: node timeout set\n"));		
		_abs_node_timeout = now + _node_timeout;
		return false;
	}
	
	if (_abs_node_timeout != time_value_type::zero &&
	    _abs_node_timeout < now)
	{
		ACE_DEBUG((LM_DEBUG, 
		  "task_connected_detect: node timeout reached with %d nodes " \
		  "found, returning success\n", nknodes));
		msg_c->success(true);
		return true;
	}
	
	return false;		
}
	
} // ns kadc
} // ns dht
