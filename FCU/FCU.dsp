# Microsoft Developer Studio Project File - Name="FCU" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=FCU - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "FCU.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "FCU.mak" CFG="FCU - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "FCU - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "FCU - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "FCU - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "..\oe103\src" /I "..\SMXUserExt\src" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /i "..\SMXUserExt\src" /i "..\OE103\src" /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 SMXUserExt.lib /nologo /subsystem:windows /machine:I386 /libpath:"..\lib\Release"

!ELSEIF  "$(CFG)" == "FCU - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "..\oe103\src" /I "..\SMXUserExt\src" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /D "_MBCS" /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x407 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x407 /i "..\SMXUserExt\src" /i "..\OE103\src" /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 SMXUserExt.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept /libpath:"..\lib\Debug"

!ENDIF 

# Begin Target

# Name "FCU - Win32 Release"
# Name "FCU - Win32 Debug"
# Begin Source File

SOURCE=.\src\res\bitmap1.bmp
# End Source File
# Begin Source File

SOURCE=.\src\CmpDirDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CmpDirDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\CmpFilDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CmpFilDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\ColorPg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ColorPg.h
# End Source File
# Begin Source File

SOURCE=.\src\ComparePg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\ComparePg.h
# End Source File
# Begin Source File

SOURCE=.\src\CopyFilesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\CopyFilesDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\res\diff_dir.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\diff_dir.ico
# End Source File
# Begin Source File

SOURCE=.\src\res\diff_fil.bmp
# End Source File
# Begin Source File

SOURCE=.\src\res\Diff_Fil.ico
# End Source File
# Begin Source File

SOURCE=.\src\DiffDirDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DiffDirDoc.h
# End Source File
# Begin Source File

SOURCE=.\src\DiffDirView.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DiffDirView.h
# End Source File
# Begin Source File

SOURCE=.\src\DiffEditCtrl.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DiffEditCtrl.h
# End Source File
# Begin Source File

SOURCE=.\src\DiffSectElt.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DiffSectElt.h
# End Source File
# Begin Source File

SOURCE=.\src\DisplayPg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\DisplayPg.h
# End Source File
# Begin Source File

SOURCE=.\src\FCU.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FCU.h
# End Source File
# Begin Source File

SOURCE=.\HtmlHelp\fcu.hm

!IF  "$(CFG)" == "FCU - Win32 Release"

!ELSEIF  "$(CFG)" == "FCU - Win32 Debug"

USERDEP__FCU_H="src\resource.h"	
# Begin Custom Build
WkspDir=.
InputPath=.\HtmlHelp\fcu.hm

"HtmlHelp\fcu.hh" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	cd $(WkspDir) 
	makehidx.bat 
	
# End Custom Build

!ENDIF 

# End Source File
# Begin Source File

SOURCE=.\src\FCU.rc
# End Source File
# Begin Source File

SOURCE=.\src\FcuCmdLineInfo.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FcuCmdLineInfo.h
# End Source File
# Begin Source File

SOURCE=.\src\FcuOptions.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FCUOptions.h
# End Source File
# Begin Source File

SOURCE=.\src\FilDiff.rc
# PROP Exclude_From_Build 1
# End Source File
# Begin Source File

SOURCE=.\src\FilDiff_res.h
# End Source File
# Begin Source File

SOURCE=.\src\FilDiffDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FilDiffDoc.h
# End Source File
# Begin Source File

SOURCE=.\src\FilDiffFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FilDiffFrm.h
# End Source File
# Begin Source File

SOURCE=.\src\FilDiffParam.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FilDiffParam.h
# End Source File
# Begin Source File

SOURCE=.\src\FilDiffView.cpp
# End Source File
# Begin Source File

SOURCE=.\src\FilDiffview.h
# End Source File
# Begin Source File

SOURCE=.\src\FilDiffXmlTags.h
# End Source File
# Begin Source File

SOURCE=.\src\res\hsize.cur
# End Source File
# Begin Source File

SOURCE=.\src\res\idr_main.ico
# End Source File
# Begin Source File

SOURCE=.\src\LINE.CXX
# End Source File
# Begin Source File

SOURCE=.\src\LINE.H
# End Source File
# Begin Source File

SOURCE=.\src\LIST.cxx
# End Source File
# Begin Source File

SOURCE=.\src\LIST.H
# End Source File
# Begin Source File

SOURCE=.\src\res\mainfram.bmp
# End Source File
# Begin Source File

SOURCE=.\src\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\src\MDIChildWndEx.cpp
# End Source File
# Begin Source File

SOURCE=.\src\MDIChildWndEx.h
# End Source File
# Begin Source File

SOURCE=.\src\PasswdDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\PasswdDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\res\question.bmp
# End Source File
# Begin Source File

SOURCE=.\src\resource.h
# End Source File
# Begin Source File

SOURCE=.\src\SaveCompDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SaveCompDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\SECTION.cxx
# End Source File
# Begin Source File

SOURCE=.\src\SECTION.H
# End Source File
# Begin Source File

SOURCE=.\src\SelectFilesDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\SelectFilesDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\STATE.H
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\src\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\src\TREE.cxx
# End Source File
# Begin Source File

SOURCE=.\src\TREE.H
# End Source File
# Begin Source File

SOURCE=.\src\UsageDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\src\UsageDlg.h
# End Source File
# Begin Source File

SOURCE=.\src\res\vsize.cur
# End Source File
# End Target
# End Project
