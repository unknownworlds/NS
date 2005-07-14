extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include "scriptengine/AvHLUA.h"
#include "scriptengine/AvHLUAUtil.h"

#ifdef AVH_CLIENT
extern double gClientTimeLastUpdate;
#endif

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int AvHLUATime_Now(lua_State *L)
{
#ifdef AVH_SERVER
	float now = gpGlobals->time;
#else
	float now = gClientTimeLastUpdate;
#endif

	lua_pushnumber(L, (float)(now));
	return 1;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int AvHLUATime_Sleep(lua_State *L)
{
	float sleeptime = (float)(luaL_checknumber(L, 1));
	if (sleeptime > 0)
	{
		return gLUA->Suspend(L, sleeptime);
	}
	else
	{
		return gLUA->Suspend(L, 0);
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int AvHLUATime_At(lua_State *L)
{
	float time = (float)(luaL_checknumber(L, 1));
	luaL_checktype(L, 2, LUA_TSTRING);

	gLUA->DelayedExecute(L, time);
	return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static const luaL_reg meta_methods [] = {
	{0,0}
};

static const luaL_reg time_methods [] = {
	{"Now", AvHLUATime_Now},
	{"Sleep", AvHLUATime_Sleep},
	{"At", AvHLUATime_At},
	{0,0}
};

void AvHLUA::RegisterNamespace_Time()
{
	UTIL_LUARegisterObject(this->mGlobalContext, time_methods, meta_methods, "Time");
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int AvHLUA::DelayedExecute(lua_State *L, float time)
{
	int args = lua_gettop(L) - 2;
	lua_State *threadContext = lua_newthread(L);
	lua_getglobal(threadContext, lua_tostring(L, 2));

	if (args > 0)
	{
		lua_xmove(L, threadContext, args + 1);
		lua_pop(threadContext, 1);
	}

	this->mTimetable.insert(pair<float, LUATimetableCallbackType>((float)(time), LUATimetableCallbackType(args, threadContext)));
	
	LUATimetableType::iterator firstCall = this->mTimetable.begin();
	this->mNextCallTime = (float)((*firstCall).first);

	return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int AvHLUA::Suspend(lua_State *L, float delay)
{
#ifdef AVH_SERVER
	float now = gpGlobals->time;
#else
	float now = gClientTimeLastUpdate;
#endif
	this->mTimetable.insert(pair<float, LUATimetableCallbackType>((float)(now + delay), LUATimetableCallbackType(0, L)));
	
	LUATimetableType::iterator firstCall = this->mTimetable.begin();
	this->mNextCallTime = (float)((*firstCall).first);

	return (lua_yield(L, 0));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int AvHLUA::Resume(lua_State *L, int numargs)
{
	int errorcode = lua_resume(L, numargs);
	if (errorcode)
	{
		AvHLUA_OnError(lua_tostring(L, -1));
	}	
	return errorcode;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool AvHLUA::ExecuteCallbacks(float time)
{
	if (time < this->mNextCallTime)
		return false;

	LUATimetableType::iterator TimetableStart = this->mTimetable.begin();
	LUATimetableType::iterator TimetableEnd = this->mTimetable.end();

	list<LUATimetableType::iterator> eraseList;

	LUATimetableType::iterator current = TimetableStart;
	while (current != TimetableEnd)
	{
		float calltime = (float)((*current).first);
        if (calltime < time)
		{
			LUATimetableCallbackType callbackfunc = (*current).second;
			int numargs = callbackfunc.first;
			lua_State *L = callbackfunc.second;
			this->Resume(L, numargs);

			eraseList.push_back(current);

			current++;
		}
		else
		{
			this->mNextCallTime = calltime;
			break;
		}
	}
	if (current == TimetableEnd)
	{
#ifdef AVH_SERVER
		this->mNextCallTime = gpGlobals->time + 99999;
#else
		this->mNextCallTime = gClientTimeLastUpdate + 99999;
#endif
	}

	while (eraseList.size() > 0)
	{
		LUATimetableType::iterator toErase = eraseList.front();
		this->mTimetable.erase(toErase);
		eraseList.pop_front();
	}

	return true;
}