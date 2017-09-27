/******************************************
 *
 *      NAME:   ASTUnaryExpr.hpp 
 *      DESC:   Change operator of a
 *				expression node. 
 *      Author: Linzer Lee
 *      Date:   2016/06/07
 *		Version:V1.0.0
 *
 ******************************************/

#ifndef ASTUNARYEXPR_HPP
#define ASTUNARYEXPR_HPP

#include "../Define.hpp"
#include "ASTExpression.hpp"

namespace IRUtil 
{
	namespace AST
	{ 
	    _abstract class ASTUnaryExpr _extends IRUtil::AST::ASTExpression
	    {
	    public:
	    	// Abstract Class ASTree's Member
	        using ASTree::getDocSet;
			// @Override
	        _override bool is_fresh();
	        _override void release();
			// Method
			ASTUnaryExpr();
			virtual ~ASTUnaryExpr();
			void setExpr(ASTExpression *expr); 
		protected:
			class pimpl_ASTUnaryExpr;
			pimpl_ASTUnaryExpr *pimpl{0};
	    };
    } 
}

#endif /* ASTUNARYEXPR_HPP */
