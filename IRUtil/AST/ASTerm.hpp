/******************************************
 *
 *      NAME:   ASTerm.hpp 
 *      DESC:   Split query string in term,
 *				which is leaf node.
 *      Author: Linzer Lee
 *      Date:   2016/06/07
 *		Version:V1.0.0
 *
 ******************************************/

#ifndef ASTERM_HPP
#define ASTERM_HPP

#include <set>
#include "ASTLeaf.hpp"

namespace IRUtil 
{
	namespace AST
	{ 
		class ASTree;
	    class ASTerm _extends IRUtil::AST::ASTLeaf
	    {
	    public:
	    	ASTerm();
	    	virtual ~ASTerm();
	    	// @Override
	        _override std::set<unsigned long long> getDocSet();
	        // Method
	        void setTerm(const std::string &term);
	    private:
	        class pimpl_ASTerm;
	        pimpl_ASTerm *pimpl{0};
	    };
    } 
}

#endif /* ASTERM_HPP */
