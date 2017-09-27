/******************************************
 *
 *      NAME:   Net.hpp 
 *      DESC:   A utility to get net pages. 
 *      Author: Linzer Lee
 *      Date:   2016/06/03
 *      Version:V1.0.0
 *
 ******************************************/
#ifndef NET_HPP
#define NET_HPP

#include <string>

#include "Define.hpp"

namespace NetUtil
{
	_final class EXPORT_API Net
    {
        typedef int NETCode;
        #define NET_OK  0
    public:
        Net();
        ~Net();
        std::string FetchPage(const char *url);
        NETCode ErrorCode();
        
        static void Net_Init_Env();
        static void Release();
    private:
        class pimpl_Net;
        pimpl_Net *pimpl{0};
    };
}

#endif /* NET_HPP */
