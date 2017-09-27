#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <map>
#include <vector>

#include "../Define.hpp"

namespace tools
{
	namespace config
	{
		_final class EXPORT_API Parser
		{
			public:
				/**
				 * Static Interface 
				 */
				static std::map<std::string, std::vector<std::string> > CommandParams(int argc,char *argv[]);
				
				/**
				 * Constructor
				 */
				Parser();
				Parser(const char*cfgpath);
				
				/**
				 * Destructor
				 */
				~Parser();
				
				/**
				 * Interface
				 */ 
				int getStateCode();
				void resetState();  
				bool loadConfig(const char*cfg);
				std::string getContent();
				std::map<std::string, std::vector<std::string>>handle();
				void config(const std::string &key, const std::vector<std::string> &vaule);
				std::vector<std::string> config(const std::string &key);
				void mergeConfig(std::map<std::string, std::vector<std::string>> config);
			protected:
				/**
				 * Initialize
				 */
				bool _init();
				
				/**
				 * Utillity
				 */
				void setStateCode(int code);
				
			private:				
				// ������״̬: 0:����, 1:�����ļ�δ��ȷ���� 2:�����ļ��﷨���� 3:������ 
				int state;
				std::string filepath;
				std::string content;
				std::map<std::string, std::vector<std::string>> cfg;
		};
	}
}
#endif
