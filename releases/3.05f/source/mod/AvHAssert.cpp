//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAssert.cpp $
// $Date: 2002/07/24 19:01:51 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAssert.cpp,v $
// Revision 1.7  2002/07/24 19:01:51  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.6  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "util/nowarnings.h"
#include <string.h>
#include <stdio.h>
#include "util/Zassert.h"

#ifdef AVH_SERVER
#include "extdll.h"
#include "dlls/util.h"
#include "cbase.h"
extern cvar_t avh_assert;
#endif

#ifdef AVH_CLIENT
#endif

#include "assert.h"

#ifdef	DEBUG
	void
DBG_AssertFunction(
	bool		fExpr,
	const char*	szExpr,
	const char*	szFile,
	int			szLine,
	const char*	szMessage)
	{
	if (fExpr)
		return;
	char szOut[512];
	if (szMessage != NULL)
		sprintf(szOut, "ASSERT FAILED: %s (%s@%d)\n%s", szExpr, szFile, szLine, szMessage);
	else
		sprintf(szOut, "ASSERT FAILED: %s (%s@%d)\n", szExpr, szFile, szLine);

	#ifdef AVH_SERVER
	ALERT(at_logged, szOut);
	
	// Ability to disable ASSERTs on server
	if(avh_assert.value == 0.0f)
	{
		UTIL_ClientPrintAll(HUD_PRINTNOTIFY, szOut); 
		return;
	}
	ALERT(at_console, szOut);
	#endif
	
	#ifdef WIN32
	_assert((void*)szExpr, (void*)szFile, szLine);
	#else
	assert(fExpr);
	#endif

	}
#endif	// DEBUG


