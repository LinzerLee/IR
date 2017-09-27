#define DLL_IMPL

#include <string>
#include "HtmlParser.hpp"

namespace NetUtil
{
    class HtmlParser::pimpl_HtmlParser
    {
    public:
        std::string htmlText;
    };
}

using namespace NetUtil;

HtmlParser::HtmlParser()
{
    pimpl = new pimpl_HtmlParser();
}

HtmlParser::~HtmlParser()
{
    if(pimpl)
    {
        delete pimpl;
        pimpl = NULL;
    }
}

void HtmlParser::setHtmlText(std::string text)
{
    pimpl->htmlText = text;
}

std::vector<std::string> HtmlParser::extractLink()
{
    std::vector<std::string> links;
    const std::string &text = pimpl->htmlText;
    auto len = text.size();
    auto pos = 0;
    
    bool tag_link_flag = false;
    for(; pos < len; ++pos)
    {
        switch(text[pos])
        {
        case '>':
                tag_link_flag = false;
                break;
        case '<':
                while(pos+1<len && iswspace(text[++pos]));
                
                if('a'!=text[pos] && 'i'!=text[pos])
                {
                    continue;
                }
                
                if('a'==text[pos] && !iswspace(text[1+pos]))
                {
                    continue;
                }
                
                if('i'==text[pos])
                {
                    if(pos+7>len || "iframe "!=text.substr(pos, 7))
                    {
                        pos += 6;
                        continue;
                    }
                }
                
                tag_link_flag = true;
                
                break;
        case 's':
                if(tag_link_flag)
                {
                    if(pos+3<=len && "src"==text.substr(pos, 3))
                    {
                        pos += 2;
                        
                        while(pos+1<len && '\"'!=text[++pos] && '\''!=text[pos]);
                        
                        auto begin = ++pos;
                        
                        if('h'!=text[begin])
                            continue;
                        
                        while(pos+1<len && '\"'!=text[++pos] && '\''!=text[pos]);
                        
                        auto end = pos;
                        
                        if(end<=len)
                        {
                            links.push_back(text.substr(begin, end - begin));
                        }
                        tag_link_flag = false;
                    }
                }
                break;
        case 'h':
                if(tag_link_flag)
                {
                    if(pos+4<=len && "href"==text.substr(pos, 4))
                    {
                        pos += 3;
                        
                        while(pos+1<len && '\"'!=text[++pos] && '\''!=text[pos]);
                        
                        auto begin = ++pos;
                        
                        if('h'!=text[begin])
                            continue;
                        
                        while(pos+1<len && '\"'!=text[++pos] && '\''!=text[pos]);
                        
                        auto end = pos;
                        
                        if(end<=len)
                        {
                            links.push_back(text.substr(begin, end - begin));
                        }
                        tag_link_flag = false;
                    }
                }
                break;
        default:
                break;
        }
    }
    
    return links;
}
