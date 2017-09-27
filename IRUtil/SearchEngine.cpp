#define DLL_IMPL

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>
#include <functional>

#include "Lexer.hpp"
#include "Inverter.hpp"
#include "SearchEngine.hpp"

#include "./AST/ASTExpression.hpp"
#include "./AST/ASTParser.hpp"

#include <db_cxx.h>
#define	IR_DOC_LINK         "ir_doc_link.db"

namespace IRUtil
{
    class SearchEngine::pimpl_SearchEngine
    {
    public:
    	~pimpl_SearchEngine();
    	SearchEngine::SE_Result getResult(FILE *ivtIdxFp, unsigned long long docID, const std::vector<std::string> &terms); 
		std::string docsPath;
		std::string cachePath;
        std::string invertIndexPath;
        std::map<std::string, unsigned long long> *dictionary{NULL};
        Lexer *lexer{NULL};
    };
}

using namespace IRUtil;

SearchEngine::pimpl_SearchEngine::~pimpl_SearchEngine()
{
	if(dictionary)
	{
		delete dictionary; 
		dictionary = NULL; 
	}
}

typedef unsigned char ubyte;
bool IS_ZH_WD(const char word[3])
{
	if ((ubyte)word[0] >= (ubyte)0XE4 && (ubyte)word[0] <= (ubyte)0XE9)
	{
		if ((ubyte)word[0] == (ubyte)0XE4 && (ubyte)word[1]<(ubyte)0XB8)
		{
			return false;
		}

		if ((ubyte)word[0] == (ubyte)0XE9 && (ubyte)word[1]>(ubyte)0XBE)
		{
			return false;
		}

		if ((ubyte)word[0] == (ubyte)0XE4 && (ubyte)word[1] == (ubyte)0XB8 && (ubyte)word[2]<(ubyte)0X80)
		{
			return false;
		}

		if ((ubyte)word[0] == (ubyte)0XE9 && (ubyte)word[1] == (ubyte)0XBE && (ubyte)word[2]>(ubyte)0XA5)
		{
			return false;
		}

		return true;
	}

	return false;
}

SearchEngine::SE_Result SearchEngine::pimpl_SearchEngine::getResult(FILE *ivtIdxFp, unsigned long long docID, const std::vector<std::string> &terms)
{
	SearchEngine::SE_Result result;
	char path[1024]{0};
	sprintf(path, "%s/%u", cachePath.c_str(), docID); 
	
	std::ifstream fin;
	std::string text;
    fin.open(path, std::ios::in);
	
	if(fin.is_open())
    {
    	size_t tPosS{std::string::npos}, tPosE{std::string::npos};
    	std::string title; 
    	while(!fin.eof()) 
    	{
    		fin >> text; 
    		if(std::string::npos==tPosS)
    		{
    			tPosS = text.find("<title>");
    			if(std::string::npos!=tPosS) 
    			{
    				tPosE = text.find("</title>");
    				if(std::string::npos!=tPosE)
    				{
    					title = text.substr(tPosS+7, tPosE-tPosS-7);
    					break; 
					}
					else
					{
						title = text.substr(tPosS+7);
					} 
				}
			}
    		else if(std::string::npos==tPosE)
			{
				tPosE = text.find("</title>");
				if(std::string::npos!=tPosE)
				{
					title += text.substr(0, tPosE);
				}
				else
					title += text;
			}	
				
			if(std::string::npos!=tPosS && std::string::npos!=tPosE) 
			{
				result.title = title;
				break; 
			}	
		}
	}
	
	fin.close();
	std::set<unsigned int> pos;
	for(auto &term : terms)
	{
		if(dictionary->count(term)>0)
		{
			// 所有文档都没有找到该关键字
			if ((*dictionary)[term] < 20)
				continue;
			auto &&doc_set_pos = Inverter::ExtractTermInfo(ivtIdxFp, (*dictionary)[term]);
			
			if(doc_set_pos.count(docID)>0)
			{
				auto &doc_pos = doc_set_pos[docID];
				
				for(auto p : doc_pos)
				{
					pos.insert(p);
				}
			}
		}
		
	}
	
	int digest_pos = *pos.begin();
	sprintf(path, "%s/%u", docsPath.c_str(), docID);
	fin.open(path, std::ios::in);
	if (fin.is_open())
	{
		if (digest_pos > 20)
			digest_pos -= 20;
		else
			digest_pos = 0;

		fin.seekg(0, std::ios::end);
		auto len = fin.tellg();
		fin.seekg(digest_pos, std::ios::beg);
		char buf[126]{0};
		int offset{0};
		if (len > digest_pos + 125)
		{
			len = 125;
			fin.read(buf, len);
		}
		else if(len > 125)
		{
			len = 125;
			fin.seekg(0, std::ios::beg);
			fin.read(buf, len);
		}
		else
		{
			fin.seekg(0, std::ios::beg);
			fin.read(buf, len);
		}
		
		for (; offset < len; ++offset)
		{
			if (IS_ZH_WD(buf + offset) || buf[offset]>0)
			{
				break;
			}
		}
		int end = len;
		end -= 3;
		if (buf[end] > 0)
		{
			buf[end + 1] = '\0';
			buf[end + 2] = '\0';
		}
		for (; offset < end; --end)
		{
			if (IS_ZH_WD(buf + end) || buf[end]>0)
			{
				break;
			}
			else
			{
				buf[end + 2] = '\0';
			}
		}

		result.digest = buf + offset;
	}
	
	fin.close();

	Db doc_link_db(NULL, NULL);
	// DOC TO LINK
	doc_link_db.set_error_stream(&std::cerr);
	doc_link_db.set_errpfx("SearchEngine");
	doc_link_db.set_pagesize(1024);		/* Page size: 1K. */
	doc_link_db.set_cachesize(0, 32 * 1024, 0);
	int ret{0};
	if (0 == (ret = doc_link_db.open(NULL, IR_DOC_LINK, NULL, DB_BTREE, DB_RDONLY, 0664)))
	{
		Dbt key, data;
		key.set_data(&docID);
		key.set_size(sizeof(unsigned long long));
		data.set_data(path);
		data.set_size(sizeof(path));
		memset(path, 0, sizeof(path));
		
		ret = doc_link_db.get(NULL, &key, &data, 0);
		if (ret == 0)
		{
			result.url = (char *)data.get_data();
		}
	}
	sprintf(path, "%d", docID);
	result.snapshot = path;
	
	// 删除空白字符
	std::string::iterator newend = std::remove_if(result.title.begin(), result.title.end(), isspace);
	result.title.erase(newend);

	newend = std::remove_if(result.digest.begin(), result.digest.end(), isspace);
	result.digest.erase(newend);

	newend = std::remove_if(result.url.begin(), result.url.end(), isspace);
	result.url.erase(newend);

	newend = std::remove_if(result.snapshot.begin(), result.snapshot.end(), isspace);
	result.snapshot.erase(newend);
	
	return result;
}

