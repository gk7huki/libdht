#include "search_handler.h"

namespace dht {

search_handler::~search_handler() {}

void 
search_handler::success(const dht::key &) { success(); }

void 
search_handler::failure(const dht::key &, int error, const char *errstr) {
    failure(error, errstr); 
}

void 
search_handler::success() {}

void 
search_handler::failure(int error, const char *errstr) {}

} // ns dht
