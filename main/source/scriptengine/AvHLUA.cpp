extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#include "scriptengine/AvHLUA.h"
#include "scriptengine/AvHLUAUtil.h"
#include <string>

#ifdef AVH_CLIENT
extern double gClientTimeLastUpdate;
#endif

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AvHLUA::AvHLUA()
{
	this->mGlobalContext = NULL;
	this->mLoaded = false;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AvHLUA::~AvHLUA()
{
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AvHLUA::Init()
{
	this->mTimetable.clear();
	bool isRestricted = false;

#ifdef AVH_SERVER
	this->mNextCallTime = gpGlobals->time + 99999;
	isRestricted = GetGameRules()->GetIsCombatMode() || GetGameRules()->GetIsNSMode();
#else
	this->mNextCallTime = gClientTimeLastUpdate + 99999;
	isRestricted = gHUD.GetIsCombatMode() || gHUD.GetIsNSMode();
#endif

	if (this->mGlobalContext != NULL)
	{
		lua_close(this->mGlobalContext);
	}
	this->mGlobalContext = lua_open();

	// load libs for context -- give full access to servers, limited to clients
#ifdef AVH_SERVER
	luaopen_io(this->mGlobalContext);
#endif
	luaopen_base(this->mGlobalContext);
	luaopen_table(this->mGlobalContext);
	luaopen_string(this->mGlobalContext);
	luaopen_math(this->mGlobalContext);

	// SERVER and CLIENT replacements
	lua_register(this->mGlobalContext, "print", AvHLUABase_Print);


#ifdef AVH_CLIENT
	// CLIENT replacements
	//lua_register(this->mGlobalContext, "assert", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "collectgarbage", AvHLUABase_Blank);
	lua_register(this->mGlobalContext, "dofile", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "error", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "_G", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "getfenv", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "getmetatable", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "gcinfo", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "ipairs", AvHLUABase_Blank);
	lua_register(this->mGlobalContext, "loadfile", AvHLUABase_Blank);
	lua_register(this->mGlobalContext, "loadlib", AvHLUABase_Blank);
	lua_register(this->mGlobalContext, "loadstring", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "next", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "pairs", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "pcall", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "rawequal", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "rawget", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "rawset", AvHLUABase_Blank);
	lua_register(this->mGlobalContext, "require", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "setfenv", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "setmetatable", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "tonumber", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "tostring", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "type", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "unpack", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "_VERSION", AvHLUABase_Blank);
	//lua_register(this->mGlobalContext, "xpcall", AvHLUABase_Blank);
#endif

	this->RegisterNamespace_Time();

#ifdef AVH_SERVER
	// SERVER API: common
	this->RegisterNamespace_Player();
	this->RegisterNamespace_Team();

	lua_register(this->mGlobalContext, "client", AvHLUABase_Client);

	// SERVER API: non-restricted 
	if (!isRestricted) 
	{

	}
#endif

#ifdef AVH_CLIENT
	// CLIENT API: common

	lua_register(this->mGlobalContext, "client", AvHLUABase_Blank);

	// CLIENT API: non-restricted
	if (!isRestricted)
	{

	}
#endif

}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
bool AvHLUA::LoadLUAForMap(const char *inMapName)
{
	ASSERT(this->mGlobalContext != NULL);
	std::string filename;
		filename = getModDirectory();
		filename += "/maps/";
		filename += inMapName;
		filename += ".lua";
	std::string msg;
	bool result = false;
    
	if (luaL_loadfile(this->mGlobalContext, filename.c_str()) || lua_pcall(this->mGlobalContext, 0, 0, 0))
	{
		AvHLUA_OnError(lua_tostring(this->mGlobalContext, -1));
	} 
	else
	{

#ifdef AVH_SERVER
		msg = "[LUA] ";
#else
		msg = "[LUA CLIENT] ";
#endif
		msg += LUA_VERSION;
		msg += "\n[LUA] ";
		msg += filename;
		msg += " is loaded\n";

		this->mLoaded = true;
		result = true;
	}
#ifdef AVH_SERVER
	ALERT(at_console, UTIL_VarArgs("%s", msg.c_str()));
#else
	gEngfuncs.pfnConsolePrint(msg.c_str());
#endif
	return result;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AvHLUA *gLUA = new AvHLUA();