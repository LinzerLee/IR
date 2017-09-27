/******************************************
 *
 *      NAME:   NetSpider
 *      DESC:   A utility to get net pages
 *      Author: Linzer Lee
 *      Date:   2016/06/03
 *      Version:V1.0.0
 *
 ******************************************/

#include <sys/types.h>
#include <sys/stat.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <string>
#include <vector>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h>

#ifdef _WIN32
extern "C" {
	extern int getopt(int, char * const *, const char *);
	extern char	*optarg;
}
#else
#include <unistd.h>
#endif

#include <db_cxx.h>

#include "NetUtil/Net.hpp"
#include "NetUtil/HtmlParser.hpp"

using std::cin;
using std::cout;
using std::cerr;
using std::endl;
using std::string;
using std::vector;
using std::ofstream;

using NetUtil::Net;
using NetUtil::HtmlParser;

#define	IR_LINK_TODO        "ir_link_todo.db"
#define	IR_LINK_VISITED     "ir_link_visited.db"
#define	IR_DOC_LINK         "ir_doc_link.db"

#define SEARCH_LIMIT_NO     1000L

Db todo_db(NULL, NULL);
Db visited_db(NULL, NULL);
Db doc_link_db(NULL, NULL);

unsigned long long link_num{ 0 }, link_cur{ 0 }, doc_num{ 0 };
char *url_buf{ NULL };
size_t url_buf_size{ 0 };
const char *docs_path = "docs";
const char *cache_path = "cache";
const char *scope = NULL;

int usage()
{
	fprintf(stderr, "usage: NetSpider [-u] [url link]\n");
	fprintf(stderr, "[-s] [url scope limit]\n");
	return (EXIT_FAILURE);
}

void adjust_url_buf(size_t nsize)
{
	auto new_size = url_buf_size;
	if (0 == new_size)
	{
		new_size = 256;
	}

	while (new_size<nsize)
	{
		new_size <<= 1;
	}

	if (new_size>url_buf_size)
	{
		if (0 == url_buf_size)
			url_buf = (char *)malloc(new_size);
		else
			url_buf = (char *)realloc(url_buf, new_size);

		url_buf_size = new_size;
	}
}

size_t load_url_to_buf(const char *url)
{
	size_t len = strlen(url) + 1;
	adjust_url_buf(len);
	memcpy(url_buf, url, len);

	return len;
}

string valid_link(const char *url)
{
	string vlink(url);
	auto pos = vlink.find('?');
	if (string::npos != pos)
		vlink.erase(vlink.begin() + pos, vlink.end());

	pos = vlink.find('#');
	if (string::npos != pos)
		vlink.erase(vlink.begin() + pos, vlink.end());

	return vlink;
}

bool verify_link(const char *url)
{
	string && vlink = valid_link(url);
	Dbt key, data;
	const char *dstr = vlink.c_str();
	auto len = strlen(dstr) + 1;
	char *buffer = (char *)malloc(len);

	if (!buffer)
		return false;

	memset(buffer, 0, len);
	memcpy(buffer, dstr, len - 1);
	key.set_data(buffer);
	key.set_size(len);

	int ret = visited_db.get(NULL, &key, &data, 0);
	if (ret)
	{
		if (!scope)
		{
			free(buffer);
			return true;
		}

		if (strlen(url) >= strlen(scope))
		{
			int pos = 0;
			while (scope[pos] != '\0' && scope[pos] == url[pos])
			{
				++pos;
			}

			if (scope[pos] == '\0')
			{
				free(buffer);
				return true;
			}
		}
	}

	free(buffer);
	return false;
}

