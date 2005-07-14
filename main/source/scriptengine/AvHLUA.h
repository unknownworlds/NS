#ifndef AVHLUA_H
#define AVHLUA_H

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}
#ifdef AVH_CLIENT
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#include "engine/progdefs.h"
#include "mod/AvHHud.h"
#include "common/cl_entity.h"
extern cl_enginefunc_t gEngfuncs;
extern AvHHud gHUD;
#endif

#ifdef AVH_SERVER
#include "util/nowarnings.h"
#include "mod/AvHPlayer.h"
#include "mod/AvHMessage.h"
#include "mod/AvHParticleTemplateServer.h"
#include "mod/AvHEntities.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHServerVariables.h"
#include "mod/AvHConstants.h"
#include "mod/AvHMarineWeapons.h"
#include "dlls/client.h"
#include "dlls/util.h"
#include "mod/AvHSoundListManager.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHTitles.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHParticleTemplate.h"
#include "common/vector_util.h"
#include "dlls/roach.h"
#include "mod/AvHSelectionHelper.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHDramaticPriority.h"
#include "mod/AvHHulls.h"
#include "mod/AvHMovementUtil.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHParticleSystemEntity.h"
#include "mod/AvHAlienAbilities.h"
#include "mod/AvHAlienAbilityConstants.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHMarineTurret.h"
#include "mod/AvHSiegeTurret.h"
#include "mod/AvHBlipConstants.h"
#include "mod/AvHParticleConstants.h"
#include "util/MathUtil.h"
#include "types.h"
#include "mod/AvHNetworkMessages.h"
#include "mod/AvHNexusServer.h"
#include "dlls/cbase.h"
#endif

#include "mod/AvHConstants.h"
#include "engine/edict.h"
#include "engine/eiface.h"
#include "common/const.h"
#include <map>
#include <list>

#include "scriptengine/AvHLUABase.h"

typedef pair<int, lua_State*> LUATimetableCallbackType;
typedef multimap<int, LUATimetableCallbackType> LUATimetableType;

class AvHLUA
{
public:
	AvHLUA();
	~AvHLUA();

	void Init();
	bool LoadLUAForMap(const char *inMapName);

	void OnLoad();
	void OnStart();
	void OnStarted();
	void OnJointeam(int inEntindex, AvHTeamNumber inTeamToJoin);

	bool mLoaded;
	
	lua_State *mGlobalContext;

	bool ExecuteCallbacks(float time);
	int Suspend(lua_State *L, float delay);
	int DelayedExecute(lua_State *L, float time);

private:

	void RegisterNamespace_Time();
#ifdef AVH_SERVER
	void RegisterNamespace_Player();
	void RegisterNamespace_Team();
#else

#endif

	int Resume(lua_State *L, int numargs);
	float mNextCallTime;
	LUATimetableType mTimetable;

};

extern AvHLUA *gLUA;

#endif