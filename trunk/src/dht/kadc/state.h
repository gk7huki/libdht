#ifndef _DHT_KADC_STATE_H_
#define _DHT_KADC_STATE_H_

#include "client.h"
#include "observer_info.h"
#include "../notify_handler.h"

namespace dht {
namespace kadc {
    class state {
        const char *_id;
    protected:
        void change_state(client *d, state *s, int t) {
            d->_change_state(s);
            d->_change_state_out(t);
        }
        void kad_context(client *d, KadCcontext kcc) {
            d->_kad_context(kcc); 
        }
        KadCcontext *kad_context(client *d) {
            return d->_kad_context();
        }

        client::message_queue_type *message_queue(client *d) {
            return d->_message_queue();
        }
        
        void task_add(client *d, task *t) {
            d->_task_add(t);
        }
        void quit_all_tasks(client *d) {
            d->_quit_all_tasks();
        }
        int running_tasks_size(client *d) {
            return d->_running_tasks_size();
        }
         
        void attach_observer_messages(client *d, const observer_info &oi) {
            d->_attach_observer_messages(oi);
        }
        bool detach_observer_messages(client *d, observer_info *oi) {
            return d->_detach_observer_messages(oi);
        }
        
        inline void external_addr(client *d, const addr_inet_type &a) { 
            d->_external_addr(a);
        }

        inline bool kadc_started(client *d) const { 
            return d->_kadc_started();
        }
        inline void kadc_started(client *d, bool t) { 
            return d->_kadc_started(t);
        }
                
        void notify(client *d, const class message *m, notify_handler *n);
        int  search_result(client *d, const class message *m, notify_handler *n);
        void search_done(client *d, const class message *m, notify_handler *n);
        
        state(const char *id = "");
        virtual ~state();
    public:
        virtual void connect(client *d, notify_handler *n) = 0;
        virtual void disconnect(client *d, notify_handler *n) = 0;

        virtual void find(client *d,
                          const key      &index,
                          search_handler *handler);
        
        virtual void store(client *d,
                           const key      &index,
                           const value    &content,
                           notify_handler *notify = NULL);
        
        inline const char *id() const { return _id; }
    };
} // ns kadc
} // ns dht

#endif // _DHT_KADC_STATE_H_
