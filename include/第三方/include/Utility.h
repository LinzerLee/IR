#pragma once

#include <cassert>
#include <string>
#include <vector>
#include <map>
#include <sstream>
#include <iostream>
#include <fstream>
#include "Define.hpp"

// ��־loader
EXPORT_API bool log_loader(std::map<std::string, std::vector<std::string>>&command_params);

// ��־unloader
EXPORT_API bool log_unloader();

// ����loader
EXPORT_API bool config_loader(std::map<std::string, std::vector<std::string>>&command_params);

// ����unloader
EXPORT_API bool config_unloader();

// loader
EXPORT_API bool loader(std::map<std::string, std::vector<std::string> >&command_params);

// unloader
EXPORT_API bool unloader();

template <class Type>
Type stringToNum(const std::string& str)
{
		std::istringstream iss(str);
        Type num;
        iss >> num;
        return num;
}

template<typename T>
void write_file(const char *filename, const T &text)
{
	std::ofstream fout;
	fout.open(filename, std::ios::app);
	if (fout.is_open())
	{
		fout << text;
	}
}

// ��ʼ��ʱ
EXPORT_API void StartTimer();

// ���ü�ʱ��
EXPORT_API void SetTimerPoint(double percentage);

// ��ӡ��־
EXPORT_API void Log(const char *fmt, ...);

// ���ö�ȡ
EXPORT_API std::vector<std::string> Config(const std::string &key);
EXPORT_API std::string ConfigFirst(const std::string &key);