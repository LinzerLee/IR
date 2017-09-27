#include "ASTUnaryExpr.hpp"
#include "pimpl_ASTUnaryExpr.hpp"

using namespace IRUtil::AST;

// @Override
bool ASTUnaryExpr::is_fresh()
{
	return ASTree::is_fresh() && ASTUnaryExpr::pimpl->expr->is_fresh();
}

void ASTUnaryExpr::release()
{
	if(ASTUnaryExpr::pimpl->expr)
	{
		ASTUnaryExpr::pimpl->expr->release();
		delete ASTUnaryExpr::pimpl->expr;
		ASTUnaryExpr::pimpl->expr = NULL;
	}
}

// Method
ASTUnaryExpr::ASTUnaryExpr()
{
	ASTUnaryExpr::pimpl = new pimpl_ASTUnaryExpr();
}
	
ASTUnaryExpr::~ASTUnaryExpr()
{
	if(ASTUnaryExpr::pimpl)
    {
        delete ASTUnaryExpr::pimpl;
        ASTUnaryExpr::pimpl = NULL;
    }		
}
		
void ASTUnaryExpr::setExpr(ASTExpression *expr)
{
	ASTUnaryExpr::pimpl->expr = expr; 
} 
