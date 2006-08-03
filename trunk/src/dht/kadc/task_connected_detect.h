#ifndef _DHT_KADC_TASK_CONNECTED_DETECT_H_
#define _DHT_KADC_TASK_CONNECTED_DETECT_H_

#include "task.h"
#include "client.h"

namespace dht {
namespace kadc {
    class task_connected_detect : public task {
        client::message_queue_type *_msg_queue;
        KadCcontext              *_kcc;
        time_value_type           _poll_interval;
        time_value_type           _conn_timeout;
        time_value_type           _node_timeout;
        time_value_type           _abs_conn_timeout;
        time_value_type           _abs_node_timeout;

        time_value_type           _abs_next_info_debug;
        time_value_type           _info_debug_interval;
        
        bool _has_timeouted(message *msg_c, int fwstatus, int nkclients);
    public:
        task_connected_detect(client::message_queue_type *q,
                              KadCcontext             *kcc);
        virtual ~task_connected_detect();
        
        virtual int svc(void);
    };
    
} // ns kadc
} // ns dht

#endif //_DHT_KADC_TASK_CONNECTED_DETECT_H_
