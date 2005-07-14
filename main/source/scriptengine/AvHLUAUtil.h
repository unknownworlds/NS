#ifndef AVHLUAUTIL_H
#define AVHLUAUTIL_H

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

#define newtable(L) (lua_newtable(L), lua_gettop(L))

/*
void UTIL_LUATablePair(char *key, const char *value);
void UTIL_LUATablePair(char *key, lua_Number value);
void UTIL_LUATablePair(char *key, bool value);
void UTIL_LUATablePair(char *key, lua_CFunction function);
*/

void UTIL_LUARegisterObject(lua_State *L, const luaL_reg *mainmethods, const luaL_reg *metamethods, char *objName);

void AvHLUA_OnError(const char *errorMsg);
#define AvHLUA_Pcall(L, a, b) \
	if (lua_pcall(L, a, b, 0)) \
	{ \
		AvHLUA_OnError(lua_tostring(L, -1)); \
	}


#endif