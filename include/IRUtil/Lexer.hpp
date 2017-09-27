/******************************************
 *
 *      NAME:   Lexer.hpp 
 *      DESC:   Component to split the words 
 *				from a document. 
 *      Author: Linzer Lee
 *      Date:   2016/06/02
 *		Version:V1.0.0
 *
 ******************************************/

#ifndef LEXER_HPP
#define LEXER_HPP

#include <string>
#include <vector>
#include <map>
#include "Define.hpp"

namespace IRUtil
{
    _final class EXPORT_API Lexer
    {
    public:
        Lexer();
        virtual ~Lexer();
        
        void setKeywords(const char *filename);
        void addKeyword(const char *keyword);
        void deleteKeyword(const char *keyword);
        bool checkKeyword(const char *term);
        void setMaxZHLen(size_t len);
        // void setText(const std::string &text);
        // std::string getToken();
        // std::string peekToken(int index);
        
        std::map<std::string, std::vector<int>>  getTerms(const std::string &text);
        std::map<std::string, std::vector<int>>  getTerms(const char *filename);
    private:
        class pimpl_Lexer;
        pimpl_Lexer *pimpl{0};
    };
}

#endif /* LEXER_HPP */
