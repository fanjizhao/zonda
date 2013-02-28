#ifndef ZONDA_COMMON_BYTE_ORDER_H_
#define ZONDA_COMMON_BYTE_ORDER_H_

#include <endian.h>
#include <byteswap.h>

namespace zonda
{
namespace common
{
    


#if __BYTE_ORDER == __BIG_ENDIAN
    #define my_ntohll(x)      (x)
    #define my_htonll(x)      (x)
#elif __BYTE_ORDER == __LITTLE_ENDIAN
    #define my_ntohll(x)     __bswap_64 (x)
    #define my_htonll(x)     __bswap_64 (x)
#else
    #error "Unsupported endianess!"    
#endif

}//namespace common
}//namespace zonda

#endif //ZONDA_COMMON_BYTE_ORDER_H_


