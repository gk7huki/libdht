#ifndef DHT_KADC_MESSAGE_H_
#define DHT_KADC_MESSAGE_H_

#include "../notify_handler.h"
#include "task.h"

namespace dht {
namespace kadc {
    class message {
        task           *_from;
        int             _msg_type;
        int             _code;
        const char     *_string;
        notify_handler *_handler;
        bool            _success;
    public:
        message(task *f, int type) : 
            _from(f),
            _msg_type(type),
            _code(0),
            _string(NULL),
            _handler(NULL),
            _success(false)
        {
        }

        virtual ~message();
        
        inline int  type() const { return _msg_type; }
        inline void type(int t)  { _msg_type = t; }

        inline task *from_task() const { return _from; }
        
        inline bool success() const { return _success; }
        inline void success(bool s) { _success = s; }

        inline notify_handler *handler() const            { return _handler; }
        inline void            handler(notify_handler *h) { _handler = h; }

        inline int  code() const { return _code; }
        inline void code(int c)  { _code = c; }
        
        inline const char *string() const { return _string; }
        inline void string(const char *s) { _string = s; }
    };      
} // ns kadc
} // ns dht

#endif //DHT_KADC_MESSAGE_H_
