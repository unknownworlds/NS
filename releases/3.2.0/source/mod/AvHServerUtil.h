//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHServerUtil.h $
// $Date: 2002/11/22 21:25:46 $
//
//-------------------------------------------------------------------------------
// $Log: AvHServerUtil.h,v $
// Revision 1.31  2002/11/22 21:25:46  Flayra
// - Adminmod fixes
//
// Revision 1.30  2002/11/15 04:42:13  Flayra
// - Moved utility function into here from client.cpp
//
// Revision 1.29  2002/11/12 22:39:25  Flayra
// - Logging changes for Psychostats compatibility
//
// Revision 1.28  2002/11/12 02:28:53  Flayra
// - Don't reset adminmod_ entities
//
// Revision 1.27  2002/10/24 21:42:12  Flayra
// - Utility function for telefragging
// - Hive technology fixes
//
// Revision 1.26  2002/10/20 21:11:34  Flayra
// - Optimizations
//
// Revision 1.25  2002/10/19 20:19:26  Flayra
// - Debugging info for linux
//
// Revision 1.24  2002/10/16 01:06:33  Flayra
// - Added generic particle event
// - Visibility tweak: enemies aren't "detected" unless they're moving.  This means the commander can only see nearby moving blips, and marines with motion-tracking won't see still aliens that are detected because they're nearby.  This also means webs won't show up when nearby.
// - Distress beacon event
//
// Revision 1.23  2002/10/03 19:06:50  Flayra
// - Profiling info for Linux build
//
// Revision 1.22  2002/07/25 16:58:00  flayra
// - Linux changes
//
// Revision 1.21  2002/07/24 19:44:05  Flayra
// - Linux issues
//
// Revision 1.20  2002/07/23 17:24:27  Flayra
// - Added random building angles for diversity, added research started hooks for distress beacon effects, added versatile alien tech tree, hives are always visible, observatories detect nearby aliens, umbra blocks most but not all bullets
//
// Revision 1.19  2002/07/08 17:16:43  Flayra
// - Tried to cut down on sound list spam by defaulting to CHAN_BODY, added debugging code for tracking down solidity issues
//
// Revision 1.18  2002/07/01 21:44:58  Flayra
// - Added primal scream and umbra support
//
// Revision 1.17  2002/06/25 18:16:57  Flayra
// - Quieted construction effects (normalized now), temporarily removed sensory chamber sight, added upgrading of armory, wrapped bullet tracing for umbra
//
// Revision 1.16  2002/06/03 16:57:44  Flayra
// - Toned down carapace and marine upgrades, removed redundant hive class name, all buildables are subject to visibility rules
//
// Revision 1.15  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_SERVERUTIL_H
#define AVH_SERVERUTIL_H

#include "util/nowarnings.h"
#include "dlls/extdll.h"
#include "dlls/weapons.h"
#include "mod/AvHMessage.h"
#include "dlls/util.h"
#include "mod/AvHEntities.h"
#include "mod/AvHHive.h"
#include "util/STLUtil.h"

class AvHPlayer;

unsigned int AvHSUTimeGetTime();
void AvHSUPrintDevMessage(const string& inMessage, bool inForce = false);

// Pass a string
#ifdef PROFILE_BUILD
const int kMaxNestLevel = 256;
static int kIndentLevel = 0;
static unsigned long kProfileStartTime[kMaxNestLevel];
#endif

#ifdef PROFILE_BUILD
#define PROFILE_START() \
	kProfileStartTime[kIndentLevel++] = AvHSUTimeGetTime(); \
    ASSERT(kIndentLevel > 0); \
    ASSERT(kIndentLevel < kMaxNestLevel);
#else
#define PROFILE_START()
#endif

