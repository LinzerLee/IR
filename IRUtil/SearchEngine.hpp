/******************************************
 *
 *      NAME:   SearchEngine.hpp 
 *      DESC:   Component to search some 
 *				documents by a query. 
 *      Author: Linzer Lee
 *      Date:   2016/06/05
 *		Version:V1.0.0
 *
 ******************************************/

#ifndef SEARCH_ENGINE_HPP
#define SEARCH_ENGINE_HPP

#include <string>
#include <vector>
#include <json\json.h>

#include "Define.hpp"

namespace IRUtil
{
	class Lexer; 
    _final class EXPORT_API SearchEngine
    {
    public:
    	typedef struct _SE_Result
		{
			std::string title;
			std::string url;
			std::string digest;
			std::string snapshot;
		}SE_Result;
    public:
        SearchEngine();
        virtual ~SearchEngine();
        
        void setDocsPath(const char *path);
		void setCachePath(const char *path); 
        void setInvertIndexPath(const char *path);
        void setLexer(Lexer *l);
        std::vector<SearchEngine::SE_Result> search(const std::string &query);
		static Json::Value SE_Result_To_JSON(const std::vector<SearchEngine::SE_Result> &results);
    private:
        class pimpl_SearchEngine;
        pimpl_SearchEngine *pimpl{0};
    };
}

#endif /* SEARCH_ENGINE_HPP */
