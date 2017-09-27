#include "ASTUnaryExpr.hpp"

namespace IRUtil
{
	namespace AST
	{ 
	    class ASTUnaryExpr::pimpl_ASTUnaryExpr
	    {
	    public:
			ASTExpression *expr{0};
	    };
    } 
}
