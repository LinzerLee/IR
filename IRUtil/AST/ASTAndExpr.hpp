/******************************************
 *
 *      NAME:   ASTAndExpr.hpp 
 *      DESC:   Combine two expression nodes
 *				into a expression by AND
 *				operator.
 *      Author: Linzer Lee
 *      Date:   2016/06/07
 *		Version:V1.0.0
 *
 ******************************************/

#ifndef ASTANDEXPR_HPP
#define ASTANDEXPR_HPP

#include <set>
#include "ASTBinaryExpr.hpp"

namespace IRUtil 
{
	namespace AST
	{ 
		class ASTExpression; 
		_final class ASTAndExpr _extends IRUtil::AST::ASTBinaryExpr
	    {
	    public:
	    	// @Override
	        _override std::set<unsigned long long> getDocSet();
	        // Method
	    };
    } 
}

#endif /* ASTANDEXPR_HPP */
