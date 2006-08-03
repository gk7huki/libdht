#include <memory>

#include <ace/Guard_T.h>

#include "../exception.h"
#include "task_find.h"
#include "message_search.h"
#include "client.h"
#include "util.h"

using namespace std;

namespace dht {
namespace kadc {

task_find::task_find(client *n,
                     client::message_queue_type *q,
                     KadCcontext             *kcc,
                     const key               &skey,
                     search_handler          *h) : task("search")
{
    if (!skey.allow_hash_transform() && skey.size() > 16)
        throw call_errorf(
            "dht::kadc::search too big a key given " \
            "and hash transform not allowed, max " \
            "length 16 bytes, given key is %d bytes",
            skey.size()
        );
    
    util::kadc_hash(&_index, skey.data(), skey.size(), 
                    skey.allow_hash_transform());
    
    _client      = n;
    _skey      = skey;
    _msg_queue = q;
    _kcc       = kcc;
    _handler   = h;
}

task_find::~task_find() {
}

int
task_find::hit_callback(KadCdictionary *d, void *context) {
    ACE_DEBUG((LM_DEBUG, "task_find::hit_callback"));
    // TODO should check quit signals and return appropriate value
    // to KadC
    try {
        task_find *self = reinterpret_cast<task_find *>(context);
        
        auto_ptr<value>          rvalue(new value);
        auto_ptr<message_search> 
          msg_s(new message_search(self, client::msg_search_result));
    
        util::kadc_result(rvalue.get(), d);
    
        msg_s->success(true);
        msg_s->handler(self->_handler);
        msg_s->search_key(&self->_skey);
        msg_s->result_value(rvalue.release());
    
        ACE_Guard<client::message_queue_type> guard_queue(*(self->_msg_queue));
        self->_msg_queue->push(msg_s.get()); msg_s.release();
        self->_msg_queue->signal();
        guard_queue.release();  
    } catch (...) {
        ACE_DEBUG((LM_CRITICAL, "dht::kadc::task_find::hit_callback FATAL exception throwed\n"));
        throw;
    }
    
    return 0;
}

int 
task_find::svc(void) {
    ACE_TRACE("task_find::svc");

#ifdef DHT_KADC_LOG_NODES
    ACE_DEBUG((DHT_KADC_LOG_NODES, 
              "dht::kadc::task_find: clients %d, contacts %d\n",
              KadC_getnknodes(_kcc),
              KadC_getncontacts(_kcc)));
#endif

#ifndef DHT_KADC_OLD_SEARCH
    // Search done message, Task exit message, search result message in the loop
    auto_ptr<message>        msg_e(new message(this, client::msg_task_exit));
    auto_ptr<message_search> 
      msg_d(new message_search(this, client::msg_search_done));

    msg_d->handler(_handler);
    msg_d->search_key(&_skey);
    msg_d->success(true);
    
    ACE_DEBUG((LM_DEBUG, "task_find: searching index: %s, "
                         "threads/duration/max_hits: %d/%d/%d\n",
                         _index.c_str(),
                         _client->find_threads(),
                         _client->find_duration(),
                         _client->find_max_hits()));

    KadCfind_params fpar;
    KadCfind_init(&fpar);
    fpar.threads  = _client->find_threads();
    fpar.max_hits = _client->find_max_hits();
    fpar.duration = _client->find_duration();
    fpar.hit_callback = task_find::hit_callback;
    fpar.hit_callback_context = reinterpret_cast<void *>(this);

    KadC_find2(_kcc, _index.c_str(), &fpar);

    ACE_DEBUG((LM_DEBUG, "task_find: sending messages\n"));
    
    ACE_Guard<client::message_queue_type> guard_queue(*_msg_queue); 
    _msg_queue->push(msg_d.get()); msg_d.release();
    _msg_queue->push(msg_e.get()); msg_e.release();
    _msg_queue->signal();
    guard_queue.release();

#else
    // Search done message, Task exit message, search result message in the loop
    auto_ptr<message>        msg_e(new message(this, client::msg_task_exit));
    auto_ptr<message_search> 
      msg_d(new message_search(this, client::msg_search_done));

    msg_d->handler(_handler);
    msg_d->search_key(&_skey);
    
    ACE_DEBUG((LM_DEBUG, "task_find: searching index: %s, "
                         "threads/duration/max_hits: %d/%d/%d\n",
                         _index.c_str(),
                         _client->find_threads(),
                         _client->find_duration(),
                         _client->find_max_hits()));


    void *resdictrbt = KadC_find(_kcc, _index.c_str(), "", 
                                 _client->find_threads(),
                                 _client->find_max_hits(),
                                 _client->find_duration());

    try {
        int nhits = rbt_size(resdictrbt);
        void *iter;

        ACE_DEBUG((LM_DEBUG, "task_find: %d hits found\n", nhits));
        // For now don't check quit signals in the middle of this
        // loop since KadC currently returns ALL the results at once
        /* send a message per each found value */
        for (iter = rbt_begin(resdictrbt); iter != NULL; 
             iter = rbt_next(resdictrbt, iter)) 
        {
            auto_ptr<value>          rvalue(new value);
            auto_ptr<message_search> 
              msg_s(new message_search(this, client::msg_search_result));
        
            util::kadc_result(rvalue.get(), rbt_value(iter));

            msg_s->success(true);
            msg_s->handler(_handler);
            msg_s->search_key(&_skey);
            msg_s->result_value(rvalue.release());

            ACE_Guard<client::message_queue_type> guard_queue(*_msg_queue); 
            _msg_queue->push(msg_s.get()); msg_s.release();
            _msg_queue->signal();
            guard_queue.release();          
        }
        util::kadc_free_search_result(resdictrbt);
        
        msg_d->success(true);
    } catch (...) {
        ACE_DEBUG((LM_CRITICAL, "dht::kadc::task_find: FATAL exception throwed\n"));
        // Search results should be freed even in case of a fatal error
        util::kadc_free_search_result(resdictrbt);
        
        throw;
    }

    ACE_DEBUG((LM_DEBUG, "task_find: sending messages\n"));
    
    ACE_Guard<client::message_queue_type> guard_queue(*_msg_queue); 
    _msg_queue->push(msg_d.get()); msg_d.release();
    _msg_queue->push(msg_e.get()); msg_e.release();
    _msg_queue->signal();
    guard_queue.release();
#endif

    ACE_DEBUG((LM_DEBUG, "task_find: exiting thread\n"));
    
    return 0;
}

} // ns kadc
} // ns dht
