#ifndef _DHT_NAME_VALUE_MAP_H_
#define _DHT_NAME_VALUE_MAP_H_

#include <map>
#include <string>
#include "exception.h"

namespace dht {
    /**
     * @brief Contains name/value pairs of strings
     * 
     * A simple class wrapping a number of name/value pairs
     * of strings to an easily accessable interface. Name
     * is an unique identifier for a value: for each name
     * only one value exists.
     * 
     * Stl style iterator is provided for going through all the
     * name/value pairs contained within.
     */
    class name_value_map {
    public:
        typedef std::map<std::string, std::string>            container_type;
        typedef container_type::iterator       iterator;
        typedef container_type::const_iterator const_iterator;
                
    private:
        container_type _str_map;
    public:
        name_value_map();
        name_value_map(const name_value_map &o);
        
        /**
         * @brief Destructor
         * 
         * Frees all data contained in the object. References
         * to the data obtained with get() or iterators are invalid.
         */
        virtual ~name_value_map();

        /**
         * @brief Obtains a reference to a stored value
         * @param key  A string specifying the key
         * 
         * If there is no value for the key, throws an exception.
         * 
         * @return  a reference to the value.
         */
        const std::string &get(const std::string &key) const;

        /**
         * @brief Obtains a reference to a stored value
         * @param key  A string specifying the key
         * @param def  If no value is found for the key, returns the string
         *             specified here.
         * @return  a reference to the value.
         */     
        const std::string &get(const std::string &key, const std::string &def) const;

        /**
         * @brief Sets the value
         * @param key          A string specifying the key
         * @param value        A string specifying the value
         * @param error_if_set If true and there already exists a value for the
         *                     key, throws an exception
         */     
        void        set(const std::string &key, const std::string &value, 
                        bool error_if_set = false);

        /**
         * @brief Checks if the key exists
         * @return  true if a value for the given key exists
         */     
        inline bool exists(const std::string &key) const;
        
        /**
         * @brief stl style iterator accessors 
         */
        inline iterator       begin();
        /**
         * @brief stl style iterator accessors 
         */
        inline const_iterator begin() const;
        /**
         * @brief stl style iterator accessors 
         */
        inline iterator       end();
        /**
         * @brief stl style iterator accessors 
         */
        inline const_iterator end() const;
    };  

    inline bool
    name_value_map::exists(const std::string &key) const {
        return _str_map.find(key) != _str_map.end() ? true : false;
    }
    
    inline name_value_map::iterator 
    name_value_map::begin() { return _str_map.begin(); }
    
    inline name_value_map::const_iterator 
    name_value_map::begin() const { return _str_map.begin(); }

    inline name_value_map::iterator 
    name_value_map::end() { return _str_map.end(); }

    inline name_value_map::const_iterator 
    name_value_map::end() const { return _str_map.end(); }  
} // namespace dht

#endif //_DHT_NAME_VALUE_MAP_H_
