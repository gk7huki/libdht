#ifndef _DHT_VALUE_H_
#define _DHT_VALUE_H_

#include "store_data.h"
#include "name_value_map.h"

namespace dht {
    /**
     * @brief A value in DHT
     * 
     * Represents a value that is stored/obtained to/from DHT with a key.
     * 
     * A value in DHT can, besides the actual value, contain meta data.
     * Not all do, so supporting meta data is optional and implementation
     * specific. If value is passed with meta data there is no guarantee
     * the DHT implementation does anything with it.
     * 
     * By default the DHT is allowed to perform a one way hash to
     * the value data to squeeze it into whatever bit size the DHT
     * supports for values. This does not apply to meta data, which must
     * be retrievable like it was stored or not at all.
     * 
     * @see key, store_data
     */
    
    class value : public store_data {
        name_value_map _meta;
    public:
        inline value();
        inline value(const void *data, size_t len, bool aht = true);
        inline value(const char *str, bool aht = true);
        inline value(const std::string &str, bool aht = true);
        virtual ~value();
        
        inline       name_value_map &meta();
        inline const name_value_map &meta() const;
    };

    inline value::value() 
        : store_data(true) {}
    inline value::value(const void *data, size_t len, bool aht) 
        : store_data(data, len, aht) {}
    inline value::value(const char *str, bool aht) 
        : store_data(str, aht) {}
    inline value::value(const std::string &str, bool aht) 
        : store_data(str, aht) {}

    inline name_value_map &value::meta() {
        return _meta; }

    inline const name_value_map &value::meta() const {
        return _meta; }
        
} // ns dht

#endif //_DHT_VALUE_H_
