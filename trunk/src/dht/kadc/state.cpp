#include "../exception.h"
#include "state.h"
#include "node.h"
#include "message.h"
#include "message_search.h"

namespace dht {
namespace kadc {

// Id is mainly for inspecting flow of states (debugging)
state::state(const char *id) : _id(id) {}
state::~state() {}

void 
state::store(node *d,
               const key      &index,
               const value    &content,
               notify_handler *notify)
{
	throw call_errorf("dht::kadc::store not connected, current state '%s'",
	                 id());			
}

void
state::find(node *d,
            const key      &index,
            search_handler *handler)
{
	throw call_errorf("dht::kadc::find not connected, current state '%s'",
	                 id());
}
          

void
state::notify(node *d, const message *m, notify_handler *n) {
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
state::search_result(node *d, const message *m, notify_handler *h) {
	const message_search *ms = dynamic_cast<const message_search *>(m);
	search_handler       *sh = dynamic_cast<search_handler *>(h);
	
	if (m && !ms) throw unexpected_errorf(
	               "search_result:COULD NOT CAST TO SEARCH MESSAGE %p",
	               m);
	if (h && !sh) throw unexpected_errorf(
	               "search_result:COULD NOT CAST TO SEARCH HANDLER %p",
	               h);
	if (sh) {
		ACE_DEBUG((LM_DEBUG, "kadc::notifying search handler of result\n"));
		return sh->found(*(ms->search_key()), *(ms->result_value()));
	}
	return 0;
}

void
state::search_done(node *d, const message *m, notify_handler *h) {
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
