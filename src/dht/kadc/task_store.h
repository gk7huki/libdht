#ifndef _DHT_KADC_TASK_STORE_H_
#define _DHT_KADC_TASK_STORE_H_

#include <string>

#include "task.h"
#include "client.h"

namespace dht {
namespace kadc {
    class task_store : public task {
        client                     *_client;
        client::message_queue_type *_msg_queue;
        std::string     _index;
        std::string     _value;
        std::string     _meta;
        notify_handler *_notify;
        KadCcontext    *_kcc;

    public:
        task_store(client *n,
                   client::message_queue_type *q,
                   KadCcontext             *kcc,
                   const key   &pkey,
                   const value &pvalue,
                   notify_handler *h);

        virtual ~task_store();

        virtual int svc(void);
    };

} // ns kadc
} // ns dht

#endif //_DHT_KADC_TASK_STORE_H_
