// memman.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "memmapfile.h"
#include "cdi_str.h"

namespace CDICommon
{
using namespace std;

class MemMapFile
{
public:
	MemMapFile();
    ~MemMapFile();
	
	typedef struct _MMF_DATA
	{
		cdi_str str_name;
		HANDLE hFile;
		HANDLE hMap;
		void *ptr;
	} MMF_DATA;

	static	void* Alloc(size_t in_count);
	static	void  Free(void* ptr);

protected:
	static std::map<void*, MMF_DATA> s_items;
	static ULONG s_mmf_idx;
};

map<void*, MemMapFile::MMF_DATA> MemMapFile::s_items;
ULONG MemMapFile::s_mmf_idx = 1;
MemMapFile g_the_mmf_obj_shtob_bylo;

MemMapFile::MemMapFile()
{
};

MemMapFile::~MemMapFile()
{	
	map<void*, MMF_DATA>::iterator i;
	MMF_DATA mmf;
	for(i = MemMapFile::s_items.begin(); i!=MemMapFile::s_items.end();i++)
	{
		mmf = (*i).second;
		UnmapViewOfFile(mmf.ptr);
		CloseHandle(mmf.hMap);
        CloseHandle(mmf.hFile);
        BOOL ret = DeleteFile(QString(mmf.str_name.c_str()).toStdWString().c_str());
		DWORD err = GetLastError();
	}
}

void* MemMapFile::Alloc(size_t in_count)
{
	MMF_DATA mmf;
    //char szBuff[MAX_PATH+1]="";
	DWORD dwSize=GetTempPath(0,0);
    wchar_t *pPath = new wchar_t[dwSize+MAX_PATH+1];

	if(dwSize-1!=GetTempPath(dwSize,pPath))
	{
		// error
		delete [] pPath;
		return NULL;
	};

    GetTempFileName(pPath, L"~mf", s_mmf_idx++, pPath);

    QString qPath = QString::fromWCharArray(pPath);
    mmf.str_name = qPath.toLocal8Bit().data();
    delete [] pPath;

    mmf.hFile = CreateFile(qPath.toStdWString().c_str(), GENERIC_READ | GENERIC_WRITE, 0,
		0, CREATE_ALWAYS, 0, NULL);
	if(INVALID_HANDLE_VALUE == mmf.hFile)
        return NULL; // error

	mmf.hMap = CreateFileMapping(mmf.hFile , NULL, PAGE_READWRITE,
		0, (DWORD)in_count, NULL);            

	if (mmf.hMap == NULL) 
	{ 
		// error
		CloseHandle(mmf.hFile);
		return NULL;
	}

	void* pAddress = MapViewOfFile(mmf.hMap, FILE_MAP_ALL_ACCESS, 0, 0, in_count);
	DWORD err=GetLastError();
	if(pAddress)
	{
		// success!!! -> add to container
		mmf.ptr = pAddress;
		MemMapFile::s_items[pAddress] = mmf;
	}
	else
	{
		CloseHandle(mmf.hMap);
		CloseHandle(mmf.hFile);
        DeleteFile(qPath.toStdWString().c_str());
	}
	return pAddress;
};

void  MemMapFile::Free(void* ptr)
{
	if(MemMapFile::s_items.find(ptr) == MemMapFile::s_items.end())
	{
		// error - invalid addres!!!
//		ASSERT(0);
		return;
	}

	// free and retuarn
	MMF_DATA mmf = MemMapFile::s_items[ptr];
	UnmapViewOfFile(mmf.ptr);
	CloseHandle(mmf.hMap);
	CloseHandle(mmf.hFile);
    QString qStr(mmf.str_name.c_str());
    BOOL ret = DeleteFile(qStr.toStdWString().c_str());
	DWORD err = GetLastError();

	MemMapFile::s_items.erase(ptr);
};

void* mmf_alloc(size_t in_count)
{
	return MemMapFile::Alloc(in_count);
};
void mmf_free(void *ptr)
{
	MemMapFile::Free(ptr);
};
};
