#include <string>
#include <iostream>

#include <dht/client.h>
#include <dht/kadc/client.h>

// A simple handler that just outputs the
// found data
class handle_search : public dht::search_handler {
public:
    bool find_started; 
    bool find_finished;

    handle_search() : find_started(false), find_finished(false) {}
    
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
        find_finished = true;
    }
    
    virtual void failure(const dht::key &k, int /*errcode*/, const char *errstr) {
        std::cout << "Finding finished with error key: " << k.c_str() 
                  << " error: " << errstr << std::endl;
        find_finished = true;
    }    
};

int main(int, char **) {
    dht::kadc::client client;
    dht::name_value_map conf;

    conf.set("init_file", "kadc.ini");
    client.init(conf);
    
    client.connect();
    
    handle_search handler;
    
    bool done = false;
    while (!done) {
        client.process();

        switch (client.in_state()) {
        case dht::client::connected:
            if (handler.find_finished) {
                std::cout << "Find done, disconnecting" << std::endl;
                client.disconnect();
                done = true;
            } else if (!handler.find_started) {
                client.find("ubuntu", &handler);
                handler.find_started = true;
            }
            break;
        case dht::client::disconnected:
            done = true;
            break;
        }
    }
    return 0;
}
