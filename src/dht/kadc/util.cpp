#include <ace/Log_Msg.h>

#include <assert.h>
#include <string.h>

#include "util.h"

namespace dht {
namespace kadc {
namespace util {

void kadc_hash(std::string *result, const void *data, int len, bool do_md4) {
    unsigned char h[16];
    char kadch[33]; // 32 + NULL
    
    if (do_md4) {
        // ACE_DEBUG((LM_DEBUG, "kadc::kadc_hash creating md4 of the data '%s'\n",
        //           data));
        MD4(h, (unsigned char *)data, len);
    } else {
        ACE_DEBUG((LM_DEBUG, "kadc::kadc_hash using data of length " \
                   "%d directly\n", len));
        assert(len <= 16);
        memset(h, 0, 16);
        memcpy(h, data, len);
    }
    
    // Create kadc style hash
    int128sprintf(kadch, h);
    *result  = "#";
    *result += kadch;
    
    ACE_DEBUG((LM_DEBUG, "kadc::kadc_hash created hash %s\n", result->c_str()));
}

void kadc_meta(std::string *result, const name_value_map &meta) {
    result->erase();
    name_value_map::const_iterator i = meta.begin();
    for (; i != meta.end(); i++) {
        if (!result->empty()) *result += ";";
        *result += i->first;
        *result += "=";
        *result += i->second;
    }
    ACE_DEBUG((LM_DEBUG, "kadc::kadc_meta created metalist %s\n", 
              result->c_str()));    
}

void kadc_result(value *v, KadCdictionary *pkd) {
    KadCtag_iter iter;
    unsigned int i;
    
    KadCtag_begin(pkd, &iter);
    // The actual value
    v->set(iter.vhash, 16);
    
    // Debugging
#ifdef DEBUG
    char vhashbuf[33];
    int128sprintf(vhashbuf, (unsigned char *)iter.vhash);
    ACE_DEBUG((LM_DEBUG, "kadc_result: value %s\n", vhashbuf));
#endif  

    // Then set the metadata
    for(i = 0; i < iter.tagsleft; i++, KadCtag_next(&iter)) {
        // Currently metadata of dht abstraction supports only
        // string types
        switch (iter.tagtype) {     
        case KADCTAG_STRING:
            ACE_DEBUG((LM_DEBUG, "kadc_result: setting meta data %s=%s\n",
              iter.tagname, iter.tagvalue));
            v->meta().set(iter.tagname, iter.tagvalue);
            break;
        default:
            ACE_DEBUG((LM_WARNING, 
              "kadc_result: warning meta tagname %s (%d) not of supported " \
              " type (string)\n",
              iter.tagname, iter.tagtype));
            break;
        }
    }
}

void kadc_free_search_result(void *resdictrbt) {
    KadCdictionary *pkd;
    void *iter;
    
    for (iter = rbt_begin(resdictrbt); iter != NULL; 
         iter = rbt_begin(resdictrbt)) 
    {
        pkd = static_cast<KadCdictionary *>(rbt_value(iter));
        rbt_erase(resdictrbt, iter);
        KadCdictionary_destroy(pkd);
    }
    rbt_destroy(resdictrbt);
}

void kadc_external_address(addr_inet_type *a, KadCcontext *pkcc) {
    unsigned long int kadc_ip = KadC_getextIP(pkcc);
    
    a->set(KadC_getourUDPport(pkcc), kadc_ip);
    
    ACE_DEBUG((LM_DEBUG, "kadc_external_address: got address %s:%d\n",
              a->get_host_addr(), a->get_port_number()));             
}

} // ns util
} // ns kadc
} // ns dht