#ifdef PROFILE_BUILD
#define PROFILE_END(theNonStringProfileName) \
{ \
    ASSERT(kIndentLevel > 0); \
	ASSERT(kIndentLevel <= kMaxNestLevel); \
    int theTime = AvHSUTimeGetTime() - kProfileStartTime[--kIndentLevel]; \
    if(theTime > 0) \
    { \
        char theIndentLevel[kMaxNestLevel+1]; \
        memset(theIndentLevel, '\t', kMaxNestLevel); \
        theIndentLevel[kIndentLevel + 1] = '\0'; \
        char theMessage[kMaxNestLevel + 256]; \
	    sprintf(theMessage, "%s Profile result (%s): %d (ms)\n", theIndentLevel, #theNonStringProfileName, theTime); \
	    ALERT(at_logged, "%s", theMessage); \
    } \
}
#else
#define PROFILE_END(s)
#endif

#ifdef PROFILE_BUILD
extern int kProfileRunConfig;
#define GET_RUN_CODE(s) (s & kProfileRunConfig)
#else
#define GET_RUN_CODE(s) true
#endif

Vector AvHSUGetRandomBuildingAngles();
char* AvHSUGetGameVersionString();
bool AvHSUGetIsRelevantForTopDownPlayer(const vec3_t& inTopDownPosition, const vec3_t& inEntityPosition, float inScalar = 1.0f);
const char* AvHSUGetTeamName(int inTeamNumber);
#ifdef USE_OLDAUTH
bool AvHSUGetIsValidAuthID(const string& inAuthID);
string AvHSUGetPlayerAuthIDString(edict_t* inPlayer);
#endif

void AvHSUKillPlayersTouchingPlayer(AvHPlayer* inPlayer, entvars_t* inInflictor);
void AvHSUKillBuildablesTouchingPlayer(AvHPlayer* inPlayer, entvars_t* inInflictor);
void AvHSUBuildingJustCreated(AvHMessageID inBuildID, CBaseEntity* theBuilding, AvHPlayer* inPlayer);

CBaseEntity* AvHSUBuildTechForPlayer(AvHMessageID inBuildID, const Vector& inLocation, AvHPlayer* inPlayer);

// Takes armor into account
//float AvHSUCalculateDamageLessArmor(entvars_t *inVictim, float flDamage, int bitsDamageType, BOOL inIsMultiplayer);
int AvHSUCalcCombatSpawnWaveSize(int inNumPlayersOnTeam, int inNumDeadPlayers);
float AvHSUCalcCombatSpawnTime(AvHClassType inClassType, int inNumPlayersOnTeam, int inNumDeadPlayersOnTeam, int thePlayersSpentLevel);
void AvHSUExplodeEntity(CBaseEntity* inEntity, Materials inMaterial);

Vector AvHSUEyeToBodyVector(entvars_t* inEye, CBaseEntity* inTarget);
float AvHSUEyeToBodyDistance(entvars_t* inEye, CBaseEntity* inTarget);
float AvHSUEyeToBodyXYDistance(entvars_t* inEye, CBaseEntity* inTarget);

bool AvHSSUGetIsClassNameFadeable(const char* inClassName);

void AvHSUPlayPhaseInEffect(int inFlags, CBaseEntity* inStartEntity, CBaseEntity* inEndEntity);
void AvHSUPlayRandomConstructionEffect(AvHPlayer* inPlayer, CBaseEntity* inConstructee);
bool AvHSUPlayerCanBuild(entvars_t* inPev);
bool AvHSUPlayParticleEvent(const char* inParticleSystemName, const edict_t* inEdict, const Vector& inOrigin, int inEventFlags = 0);
void AvHSUPlayNumericEvent(float inNumber, const edict_t* inEdict, Vector& inOrigin, int inEventFlags, int inNumericEventType, int inTeamNumber);
void AvHSUPlayNumericEventAboveStructure(float inNumber, AvHBaseBuildable* inBuildable, int inNumericEventType = kNumericalInfoResourcesEvent);

void AvHSURemoveAllEntities(const char* inClassName);
void AvHSUResupplyFriendliesInRange(int inNumEntitiesToCreate, AvHPlayer* inPlayer, int inRange = -1);
bool AvHSUGetIsOftRepeatedAlert(AvHAlertType inAlertType);
bool AvHSUGetIsUrgentAlert(AvHAlertType inAlertType);
bool AvHSUGetIsExternalClassName(const char* inClassName);
bool AvHSUGetIsSubjectToVisibilityRules(CBaseEntity* inEntity);

int AvHSUGetNumHumansInGame(void);

AvHHive* AvHSUGetRandomActiveHive(AvHTeamNumber inTeam);
AvHHive* AvHSUGetRandomActivateableHive(AvHTeamNumber inTeam);
int AvHSUGetWeaponStayTime();

void AvHSUResearchStarted(CBaseEntity* inResearchEntity, AvHMessageID inResearchingTech);
void AvHSUResearchComplete(CBaseEntity* inResearchEntity, AvHMessageID inResearchingTech);

CBaseEntity* AvHSUGetEntityFromIndex(int inEntityIndex);
void AvHSUSetCollisionBoxFromSequence(entvars_t* inPev);

CGrenade* AvHSUShootServerGrenade(entvars_t* inOwner, Vector inOrigin, Vector inVelocity, float inTime, bool inHandGrenade = true);
void AvHSUKnockPlayerAbout(CBaseEntity* inAttcker, CBaseEntity* inVictim, int inForce);

void AvHSUServerTraceBullets(const Vector& inStart, const Vector& inEnd, IGNORE_MONSTERS inIgnoreMonsters, edict_t* inIgnoreEdict, TraceResult& outTraceResult, bool& outProtected);

void AvHSUUpdateHiveTechology(AvHTeamNumber inTeamNumber, AvHMessageID inBuildID);

bool AvHSUGetIsDebugging();
void AvHSUSetIsDebugging(bool inState);
void AvHSUAddDebugPoint(float inX, float inY, float inZ);

int AvHSUGetValveHull(int inHull);
AvHUser3 AvHSUGetGroupTypeFromSelection(const EntityListType& inEntityListType);
void AvHSURemoveEntityFromHotgroupsAndSelection(int inEntityIndex);

string AvHSUGetLocationText(CBaseEntity* inEntity);
void AvHSUCreateUmbraCloud(vec3_t inOrigin, AvHTeamNumber inTeamNumber, CBaseEntity* inCreatingEntity);
void AvHSUExplosiveForce(const Vector& inOrigin, int inRadius, float inForceScalar, const CBaseEntity* inAttacker, const CBaseEntity* inIgnorePlayer = NULL);
bool AvHSUGetIsEnoughRoomForHull(const vec3_t& inCenter, int inHull, edict_t* inIgnoreEntity, bool inIgnorePlayers = false, bool inIgnoreEntities = false);

bool AvHCheckLineOfSight(const Vector& vecStart, const Vector& vecEnd, edict_t* pentIgnore, IGNORE_MONSTERS igmon = ignore_monsters, edict_t* pEntTarget = NULL);

bool AvHSUGetIsResearchApplicable(CBaseEntity* inResearchEntity, AvHMessageID inResearchingTech);

// Use theEntity to call stuff
#define FOR_ALL_ENTITIES(inEntityClassName, inBaseClassPointer) \
{\
	edict_t* pent = FIND_ENTITY_BY_CLASSNAME(NULL, inEntityClassName); \
	while (!FNullEnt(pent)) \
	{ \
		CBaseEntity* theBaseEntity = CBaseEntity::Instance(pent); \
		if(theBaseEntity) \
		{ \
			const char* theClassName = STRING(theBaseEntity->pev->classname); \
			if(!AvHSUGetIsExternalClassName(theClassName)) \
			{ \
				inBaseClassPointer theEntity = dynamic_cast<inBaseClassPointer>(theBaseEntity); \
				if(theEntity && theEntity->pev) \
				{ \
	
#define END_FOR_ALL_ENTITIES(inEntityClassName) \
				} \
			}\
		}\
		pent = FIND_ENTITY_BY_CLASSNAME(pent, inEntityClassName); \
	} \
}

#define FOR_ALL_BASEENTITIES() \
{ \
	for(int theBaseEntityLoopIndex = 0; theBaseEntityLoopIndex < gpGlobals->maxEntities; theBaseEntityLoopIndex++) \
	{ \
		edict_t* theBaseEntityEdict = g_engfuncs.pfnPEntityOfEntIndex(theBaseEntityLoopIndex); \
		if(theBaseEntityEdict != NULL) \
		{ \
			CBaseEntity* theBaseEntity = CBaseEntity::Instance(theBaseEntityEdict); \
			if(theBaseEntity && theBaseEntity->pev) \
			{ \
				const char* theClassName = STRING(theBaseEntity->pev->classname); \
				if(!AvHSUGetIsExternalClassName(theClassName)) \
				{ \

#define END_FOR_ALL_BASEENTITIES() \
				} \
			} \
		} \
	} \
}

template <class T>
bool AvHSUGetEntityFromIndex(int inEntityIndex, T& outValue)
{
	bool theSuccess = false;
	
	CBaseEntity* theEntity = AvHSUGetEntityFromIndex(inEntityIndex);
	if(theEntity)
	{
		outValue = dynamic_cast<T>(theEntity);
		if(outValue)
		{
			theSuccess = true;
		}
	}
	
	return theSuccess;
}

class CachedEntity
{
public:
	CachedEntity()
	{
		this->ResetCachedEntity();
	}
	
	void ResetCachedEntity()
	{
		mVisible = 0;
		mLastTimeVisible = -1;
		mServerTick = 0;
		mNeedsVisibilityCheck = false;
	}
	
	int			mVisible;
	float		mLastTimeVisible;
	int			mServerTick;
	bool		mNeedsVisibilityCheck;
};

class CachedVisibility
{
public:
	CachedVisibility()
	{
		this->ResetCachedEntity();
	}
	
	void ResetCachedEntity()
	{
		mPVS = mPAS = NULL;
		mComputed = false;
	}
	
	unsigned char*	mPVS;
	unsigned char*	mPAS;
	bool			mComputed;
};

#endif
