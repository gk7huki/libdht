#include "name_value_map.h"

using namespace std;

namespace dht {

name_value_map::name_value_map() {}
name_value_map::name_value_map(const name_value_map &o) {
    _str_map = o._str_map;  
}

name_value_map::~name_value_map() {}

const string &
name_value_map::get(const string &key) const {
    container_type::const_iterator i = _str_map.find(key);
    
    if (i == _str_map.end()) {
        throw call_errorf(
            "no key found for mandatory option %s",
            key.c_str()
        );
    }
    
    return i->second;
}

const string &
name_value_map::get(const string &key, const string &def) const {
    container_type::const_iterator i = _str_map.find(key);

    return (i == _str_map.end() ? def : i->second);
}

void
name_value_map::set(const string &key, const string &value,
                    bool error_if_set)
{
    container_type::const_iterator i = _str_map.find(key);
    
    if (error_if_set && i != _str_map.end()) {
        throw call_errorf(
            "option %s already set with value " \
            "%s, tried setting it to %s",
            key.c_str(), i->second.c_str(), value.c_str()
        );
    }
    
    _str_map[key] = value;
}

} // ns dht
