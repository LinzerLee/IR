/******************************************
 *
 *      NAME:   ASTParser.hpp 
 *      DESC:   Create AST by parsing query
 *				string.
 *      Author: Linzer Lee
 *      Date:   2016/06/09
 *		Version:V1.0.0
 *
 ******************************************/


#ifndef ASTPARSER_HPP
#define ASTPARSER_HPP

#include <string>
#include <map>

#include "../Define.hpp"

namespace IRUtil 
{
	class Lexer;
	namespace AST
	{  
		class ASTExpression; 
	    class ASTParser
	    {
	    public:
			// Method 
	        ASTParser();
			virtual ~ASTParser(); 
			void init(Lexer *lexer, FILE *ivtIdxFp, std::map<std::string, unsigned long long> *dict);
			void setLexer(IRUtil::Lexer *lexer); 
			ASTExpression *expression(std::string query); 
	    private:
			class pimpl_ASTParser;
			pimpl_ASTParser *pimpl{0};
	    };
    } 
}

#endif /* ASTPARSER_HPP */
