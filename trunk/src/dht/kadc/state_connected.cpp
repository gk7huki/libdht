#include <memory>

#include "../exception.h"
#include "state_connected.h"
#include "state_disconnecting.h"
#include "task_store.h"
#include "task_find.h"
#include "client.h"

using namespace std;

namespace dht  {
namespace kadc {

state_connected *state_connected::_instance = NULL;

state_connected *
state_connected::instance() {
    if (!_instance) _instance = new state_connected;
    return _instance;
}

void
state_connected::connect(client *d, notify_handler *n) {
    throw call_error("kadc::connect already connected!");
}

void
state_connected::disconnect(client *d, notify_handler *n) {
    this->change_state(d, state_disconnecting::instance(), client::disconnecting);
    state_disconnecting::instance()->prepare_disconnecting(d, n);
}

void 
state_connected::store(client *d,
                       const key      &index,
                       const value    &content,
                       notify_handler *n)
{
    // Start task that handles storeing
    KadCcontext              *kccptr = this->kad_context(d);
    client::message_queue_type *msg_q  = this->message_queue(d);
    task *t = new task_store(d, msg_q, kccptr, index, content, n);  
    this->task_add(d, t);
    if (n) this->attach_observer_messages(d, observer_info(this, n, t));
}

void 
state_connected::find(client *d,
                      const key      &index,
                      search_handler *handler)
{
    // Start task that handles searching
    KadCcontext              *kccptr = this->kad_context(d);
    client::message_queue_type *msg_q  = this->message_queue(d);
    task *t = new task_find(d, msg_q, kccptr, index, handler);
    this->task_add(d, t);
    if (handler) 
        this->attach_observer_messages(d, observer_info(this, handler, t));
    
}                      

int
state_connected::received_message(client *d, message *m, const observer_info &oi) {
    switch (m->type()) {
    case client::msg_store:
        // Received when storing of value finished
        this->notify(d, m, oi.handler());
            // Remove this observer
        return 1;
    case client::msg_search_result:
        // Received when one result for a search is obtained
        // The handler might request no more results to be delivered
        return this->search_result(d, m, oi.handler());
    case client::msg_search_done:
        // Received when search is finished
        this->search_done(d, m, oi.handler());
        // Remove this observer
        return 1;
    }
    
    return 0;
}

} // ns kadc
} // ns dht
