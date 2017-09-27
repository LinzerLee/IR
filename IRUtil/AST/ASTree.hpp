/******************************************
 *
 *      NAME:   ASTree.hpp 
 *      DESC:   Abstract Class to define
 *				Abstract Syntax Tree.
 *      Author: Linzer Lee
 *      Date:   2016/06/07
 *		Version:V1.0.0
 *
 ******************************************/

#ifndef ASTREE_HPP
#define ASTREE_HPP

#include <string>
#include <map> 
#include <set>
#include "../Define.hpp"

namespace IRUtil 
{
	class Lexer;
	namespace AST
	{ 
	    _abstract class ASTree
	    {
	    public:
	    	// Abstract Method 
	        _override std::set<unsigned long long> getDocSet()=0;
	        // Method 
	        ASTree();
			virtual ~ASTree(); 
	    	void init(Lexer *lexer, FILE *ivtIdxFp, std::map<std::string, unsigned long long> *dict);
	    	void init(const ASTree *tree);
	        _override bool is_fresh();
	    protected: 
	    	class pimpl_ASTree;
			pimpl_ASTree *pimpl{0};
	    };
	} 
}

#endif /* ASTREE_HPP */
