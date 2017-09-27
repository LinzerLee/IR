#define DLL_IMPL

#include <map>
#include <cstring>

#include "Inverter.hpp"
#include "Lexer.hpp"
 
namespace IRUtil
{
    class Inverter::pimpl_Inverter
    {
    public:
    	Lexer *lexer{NULL}; 
    	std::map<std::string, std::vector<std::pair<int, std::vector<int>>>> iindex;
    };
    
	#pragma pack(push, 1)    
    typedef unsigned short  _Int16;
	typedef unsigned int    _Int32;
	typedef unsigned char   _Byte;
	
	/*
	 * 词典文件头24B各字段说明：(共20个字节)
	 * Magic：4B：0x1A 2B 3C 4D:用来标示文件的开始
	 * Major：2B，0x01 00:当前文件主要的版本号
	 * Minor：2B，0x01 00当前文件次要的版本号
	 * ThisZone：4B当地的标准时间；全零
	 * SigFigs：4B时间戳的精度；全零
	 * SnapLen：4B最大的存储长度
	 */
	
	typedef struct _ivt_idx_file_header
	{
	        _Int32  magic;
	        _Int16  version_major;
	        _Int16  version_minor;
	        _Int32  thiszone;
	        _Int32  sigfigs;
	        _Int32  snaplen;
	}ivt_idx_file_header, *pivt_idx_file_header;
	
	/*
	 * 词典中词项信息 (共8个字节)
	 * 字段说明：
	 * Termlen：4B 词项信息的总长度
	 * Df：2B 文档频率 
	 * Len：1B 词项字符串长度 包括'\0'
	 * State：1B 词项状态(为零)
	 * 以下开始为可变长度
	 * Name[Len] 词项字符串
	 * 以下三种信息交替重复 
	 * DocID: 4B 文档ID
	 * Size: 2B 位置个数
	 * [Pos1(2B), Pos2(2B), Pos3(2B), ...] Size个位置信息 
	 */
 
	typedef struct _ivt_idx_term_hdr{
        _Int32 termlen;
        _Int16 df; 
        _Byte len;
        _Byte status;
	}ivt_idx_term_hdr, *pivt_idx_term_hdr;
	#pragma pack(pop)
}

using namespace IRUtil;

Inverter::Inverter()
{
    pimpl = new pimpl_Inverter();
}

Inverter::~Inverter()
{
    if(pimpl)
    {
        delete pimpl;
        pimpl = NULL;
    }
}

void Inverter::setLexer(Lexer *l)
{
	pimpl->lexer = l; 
} 

void Inverter::addTerms(unsigned long long doc_num, const std::map<std::string, std::vector<int>> &terms)
{
	for(auto it=terms.begin(); it!=terms.end(); ++it)
	{
		auto &docs = pimpl->iindex[it->first];
		auto iit=docs.begin();
		for(; iit!=docs.end(); ++iit)
		{
			if(iit->first<doc_num)
				continue;
			
			if(iit->first==doc_num)
				return;
				
			break;
		}
		
		docs.insert(iit, std::make_pair(doc_num, it->second));
	}
} 

void Inverter::addDoc(unsigned long long doc_num, const char *filename)
{
	addTerms(doc_num, pimpl->lexer->getTerms(filename));
} 

void Inverter::serialize(const char *filename)
{
	FILE *pFile = fopen(filename, "wb");
	if(NULL==pFile)
    {
        printf("创建ii文件失败\n");
        return;
    }
          
	ivt_idx_file_header dfh;
	dfh.magic = 0X1A2B3C4D;
	dfh.version_major = 0X0100;
	dfh.version_minor = 0X0100;
	dfh.thiszone = 0X00;
	dfh.sigfigs = 0X00;
	dfh.snaplen = sizeof(ivt_idx_file_header); 
	
	fwrite((void*)&dfh, 1, sizeof(ivt_idx_file_header), pFile);
	
	ivt_idx_term_hdr dth;
	for(auto it=pimpl->iindex.begin(); it!=pimpl->iindex.end(); ++it)
	{
		dth.termlen = sizeof(ivt_idx_term_hdr);
		dth.df = it->second.size();
		dth.len = strlen(it->first.c_str()) + 1; 
		dth.status = 0;
		
		fwrite((void*)&dth, 1, sizeof(ivt_idx_term_hdr), pFile);
		
		fwrite((void*)it->first.c_str(), 1, dth.len, pFile);
		dth.termlen += dth.len;
		
		auto &docs = it->second;
		_Int32 docID; 
		_Int16 size;
		_Int16 pos; 
		for(auto iit=docs.begin(); iit!=docs.end(); ++iit)
		{
			docID = iit->first;
			size = iit->second.size(); 
			fwrite((void*)&docID, 1, sizeof(_Int32), pFile);
			fwrite((void*)&size, 1, sizeof(_Int16), pFile);
			dth.termlen += sizeof(_Int32) + sizeof(_Int16);
			for(int i=0; i<size; ++i)
			{
				pos = iit->second[i];
				fwrite((void*)&pos, 1, sizeof(_Int16), pFile);
			}
			dth.termlen += sizeof(_Int16) * size;
		}
		
		fseek(pFile, -dth.termlen, SEEK_CUR); 
		fwrite((void*)&dth, 1, sizeof(ivt_idx_term_hdr), pFile);
		fseek(pFile, dth.termlen-sizeof(ivt_idx_term_hdr), SEEK_CUR); 
		
		dfh.snaplen += dth.termlen;
	}
	
	fseek(pFile, 0, SEEK_SET); 
	fwrite((void*)&dfh, 1, sizeof(ivt_idx_file_header), pFile);
		
	fclose(pFile); 
}

