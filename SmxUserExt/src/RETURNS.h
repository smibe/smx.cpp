/*******************************************************************************
// Copyright (C), GeH
// All rights reserved. 
//------------------------------------------------------------------------------
//
// PROJECT   : FCU - File Compare Utility
// $RCSfile: RETURNS.h,v $
// 
// ABSTRACT:
//
//------------------------------------------------------------------------------
//
// $Header: D:\\CVSRoot/SmxUserExt/src/RETURNS.h,v 1.1 2002/10/12 07:20:55 GeH Exp $
//
********************************************************************************/
// START AUTODUCK parsing
// @doc

// ==========================================================================
// 							Return Codes specification
// ==========================================================================

// Header file : returns.h

// Source : Periphere NV (F.Melendez)
// Creation Date : 	   16th November 1995
// Last Modification : 16th November 1995
                          
// //////////////////////////////////////////////////////////////////////////
// Desciption :         
//		Contains some return codes used internally in dirspec 

// Remark:
//		***

// Prerequisites (necessary conditions):
//		***

/////////////////////////////////////////////////////////////////////////////
#ifndef __RETURNS_H__
#define __RETURNS_H__

#ifndef WIN32
#define DRIVE_CDROM 5
#define DRIVE_RAMDISK 6
#endif

// Statuses
#define SUCCESS 0
#define F_OPENFILE 1
#define F_MEMORY 2
#define F_NOINDEXES 3
#define F_SEEK 4
#define F_READ 5
#define F_NOTFOUND 6                                     
#define F_CREATEPROCESS 7
#define F_GETCURRENTDIR 8
#define F_SETCURRENTDIR 9
#define F_CREATEDIR 10
#define F_COPYFILE 11
#define F_NOCD 12
#define F_NOVOL 13
#define F_CANCEL 14
#define F_INSTALLPROG 15
#define F_INVALIDDIRLEN 16
#define F_NODRIVE 17
#define F_RESERVED 18
#define F_MIPSONLY 19
#define F_INTELONLY 20
#define F_ALPHAONLY 21
#define F_32ONLY 22
#define F_DELETEFILE 23
#define F_DELETEDIR 24
#define F_NOWAIT 25
#define F_WRITE 26
#define F_WIN95ONLY 27

// platforms                 
#define DOSEXE 1
#define WINEXE 16
#define NTIEXE 32
#define NTMEXE 33
#define NTAEXE 34
#define NTINTELEXE 35
#define WIN95EXE 40

// directory defines
#define MAXDIRLEN 256
#define NUMDRIVES 26
#define FIXED 0
#define REMOVEABLE 1
#define REMOTE 2
#define CDROM 3

#endif // __RETURNS_H__
// ==========================================================================