void write_file(const char *cache_path, const char *docs_path, unsigned long long id, string & content)
{
	char filename[32];
	sprintf(filename, "%s/%llu", cache_path, id);
	ofstream fout;
	fout.open(filename);
	if (fout.is_open())
	{
		fout << content;
		fout.close();

		sprintf(filename, "%s/%llu", docs_path, id);
		fout.open(filename);
		if (fout.is_open())
		{
			long pos{ 0 }, len = content.size();
			bool tag_start_flag{ false }, tag_end_flag{ true }, wspace_flag{ false }, tag_delete_flag{ false };
			while (pos<len)
			{
				if (iswspace(content[pos]))
				{
					if (!wspace_flag)
					{
						fout << ' ';
						wspace_flag = true;
					}
					++pos;
					continue;
				}

				switch (content[pos])
				{
				case '<':
					if (tag_delete_flag)
					{
						++pos;
						continue;
					}

					tag_start_flag = true;
					tag_end_flag = false;

					tag_delete_flag = true;
					if (pos + 6>len || "script" == content.substr(pos + 1, 6))
					{
						pos += 6;
						continue;
					}
					else if (pos + 8>len || "noscript" == content.substr(pos + 1, 8))
					{
						pos += 8;
						continue;
					}
					else if (pos + 5>len || "style" == content.substr(pos + 1, 5))
					{
						pos += 5;
						continue;
					}
					/*
					else if(pos+5>len || "input"==content.substr(pos+1, 5))
					{
					pos += 5;
					continue;
					}
					else if(pos+8>len || "textarea"==content.substr(pos+1, 8))
					{
					pos += 8;
					continue;
					}
					*/
					else if (pos + 4>len || "code" == content.substr(pos + 1, 4))
					{
						pos += 4;
						continue;
					}
					else if (pos + 1>len || '#' == content[pos + 1])
					{
						pos += 1;
						continue;
					}

					tag_delete_flag = false;
					break;
				case '>':
					if (tag_delete_flag)
					{
						if (pos - 7>0 && "/script" == content.substr(pos - 7, 7))
						{
							tag_delete_flag = false;
						}
						else if (pos - 9>0 && "/noscript" == content.substr(pos - 9, 9))
						{
							tag_delete_flag = false;
						}
						else if (pos - 6>0 && "/style" == content.substr(pos - 6, 6))
						{
							tag_delete_flag = false;
						}
						/*
						else if(pos-6>0 && "/input"==content.substr(pos-6, 6))
						{
						tag_delete_flag = false;
						}
						else if(pos-9>0 && "/textarea"==content.substr(pos-9, 9))
						{
						tag_delete_flag = false;
						}
						*/
						else if (pos - 5>0 && "/code" == content.substr(pos - 5, 5))
						{
							tag_delete_flag = false;
						}
						else if (pos - 2>0 && "/#" == content.substr(pos - 2, 2))
						{
							tag_delete_flag = false;
						}
					}

					if (tag_start_flag)
					{
						tag_end_flag = true;
						tag_start_flag = false;
					}
					break;
				case '&':
					if (!tag_delete_flag && tag_end_flag)
					{
						while (++pos<len && ';' != content[pos]);
					}
					break;
				default:
					if (!tag_delete_flag && tag_end_flag)
					{
						fout << content[pos];
						wspace_flag = false;
					}
				}

				++pos;
			}

			fout.close();
		}
	}
}

bool is_static_page(const char *url)
{
	string link(url);

	auto pos = link.find_last_of('.');
	if (string::npos != pos)
	{
		++pos;
		auto suffix = link.substr(pos, link.size() - pos);

		if ("html" == suffix || "htm" == suffix)
			return true;
	}

	return false;
}

