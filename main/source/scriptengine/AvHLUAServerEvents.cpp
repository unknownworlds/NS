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
	if (this->mLoaded && this->definedOnLoad)
	{
		lua_State *threadState = lua_newthread(this->mGlobalContext);
		lua_getglobal(threadState, "OnLoad");
		if (!lua_isfunction(threadState, -1))
		{
			// not found, mark and exit
			this->definedOnLoad = false;
			return;
		}

		if (int errorcode = lua_resume(threadState, 2))
			AvHLUA_OnError(lua_tostring(threadState, -1));
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool AvHLUA::OnStartCheck()
{
	if (this->mLoaded && this->definedOnStartCheck)
	{
		lua_State *threadState = lua_newthread(this->mGlobalContext);
		lua_getglobal(threadState, "OnStartCheck");
		if (!lua_isfunction(threadState, -1))
		{
			// not found, mark and exit
			this->definedOnStartCheck = false;
			return false;
		}
		
		if (int errorcode = lua_resume(threadState, 2))
			AvHLUA_OnError(lua_tostring(threadState, -1));

		// Return the team that won
		if (lua_isnumber(threadState, -1))
            return (bool)(lua_toboolean(threadState, -1));
	}
	return false;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AvHLUA::OnStart()
{
	if (this->mLoaded && this->definedOnStart)
	{
		lua_State *threadState = lua_newthread(this->mGlobalContext);
		lua_getglobal(threadState, "OnStart");
		if (!lua_isfunction(threadState, -1))
		{
			// not found, mark and exit
			this->definedOnStart = false;
			return;
		}

		if (int errorcode = lua_resume(threadState, 2))
			AvHLUA_OnError(lua_tostring(threadState, -1));
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AvHLUA::OnStarted()
{
	if (this->mLoaded && this->definedOnStarted)
	{
		lua_State *threadState = lua_newthread(this->mGlobalContext);
		lua_getglobal(threadState, "OnStarted");
		if (!lua_isfunction(threadState, -1))
		{
			// not found, mark and exit
			this->definedOnStarted = false;
			return;
		}

		if (int errorcode = lua_resume(threadState, 2))
			AvHLUA_OnError(lua_tostring(threadState, -1));
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AvHLUA::OnVictory(AvHTeamNumber inTeamNumber)
{
	if (this->mLoaded && this->definedOnVictory)
	{
		lua_State *threadState = lua_newthread(this->mGlobalContext);
		lua_getglobal(threadState, "OnVictory");
		if (!lua_isfunction(threadState, -1))
		{
			// not found, mark and exit
			this->definedOnVictory = false;
			return;
		}

		lua_pushnumber(threadState, inTeamNumber);
		if (int errorcode = lua_resume(threadState, 2))
			AvHLUA_OnError(lua_tostring(threadState, -1));
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AvHTeamNumber AvHLUA::OnVictoryCheck()
{
	if (this->mLoaded && this->definedOnVictoryCheck)
	{
		lua_State *threadState = lua_newthread(this->mGlobalContext);
		lua_getglobal(threadState, "OnVictoryCheck");
		if (!lua_isfunction(threadState, -1))
		{
			// not found, mark and exit
			this->definedOnJointeam = false;
			return TEAM_IND;
		}

		if (int errorcode = lua_resume(threadState, 2))
			AvHLUA_OnError(lua_tostring(threadState, -1));

		// Return the team that won
		if (lua_isnumber(threadState, -1))
            return (AvHTeamNumber)((int)lua_tonumber(threadState, -1));

	}
	return TEAM_IND;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AvHLUA::OnJointeam(int inEntindex, AvHTeamNumber inTeamToJoin)
{
	if (this->mLoaded && this->definedOnJointeam)
	{
		lua_State *threadState = lua_newthread(this->mGlobalContext);
		lua_getglobal(threadState, "OnJointeam");
		if (!lua_isfunction(threadState, -1))
		{
			// not found, mark and exit
			this->definedOnJointeam = false;
			return;
		}

		lua_pushnumber(threadState, inEntindex);
		lua_pushnumber(threadState, (int)(inTeamToJoin));
		if (int errorcode = lua_resume(threadState, 2))
			AvHLUA_OnError(lua_tostring(threadState, -1));

		// all values returned are on the stack if no error
	}
}
// ---------------------------------------------------------------------