void get_ivt_idx_term_info(FILE *fp, ivt_idx_term_hdr *phdr, void **pbuf, size_t& buf_size, size_t& content_len)
{
	fread((void*)phdr, sizeof(ivt_idx_term_hdr), 1, fp);
	content_len = phdr->termlen - sizeof(ivt_idx_term_hdr);
	if(buf_size<content_len) 
	{
		free(*pbuf);
		*pbuf = malloc(content_len);
		buf_size = content_len;
	}
	fread(*pbuf, content_len, 1, fp);
	
	return;
}

void Inverter::Merge(const char *in1, const char *in2, const char *out)
{
	long iFileLen1{0L}, iFileLen2{0L}, iFileLenOut{0L};
	// In1 
	FILE *pf1 = fopen(in1, "rb");
	if(NULL==pf1)
    {
        printf("打开%s文件失败\n", in1);
        return;
    }
    
    fseek(pf1, 0, SEEK_END);
    iFileLen1 = ftell(pf1);
    fseek(pf1, 0, SEEK_SET);
    
    if(iFileLen1<sizeof(ivt_idx_file_header))
    {
        printf("%s文件格式错误\n", in1);
        return;
    }
    // In2 
    FILE *pf2 = fopen(in2, "rb");
	if(NULL==pf2)
    {
        printf("打开%s文件失败\n", in2);
        return;
    }
    
    fseek(pf2, 0, SEEK_END);
    iFileLen2 = ftell(pf2);
    fseek(pf2, 0, SEEK_SET);
    
    if(iFileLen2<sizeof(ivt_idx_file_header))
    {
        printf("%s文件格式错误\n", in2);
        return;
    }
    
    // Out 
	FILE *pfOut = fopen(out, "wb");
	if(NULL==pfOut)
    {
        printf("打开%s文件失败\n", out);
        return;
    }
	
    ivt_idx_file_header dfh1, dfh2, dfhOut;
    
    fread((void*)&dfh1, sizeof(ivt_idx_file_header), 1, pf1);
    if(iFileLen1!=dfh1.snaplen)
    {
        printf("%s文件格式错误\n", in1);
        return;
    }
    
    fread((void*)&dfh2, sizeof(ivt_idx_file_header), 1, pf2);
    if(iFileLen2!=dfh2.snaplen)
    {
        printf("%s文件格式错误\n", in2);
        return;
    }
    // 写入合并文件头信息 
    dfhOut.magic = 0X1A2B3C4D;
	dfhOut.version_major = 0X0100;
	dfhOut.version_minor = 0X0100;
	dfhOut.thiszone = 0X00;
	dfhOut.sigfigs = 0X00;
	dfhOut.snaplen = sizeof(ivt_idx_file_header); 
	fwrite((void*)&dfhOut, 1, sizeof(ivt_idx_file_header), pfOut);
	
	ivt_idx_term_hdr dth1, dth2, dthTemp;
	char *buf1{NULL}, *buf2{NULL};
	size_t buf1_size{0}, buf2_size{0};
	size_t len1{0}, len2{0};
	
	// 认为第一个之前是相等的 
	int ret = 0; 
	int offset1, offset2;
	while(ftell(pf1)<dfh1.snaplen || ftell(pf2)<dfh2.snaplen)
	{
		// 上次比较结果 
		if(ret<0) 
		{
			if(ftell(pf1)>=dfh1.snaplen)
				break;
			// 读文件1的词项信息 
			get_ivt_idx_term_info(pf1, &dth1, (void **)&buf1, buf1_size, len1);
		} 
		else if(ret>0) 
		{
			if(ftell(pf2)>=dfh2.snaplen)
				break;
			// 读文件2的词项信息
			get_ivt_idx_term_info(pf2, &dth2, (void **)&buf2, buf2_size, len2);
		}
		else
		{
			if(ftell(pf1)>=dfh1.snaplen || ftell(pf2)>=dfh2.snaplen)
				break;
			// 读文件1的词项信息 
			get_ivt_idx_term_info(pf1, &dth1, (void **)&buf1, buf1_size, len1);
			// 读文件2的词项信息
			get_ivt_idx_term_info(pf2, &dth2, (void **)&buf2, buf2_size, len2);
		} 
		 
		// 比较词项的先后顺序 
		ret = strcmp(buf1, buf2);
		// 文件1中的词项靠前 
		if(ret<0) 
		{
			fwrite((void*)&dth1, sizeof(ivt_idx_term_hdr), 1, pfOut);
			fwrite(buf1, len1, 1, pfOut);
			// 更新文件头长度 
			dfhOut.snaplen += dth1.termlen;
		}
		// 文件2中的词项靠前
		else if(ret>0) 
		{
			fwrite((void*)&dth2, sizeof(ivt_idx_term_hdr), 1, pfOut);
			fwrite(buf2, len2, 1, pfOut);
			// 更新文件头长度 
			dfhOut.snaplen += dth2.termlen;
		}
		// 相同的词项
		else
		{
			// 默认不同的倒排索引文件建立在不同的文档集上 
			dthTemp.termlen = sizeof(ivt_idx_term_hdr);
			dthTemp.df = dth1.df + dth2.df; 
			dthTemp.len = dth1.len; 
			dthTemp.status = 0;
			// 词项信息头 
			fwrite((void*)&dthTemp, sizeof(ivt_idx_term_hdr), 1, pfOut);
			// 词项名 
			fwrite(buf1, 1, dth1.len, pfOut);
			dthTemp.termlen += dthTemp.len;
			
			offset1 = offset2 = dthTemp.len;
			
			// 归并文档ID 
			_Int32 *docID1{0}, *docID2{0};
			_Int16 *size{0};
			
			int docRet = 0;
			while(offset1<len1 && offset2<len2) 
			{
				// 上次比较结果
				if(docRet<0) 
				{
					// 更新docID1
					docID1 = (_Int32 *)(buf1 + offset1); 
				}
				else if(docRet>0)
				{
					// 更新docID2
					docID2 = (_Int32 *)(buf2 + offset2); 
				}
				else
				{
					// 更新docID1
					docID1 = (_Int32 *)(buf1 + offset1); 
					// 更新docID2
					docID2 = (_Int32 *)(buf2 + offset2); 
				} 
				 
				// 不存在 docID1==docID2(不同倒排索引文件处理不同的文档集) 
				if(*docID1<*docID2)
					docRet = -1;
				else if(*docID1>*docID2)
					docRet = 1;
				else
					docRet = 0; 
					
				// docID1较小 
				if(docRet<0) 
				{
					// Doc ID 
					fwrite((void*)docID1, 1, sizeof(_Int32), pfOut);
					offset1 += sizeof(_Int32);
					// Size 
					size = (_Int16 *)(buf1 + offset1); 
					fwrite((void*)size, 1, sizeof(_Int16), pfOut);
					offset1 += sizeof(_Int16);
					// Position 
					fwrite((void*)(buf1+offset1), sizeof(_Int16), *size, pfOut);
					offset1 += sizeof(_Int16) * (*size);
				}
				// docID2较小 
				else
				{
					// Doc ID 
					fwrite((void*)docID2, 1, sizeof(_Int32), pfOut);
					offset2 += sizeof(_Int32);
					// Size 
					size = (_Int16 *)(buf2 + offset2); 
					fwrite((void*)size, 1, sizeof(_Int16), pfOut);
					offset2 += sizeof(_Int16);
					// Position 
					fwrite((void*)(buf2+offset2), sizeof(_Int16), *size, pfOut);
					offset2 += sizeof(_Int16) * (*size);
				} 
				
				dthTemp.termlen += sizeof(_Int32) + sizeof(_Int16) + sizeof(_Int16) * (*size);
			}
			
			// 缓冲区1
			if(offset1<len1)
			{
				fwrite((void*)(buf1+offset1), len1-offset1, 1, pfOut);
				dthTemp.termlen += len1-offset1;
			}
			// 缓冲区2
			else if(offset2<len2)
			{
				fwrite((void*)(buf2+offset2), len2-offset2, 1, pfOut);
				dthTemp.termlen += len2-offset2;
			}
			
			fseek(pfOut, -dthTemp.termlen, SEEK_CUR); 
			fwrite((void*)&dthTemp, 1, sizeof(ivt_idx_term_hdr), pfOut);
			fseek(pfOut, dthTemp.termlen-sizeof(ivt_idx_term_hdr), SEEK_CUR);
			
			dfhOut.snaplen += dthTemp.termlen;
		}
	}
	// 最后一次比较结果
	if(ret<0) 
	{
		fwrite((void*)&dth2, sizeof(ivt_idx_term_hdr), 1, pfOut);
		fwrite(buf2, len2, 1, pfOut);
		// 更新文件头长度 
		dfhOut.snaplen += dth2.termlen;
	}
	else if(ret>0)
	{
		fwrite((void*)&dth1, sizeof(ivt_idx_term_hdr), 1, pfOut);
		fwrite(buf1, len1, 1, pfOut);
		// 更新文件头长度 
		dfhOut.snaplen += dth1.termlen;
	}
	
	// 定义缓冲区 
	size_t length = 2048;
	void *temp = malloc(length);
	// 归并文件1剩余数据
	while(ftell(pf1)<dfh1.snaplen)
	{
		auto left_len = dfh1.snaplen - ftell(pf1);
		
		if(left_len>length)
		{
			fread(temp, length, 1, pf1);
			fwrite(temp, length, 1, pfOut);
			// 更新文件头长度 
			dfhOut.snaplen += length;
		}
		else
		{
			fread(temp, left_len, 1, pf1);
			fwrite(temp, left_len, 1, pfOut);
			// 更新文件头长度 
			dfhOut.snaplen += left_len;
		}
	}
	
	// 归并文件2剩余数据
	while(ftell(pf2)<dfh2.snaplen)
	{
		auto left_len = dfh2.snaplen - ftell(pf2);
		
		if(left_len>length)
		{
			fread(temp, length, 1, pf2);
			fwrite(temp, length, 1, pfOut);
			// 更新文件头长度 
			dfhOut.snaplen += length;
		}
		else
		{
			fread(temp, left_len, 1, pf2);
			fwrite(temp, left_len, 1, pfOut);
			// 更新文件头长度 
			dfhOut.snaplen += left_len;
		}
	}
	
	fseek(pfOut, 0, SEEK_SET); 
	fwrite((void*)&dfhOut, 1, sizeof(ivt_idx_file_header), pfOut);
	fclose(pfOut); 
	
	fclose(pf1); 
	fclose(pf2); 
	
	if(temp) 
		free(temp); 
	
	if(buf1) 
		free(buf1);
		
	if(buf2) 
		free(buf2);
		
	return; 
}

