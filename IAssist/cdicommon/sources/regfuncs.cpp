#include "stdafx.h"

namespace CDICommon
{

BOOL ReadStringReg(HKEY hRootKey, LPCSTR inRootKey, LPSTR inKey, LPSTR outData, DWORD* size)
{
	HKEY hKey;
	DWORD dwType = REG_SZ, dwSize = sizeof(DWORD);
	DWORD idx=0;
	BOOL ret = TRUE;

	if(hRootKey == NULL)
		hRootKey = HKEY_LOCAL_MACHINE;

	if (ERROR_SUCCESS != RegOpenKey(hRootKey,(LPCTSTR)inRootKey, &hKey ))
		return FALSE;

	//
    if(ERROR_SUCCESS != RegQueryValueEx(hKey, QString(inKey).toStdWString().c_str(), 0, &dwType, (LPBYTE) outData, size))
	{
		// error
		//RegCloseKey(hKey);
		ret= FALSE;
	}

	RegCloseKey(hKey);

	return ret;
}

BOOL WriteStringReg(HKEY hRootKey, LPCSTR inRootKey, LPSTR inKey, LPCSTR inData)
{
	HKEY hKey;
	DWORD dwType = REG_SZ, dwSize = sizeof(DWORD);
	DWORD idx=0;
	BOOL ret = TRUE;
	DWORD size = strlen(inData)+1;

	if(hRootKey == NULL)
		hRootKey = HKEY_LOCAL_MACHINE;

	if (ERROR_SUCCESS != RegCreateKey(hRootKey,(LPCTSTR)inRootKey, &hKey ))
		return FALSE;

	//
    if(ERROR_SUCCESS != RegSetValueEx(hKey, QString(inKey).toStdWString().c_str(), 0, dwType, (LPBYTE) inData, size))
	{
		// error
		//RegCloseKey(hKey);
		ret= FALSE;
	}

	RegCloseKey(hKey);

	return ret;
}

BOOL ReadDWORDReg(HKEY hRootKey, LPCSTR inRootKey, LPSTR inKey, DWORD* outData)
{
	HKEY hKey;
	DWORD dwType = REG_DWORD, dwSize = sizeof(DWORD);
	DWORD idx=0;
	BOOL ret = TRUE;

	if(hRootKey == NULL)
		hRootKey = HKEY_LOCAL_MACHINE;

	if (ERROR_SUCCESS != RegOpenKey(hRootKey,(LPCTSTR)inRootKey, &hKey ))
		return FALSE;

	//
    if(ERROR_SUCCESS != RegQueryValueEx(hKey, QString(inKey).toStdWString().c_str(), 0, &dwType, (LPBYTE) outData, &dwSize))
	{
		// error
		//RegCloseKey(hKey);
		ret = FALSE;
	}

	RegCloseKey(hKey);

	return ret;
}

BOOL ReadBINReg(HKEY hRootKey, LPCSTR inRootKey, LPSTR inKey, VOID* outData, DWORD* size)
{
	HKEY hKey;
	DWORD dwType = REG_BINARY;
	DWORD idx=0;
	BOOL ret = TRUE;

	if(hRootKey == NULL)
		hRootKey = HKEY_LOCAL_MACHINE;

	if (ERROR_SUCCESS != RegOpenKey(hRootKey,(LPCTSTR)inRootKey, &hKey ))
		return FALSE;

	//
    if(ERROR_SUCCESS != RegQueryValueEx(hKey, QString(inKey).toStdWString().c_str(), 0, &dwType, (LPBYTE) outData, size))
	{
		// error
		//RegCloseKey(hKey);
		ret= FALSE;
	}
	RegCloseKey(hKey);

	return ret;
};

BOOL WriteBINReg(HKEY hRootKey, LPCSTR inRootKey, LPSTR inKey, VOID* inData, DWORD size)
{
	HKEY hKey;
	BOOL ret = TRUE;

	if(hRootKey == NULL)
		hRootKey = HKEY_LOCAL_MACHINE;

	if (ERROR_SUCCESS != RegCreateKey(hRootKey,(LPCTSTR)inRootKey, &hKey ))
		return FALSE;

	//
    if(ERROR_SUCCESS != RegSetValueEx(hKey, QString(inKey).toStdWString().c_str(), 0, REG_BINARY, (LPBYTE) inData, size))
	{
		// error
		
		ret= FALSE;
	}

	RegCloseKey(hKey);

	return ret;
};

BOOL WriteDWORDReg(HKEY hRootKey, LPCSTR inRootKey, LPSTR inKey, DWORD* inData)
{
	HKEY hKey;
	BOOL ret = TRUE;

	if(hRootKey == NULL)
		hRootKey = HKEY_LOCAL_MACHINE;

	if (ERROR_SUCCESS != RegCreateKey(hRootKey,(LPCTSTR)inRootKey, &hKey ))
		return FALSE;

	//
    if(ERROR_SUCCESS != RegSetValueEx(hKey, QString(inKey).toStdWString().c_str(), 0, REG_DWORD, (LPBYTE) inData, sizeof(DWORD)))
	{
		// error
		ret= FALSE;
	}

	RegCloseKey(hKey);

	return ret;
};
} //end of namespace CDICommon
