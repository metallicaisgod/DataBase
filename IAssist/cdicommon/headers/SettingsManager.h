#pragma once
#include "regfuncs.h"
#include "cdi_str.h"

#define SETTINGS_ROOT "CTA_Settings"
namespace CDICommon
{

CString GetAppExeFolder();
class SettingsManager
{
public:
	SettingsManager();
	~SettingsManager(void);
	bool GetParam(void *pParam, int size, LPSTR strRoot, LPSTR strKey, int idx=-1);
	DWORD GetParamDataLen(LPSTR strRoot, LPSTR strKey, int idx=-1);
	
	template <class _A>
	bool GetParamT(_A *pParam, LPSTR strRoot, LPSTR strKey, int idx=-1)
	{ 
		return GetParam(pParam, sizeof(_A), strRoot, strKey, idx) ; 
	};
	template <class _A>
	bool GetValidParamT(_A *pParam, LPSTR strRoot, LPSTR strKey, _A min, _A max, 
		_A def, bool min_strict=true, bool max_strict=true, int idx=-1)
	{ 
		if(!GetParam(pParam, sizeof(_A), strRoot, strKey, idx))
		{
			*pParam = def;
			return false;
		}
		// validate the parameters
		if(min_strict)
		{	
			// strict validation
			if(*pParam <= min)
			{
				*pParam = def;
				return false;
			}
		}
		else 
		{
			// unstrict validation
			if(*pParam < min)
			{
				*pParam = def;
				return false;
			}
		}
		if(max_strict)
		{
			// strict validation
			if(*pParam >= max)
			{
				*pParam = def;
				return false;
			}
		}
		else 
		{
			// unstrict validation
			if(*pParam > max)
			{
				*pParam = def;
				return false;
			}
		}	
		// validation Ok!
		return true; 
	};
	bool GetString(cdi_str* outStr, LPSTR strRoot, LPSTR strKey, int idx=-1);
	bool GetString(CString* outStr, LPSTR strRoot, LPSTR strKey, int idx=-1);
	

	bool SetParam(void *pParam, int size, LPSTR strRoot, LPSTR strKey, int idx=-1);
	template <class _A>
	bool SetParamT(_A *pParam, LPSTR strRoot, LPSTR strKey, int idx=-1)
	{ return SetParam(pParam, sizeof(_A), strRoot, strKey, idx) ; };

	bool SetString(cdi_str* inStr, LPSTR strRoot, LPSTR strKey, int idx=-1);
	bool SetString(CString* inStr, LPSTR strRoot, LPSTR strKey, int idx=-1);
	

	static void SetApp(CWinApp* in) {s_pApp=in;};
protected:
	static CWinApp* s_pApp;
};

}; // end of namespace CDICommon;