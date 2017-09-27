/******************************************
 *
 *      NAME:   ASTermExpr.hpp 
 *      DESC:   Combine two expression nodes
 *				into a expression by NOT
 *				operator.
 *      Author: Linzer Lee
 *      Date:   2016/06/07
 *		Version:V1.0.0
 *
 ******************************************/

#ifndef ASTERMEXPR_HPP
#define ASTERMEXPR_HPP

#include <set>
#include "ASTUnaryExpr.hpp"

namespace IRUtil 
{
	namespace AST
	{ 
		class ASTExpression; 
		class ASTerm; 
		_final class ASTermExpr _extends IRUtil::AST::ASTUnaryExpr
	    {
	    public:
	    	// @Override
	        _override std::set<unsigned long long> getDocSet();
	        _override bool is_fresh();
	        _override void release();
	        // Method
			ASTermExpr();
			virtual ~ASTermExpr(); 
	        void setTerm(ASTerm *term); 
	    private:
	        class pimpl_ASTermExpr;
	        pimpl_ASTermExpr *pimpl{0};
	    };
    } 
}

#endif /* ASTERMEXPR_HPP */
