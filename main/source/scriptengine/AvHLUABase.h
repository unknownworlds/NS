#ifndef AVHLUABASE_H
#define AVHLUABASE_H

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

int AvHLUABase_Blank(lua_State *L);
int AvHLUABase_Print(lua_State *L);
int AvHLUABase_Client(lua_State *L);

#endif