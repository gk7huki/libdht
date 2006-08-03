#include "message_search.h"

namespace dht {
namespace kadc {

message_search::~message_search() {
    ACE_DEBUG((LM_DEBUG, "message_search: deleting result value %d\n", _rvalue));
    delete _rvalue;
    ACE_DEBUG((LM_DEBUG, "message_search: deleted\n"));
    // search key is not deleted, since it belongs to task_find and is deleted
    // with it.
}

} // ns kadc
} // ns dht
