#ifndef _DHT_KADC_OBSERVER_INFO_H_
#define _DHT_KADC_OBSERVER_INFO_H_

namespace dht {
namespace kadc {

    class observer_info {
        class observer_message  *_observer;
        class notify_handler    *_handler;
        class task              *_from_task;
    public:
        observer_info(class observer_message *o, 
                      class notify_handler *h = NULL, 
                      class task *t = NULL) 
            : _observer(o), _handler(h), _from_task(t) {}
            
        class observer_message *observer()  const { return _observer; }
        class notify_handler   *handler()   const { return _handler;  }
        void                    handler(class notify_handler *h) { 
            _handler = h; 
        }
        class task             *from_task() const { return _from_task; }
    };  
} // ns kadc
} // ns dht

#endif //_DHT_KADC_OBSERVER_INFO_H_
