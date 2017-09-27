/******************************************
 *
 *      NAME:   ASTExpression.hpp 
 *      DESC:   Combine leaf nodes or 
 *				expression nodes into a
 *				expression.  
 *      Author: Linzer Lee
 *      Date:   2016/06/07
 *		Version:V1.0.0
 *
 ******************************************/


#ifndef ASTEXPRESSION_HPP
#define ASTEXPRESSION_HPP

#include <string>

#include "../Define.hpp"
#include "ASTree.hpp"

namespace IRUtil 
{
	namespace AST
	{ 
		_abstract class ASTExpression _extends IRUtil::AST::ASTree
	    {
	    	typedef char PriorityType;
	    public:
	    	enum class Priority: PriorityType
	    	{
	    		Low = 0,
	    		Middle,
				High
			}; 
	    	// Abstract Class ASTree's Member 
	        using ASTree::getDocSet;
	        // Abstract Method
			_override void release()=0;
	        // Method
	        ASTExpression();
			virtual ~ASTExpression(); 
			void setSign(const std::string &sign); 
			void setPriority(ASTExpression::Priority prior); 
	    protected:
			class pimpl_ASTExpression;
			pimpl_ASTExpression *pimpl{0};
	    };
    } 
}

#endif /* ASTEXPRESSION_HPP */
