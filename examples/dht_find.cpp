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

#include "dht/node.h"
#include "dht/kadc/node.h"

const char *opt_key   = NULL;

dht::node *dht_node;

const char *usage = 
"Usage: dht_find key";

// An observer class for handling reporting different states
// and catching success/failure of store operations
class dht_handler : public dht::event_observer, public dht::search_handler {
	bool _find_started;
	bool _find_finished;
public:
	dht_handler() : _find_started(false), _find_finished(false) {}
	
	// dht::event_observer interface
	virtual int state_changed(int state) {
		std::cout << "DHT state changed to "
		          << dht_node->state_str(state) << std::endl;
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
	dht_node->observer_attach(handler, dht::event_observer::mask_all);
	
	dht_node->connect();

	// Do processing until disconnected
	while (dht_node->in_state() != dht::node::disconnected) {
		// Also ACE's reactor can be used instead of dht_node->process()
		dht_node->process();

		// When state changes, process returns (might return earlier also).
		// Check what the state is and do find if connected
		switch (dht_node->in_state()) {
		case dht::node::connected:
			if (!handler->find_started()) {
				std::cout << "Starting finding key " << opt_key << std::endl;
				handler->find_start();
				dht_node->find(opt_key, handler);
			} else if (handler->find_finished()) {
				// When store is done, disconnect DHT
				dht_node->disconnect();
			}
		}
	}	
	dht_node->observer_remove(handler, dht::event_observer::mask_all);
	delete handler;
}

dht::node *
dht_create() {
	dht::node *n = new dht::kadc::node;
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
	dht_node  = dht_create();
		  
	find();
	
	std::cout << "Destroying dht_node" << std::endl;
	
	delete dht_node;
	
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

#if 0
#include <ace/Get_Opt.h>

#include <list>
#include <string>
#include <utility>
#include <iostream>

#define INIT_FILE "kadc.ini"

// For now like this, maybe later should use a factory to abstract away
// the used DHT implementation
#include "dht/kadc/node.h"

using namespace std;

typedef list<dht::key> find_list;

find_list find_items;

dht::node *dht_node;

const char *usage = 
"\nUsage: dht_find [opts] key ...\n" \
" opts:\n" \
"  -d [dht]  dht implementation to use. TODO. \n" \
"\n";

class success_counter : public dht::search_handler {
	const char *_id;
	unsigned int _counter;
public:
	success_counter(const char *id) : _id(id), _counter(0) {}
	void reset()           { _counter = 0; }
	unsigned int &count()  { return _counter; }
	const char *id() const { return _id; }
	
	virtual void success() {
		ACE_DEBUG((LM_INFO, "USER: successfull %s\n", _id));
		_counter++;
	}
	virtual void failure(int error, const char *errstr) {
		ACE_DEBUG((LM_INFO, "USER: failed %s\n", _id));
		throw dht::exception(error, errstr);
	}
	
	int found(const dht::key &, const dht::value &) {
		throw dht::exceptionf(0, "Not supposed to receive this here %s",
		                     _id);
	}	
};

class find_counter : public success_counter {
public:
	find_counter(const char *id) : success_counter(id) {}
	
	int found(const dht::key &k, const dht::value &v) {
		// Print out the key and value
		cout << "Found a value for key " << k.c_str() << ":" << endl;
		cout << "  value: " << v.c_str() << endl;
		// Print out meta data
		dht::name_value_map::const_iterator i = v.meta().begin();
		for (; i != v.meta().end(); i++) {
			cout << "   meta: " << i->first.c_str() << "="
			                    << i->second.c_str() << endl;
		}
		return 0;
	}
};

void operation_simple() {
	// First connect
	auto_ptr<success_counter> conn_count(new success_counter("connect"));
	dht_node->connect(conn_count.get());
	// Wait until connected
	while (!conn_count->count()) {
		dht_node->process();
	}
	
	cout << "External address: " << dht_node->external_addr().get_host_addr()
	     << ":" << dht_node->external_addr().get_port_number() << endl;
	     
	//char ch;
	//cout << "Press enter when ready" << endl;
	//cin  >> ch;
	
	// Create a notify instance for searching
	auto_ptr<find_counter> find_count(new find_counter("find"));
	// Then search for the keys
	unsigned int find_total = find_items.size();
	while (find_count->count() != find_total) {
		// Only find item if there are no pending searches
		while (find_items.size() > 0 &&
		       find_total - (find_count->count() + 
		                     find_items.size()) < 2) 
		{
			find_list::iterator i = find_items.begin();
			dht_node->find(*i, find_count.get());
			find_items.pop_front();
		}
		dht_node->process();
	}

	// Then disconnect and wait until disconnect is finished
	auto_ptr<success_counter> disconn_count(new success_counter("disconnect"));
	dht_node->disconnect(disconn_count.get());
	// Wait until connected
	while (!disconn_count->count()) {
		dht_node->process();
	}
}

void parse_cmd_line_opts(ACE_Get_Opt &cmd_opts, map<char, string> &conf_values) {
	int option;
	while ((option = cmd_opts ()) != EOF) {
		switch (option) {
		case ':':
			throw dht::exception(0, "option missing");
		case 'd':
			conf_values[option] = cmd_opts.opt_arg();
			break;
		default:
			throw dht::exception(0, "invalid option");
		}
	}
}

void parse_cmd_line_keyvalue(ACE_Get_Opt &cmd_opts)
{
	for (int i = cmd_opts.opt_ind(); i < cmd_opts.argc(); i++) {
		dht::key   k(cmd_opts.argv()[i]);
		
		find_items.push_back(k);
	}	
}

dht::node *
dht_create() {
	dht::node *n = new dht::kadc::node;
	dht::name_value_map conf;
	
	conf.set("init_file", INIT_FILE);
	n->init(conf);
	return n;
}
	
int
do_main(int argc, ACE_TCHAR *argv[]) {
	static const ACE_TCHAR options[] = ":r:d:e:";
	ACE_Get_Opt cmd_opts(argc, argv, options, 1, 1);

	map<char, string> conf_opts;
	
	conf_opts['d'] = "kadc";
	
	parse_cmd_line_opts(cmd_opts,  conf_opts);
	parse_cmd_line_keyvalue(cmd_opts);
	
	if (find_items.size() <= 0) 
		throw dht::exception(0, "at least one key to find must be specified");

	dht_node = dht_create();
		  
	operation_simple();

	ACE_DEBUG((LM_DEBUG, "Destroying dht_node"));
	delete dht_node;

	return 0;
}


int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
	try {
		do_main(argc, argv);
	} catch (exception &e) {
		ACE_ERROR((LM_ERROR, "Exception caught:\n"));
		ACE_ERROR((LM_ERROR, "%s\n", e.what()));
		ACE_ERROR((LM_ERROR, usage));
		return -1;
	}

	return 0;
}

#endif
