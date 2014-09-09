#pragma once
#include "crc32.h"

#define MAX_KEYSTR_LEN 100
#define CDI_WS_HEADER_SIZE 0x40
#define CDI_WS_FNUM_POS 0x10
#define _USE_CRC32 
#include <vector>

#define CDI_WS_PACK_UNPACKED 0
#define CDI_WS_PACK_BZIP     1

namespace CDICommon
{
// cdi file header
#pragma pack(push,1)
struct FileHeader
{
	char  szSig[0xC];
    unsigned long ws_ver;
    unsigned long fields_num;
	char  app_ver[0x3];
    unsigned short  app_build;
	char  pack_method;
    unsigned long unpacked_size;
    unsigned long pack_size[4];
	char  spare[0x12];
	
};
#pragma pack(pop)

// cdi project file header size

class CDIWorkspace
{
public:
	CDIWorkspace(void);
	virtual ~CDIWorkspace(void);
	virtual int OpenFileWr(const char* inName);
	virtual int OpenFileRd(const char* inName, bool bProgr=true);
	virtual int CloseFile();
    virtual int AddData(char*, void*, unsigned long, bool copy_data = false);
    virtual int AddDataEnc(char*, void*, unsigned long, bool copy_data = false);
    virtual int GetData(char*, void*, unsigned long, bool vital=true);
    virtual int GetDataDec(char*, void*, unsigned long, bool vital=true);
	virtual int AddCdiStr(char*, cdi_str*);
	virtual int GetCdiStr(char*, cdi_str*, bool vital=true);
    virtual int GetDataIdx(char*, std::vector<int>*, void*, unsigned long, bool vital=true);
    virtual int AddDataIdx(char*, std::vector<int>*, void*, unsigned long, bool copy_data = false);
    virtual int GetDataIdxDec(char*, std::vector<int>*, void*, unsigned long,  bool vital=true);
    virtual int AddDataIdxEnc(char*, std::vector<int>*, void*, unsigned long, bool copy_data = false);
	virtual int AddCdiStrIdx(char*, std::vector<int>*, cdi_str*);
	virtual int GetCdiStrIdx(char*, std::vector<int>*, cdi_str*, bool vital=true);
    void SetVersion(char major,char mid,char minor,unsigned short build)
	{ m_hdr.app_ver[0]=major;  m_hdr.app_ver[1]=mid;  m_hdr.app_ver[2]=minor;  m_hdr.app_build=build; };

	template <class _A>
	int AddDataVector(char* inKey, std::vector<_A>& inVect, bool copy_data = false)
	{
		int idx=0;
		int ret=0;
        unsigned long size = inVect.size();
		cdi_str str_key = inKey;
        AddData((char*)str_key.c_str(), &size, sizeof(unsigned long),  copy_data);
        for(typename std::vector<_A>::iterator i=inVect.begin(); i!=inVect.end(); i++)
		{
			str_key+=long(idx++);
			_A Data = (*i);
			ret += AddData((char*)str_key.c_str(), &Data, sizeof(_A),  copy_data);
		}
		return ret;
	}
	template <class _A>
	int GetDataVector(char* inKey, std::vector<_A>& outVect)
	{
		int idx=0;
		int size=0;
		int ret=0;
		cdi_str str_key = inKey;
        GetData((char*)str_key.c_str(), &size, sizeof(unsigned long));
		for(int i=0; i<size; i++)
		{
			str_key+=long(i);
			_A Data;
			ret += GetData((char*)str_key.c_str(), &Data, sizeof(_A));
			outVect.push_back(Data);
		}
		return ret;
	};

	template <class _A>
	int AddDataVectorIdx(char* inKey, std::vector<int>* idxs, std::vector<_A>& inVect, bool copy_data = false)
	{
		char szBuff[256]="";
		char szIdxStr[100]="";

		// build index string
		for(vector<int>::iterator i = idxs->begin(); i != idxs->end(); i++)
		{
			char sz[100]="";
			sprintf(sz, "%i", (*i));
			strcat(szIdxStr, "$");
			strcat(szIdxStr, sz);
		}

		// build key string
		sprintf(szBuff, "%s%s", inKey, szIdxStr);

		return AddDataVector<_A>(szBuff, inVect, copy_data);
	}
	template <class _A>
	int GetDataVectorIdx(char* inKey, std::vector<int>* idxs, std::vector<_A>& outVect)
	{
		char szBuff[256]="";
		char szIdxStr[100]="";

		// build index string
		for(vector<int>::iterator i = idxs->begin(); i != idxs->end(); i++)
		{
			char sz[100]="";
			sprintf(sz, "%i", (*i));
			strcat(szIdxStr, "$");
			strcat(szIdxStr, sz);
		}

		// build key string
		sprintf(szBuff, "%s%s", inKey, szIdxStr);

		return GetDataVector<_A>(szBuff, outVect);
	};
	
	bool IsExist(char*, std::vector<int>* p =NULL);
protected:
	virtual int WriteHeader(FILE* inFile);
	virtual int ReadHeader(FILE* inFile);
	int m_Count;
	char m_PackMethod;
	
#ifdef _USE_CRC32 
	// saving data
	std::vector<unsigned long>  m_save_keys;
	std::vector<unsigned char*> m_save_buff;
	std::vector<long> m_data_save_len;

	// loading data
    std::vector<void *>  m_ld_ptrs;
	
    std::map<unsigned long, unsigned long>  m_ld_keys;
    std::map<cdi_str, unsigned long> m_ld_str_keys;
    std::map<unsigned long, unsigned long> m_coll_idx;
    std::vector<unsigned long>  m_ld_size;
    std::vector<unsigned long>  m_ld_crcs;

    std::vector<unsigned long>  m_idx_got;
	
	virtual int WriteCRC32(FILE*,unsigned long);
	virtual int ReadCRC32(FILE*,unsigned long*);
#else // use strings
	// saving data	
    std::vector<cdi_str>  m_save_keys;
	std::vector<unsigned char*> m_save_buff;
	std::vector<long> m_data_save_len;

	// loading data
    std::vector<void *>   m_ld_ptrs;
    std::map<cdi_str, unsigned long> m_ld_keys;
    std::vector<unsigned long>   m_ld_size;

	virtual int WriteString(FILE*,const char*);
	virtual int ReadString(FILE*,char*);
#endif
	//std::vector<unsigned char*> m_data_copy;
	FILE *m_file;
    unsigned char *m_pBuff;
	//fpos_t m_num_pos;
    unsigned long m_fields_num;
	bool m_write_pos;
	FileHeader m_hdr;
	size_t m_appendix_sz;
    unsigned char *m_pAppx;
	cdi_str m_FileName;
	bool m_bOpenWrt;
public:
	bool CreateAppendix();
    bool GetAppendix(void *pData, unsigned long *size);
	size_t GetAppendixSize(){ return m_appendix_sz; };

	void AddKey(bool in){ m_bAddKey=in; };
	bool SetPacking(char packing);
	char GetPacking(){return m_hdr.pack_method;};
protected:
	bool m_bAddKey;
	bool PackData();
    unsigned char* UnpackData(unsigned char*, int);
		
};

} //end of namespace CDICommon
