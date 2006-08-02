#include <ace/Guard_T.h>

#include <list>
#include <algorithm>

#include "../notify_handler.h"
#include "client.h"
#include "observer_message.h"
#include "task_connected_detect.h"
#include "state_disconnected.h"
#include "reactor_event_handler.h"

using namespace std;

namespace dht {
namespace kadc {

client::client()	{
	_state     = state_disconnected::instance();
	_state_out = disconnected;
	_kstarted  = false;
	_kcc.s     = KADC_NEVER_STARTED;
	
	// Use KadC defaults
	_find_threads = _find_duration = 0;
	_find_max_hits = 500;
	_store_threads = _store_duration = 0;
	
	// Unless otherwise instructed, use ACE's system wide reactor
	_reactor   = reactor_type::instance();
	_rehandler = new reactor_event_handler(this);
	_msg_queue.target(_rehandler);
}

client::~client()	{
	ACE_DEBUG((LM_DEBUG, "dht::kadc::client: dtor called\n"));
	// Must wait/kill every thread that is spawned.
	_wait_running_tasks();
	ACE_DEBUG((LM_DEBUG, "dht::kadc::client: dtor deleting reactor event handler\n"));
	delete _rehandler;
}

void
client::init(const name_value_map &opts) {
	ACE_DEBUG((LM_DEBUG, "kadc::init called\n"));
	_init_file = opts.get("init_file");
}

void
client::deinit() {
	ACE_DEBUG((LM_DEBUG, "kadc::deinit called\n"));
	// _quit_and_wait_running_tasks();
	
	if (_kadc_started()) {
		ACE_DEBUG((LM_DEBUG, "kadc::deinit: calling KadC_stop\n"));
		int kcs = KadC_stop(&_kcc);
		if(kcs != KADC_OK) {
			ACE_DEBUG((LM_DEBUG, "kadc::deinit: KadC_stop returned " \
			  "error %d:%s %s\n", _kcc.s, _kcc.errmsg1, _kcc.errmsg2));	
		} else {
			ACE_DEBUG((LM_DEBUG, "kadc::deinit: KadC_stop success\n"));
		}
		_kadc_started(false);
	}		
}

void
client::logfile(const std::string &f) {
	FILE *fp = KadClog_open(const_cast<char *>(f.c_str()));
	if (fp == NULL)
		throw io_errorf("Could not open logfile %s for writing",
		                f.c_str());
}

void
client::_wait_running_tasks() {
	// TODO should use ACE_Thread_Manager to faciliate 
	// timeout on waiting for the threads?
	running_tasks_type::iterator i = _running_tasks.begin();
	for (; i != _running_tasks.end(); i++) {
		task *t = i->first;
		ACE_DEBUG((LM_DEBUG, "dht::kadc::client: dtor waiting for thread %s\n",
		          t->id()));
		t->join();
	}	
}

void
client::connect(notify_handler *notify) {
	ACE_DEBUG((LM_DEBUG, "kadc::connect called\n"));
	_state->connect(this, notify);
}

void
client::disconnect(notify_handler *notify) {
	ACE_DEBUG((LM_DEBUG, "kadc::disconnect called\n"));
	_state->disconnect(this, notify);
}

void
client::store(const key   &index,
              const value &content,
              notify_handler *notify)
{
	ACE_DEBUG((LM_DEBUG, "kadc::store called\n"));
	_state->store(this, index, content, notify);
}

void
client::find(const key      &index,
           search_handler *handler)
{
	ACE_DEBUG((LM_DEBUG, "kadc::find called\n"));
	_state->find(this, index, handler);	
}

const addr_inet_type &
client::external_addr() {
	return _ext_addr;	
}

reactor_type *
client::reactor() {
	return _reactor;
}

void
client::reactor(reactor_type *r) {
	if (r == NULL) {
		throw call_error("dht::kadc::reactor NULL pointer not allowed");
	}
	_reactor = r;
}

int
client::process(time_value_type *max_wait) {
	return _reactor->handle_events(max_wait);
}

int
client::process(time_value_type &max_wait) {
	return _reactor->handle_events(max_wait);
}

int 
client::handler_cancel(notify_handler *handler) {
	// Must go through the observer list and if the handler matches,
	// set it to NULL so that it won't be called.
	ACE_DEBUG((LM_DEBUG, "dht::kadc: handler_cancel\n"));
	int counter = 0;
	message_obsvs_type::iterator obs_i = _msg_observers.begin();
	for (; obs_i != _msg_observers.end(); obs_i++) {
		if (obs_i->handler() == handler) {
			ACE_DEBUG((LM_DEBUG, "dht::kadc: removing handler ptr %d\n",
			          obs_i->handler()));
			obs_i->handler(NULL);
			counter++;
		}
	}

	ACE_DEBUG((LM_DEBUG, "dht::kadc: handler_cancel removed %d handlers\n",
	           counter));
	           
	return counter;
}

void
client::_process_queue() {
	list<message *> rec_msgs;
	list<message *>::iterator i;
	
	ACE_Guard<message_queue_type> guard(_msg_queue); // .acquire();
	while (_msg_queue.size() > 0) {
		ACE_DEBUG((LM_DEBUG, "%t kadc::process_queue purging msg queue (size %d)\n",
		          _msg_queue.size()));
		rec_msgs.push_back(_msg_queue.front());
		_msg_queue.pop();
	}

	guard.release(); // _msg_queue.release();
	
	for (i = rec_msgs.begin(); i != rec_msgs.end(); i++)
		_process_msg(*i);
}

void
client::_process_msg(message *tm) {
	// ACE_DEBUG((LM_DEBUG, "kadc::processing message %d\n", tm->type()));
	switch (tm->type()) {
	case msg_connect:
	case msg_disconnect:
	case msg_store:
	case msg_search_result:
	case msg_search_done:
		break;
	case msg_task_exit:
	{
		task *t = tm->from_task();
		ACE_DEBUG((LM_DEBUG, "kadc::process task exit message from '%s'\n",
		           t->id()));

		_running_tasks.erase(t);
		ACE_DEBUG((LM_DEBUG, "kadc::process remaining running tasks %d\n",
		          _running_tasks.size()));
		ACE_DEBUG((LM_DEBUG, "kadc::process waiting for task to exit\n"));
		t->join();
		ACE_DEBUG((LM_DEBUG, "kadc::process task exited, deleting\n"));
		delete t;
	}
		break;
	default:
		ACE_ERROR((LM_ERROR, "Unrecognized message %d\n", tm->type()));
	}

	// Let each observer process the message
	message_obsvs_type::iterator obs_i = _msg_observers.begin();
	list<message_obsvs_type::iterator> rm_observers;
	for (; obs_i != _msg_observers.end(); obs_i++) {
		if (!obs_i->from_task() || obs_i->from_task() == tm->from_task()) {
			ACE_DEBUG((LM_DEBUG, "kadc::process_msg letting observer process " \
		          "message\n"));		
			if (obs_i->observer()->received_message(this, tm, *obs_i)) {
				// ACE_DEBUG((LM_DEBUG, "kadc::process_msg observer to remove list\n"));			
				rm_observers.push_back(obs_i);
			}
		}
	}

	list<message_obsvs_type::iterator>::iterator rm_i = rm_observers.begin();
	for (; rm_i != rm_observers.end(); rm_i++) {
		_msg_observers.erase(*rm_i);
		ACE_DEBUG((LM_DEBUG, "kadc::process removed observer, size %d\n",
		          _msg_observers.size()));
	}

	ACE_DEBUG((LM_DEBUG, "kadc::process deleting task ptr %d\n", tm));
	delete tm;
	ACE_DEBUG((LM_DEBUG, "kadc::process deleted\n"));
}

void 
client::_change_state(state *s) { 
	ACE_DEBUG((LM_DEBUG, "kadc::change_state new state %s\n", s->id()));
	_state = s; 
}

void 
client::_change_state_out(int t) { 
	_state_out = t;
	this->observer_notifier()->state_changed(t);
}

void
client::_task_add(task *t) {
	_running_tasks[t] = t; // .push_back(t);
	ACE_DEBUG((LM_DEBUG, "kadc::task_add running tasks size %d\n",
	          _running_tasks.size()));
	t->activate();
}

void
client::_quit_all_tasks() {
	running_tasks_type::iterator i = _running_tasks.begin();
	for (; i != _running_tasks.end(); i++)
		_quit_task(i->second);
		
	ACE_DEBUG((LM_DEBUG, "kadc::quit_all_tasks signaled all tasks\n"));	
}

void 
client::_quit_task(task *t) {
	ACE_DEBUG((LM_DEBUG, "kadc::quit_task sending quit signal to task %s\n",
	          t->id()));
	t->acquire();
	t->quit(true);
	t->release();
}

void
client::_attach_observer_messages(const observer_info &oi) {
	_msg_observers.push_back(oi);
	ACE_DEBUG((LM_DEBUG, "kadc::attach_observer_messages number of " \
	          "observers %d\n",
	          _msg_observers.size()));
}

bool
client::_detach_observer_messages(observer_info *oi) {
	ACE_DEBUG((LM_DEBUG, "kadc::detach_observer_messages removing observer\n"));

	message_obsvs_type::iterator i = _msg_observers.begin();
	for (; i != _msg_observers.end(); i++) {
		if (i->observer() == oi->observer()) break;
	}
	if (i != _msg_observers.end()) {
		*oi   = *i;
		_msg_observers.erase(i);
		return true;
	} else {
		ACE_DEBUG((LM_DEBUG, 
		  "kadc::detach_observer_messages observer not found"));
	}

	return false;
}

	} // ns kadc
} // ns dht
