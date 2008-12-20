#ifndef DHT_NOTIFY_HANDLER_H_
#define DHT_NOTIFY_HANDLER_H_

namespace dht {
    /**
     * @class notify_handler notify_handler.h dht/notify_handler.h
     * @brief Interface for handling success/failure notifications.
     * 
     * Classes that implement this interface can receive and react to
     * notifications from DHT implementations.
     */
    class notify_handler {
    public:
        virtual ~notify_handler();
        
        /**
         * @brief Called in case of success
         */
        virtual void success() = 0;
        /**
         * @brief Called in case of a failure
         * @param error  an error code
         * @param errstr a NULL terminated string giving exact reason for
         *               the error
         */
        virtual void failure(int error, const char *errstr) = 0;
    };
}

#endif //DHT_NOTIFY_HANDLER_H_
