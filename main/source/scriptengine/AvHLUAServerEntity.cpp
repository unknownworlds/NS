// ---------------------------------------------------------------------
// SERVER ENTITY
extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include "scriptengine/AvHLUA.h"
#include "scriptengine/AvHLUAUtil.h"

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int AvHLUAEntity_IsPlayer(lua_State *L)
{
	int entindex = luaL_checkint(L, 1);
	CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(entindex));
	// TODO: Errorcheck- and handling
	if (theEntity)
		lua_pushboolean(L, theEntity->IsPlayer());
	return 1;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int AvHLUAEntity_GetPlayer(lua_State *L)
{
	int entindex = luaL_checkint(L, 1);
	// TODO: Optimization based on entindex?
	CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(entindex));
	if (theEntity && theEntity->IsPlayer())
		lua_pushnumber(L, entindex);
	else
		// TODO: Error
		lua_pushnumber(L, 0);
	return 1;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static int AvHLUAEntity_Name(lua_State *L)
{
	int entindex = luaL_checkint(L, 1);
	CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(entindex));
	// TODO: Errorcheck and handling
	if (theEntity && theEntity->pev->netname)
		lua_pushstring(L, STRING(theEntity->pev->netname));
	else
		lua_pushstring(L, "unnamed");
	return 1;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
static const luaL_reg meta_methods [] = {
	{0,0}
};

static const luaL_reg player_methods [] = {
	{"IsPlayer", AvHLUAEntity_IsPlayer},
	{"GetPlayer", AvHLUAEntity_GetPlayer},
	{"Name", AvHLUAEntity_Name},
	{0,0}
};


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AvHLUA::RegisterNamespace_Player()
{
	UTIL_LUARegisterObject(this->mGlobalContext, player_methods, meta_methods, "Player");
}