#include "stdafx.h"
#include "debug.h"

#ifdef _CDI_DEBUG
void __cdecl CdiDebugPrint(PCSTR fmtstr, ...)
{
	char szDebugBuff[0x100]="";
	va_list arglist;
	va_start(arglist, fmtstr);
    if(arglist)
		vsprintf(szDebugBuff, fmtstr, arglist);
	else
		sprintf(szDebugBuff, fmtstr, arglist);
    va_end(arglist);

    qDebug(szDebugBuff);

}
#endif
