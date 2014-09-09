#include "stdafx.h"

#include "cdi_str.h"
#include "graphic.h"
#include "ext_resources.h"

namespace CDICommon
{
//extern HINSTANCE g_hInst;
//extern HINSTANCE g_hRedDll;


cdi_str& LoadCdiStr(cdi_str& inStr, int inID)
{
//	HINSTANCE hMod = g_hRedDll;
    char pBuff[256] = "";
//	LoadString((HINSTANCE) hMod, inID, pBuff,  sizeof(pBuff));
	inStr = pBuff;
	return inStr;
};

cdi_str LoadCdiStr(int inID)
{
//	HINSTANCE hMod = g_hRedDll;
    char pBuff[256] = "";
	cdi_str the_string = "";
//	LoadString((HINSTANCE) hMod, inID, pBuff,  sizeof(pBuff));
	the_string = cdi_str(pBuff);
	return the_string;
};

void BuildFilterStr(int *pIDs, int id_size, char *pFilterStr)
{
	cdi_str str;
	char *pBuff = pFilterStr;
	for(int i = 0; i<id_size; i++)
	{
		LoadCdiStr(str, pIDs[i]);
		strcpy(pBuff, str.c_str());
		pBuff+=str.length();
		*pBuff = 0;
		pBuff++;
	}
};

cdi_str& operator+=(cdi_str& str, long inN)
{
	char szBuff[0x100]="";
	sprintf(szBuff,"%i",inN);
	str.append(cdi_str(szBuff));
	return str;
};
cdi_str operator+(cdi_str& str, long inN)
{
	char szBuff[0x100]="";
	cdi_str new_str = str;
	sprintf(szBuff,"%i",inN);
	new_str.append(cdi_str(szBuff));
	return new_str;
};

cdi_str GetDirectionString(int inDir)
{

	if((Direction)inDir == Left)
		return LoadCdiStr(IDS_DIR_LEFT);
	else if((Direction)inDir == Right)
		return LoadCdiStr(IDS_DIR_RIGHT);
	else if((Direction)inDir == Top)
		return LoadCdiStr(IDS_DIR_TOP);
	else if((Direction)inDir == Bottom)
		return LoadCdiStr(IDS_DIR_BOTTOM);
	else if((Direction)inDir == Anterior)
		return LoadCdiStr(IDS_DIR_ANTERIOR);
	else if((Direction)inDir == Posterior)
		return LoadCdiStr(IDS_DIR_PASTERIOR);
	else if((Direction)inDir == Lingual)
		return LoadCdiStr(IDS_DIR_LINGUAL);
	else if((Direction)inDir == Bukkal)
		return LoadCdiStr(IDS_DIR_BUKKAL);

	return cdi_str("");
	
}

}; // end of namespace CDICommon
