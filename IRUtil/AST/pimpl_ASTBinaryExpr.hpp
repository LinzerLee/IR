#include "ASTBinaryExpr.hpp"

namespace IRUtil
{
	namespace AST
	{ 
	    class ASTBinaryExpr::pimpl_ASTBinaryExpr
	    {
	    public:
			ASTExpression *left{0};
			ASTExpression *right{0};
	    };
    } 
}
