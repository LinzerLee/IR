/******************************************
 *
 *      NAME:   HtmlParser.hpp 
 *      DESC:   A utility to extract links
 *				from net pages. 
 *      Author: Linzer Lee
 *      Date:   2016/06/03
 *      Version:V1.0.0
 *
 ******************************************/

#ifndef HTMLPARSER_HPP
#define HTMLPARSER_HPP

#include <vector>
#include <string>

#include "Define.hpp"

namespace NetUtil
{
	_final class EXPORT_API HtmlParser
    {
    public:
        HtmlParser();
        ~HtmlParser();
        
        void setHtmlText(std::string text);
        std::vector<std::string> extractLink();
    private:
        class pimpl_HtmlParser;
        pimpl_HtmlParser *pimpl{0};
    };
}

#endif /* HTMLPARSER_HPP */
