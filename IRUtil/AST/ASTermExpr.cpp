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
	// ASTermExprʹ��ASTerm��getDocSet() 
	
	return ASTermExpr::pimpl->term->getDocSet();
} 

bool ASTermExpr::is_fresh()
{
	// ASTermExpr�Ƿ�ˢ��������ASTerm�Ƿ�ˢ�� 
	// �̳���UnaryExpr��expr���ٱ�ʹ�� 
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
