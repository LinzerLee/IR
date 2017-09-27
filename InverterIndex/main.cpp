/******************************************
 *
 *      NAME:   InverterIndex
 *      DESC:   A daemon to construct 
 *				inverter index in a document
 *				set. 
 *      Author: Linzer Lee
 *      Date:   2016/06/02
 *      Version:V1.0.0
 *
 ******************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <vector>
#include <map>

#ifdef _WIN32
#include "dirent.h"
#else
#include <direct.h>
#include <unistd.h>
#endif

#include <Utility.h>
#include <tools/config/Parser.h>
#include <IRUtil/Lexer.hpp>
#include <IRUtil/Inverter.hpp>

using namespace std;
using namespace IRUtil;
using namespace tools::config;

int usage()
{
	printf("usage: InvertIndex [Options] [Params]\n");
	printf("Options:\n");
	printf("-H	--help		Print this message and exit\n");
	printf("-M	--merge *.ii files		some files or set -INPUT\n");
	printf("Params:\n");
	printf("-k	--keywords file		This file contains key words\n");
	printf("-INPUT	--doc set directory		Create invert index from doc set dir\n");
	printf("-o	--output file name(M)	Default is out.ii\n");
	printf("-OUTPUT	--output directory		Output invert index file will be here\n");
	printf("-CFG	--config file		Config file path\n");
	printf("-LOG	--log file		Log file path\n");

    return (EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
	bool merge_flag{false};

    if (argc<2)
	{
		return usage();
	}

	map<string, vector<string> > command_params = Parser::CommandParams(argc, argv);
	// H选项检测
	if (command_params.count("H") > 0)
	{
		usage();
		printf("Exit Code : %d.", EXIT_SUCCESS);
		return EXIT_SUCCESS;
	}
	// 合并倒排索引文件M选项检测
	if (command_params.count("M") > 0)
	{
		merge_flag = true;
	}
	// M标记参数检测
	if (command_params["o"].size() != 1)
	{
		return usage();
	}
	
	// Utility组件加载器
	printf("Lodding Utility component...\n");
	if (!loader(command_params))
	{
		printf("Lode Utility component incomplete.\n");
		printf("Error Code : %d\n", EXIT_FAILURE);
		return EXIT_FAILURE;
	}

	Log("[InverIndex]:");
	Log("Lode Utility component complete.");
	StartTimer();

	DIR* pdir = NULL;
	dirent* ptr = NULL;
	char path[512];
	Lexer lexer;
	Inverter inverter;
	string output_file, input_dir;

	if (command_params["INPUT"].size() == 1)
	{
		input_dir = command_params["INPUT"][0];
		auto pos = input_dir.size() - 1;

		if ('\\' == input_dir[pos] || '/' == input_dir[pos])
			input_dir.erase(pos, 1);
	}

	if (command_params["OUTPUT"].size() == 1)
	{
		string &output_dir = command_params["OUTPUT"][0];
		auto pos = output_dir.size() - 1;

		if ('\\' == output_dir[pos] || '/' == output_dir[pos])
			output_dir.erase(pos, 1);

		output_file = output_dir + "/" + command_params["o"][0];
	}
	else
	{
		output_file = command_params["o"][0];
	}

	if (merge_flag)
		goto MERGE_FLAG;

Inver_Index:
	Log("Construct inver index operator will be executed...");
	Log("Checking environment...");
	if (command_params["k"].size() < 1 || command_params["INPUT"].size() < 1)
	{
		usage();
		Log("Exit Code : %d.", EXIT_FAILURE);
		return EXIT_FAILURE;
	}
	Log("Check environment complete.");
	lexer.setKeywords(command_params["k"][0].c_str());
	inverter.setLexer(&lexer);

	input_dir = command_params["INPUT"][0];
	auto pos = input_dir.size() - 1;

	// 打开文档集 
	if (!(pdir = opendir(input_dir.c_str())))
	{
		Log("打开目录失败 : %s", input_dir.c_str());
		Log("Exit Code : %d.", EXIT_FAILURE);
		return EXIT_FAILURE;
	}

	// 循环取出文档集中的文档 
	while ((ptr = readdir(pdir)) != 0)
	{
		sprintf(path, "%s/%s", input_dir.c_str(), ptr->d_name);
		inverter.addDoc(atol(ptr->d_name), path);
		Log("%s construct inver index complete.", ptr->d_name);
	}

	closedir(pdir);

	inverter.serialize(output_file.c_str());
	Log("%s serialize complete.", output_file.c_str());

	inverter.clear();
	goto MAIN_END;

MERGE_FLAG:
	Log("Merge inver index operator will be executed...");
	Log("Checking environment...");
	// 至少合并两个倒排索引文件
	if (command_params["M"].size()==1)
	{
		Log("Merge file number not less than 2.");
		usage();
		Log("Exit Code : %d.", EXIT_FAILURE);
		return EXIT_FAILURE;
	}
	// 合并-M参数后的文件
	else if (command_params["M"].size() > 1)
	{
		Log("Check environment complete.");
MERGE_START:
		const char* temp_file = "_temp.ii";
		const char* out_file = output_file.c_str();
		Log("%s is merge complete...", command_params["M"][0].c_str());
		Inverter::Merge(command_params["M"][0].c_str(), command_params["M"][1].c_str(), out_file);
		Log("%s is merge complete...", command_params["M"][1].c_str());
		for (int i = 2; i < command_params["M"].size(); ++i)
		{
			const char* tmp = temp_file;
			temp_file = out_file;
			out_file = tmp;
			Inverter::Merge(temp_file, command_params["M"][i].c_str(), out_file);
			Log("%s is merge complete...", command_params["M"][i].c_str());
		}
		if (out_file != output_file)
		{
			const char* tmp = temp_file;
			temp_file = out_file;
			out_file = tmp;
			ifstream in;
			ofstream out;
			in.open(temp_file, ios::binary);//打开源文件
			if (in.fail())//打开源文件失败
			{
				Log("%s 打开失败", temp_file);
				Log("Exit Code : %d.", EXIT_FAILURE);
				return EXIT_FAILURE;
			}
			out.open(out_file, ios::binary);//创建目标文件
			if (out.fail())//创建文件失败
			{
				Log("%s 打开失败", out_file);
				Log("Exit Code : %d.", EXIT_FAILURE);
				return EXIT_FAILURE;
			}
			out << in.rdbuf();
			in.close();
			out.close();
			Log("Operator complete");
			SetTimerPoint(1.0f);
		}
	}
	// 合并-INPUT目录下的文件
	else if(command_params["INPUT"].size() < 1)
	{
		usage();
		Log("Exit Code : %d.", EXIT_FAILURE);
		return EXIT_FAILURE;
	}
	else
	{
		Log("Check environment complete.");
		// 打开倒排索引集
		if (!(pdir = opendir(command_params["INPUT"][0].c_str())))
		{
			Log("%s 打开失败", command_params["INPUT"][0].c_str());
			Log("Exit Code : %d.", EXIT_FAILURE);
			return EXIT_FAILURE;
		}
		command_params["M"].clear();
		// 循环取出倒排索引集中的文件 
		while ((ptr = readdir(pdir)) != 0)
		{
			if (0==strcmp(ptr->d_name, ".") || 0==strcmp(ptr->d_name, ".."))
				continue;
			sprintf(path, "%s/%s", input_dir.c_str(), ptr->d_name);
			command_params["M"].push_back(path);
		}
		goto MERGE_START;
	}
	
MAIN_END:
	// Utility组件卸载器
	Log("Unlodding Utility component...");
	Log("Exit Code : %d.", EXIT_SUCCESS);
	unloader();
	printf("Unlode Utility component complete.\n");
    return (EXIT_SUCCESS);
}



