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
	        std::string sign;			// ��ѯ���� 
	        ASTExpression::Priority priority{ASTExpression::Priority::Low};		// �������ȼ� 
	    	static std::map<std::string, ASTExpression *> regExpr;	// ע��ı��ʽ--�����Զ�������ʽģʽ     
	    };
	    std::map<std::string, ASTExpression *> ASTExpression::pimpl_ASTExpression::regExpr; 
    } 
}
