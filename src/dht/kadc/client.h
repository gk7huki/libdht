#ifndef DHT_KADC_CLIENT_H_
#define DHT_KADC_CLIENT_H_

#include <utility>
#include <algorithm>
#include <string>
#include <list>
#include <map>

#include "../client.h"
#include "shared_queue.h"
#include "message.h"
#include "observer_info.h"

// TODO these should really be in .cpp so that as little as possible
// of kadc files get included in apps that use dht abstraction
extern "C" {
#include <KadC/int128.h>
#include <KadC/rbt.h>
#include <KadC/MD4.h>
#include <KadC/KadCalloc.h>
#include <KadC/KadClog.h>
#include <KadC/KadCapi.h>
}

namespace dht {
namespace kadc {
    using namespace std;

    /**
     * @class client client.h dht/kadc/client.h
     * @brief Implementation of DHT that uses KadC library
     * 
     * This implementation of dht::client interface uses KadC
     * C-library under the hood. KadC provides access to
     * eDonkey's Overnet network.
     * 
     * KadC uses for initialization a file that contains
     * list of contact nodes to other possible peers. This
     * file has to be given as options in initialization.
     * KadC distribution has an example of the file.
     * 
     * A little code snippet example:
     * 
     * @code
     * #include <dht/kadc/client.h>
     * 
     * int main() {
     *    dht::kadc::client client
     *    dht::name_value_map conf;
     * 
     *    conf.set("init_file", "kadc.ini");
     *    client.init(conf);
     *    do_stuff(client);
     *    return 0;
     * }
     * @endcode
     * 
     * Overnet and KadC and therefore this implemenatation also
     * supports metadata.
     * 
     * @see KadC homepage http://kadc.sourceforge.net/
     */
    class client : public dht::client {
    public:
        /// @cond KADC_INTERNAL
        typedef shared_queue<message *> message_queue_type;
        friend class state;
        friend class reactor_event_handler;     
        /// @endcond
    private:
        // Mutable since KadC's functions, even non-modifying, do not
        // specify const
        mutable KadCcontext    _kcc;
        bool           _kstarted;
        string         _init_file;
        addr_inet_type _ext_addr;
        reactor_type  *_reactor;

        size_t _find_threads,
               _find_duration,
               _store_threads,
               _store_duration;
        size_t _find_max_hits;
               
        typedef map<task *, task *> running_tasks_type;
        typedef list<observer_info> message_obsvs_type;
        
        running_tasks_type _running_tasks;
        message_queue_type _msg_queue;
        message_obsvs_type _msg_observers;

        class reactor_event_handler *_rehandler;
        
        class state *_state;
        int          _state_out;
        
        // Functions that state objects need
        inline void         _kad_context(KadCcontext kcc) { _kcc = kcc; }
        inline KadCcontext *_kad_context() { return &_kcc; }

        // Used privately to set external address
        inline void _external_addr(const addr_inet_type &a) { _ext_addr = a; }

        inline message_queue_type *_message_queue() { return &_msg_queue; }
        // Changes internal state
        void _change_state(class state *s);
        // Changes state that can be queried by application
        void _change_state_out(int t);
        void _task_add(task *t);
        void _quit_all_tasks();
        void _wait_running_tasks();
        void _quit_task(task *t);
        inline int _running_tasks_size() { return _running_tasks.size(); }
        
        void _attach_observer_messages(const observer_info &oi);
        bool _detach_observer_messages(observer_info *oi);

        inline bool _kadc_started() const { return _kstarted; }
        inline void _kadc_started(bool t) { _kstarted = t; }
        
        void _process_queue();
        void _process_msg(message *tm);
    public:
        /// @cond KADC_INTERNAL
        const static int msg_connect       = 1;
        const static int msg_disconnect    = 2;
        const static int msg_store         = 3;
        const static int msg_search_result = 4;
        const static int msg_search_done   = 5;
        const static int msg_task_exit     = 6;
        /// @endcond
        
        client();
        virtual ~client();

        /**
         * @brief returns KadC's initialisation file path
         */
        inline const char *init_file() { return _init_file.c_str(); }
        
        /**
         * @brief Initialisation of KadC
         * 
         * Supported keys in dht::kadc::client:
         * - init_file
         */
        virtual void init(const name_value_map &opts);
        
