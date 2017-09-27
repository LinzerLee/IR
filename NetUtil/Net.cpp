#define DLL_IMPL

#include <string>
#include <curl/curl.h>
#include "Define.hpp"
#include "Net.hpp"

namespace NetUtil
{
    class Net::pimpl_Net
    {
    public:
        CURL *curl;             //定义CURL类型的指针
        CURLcode res;           //定义CURLcode类型的变量，保存返回状态码
        std::string result;
        
        pimpl_Net();
        ~pimpl_Net();
        
        void init();
        void cleanup();
        static size_t write_function(void *ptr, size_t size, size_t nmemb, void *buffer);
    };
}

using namespace NetUtil;

Net::pimpl_Net::pimpl_Net()
{
    init();
}

Net::pimpl_Net::~pimpl_Net()
{
    cleanup();
}

void Net::pimpl_Net::init()
{
    curl = curl_easy_init();        //初始化一个CURL类型的指针
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, false);  // 跳过证书检查
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, true); // 从证书中检查SSL加密算法是否存在
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, pimpl_Net::write_function);   //设置处理数据的函数
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &result);   //数据存储的对象指针
}

void Net::pimpl_Net::cleanup()
{
    curl_easy_cleanup(curl);        //清除curl操作
    curl = NULL;
    result.clear();
}

size_t Net::pimpl_Net::write_function(void *ptr, size_t size, size_t nmemb, void *buffer)
{
    std::string *strbuf = (std::string *)buffer;
    strbuf->append((char*)ptr, size * nmemb);
    
    return (size * nmemb);
}

Net::Net()
{
    pimpl = new pimpl_Net();
}

Net::~Net()
{
    if(pimpl)
    {
        pimpl->cleanup();
        delete pimpl;
        pimpl = NULL;
    }
}

void Net::Net_Init_Env()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

void Net::Release()
{
    curl_global_cleanup();
}

Net::NETCode Net::ErrorCode()
{
    return pimpl->res;
}

std::string Net::FetchPage(const char *url)
{
    pimpl->cleanup();
    pimpl->init();
    curl_easy_setopt(pimpl->curl, CURLOPT_URL, url);
    pimpl->res = curl_easy_perform(pimpl->curl);    //curl链接
    
    return std::string(pimpl->result);
}




