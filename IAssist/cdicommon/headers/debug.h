#pragma once

#define _MESURE_TIME
//#define _CDI_DEBUG

#ifdef _CDI_DEBUG
void __cdecl CdiDebugPrint(PCSTR fmtstr, ...);
#define CdiDbgPrint(_x_) CdiDebugPrint _x_
#else
#define CdiDbgPrint(_x_)
#endif

// Time measurement
#ifdef _MESURE_TIME 
	#define INIT_TIME_MESURE          LARGE_INTEGER ttt1, ttt2, freq; char szBuff[256]; QueryPerformanceFrequency(&freq);
	#define TIME_MESURE_START         QueryPerformanceCounter(&ttt1);
	#define TIME_MESURE_FINISH(_arg_) QueryPerformanceCounter(&ttt2); \
        qDebug(_arg_) ; \
		sprintf(szBuff, "%i usecs\r\n",(ULONG)((ttt2.QuadPart - ttt1.QuadPart)*1000000/(freq.QuadPart))); \
        qDebug(szBuff);
	#define TIME_MESURE_ACCUMULATE(sum) QueryPerformanceCounter(&ttt2); \
		sum += (ULONG)((ttt2.QuadPart - ttt1.QuadPart)*1000000/(freq.QuadPart));
#else //_MESURE_TIME
	#define INIT_TIME_MESURE 
	#define TIME_MESURE_START
	#define TIME_MESURE_FINISH(_arg_)
	#define TIME_MESURE_ACCUMULATE(sum) 
#endif _MESURE_TIME

