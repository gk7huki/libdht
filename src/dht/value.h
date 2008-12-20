#ifndef DHT_VALUE_H_
#define DHT_VALUE_H_

#include "store_data.h"
#include "name_value_map.h"

namespace dht {
    /**
     * @class value value.h dht/value.h
     * @brief A value in DHT
     * 
     * Represents a value that is stored/obtained to/from DHT with a key.
     * 
     * A value in DHT can, besides the actual value, contain meta data.
     * Not all DHTs do, so supporting meta data is optional and implementation
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
        /**
         * @brief default constructor
         * 
         * Constructs an empty value.
         */    
        inline value();
        /**
         * @brief constructs value from data
         * @param data  pointer to data
         * @param len   data length
         * @param aht   if true a one way hash to the data is allowed
         */        
        inline value(const void *data, size_t len, bool aht = true);
        /**
         * @brief constructs value from C-string
         * @param str   NULL terminated string
         * @param aht   if true a one way hash to the data is allowed
         */
        inline value(const char *str, bool aht = true);
        /**
         * @brief constructs value from string
         * @param str   string
         * @param aht   if true a one way hash to the data is allowed
         */        
        inline value(const std::string &str, bool aht = true);
        virtual ~value();
        
        /**
         * @brief returns reference to the meta data
         * 
         * The meta data is stored as dht::name_value_map
         * structure. The meta values can be set by obtaining
         * the reference with this function to the 
         * name_value_map.
         */
        inline       dht::name_value_map &meta();
        /**
         * @brief returns const reference to the meta data
         * 
         * The meta data is stored as dht::name_value_map
         * structure.
         */
        inline const dht::name_value_map &meta() const;
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

#endif //DHT_VALUE_H_