SearchEngine::SearchEngine()
{
    pimpl = new pimpl_SearchEngine();
}

SearchEngine::~SearchEngine()
{
    if(pimpl)
    {
        delete pimpl;
        pimpl = NULL;
    }
}

void SearchEngine::setDocsPath(const char *path)
{
	pimpl->docsPath = path; 
} 

void SearchEngine::setCachePath(const char *path)
{
	pimpl->cachePath = path; 
} 

void SearchEngine::setInvertIndexPath(const char *path)
{
	pimpl->invertIndexPath = path; 
	pimpl->dictionary = Inverter::CreateDictionary(pimpl->invertIndexPath.c_str());
} 

void SearchEngine::setLexer(Lexer *l)
{
	pimpl->lexer = l; 
} 

std::vector<SearchEngine::SE_Result> SearchEngine::search(const std::string &query)
{
	std::vector<SearchEngine::SE_Result> results;
	
	FILE *pFile = fopen(pimpl->invertIndexPath.c_str(), "rb");
	if(NULL==pFile)
    {
        printf("打开 %s 文件失败\n", pimpl->invertIndexPath.c_str());
        return results;
    }
	
	// 自动构建表达式
	AST::ASTParser parser;
	parser.init(pimpl->lexer, pFile, pimpl->dictionary); 
	auto expr = parser.expression(query); 
	
	std::set<unsigned long long> && doc_set = expr->getDocSet();
	
	expr->release();
	delete expr;
	expr = NULL;
	
	std::map<std::string, std::vector<int>> && termsPos = pimpl->lexer->getTerms(query); 
	std::vector<std::string> terms;
	for(auto &term : termsPos)
	{
		terms.push_back(term.first);
	} 
	
	for(auto &docID : doc_set)
	{
		results.push_back(pimpl->getResult(pFile, docID, terms));
	}  
	
	fclose(pFile);
	return results;
}

Json::Value SearchEngine::SE_Result_To_JSON(const std::vector<SearchEngine::SE_Result> &results)
{
	Json::Value arrayObj;
	for (auto &r : results)
	{
		Json::Value result;
		result["title"] = r.title;
		result["digest"] = r.digest;
		result["url"] = r.url;
		result["snapshot"] = r.snapshot;
		arrayObj.append(result);
	}

	return arrayObj;
}