//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: localassert.h $
// $Date: 2002/08/16 02:22:41 $
//
//-------------------------------------------------------------------------------
// $Log: localassert.h,v $
// Revision 1.4  2002/08/16 02:22:41  Flayra
// - Document header
//
//===============================================================================
// Renamed this file for case-sensitivity issues under Linux
#ifndef LOCAL_ASSERT_H
#define LOCAL_ASSERT_H

#include "util/nowarnings.h"
#include "types.h"

#ifdef	DEBUG
void DBG_AssertFunction(bool fExpr, const char* szExpr, const char* szFile, int szLine, const char* szMessage);
#define ASSERT(f)		DBG_AssertFunction(f, #f, __FILE__, __LINE__, NULL)
#define ASSERTSZ(f, sz)	DBG_AssertFunction(f, #f, __FILE__, __LINE__, sz)
#else	// !DEBUG
#define ASSERT(f)
#define ASSERTSZ(f, sz)
#endif	// !DEBUG

#endif