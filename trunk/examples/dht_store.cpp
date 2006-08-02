/**
 * File: dht_store.cpp
 * 
 * A simple example of how to store to DHT using libdht.
 * This example uses kadc implementation.
 * 
 * A kadc.ini file is expected to be available which holds
 * possible contact nodes. If there is problems getting 
 * connected, it might be necessary to 
 * Example:
 * ./
 */
#include <list>
#include <string>
#include <utility>
#include <iostream>

#include "dht/client.h"
#include "dht/kadc/client.h"

const char *opt_key   = NULL;
const char *opt_value = NULL;

dht::client *dht_client;

const char *usage = 
"Usage: dht_store key value";

// An observer class for handling reporting different states
// and catching success/failure of store operations
class dht_handler : public dht::event_observer, public dht::notify_handler {
	bool _store_started;
	bool _store_finished;
public:
	dht_handler() : _store_started(false), _store_finished(false) {}
	
	// dht::event_observer interface
	virtual int state_changed(int state) {
		std::cout << "DHT state changed to "
		          << dht_client->state_str(state) << std::endl;
		return 0;
	}	
	
	// dht::notify_handler interface
	virtual void success() {
		std::cout << "Stored value successfully" << std::endl;
		_store_finished = true;
	}
	virtual void failure(int /*err_code*/, const char *errstr) {
		std::cout << "Storing value failed: " << errstr << std::endl;
		_store_finished = true;
	}

	// Helper functions	
	inline void store_start() { _store_started = true; }
	inline bool store_started()  const { return _store_started;  }
	inline bool store_finished() const { return _store_finished; }
};

void store() {
	// Install observer to notify of different events
	dht_handler *handler = new dht_handler;
	dht_client->observer_attach(handler, dht::event_observer::mask_all);
	
	dht_client->connect();

	// Do processing until disconnected
	while (dht_client->in_state() != dht::client::disconnected) {
		// Also ACE's reactor can be used instead of dht_client->process()
		dht_client->process();

		// When state changes, process returns (might return earlier also).
		// Check what the state is and do store if connected
		switch (dht_client->in_state()) {
		case dht::client::connected:
			if (!handler->store_started()) {
				std::cout << "Starting store of key " << opt_key << std::endl;
				handler->store_start();
				// DHT's key/value pairs are usually limited to, say,
				// 128 bit hash, so store the actual value also to
				// meta data
				dht::value store_value(opt_value);
				store_value.meta().set(opt_key, opt_value);
				dht_client->store(opt_key, store_value, handler);
			} else if (handler->store_finished()) {
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
	if (argc != 3)
		throw "Invalid number of arguments";

	opt_key   = argv[1];
	opt_value = argv[2];
	dht_client  = dht_create();
		  
	store();
	
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

