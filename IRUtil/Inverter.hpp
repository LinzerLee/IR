/******************************************
 *
 *      NAME:   Inverter.hpp 
 *      DESC:   Component to construct 
 *				inverter index. 
 *      Author: Linzer Lee
 *      Date:   2016/06/02
 *		Version:V1.0.0
 *
 ******************************************/

#ifndef INVERTER_HPP
#define INVERTER_HPP

#include <string>
#include <vector>
#include <map>

#include "Define.hpp"

namespace IRUtil
{
	class Lexer; 
    class EXPORT_API Inverter
    {
    public:
        Inverter();
        virtual ~Inverter();
        
        void setLexer(Lexer *l); 
        void addTerms(unsigned long long doc_num, const std::map<std::string, std::vector<int>> &terms);
        void addDoc(unsigned long long doc_num, const char *filename);
        void serialize(const char *filename);
		// void print(const char *filename);
		void clear(); 
		
		static void Merge(const char *in1, const char *in2, const char *out);
		static std::map<std::string, unsigned long long> *CreateDictionary(const char *filename);
		static std::map<unsigned long long, std::vector<unsigned int>> ExtractTermInfo(FILE *ivtIdxFp, size_t offset);  
    private:
        class pimpl_Inverter;
        pimpl_Inverter *pimpl{0};
    };
}

#endif /* INVERTER_HPP */
