#ifndef _DHT_KADC_TASK_FIND_H_
#define _DHT_KADC_TASK_FIND_H_

#include <string>

#include "../key.h"
#include "task.h"
#include "client.h"

namespace dht {
namespace kadc {
    using namespace std;
    class task_find : public task {
        client                     *_client;
        client::message_queue_type *_msg_queue;

        key             _skey;      
        string          _index;
        search_handler *_handler;
        KadCcontext    *_kcc;
        
#if 0
        // TODO configurable...
        // KadC parameters, use defaults when applicable
        const static int _nthreads = 0;
        const static int _duration = 0;
        const static int _max_hits = 500;
#endif
    public:
        task_find(client *n,
                  client::message_queue_type *q,
                  KadCcontext             *kcc,
                  const key               &skey,
                  search_handler          *h);

        virtual ~task_find();
        
        virtual int svc(void);
        
        static int hit_callback(KadCdictionary *d, void *context);
    };
    
} // ns kadc
} // ns dht

#endif //_DHT_KADC_TASK_FIND_H_
