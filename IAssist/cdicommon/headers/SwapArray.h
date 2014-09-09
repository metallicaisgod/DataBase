#pragma once

using namespace std;
namespace CDICommon
{
#ifndef _SWARR_IDX
#define _SWARR_IDX
int g_SwapArray_Idx = 1;
#endif //|_SWARR_IDX

template <class _A>
class SwapArray
{
public:
	SwapArray(_A* pData, int n)
	{
		m_pBuff = new _A[n];
		m_nSize = n;
		for(int i=0;i<n;i++)
			m_pBuff[i] = pData[i];

	};
	virtual ~SwapArray()
	{
		if(m_pBuff)
			delete [] m_pBuff ;
		if(!m_strFile.empty())
			DeleteFile(m_strFile.c_str());
	};
	
	int GetSize() { return m_nSize; }
	bool Swap()
	{
		char szBuff[MAX_PATH+1]="";
		DWORD dwSize=GetTempPath(0,0);
       	char *pPath = new char[dwSize+MAX_PATH+1];
		if(dwSize-1!=GetTempPath(dwSize,pPath))
		{
			MessageBox(NULL, "Swap error - tmppath!", "Error",  MB_OK);
			delete [] pPath;
			return false;
		};

		GetTempFileName(pPath, "CDI", g_SwapArray_Idx++, pPath);

		FILE *pF = fopen(pPath, "w");

		m_strFile = pPath;
		delete [] pPath;

		if(pF==NULL)
		{
			MessageBox(NULL, "Swap save error - open!", "Error", MB_OK);
			m_strFile = "";
			return false;
		};
		
		if(1 != fwrite(m_pBuff, m_nSize*sizeof(_A),1,pF))
		{
			MessageBox(NULL, "Swap save error - write!", "Error", MB_OK);
			fclose(pF);
			m_strFile = "";
			return false;
		};
		
		delete [] m_pBuff ;
		m_pBuff = NULL;
		fclose(pF);
	};
	
	bool Load()
	{
		FILE *pF = fopen(m_strFile.c_str(), "r");
		if(pF==NULL)
		{
			MessageBox(NULL, "Swap load error - open!", "Error", MB_OK);
			return false;
		};
		m_pBuff = new _A[m_nSize];
		if(1 != fread(m_pBuff, m_nSize*sizeof(_A),1,pF))
		{
			MessageBox(NULL, "Swap load error - write!", "Error", MB_OK);
			fclose(pF);
			return false;
		};
		
		fclose(pF);
		DeleteFile(m_strFile.c_str());
		m_strFile = "";
	};

	_A GetAt(int idx)
	{

		if(m_pBuff)
			return m_pBuff[idx];
		// get data from swap file
		if(Load())
			return m_pBuff[idx];

		// return something
		_A a;
		memset(&a, 0, sizeof(_A));
		return a;
	};


protected:

	_A *m_pBuff;
	int m_nSize;
	std::string m_strFile;
};

};