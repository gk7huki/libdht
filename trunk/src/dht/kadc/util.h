#ifndef _DHT_KADC_UTIL_H_
#define _DHT_KADC_UTIL_H_

#include <string>

#include "../name_value_map.h"
#include "../value.h"
#include "client.h"

namespace dht {
namespace kadc {
namespace util {

void kadc_hash(std::string *result, const void *data, int len, bool do_md4);
void kadc_meta(std::string *result, const name_value_map &meta);
void kadc_result(value *v, KadCdictionary *pkd);
void kadc_free_search_result(void *resdictrbt);
void kadc_external_address(addr_inet_type *result, KadCcontext *pkcc);

} // ns util
} // ns kadc
} // ns dht
    
#endif //_DHT_KADC_UTIL_H_
