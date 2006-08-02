#ifndef _DHT_KADC_CLIENT_H_
#define _DHT_KADC_CLIENT_H_

#include <utility>
#include <algorithm>
#include <string>
#include <list>
#include <map>

#include "../client.h"
#include "shared_queue.h"
#include "message.h"
#include "observer_info.h"

// TODO these should really be in .cpp so that as little as possible
// of kadc files get included in apps that use dht abstraction
extern "C" {
#include <KadC/int128.h>
#include <KadC/rbt.h>
#include <KadC/MD4.h>
#include <KadC/KadCalloc.h>
#include <KadC/KadClog.h>
#include <KadC/KadCapi.h>
}

namespace dht {
namespace kadc {
	using namespace std;

	class client : public dht::client {
	public:
		typedef shared_queue<message *> message_queue_type;
		friend class state;
		friend class reactor_event_handler;		
	private:
		KadCcontext    _kcc;
		bool           _kstarted;
		string         _init_file;
		addr_inet_type _ext_addr;
		reactor_type  *_reactor;

		size_t _find_threads,
		       _find_duration,
		       _store_threads,
		       _store_duration;
		size_t _find_max_hits;
		       
		typedef map<task *, task *> running_tasks_type;
		typedef list<observer_info> message_obsvs_type;
		
		running_tasks_type _running_tasks;
		message_queue_type _msg_queue;
		message_obsvs_type _msg_observers;

		class reactor_event_handler *_rehandler;
		
		class state *_state;
		int          _state_out;
		
		// Functions that state objects need
		inline void         _kad_context(KadCcontext kcc) { _kcc = kcc; }
		inline KadCcontext *_kad_context() { return &_kcc; }

		// Used privately to set external address
		inline void _external_addr(const addr_inet_type &a) { _ext_addr = a; }

		inline message_queue_type *_message_queue() { return &_msg_queue; }
		// Changes internal state
		void _change_state(class state *s);
		// Changes state that can be queried by application
		void _change_state_out(int t);
		void _task_add(task *t);
		void _quit_all_tasks();
		void _wait_running_tasks();
		void _quit_task(task *t);
		inline int _running_tasks_size() { return _running_tasks.size(); }
		
		void _attach_observer_messages(const observer_info &oi);
		bool _detach_observer_messages(observer_info *oi);

		inline bool _kadc_started() const { return _kstarted; }
		inline void _kadc_started(bool t) { _kstarted = t; }
		
		void _process_queue();
		void _process_msg(message *tm);
	public:

		const static int msg_connect       = 1;
		const static int msg_disconnect    = 2;
		const static int msg_store         = 3;
		const static int msg_search_result = 4;
		const static int msg_search_done   = 5;
		const static int msg_task_exit     = 6;
		
		client();
		virtual ~client();

		inline const char *init_file() { return _init_file.c_str(); }
		// inline message_queue_type &message_queue() { return _msg_queue; }

		// virtual state_type in_state() { return _state_out; };
		
		virtual void init(const name_value_map &opts);
		virtual void deinit();
		
		virtual void connect(notify_handler    *notify = NULL);
		virtual void disconnect(notify_handler *notify = NULL);
		
		virtual void find(const key      &index,
		                  search_handler *handler);

		virtual void store(const key      &index,
		                   const value    &content,
		                   notify_handler *notify = NULL);

		virtual const addr_inet_type &external_addr();

		virtual int process(time_value_type &max_wait);
		virtual int process(time_value_type *max_wait = NULL);
		virtual reactor_type *reactor();
		virtual void          reactor(reactor_type *);
		virtual int handler_cancel(notify_handler *handler);

		// Some function to set/get KadC specific parameters
		// 0 value uses the default parameter on all of these.
		inline size_t find_threads(size_t t) { 
			return _find_threads = std::min<size_t>(t, 20); 
		}
		inline size_t find_threads() const { return _find_threads; }

		inline size_t find_duration(size_t t) {
			return _find_duration = std::min<size_t>(t, 200);
		}
		inline size_t find_duration() const { return _find_duration; }

		inline size_t find_max_hits(size_t t) {
			return _find_max_hits = std::min<size_t>(t, 20);
		}
		
		inline size_t find_max_hits() const { return _find_max_hits; }

		inline size_t store_threads(size_t t) {
			return _store_threads = std::min<size_t>(t, 20);
		}
		inline size_t store_threads() const { return _store_threads; }

		inline size_t store_duration(size_t t) {
			return _store_duration = std::min<size_t>(t, 200);
		}
		inline size_t store_duration() const { return _store_duration; }
		
		/// Sets the file where KadC writes its (debug) logging
		/// By default the logging output is written to stderr.
		/// Throws io_error exception if couldn't open the log file
		static void logfile(const std::string &path);
//		virtual const addr_inet_type *external_addr() = 0;		
	};
		
} // ns kadc
} // ns dht

#endif //_DHT_KADC_CLIENT_H_