        /**
         * @brief Deinitialises
         * 
         * Since KadC's deinitialisation can take several seconds,
         * it is not done in destructor as deleting instance
         * should be as fast as possible. This function can
         * be called instead. The function returns only after
         * deinitialisation has been done.
         * 
         * Note that if you call disconnect and wait until state
         * changes to disconnected, deinitialisation of KadC
         * will be done at the same time. Disconnecting is
         * the recommended way.
         * 
         * @see disconnect()
         */
        virtual void deinit();
        
        virtual void connect(dht::notify_handler    *handler = NULL);
        virtual void disconnect(dht::notify_handler *handler = NULL);
        
        virtual void find(const dht::key      &fkey,
                          dht::search_handler *handler);

        virtual void store(const dht::key      &skey,
                           const dht::value    &svalue,
                           dht::notify_handler *handler = NULL);

        virtual const addr_inet_type &external_addr();

        virtual int process(time_value_type &max_wait);
        virtual int process(time_value_type *max_wait = NULL);
        virtual reactor_type *reactor();
        virtual void          reactor(reactor_type *reactor);
        virtual int handler_cancel(notify_handler *handler);

        /**
         * @brief Sets number of threads used for find operations
         * @param t number of threads to use or 0 for KadC default
         * 
         * KadC is thread based. With this parameter you can
         * set how many threads are used when a find operation
         * is done.
         */
        inline size_t find_threads(size_t t) { 
            return _find_threads = std::min<size_t>(t, 20); 
        }
        /**
         * @brief Gets number of threads used for find operations
         * 
         * Can be 0 for default value.
         */
        inline size_t find_threads() const { return _find_threads; }

        /**
         * @brief Sets the duration of find operation in seconds
         * @param s number of seconds or 0 for KadC default
         */
        inline size_t find_duration(size_t s) {
            return _find_duration = std::min<size_t>(s, 200);
        }
        /**
         * @brief Gets the duration of find operation in seconds
         * 
         * Can be 0 if KadC default used.
         */
        inline size_t find_duration() const { return _find_duration; }

        /// @cond KADC_DEPRECATED
        inline size_t find_max_hits(size_t t) {
            return _find_max_hits = std::min<size_t>(t, 20);
        }
        
        inline size_t find_max_hits() const { return _find_max_hits; }
        /// @endcond

        /**
         * @brief Sets number of threads used for store operations
         * @param t number of threads to use or 0 for KadC default
         * 
         * KadC is thread based. With this parameter you can
         * set how many threads are used when a store operation
         * is done.
         */
        inline size_t store_threads(size_t t) {
            return _store_threads = std::min<size_t>(t, 20);
        }
        /**
         * @brief Gets number of threads used for store operations
         * 
         * Can be 0 for default value.
         */
        inline size_t store_threads() const { return _store_threads; }

        /**
         * @brief Sets the duration of store operation in seconds
         * @param s number of seconds or 0 for KadC default
         */
        inline size_t store_duration(size_t s) {
            return _store_duration = std::min<size_t>(s, 200);
        }
        /**
         * @brief Gets the duration of store operation in seconds
         * 
         * Can be 0 if KadC default used.
         */
        inline size_t store_duration() const { return _store_duration; }
        
        /**
         * @brief Writes KadC's initialization file to disk
         * @param target_file Path to file or if not specified/NULL 
         *                    then the one that was used for 
         *                    initialization
         * @return 0 if success, otherwise KadC error code
         * 
         * KadC keeps a list of contact nodes that has to be fed to
         * the implementatino during startup (using init() function).
         * This list of contact nodes is updated during operation
         * so that the list is kept up-to-date with fresh contacts
         * and non-responsive contacts are removed.
         * 
         * When disconnected or deinitializing this list is written
         * back to disk. In case the current list is wanted to be
         * saved in middle of operation, this function can be called.
         */
        int write_inifile(const char *target_file = NULL);

        /**
         * @brief returns number of nodes that have been contacted successfully
         */        
        size_t contacted_nodes() const;
        /**
         * @brief returns total number of contacts
         */        
        size_t contacts() const;

        /**
         * @brief Sets KadC log file location
         * @param path path to the log file
         * @exception io_error thrown if the log file specified 
         *            couldn't be opened for writing
         * 
         * By default KadC logging is written to STDERR. If you
         * call this function with a valid path then the log
         * is written to the file specified.
         * 
         * This is a static function and thus affects all KadC
         * instances.
         */
        static void logfile(const std::string &path);
//      virtual const addr_inet_type *external_addr() = 0;      
    };
        
} // ns kadc
} // ns dht

#endif //DHT_KADC_CLIENT_H_
