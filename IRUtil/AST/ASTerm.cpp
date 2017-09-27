#include <string>
#include "pimpl_ASTree.hpp"
#include "ASTerm.hpp"
#include "../Inverter.hpp"
#include "../Lexer.hpp"

namespace IRUtil
{
	namespace AST
	{ 
	    class ASTerm::pimpl_ASTerm
	    {
	    public:
	        std::string term;
	    };
    } 
}

using namespace IRUtil::AST;

// @Override
std::set<unsigned long long> ASTerm::getDocSet()
{
	if(!is_fresh())
	{
		ASTree::pimpl->is_fresh = true; 
		ASTree::pimpl->doc_set.clear();
		if(!ASTree::pimpl->lexer->checkKeyword(ASTerm::pimpl->term.c_str()))
		{
			return ASTree::pimpl->doc_set;
		}
		
		std::map<unsigned long long, std::vector<unsigned int>> &&doc_pos = Inverter::ExtractTermInfo(ASTree::pimpl->ivtIdxFp, (*ASTree::pimpl->dictionary)[ASTerm::pimpl->term]);
		for(auto &d : doc_pos) 
		{
			ASTree::pimpl->doc_set.insert(d.first); 
		}
	}

	return ASTree::pimpl->doc_set;
}

// Method
ASTerm::ASTerm()
{
	ASTerm::pimpl = new pimpl_ASTerm();
}

ASTerm::~ASTerm()
{
	if(ASTerm::pimpl)
    {
        delete ASTerm::pimpl;
        ASTerm::pimpl = NULL;
    }
}

void ASTerm::setTerm(const std::string &term)
{
	ASTerm::pimpl->term = term;
	ASTree::pimpl->is_fresh = false; 
}