int main(int argc, char *argv[])
{
	int ch;
	const char *url = NULL;

	while ((ch = getopt(argc, argv, "u:s:")) != EOF)
		switch (ch)
		{
		case 'u':
			url = optarg;
			break;
		case 's':
			scope = optarg;
			break;
		case '?':
		default:
			return (usage());
		}

#ifdef _WIN32
	mkdir(docs_path);
	mkdir(cache_path);
#else 
	mkdir(docs_path, 0777);
	mkdir(cache_path, 0777);
#endif 

	// TODO
	todo_db.set_error_stream(&cerr);
	todo_db.set_errpfx("NetSpider");
	todo_db.set_pagesize(1024);		/* Page size: 1K. */
	todo_db.set_cachesize(0, 32 * 1024, 0);
	todo_db.open(NULL, IR_LINK_TODO, NULL, DB_BTREE, DB_CREATE, 0664);

	// VISITED
	visited_db.set_error_stream(&cerr);
	visited_db.set_errpfx("NetSpider");
	visited_db.set_pagesize(1024);		/* Page size: 1K. */
	visited_db.set_cachesize(0, 32 * 1024, 0);
	visited_db.open(NULL, IR_LINK_VISITED, NULL, DB_BTREE, DB_CREATE, 0664);

	// DOC TO LINK
	doc_link_db.set_error_stream(&cerr);
	doc_link_db.set_errpfx("NetSpider");
	doc_link_db.set_pagesize(1024);		/* Page size: 1K. */
	doc_link_db.set_cachesize(0, 32 * 1024, 0);
	doc_link_db.open(NULL, IR_DOC_LINK, NULL, DB_BTREE, DB_CREATE, 0664);

	Net net;
	HtmlParser htmlParser;

	Net::Net_Init_Env();

	if (url == NULL)
	{
		return (usage());
	}
	adjust_url_buf(strlen(url));

	int ret;
	Dbt key, data, doc_link_key, null_data(NULL, 0);
	key.set_data(&++link_num);
	key.set_size(sizeof(unsigned long long));
	data.set_data(url_buf);
	data.set_size(load_url_to_buf(url));

	todo_db.put(0, &key, &data, DB_NOOVERWRITE);

	try
	{
		const char *url;
		while (doc_num<SEARCH_LIMIT_NO && link_cur<link_num)
		{
			key.set_data(&++link_cur);
			key.set_size(sizeof(unsigned long long));
			if (todo_db.get(NULL, &key, &data, 0) != 0)
			{
				todo_db.del(NULL, &key, 0);
				continue;
			}

			url = (const char *)data.get_data();
			if (!verify_link(url))
				continue;

			string && page = net.FetchPage(url);
			// ±£´æ¾²Ì¬ÎÄµµ
			if (is_static_page(url))
			{
				write_file(cache_path, docs_path, ++doc_num, page);
				cout << "No." << doc_num << " " << url << endl;
				doc_link_key.set_data(&doc_num);
				doc_link_key.set_size(sizeof(unsigned long long));
				doc_link_db.put(0, &doc_link_key, &data, DB_NOOVERWRITE);
			}

			string && vlink = valid_link(url);

			data.set_data(url_buf);
			data.set_size(load_url_to_buf(vlink.c_str()));
			visited_db.put(0, &data, &null_data, DB_NOOVERWRITE);
			todo_db.del(NULL, &key, 0);

			if (NET_OK == net.ErrorCode())
			{
				htmlParser.setHtmlText(page);
				vector<string> &&links = htmlParser.extractLink();
				for (auto it = links.begin(); it != links.end(); ++it)
				{
					if (!verify_link(it->c_str()))
						continue;
					key.set_data(&++link_num);

					data.set_data(url_buf);
					data.set_size(load_url_to_buf(it->c_str()));
					// cout << "extract("<< doc_num << "): " << url_buf << endl;
					if (0 != todo_db.put(0, &key, &data, DB_NOOVERWRITE))
					{
						--link_num;
					}
				}
			}
		}
	}
	catch (DbException &dbe)
	{
		todo_db.close(0);
		visited_db.close(0);
		doc_link_db.close(0);
		cerr << "NetSpider: " << dbe.what() << "\n";
		return (EXIT_FAILURE);
	}
	todo_db.close(0);
	visited_db.close(0);
	doc_link_db.close(0);
	Net::Release();
	return (EXIT_SUCCESS);
}