std::map<std::string, unsigned long long> *Inverter::CreateDictionary(const char *filename)
{
	FILE *pFile = fopen(filename, "rb");
	if(NULL==pFile)
    {
        printf("打开ii文件失败\n");
        return NULL;
    }
    long iFileLen{0L};
    
    fseek(pFile, 0, SEEK_END);
    iFileLen = ftell(pFile);
    fseek(pFile, 0, SEEK_SET);
    
    if(iFileLen<sizeof(ivt_idx_file_header))
    {
        printf("ii文件格式错误\n");
		fclose(pFile);
        return NULL;
    }
    
    ivt_idx_file_header dfh;
    fread((void*)&dfh, sizeof(ivt_idx_file_header), 1, pFile);
    
    if(iFileLen!=dfh.snaplen)
    {
        printf("ii文件格式错误\n");
		fclose(pFile);
        return NULL;
    }
    
	ivt_idx_term_hdr dth;
	char *buffer{NULL};
	size_t buf_size{0};
	auto dictionary = new std::map<std::string, unsigned long long>();
	while(ftell(pFile)<dfh.snaplen)
	{
		long offset = ftell(pFile);
		fread((void*)&dth, sizeof(ivt_idx_term_hdr), 1, pFile);
		
		auto len = dth.termlen-sizeof(ivt_idx_term_hdr);
		if(buf_size<dth.len) 
		{
			free(buffer);
			buffer = (char *)malloc(dth.len);
			buf_size = dth.len;
		}
		fread((void*)buffer, dth.len, 1, pFile);
		(*dictionary)[buffer] = offset;
		fseek(pFile, len-dth.len, SEEK_CUR);
	}
	
	if(buffer) 
		free(buffer);

	fclose(pFile);	
	return dictionary;
} 

