#include "stdafx.h"
#include "email_send.h" 
#include <mapi.h>

namespace CDICommon
{

bool EmailSend(char* addr, char* subj, char* attach_file, char* body)
{
	
//	HKEY hOpenKey=NULL;
//	char szMailClDef[MAX_PATH]="";
//	DWORD dwMailClDefSize=MAX_PATH;
    bool bSent = false;

//    QSettings settings("HKEY_LOCAL_MACHINE\\SOFTWARE\\Clients\\Mail",
//                       QSettings::NativeFormat);
//	if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,"SOFTWARE\\Clients\\Mail",0,KEY_READ,&hOpenKey) == ERROR_SUCCESS)
//	{
//		if(RegQueryValueEx(hOpenKey,NULL,0,0,(LPBYTE)szMailClDef,&dwMailClDefSize)==ERROR_SUCCESS)
//		{
            //if (!strcmp("Outlook Express",szMailClDef))
//			{
				HINSTANCE   hMapiLib;

				MapiRecipDesc empf;

				empf.ulReserved        = 0;  
				empf.ulRecipClass      = MAPI_TO;       
				empf.lpszName          = " ";                  
				empf.lpszAddress       = addr; 
				empf.ulEIDSize         = 0;
				empf.lpEntryID         = NULL;

				MapiMessage mames;

				mames.ulReserved			= 0;
				mames.lpszSubject			= subj;
				mames.lpszNoteText			= body;
				mames.lpszMessageType		= NULL;
				mames.lpszDateReceived		= NULL;
				mames.lpszConversationID	= NULL;
				mames.flFlags				= 0;
				mames.lpOriginator			= NULL;     
				mames.nRecipCount			= 1;   
				mames.lpRecips				= &empf;
				mames.nFileCount			= 1;							

				MapiFileDesc FileDesc;
                QString strFile = attach_file;
//				int st=0, st1=0;
//				while(st1!=-1)
//				{
//					st = st1+1;
//					st1 = strFile("\\",st);
//				}
//				strFile = strFile.Right(strFile.GetLength()-st);
                strFile = QFileInfo(strFile).fileName();

				FileDesc.flFlags			= 0;//MAPI_OLE_STATIC;
				FileDesc.lpFileType			= NULL;
                FileDesc.lpszFileName		= strFile.toLocal8Bit().data();
				FileDesc.lpszPathName		= attach_file;
				FileDesc.nPosition			= -1;
				FileDesc.ulReserved			= 0;

				mames.lpFiles	= &FileDesc;
				//mames.lpFiles->flFlags		    = MAPI_OLE;
				//mames.lpFiles->lpFileType		= NULL;
				//mames.lpFiles->lpszFileName		= _T("log.ipx");
				//mames.lpFiles->lpszPathName		= _T("C:\\log.ipx");
				//mames.lpFiles->nPosition		= -1;
				//mames.lpFiles->ulReserved		= 0;

                //CWnd *pW = AfxGetMainWnd();
                QWindow * pW = qApp->focusWindow();
				HWND hWnd=NULL;
				if(pW)
                    hWnd = (HWND)pW->winId();

				LPMAPISENDMAIL MAPISendMail; 
                if ( (hMapiLib = LoadLibrary(L"MAPI32.DLL")) == INVALID_HANDLE_VALUE )
				{
                    MessageBox(hWnd, L"Can't load MAPI32.DLL",L"Error", MB_ICONERROR);
				} 
				if ((MAPISendMail = (LPMAPISENDMAIL)GetProcAddress (hMapiLib, "MAPISendMail")) == NULL) 
				{
                    MessageBox(hWnd, L"Cannot find MAPISendMail", L"Error", MB_ICONERROR);
				} 
				
				ULONG ret = MAPISendMail (0, (ULONG)hWnd, &mames, MAPI_DIALOG, 0);
				if(ret == SUCCESS_SUCCESS || ret == MAPI_USER_ABORT)
					bSent=true;
				FreeLibrary(hMapiLib);
            //}
			if(!bSent)
			{
		
                QString str;
                str = QString("mailto:%1?subject=%2&Attach=%3&body=%4").arg(addr).arg(subj).arg(attach_file).arg(body);
                str.replace(QString(" "),QString("%20"));
                str.replace(QString("\r\n"),QString("%0A"));
                str.replace(QString("\t"),QString("%20%20%20%20"));
                wchar_t * array = new wchar_t[str.length()];
                str.toWCharArray(array);
                ShellExecute(NULL,NULL,array,NULL,NULL,SW_SHOWNORMAL);
                delete [] array;
			}
//		}
//		RegCloseKey(hOpenKey);
//	}

	return true;

}

}
