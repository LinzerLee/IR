#include <algorithm>
#include <iterator>

#include "pimpl_ASTree.hpp"
#include "pimpl_ASTBinaryExpr.hpp"
#include "ASTOrExpr.hpp"
#include "ASTExpression.hpp"

using namespace IRUtil::AST;

// @Override
std::set<unsigned long long> ASTOrExpr::getDocSet()
{
	if(is_fresh())
		return ASTree::pimpl->doc_set;
		
	ASTree::pimpl->doc_set.clear();
	auto &&left = ASTBinaryExpr::pimpl->left->getDocSet();
	auto &&right = ASTBinaryExpr::pimpl->right->getDocSet();
	std::set_union(left.begin(), left.end(), right.begin(), right.end(), std::inserter(ASTree::pimpl->doc_set, ASTree::pimpl->doc_set.begin()));
	ASTree::pimpl->is_fresh = true;
	
	return ASTree::pimpl->doc_set; 
} 

// Method
