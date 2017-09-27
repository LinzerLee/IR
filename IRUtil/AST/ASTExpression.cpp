#include "ASTExpression.hpp"
#include "pimpl_ASTExpression.hpp" 

using namespace IRUtil::AST;

// Method 
ASTExpression::ASTExpression()
{
	ASTExpression::pimpl = new pimpl_ASTExpression();
}
 
ASTExpression::~ASTExpression()
{
	if(ASTExpression::pimpl)
    {
        delete ASTExpression::pimpl;
        ASTExpression::pimpl = NULL;
    }
} 
		
void ASTExpression::setSign(const std::string &sign)
{
	ASTExpression::pimpl->sign = sign; 
}
 
void ASTExpression::setPriority(ASTExpression::Priority prior)
{
	ASTExpression::pimpl->priority = prior; 
} 

