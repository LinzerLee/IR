/******************************************
 *
 *      NAME:   SearchEngine
 *      DESC:   A daemon to provide query
 *				service. 
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
#include <json\json.h>
#include <Utility.h>
#include <tools/config/Parser.h>
#include <IRUtil/Lexer.hpp>
#include <IRUtil/SearchEngine.hpp>
#include <IRUtil/Inverter.hpp>
using namespace std;
using namespace IRUtil;
using namespace tools::config;

int usage()
{
	printf("usage: InvertIndex [Options] [Params]\n");
	printf("Options:\n");
	printf("-H	--help		Print this message and exit\n");
	printf("-D	--daemon process		Deamon process\n");
	printf("Params:\n");
	printf("-CFG	--config file		Config file path\n");
	printf("-LOG	--log file		Log file path\n");

	return (EXIT_FAILURE);
}

int main(int argc, char *argv[])
{
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
	
	// 参数检测
	if (command_params.count("D") > 0)
	{
		printf("Deamon process is not support.");
		printf("Exit Code : %d.", EXIT_SUCCESS);
		return EXIT_SUCCESS;
	}

	// Utility组件加载器
	printf("Lodding Utility component...\n");
	if (!loader(command_params))
	{
		printf("Lode Utility component incomplete.\n");
		printf("Error Code : %d\n", EXIT_FAILURE);
		return EXIT_FAILURE;
	}

	Log("[SearchService]:");
	Log("Lode Utility component complete.");
	StartTimer();

	Lexer lexer;
    lexer.setKeywords(ConfigFirst("keywords").c_str());
    
	SearchEngine se;
	se.setDocsPath(ConfigFirst("docs").c_str());
	se.setCachePath(ConfigFirst("cache").c_str());
	se.setInvertIndexPath(ConfigFirst("inverindex").c_str());
	se.setLexer(&lexer); 
	
	vector<string> &&qs = Config("query");
	string query;
	for (auto &q : qs)
	{
		query += q;
	}
	
	Log("Query:%s", query.c_str());
	
	std::vector<SearchEngine::SE_Result> &&results = se.search(query);
	/*
	auto tmp = Inverter::CreateDictionary("data/out/newout.ii");
	for (auto &t : *tmp)
	{
		printf("%s-->%u\n", t.first.c_str(), t.second);
	}
	delete tmp;
	*/
	// Json::Value &&json = SearchEngine::SE_Result_To_JSON(results);
	// std::string jstr = json.toStyledString;
	// Log("%s", jstr.c_str());

	for(auto &r : results) 
	{
		Log("title:%s", r.title.c_str()); 
		Log("digest:%s", r.digest.c_str());
		Log("url:%s", r.url.c_str());
		Log("snapshot:%s", r.snapshot.c_str());
	}

MAIN_END:
	// Utility组件卸载器
	Log("Unlodding Utility component...");
	Log("Exit Code : %d.", EXIT_SUCCESS);
	unloader();
	printf("Unlode Utility component complete.\n");
	return (EXIT_SUCCESS);
}



