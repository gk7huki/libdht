#ifndef _DHT_KADC_SHARED_QUEUE_H_
#define _DHT_KADC_SHARED_QUEUE_H_

#include <ace/Condition_T.h>
#include <ace/Thread_Mutex.h>
#include <queue>

#include "../node.h"
#include "reactor_event_handler.h"

namespace dht {
namespace kadc {
	using namespace std;
	
	template <class T>
	class shared_queue {
		queue<T> _q;
		ACE_Thread_Mutex _m;
		reactor_event_handler *_target;
		
	public:
		shared_queue() {
			// _cond = new ACE_Condition<ACE_Thread_Mutex>(_m);
		}
		~shared_queue() {
			// delete _cond;
		}
		
		inline reactor_event_handler *target()       { return _target; }
		inline void target(reactor_event_handler *t) { _target = t; }
		
		inline int acquire() {
			return _m.acquire();
		}
		inline int release() {
			return _m.release();
		}

		inline void signal() {
			_target->signal();
		}

		inline int size() const       { return _q.size(); }
		inline const T &front() const { return _q.front(); }
		inline void push(const T &e)  { _q.push(e); }
		inline void pop()             { _q.pop(); }
	};		
} // ns kadc
} // ns dht

#endif //_DHT_KADC_SHARED_QUEUE_H_
