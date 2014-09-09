#include "stdafx.h"
#include "cdi_str.h"
#include "debug.h"
#include "cdiworkspace.h"
#include "ext_resources.h"
#include "cdierrorexception.h"
#include "ProgressDialog.h"
#include "bzlib.h"
//#include "../ctassist/CTAssist.h"
#define _CDI_DEBUG
using namespace std;

//extern CWinAppEx g_theApp;

namespace CDICommon
{

CDIWorkspace::CDIWorkspace(void):
m_file(NULL),
m_Count(0),
m_pBuff(NULL),
m_write_pos(false),
m_fields_num(0),
m_appendix_sz(0),
m_pAppx(NULL),
m_bAddKey(false),
m_PackMethod(CDI_WS_PACK_UNPACKED),
m_bOpenWrt(false)
{
	memset(&m_hdr, 0, sizeof(FileHeader));
	m_hdr.ws_ver = 0x01000000;
}

CDIWorkspace::~CDIWorkspace(void)
{
	// delete appendix
	if(m_pAppx)
		delete [] m_pAppx;
	// close if opened
	if(m_file)
		CloseFile();
	// delete buffer
	if(m_pBuff)
		delete [] m_pBuff ;
	// delete data loaded
#ifdef _USE_CRC32 
	map<unsigned long, unsigned char*>::iterator i;
#else
	map<cdi_str, unsigned char*>::iterator i;
#endif //

}

int CDIWorkspace::AddData(char* inStr, void* ptr, DWORD size, bool copy_data)
{
	if(m_file)
	{
		m_fields_num++;
		if(size>0xFF000000)
			size = size;
		ULONG crc_val = key17_str(inStr, m_bAddKey);
		fwrite(&crc_val, sizeof(ULONG), 1 , m_file);
		fwrite(&size, sizeof(DWORD),1,m_file);
		fwrite(ptr, size, 1, m_file);
		return 1;
	}
	return 0;
};
int CDIWorkspace::AddDataEnc(char* inStr, void* ptr, DWORD size, bool copy_data)
{
	//int size_n = size/8;
	//g_theApp.m_Lic.Encrypt3DES(ptr, size_n*8);
    return AddData(inStr, ptr, size, copy_data);
};
int CDIWorkspace::GetData(char* inStr, void* ptr, DWORD size, bool vital)
{
#ifndef _USE_CRC32 
#pragma error(Nefiga ne implemented!!!)
#endif //
	//CdiDebugPrint("CDIWorkspace::GetData: key=%s, ptr=08x%, size=%i\n", inStr, (ULONG)(ptr), size);
	DWORD crc_key = key17_str(inStr, m_bAddKey);
	//if(crc_key == 0x22944f1a)
	//	crc_key = 0x22944f1a;
	std::map<DWORD, DWORD>::iterator ik;
	ik = m_ld_keys.find(crc_key);//=find(m_ld_keys.begin(), m_ld_keys.end(), crc_key);

	if(0xc733fbe9 == crc_key)
		crc_key = crc_key;
	if(ik==m_ld_keys.end())
	{
		// matched data nefiga ne found - nezachot!!!
		if(ptr && size && vital)
			throw CDIErrorException(LoadCdiStr(IDS_ERROR_READ_WS_ERROR));
		return 0;
	};
	int idx = (*ik).second;

	// check for collision
	char szTmpKey[0x100]="";
	if(idx==-1)
	{
		// start collision counter
		if(m_coll_idx.find(crc_key) == m_coll_idx.end())
			m_coll_idx[crc_key]=1;
		
		// build collision key string
		DWORD col_idx = m_coll_idx[crc_key];
		sprintf(szTmpKey, "%08x_%i", crc_key, col_idx);

		// check key string
		if(m_ld_str_keys.find(cdi_str(szTmpKey)) == m_ld_str_keys.end())
		{
			// reset collision counter
			m_coll_idx[crc_key]=1;
			col_idx = m_coll_idx[crc_key];
			sprintf(szTmpKey, "%08x_%i", crc_key, col_idx);
		}
		idx = m_ld_str_keys[cdi_str(szTmpKey)];
		
		// increase collision counter
		if(ptr)
			m_coll_idx[crc_key]+=1;
	}
	
	// check the size
	DWORD ld_sz = *(m_ld_size.begin()+idx);
	if(ptr==NULL)
	{
		// need only size
		return ld_sz;
	}
	if(size==-1)
	{
		// kakaya-to fignya with buffer size - nezachot!!!
        throw CDIErrorException(LoadCdiStr(IDS_ERROR_READ_WS_ERROR));	
	}
	if(ld_sz>size)
	{
		// kakaya-to fignya with buffer size - nezachot!!!
        throw CDIErrorException(LoadCdiStr(IDS_ERROR_READ_WS_ERROR));
	};

	// copy the buffer
	void* ld_ptr = *(m_ld_ptrs.begin()+idx);
	m_appendix_sz-=ld_sz+2*sizeof(DWORD);
	m_idx_got.push_back(idx);
	memcpy(ptr, ld_ptr, ld_sz);

	return 1;

};

bool CDIWorkspace::GetAppendix(void *pData, ULONG *size)
{
	return true;
};


int CDIWorkspace::GetDataDec(char* inStr, void* ptr, DWORD size, bool vital)
{
	//int size_n = size/8;
	int ret = GetData(inStr, ptr, size, vital);
	//g_theApp.m_Lic.Decrypt3DES(ptr, size_n*8);
	return ret;
};

int CDIWorkspace::GetDataIdx(char* inStr, vector<int> *pIdxVec, void* ptr, DWORD size, bool vital)
{
	char szBuff[256]="";
	char szIdxStr[100]="";

	// build index string
	for(vector<int>::iterator i = pIdxVec->begin(); i != pIdxVec->end(); i++)
	{
		char sz[100]="";
        sprintf(sz, "%i", (*i));
		strcat(szIdxStr, "$");
		strcat(szIdxStr, sz);
	}

	// build key string
	sprintf(szBuff, "%s%s", inStr, szIdxStr);

	return GetData(szBuff, ptr, size, vital);
}
int CDIWorkspace::GetDataIdxDec(char* inStr, vector<int> *pIdxVec, void* ptr, DWORD size, bool vital)
{
	char szBuff[256]="";
	char szIdxStr[100]="";

	// build index string
	for(vector<int>::iterator i = pIdxVec->begin(); i != pIdxVec->end(); i++)
	{
		char sz[100]="";
        sprintf(sz, "%i", (*i));
		strcat(szIdxStr, "$");
		strcat(szIdxStr, sz);
	}

	// build key string
	sprintf(szBuff, "%s%s", inStr, szIdxStr);

	return GetDataDec(szBuff, ptr, size, vital);
};

int CDIWorkspace::AddDataIdx(char* inStr, vector<int>* pIdxVec, void* ptr, DWORD size, bool copy_data)
{
	char szBuff[256]="";
	char szIdxStr[100]="";

	// build index string
	for(vector<int>::iterator i = pIdxVec->begin(); i != pIdxVec->end(); i++)
	{
		char sz[100]="";
        sprintf(sz, "%i", (*i));
		strcat(szIdxStr, "$");
		strcat(szIdxStr, sz);
	}

	// build key string
	sprintf(szBuff, "%s%s", inStr, szIdxStr);

	return AddData(szBuff, ptr, size, copy_data);

}
int CDIWorkspace::AddDataIdxEnc(char* inStr, vector<int>* pIdxVec, void* ptr, DWORD size, bool copy_data)
{
	char szBuff[256]="";
	char szIdxStr[100]="";

	// build index string
	for(vector<int>::iterator i = pIdxVec->begin(); i != pIdxVec->end(); i++)
	{
		char sz[100]="";
        sprintf(sz, "%i", (*i));
		strcat(szIdxStr, "$");
		strcat(szIdxStr, sz);
	}

	// build key string
	sprintf(szBuff, "%s%s", inStr, szIdxStr);

	return AddDataEnc(szBuff, ptr, size, copy_data);
};
	
int CDIWorkspace::WriteHeader(FILE* inFile)
{
	// write dummy header
	unsigned char arr[CDI_WS_HEADER_SIZE];
	memset(arr,0,sizeof(arr));
	//fwrite(arr, CDI_WS_HEADER_SIZE, 1, inFile);
	fwrite(&m_hdr, CDI_WS_HEADER_SIZE, 1, inFile);

	if(ferror(inFile))
		throw CDIErrorException(LoadCdiStr(IDS_ERROR_WRITE_WS_ERROR));

	return 1;
};
int CDIWorkspace::ReadHeader(FILE* inFile)
{
	// skip header
	//fseek(inFile, CDI_WS_HEADER_SIZE, SEEK_CUR);
	fread(&m_hdr, CDI_WS_HEADER_SIZE, 1, inFile);
	return 1;
};

#ifdef _USE_CRC32
int CDIWorkspace::WriteCRC32(FILE* inFile, unsigned long val)
{
	// write crc32
	fwrite(&val, sizeof(unsigned long), 1, inFile);
	if(ferror(inFile))
		throw CDIErrorException(LoadCdiStr(IDS_ERROR_WRITE_WS_ERROR));
	
	return 1;
};
int CDIWorkspace::ReadCRC32(FILE* inFile, unsigned long* crc32)
{
	// read data
	fread(crc32, sizeof (unsigned long), 1, inFile);
	if( ferror(inFile))
		throw CDIErrorException(LoadCdiStr(IDS_ERROR_READ_WS_ERROR));
    return 1;
};
#else 
int CDIWorkspace::WriteString(FILE* inFile, const char* inStr)
{
	// write string length
	int len = strlen(inStr);
	if(len > MAX_KEYSTR_LEN)
		throw CDIErrorException(LoadCdiStr(IDS_ERROR_WRITE_WS_ERROR));
	char ch_len = 0xFF&len;
	// write string length
	fwrite(&ch_len, sizeof(ch_len), 1, inFile);
	// write data
	fwrite(inStr, len, 1, inFile);
	if(ferror(inFile))
		throw CDIErrorException(LoadCdiStr(IDS_ERROR_WRITE_WS_ERROR));
	
	return 1;
};
int CDIWorkspace::ReadString(FILE* inFile, char* inStr)
{
	// read string length
	char ch_len = 0;
	int ret = fread(&ch_len, sizeof(ch_len), 1, inFile);
	if(ch_len > MAX_KEYSTR_LEN || ferror(inFile) )
		throw CDIErrorException(LoadCdiStr(IDS_ERROR_READ_WS_ERROR));
	
	// read data
	ret = fread(inStr, ch_len, 1, inFile);
	if( ferror(inFile))
		throw CDIErrorException(LoadCdiStr(IDS_ERROR_READ_WS_ERROR));
	
	// set last zero
	inStr[ch_len] = 0;
    return 1;
};
#endif

int CDIWorkspace::AddCdiStr(char* inKeyStr, cdi_str* inString)
{
	DWORD size = (DWORD)inString->length()+1;
	AddData(inKeyStr, (void*)inString->c_str(), size);
	return 1;
};
int CDIWorkspace::GetCdiStr(char* inKeyStr, cdi_str* outString, bool vital)
{
	int size = GetData(inKeyStr,NULL, 0, vital);
	char *buff = new char[size+1];
	*buff = 0;
	GetData(inKeyStr,buff, size+1, vital);
	*outString = buff;
	delete [] buff;
	return 1;
};

int CDIWorkspace::AddCdiStrIdx(char* inStr, std::vector<int>* pIdxVec, cdi_str* sData)
{
	char szBuff[256]="";
	char szIdxStr[100]="";

	// build index string
	for(vector<int>::iterator i = pIdxVec->begin(); i != pIdxVec->end(); i++)
	{
		char sz[100]="";
        sprintf(sz, "%i", (*i));
		strcat(szIdxStr, "$");
		strcat(szIdxStr, sz);
	}

	// build key string
	sprintf(szBuff, "%s%s", inStr, szIdxStr);

	return AddCdiStr(szBuff, sData);
};
int CDIWorkspace::GetCdiStrIdx(char* inStr, std::vector<int>* pIdxVec, cdi_str* sData, bool vital)
{
	char szBuff[256]="";
	char szIdxStr[100]="";

	// build index string
	for(vector<int>::iterator i = pIdxVec->begin(); i != pIdxVec->end(); i++)
	{
		char sz[100]="";
        sprintf(sz, "%i", (*i));
		strcat(szIdxStr, "$");
		strcat(szIdxStr, sz);
	}

	// build key string
	sprintf(szBuff, "%s%s", inStr, szIdxStr);

	return GetCdiStr(szBuff, sData, vital);
};
int CDIWorkspace::OpenFileWr(const char* inName)
{
	m_file = fopen(inName, "wb");
	if(m_file==NULL) // shit happenes
	{
		//Error!
		throw CDIErrorException(LoadCdiStr(IDS_ERROR_FILE_NOT_FOUND)+ " (Write)\n"+ cdi_str(inName));
	};

	// write the header
	WriteHeader(m_file);

	m_FileName = inName;

	m_bOpenWrt = true;

	return 1;
};

int CDIWorkspace::OpenFileRd(const char* inName, bool bProgr)
{
	if(inName)
		CdiDbgPrint(("CDIWorkspace::OpenFileRd->ENTER, File: %s\n", inName));
	else
		CdiDbgPrint(("CDIWorkspace::OpenFileRd->ENTER, File: NULL\n"));

		
	if(0<strlen(inName))
		m_file = fopen(inName, "rb");
	if(m_file==NULL) // shit happenes
	{
		//Error!
		CdiDbgPrint(("CDIWorkspace::OpenFileRd: ERROR!!! %s not found\n", inName));
		throw CDIErrorException(LoadCdiStr(IDS_ERROR_FILE_NOT_FOUND)+ " (Read)\n"+ cdi_str(inName));
		return 0;
	};
	fpos_t pos;
	if(0!=fseek(m_file, 0, SEEK_END)|| 0!= fgetpos(m_file, &pos))
	{
		//Error!
		fclose(m_file);
		m_file = NULL;
		throw CDIErrorException(LoadCdiStr(IDS_ERROR_READ_WS_ERROR));
		return 0;
		
	}
	fseek(m_file, 0, SEEK_SET);
	// allocate buffer for data
	m_pBuff = new BYTE [(size_t)pos];

	// read the header
	ReadHeader(m_file);

	// progress bar data
	int percent=CProgressDialog::GetPercent(), j=0;
	int perc_max = 100;
	int size = m_Count;
	int perc_step = size/(perc_max-10);

	if(bProgr)
		CProgressDialog::SetPercent(5);

	// read the rest data
	size_t fsz = (size_t)pos-sizeof(FileHeader);
	if(fsz != fread(m_pBuff, 1, fsz, m_file))
	{
		fclose(m_file);
		m_file = NULL;
		throw CDIErrorException(LoadCdiStr(IDS_ERROR_READ_WS_ERROR));
	}

	if(bProgr)
		CProgressDialog::SetPercent(10);

	// unpack data, if necessary
	m_PackMethod =  m_hdr.pack_method;
	if(m_PackMethod>0)
	{
		m_pBuff = UnpackData(m_pBuff, fsz);
		if(m_pBuff)
			fsz = m_hdr.unpacked_size;
		else
			throw CDIErrorException(LoadCdiStr(IDS_ERROR_READ_WS_ERROR));

	}
	
	// fill indexing data
	BYTE *pCurrPos = m_pBuff;
	DWORD *crc_key;
	DWORD *ld_size;
	BYTE  *ldata;
	DWORD idx=0;
	map<DWORD, DWORD>  coll_keys;
	char szTmpKey[0x100]="";
	while (pCurrPos< m_pBuff+fsz)
	{
		crc_key = (DWORD *)pCurrPos;
		pCurrPos+=sizeof(DWORD);
		ld_size = (DWORD *)pCurrPos;
		pCurrPos+=sizeof(DWORD);
		ldata = pCurrPos;
		pCurrPos+=(*ld_size);	
		// set loading data
		m_ld_ptrs.push_back(ldata);
		m_ld_size.push_back(*ld_size);
		m_ld_crcs.push_back(*crc_key);
		m_appendix_sz+=(*ld_size) + 2*sizeof(DWORD);
		if(0x6ad09f33 == *crc_key)//0xc733fbe9
			crc_key = crc_key;
		if(m_ld_keys.find(*crc_key)==m_ld_keys.end())
		{
			m_ld_keys[*crc_key] = idx;
		}
		else
		{
            qDebug("Collision detected in Workspace\n");
			DWORD idx_tmp = m_ld_keys[*crc_key];
			if(idx_tmp != -1)
			{
				coll_keys[*crc_key] = 1;
				m_ld_keys[*crc_key] = -1;

				sprintf(szTmpKey, "%08x_%i", *crc_key, coll_keys[*crc_key]);
				m_ld_str_keys[cdi_str(szTmpKey)] = idx_tmp;

				coll_keys[*crc_key] += 1;
			}
			else
				coll_keys[*crc_key] += 1;

			sprintf(szTmpKey, "%08x_%i", *crc_key, coll_keys[*crc_key]);
						
			m_ld_str_keys[cdi_str(szTmpKey)] = idx;
		}

		idx ++;

		if(j++==perc_step && size!=0)
		{
			if(bProgr)
				CProgressDialog::SetPercent(percent++);
			j=0;
		}

		if(size==0)
		{
			if(j==200 && bProgr)
				CProgressDialog::SetPercent(20);
			if(j==350 && bProgr)
				CProgressDialog::SetPercent(40);
			if(j==500 && bProgr)
				CProgressDialog::SetPercent(60);
			if(j==650 && bProgr)
				CProgressDialog::SetPercent(80);
		}
	}
	if(size==0 && bProgr)
		CProgressDialog::SetPercent(100);

	m_FileName = inName;
	m_bOpenWrt = false;

	CdiDbgPrint(("CDIWorkspace::OpenFileRd->LEAVE\n"));
	
	return 1;
};
int CDIWorkspace::CloseFile()
{
	if(m_file)
	{
		if(m_write_pos)
		{
			fseek(m_file, CDI_WS_FNUM_POS, SEEK_SET);
			fwrite(&m_fields_num, sizeof(DWORD), 1, m_file);
			fseek(m_file, 0, SEEK_END);
		}
		fclose(m_file);
		m_file = NULL;

		// pack file if necessary
		if(m_bOpenWrt && m_PackMethod>0)
		{
			PackData();
		}
	}
	return 1;
};

bool CDIWorkspace::IsExist(char* strKey, std::vector<int>* ids)
{
	char szBuff[256]="";
	char szIdxStr[100]="";

	char* str = strKey;
	if(ids)
	{
		// build index string
		for(vector<int>::iterator i = ids->begin(); i != ids->end(); i++)
		{
			char sz[100]="";
			sprintf(sz, "%i", (*i));
			strcat(szIdxStr, "$");
			strcat(szIdxStr, sz);
		}

		// build key string
		sprintf(szBuff, "%s%s", strKey, szIdxStr);
		str = szBuff;
	}

	DWORD crc_key = key17_str(str, m_bAddKey);
	std::map<DWORD, DWORD>::iterator ik;
	ik = m_ld_keys.find(crc_key);
	if(ik==m_ld_keys.end())
	{
		return false;
	};

	return true;

};

bool CDIWorkspace::CreateAppendix()
{
	unsigned long idx_max = m_ld_ptrs.size();

	if(m_pAppx)
		delete [] m_pAppx;
	m_pAppx = new BYTE[m_appendix_sz];
	size_t curr_size=0;
	for(unsigned long i=0;i<idx_max;i++)
	{
		vector<DWORD>::iterator ii = find(m_idx_got.begin(), m_idx_got.end(), i );
		if(m_idx_got.end() == ii)
		{
			void *ldata = m_ld_ptrs[i];
			DWORD ld_size = m_ld_size[i];
			DWORD crc_key = m_ld_crcs[i];
			size_t new_size = curr_size+2*sizeof(DWORD)+ld_size;
			if(new_size<m_appendix_sz)
			{
				memcpy(m_pAppx, &crc_key, sizeof(DWORD));
				m_pAppx+=sizeof(DWORD);
				memcpy(m_pAppx, &ld_size, sizeof(DWORD));
				m_pAppx+=sizeof(DWORD);
				memcpy(m_pAppx, ldata, ld_size);
				m_pAppx+=ld_size;
			}
			else
			{
//				ASSERT(0);// it should never be
				return false;
			}

			curr_size=new_size;
		}
	}

	return true;
};
bool CDIWorkspace::SetPacking(char packing)
{
	//if(packing>1 || packing<0)
	//	return false;
	m_PackMethod = packing;
	return true;
};
bool CDIWorkspace::PackData()
{
	FILE *f = fopen(m_FileName.c_str(), "rb");

	if(f==NULL) // shit happenes
	{
		//Error!
		CdiDbgPrint(("CDIWorkspace::PackData: ERROR!!! %s not found\n", m_FileName.c_str()));
		return false;
	};
	fpos_t org_len=0;
	if(0!=fseek(f, 0, SEEK_END)|| 0!= fgetpos(f, &org_len))
	{
		//Error!
		fclose(f);
		CdiDbgPrint(("CDIWorkspace::PackData: ERROR!!! fseek failed\n"));
		return false;
		
	}
	fseek(f, 0, SEEK_SET);
	
	fpos_t body_len = org_len-sizeof(FileHeader);

	// allocate buffer for data
	char *pBuff = new char [(size_t)org_len];
	char *pBuffComp = new char [(size_t)org_len];
	FileHeader *pHdr = (FileHeader *)pBuff;

	// read all the file and close it
	fread(pBuff, 1, (size_t)org_len, f);
	fclose(f);
	f=NULL;

	// get pointer to data
	char *pBuffData =  pBuff+sizeof(FileHeader);

	// pack the data
	unsigned int comp_len=(unsigned int)org_len;
	unsigned int uncomp_len=(unsigned int)org_len-sizeof(FileHeader);
	INIT_TIME_MESURE
	TIME_MESURE_START

	unsigned int unpack_sec_size[4];
	unpack_sec_size[0]=unpack_sec_size[1]=unpack_sec_size[2] = uncomp_len/4;
	unpack_sec_size[3]=uncomp_len-3*(uncomp_len/4);
	unsigned int pack_sec_size[4];
	pack_sec_size[0]=pack_sec_size[1]=pack_sec_size[2] = uncomp_len/4;
	pack_sec_size[3]=uncomp_len-3*(uncomp_len/4);

	char *pPackSec[4];
	pPackSec[0]=pBuffComp;
	pPackSec[1]=pPackSec[0]+unpack_sec_size[0];
	pPackSec[2]=pPackSec[1]+unpack_sec_size[1];
	pPackSec[3]=pPackSec[2]+unpack_sec_size[2];
	char *pUnpackSec[4];
	pUnpackSec[0]=pBuffData;
	pUnpackSec[1]=pUnpackSec[0]+unpack_sec_size[0];
	pUnpackSec[2]=pUnpackSec[1]+unpack_sec_size[1];
	pUnpackSec[3]=pUnpackSec[2]+unpack_sec_size[2];
	int comp_ret[4]={0,0,0,0};
	#pragma omp parallel
	{
		#pragma omp sections
		{
			#pragma omp section
			comp_ret[0] = BZ2_bzBuffToBuffCompress(pPackSec[0], &pack_sec_size[0], pUnpackSec[0], unpack_sec_size[0], 1, 0, 30);
			#pragma omp section
			comp_ret[1] = BZ2_bzBuffToBuffCompress(pPackSec[1], &pack_sec_size[1], pUnpackSec[1], unpack_sec_size[1], 1, 0, 30);
			#pragma omp section
			comp_ret[2] = BZ2_bzBuffToBuffCompress(pPackSec[2], &pack_sec_size[2], pUnpackSec[2], unpack_sec_size[2], 1, 0, 30);
			#pragma omp section
			comp_ret[3] = BZ2_bzBuffToBuffCompress(pPackSec[3], &pack_sec_size[3], pUnpackSec[3], unpack_sec_size[3], 1, 0, 30);


		}
	}
    TIME_MESURE_FINISH(("Compress time: "));
    TIME_MESURE_START
	if(comp_ret[0]||comp_ret[1]||comp_ret[2]||comp_ret[3])
	{
		CdiDbgPrint(("CDIWorkspace::PackData: ERROR!!! Compress failed ret=%i,%i,%i,%i\n", comp_ret[0],comp_ret[1],comp_ret[2],comp_ret[3]));
		delete pBuff;
		delete pBuffComp;
		return false;
	}

	// open file to write packed veriosn
	f = fopen(m_FileName.c_str(), "wb");
	if(f==NULL)
	{
		//Error!
		CdiDbgPrint(("CDIWorkspace::PackData: fopen wb failed\n"));
		return false;
		
	}

	// Write packed data
	pHdr->pack_method = m_PackMethod;
	pHdr->unpacked_size = uncomp_len;
	pHdr->pack_size[0]=pack_sec_size[0];
	pHdr->pack_size[1]=pack_sec_size[1];
	pHdr->pack_size[2]=pack_sec_size[2];
	pHdr->pack_size[3]=pack_sec_size[3];

	// write header
	size_t written = fwrite(pHdr, 1, sizeof(FileHeader),f);
	if(sizeof(FileHeader)!= written)
	{
		CdiDbgPrint(("CDIWorkspace::PackData: ERROR!!! fwrite wb FileHeader failed - unpacket file written\n"));
		// write unpacked file
		fseek(f, 0, SEEK_SET);
		pHdr->pack_method = 0;
		pHdr->unpacked_size = 0;
		fwrite(pBuff, 1,(size_t) org_len,f);
		fclose(f);
		delete pBuff;
		delete pBuffComp;
		return false;
	}
	written=0;
	for(int i=0;i<4;i++)
		written += fwrite(pPackSec[i], 1, pack_sec_size[i],f);
	if((pack_sec_size[0]+pack_sec_size[1]+pack_sec_size[2]+pack_sec_size[3])!=written)
	{
		CdiDbgPrint(("CDIWorkspace::PackData: ERROR!!! fwrite wb BuffComp failed - unpacket file written\n"));
		// write unpacked file
		fseek(f, 0, SEEK_SET);
		pHdr->pack_method = 0;
		pHdr->unpacked_size = 0;
		fwrite(pBuff, 1, (size_t)org_len,f);
		fclose(f);
		delete pBuff;
		delete pBuffComp;
		return false;
	}

	delete pBuff;
	delete pBuffComp;

	TIME_MESURE_FINISH(("Compressed file write time: "));

	fclose(f);
	return true;

};

BYTE* CDIWorkspace::UnpackData(BYTE* data, int len)
{
	// check unpacked data size
	unsigned int decomp_len = m_hdr.unpacked_size;
	if(decomp_len ==0)
		return NULL;

	unsigned int unpack_sec_size[4];
	unpack_sec_size[0]=unpack_sec_size[1]=unpack_sec_size[2] = decomp_len/4;
	unpack_sec_size[3]=decomp_len-3*(decomp_len/4);

	// allocate new buffer
	//BYTE *pNewData = new BYTE[decomp_len];
	// copy header
	//memcpy(pNewData,  data, CDI_WS_HEADER_SIZE);

	// get pointer for decompression
	char *pCompData[4];
	pCompData[0]=(char*)data;
	pCompData[1]=pCompData[0]+m_hdr.pack_size[0];
	pCompData[2]=pCompData[1]+m_hdr.pack_size[1];
	pCompData[3]=pCompData[2]+m_hdr.pack_size[2];

	char *pUncompData[4];
	pUncompData[0]=new char[decomp_len];;
	pUncompData[1]=pUncompData[0]+unpack_sec_size[0];
	pUncompData[2]=pUncompData[1]+unpack_sec_size[1];
	pUncompData[3]=pUncompData[2]+unpack_sec_size[2];

	int decomp_ret[4]={0,0,0,0};

	INIT_TIME_MESURE
	TIME_MESURE_START

	#pragma omp parallel
	{
		#pragma omp sections
		{
			#pragma omp section
			decomp_ret[0]=BZ2_bzBuffToBuffDecompress(pUncompData[0], &unpack_sec_size[0], (char*)pCompData[0], m_hdr.pack_size[0], 0, 0);
			#pragma omp section
			decomp_ret[1]=BZ2_bzBuffToBuffDecompress(pUncompData[1], &unpack_sec_size[1], (char*)pCompData[1], m_hdr.pack_size[1], 0, 0);
			#pragma omp section
			decomp_ret[2]=BZ2_bzBuffToBuffDecompress(pUncompData[2], &unpack_sec_size[2], (char*)pCompData[2], m_hdr.pack_size[2], 0, 0);
			#pragma omp section
			decomp_ret[3]=BZ2_bzBuffToBuffDecompress(pUncompData[3], &unpack_sec_size[3], (char*)pCompData[3], m_hdr.pack_size[3], 0, 0);
		}
	}
	TIME_MESURE_FINISH(("Decompress time: "));

	if(decomp_ret[0] || decomp_ret[1] ||decomp_ret[2]||decomp_ret[3])
	{
		delete [] pUncompData[0];
		return NULL;
	}
	delete [] data;
	
	return (BYTE*)pUncompData[0];

};
} // end of namespace CDICommon

