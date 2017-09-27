#include "ASTree.hpp" 

namespace IRUtil
{
	namespace AST
	{ 
		class ASTree::pimpl_ASTree
	    {
	    public:
	        FILE *ivtIdxFp;
	        Lexer *lexer;
	        std::map<std::string, unsigned long long> *dictionary;
	        std::set<unsigned long long> doc_set;
	        bool is_fresh{false};
	    };
    } 
}
