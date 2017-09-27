#ifndef APPENDER_H
#define APPENDER_H

#include <cstdio>
#include <string>

#include "../Define.hpp"

namespace tools
{
	namespace log
	{
		class Layout;
		_final class EXPORT_API Appender
		{
			public:
				/**
				 * Constructor
				 */
				Appender();
				Appender(Layout* layout, FILE* device=stderr);
				
				/**
				 * Destructor
				 */
				~Appender();
				
				/**
				 * Interface
				 */
				int getStateCode();
				void resetState();  
				bool resetDevice(FILE* device); 
				bool resetLayout(Layout* layout); 
				bool print(std::string msg= std::string());
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
				/**
				 * Property
				 */
				int state;		// Appender״̬: 0:����, -1:����, 2:������豸, 3:�豸д�����
				Layout *layout;
				FILE *device; 
		};
	}
}
#endif
