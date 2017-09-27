/******************************************
 *
 *      NAME:   ASTNotExpr.hpp 
 *      DESC:   Combine two expression nodes
 *				into a expression by NOT
 *				operator.
 *      Author: Linzer Lee
 *      Date:   2016/06/07
 *		Version:V1.0.0
 *
 ******************************************/

#ifndef ASTNOTEXPR_HPP
#define ASTNOTEXPR_HPP

#include <set>
#include "ASTUnaryExpr.hpp"

namespace IRUtil 
{
	namespace AST
	{ 
		class ASTExpression; 
		_final class ASTNotExpr _extends IRUtil::AST::ASTUnaryExpr
	    {
	    public:
	    	// @Override
	        _override std::set<unsigned long long> getDocSet();
	        // Method
	    };
    } 
}

#endif /* ASTNOTEXPR_HPP */
