extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include "scriptengine/AvHLUA.h"
#include "scriptengine/AvHLUAUtil.h"


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// replacement: blank
int AvHLUABase_Blank(lua_State *L)
{
	return 0;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// replacement: print
int AvHLUABase_Print(lua_State *L)
{
	if (lua_gettop(L) > 0)
	{
		std::string theConsoleString;

#ifdef AVH_CLIENT
		theConsoleString = "[LUA CLIENT] ";
		theConsoleString += lua_tostring(L, 1);
		theConsoleString += "\n";
		gEngfuncs.pfnConsolePrint(theConsoleString.c_str());
#else
		theConsoleString = "[LUA] ";
		theConsoleString += lua_tostring(L, 1);
		theConsoleString += "\n";
		ALERT(at_console, UTIL_VarArgs("%s", theConsoleString.c_str()));
#endif

	}
	return 0;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
int AvHLUABase_Client(lua_State *L)
{
#ifdef AVH_SERVER
	luaL_checktype(L, 1, LUA_TNUMBER);
	CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(lua_tonumber(L, 1)));

	if (theEntity && theEntity->IsPlayer())
		NetMsg_LUAMessage(theEntity->pev, L);
	else
		int a = 0; // TODO: ADD ERROR CHECK

	return 0;
#endif
#ifdef AVH_CLIENT
	return 0;
#endif
}