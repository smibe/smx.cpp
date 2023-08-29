// FCUKeyGen.cpp : Defines the initialization routines for the DLL.
//

#include "stdafx.h"
#include "FCUKGen.h"
#include "key_intf.h"
#include "..\..\SmxUserExt\src\crypt.h"

#ifdef KEYGEN_API
#define DLLEXPORT __declspec( dllexport )
#else 
#define DLLEXPORT __declspec( dllimport )
#endif

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

CString m_strKey;

// The one and only CFCUKeyGenApp object
CFCUKeyGenApp theApp;

/* generate a key from the supplied input
 * 4000 bytes of memory have been allocated for each result 
 * you should ensure that your output does not get longer ! */
extern "C"
{
  DLLEXPORT int __stdcall
  GenKeyEx(char * args[], char *result1, char *result2)
  {
    CFCUKeyGen KeyGen;
    return KeyGen.GenKeyEx(args, result1, result2, 4000);
  }
}

#define PASSWD_LEN    12  //length of a password
CTime   gl_PasswdTime(1972, 11, 14, 0, 0, 0);
LPCTSTR szCode = _T("2541");
LPCTSTR szIs   = _T("is");

CString GetPasswdKey(LPCTSTR pszRegName)
{
  CString str1;
  CString str2;
  str1.LoadString(IDS_small);
  str2.LoadString(IDS_beautiful);

  if (pszRegName)
    return str1.Left(2) + TCHAR('#') + str1.Mid(2) + szIs[0] + str2.Left(3) + szCode + pszRegName + str2.Mid(3);
  else
    return str1.Left(2) + TCHAR('#') + str1.Mid(2) + szIs[0] + str2.Left(3) + szCode + str2.Mid(3);
}


CFCUKeyGenApp::CFCUKeyGenApp()
{
}

CFCUKeyGen::CFCUKeyGen()
{
  m_pszRegName = NULL;
  m_pszEmail   = NULL;
  m_nLanguage  = 0;
}

int CFCUKeyGen::GenKeyEx(char * args[], char *pszResult1, char *pszResult2, int nBufSize)
{
  // parse input array
  if (!ParseInput(args))
  {
    strcpy(pszResult1, "Could not parse the arguments!");
    return ERC_BAD_INPUT;
  }    

  // first check for valid input
  if (strlen(m_pszRegName) < 8) 
  {
    strcpy(pszResult1, "<reg_name> must have at least 8 characters!");
    return ERC_BAD_INPUT;
  }

  if (strlen(m_pszEmail) < 6) 
  {
    strcpy(pszResult1, "<email> must have at least 6 characters!");
    return ERC_BAD_INPUT;
  }

  // generate the key
  CString str = GetKey(m_pszRegName);

  //key = CalcKey(reg_name);
  //wsprintf(pszResult2, "%x", key);

  // generate the userkey
  if (m_nLanguage == liGerman)
    wsprintf(pszResult1, "Benutzer: %s\r\nSchlüssel: %s", m_pszRegName, str);
  else
    wsprintf(pszResult1, "Username: %s\r\nKey: %s", m_pszRegName, str);

  return ERC_SUCCESS;
}

// parse input array and assign values to variables
bool CFCUKeyGen::ParseInput(char *args[])
{
  // check for null array pointer
  if (!args) return 
    false;

  // get input values from array 
  while (args)
  {
    char *pszTag   = NULL;
    char *pszValue = NULL;

    // get next tag
    pszTag = *args++;
    if (!pszTag)	
      break;

    // get assigned value for tag 
    pszValue = *args++;
    if (!pszValue)	
      break; // oops a tag without a value

    // assign tag value
    // ADD MORE VALUES AS YOU NEED THEM
    if (!stricmp(pszTag, "REG_NAME")) 
      m_pszRegName = pszValue;
    else if (!stricmp(pszTag, "EMAIL")) 
      m_pszEmail = pszValue;
    else if (!stricmp(pszTag, "LANGUAGE_ID")) 
      m_nLanguage = atoi(pszValue);
  }

  return true;
}

LPCTSTR CFCUKeyGen::GetKey(LPCTSTR pszRegName)
{
  m_strKey.Format("%02d%02d%02d", gl_PasswdTime.GetDay(), gl_PasswdTime.GetMonth(),  gl_PasswdTime.GetYear() - 1966);
  CString strKey = GetPasswdKey(pszRegName);
  CCrypt Crypt;

  Crypt.EncryptDigits(m_strKey, strKey, PASSWD_LEN);

  return m_strKey;
}

