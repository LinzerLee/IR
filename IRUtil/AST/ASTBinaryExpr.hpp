/******************************************
 *
 *      NAME:   ASTBinaryExpr.hpp 
 *      DESC:   Combine two expression nodes
 *				into a expression.  
 *      Author: Linzer Lee
 *      Date:   2016/06/07
 *		Version:V1.0.0
 *
 ******************************************/

#ifndef ASTBINARYEXPR_HPP
#define ASTBINARYEXPR_HPP

#include "../Define.hpp"
#include "ASTExpression.hpp"

namespace IRUtil 
{
	namespace AST
	{ 
	    _abstract class ASTBinaryExpr _extends IRUtil::AST::ASTExpression
	    {
	    public:
	    	// Abstract Class ASTree's Member
	        using ASTree::getDocSet;
			// @Override
	        _override bool is_fresh();
	        _override void release();
	        // Method
	        ASTBinaryExpr();
			virtual ~ASTBinaryExpr(); 
			void setLeftExpr(ASTExpression *left); 
			void setRightExpr(ASTExpression *right); 
		protected:
			class pimpl_ASTBinaryExpr;
			pimpl_ASTBinaryExpr *pimpl{0};
	    };
    } 
}

#endif /* ASTBINARYEXPR_HPP */
