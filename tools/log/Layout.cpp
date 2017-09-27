#define DLL_IMPL

#include <ctime>
#include <string>
#include <map>

#include "Layout.h"

using tools::log::Layout;
using std::string;
using std::map;

/**
 * Constructor
 */
Layout::Layout()
{
	_init();
	this->pattern = string("");
	this->params = map<string, string>();
}

Layout::Layout(string pattern, map<string, string> params/*=map<string, string>()*/)
{
	_init();
	this->pattern = pattern;
	this->params = params;
}

/**
 * Destructor
 */
Layout::~Layout()
{
}

/**
 * Initialize
 */
bool Layout::_init()
{
	// ��ʼ��״̬
	state = 0;
	// Ĭ�ϲ���
	default_params["a"] = "";		// ���ڼ�����д
	default_params["A"] = "";		// ���ڼ���ȫ��
	default_params["b"] = "";		// �·����Ƶ���д
	default_params["B"] = "";		// �·����Ƶ�ȫ��
	default_params["d"] = "";		// �����ֱ�ʾ���µĵڼ��� (��ΧΪ 00 �� 31)--����
	default_params["H"] = "";		// �� 24 Сʱ�����ֱ�ʾСʱ�� (��ΧΪ 00 �� 23)
	default_params["I"] = "";		// �� 12 Сʱ�����ֱ�ʾСʱ�� (��ΧΪ 01 �� 12)
	default_params["j"] = "";		// �����ֱ�ʾ����ȵĵڼ��� (��ΧΪ 001 �� 366)
	default_params["m"] = "";		// �·ݵ����� (��Χ�� 1 �� 12)
	default_params["c"] = "";		// ���ض�����ʱ��ϼѱ�ʾ�ַ���
	default_params["M"] = "";		// ����
	default_params["p"] = "";		// �� AM �� PM ��ʾ���ض�ʱ��
	default_params["S"] = "";		// ����
	default_params["U"] = "";		// ���ֱ�ʾΪ����ȵĵڼ��ܣ���һ�������ɵ�һ�����տ�ʼ
	default_params["W"] = "";		// ���ֱ�ʾΪ����ȵĵڼ��ܣ���һ�������ɵ�һ����һ��ʼ
	default_params["w"] = "";		// �����ֱ�ʾ���ܵĵڼ��� ( 0 Ϊ����)
	default_params["x"] = "";		// ����ʱ������ڱ�ʾ��
	default_params["X"] = "";		// �������ڵ�ʱ���ʾ��
	default_params["y"] = "";		// ��λ���ֱ�ʾ��� (��Χ�� 00 �� 99)
	default_params["Y"] = "";		// ������������ֱ�ʾ������λ��
	default_params["Z"] = "";		// ʱ����������д
	default_params["news"] = "";		// ˵��
	
	return true; 
}

/**
 * Utillity
 */
void Layout::setStateCode(int code)
{
	state = code;
}

/**
 * Interface
 */
int Layout::getStateCode()
{
	return state;
}

void Layout::resetState()
{
	setStateCode(0);
}

void Layout::resetPattern(string pattern)
{
	this->pattern = pattern;
	this->resetState();
}

string Layout::getPattern()
{
	return this->pattern;
}

void Layout::resetParams(map<string, string> params)
{
	this->params = params;
	this->resetState();
}

map<string, string> Layout::getParams()
{
	return this->params;
}

void Layout::resetParam(string key, string value)
{
	this->params[key] = value;
	this->resetState();
}

string Layout::getParam(string key)
{
	return this->params[key];
}

string Layout::parser(string msg/*=string()*/)
{
	if(string("")==this->pattern)
	{
		return pattern;
	}

	string::size_type len = this->pattern.size();
	string::size_type pos = this->pattern.find('%');

	if(string::npos==pos)
	{
		return pattern;
	}

	string result;
	string::size_type lastpos=0, l=0;
	while(string::npos!=pos && pos<len)
	{
		result += pattern.substr(lastpos, pos-lastpos);
		if(pos+1<len)
		{
			if('%'==pattern[pos+1])
			{
				result += "%";
				lastpos = pos+2;
			}
			else
			{
				// ���ҿ��滻�ַ�
				// 1:��params�в���
				map<string, string>::iterator it = this->params.begin();
				for(; it!=this->params.end(); ++it)
				{
					l = it->first.size();
					if(pos+l>=len)
					{
						continue;
					}

					string tmp = pattern.substr(pos+1, l);
					// �ҵ����滻����
					if(tmp==it->first)
					{
						result += it->second;
						lastpos = pos+l+1;
						break;
					}
				}

				// 2:params��δ�ҵ�,ȡϵͳĬ�ϵ�
				if(it==this->params.end())
				{
					for(it=default_params.begin(); it!=default_params.end(); ++it)
					{
						l = it->first.size();
						if(pos+l>=len)
						{
							continue;
						}

						string tmp = pattern.substr(pos+1, l);

						// �ҵ����滻����
						if(tmp==it->first)
						{
							if("news"==tmp)
							{
								result += msg;
							}
							else
							{
								time_t rawtime;
								time(&rawtime);
								char t[64];
								strftime(t, sizeof(t), string("%"+tmp).c_str(), localtime(&rawtime));
								result += t;
							}

							lastpos = pos+l+1;
							break;
						}
					}
				}

				// 3:��δ�ҵ�
				if(it==this->default_params.end())
				{
					result += "%";
					lastpos = pos+1;
				}
			}

			pos = this->pattern.find('%', lastpos);
		}
		else
		{
			break;
		}
	}

	if(lastpos<len)
	{
		result += pattern.substr(lastpos);
	}
	return result;
}

