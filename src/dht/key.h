#ifndef DHT_KEY_H_
#define DHT_KEY_H_

#include "store_data.h"

namespace dht {
    /**
     * @class key key.h dht/key.h
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
        /**
         * @brief default constructor
         * 
         * Constructs an empty key.
         */
        inline key();
        // inline key(bool aht); //  = true);
        /**
         * @brief constructs key from data
         * @param data  pointer to data
         * @param len   data length
         * @param aht   if true a one way hash to the data is allowed
         */
        inline key(const void *data, size_t len, bool aht = true);
        /**
         * @brief constructs key from C-string
         * @param str   NULL terminated string
         * @param aht   if true a one way hash to the data is allowed
         */
        inline key(const char   *str, bool aht = true);
        /**
         * @brief constructs key from string
         * @param str   string
         * @param aht   if true a one way hash to the data is allowed
         */
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
