#include "stdafx.h"

namespace CDICommon
{
bool IsSHIFT_Pressed()
{
	bool ret = ((GetAsyncKeyState(VK_SHIFT) & 0x8000)==0x8000) ;
	return ret;
};
bool IsALT_Pressed()
{
	bool ret = ((GetAsyncKeyState(VK_MENU) & 0x8000)==0x8000) ;
	return ret;
};
bool IsCTRL_Pressed()
{
	bool ret = ((GetAsyncKeyState(VK_CONTROL) & 0x8000)==0x8000) ;
	return ret;
};
}