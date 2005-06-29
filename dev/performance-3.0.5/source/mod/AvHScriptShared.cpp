//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHScriptShared.cpp $
// $Date: 2002/07/24 18:45:43 $
//
//-------------------------------------------------------------------------------
// $Log: AvHScriptShared.cpp,v $
// Revision 1.2  2002/07/24 18:45:43  Flayra
// - Linux and scripting changes
//
// Revision 1.1  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "util/nowarnings.h"
#include "mod/AvHScriptManager.h"
#include "util/Checksum.h"
#include "util/STLUtil.h"

#ifdef AVH_SERVER
#include "extdll.h"
#include "dlls/util.h"
#include "cbase.h"
#endif

#ifdef AVH_CLIENT
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#endif

#include "mod/AvHSharedUtil.h"

extern AvHScriptInstance* gRunningScript;

static int isnil(lua_State* inState)
{
	int theIsNil = lua_isnil(inState, 1);
	lua_pushnumber(inState, theIsNil);
	return 1;
}

// string functionName, float time from now
static int setCallback(lua_State* inState)
{
	// Must be set by ScriptInstance::Run()
	ASSERT(gRunningScript);
	
	const char* inFunctionName = lua_tostring(inState, 1);
	ASSERT(inFunctionName);
	
	float theCurrentTime = 0;
	
#ifdef AVH_SERVER
	theCurrentTime = gpGlobals->time;
#else
	theCurrentTime = gEngfuncs.GetClientTime();
#endif
	
	
	float inTime = theCurrentTime + lua_tonumber(inState, 2);

	string theFunctionNameString(inFunctionName);
	gRunningScript->AddCallback(theFunctionNameString, inTime);
	
	return 0;
}

// returns world time
static int getTime(lua_State* inState)
{
	float theTime = AvHSHUGetTime();
	lua_pushnumber(inState, theTime);
	
	return 1;
}

void AvHScriptInstance::InitShared()
{
	lua_register(this->mState, "isnil", isnil);
	lua_register(this->mState, "setCallback", setCallback);
	lua_register(this->mState, "getTime", getTime);
}

