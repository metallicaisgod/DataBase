//// stdafx.h : include file for standard system include files,
//// or project specific include files that are used frequently,
//// but are changed infrequently

//#pragma once

//#ifndef _SECURE_ATL
//#define _SECURE_ATL 1
//#endif

//#ifndef VC_EXTRALEAN
//#define VC_EXTRALEAN // Exclude rarely-used stuff from Windows headers
//#endif

//#ifndef WINVER                          // Specifies that the minimum required platform is Windows Vista.
//#define WINVER 0x0400           // Change this to the appropriate value to target other versions of Windows.
//#endif

//#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows Vista.
//#define _WIN32_WINNT 0x0400     // Change this to the appropriate value to target other versions of Windows.
//#endif

//#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
//#define _WIN32_WINDOWS 0x0410 // Change this to the appropriate value to target Windows Me or later.
//#endif

//#ifndef _WIN32_IE                       // Specifies that the minimum required platform is Internet Explorer 7.0.
//#define _WIN32_IE 0x0400        // Change this to the appropriate value to target other versions of IE.
//#endif


//#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS // some CString constructors will be explicit

//// turns off MFC's hiding of some common and often safely ignored warning messages
//#define _AFX_ALL_WARNINGS

//#define _BIND_TO_CURRENT_VCLIBS_VERSION 1

//#include <afxwin.h>         // MFC core and standard components
//#include <afxext.h>         // MFC extensions

//#include <afxhtml.h>
//#include <afxdisp.h>        // MFC Automation classes

//#ifndef _AFX_NO_OLE_SUPPORT
//#include <afxdtctl.h>       // MFC support for Internet Explorer 4 Common Controls
//#endif
//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>         // MFC support for Windows Common Controls
//#endif // _AFX_NO_AFXCMN_SUPPORT

//#include <afxcontrolbars.h>	// MFC support for ribbons and control bars

//#if defined _M_IX86
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_IA64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#elif defined _M_X64
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#else
//#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
//#endif

//#ifdef _DEBUG
//#define _HAS_ITERATOR_DEBUGGING 0
//#define _SECURE_SCL 0
//#endif //

// turns off MFC's hiding of some common and often safely ignored warning messages
//#define _AFX_ALL_WARNINGS

#include <QtCore>
#include <QtGui>

#include <algorithm>
#include <string.h>
#include <limits>
#include <vector>
#include <map>
#include <deque>
#include <cmath>
#include <stdio.h>

#include <process.h>
#include <iostream>

//#ifndef _AFX_NO_AFXCMN_SUPPORT
//#include <afxcmn.h>			// MFC support for Windows Common Controls
//#endif // _AFX_NO_AFXCMN_SUPPORT
