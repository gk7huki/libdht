#ifndef _DHT_CLIENT_H_
#define _DHT_CLIENT_H_

#include "common.h"
#include "key.h"
#include "value.h"
#include "name_value_map.h"
#include "notify_handler.h"
#include "search_handler.h"
#include "event_observer_notifier.h"

namespace dht {
    /**
     * @brief Interface for DHT (Distributed Hash Table) implementations
     * 
     * The interface does not need to be thread safe. If 
     * the functions in the class are called from several threads
     * the application must manage the synchronization so that
     * several functions are not called at once. Likewise, if the
     * implementation of the interface internally uses threads it must
     * make sure all handlers are called only from the thread that
     * called process() function.
     * 
     * Several of the functions can receive pointer to handler that
     * will be called when the operation is finished. The handler must
     * only be called while being in process() and from the same thread
     * that called process(). This rule must be obeyed even in the case
     * when the call's success/failure status could be directly determined
     * when calling the function.
     * 
     * Generally all heavy weight processing must happen in process() functions.
     * The other functions, such as connect(), find(), store() and
     * disconnect() should delay processing that requires waiting, for 
     * example network operations to finish, to when user calls one of 
     * the process() functions. The calling application should for the
     * most time be calling process() functions and react when handler objects
     * are notified of new events.
     * 
     * The functions are allowed to throw exceptions in case of errors.
     * Exceptions should however be reserved for serious usage/operational
     * errors, such as calling store() without being connected, or running 
     * out of memory. If the function that received a handler throws exception, 
     * the handler's success/failure function will not be called.
     * In other error events, such as timeout of connecting,
     * the specified handler if any should be called with failure status.
     * 
     * In general the typical usage sequence of the interface is the following:
     * - new client
     * - init()
     * - connect()
     * - loop in process()
     *   - if connected find(), store() etc. as long as needed
     * - disconnect()
     * - loop in process() until disconnected
     * - delete client
     */
    class client {
        event_observer_notifier *_obs_notifier;
    protected:
        inline event_observer_notifier *observer_notifier() {
            return _obs_notifier;
        }
        
    public:
        enum {
            disconnected = 1,
            connecting,
            connected,
            disconnecting,
        };
        
        client();
        /**
         * @brief Destructor
         * 
         * When destructor called the implementation should clean up as
         * quickly as possible. If any handlers are pending they do
         * not need to be called. For clean shutdown disconnect() should
         * be called first (if connect() has been called) */
        virtual ~client();

        /**
         * @brief Returns the state of DHT
         */     
        virtual int in_state();
        /**
         * @brief Returns the state of DHT as a string
         */
        virtual const char *in_state_str();
        virtual const char *state_str(int state);
        
        /**
         * @brief Initializes the implementation
         * @param opts  String options, implementation specific
         * 
         * Should be called once before any other functions if
         * separate initialization needed by the implementation
         */     
        virtual void init(const name_value_map &opts) = 0;

        virtual void observer_attach(event_observer *h,
                                     int event_mask);
        virtual void observer_remove(event_observer *h);
        virtual void observer_remove(event_observer *h, int event_mask);
        
        /**
         * @brief Connects client to DHT
         * @param handler The handler to call when connect finished/failed.
         */
        virtual void connect(notify_handler  *handler = NULL) = 0;

        /**
         * @brief Disconnects client from DHT
         * @param handler The handler to call when disconnect finished/failed.
         */
        virtual void disconnect(notify_handler *handler = NULL) = 0;

        /**
         * @brief Searches the DHT for value matching the given key
         * @param skey     The key to search
         * @param handler  The handler which is notified when a value is
         *                 found and also finally when search is finished.
         *                 If the same key/value pair is found many times
         *                 from the DHT, the handler is allowed to be called
         *                 more than once for the same key/value pair.
         * 
         * @see search_handler
         */     
        virtual void find(const key      &skey,
                          search_handler *handler) = 0;

