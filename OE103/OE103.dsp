# Microsoft Developer Studio Project File - Name="OE103" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=OE103 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "OE103.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "OE103.mak" CFG="OE103 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "OE103 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "OE103 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE "OE103 - Win32 Release MFC Static" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/OE103", NEAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "OE103 - Win32 Release"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin\oe50a.lib"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "." /I "..\SmxUserExt\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\Debug\oe50a.lib"

!ELSEIF  "$(CFG)" == "OE103 - Win32 Debug"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 2
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\bin\oe50ad.lib"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "." /I "..\SmxUserExt\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\Debug\oe50ad.lib"

!ELSEIF  "$(CFG)" == "OE103 - Win32 Release MFC Static"

# PROP BASE Use_MFC 2
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "OE103___Win32_Release_MFC_Static"
# PROP BASE Intermediate_Dir "OE103___Win32_Release_MFC_Static"
# PROP BASE Target_Dir ""
# PROP Use_MFC 1
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\bin\Release_MFC_Static"
# PROP Intermediate_Dir "Release_MFC_Static"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /I "." /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MT /W3 /GX /O2 /I "." /I "..\..\SmxUserExt\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo /out:"..\..\lib\oe50a.lib"
# ADD LIB32 /nologo /out:"..\..\lib\oe50.lib"

!ENDIF 

# Begin Target

# Name "OE103 - Win32 Release"
# Name "OE103 - Win32 Debug"
# Name "OE103 - Win32 Release MFC Static"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\src\EditChroma.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EditCopy.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EditCore.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EditCut.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EditDelete.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EditFile.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EditFind.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EditInsert.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EditJoinLines.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EditLine.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EditPaste.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EditSplitLine.cpp
# End Source File
# Begin Source File

SOURCE=.\src\EditUndo.cpp
# End Source File
# Begin Source File

SOURCE=.\src\OEDll.cpp
# End Source File
# Begin Source File

SOURCE=.\src\OEFindDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\OEPageset.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SecEdit.rc
# End Source File
# Begin Source File

SOURCE=.\src\SECEditCore.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SECEditCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SECEditCtrlOleDropTarget.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SECEditHint.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SECEditView.cpp
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.cpp

!IF  "$(CFG)" == "OE103 - Win32 Release"

# ADD CPP /I "src" /Yc"stdafx.h"
# SUBTRACT CPP /I "."

!ELSEIF  "$(CFG)" == "OE103 - Win32 Debug"

# ADD CPP /I "src" /Yc"stdafx.h"
# SUBTRACT CPP /I "."

!ELSEIF  "$(CFG)" == "OE103 - Win32 Release MFC Static"

# ADD CPP /Yc"stdafx.h"

!ENDIF 

# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\src\editres.h
# End Source File
# Begin Source File

SOURCE=.\src\OEdit.h
# End Source File
# Begin Source File

SOURCE=.\src\OEDll.h
# End Source File
# Begin Source File

SOURCE=.\src\OEFindDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\OEPageset.h
# End Source File
# Begin Source File

SOURCE=.\src\OETempl.h
# End Source File
# Begin Source File

SOURCE=.\src\SECEdit.h
# End Source File
# Begin Source File

SOURCE=.\src\SECEditCore.inl
# End Source File
# Begin Source File

SOURCE=.\src\SECEditCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\SECEditCtrlOleDropTarget.h
# End Source File
# Begin Source File

SOURCE=.\src\SECEditHint.h
# End Source File
# Begin Source File

SOURCE=.\src\SECEditView.h
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\src\Struct.h
# End Source File
# Begin Source File

SOURCE=.\src\View_Core.inl
# End Source File
# Begin Source File

SOURCE=.\src\ViewBackspace.inl
# End Source File
# Begin Source File

SOURCE=.\src\ViewCopy.inl
# End Source File
# Begin Source File

SOURCE=.\src\ViewCursor.inl
# End Source File
# Begin Source File

SOURCE=.\src\ViewCut.inl
# End Source File
# Begin Source File

SOURCE=.\src\ViewDelete.inl
# End Source File
# Begin Source File

SOURCE=.\src\ViewInsert.inl
# End Source File
# Begin Source File

SOURCE=.\src\ViewJoinLines.inl
# End Source File
# Begin Source File

SOURCE=.\src\ViewPaint.inl
# End Source File
# Begin Source File

SOURCE=.\src\ViewPaste.inl
# End Source File
# Begin Source File

SOURCE=.\src\ViewSelect.inl
# End Source File
# Begin Source File

SOURCE=.\src\ViewSplitLine.inl
# End Source File
# Begin Source File

SOURCE=.\src\ViewTab.inl
# End Source File
# End Group
# Begin Source File

SOURCE=.\src\res\idccurso.cur
# End Source File
# End Target
# End Project
