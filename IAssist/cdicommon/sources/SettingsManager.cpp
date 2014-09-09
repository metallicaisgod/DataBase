#include "stdafx.h"
#include "settingsmanager.h"

namespace CDICommon
{

CWinApp* SettingsManager::s_pApp = NULL;

CString GetAppExeFolder()
{
	LPWSTR pCmdLine = GetCommandLineW();
	CString strAppDir;
	int NumArgs=0;
	LPWSTR* pArgs = CommandLineToArgvW(  (LPCWSTR) pCmdLine, &NumArgs);
	int len = (int)wcslen(pArgs[0]);
	char *szCmdLine = new char[len+1];
    WideCharToMultiByte(CP_OEMCP, 0, pArgs[0], -1, szCmdLine, len+1, NULL, NULL);
	int i= len;
    while(szCmdLine[i]!='\\' && szCmdLine[i]!='/')
	{
		if(szCmdLine[i]=='\\' || szCmdLine[i]=='/')
			break;
		i--;
	};
	szCmdLine[i+1]=0;
	strAppDir = szCmdLine;
	delete [] szCmdLine;
	return strAppDir;

}
SettingsManager::SettingsManager()
{
}

SettingsManager::~SettingsManager(void)
{
}

bool SettingsManager::GetParam(void *pParam, int inSize, LPSTR strRoot, LPSTR inStrKey, int idx)
{
	DWORD size = inSize;
	BOOL ret = FALSE;
	HKEY hKey=NULL;
	cdi_str strKey = cdi_str(inStrKey);
	if(idx!=-1)
	{	
		char sz[256]="";
		sprintf(sz, "%i",idx);
		strKey+=cdi_str(sz);
	};

	if(ERROR_SUCCESS == RegCreateKey(s_pApp->GetAppRegistryKey(),SETTINGS_ROOT, &hKey ))
	{
		ret = ReadBINReg(hKey,(LPSTR)strRoot, (LPSTR)strKey.c_str(), pParam, &size);
		RegCloseKey(hKey);
		if(ret)
			return true;
		else
			return false;
	}
	return false;
};
DWORD SettingsManager::GetParamDataLen(LPSTR strRoot, LPSTR inStrKey, int idx)
{
	DWORD size = 0;
	BOOL ret = FALSE;
	HKEY hKey=NULL;
	cdi_str strKey = cdi_str(inStrKey);
	if(idx!=-1)
	{	
		char sz[256]="";
		sprintf(sz, "%i",idx);
		strKey+=cdi_str(sz);
	};

	if(ERROR_SUCCESS == RegCreateKey(s_pApp->GetAppRegistryKey(),SETTINGS_ROOT, &hKey ))
	{
		ret = ReadBINReg(hKey,(LPSTR)strRoot, (LPSTR)strKey.c_str(), NULL, &size);
		RegCloseKey(hKey);
	}
	return size;
};
bool SettingsManager::SetParam(void *pParam, int size, LPSTR strRoot, LPSTR inStrKey, int idx)
{
	BOOL ret;
	cdi_str strKey = cdi_str(inStrKey);
	HKEY hKey;

	if(idx!=-1)
	{	
		char sz[256]="";
		sprintf(sz, "%i",idx);
		strKey+=cdi_str(sz);
	};

	if(ERROR_SUCCESS == RegCreateKey(s_pApp->GetAppRegistryKey(),SETTINGS_ROOT, &hKey ))
	{
		ret = WriteBINReg(hKey, (LPSTR)strRoot, (LPSTR)strKey.c_str(), pParam, size);
		RegCloseKey(hKey);
		if(ret)
			return true;
		else
			return false;
	}
	return false;
};


bool SettingsManager::GetString(cdi_str* outStr, LPSTR strRoot, LPSTR strKey, int idx)
{
	DWORD size = GetParamDataLen(strRoot, strKey, idx);
	char *pBuff = new  char[size+1];
	bool ret = GetParam(pBuff, size+1, strRoot, strKey, idx);
	if(ret)
		*outStr = cdi_str(pBuff);
	delete [] pBuff;
	return ret;
};
bool SettingsManager::GetString(CString* outStr, LPSTR strRoot, LPSTR strKey, int idx)
{
	cdi_str str;
	bool ret = GetString(&str,  strRoot, strKey, idx);
	*outStr = str.c_str();
	return ret;

};

bool SettingsManager::SetString(cdi_str* inStr, LPSTR strRoot, LPSTR strKey, int idx)
{
	return SetParam((void*)inStr->c_str(), (int)inStr->length()+1, strRoot, strKey, idx);
};
	
bool SettingsManager::SetString(CString* inStr, LPSTR strRoot, LPSTR strKey, int idx)
{
	return SetParam((void*)LPCTSTR(*inStr), inStr->GetLength()+1, strRoot, strKey, idx);
};

}; // end of namespace CDICommon