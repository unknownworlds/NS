extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include "scriptengine/AvHLUAUtil.h"
#include "scriptengine/AvHLUA.h"
extern AvHLUA *gLUA;

/*
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UTIL_LUATablePair(char *key, const char *value)
{
	lua_pushstring(gLUA->mGlobalContext, key);
	lua_pushstring(gLUA->mGlobalContext, value);
	lua_settable(gLUA->mGlobalContext, -3);
}

void UTIL_LUATablePair(char *key, lua_Number value)
{
	lua_pushstring(gLUA->mGlobalContext, key);
	lua_pushnumber(gLUA->mGlobalContext, value);
	lua_settable(gLUA->mGlobalContext, -3);
}

void UTIL_LUATablePair(char *key, bool value)
{
	lua_pushstring(gLUA->mGlobalContext, key);
	lua_pushboolean(gLUA->mGlobalContext, value);
	lua_settable(gLUA->mGlobalContext, -3);
}

void UTIL_LUATablePair(char *key, lua_CFunction function)
{
	lua_pushstring(gLUA->mGlobalContext, key);
	lua_pushcfunction(gLUA->mGlobalContext, function);
	lua_settable(gLUA->mGlobalContext, -3);
}
*/

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void UTIL_LUARegisterObject(lua_State *L, const luaL_reg *mainmethods, const luaL_reg *metamethods, char *objName)
{
	lua_pushstring(L, objName);
	int methods = newtable(L);
	int metatable = newtable(L);
	lua_pushliteral(L, "__index");
	lua_pushvalue(L, methods);
	lua_settable(L, metatable);
	lua_pushliteral(L, "__metatable");
	lua_pushvalue(L, methods);
	lua_settable(L, metatable);
	luaL_openlib(L, 0, metamethods,  0);
	luaL_openlib(L, 0, mainmethods, 1);
	lua_settable(L, LUA_GLOBALSINDEX);
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AvHLUA_OnError(const char *errorMsg)
{
	std::string msg;
#ifdef AVH_CLIENT
	msg = "[LUA CLIENT] Error: ";
	msg += errorMsg;
	msg += "\n";
	gEngfuncs.pfnConsolePrint(msg.c_str());
#else
	msg += "[LUA] Error: ";
	msg += errorMsg;
	msg += "\n";
	ALERT(at_console, UTIL_VarArgs("%s", msg.c_str()));
#endif
}
