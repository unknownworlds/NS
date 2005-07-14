// ---------------------------------------------------------------------
// SERVER EVENTS
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include "scriptengine/AvHLUA.h"
#include "scriptengine/AvHLUAUtil.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AvHLUA::OnLoad()
{
	if (this->mLoaded)
	{
		lua_getglobal(this->mGlobalContext, "OnLoad");
		AvHLUA_Pcall(this->mGlobalContext, 0, 0);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AvHLUA::OnStart()
{
	if (this->mLoaded)
	{
		lua_getglobal(this->mGlobalContext, "OnStart");
		AvHLUA_Pcall(this->mGlobalContext, 0, 0);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AvHLUA::OnStarted()
{
	if (this->mLoaded)
	{
		lua_getglobal(this->mGlobalContext, "OnStarted");
		AvHLUA_Pcall(this->mGlobalContext, 0, 0);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AvHLUA::OnJointeam(int inEntindex, AvHTeamNumber inTeamToJoin)
{
	if (this->mLoaded)
	{
		lua_State *threadState = lua_newthread(this->mGlobalContext);
		lua_getglobal(threadState, "OnJointeam");
		lua_pushnumber(threadState, inEntindex);
		lua_pushnumber(threadState, (int)(inTeamToJoin));
		if (int errorcode = lua_resume(threadState, 2))
		{
			AvHLUA_OnError(lua_tostring(threadState, -1));
		}
		else
		{
			// all values returned are on the stack
		}
	}
}
// ---------------------------------------------------------------------