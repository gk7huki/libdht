#ifndef _DHT_COMMON_H_
#define _DHT_COMMON_H_

#include <ace/Log_Msg.h>
#include <ace/Addr.h>
#include <ace/INET_Addr.h>
#include <ace/Time_Value.h>
#include <ace/Reactor.h>

/**
 * @file    common.h
 * @date    Apr 9, 2005
 * @author  Arto Jalkanen
 * @brief   Some common definitions for DHT client
 *
 */

namespace dht {
    typedef ACE_INET_Addr   addr_inet_type;
    typedef ACE_Time_Value  time_value_type;
    typedef ACE_Reactor     reactor_type;    
}

#endif //_REUDP_COMMON_H_
