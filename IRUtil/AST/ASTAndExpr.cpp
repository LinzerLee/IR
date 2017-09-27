#include <iterator>
#include <algorithm>
#include <typeinfo>

#include "pimpl_ASTree.hpp"
#include "pimpl_ASTBinaryExpr.hpp"
#include "ASTNotExpr.hpp"
#include "ASTAndExpr.hpp"
#include "ASTExpression.hpp"

using namespace IRUtil::AST;

// @Override
std::set<unsigned long long> ASTAndExpr::getDocSet()
{
	if(is_fresh())
		return ASTree::pimpl->doc_set;
		
	ASTree::pimpl->doc_set.clear();
	auto &&left = ASTBinaryExpr::pimpl->left->getDocSet();
	auto &&right = ASTBinaryExpr::pimpl->right->getDocSet();
	auto left_hash_code = typeid(*ASTBinaryExpr::pimpl->left).hash_code();
	auto right_hash_code = typeid(*ASTBinaryExpr::pimpl->right).hash_code(); 
	if(left_hash_code==typeid(ASTNotExpr).hash_code() || right_hash_code==typeid(ASTNotExpr).hash_code())
	{
	   	if(left_hash_code!=typeid(ASTNotExpr).hash_code() && right_hash_code==typeid(ASTNotExpr).hash_code()) 
	   	{
	   		std::set_difference(left.begin(), left.end(), right.begin(), right.end(), std::inserter(ASTree::pimpl->doc_set, ASTree::pimpl->doc_set.begin()));
		} 
		else if(left_hash_code==typeid(ASTNotExpr).hash_code() && right_hash_code!=typeid(ASTNotExpr).hash_code()) 
		{
			std::set_difference(right.begin(), right.end(), left.begin(), left.end(), std::inserter(ASTree::pimpl->doc_set, ASTree::pimpl->doc_set.begin()));
		} 
	}
	else
	{
		std::set_intersection(left.begin(), left.end(), right.begin(), right.end(), std::inserter(ASTree::pimpl->doc_set, ASTree::pimpl->doc_set.begin()));
	} 
	
	ASTree::pimpl->is_fresh = true;
	return ASTree::pimpl->doc_set; 
} 

