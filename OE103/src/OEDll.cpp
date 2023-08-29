/////////////////////////////////////////////////////////////////////////////
// @doc OEDll
// @module OEDll.cpp | Extension DLL initialization.
// 
// Objective Edit<tm>
// <nl>Copyright <cp> 1998 Stingray Software, Inc. All rights reserved.
// 
// This source code is only intended as a supplement to the Objective 
// Edit User's Guide and related electronic documentation provided with 
// the library. See these sources for detailed information regarding the 
// Objective Diagram product.
// 
// Author: Stephen A. Danielson
// <nl>Created: 4/98
// 
/////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#ifdef _OEDLL

#include "OEDll.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// The following preprocessor definitions determine which version of the
// DLL needs to be initialized.

#if (_MFC_VER >= 0x0421)	// Visual C++ 5.0
	#ifndef _UNICODE
		#if defined(_DEBUG)
			LPCTSTR szLibName = _T("OE50asd.DLL");
		#else
			LPCTSTR szLibName = _T("OE50as.DLL");
		#endif // _DEBUG
	#else
		#if defined(_DEBUG)
			LPCTSTR szLibName = _T("OE50asud.DLL");
		#else
			LPCTSTR szLibName = _T("OE50asu.DLL");
		#endif // _DEBUG
	#endif // _UNICODE
#elif (_MFC_VER == 0x0420)	// Visual C++ 4.2
	#ifndef _UNICODE
		#if defined(_DEBUG)
			LPCTSTR szLibName = _T("OE42asd.DLL");
		#else
			LPCTSTR szLibName = _T("OE42as.DLL");
		#endif // _DEBUG
	#else
		#if defined(_DEBUG)
			LPCTSTR szLibName = _T("OE42asud.DLL");
		#else
			LPCTSTR szLibName = _T("OE42asu.DLL");
		#endif // _DEBUG
	#endif // _UNICODE
#elif (_MFC_VER == 0x0410)	// Visual C++ 4.1
	#ifndef _UNICODE
		#if defined(_DEBUG)
			LPCTSTR szLibName = _T("OE41asd.DLL");
		#else
			LPCTSTR szLibName = _T("OE41as.DLL");
		#endif // _DEBUG
	#else
		#if defined(_DEBUG)
			LPCTSTR szLibName = _T("OE41asud.DLL");
		#else
			LPCTSTR szLibName = _T("OE41asu.DLL");
		#endif // _DEBUG
	#endif // _UNICODE
#elif (_MFC_VER == 0x0400)	// Visual C++ 4.0
	#ifndef _UNICODE
		#if defined(_DEBUG)
			LPCTSTR szLibName = _T("OE40asd.DLL");
		#else
			LPCTSTR szLibName = _T("OE40as.DLL");
		#endif // _DEBUG
	#else
		#if defined(_DEBUG)
			LPCTSTR szLibName = _T("OE40asud.DLL");
		#else
			LPCTSTR szLibName = _T("OE40asu.DLL");
		#endif // _DEBUG
	#endif // _UNICODE
#endif // (_MFC_VER >= 0x0421)

/////////////////////////////////////////////////////////////////////////////
// Initialization of MFC Extension DLL

#include "afxdllx.h"    // standard MFC Extension DLL routines

static AFX_EXTENSION_MODULE NEAR OeExtensionDLL = { NULL, NULL };

extern "C" int APIENTRY
DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
    if (dwReason == DLL_PROCESS_ATTACH)
    {
		TRACE1("%s Initializing!\n", szLibName);
		
		// Extension DLL one-time initialization
		AfxInitExtensionModule(OeExtensionDLL, hInstance);
		
		// Insert this DLL into the resource chain
		new CDynLinkLibrary(OeExtensionDLL);
    }
    else if (dwReason == DLL_PROCESS_DETACH)
    {
		TRACE1("%s Terminating!\n", szLibName);
    }
    
    // Unused
    lpReserved;
    
    return 1;   // ok
}

extern "C" extern void WINAPI OEInitDll()
{
    // Insert this DLL into the resource chain
    new CDynLinkLibrary(OeExtensionDLL);
}

/////////////////////////////////////////////////////////////////////////////

#endif // _OEDLL




