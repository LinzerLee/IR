#include "ASTree.hpp"
#include "../Lexer.hpp" 
#include "pimpl_ASTree.hpp" 

using namespace IRUtil::AST;

ASTree::ASTree()
{
	ASTree::pimpl = new pimpl_ASTree();
}
 
ASTree::~ASTree()
{
	if(ASTree::pimpl)
    {
        delete ASTree::pimpl;
        ASTree::pimpl = NULL;
    }
} 
				
void ASTree::init(IRUtil::Lexer *lexer, FILE *ivtIdxFp, std::map<std::string, unsigned long long> *dict)
{
	ASTree::pimpl->ivtIdxFp = ivtIdxFp;
	ASTree::pimpl->lexer = lexer;
	ASTree::pimpl->dictionary = dict;
} 
    	
void ASTree::init(const ASTree *tree)
{
	ASTree::pimpl->ivtIdxFp = tree->ASTree::pimpl->ivtIdxFp;
	ASTree::pimpl->lexer = tree->ASTree::pimpl->lexer;
	ASTree::pimpl->dictionary = tree->ASTree::pimpl->dictionary;
} 

bool ASTree::is_fresh()
{
	return ASTree::pimpl->is_fresh;
}
