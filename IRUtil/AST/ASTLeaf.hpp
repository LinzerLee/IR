/******************************************
 *
 *      NAME:   ASTLeaf.hpp 
 *      DESC:   Abstract Class to define
 *				Abstract Syntax Tree Leaf.
 *      Author: Linzer Lee
 *      Date:   2016/06/07
 *		Version:V1.0.0
 *
 ******************************************/

#ifndef ASTLEAF_HPP
#define ASTLEAF_HPP

#include "../Define.hpp"
#include "ASTree.hpp"

namespace IRUtil
{
	namespace AST
	{ 
	    _abstract class ASTLeaf _extends IRUtil::AST::ASTree
	    {
	    public:
	    	// Abstract Class ASTree's Member
	        using ASTree::getDocSet;
	    };
    } 
}

#endif /* ASTLEAF_HPP */
