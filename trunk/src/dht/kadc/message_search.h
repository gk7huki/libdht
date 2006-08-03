#ifndef _DHT_KADC_MESSAGE_SEARCH_H_
#define _DHT_KADC_MESSAGE_SEARCH_H_

#include "../key.h"
#include "../value.h"

#include "message.h"

namespace dht {
namespace kadc {
    class message_search : public message {
        key     *_skey;   // search key
        value   *_rvalue; // result value

    public:
        message_search(task *f, int type) : message(f, type)
        {
            _skey   = NULL;
            _rvalue = NULL;
        }

        virtual ~message_search();

        inline const key  *search_key() const { return _skey; }
        inline void        search_key(key *k) { _skey = k; }

        inline const value *result_value() const   { return _rvalue; }
        inline void         result_value(value *v) { _rvalue = v; }
    };      
} // ns kadc
} // ns dht

#endif //_DHT_KADC_MESSAGE_SEARCH_H_
