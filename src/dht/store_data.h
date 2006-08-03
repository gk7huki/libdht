#ifndef _DHT_STORE_DATA_H_
#define _DHT_STORE_DATA_H_

#include "basic_data.h"

namespace dht {
    /**
     * @brief Represents data that is stored to DHT
     * 
     * Common base class to key and value types, not used by itself.
     * 
     * Same as basic_data, but has an extra parameter flag which
     * is used by dht::client implementations to determine if the
     * passed key & value must be taken literally or if a one
     * way hash can be performed on them to squeeze the data into
     * something that fits into the DHT.
     */
    class store_data : public basic_data {
        bool _allow_hash_transform;

        // Since this class is not meant to be used by itself,
        // disallow constructor and destructor except by key and value 
        // classes
        inline store_data(bool aht);
        inline store_data(const void *data, size_t len, bool aht);
        inline store_data(const char *str, bool aht);
        inline store_data(const std::string &str, bool aht);
        inline store_data(const store_data &o);

        virtual ~store_data();
        
        friend class key;
        friend class value;
    public:     
        /**
         * @return  true if the data is allowed to be transformed, false if not.
         */
        inline bool allow_hash_transform() const;
        /**
         * @return  true if the data is allowed to be transformed, false if not.
         */
        inline void allow_hash_transform(bool a);
        
        inline store_data &operator=(const store_data &o);
    };
    
    inline store_data::store_data(bool aht) : basic_data() { 
        _allow_hash_transform = aht;
    }
    
    inline store_data::store_data(const void *data, size_t len, bool aht) 
        : basic_data(data, len) { _allow_hash_transform = aht; }

    inline store_data::store_data(const char *str, bool aht) 
        : basic_data(str) 
    {
        _allow_hash_transform = aht;
    }
        
    inline store_data::store_data(const std::string &str, bool aht) 
        : basic_data(str) 
    {
        _allow_hash_transform = aht;
    }

    inline store_data::store_data(const store_data &o) : basic_data(o) { 
        _allow_hash_transform = o._allow_hash_transform;
    }

    inline bool 
    store_data::allow_hash_transform() const { return _allow_hash_transform; }
    inline void
    store_data::allow_hash_transform(bool a) { _allow_hash_transform = a; }
    
    inline store_data &store_data::operator=(const store_data &o) {
        basic_data::operator=(o);
        _allow_hash_transform = o._allow_hash_transform;
        return *this;
    }
    
    
}

#endif //_VALUE_H_
