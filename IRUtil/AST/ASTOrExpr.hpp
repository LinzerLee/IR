/******************************************
 *
 *      NAME:   ASTOrExpr.hpp 
 *      DESC:   Combine two expression nodes
 *				into a expression by OR
 *				operator.
 *      Author: Linzer Lee
 *      Date:   2016/06/07
 *		Version:V1.0.0
 *
 ******************************************/

#ifndef ASTOREXPR_HPP
#define ASTOREXPR_HPP

#include <set>
#include "ASTBinaryExpr.hpp"

namespace IRUtil 
{
	namespace AST
	{ 
		class ASTExpression; 
		_final class ASTOrExpr _extends IRUtil::AST::ASTBinaryExpr
	    {
	    public:
	    	// @Override
	        _override std::set<unsigned long long> getDocSet();
	        // Method
	    };
    } 
}

#endif /* ASTOREXPR_HPP */
