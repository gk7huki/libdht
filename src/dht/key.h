#ifndef _DHT_KEY_H_
#define _DHT_KEY_H_

#include "store_data.h"

namespace dht {
    /**
     * @brief A DHT key
     * 
     * Represents a key that is used to index a value in DHT.
     * 
     * By default the DHT is allowed to perform a one way hash to
     * the key data to squeeze it into whatever bit size the DHT
     * supports for keys.
     * 
     * @see value, store_data
     */
    class key : public store_data {
    public:
        inline key();
        // inline key(bool aht); //  = true);
        inline key(const void *data, size_t len, bool aht = true);
        inline key(const char   *str, bool aht = true);
        inline key(const std::string &str, bool aht = true);
        virtual ~key();
    };

    inline key::key() 
        : store_data(true) {}   

/*  inline key::key(bool aht) 
        : store_data(aht) {
        ACE_DEBUG((LM_DEBUG, "key::constructor aht\n"));
        }   */
    inline key::key(const void *data, size_t len, bool aht) 
        : store_data(data, len, aht) {}

    inline key::key(const char *str, bool aht) 
        : store_data(str, aht) {}

    inline key::key(const std::string &str, bool aht) 
        : store_data(str, aht) {}

}

#endif //_KEY_H_
