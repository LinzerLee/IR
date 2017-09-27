#define DLL_IMPL

#include <string>
#include <vector>
#include <set>
#include <iostream>
#include <fstream>
// #include <List>

#include "Lexer.hpp"

namespace IRUtil
{
    class Lexer::pimpl_Lexer
    {
    	typedef unsigned char ubyte;
    public:
    	static bool IS_ZH_WD(const std::string word);
    public:
        std::set<std::string> keywords;
        size_t max_zh_len{5};  
        // std::string text;
        // std::List tokenQueue;
    };
}

using namespace IRUtil;

bool Lexer::pimpl_Lexer::IS_ZH_WD(const std::string word)
{
	if((ubyte)word[0]>=(ubyte)0XE4 && (ubyte)word[0]<=(ubyte)0XE9)
	{
		if((ubyte)word[0]==(ubyte)0XE4 && (ubyte)word[1]<(ubyte)0XB8)
		{
			return false;
		}
		
		if((ubyte)word[0]==(ubyte)0XE9 && (ubyte)word[1]>(ubyte)0XBE)
		{
			return false;
		}
		
		if((ubyte)word[0]==(ubyte)0XE4 && (ubyte)word[1]==(ubyte)0XB8 && (ubyte)word[2]<(ubyte)0X80)
		{
			return false;
		}
		
		if((ubyte)word[0]==(ubyte)0XE9 && (ubyte)word[1]==(ubyte)0XBE && (ubyte)word[2]>(ubyte)0XA5)
		{
			return false;
		}
		
		return true;
	}
	
	return false;
}

Lexer::Lexer()
{
    pimpl = new pimpl_Lexer();
}

Lexer::~Lexer()
{
    if(pimpl)
    {
        delete pimpl;
        pimpl = NULL;
    }
}

void Lexer::setMaxZHLen(size_t len)
{
	pimpl->max_zh_len = len;
}

void Lexer::setKeywords(const char *filename)
{
    std::ifstream fin;
    fin.open(filename);
    if(fin.is_open())
    {
        char line[32]={0};
        
        while(fin.getline(line, sizeof(line)))
        {
            pimpl->keywords.insert(line);
        }
        fin.close();
    }
}

void Lexer::addKeyword(const char *keyword)
{
	pimpl->keywords.insert(keyword);
}

void Lexer::deleteKeyword(const char *keyword)
{
	pimpl->keywords.erase(keyword);
} 

bool Lexer::checkKeyword(const char *term)
{
	if(pimpl->keywords.count(term)>0) 
		return true;
		
	return false;
}

std::map<std::string, std::vector<int>> Lexer::getTerms(const std::string &text)
{
	auto result = std::map<std::string, std::vector<int>>();
    auto max_zh_size = pimpl->max_zh_len * 3;
    std::string zh_word;
    
	long pos=text.size()-1, lexer_start_pos{-1};
    bool en_start_flag{false}, zh_start_flag{false}, is_keyword{false};
    while(pos>=0 || zh_start_flag)
    {
        // 前向探查超过最大值 
        if(pos<0 || (zh_start_flag && lexer_start_pos-pos>=max_zh_size))
        {
        	if(is_keyword)
            {
                // write_file(zh_word);
                result[zh_word].push_back(lexer_start_pos-zh_word.size()+1);
                is_keyword = false;
			}
				
        	pos = lexer_start_pos-zh_word.size();
        	zh_start_flag = false; 
        		 
        	continue; 
		}
			
        if((text[pos]>='a' && text[pos]<='z') || (text[pos]>='A' && text[pos]<='Z'))
        {
            if(zh_start_flag)
            {
                if(is_keyword)
                {
                	// write_file(zh_word);
                	result[zh_word].push_back(lexer_start_pos-zh_word.size()+1);
                	is_keyword = false;
				}
        		pos = lexer_start_pos-zh_word.size();
        		zh_start_flag = false; 
        		continue; 
            }
                
            if(!en_start_flag)
            {
                en_start_flag = true;
                lexer_start_pos = pos;
            }
            --pos;
            continue;
        }
        else if(text[pos]<0 && pos>=2 && pimpl_Lexer::IS_ZH_WD(text.substr(pos-2, 3)))
        {
            if(en_start_flag)
            {
                std::string && l = text.substr(pos+1, lexer_start_pos-pos);
                if(pimpl->keywords.count(l)>0)
                {
                    // write_file(l);
                    result[l].push_back(pos+1);
				}
					
                en_start_flag = false;
            }
                
            if(!zh_start_flag)
            {
                zh_start_flag = true;
                lexer_start_pos = pos;
                zh_word = text.substr(pos-2, 3);
            }
            else
            {
                std::string && tmp = text.substr(pos-2, lexer_start_pos-pos+3);
					 
                if(pimpl->keywords.count(tmp)>0)
                {
                	zh_word.swap(tmp);
                	is_keyword = true;
				}	
			}
            pos -= 3;
            continue;
        }
        else
        {
            if(zh_start_flag)
            {
                if(is_keyword)
                {
                	// write_file(zh_word);
                	result[zh_word].push_back(lexer_start_pos-zh_word.size()+1);
                	is_keyword = false;
				}
        		pos = lexer_start_pos-zh_word.size();
        		zh_start_flag = false; 
        		continue; 
            } 
                
            if(en_start_flag)
            {
                std::string && l = text.substr(pos+1, lexer_start_pos-pos);
                if(pimpl->keywords.count(l)>0)
                {
                    // write_file(l);
                    result[l].push_back(pos+1);
				}
                en_start_flag = false;
            }
            --pos;
        }
    }
        
	if(zh_start_flag)
    {
        if(is_keyword)
        {
            // write_file(zh_word);
            result[zh_word].push_back(lexer_start_pos-zh_word.size()+1);
            is_keyword = false;
		}
        pos = lexer_start_pos-zh_word.size();
        zh_start_flag = false;
    } 
                
    if(en_start_flag)
    {
        std::string && l = text.substr(pos+1, lexer_start_pos-pos);
        if(pimpl->keywords.count(l)>0)
        {
            // write_file(l);
            result[l].push_back(pos+1);
		}
        en_start_flag = false;
    }
    
    return result; 
} 

std::map<std::string, std::vector<int>> Lexer::getTerms(const char *filename)
{
	std::ifstream fin;
    fin.open(filename);
    if(fin.is_open())
    {
        std::istreambuf_iterator<char> beg(fin), end;
        std::string text(beg, end);
        fin.close();
        
        return getTerms(text); 
    }
    
    return std::map<std::string, std::vector<int>>();
}
/*
void Lexer::setText(const std::string &text)
{
	pimpl->text = text;
	pimpl->tokenQueue.clear();
	
}

std::string Lexer::getToken()
{
	if(!pimpl->tokenQueue.empty())
	{
		std::string &&tmp = pimpl->tokenQueue.front();
		pimpl->tokenQueue.erase(0);
		
		return tmp;
	}
	else
		std::string(); 
}

std::string Lexer::peekToken(int index)
{
	if(pimpl->tokenQueue.size()>index)
	{ 
		auto pos = pimpl->tokenQueue.begin(); 
		std::advance(pos, index);
		
		return *pos;
	} 
	else
		std::string(); 
}
*/
