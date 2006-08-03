#include "task.h"

namespace dht {
namespace kadc {

task::task(const char *id) : _quit(false), _id(id) {
    _cond = new cond_type(_m);
    
}
    
task::~task() {
    delete _cond;   
}
    
int 
task::wait(const ACE_Time_Value &timeout) {
    ACE_Time_Value abstime = ACE_OS::gettimeofday() + timeout;
    return _cond->wait(&abstime);
}

void
task::quit(bool val) {
    _quit = val;
    // ACE_DEBUG((LM_DEBUG, "task::quit sending signal to task '%s'", id()));
    // ACE_DEBUG((LM_DEBUG, "task::quit is %d\n", _quit));
    _cond->signal();
}

bool
task::quit() {
    bool ret;
    ret = _quit;
    // ACE_DEBUG((LM_DEBUG, "task::quit is2 %d\n", _quit));
    return ret;
}

} // ns kadc
} // ns dht
