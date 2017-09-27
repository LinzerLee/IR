#include <stack> 
#include <vector>

#include "ASTParser.hpp" 
#include "../Lexer.hpp"

#include "ASTerm.hpp"
#include "ASTAndExpr.hpp"
#include "ASTermExpr.hpp"
#include "ASTNotExpr.hpp"
#include "ASTOrExpr.hpp"

namespace IRUtil
{
	namespace AST
	{ 
		class ASTParser::pimpl_ASTParser
	    {
	    public:
	        FILE *ivtIdxFp;
	        Lexer *lexer;
	        std::map<std::string, unsigned long long> *dictionary;
	    };
    } 
}

using namespace IRUtil::AST;

ASTParser::ASTParser()
{
	ASTParser::pimpl = new pimpl_ASTParser();
}
 
ASTParser::~ASTParser()
{
	if(ASTParser::pimpl)
    {
        delete ASTParser::pimpl;
        ASTParser::pimpl = NULL;
    }
} 

void ASTParser::init(IRUtil::Lexer *lexer, FILE *ivtIdxFp, std::map<std::string, unsigned long long> *dict)
{
	ASTParser::pimpl->ivtIdxFp = ivtIdxFp;
	ASTParser::pimpl->lexer = lexer;
	ASTParser::pimpl->dictionary = dict;
} 

void ASTParser::setLexer(IRUtil::Lexer *lexer)
{
	ASTParser::pimpl->lexer = lexer;
} 
 
ASTExpression *ASTParser::expression(std::string query)
{
	size_t pos{0};
	int bracket_start_pos{-1};
	int deep=0; 
	std::stack<ASTExpression *> exprStack;
	std::stack<std::string> optrStack;
	std::string termStr;
	
	while(pos<query.size()) 
	{
		if(query[pos]=='(') 
		{
			if(deep==0)
				bracket_start_pos = pos;
			
			++deep;
		}
		else if(query[pos]==')') 
		{
			--deep;
			
			if(deep==0)
			{
				std::string q = query.substr(bracket_start_pos+1, pos-1-bracket_start_pos);
				auto expr = expression(q);
				if(expr!=NULL)
				{
					while(!optrStack.empty())
					{
						if(optrStack.top()=="!")
						{
							auto notExpr = new ASTNotExpr();
							notExpr->init(expr);
							notExpr->setExpr(expr);
							expr = notExpr;
							optrStack.pop();
						}
						else if(optrStack.top()=="&&")
						{
							if(!exprStack.empty())
							{
								auto andExpr = new ASTAndExpr();
								andExpr->init(expr);
								andExpr->setLeftExpr(exprStack.top());
								andExpr->setRightExpr(expr);
								expr = andExpr;
								exprStack.pop();
							}
							optrStack.pop();
						}
						// OR
						else
						{
							break;
						}
					}
					exprStack.push(expr);
				}
				
				bracket_start_pos = -1;
			}
		}
		else if(deep==0 && query[pos]=='!') 
		{
			optrStack.push("!");
		}
		else if(deep==0 && pos+1<query.size() && query.substr(pos, 2)=="&&") 
		{
			if(exprStack.size()>0)
				optrStack.push("&&");
			pos += 2;
			
			continue;
		}
		else if(deep==0 && pos+1<query.size() && query.substr(pos, 2)=="||") 
		{
			if(exprStack.size()>0)
				optrStack.push("||");
			pos += 2;
			
			continue;
		}
		else if(deep==0) 
		{
			while(pos<query.size() && query[pos]!='(' && query[pos]!=')' && query[pos]!='&' && query[pos]!='|' && query[pos]!='!')
			{
				termStr += query[pos]; 
				++pos;
			}
			
			if(termStr.size()>0)
			{
				auto &&terms = ASTParser::pimpl->lexer->getTerms(termStr);
				std::vector<ASTExpression *> arrayExprs;
				for(auto &t : terms)
				{
					auto asTerm = new ASTerm();
					asTerm->init(ASTParser::pimpl->lexer, ASTParser::pimpl->ivtIdxFp, ASTParser::pimpl->dictionary);
					asTerm->setTerm(t.first);
					
					auto expr = new ASTermExpr(); 
					expr->init(asTerm); 
					expr->setTerm(asTerm);
					arrayExprs.push_back(expr);
				}
				
				for(int i=arrayExprs.size()-2; i>=0; --i)
				{
					auto andExpr = new ASTAndExpr();
					andExpr->init(arrayExprs[0]); 
					andExpr->setLeftExpr(arrayExprs[i]);
					andExpr->setRightExpr(arrayExprs[i+1]);
					arrayExprs[i+1] = NULL;
					arrayExprs[i] = andExpr;
				}
				
				ASTExpression *expr{0};
				
				if(!arrayExprs.empty())
					expr = arrayExprs[0];
					
				while(expr && !optrStack.empty())
				{
					if(optrStack.top()=="!")
					{
						auto notExpr = new ASTNotExpr();
						notExpr->init(expr);
						notExpr->setExpr(expr);
						expr = notExpr;
						optrStack.pop();
					}
					else if(optrStack.top()=="&&")
					{
						if(!exprStack.empty())
						{
							auto andExpr = new ASTAndExpr();
							andExpr->init(expr);
							andExpr->setLeftExpr(exprStack.top());
							andExpr->setRightExpr(expr);
							expr = andExpr;
							exprStack.pop();
						}
						optrStack.pop();
					}
					else
					{
						break;
					}
				}
				if(expr)
					exprStack.push(expr);
				termStr.clear(); 
			} 
			
			continue;
		}
		
		++pos;
	} 
	
	// OR
	if(!exprStack.empty())
	{
		auto expr = exprStack.top();
		exprStack.pop();
		
		while(!exprStack.empty())
		{
			auto pt = exprStack.top();
			exprStack.pop();
			auto orExpr = new ASTOrExpr();
			orExpr->init(expr);
			orExpr->setLeftExpr(pt);
			orExpr->setRightExpr(expr);
			expr = orExpr;
		}
		return expr; 
	}	
	else
		return NULL;
		
	
}
