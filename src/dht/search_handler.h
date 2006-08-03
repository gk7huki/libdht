#ifndef _DHT_SEARCH_HANDLER_H_
#define _DHT_SEARCH_HANDLER_H_

#include "notify_handler.h"
#include "key.h"
#include "value.h"

namespace dht {
    /**
     * @brief Interface for handling DHT search results.
     * 
     * Classes that implement this interface can receive and react to
     * notifications from DHT implementations.
     */
    
    class search_handler : public notify_handler {
    public:
        virtual ~search_handler();

        /**
         * @brief Called when a new search result has been obtained.
         * @param k  The key that was searched
         * @param v  The value that was obtained
         * 
         * This function is called for each result that is found
         * for the searched key. If the function returns non-zero,
         * then no more search results for the search operation
         * are returned. If 0 returned, operation continues as normal.
         * Notice that if non-zero is returned, the success() and
         * failure() routines will NOT be called.
         */
        virtual int found(const key &k, const value &v) = 0;
        
        /**
         * @brief  Called if search finished successfully
         * @param k the key that was searched
         * 
         * Note that even if no results were found for the key,
         * success will be called unless an abnormal error happened.
         */
        virtual void success(const key &) { success(); }

        /**
         * @brief Called if search terminated because of an error
         * @param k the key that was searched
         * @param error  an error code
         * @param errstr a NULL terminated string giving exact reason for
         *               the error
         */
        virtual void failure(const key &, 
                             int error, const char *errstr)
        { failure(error, errstr); }
        
        virtual void success() {}
        virtual void failure(int, const char *) {}
    };
}

#endif //_DHT_SEARCH_HANDLER_H_
