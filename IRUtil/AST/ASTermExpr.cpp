#include "ASTermExpr.hpp"
#include "ASTExpression.hpp"
#include "ASTerm.hpp"
 
namespace IRUtil
{
	namespace AST
	{ 
	    class ASTermExpr::pimpl_ASTermExpr
	    { 
	    public: 
	    	ASTerm *term{0}; 
	    };
    } 
}

using namespace IRUtil::AST;

// @Override
std::set<unsigned long long> ASTermExpr::getDocSet()
{
	// ASTermExpr使用ASTerm的getDocSet() 
	
	return ASTermExpr::pimpl->term->getDocSet();
} 

bool ASTermExpr::is_fresh()
{
	// ASTermExpr是否刷新依赖于ASTerm是否刷新 
	// 继承于UnaryExpr的expr不再被使用 
	return ASTermExpr::pimpl->term->is_fresh();
}

void ASTermExpr::release()
{
	if(ASTermExpr::pimpl->term)
	{
		delete ASTermExpr::pimpl->term;
		ASTermExpr::pimpl->term = NULL;
	}
}
// Method
ASTermExpr::ASTermExpr()
{
	ASTermExpr::pimpl = new pimpl_ASTermExpr();
}
 
ASTermExpr::~ASTermExpr()
{
	if(ASTermExpr::pimpl)
    {
        delete ASTermExpr::pimpl;
        ASTermExpr::pimpl = NULL;
    }
}

void ASTermExpr::setTerm(ASTerm *term)
{
	ASTermExpr::pimpl->term = term; 
} 