        /**
         * @brief Stores a value to DHT
         * @param skey     The key that can be used to find the value
         * @param svalue   The value to store into DHT
         * @param handler  The handler which is notified when storing the
         *                 value is done. A successfull store does not have 
         *                 to mean the value can be immediately retrieved 
         *                 from the DHT using find().
         * 
         * @see search_handler
         */     
        virtual void store(const key      &skey,
                           const value    &svalue,
                           notify_handler *handler = NULL) = 0;

        /**
         * @brief Gets client's external address
         * 
         * Returns the IP address of the client as seen by other
         * participants of the network. If the address can not be 
         * detected, should point to 0.0.0.0 address. Calling
         * the function must be safe when connected, is allowed to throw
         * an exception in other states.
         */
        virtual const addr_inet_type &external_addr() = 0;

        /**
         * @brief Does DHT specific processing
         * @param max_wait Maximum time to wait
         * 
         * The function does DHT specific processing only for maximum of
         * time specified with max_wait. It must return if
         * an event that triggers calling a handler happens. It is allowed
         * to call several handlers at one go if several events happen at the
         * same time. It is also allowed to return earlier even if no events
         * that cause calling of a handler happens. Consider max_wait as a
         * hint how long the application is willing to wait.
         * 
         * The max_wait value is is decremented to reflect how much 
         * time this call took. For instance, if a time value of 3 
         * seconds is passed to process and an event occurs after 2 
         * seconds, max_wait will equal 1 second. This can be used 
         * if an application wishes to handle events for some 
         * fixed amount of time.
         * 
         * @see reactor()
         */     
        virtual int process(time_value_type &max_wait) = 0;

        /**
         * @brief Does DHT specific processing
         * @param max_wait Maximum time to wait or NULL if indefinite.
         * 
         * The function does DHT specific processing only for maximum of
         * time specified with max_wait. It must return if
         * an event that triggers calling a handler happens. It is allowed
         * to call several handlers at one go if several events happen at the
         * same time. It is also allowed to return earlier even if no events
         * that cause calling of a handler happens. Consider max_wait as a
         * hint how long the application is willing to wait. If max_wait
         * is NULL then the call blocks indefinitely until some event occurs.
         * 
         * The max_wait value is is decremented to reflect how much 
         * time this call took. For instance, if a time value of 3 
         * seconds is passed to process and an event occurs after 2 
         * seconds, max_wait will equal 1 second. This can be used 
         * if an application wishes to handle events for some 
         * fixed amount of time.
         * 
         * @see reactor
         */     
        virtual int process(time_value_type *max_wait = NULL) = 0;
        
        /**
         * @brief Returns the reactor which is used for event demultiplexing
         * 
         * Besides calling process(), where implementations usually should 
         * use internally (ACE's) reactor for its processing needs, the 
         * application can directly call reactor. The DHT 
         * implementation registers the events it looks for to 
         * the reactor and thus will be notified accordingly if 
         * the application is running in an reactor loop.
         * Calling directly the reactor is an alternative to using process()
         * and usually the recommended way if the application needs to look
         * for other I/O events besides the DHT. See ACE documentation for
         * information on reactor usage.
         * 
         * By default implementations should use ACE_Reactor::instance()
         * as their reactor unless otherwise set by the application.
         */
        virtual reactor_type *reactor() = 0;

        /**
         * @brief Sets the reactor which is used for event demultiplexing
         * @param reactor The reactor DHT implementation should use. 
         *                
         * Besides calling process(), which usually should use internally
         * (ACE's) reactor for its processing needs, the application can
         * directly call reactor. The DHT implementation registers
         * the events it looks for to the reactor and thus will be notified
         * accordingly if the application is running in an reactor loop.
         * Calling directly the reactor is an alternative to using process()
         * and usually the recommended way if the application needs to look
         * for other I/O events besides the DHT. See ACE documentation for
         * information on reactor usage.
         * 
         * By default implementations should use ACE_Reactor::instance()
         * as their reactor unless otherwise set by the application.
         */
        virtual void          reactor(reactor_type *) = 0;
        
        
        // TODO documentation
        virtual int handler_cancel(notify_handler *handler) = 0;
        
    };
}

#endif //_DHT_CLIENT_H_
