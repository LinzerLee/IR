#include "pimpl_ASTUnaryExpr.hpp"
#include "ASTNotExpr.hpp"
#include "ASTExpression.hpp"

using namespace IRUtil::AST;

// @Override
std::set<unsigned long long> ASTNotExpr::getDocSet()
{
	// ASTermExprʹ��expr��getDocSet() 
	return ASTUnaryExpr::pimpl->expr->getDocSet();
} 

// Method
