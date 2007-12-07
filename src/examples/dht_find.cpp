/**
 * File: dht_find.cpp
 * 
 * A simple example of how to find from DHT using libdht.
 * This example uses kadc implementation.
 * 
 * A kadc.ini file is expected to be available which holds
 * possible contact nodes. If there is problems getting 
 * connected, it might be necessary to 
 */
#include <list>
#include <string>
#include <utility>
#include <iostream>

#include "dht/client.h"
#include "dht/kadc/client.h"

const char *opt_key   = NULL;

dht::client *dht_client = NULL;

const char *usage = 
"Usage: dht_find key";

// An observer class for handling reporting different states
// and catching success/failure of store operations
class dht_handler : 
    public dht::event_observer, 
    public dht::search_handler 
{
    bool _find_started;
    bool _find_finished;
public:
    dht_handler() : _find_started(false), _find_finished(false) {}
    
    // dht::event_observer interface
    virtual int state_changed(int state) {
        std::cout << "DHT state changed to "
                  << dht_client->state_str(state) << std::endl;
        return 0;
    }   
    
    // dht::search_handler interface
    virtual int found(const dht::key &k, const dht::value &v) {
        std::cout << "Found key: " << k.c_str() << " "
                  << "with value: " << v.c_str() << std::endl;
        // Output meta information, if any
        const dht::name_value_map &meta = v.meta();
        dht::name_value_map::const_iterator i = meta.begin();
        for (; i != meta.end(); i++) {
            std::cout << "  Meta key/value: " 
                      << i->first << "/"
                      << i->second << std::endl;
        }
        return 0;
    }

    virtual void success(const dht::key &k) {
        std::cout << "Finding finished for key: " << k.c_str() << std::endl;
        _find_finished = true;
    }
    
    virtual void failure(const dht::key &k, int /*errcode*/, const char *errstr) {
        std::cout << "Finding finished with error key: " << k.c_str() 
                  << " error: " << errstr << std::endl;
        _find_finished = true;
    }

    // Helper functions 
    inline void find_start() { _find_started = true; }
    inline bool find_started()  const { return _find_started;  }
    inline bool find_finished() const { return _find_finished; }
};

void find() {
    // Install observer to notify of different events
    dht_handler *handler = new dht_handler;
    dht_client->observer_attach(handler, dht::event_observer::mask_all);
    
    dht_client->connect();

    // Do processing until disconnected
    while (dht_client->in_state() != dht::client::disconnected) {
        // Also ACE's reactor can be used instead of dht_client->process()
        dht_client->process();

        // When state changes, process returns (might return earlier also).
        // Check what the state is and do find if connected
        switch (dht_client->in_state()) {
        case dht::client::connected:
            if (!handler->find_started()) {
                std::cout << "Starting finding key " << opt_key << std::endl;
                handler->find_start();
                dht_client->find(opt_key, handler);
            } else if (handler->find_finished()) {
                // When store is done, disconnect DHT
                dht_client->disconnect();
            }
        }
    }   
    dht_client->observer_remove(handler, dht::event_observer::mask_all);
    delete handler;
}

dht::client *
dht_create() {
    dht::client *n = new dht::kadc::client;
    dht::name_value_map conf;
    
    // KadC needs initialization file
    conf.set("init_file", "kadc.ini");
    n->init(conf);
    return n;
}
    
int
do_main(int argc, ACE_TCHAR *argv[]) {
    if (argc != 2)
        throw "Invalid number of arguments";

    opt_key   = argv[1];
    dht_client  = dht_create();
          
    find();
    
    std::cout << "Destroying dht_client" << std::endl;
    
    delete dht_client;
    
    return 0;
}


int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
    if (argc <= 1) {
        std::cerr << usage << std::endl;
        return -1;
    }
    
    try {
        do_main(argc, argv);
    } catch (std::exception &e) {
        ACE_ERROR((LM_ERROR, "Exception caught:\n"));
        ACE_ERROR((LM_ERROR, "%s\n", e.what()));
        ACE_ERROR((LM_ERROR, usage));
        return -1;
    } catch (const char *err) {
        std::cerr << "Error: " << err << std::endl;
        std::cerr << usage << std::endl;
        return -1;
    }

    return 0;
}
