#ifndef _DHT_BASIC_DATA_H_
#define _DHT_BASIC_DATA_H_

#include <ace/Log_Msg.h>
#include <stddef.h>

#include <string>

namespace dht {
    /**
     * @brief A simple data block
     * 
     * This class encapsulates a simple data block which may contain
     * also non-alphanumeric data. Should be used for relatively short
     * amounts of data.
     */
    class basic_data {
        std::string _data;
    public:
        /**
         * @brief constructor
         */
        inline basic_data();
        /**
         * @brief constructor
         * @param data  a pointer to the data
         * @param len   length of the data
         * 
         * Copies the data.
         */
        inline basic_data(const void *data, size_t len);
        /**
         * @brief constructor
         * @param str string object
         * 
         * Copies the string data
         */
        inline basic_data(const std::string &str);
        /**
         * @brief constructor
         * @param str NULL terminated C string
         * 
         * Copies the string data
         */
        inline basic_data(const char *str);
        /**
         * @brief Copy constructor
         * @param o  object to initialize from
         *
         * Copies the data
         */
        inline basic_data(const basic_data &o);

        /**
         * @brief Destructor
         *
         * Frees the data held by this object
         */
        virtual ~basic_data();

        /**
         * @brief Sets data
         * @param data  a pointer to the data
         * @param len   length of the data
         * 
         * Copies the data.
         */     
        inline void set(const void *data, size_t len);
        /**
         * @brief Sets data from string object
         * @param str string object
         * 
         * Copies the string data
         */
        inline void set(const std::string &str);
        /**
         * @brief Sets data from C string
         * @param str NULL terminated C string
         * 
         * Copies the string data
         */
        inline void set(const char *str);
        
        /**
         * @brief Returns a pointer to the data
         * @return a pointer to the data.
         */
        inline const void *data() const;
        /**
         * @brief Returns a pointer to the data
         * @return a NULL terminated pointer to the data
         */
        inline const char *c_str() const;
        /**
         * @brief Returns the length of the data
         * @return length of the data
         */
        inline size_t size() const;             

        inline basic_data &operator=(const basic_data &o);

    };

    inline basic_data::basic_data() {}
    inline basic_data::basic_data(const void *data, size_t len) { 
        set((const char *)data, len); }
    inline basic_data::basic_data(const std::string &str)         { set(str); }
    inline basic_data::basic_data(const char   *str)         { set(str); }
    inline basic_data::basic_data(const basic_data &o)       { 
        _data = o._data;
    }
    
    inline void basic_data::set(const void *data, size_t len) {
        _data.assign((const char *)data, len);
    }
    inline void basic_data::set(const std::string &str) {
        // ACE_DEBUG((LM_DEBUG, "basic_data::set string: %s\n", str.c_str()));
        _data.assign(str);
    }
    inline void basic_data::set(const char *str) {
        // ACE_DEBUG((LM_DEBUG, "basic_data::set C string: %s\n", str));
        _data.assign(str);
    }
    inline const void *basic_data::data() const {
        return (const void *)_data.data();
    }
    inline const char *basic_data::c_str() const {
        return _data.c_str();
    }
    inline size_t basic_data::size() const {
        return _data.size();
    }   

    inline basic_data &basic_data::operator=(const basic_data &o) {     
        _data = o._data;
        return *this;
    }

} // ns dht
#endif // _DHT_BASIC_DATA_H_