std::map<unsigned long long, std::vector<unsigned int>> Inverter::ExtractTermInfo(FILE *ivtIdxFp, size_t offset)
{
	std::map<unsigned long long, std::vector<unsigned int>> doc_set;
	fseek(ivtIdxFp, offset, SEEK_SET);
	ivt_idx_term_hdr dth;
	fread((void*)&dth, sizeof(ivt_idx_term_hdr), 1, ivtIdxFp);
	
	char *buffer = (char *)malloc(dth.termlen - sizeof(ivt_idx_term_hdr));
	fread((void*)buffer, dth.termlen - sizeof(ivt_idx_term_hdr), 1, ivtIdxFp);
	
	size_t index = dth.len;
	
	if(buffer==NULL)
		return doc_set;
		
	_Int32 *docID;
	_Int16 *size;
	for(int i=0; i<dth.df; ++i)
	{
		docID = (_Int32 *)(buffer + index);
		index += sizeof(_Int32);
		size = (_Int16 *)(buffer + index);
		index += sizeof(_Int16);
		_Int16 *pos; 
		for(int j=0; j<*size; ++j) 
		{
			pos = (_Int16 *)(buffer + index);
			doc_set[*docID].push_back(*pos);
			index += sizeof(_Int16);
		}
	}
		
	if(buffer=NULL)
		free(buffer);
	
	return doc_set;
}

