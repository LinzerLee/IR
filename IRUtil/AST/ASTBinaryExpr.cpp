#include "ASTBinaryExpr.hpp"
#include "pimpl_ASTBinaryExpr.hpp"
#include "pimpl_ASTree.hpp"

using namespace IRUtil::AST;

// @Override
bool ASTBinaryExpr::is_fresh()
{
	return ASTree::is_fresh() && ASTBinaryExpr::pimpl->left->is_fresh() && ASTBinaryExpr::pimpl->right->is_fresh();
}

void ASTBinaryExpr::release()
{
	if(ASTBinaryExpr::pimpl->left)
	{
		ASTBinaryExpr::pimpl->left->release();
		delete ASTBinaryExpr::pimpl->left;
		ASTBinaryExpr::pimpl->left = NULL;
	}
	
	if(ASTBinaryExpr::pimpl->right)
	{
		ASTBinaryExpr::pimpl->right->release();
		delete ASTBinaryExpr::pimpl->right;
		ASTBinaryExpr::pimpl->right = NULL;
	}
	
}

// Method
ASTBinaryExpr::ASTBinaryExpr()
{
	ASTBinaryExpr::pimpl = new pimpl_ASTBinaryExpr();
}
 
ASTBinaryExpr::~ASTBinaryExpr()
{
	if(ASTBinaryExpr::pimpl)
    {
        delete ASTBinaryExpr::pimpl;
        ASTBinaryExpr::pimpl = NULL;
    }
} 	
		
void ASTBinaryExpr::setLeftExpr(ASTExpression *left)
{
	ASTBinaryExpr::pimpl->left = left; 
}
 
void ASTBinaryExpr::setRightExpr(ASTExpression *right)
{
	ASTBinaryExpr::pimpl->right = right; 
} 
