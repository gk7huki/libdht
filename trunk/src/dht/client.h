#ifndef _DHT_CLIENT_H_
#define _DHT_CLIENT_H_

#include "common.h"
#include "key.h"
#include "value.h"
#include "name_value_map.h"
#include "notify_handler.h"
#include "search_handler.h"
#include "event_observer_notifier.h"

/**
 * @mainpage Quick help for libdht
 * 
 * @section Introduction
 * 
 * libdht is a cross-platform C++ library providing an 
 * interface for accessing Dynamic Hash Tables (DHTs) with 
 * a unified interface.
 * 
 * At the moment eDonkey's Overnet network is implemented
 * using KadC library.
 * 
 * The library provides an event based access to following
 * DHT features:
 * - connecting
 * - disconnecting
 * - retrieving values given key
 * - storing values with a key
 * 
 * Metadata for a key/value pair is also supported if the
 * underlying network supports it.
 * 
 * @section Installing
 * 
 * @subsection Dependencies
 * - ACE networking library http://www.cs.wustl.edu/~schmidt/ACE.html
 * - KadC library http://kadc.sourceforge.net
 * - SCons build system http://www.scons.org/
 * 
 * @subsection compiling
 * 
 * After compiling and installing ACE, KadC and SCons,
 * copy the file build_config.py.dist to build_config.py.
 * Edit build_config.py to have correct paths to the
 * needed libraries.
 * 
 * Compiling after that is a simple matter of issuing
 * \verbatim
   scons
   \endverbatim
 * 
 * The library will be compiled under build directory, under
 * the corresponding architecture (eg. build/win32 or build/linux etc.)
 * 
 * @section Examples
 * 
 * See example subdirectory for some examples. At the moment, 
 * there is no makefile or a scons file for creating executables
 * of these yet, so if you want to compile these you have 
 * to create them yourself. Something to change for the future,
 * and if someone comes up with a good scons file for it before that,
 * send me a patch.
 * 
 * A quite minimal example of how easy it is to use the library
 * is the following example which tries finding values
 * for the key "ubuntu" from Overnet DHT:
 * 
 * @include simple_find_example.cpp
 * 
 */

namespace dht {
    /**
     * @class client client.h dht/client.h
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
     * The most convenient way to obtain results is by adding observers for different
     * events, such as change of state (from connecting to connected),
     * search results etc. Several of the functions can also receive pointer 
     * to handler that will be called when the operation is finished. The handler 
     * will only be called while being in process() and from the same thread
     * that called process(). This rule must be obeyed by implementations
     * even in the case when the call's success/failure status could be 
     * directly determined when calling the function.
     * 
     * Generally all heavy weight processing must happen in process() functions.
     * The other functions, such as connect(), find(), store() and
     * disconnect() should delay processing that requires waiting, for 
     * example network operations to finish, to when user calls one of 
     * the process() functions. The calling application should for the
     * most time be calling process() functions and react when observers or
     * handler objects are notified of new events.
     * 
     * The functions are allowed to throw exceptions in case of errors.
     * Exceptions should be reserved for serious usage/operational
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
         * @brief Returns the state of client (disconnected, connecting etc.)
         */     
        virtual int in_state() const;
        /**
         * @brief Returns the current state of client as a string
         */
        virtual const char *in_state_str() const;
        /**
         * @brief Returns the client state as a string
         * @param state  the state
         * 
         * @see in_state()
         */
        virtual const char *state_str(int state) const;
        
        /**
         * @brief Optional initialization for the implementation
         * @param opts  name/value pairs of options, implementation specific
         * 
         * Should be called once before any other functions if
         * separate initialization needed by the implementation
         */     
        virtual void init(const dht::name_value_map &opts) = 0;

        /**
         * @brief Adds observer
         * @param h    observer
         * @param event_mask events that will cause observer to be notified
         * 
         * Can be used to add observers for specified events.
         * See event_observer class for allowed event masks.
         */     
        virtual void observer_attach(dht::event_observer *h,
                                     int event_mask);
        /**
         * @brief Removes observer
         * @param h       the observer to remove
         * 
         * Removes the specified observer
         */     
        virtual void observer_remove(dht::event_observer *h);
        /**
         * @brief Removes observer
         * @param h       the observer to remove
         * @param event_mask    mask of events from which to remove
         * 
         * Removes the specified observer with specified event mask
         */     
        virtual void observer_remove(dht::event_observer *h, int event_mask);
        
        /**
         * @brief Connects client to DHT
         * @param handler Optional handler to call when connect finished/failed.
         * 
         * Function returns immediately, success or failure is notified
         * separately once in process(). 
         * 
         * Instead of using the handler to get notified of connection, 
         * it is often better to add an observer for state changes 
         * to get notified when in connected state
         */
        virtual void connect(dht::notify_handler  *handler = NULL) = 0;

        /**
         * @brief Disconnects client from DHT
         * @param handler The handler to call when disconnect finished/failed.
         * 
         * Function returns immediately, success or failure is notified
         * separately once in process(). 
         * 
         * Instead of using the handler to get notified of disconnect, 
         * it is often better to add an observer for state changes 
         * to get notified when in disconnected state
         */
        virtual void disconnect(dht::notify_handler *handler = NULL) = 0;

        /**
         * @brief Searches the DHT for value matching the given key
         * @param fkey     The key to search
         * @param handler  The handler which is notified when a value is
         *                 found and also finally when search is finished.
         *                 If the same key/value pair is found many times
         *                 from the DHT, the handler is allowed to be called
         *                 more than once for the same key/value pair.
         * 
         * @see search_handler
         */     
        virtual void find(const dht::key      &fkey,
                          dht::search_handler *handler) = 0;

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
        virtual void store(const dht::key      &skey,
                           const dht::value    &svalue,
                           dht::notify_handler *handler = NULL) = 0;

        /**
         * @brief Gets client's external address
         * 
         * Returns the IP address of the client as seen by other
         * participants of the network. If the address can not be 
         * detected, should point to INADDR_NONE address. Calling
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
         * an event that triggers calling a handler or observer happens. 
         * It is allowed to call several handlers at one go if several 
         * events happen at the same time. It is also allowed to return earlier 
         * even if no events that cause calling of a handler happens. Consider 
         * max_wait as a hint how long the application is willing to wait.
         * 
         * The max_wait value is decremented to reflect how much 
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
         * an event that triggers calling a handler or observer happens. 
         * It is allowed to call several handlers at one go if several 
         * events happen at the same time. It is also allowed to return earlier 
         * even if no events that cause calling of a handler happens. Consider 
         * max_wait as a hint how long the application is willing to wait.
         * 
         * The max_wait value is decremented to reflect how much 
         * time this call took. For instance, if a time value of 3 
         * seconds is passed to process and an event occurs after 2 
         * seconds, max_wait will equal 1 second. This can be used 
         * if an application wishes to handle events for some 
         * fixed amount of time.
         * 
         * @see reactor()
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
        virtual void          reactor(reactor_type *reactor) = 0;
        
        
        /**
         * @brief cancels a handler
         * @param handler  the handler to cancel
         * 
         * If a handler has been registered for calling, for example
         * in connect(), disconnect() or find(), then it can be cancelled
         * by calling this function. If called the specified handler won't
         * be called.
         */
        virtual int handler_cancel(dht::notify_handler *handler) = 0;
        
    };
}

#endif //_DHT_CLIENT_H_
