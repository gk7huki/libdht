#ifndef _DHT_KADC_TASK_H_
#define _DHT_KADC_TASK_H_

#include <ace/Condition_T.h>
#include <ace/Recursive_Thread_Mutex.h>
#include <ace/Task.h>

namespace dht {
namespace kadc {
    using namespace std;
    
    class task : public ACE_Task_Base {
        volatile bool _quit;
        ACE_Thread_Mutex _m;
        typedef ACE_Condition<ACE_Thread_Mutex> cond_type;
        cond_type *_cond;
        const char *_id;
    public:
        task(const char *id = "");
        virtual ~task();
        
        inline int acquire() { return _m.acquire(); }
        inline int release() { return _m.release(); }
        int wait(const ACE_Time_Value &timeout);
        void quit(bool val);
        bool quit();
        
        inline int join() { return ACE_Task_Base::wait(); }
        inline const char *id() { return _id; }
    };
    
} // ns kadc
} // ns dht

#endif //_DHT_KADC_TASK_H_