void Inverter::clear()
{
	pimpl->iindex.clear(); 
}
/*
void Inverter::print(const char *filename)
{
	if(NULL==filename) 
	{
		for(auto it=pimpl->iindex.begin(); it!=pimpl->iindex.end(); ++it)
		{
			write_file(it->first+"(");
			write_file(it->second.size());
			write_file("):\n");
			auto &docs = pimpl->iindex[it->first];
			for(auto iit=docs.begin(); iit!=docs.end(); ++iit)
			{
				auto doc_num = iit->first; 
				write_file("\t[");
				write_file(doc_num);
				write_file(":");
				
				for(auto iiit=iit->second.begin(); iiit!=iit->second.end(); ++iiit)
				{
					write_file(*iiit);
					write_file(",");
				}
				
				write_file("]\n");
			}
		}
	} 
	else
	{
		FILE *pFile = fopen(filename, "rb");
		if(NULL==pFile)
	    {
	        printf("打开ii文件失败\n");
	        return;
	    }
	    long iFileLen{0L};
	    
	    fseek(pFile, 0, SEEK_END);
	    iFileLen = ftell(pFile);
	    fseek(pFile, 0, SEEK_SET);
	    
	    if(iFileLen<sizeof(ivt_idx_file_header))
	    {
	        printf("ii文件格式错误\n");
	        return;
	    }
	    
	    ivt_idx_file_header dfh;
	    fread((void*)&dfh, sizeof(ivt_idx_file_header), 1, pFile);
	    
	    if(iFileLen!=dfh.snaplen)
	    { 
	        printf("ii文件格式错误\n");
	        return;
	    }
	    
		ivt_idx_term_hdr dth;
		char *buffer{NULL};
		size_t buf_size{0};
		size_t len{0};
		while(ftell(pFile)<dfh.snaplen)
		{
			get_ivt_idx_term_info(pFile, &dth, (void **)&buffer, buf_size, len);
			
			write_file(std::string(buffer)+"(");
			write_file(dth.df);
			write_file("):\n");
			int offset = dth.len;
			for(int i=0; i<dth.df; ++i)
			{
				_Int32 *docID = (_Int32 *)(buffer + offset);
				offset += sizeof(_Int32);
				write_file("\t[");
				write_file(*docID);
				write_file(":");
				_Int16 size = *(_Int16 *)(buffer + offset);
				offset += sizeof(_Int16);
				for(int j=0; j<size; ++j)
				{
					_Int16 pos = *(_Int16 *)(buffer + offset);
					offset += sizeof(_Int16);
					write_file(pos);
					write_file(",");
				}
				write_file("]\n");
			}
			
		}
		if(buffer) 
			free(buffer);
	} 
}
*/
