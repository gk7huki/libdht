#include "../exception.h"
#include "state.h"
#include "client.h"
#include "message.h"
#include "message_search.h"

namespace dht {
namespace kadc {

// Id is mainly for inspecting flow of states (debugging)
state::state(const char *id) : _id(id) {}
state::~state() {}

void 
state::store(client *d,
               const key      &index,
               const value    &content,
               notify_handler *notify)
{
    throw call_errorf("dht::kadc::store not connected, current state '%s'",
                     id());         
}

void
state::find(client *d,
            const key      &index,
            search_handler *handler)
{
    throw call_errorf("dht::kadc::find not connected, current state '%s'",
                     id());
}
          

void
state::notify(client *d, const message *m, notify_handler *n) {
    if (n) {
        if (m->success()) {
            ACE_DEBUG((LM_DEBUG, "kadc::notifying handler of success\n"));
            n->success();
        } else {
            ACE_DEBUG((LM_DEBUG, "kadc::notifying handler of failure\n"));
            n->failure(m->code(), m->string());
        }
    }
}

int
state::search_result(client *d, const message *m, notify_handler *h) {
    const message_search *ms = dynamic_cast<const message_search *>(m);
    search_handler       *sh = dynamic_cast<search_handler *>(h);
    
    if (m && !ms) throw unexpected_errorf(
                   "search_result:COULD NOT CAST TO SEARCH MESSAGE %p",
                   m);
    if (h && !sh) throw unexpected_errorf(
                   "search_result:COULD NOT CAST TO SEARCH HANDLER %p",
                   h);
    const key   &k = *(ms->search_key());
    const value &v = *(ms->result_value());
    
    int ret = this->observer_notifier(d)->search_result(k, v);
    if (sh) {
        // The value returned by explicitly set handler always overrides
        // one returned by observer.        
        ACE_DEBUG((LM_DEBUG, "kadc::notifying search handler of result\n"));
        return sh->found(*(ms->search_key()), *(ms->result_value()));
    }
    return ret;
}

void
state::search_done(client *d, const message *m, notify_handler *h) {
    const message_search *ms = dynamic_cast<const message_search *>(m);
    search_handler       *sh = dynamic_cast<search_handler *>(h);
    
    if (m && !ms) throw unexpected_errorf(
                   "search_result:COULD NOT CAST TO SEARCH MESSAGE %p",
                   m);
    if (h && !sh) throw unexpected_errorf(
                   "search_result:COULD NOT CAST TO SEARCH HANDLER %p",
                   h);
    if (sh) {
        if (ms->success()) {
            ACE_DEBUG((LM_DEBUG, "kadc::notifying search handler of success\n"));
            sh->success(*(ms->search_key()));
        } else {
            ACE_DEBUG((LM_DEBUG, "kadc::notifying search handler of failure\n"));
            sh->failure(*(ms->search_key()), m->code(), m->string());
        }
    }
}

} // ns kadc
} // ns dht
