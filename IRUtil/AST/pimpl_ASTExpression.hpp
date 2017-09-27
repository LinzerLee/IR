#include <map>
#include <string>

#include "ASTExpression.hpp" 

namespace IRUtil
{
	namespace AST
	{ 
	    class ASTExpression::pimpl_ASTExpression
	    {
	    public:
	        std::string sign;			// 查询符号 
	        ASTExpression::Priority priority{ASTExpression::Priority::Low};		// 符号优先级 
	    	static std::map<std::string, ASTExpression *> regExpr;	// 注册的表达式--用于自动构造表达式模式     
	    };
	    std::map<std::string, ASTExpression *> ASTExpression::pimpl_ASTExpression::regExpr; 
    } 
}
