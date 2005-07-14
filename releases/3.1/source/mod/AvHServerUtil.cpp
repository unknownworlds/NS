//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHServerUtil.cpp $
// $Date: 2002/11/22 21:25:46 $
//
//-------------------------------------------------------------------------------
// $Log: AvHServerUtil.cpp,v $
// Revision 1.42  2002/11/22 21:25:46  Flayra
// - Adminmod fixes
//
// Revision 1.41  2002/11/15 04:42:13  Flayra
// - Moved utility function into here from client.cpp
//
// Revision 1.40  2002/11/12 22:39:25  Flayra
// - Logging changes for Psychostats compatibility
//
// Revision 1.39  2002/11/12 02:28:53  Flayra
// - Don't reset adminmod_ entities
//
// Revision 1.38  2002/11/03 04:51:55  Flayra
// - Refactoring for AddToFullPack changes
//
// Revision 1.37  2002/10/24 21:42:12  Flayra
// - Utility function for telefragging
// - Hive technology fixes
//
// Revision 1.36  2002/10/20 21:11:34  Flayra
// - Optimizations
//
// Revision 1.35  2002/10/19 21:09:56  Flayra
// - Debugging info for linux
//
// Revision 1.34  2002/10/19 20:58:31  Flayra
// - Debugging info for linux
//
// Revision 1.33  2002/10/18 22:22:19  Flayra
// - Sensory chamber triggers vocal alert
//
// Revision 1.32  2002/10/16 01:06:33  Flayra
// - Added generic particle event
// - Visibility tweak: enemies aren't "detected" unless they're moving.  This means the commander can only see nearby moving blips, and marines with motion-tracking won't see still aliens that are detected because they're nearby.  This also means webs won't show up when nearby.
// - Distress beacon event
//
// Revision 1.31  2002/10/07 17:49:23  Flayra
// - Umbra balance change
//
// Revision 1.30  2002/10/03 19:06:50  Flayra
// - Profiling info for Linux build
//
// Revision 1.29  2002/09/23 22:30:05  Flayra
// - Commander dropped weapons live forever
// - Observatory and sensory chambers check range in 2D for commander regions
// - Added turret factory upgrading (for siege)
//
// Revision 1.28  2002/09/09 20:05:59  Flayra
// - Sensory chambers now detect enemies in range
//
// Revision 1.27  2002/08/31 18:01:03  Flayra
// - Work at VALVe
//
// Revision 1.26  2002/08/16 02:44:11  Flayra
// - New damage types
//
// Revision 1.25  2002/08/09 00:52:14  Flayra
// - Removed old #ifdef
//
// Revision 1.24  2002/08/02 21:50:24  Flayra
// - Removed hives-are-visible code, I think it's safe again
//
// Revision 1.23  2002/07/25 16:58:00  flayra
// - Linux changes
//
// Revision 1.22  2002/07/23 17:24:27  Flayra
// - Added random building angles for diversity, added research started hooks for distress beacon effects, added versatile alien tech tree, hives are always visible, observatories detect nearby aliens, umbra blocks most but not all bullets
//
// Revision 1.21  2002/07/08 17:16:43  Flayra
// - Tried to cut down on sound list spam by defaulting to CHAN_BODY, added debugging code for tracking down solidity issues
//
// Revision 1.20  2002/07/01 21:44:58  Flayra
// - Added primal scream and umbra support
//
// Revision 1.19  2002/06/25 18:16:56  Flayra
// - Quieted construction effects (normalized now), temporarily removed sensory chamber sight, added upgrading of armory, wrapped bullet tracing for umbra
//
// Revision 1.18  2002/06/03 16:57:44  Flayra
// - Toned down carapace and marine upgrades, removed redundant hive class name, all buildables are subject to visibility rules
//
// Revision 1.17  2002/05/28 18:13:50  Flayra
// - Sensory chambers contribute to hive sight
//
// Revision 1.16  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "util/nowarnings.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHPlayer.h"
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "mod/AvHPlayerUpgrade.h"
#include "common/damagetypes.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHSharedUtil.h"
#include "util/MathUtil.h"
#include "engine/studio.h"
#include "mod/AvHSoundListManager.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHAlienEquipment.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHParticleTemplateServer.h"
#include "mod/AvHAlienWeapons.h"
#include "common/com_model.h"
#include "mod/AvHHulls.h"
#include "mod/AnimationUtil.h"

int   NS_PointContents(const hull_t *hull, int num, float p[3]);
float NS_TraceLineAgainstEntity(int inEntityIndex, float inTime, const float inRayOrigin[3], const float inRayDirection[3]);

extern const float AVH_INFINITY;

extern playermove_t* pmove;

#ifdef WIN32
#include "mmsystem.h"
#endif

extern int	gTeleportEventID;
extern int	gParticleEventID;
extern int  gNumericalInfoEventID;
extern int	gDistressBeaconEventID;
extern int	gUmbraCloudEventID;
extern AvHParticleTemplateListServer	gParticleTemplateList;

extern int gPhaseInEventID;
extern AvHSoundListManager				gSoundListManager;

unsigned int AvHSUTimeGetTime()
{
	unsigned int theTime = 0;
	
#ifdef WIN32
	theTime = timeGetTime();
#else
	FILE* theFilePointer;
	double theIdleTime;
	double theUpTime;
	
	/* Read the system uptime and accumulated idle time from /proc/uptime. We're disregarding theIdleTime  */
	theFilePointer = fopen ("/proc/uptime", "r");
	
	if(fscanf(theFilePointer, "%lf %lf\n", &theUpTime, &theIdleTime) == 2)
	{
		/* uptime is in seconds... we want milliseconds */
		theTime = (unsigned int)(theUpTime*1000);
	}
	
	fclose (theFilePointer);
	#endif

	return theTime;
}

int AvHSUCalcCombatSpawnWaveSize(int inNumPlayersOnTeam, int inNumDeadPlayers)
{
	int theSpawnWaveSize = min(inNumDeadPlayers, BALANCE_VAR(kCombatMaxPlayersPerWave));
	return theSpawnWaveSize;
}

float AvHSUCalcCombatSpawnTime(AvHClassType inClassType, int inNumPlayersOnTeam, int inNumDeadPlayers, int inPlayersSpentLevel)
{
	int theWaveSize = AvHSUCalcCombatSpawnWaveSize(inNumPlayersOnTeam, inNumDeadPlayers);
	float theSpawnTime = BALANCE_VAR(kCombatBaseRespawnTime) + max((theWaveSize-1), 0)*BALANCE_VAR(kCombatAdditiveRespawnTime);
    return theSpawnTime;
}

char* AvHSUGetGameVersionString()
{
    static char theGameVersion[1024];

    string theGameVersionString;
    
    theGameVersionString = "v"	+ MakeStringFromInt(BALANCE_VAR(kGameVersionMajor)) + "." + MakeStringFromInt(BALANCE_VAR(kGameVersionMinor))	+ "." + MakeStringFromInt(BALANCE_VAR(kGameVersionRevision));
    
    //memset(theGameVersion, 0, 1024);
    strcpy(theGameVersion, theGameVersionString.c_str());
    
    return theGameVersion;
}

bool AvHSUGetIsRelevantForTopDownPlayer(const vec3_t& inTopDownPosition, const vec3_t& inEntityPosition, float inScalar)
{
	bool theIsRelevant = false;
	
	//if(inEntityPosition.z <= inTopDownPosition.z)
	//{
	float theXDist = fabs(inTopDownPosition.x - inEntityPosition.x);
	float theYDist = fabs(inTopDownPosition.y - inEntityPosition.y);
	float theXYDistance = sqrt(theXDist*theXDist + theYDist*theYDist);
	
	float theCullDistance = GetGameRules()->GetMapExtents().GetTopDownCullDistance()*inScalar;
	if(theXYDistance <= theCullDistance)
	{
		theIsRelevant = true;
	}
	//}
	
	return theIsRelevant;
}

Vector AvHSUGetRandomBuildingAngles()
{
	int theX = 0;//g_engfuncs.pfnRandomLong(0, 360);
	int theY = g_engfuncs.pfnRandomLong(0, 360);
	int theZ = 0;
	
	Vector theRandomAngles(theX, theY, theZ);

	return theRandomAngles;
}

const char* AvHSUGetTeamName(int inTeamNumber)
{
	const char* theTeamName = "none";

	const AvHTeam* theTeamPointer = GetGameRules()->GetTeam(AvHTeamNumber(inTeamNumber));
	if(theTeamPointer)
	{
		theTeamName = theTeamPointer->GetTeamName();
	}

	return theTeamName;
}

#ifdef USE_OLDAUTH
// Steam IDs
const char* kSteamIDPending = "STEAM_ID_PENDING";
const char* kSteamIDLocal = "STEAM_ID_LOOPBACK";
const char* kSteamIDBot = "BOT";
const char* kSteamIDInvalidID = "-1";
const char* kSteamIDDefault = "STEAM_0:0:0";
const char* kSteamIDPrefix = "STEAM_";
bool AvHSUGetIsValidAuthID(const string& inAuthID)
{
	bool theIsValid = true;

	// "0" is WONid that hasn't been entered
	if((inAuthID == "") || (inAuthID == " ") || (inAuthID == "0") || (inAuthID == kSteamIDDefault) || (inAuthID == kSteamIDInvalidID) || (inAuthID == kSteamIDBot) || (inAuthID == kSteamIDLocal))
	{
		theIsValid = false;
	}

	return theIsValid;
}
// Function that is backwards-compatible with WON ids 
string AvHSUGetPlayerAuthIDString(edict_t* inPlayer)
{
	string thePlayerAuthID;
	
	// Try to get SteamID
	const char* theSteamID = g_engfuncs.pfnGetPlayerAuthId(inPlayer);
	if(strcmp(theSteamID, kSteamIDInvalidID))
	{
		thePlayerAuthID = theSteamID;
	}
	// If that fails, get WonID and put it into a string
	else
	{
		int theWonID = g_engfuncs.pfnGetPlayerWONId(inPlayer);
		thePlayerAuthID = MakeStringFromInt(theWonID);
	}
	
	return thePlayerAuthID;
}
#endif

void AvHSUKillPlayersTouchingPlayer(AvHPlayer* inPlayer, entvars_t* inInflictor)
{
	// If new player is stuck inside another player, kill old player
	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
	if((theEntity != inPlayer) && (theEntity->GetIsRelevant()))
	{
		// tankefugl: 0000892 -- fixed to allow spawnkilling of crouching players on IP
		float theDistanceToPlayer = VectorDistance(inPlayer->pev->origin, theEntity->pev->origin);
		float zDistance = inPlayer->pev->origin[2] - theEntity->pev->origin[2];
		if(theDistanceToPlayer < 30 || (theDistanceToPlayer < 40 && zDistance > 0 && zDistance < 40))
		{
			theEntity->TakeDamage(inInflictor, theEntity->pev, 10000, DMG_GENERIC);
		}
	}
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
}

void AvHSUBuildingJustCreated(AvHMessageID inBuildID, CBaseEntity* theBuilding, AvHPlayer* inPlayer)
{
	if((inBuildID == BUILD_RESOURCES) || (inBuildID == ALIEN_BUILD_RESOURCES))
	{
		// Add it to team for performance reasons (so world doesn't have to be polled during AvHTeam::UpdateResources)
		AvHTeam* theTeam = GetGameRules()->GetTeam((AvHTeamNumber)theBuilding->pev->team);
		ASSERT(theTeam);
		theTeam->AddResourceTower(theBuilding->entindex());
	}

	// Don't expire weapons the commander drops
	AvHBasePlayerWeapon* theBaseWeapon = dynamic_cast<AvHBasePlayerWeapon*>(theBuilding);
	if(theBaseWeapon && (inPlayer->GetUser3() == AVH_USER3_COMMANDER_PLAYER))
	{
		theBaseWeapon->SetGroundLifetime(-1);
	}
	
	AvHBuildable* theBuildable = dynamic_cast<AvHBuildable*>(theBuilding);
	if(theBuildable && inPlayer)
	{
		theBuildable->SetBuilder(inPlayer->entindex());
	}
	
	// If it was an alien tech building, look for a hive that doesn't support this tech and make it support it
	if((inBuildID == ALIEN_BUILD_DEFENSE_CHAMBER) || (inBuildID == ALIEN_BUILD_MOVEMENT_CHAMBER) || (inBuildID == ALIEN_BUILD_SENSORY_CHAMBER))
	{
		if(inPlayer)
		{
			AvHSUUpdateHiveTechology(inPlayer->GetTeam(), inBuildID);
		}
	}

	const char* theClassName = STRING(theBuilding->pev->classname);
	if(inPlayer && theClassName)
	{
		inPlayer->LogPlayerAction("structure_built", theClassName);
	}

	// Notify player and his teammates
	if(!GetGameRules()->GetIsCombatMode())
	{
		inPlayer->PlayHUDStructureNotification(inBuildID, theBuilding->pev->origin);
	}
}

CBaseEntity* AvHSUBuildTechForPlayer(AvHMessageID inBuildID, const Vector& inLocation, AvHPlayer* inPlayer)
{
	CBaseEntity* theEntity = NULL;

	if(!GetGameRules()->GetIsCombatMode() || AvHSHUGetIsCombatModeTech(inBuildID))
	{
		char* theClassName;

		if(AvHSHUGetBuildTechClassName(inBuildID, theClassName))
		{
			// Create without owner to fix solidity problems?
			//theEntity = CBaseEntity::Create(theClassName, inLocation, theAngles, inPlayer->edict());
			theEntity = CBaseEntity::Create(theClassName, inLocation, AvHSUGetRandomBuildingAngles());
			if(theEntity)
			{
				// Set team
				theEntity->pev->team = inPlayer->pev->team;

				// Set any team-wide upgrades
				AvHTeam* theTeam = inPlayer->GetTeamPointer();
				ASSERT(theTeam);
				theEntity->pev->iuser4 |= theTeam->GetTeamWideUpgrades();

				AvHSUBuildingJustCreated(inBuildID, theEntity, inPlayer);

				if(!theEntity->IsInWorld() && GetGameRules()->GetIsCombatMode())
				{
					//voogru: okay, if for WHATEVER reason this isnt touched by a player, set it to remove in 2.5 seconds.
					theEntity->SetThink(&CBaseEntity::SUB_Remove);
					theEntity->pev->nextthink = gpGlobals->time + 2.5f;

					//voogru: force them to touch it.
					DispatchTouch(ENT(theEntity->pev), ENT(inPlayer->pev));
				}

				// Do special stuff for some buildings (special case scan so it doesn't "teleport in")
				if(inBuildID != BUILD_SCAN)
				{
					//voogru: play effect at player origin in combat, cause the item is in the middle of nowhere.
					Vector vecOrigin = (theEntity->IsInWorld() && !GetGameRules()->GetIsCombatMode()) ? inLocation : inPlayer->pev->origin;
					PLAYBACK_EVENT_FULL(0, inPlayer->edict(), gPhaseInEventID, 0, vecOrigin, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
				}
			}
		}
	}
	
	return theEntity;
}

void AvHSUExplodeEntity(CBaseEntity* inEntity, Materials inMaterial)
{
	CBreakable* theDebris = NULL;
	theDebris = GetClassPtr((CBreakable*)NULL);
	
	UTIL_SetOrigin(VARS(theDebris->pev), inEntity->pev->origin);
	
	theDebris->pev->model = inEntity->pev->model;
	
	theDebris->SetMaterial(inMaterial);
	
	theDebris->Spawn();
	
	theDebris->Die();

	UTIL_Remove(theDebris);
}

Vector AvHSUEyeToBodyVector(entvars_t* inEye, CBaseEntity* inTarget)
{
	Vector vecMid = inEye->origin + inEye->view_ofs;
	Vector vecMidEnemy = inTarget->BodyTarget(vecMid);
	Vector vecDirToEnemy = vecMidEnemy - vecMid;
	return vecDirToEnemy;
}

float AvHSUEyeToBodyDistance(entvars_t* inEye, CBaseEntity* inTarget)
{
	return AvHSUEyeToBodyVector(inEye, inTarget).Length();
}

float AvHSUEyeToBodyXYDistance(entvars_t* inEye, CBaseEntity* inTarget)
{
	return AvHSUEyeToBodyVector(inEye, inTarget).Length2D();
}

bool AvHSSUGetIsClassNameFadeable(const char* inClassName)
{
	bool theSuccess = false;
	
	if(/*FStrEq(inClassName, kesFuncDoor) || FStrEq(inClassName, "func_door_rotating") || FStrEq(inClassName, "momentary_door") ||*/ FStrEq(inClassName, kesSeethrough) || FStrEq(inClassName, kesSeethroughDoor))
	{
		theSuccess = true;
	}
	
	return theSuccess;
}

void AvHSUPlayPhaseInEffect(int inFlags, CBaseEntity* inStartEntity, CBaseEntity* inEndEntity)
{
	// Play sound and implosion effect
	EMIT_SOUND(ENT(inStartEntity->pev), CHAN_AUTO, kPhaseGateTransportSound, .8, ATTN_NORM);

	MESSAGE_BEGIN(MSG_PAS, SVC_TEMPENTITY, inStartEntity->pev->origin);
		WRITE_BYTE(TE_IMPLOSION);
		WRITE_COORD(inStartEntity->pev->origin.x);
		WRITE_COORD(inStartEntity->pev->origin.y);
		WRITE_COORD(inStartEntity->pev->origin.z + 16);
		WRITE_BYTE(255);
		WRITE_BYTE(15);
		WRITE_BYTE(4);
	MESSAGE_END();


	// Play sound and particles
	EMIT_SOUND(ENT(inEndEntity->pev), CHAN_AUTO, kPhaseGateTransportSound, .8, ATTN_NORM);

	PLAYBACK_EVENT_FULL(inFlags, inEndEntity->edict(), gTeleportEventID, 0, inEndEntity->pev->origin, inEndEntity->pev->angles, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
}

void AvHSUPlayRandomConstructionEffect(AvHPlayer* inPlayer, CBaseEntity* inConstructee)
{
	bool theIsMarine = inPlayer->GetIsMarine();
	float theVolume = .3f*AvHPlayerUpgrade::GetSilenceVolumeLevel(inPlayer->GetUser3(), inPlayer->pev->iuser4);
	if(!theIsMarine)
	{
		gSoundListManager.PlaySoundInList(kAlienConstructionSoundList, inConstructee, CHAN_BODY, theVolume);

		// TODO: Play cool particle puff or something
	}
	else
	{
		gSoundListManager.PlaySoundInList(kMarineConstructionSoundList, inConstructee, CHAN_BODY, theVolume);

		// Play sparks every other time
		if(RANDOM_LONG(0, 1) == 1)
		{
			// Emit sparks from most of the way up the turret
			Vector theVector(inConstructee->pev->origin);
			float theRandomFloat = RANDOM_FLOAT(0.5F, 1.0f);
			float theHeight = theRandomFloat*(inConstructee->pev->absmax.z - inConstructee->pev->absmin.z);
			theVector.z += theHeight;
			UTIL_Sparks(theVector);
		}
		
	}
}

bool AvHSUPlayerCanBuild(entvars_t* inPev)
{
	bool thePlayerCanBuild = false;
	
	// If player is a marine or a builder-alien, he can build
	if((inPev->iuser3 == AVH_USER3_MARINE_PLAYER) || (inPev->iuser3 == AVH_USER3_ALIEN_PLAYER2))
	{
		thePlayerCanBuild = true;
	}
	
	return thePlayerCanBuild;
}

bool AvHSUPlayParticleEvent(const char* inParticleSystemName, const edict_t* inEdict, const Vector& inOrigin, int inEventFlags)
{
	bool theSuccess = false;

	// Look up particle system template by name
	uint32 theTemplateIndex = 0;
	if(gParticleTemplateList.GetTemplateIndexWithName(inParticleSystemName, theTemplateIndex))
	{
		PLAYBACK_EVENT_FULL(inEventFlags, inEdict, gParticleEventID, 0, (float*)&inOrigin, (float*)&g_vecZero, 0.0f, 0.0, theTemplateIndex, 0, 0, 0);

		theSuccess = true;
	}

	return theSuccess;
}

void AvHSUPlayNumericEvent(float inNumber, const edict_t* inEdict, Vector& inOrigin, int inEventFlags, int inNumericEventType, int inTeamNumber)
{
	PLAYBACK_EVENT_FULL(0, inEdict, gNumericalInfoEventID, 0, inOrigin, (float *)&g_vecZero, inNumber, 0.0, inNumericEventType, inTeamNumber, 0, 0);
}

void AvHSUPlayNumericEventAboveStructure(float inNumber, AvHBaseBuildable* inBuildable, int inNumericEventType)
{
    // Generate visual resource event
    Vector theMinSize;
    Vector theMaxSize;
    AvHSHUGetSizeForTech(inBuildable->GetMessageID(), theMinSize, theMaxSize);

    Vector theStartPos = inBuildable->pev->origin;
    theStartPos.z += theMaxSize.z;

    AvHSUPlayNumericEvent(inNumber, inBuildable->edict(), theStartPos, 0, inNumericEventType, inBuildable->pev->team);
}

void AvHSURemoveAllEntities(const char* inClassName)
{
	FOR_ALL_ENTITIES(inClassName, CBaseEntity*);
	UTIL_Remove(theEntity);
	END_FOR_ALL_ENTITIES(inClassName);
}

// Works for ammo and health only, for use in Combat mode.
void AvHSUResupplyFriendliesInRange(int inNumEntitiesToCreate, AvHPlayer* inPlayer, int inRange)
{
	// Create list of nearby eligible players
	PlayerListType thePlayerList;

	thePlayerList.push_back(inPlayer);

	if(inRange > 0)
	{
		CBaseEntity* theEntity = NULL;
		while ((theEntity = UTIL_FindEntityInSphere(theEntity, inPlayer->pev->origin, inRange)) != NULL)
		{
			const char* theClassName = STRING(theEntity->pev->classname);
			if(!AvHSUGetIsExternalClassName(theClassName))
			{
				AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
				if(thePlayer && thePlayer->GetIsRelevant() && (thePlayer->GetTeam() == inPlayer->GetTeam()) && (thePlayer != inPlayer))
				{
					thePlayerList.push_back(thePlayer);
				}
			}
		}
	}

	// While there are more to supply
	while(inNumEntitiesToCreate > 0)
	{
		// Pick the most eligible player, giving preference to creating player
		int theRandomOffset = RANDOM_LONG(0, thePlayerList.size() - 1);
		AvHPlayer* thePlayer = thePlayerList[theRandomOffset];

		// Give player entity
		//AvHSUBuildTechForPlayer(inMessageID, thePlayer->pev->origin, inPlayer);
		
		// puzl: 1017 combat resupply amount
		BOOL theHelpedPlayer = AvHHealth::GiveHealth(thePlayer, BALANCE_VAR(kPointsPerHealth));

		if(!theHelpedPlayer)
		{
			theHelpedPlayer = AvHGenericAmmo::GiveAmmo(thePlayer);
		}

		if(theHelpedPlayer)
		{
			// Play event for each person helped
			PLAYBACK_EVENT_FULL(0, thePlayer->edict(), gPhaseInEventID, 0, thePlayer->pev->origin, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
		}
		
		// Decrement
		inNumEntitiesToCreate--;
	}
}

bool AvHSUGetIsOftRepeatedAlert(AvHAlertType inAlertType)
{
    bool theIsOftRepeated = false;

    switch(inAlertType)
    {
    case ALERT_UNDER_ATTACK:
    case ALERT_HIVE_DYING:
    case ALERT_PLAYER_ENGAGE:
    case ALERT_SENTRY_FIRING:
    case ALERT_SENTRY_DAMAGED:
    case ALERT_ORDER_NEEDED:
    case ALERT_SOLDIER_NEEDS_AMMO:
    case ALERT_SOLDIER_NEEDS_HEALTH:
        theIsOftRepeated = true;
        break;
    }

    return theIsOftRepeated;
}

bool AvHSUGetIsUrgentAlert(AvHAlertType inAlertType)
{
	bool theIsUrgent = false;
	
	switch(inAlertType)
	{
	// Very important and relatively rare
	case ALERT_HIVE_COMPLETE:
	case ALERT_NEW_TRAIT:
	case ALERT_LOW_RESOURCES:
    case ALERT_UNDER_ATTACK:
    case ALERT_HIVE_DYING:

	// These must always be played because they don't have any notification on the commander UI
	case ALERT_RESEARCH_COMPLETE:
	case ALERT_UPGRADE_COMPLETE:
		theIsUrgent = true;
		break;
	}
	
	return theIsUrgent;
}

// Used to screen for non-NS entities (they can't use RTTI among others)
bool AvHSUGetIsExternalClassName(const char* inClassName)
{
	bool theIsExternal = false;
	
	if(inClassName && (!strncmp(inClassName, "adminmod_", 9)))
	{
		theIsExternal = true;
	}
	
	return theIsExternal;
}

bool AvHSUGetIsSubjectToVisibilityRules(CBaseEntity* inEntity)
{
	bool theIsSubjectToVis = false;

	//char theErrorString[256];
	const char* theEntityName = "unknown";
	if(inEntity && inEntity->pev)
	{
		const char* theTentativeEntityName = NULL;
		theTentativeEntityName = STRING(inEntity->pev->classname);
		if(theTentativeEntityName)
		{
			theEntityName = theTentativeEntityName;
		}
	}
	
	// Players
	if(!strcmp(theEntityName, kAvHPlayerClassName))
	{
		theIsSubjectToVis = true;
	}
	// All buildables
	else if(GetHasUpgrade(inEntity->pev->iuser4, MASK_BUILDABLE))
	{
		theIsSubjectToVis = true;
	}
	// Hives
	else if(!strcmp(theEntityName, kesTeamHive))
	{
		theIsSubjectToVis = true;
	}
	// Webs
	else if(!strcmp(theEntityName, kesTeamWebStrand))
	{
		theIsSubjectToVis = true;
	}
	// Visibility for particle systems
	else if(!strcmp(theEntityName, kesParticles))
	{
		theIsSubjectToVis = true;
	}
	else if(!strcmp(theEntityName, kesParticlesCustom))
	{
		theIsSubjectToVis = true;
	}
	// Func resources
	else if(!strcmp(theEntityName, kesFuncResource))
	{
		theIsSubjectToVis = true;
	}
	else if(dynamic_cast<AvHBaseBuildable*>(inEntity))
	{
		theIsSubjectToVis = true;
	}
	
	//sprintf(theErrorString, "Entity %s subject to vis: %d.\n", theEntityName, theIsSubjectToVis);
	//ALERT(at_logged, theErrorString);
	
	return theIsSubjectToVis;
}

int AvHSUGetNumHumansInGame(void)
{
	int theCount = 0;
	
	for(int theIndex = 1; theIndex <= gpGlobals->maxClients; theIndex++ )
	{
		CBaseEntity* thePlayer = UTIL_PlayerByIndex(theIndex);
		
		if(thePlayer == NULL)
			continue;
		
		
		if(FNullEnt( thePlayer->pev ) )
			continue;
		
		
		if(FStrEq(STRING(thePlayer->pev->netname), "" ))
			continue;
		
		
        if(FBitSet(thePlayer->pev->flags, FL_FAKECLIENT))
            continue;
		
        theCount++;
    }
	
    return theCount;
}

AvHHive* AvHSUGetRandomActiveHive(AvHTeamNumber inTeam)
{
	AvHHive* theHive = NULL;

	int theNumActiveHives = 0;
	vector<AvHHive*> theHiveList;
	
	FOR_ALL_ENTITIES(kesTeamHive, AvHHive*)
		if(theEntity->GetIsActive() && (theEntity->GetTeamNumber() == inTeam))
		{
			theHiveList.push_back(theEntity);
		}
	END_FOR_ALL_ENTITIES(kesTeamHive)

	if(theHiveList.size() > 0)
	{
		int theRandomHive = RANDOM_LONG(0, theHiveList.size()-1);
		theHive = theHiveList[theRandomHive];
	}

	return theHive;
}

AvHHive* AvHSUGetRandomActivateableHive(AvHTeamNumber inTeam)
{
	AvHHive* theHive = NULL;
	
	int theNumActiveHives = 0;
	vector<AvHHive*> theHiveList;
	
	FOR_ALL_ENTITIES(kesTeamHive, AvHHive*)
		bool theCanBecomeActive = theEntity->CanBecomeActive();
		if(theCanBecomeActive /*&& (theEntity->GetTeamNumber() == inTeam)*/)
		{
			theHiveList.push_back(theEntity);
		}
	END_FOR_ALL_ENTITIES(kesTeamHive)
		
	if(theHiveList.size() > 0)
	{
		int theRandomHive = RANDOM_LONG(0, theHiveList.size()-1);
		theHive = theHiveList[theRandomHive];
	}
		
	return theHive;
}

int AvHSUGetWeaponStayTime()
{
	int theWeaponStayTime = BALANCE_VAR(kWeaponStayTime);
	
	if(GetGameRules()->GetIsCombatMode())
	{
		theWeaponStayTime = BALANCE_VAR(kCombatWeaponStayTime);
	}
	
	return theWeaponStayTime;
}

void AvHSUResearchStarted(CBaseEntity* inResearchEntity, AvHMessageID inResearchingTech)
{
	if(inResearchingTech == RESEARCH_DISTRESSBEACON)
	{
		// Playback event for all, sending time it takes to finish
		int theDistressBeaconTime = GetGameRules()->GetBuildTimeForMessageID(inResearchingTech);

		// Play sound at center of marine spawn
		//Vector theSoundOrigin = GetGameRules()->GetSpawnAreaCenter((AvHTeamNumber)inResearchEntity->pev->team);

		PLAYBACK_EVENT_FULL(0, inResearchEntity->edict(), gDistressBeaconEventID, 0, inResearchEntity->pev->origin, (float *)&g_vecZero, 0.0, 0.0, theDistressBeaconTime, 0, 0, 0 );

		// Play special siren
		//EMIT_SOUND(inResearchEntity->edict(), CHAN_AUTO, kDistressBeaconSound, 1.0f, ATTN_IDLE);
	}
}

void AvHSUResearchComplete(CBaseEntity* inResearchEntity, AvHMessageID inResearchingTech)
{
	// Watch for various research messages
	//if(inResearchingTech == RESEARCH_REINFORCEMENTS)
	//{
	//	AvHInfantryPortal* thePortal = dynamic_cast<AvHInfantryPortal*>(inResearchEntity);
	// This should never happen but don't crash right now while I fix the bug for the playtest
	//ASSERT(thePortal);
	//if(thePortal)
	//{
	//	thePortal->SetReinforcements(thePortal->GetReinforcements() + 1);
	//}
	//}
	
	if(inResearchingTech == RESOURCE_UPGRADE)
	{
		AvHResourceTower* theTower = dynamic_cast<AvHResourceTower*>(inResearchEntity);
		if(theTower)
		{
			theTower->Upgrade();
		}
		// This should never happen but don't crash right now while I fix the bug for the playtest
		//else
		//{
		//	ASSERT(false);
		//}
	}			
	else if(inResearchingTech == ARMORY_UPGRADE)
	{
		AvHArmory* theArmory = dynamic_cast<AvHArmory*>(inResearchEntity);
		if(theArmory)
		{
			theArmory->Upgrade();
		}
	}
	else if(inResearchingTech == TURRET_FACTORY_UPGRADE)
	{
		AvHTurretFactory* theTurretFactory = dynamic_cast<AvHTurretFactory*>(inResearchEntity);
		if(theTurretFactory)
		{
			theTurretFactory->Upgrade();
		}
	}
	else if(inResearchingTech == RESEARCH_DISTRESSBEACON)
	{
		// Player distress sound effect?
		
		// Immediately respawn all marines at base!
		FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
			if(theEntity->pev->team == inResearchEntity->pev->team)
			{
				bool theSuccess = false;

				AvHPlayMode thePlayMode = theEntity->GetPlayMode();

				// Respawn dead/waiting players
				if((thePlayMode == PLAYMODE_REINFORCING) || (thePlayMode == PLAYMODE_AWAITINGREINFORCEMENT) || (!theEntity->IsAlive() && (thePlayMode == PLAYMODE_PLAYING)))
				{
					theEntity->SetPlayMode(PLAYMODE_PLAYING);
					theSuccess = true;
				}
				// teleport existing players back 
				else if((thePlayMode == PLAYMODE_PLAYING) && theEntity->GetCanBeAffectedByEnemies())
				{
					if ( GetGameRules()->CanPlayerBeacon(theEntity) ) 
					{
						edict_t* theSpawnPoint = GetGameRules()->SelectSpawnPoint(theEntity);

						if(theSpawnPoint)
						{
							if ( VectorDistance(theSpawnPoint->v.origin, theEntity->pev->origin) > BALANCE_VAR(kDistressBeaconRange)) 
							{
								theEntity->InitPlayerFromSpawn(theSpawnPoint);

								// Reset anything some things
								theEntity->SetEnsnareState(false);
								theEntity->SetIsStunned(false);
								theSuccess = true;
							}
						}
					}
				}

				if(theSuccess)
				{
					// Play special phase effect/sound for player
					int theFlags = 0;
					AvHSUPlayPhaseInEffect(theFlags, theEntity, theEntity);
				}
			}
		END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
	}
	else if(inResearchingTech == RESEARCH_ELECTRICAL)
	{
		AvHBaseBuildable* theBuildable = dynamic_cast<AvHBaseBuildable*>(inResearchEntity);
		if(theBuildable)
		{
			// Only allow certain structures to be electrified (fix for exploit timing/selection exploit)
			AvHMessageID theBuildableMessage = theBuildable->GetMessageID();
			switch(theBuildableMessage)
			{
			case BUILD_RESOURCES:
			case BUILD_TURRET_FACTORY:
			case TURRET_FACTORY_UPGRADE:
				SetUpgradeMask(&theBuildable->pev->iuser4, MASK_UPGRADE_11);
				break;
			}
		}
	}
}

template<class x> bool isMember(CBaseEntity* object)
{
	return (object != NULL) && (dynamic_cast<x*>(object) != NULL);
}

//TODO: reimplememnt this functionality as a virtual function under CBaseBuildable to keep the
// information tied directly to the class that it describes, reducing total locations to track (KGP)
bool AvHSUGetIsResearchApplicable(CBaseEntity* inResearchEntity, AvHMessageID inResearchingTech)
{
	bool theIsApplicable = false;

	switch(inResearchingTech)
	{
	case RESOURCE_UPGRADE:
		theIsApplicable = isMember<AvHResourceTower>(inResearchEntity);
		break;
	case ARMORY_UPGRADE:
	case RESEARCH_GRENADES:
		theIsApplicable = isMember<AvHArmory>(inResearchEntity);
		break;
	case TURRET_FACTORY_UPGRADE:
		theIsApplicable = isMember<AvHTurretFactory>(inResearchEntity);
		break;
	case RESEARCH_DISTRESSBEACON:
	case RESEARCH_MOTIONTRACK:
	case RESEARCH_PHASETECH:
		theIsApplicable = isMember<AvHObservatory>(inResearchEntity);
		break;
	case RESEARCH_ARMOR_ONE:
	case RESEARCH_ARMOR_TWO:
	case RESEARCH_ARMOR_THREE:
	case RESEARCH_WEAPONS_ONE:
	case RESEARCH_WEAPONS_TWO:
	case RESEARCH_WEAPONS_THREE:
	case RESEARCH_CATALYSTS:
		theIsApplicable = isMember<AvHArmsLab>(inResearchEntity);
		break;
	case RESEARCH_HEAVYARMOR:
	case RESEARCH_JETPACKS:
		theIsApplicable = isMember<AvHPrototypeLab>(inResearchEntity);
		break;
	case RESEARCH_ELECTRICAL:
		theIsApplicable = isMember<AvHResourceTower>(inResearchEntity) || isMember<AvHTurretFactory>(inResearchEntity);
		break;
	}
	return theIsApplicable;
}

void AvHSUSetCollisionBoxFromSequence(entvars_t* inPev)
{
	ASSERT(inPev);
	
	studiohdr_t* theStudioHeader = (studiohdr_t*)GET_MODEL_PTR( ENT(inPev) );
	if(theStudioHeader == NULL)
	{
		ALERT(at_console, "AvHSUSetCollisionBoxFromSequence(): Invalid model\n");
	}
	
	mstudioseqdesc_t* theSeqDesc = (mstudioseqdesc_t *)((byte *)theStudioHeader + theStudioHeader->seqindex);
	
	Vector theMin = theSeqDesc[inPev->sequence].bbmin;
	Vector theMax = theSeqDesc[inPev->sequence].bbmax;
	
	UTIL_SetSize(inPev, theMin, theMax);
}



////=========================================================================
//// Returns 0 if the area around obj is safe to build in
//int CBaseEntity::CheckArea( CBaseEntity *pIgnore )
//{
//	TraceResult tr;
//	Vector vecOrg = pev->origin;
//	
//	// Check the origin
//	int iContents = UTIL_PointContents(vecOrg);
//	if ( iContents != CONTENT_EMPTY && iContents != CONTENT_WATER ) 
//		return CAREA_BLOCKED;
//	
//	Vector vecIgnoreOrg = pIgnore->pev->origin;
//	// Get the player's origin irrelevant of crouching
//	if ( pIgnore->pev->flags & FL_DUCKING )
//	{
//		vecIgnoreOrg = vecIgnoreOrg + (VEC_DUCK_HULL_MIN -
//			VEC_HULL_MIN);
//	}
//	// Trace a hull
//	UTIL_TraceHull( vecIgnoreOrg, pev->origin, ignore_monsters,
//		large_hull, edict(), &tr );
//	CBaseEntity *pEntity = CBaseEntity::Instance(tr.pHit);
//	if (tr.flFraction != 1 || tr.fAllSolid == 1)
//		return CAREA_BLOCKED;
//	
//	// Check for solid entities in the area
//	CBaseEntity *pEnt = NULL;
//	while ( (pEnt = UTIL_FindEntityInSphere( pEnt, pev->origin, 48 )) !=
//		NULL )
//	{
//		// If it's not the engineer, and its a solid entity, fail
//		if (pEnt != pIgnore && pEnt != this && pEnt->pev->solid >
//			SOLID_TRIGGER)
//			return CAREA_BLOCKED;
//	}
//	
//	// Cycle through all the Nobuild zones in the map and make sure this
//	isn't in one of them
//		CBaseEntity *pNoBuild = UTIL_FindEntityByClassname( NULL,
//		"func_nobuild" );
//	while ( pNoBuild )
//	{
//		// Check to see if we're building in this zone
//		if ( vecOrg.x >= pNoBuild->pev->mins.x && vecOrg.y >=
//			pNoBuild->pev->mins.y && vecOrg.z >= pNoBuild->pev->mins.z && 
//			vecOrg.x <= pNoBuild->pev->maxs.x &&
//			vecOrg.y <= pNoBuild->pev->maxs.y && vecOrg.z <= pNoBuild->pev->maxs.z )
//			return CAREA_NOBUILD;
//		
//		pNoBuild = UTIL_FindEntityByClassname( pNoBuild,
//			"func_nobuild" );
//	}
//	
//	// Check below
//	UTIL_TraceLine( vecOrg, vecOrg + Vector(0,0,-64),
//		dont_ignore_monsters, edict(), &tr );
//	if ( tr.flFraction == 1.0 )
//		return CAREA_BLOCKED;
//	
//	return CAREA_CLEAR;
//}

CBaseEntity* AvHSUGetEntityFromIndex(int inEntityIndex)
{
	CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(inEntityIndex));
	return theEntity;
}


CGrenade* AvHSUShootServerGrenade(entvars_t* inOwner, Vector inOrigin, Vector inVelocity, float inTime, bool inHandGrenade)
{
	CGrenade* theGrenade = CGrenade::ShootExplosiveTimed(inOwner, inOrigin, inVelocity, inTime);
    ASSERT(theGrenade);
    
	theGrenade->pev->team = inOwner->team;

	// Set it as a marine item so it gets damage upgrades
	theGrenade->pev->iuser3 = AVH_USER3_MARINEITEM;
	theGrenade->pev->iuser4 = inOwner->iuser4;

    if(inHandGrenade)
    {
        theGrenade->pev->classname = MAKE_STRING("handgrenade");
    }

//	if(!GetGameRules()->GetDrawInvisibleEntities())
//	{
//		theGrenade->pev->effects |= EF_NODRAW;
//	}

	return theGrenade;
}

void AvHSUKnockPlayerAbout(CBaseEntity* inAttcker, CBaseEntity* inVictim, int inForce)
{
	if(inAttcker && inVictim->IsPlayer())
	{
		AvHPlayer* theVictim = dynamic_cast<AvHPlayer*>(inVictim);

		//Dont _ever_ knock around teammates
		if(inAttcker->pev->team != inVictim->pev->team && theVictim && theVictim->GetCanBeAffectedByEnemies())
		{
			Vector VecDir;
			VecDir = inVictim->pev->origin - inAttcker->pev->origin;
			VecDir = VecDir.Normalize();

			if(theVictim->GetIsMarine() && theVictim->GetHasHeavyArmor())
				inForce = inForce * 0.50; 

			//If they are in the air they have another 50% less knockback (to prevent them from becoming superplayer
			if(!(theVictim->pev->flags & FL_ONGROUND) || !theVictim->pev->groundentity)
			{
				inForce = inForce * 0.50;
			}
			// Reduce knockback to 1/3 if user is ducked and on the ground
			else if ((theVictim->pev->bInDuck) || (theVictim->pev->flags & FL_DUCKING))
			{
				inForce = inForce * 0.33;
			}

			inVictim->pev->punchangle.z = -18;
			inVictim->pev->punchangle.x = 5;
			inVictim->pev->velocity = VecDir * max(0.0f, 120 - VecDir.Length() ) * inForce/75;
		}
	}

	/*AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inEntity);
	if(thePlayer && thePlayer->GetCanBeAffectedByEnemies())
	{
	inEntity->pev->punchangle.z = -18;
	inEntity->pev->punchangle.x = 5;
	if(!(inEntity->pev->flags & FL_ONGROUND) || !inEntity->pev->groundentity) // if the player is in midair, don't use full force
	{
	inEntity->pev->velocity = inEntity->pev->velocity - gpGlobals->v_right*(inForce/10);
	}
	else
	{
	inEntity->pev->velocity = inEntity->pev->velocity - gpGlobals->v_right*inForce;
	}
	}
	}*/
}

void AvHGetLineBounds(const Vector& vecStart, const Vector& vecEnd, Vector& outMins, Vector& outMaxs)
{
    
    outMins = vecStart;
    outMaxs = vecStart;
    
    for (int i = 0; i < 3; ++i)
    {
        
        if (vecStart[i] < outMins[i])
        {
            outMins[i] = vecStart[i];
        }
        
        if (vecStart[i] > outMaxs[i])
        {
            outMaxs[i] = vecStart[i];
        }

        if (vecEnd[i] < outMins[i])
        {
            outMins[i] = vecEnd[i];
        }

        if (vecEnd[i] > outMaxs[i])
        {
            outMaxs[i] = vecEnd[i];
        }

    }

    // Increase the box by a bit since most structure hulls are not very
    // accurately sized.

    const float kBoundingBoxPadding = 100; 

    outMins[0] -= kBoundingBoxPadding;
    outMins[1] -= kBoundingBoxPadding;
    outMins[2] -= kBoundingBoxPadding;

    outMaxs[0] += kBoundingBoxPadding;
    outMaxs[1] += kBoundingBoxPadding;
    outMaxs[2] += kBoundingBoxPadding;

}

/**
 * Drop in replacement for UTIL_TraceLine which works properly for
 */
void AvHTraceLine(const Vector& vecStart, const Vector& vecEnd, IGNORE_MONSTERS igmon, edict_t *pentIgnore, TraceResult *ptr)
{

    // Trace against the world.

    UTIL_TraceLine(vecStart, vecEnd, dont_ignore_monsters, pentIgnore, ptr);

    // Trace against the players/structures.

    if (igmon != ignore_monsters)
    {

        vec3_t theRayOrigin;
        vec3_t theRayDirection;

        VectorCopy(vecStart, theRayOrigin);
        VectorSubtract(vecEnd, vecStart, theRayDirection);

        vec3_t theLineMins;
        vec3_t theLineMaxs;

        AvHGetLineBounds(vecStart, vecEnd, theLineMins, theLineMaxs);
    
        // Get the bounding box for the line.

        const int kMaxNumEntities = 128;
        CBaseEntity* pList[kMaxNumEntities];

        int theNumEntities = UTIL_EntitiesInBox(pList, kMaxNumEntities, theLineMins, theLineMaxs, (FL_CLIENT | FL_MONSTER));
    
        for (int i = 0; i < theNumEntities; ++i)
        {

            edict_t* theEdict = pList[i]->edict();

			// tankefugl: 0000941 -- added check to remove testing of spectators
			if ((!(pList[i]->pev->iuser1 > 0 || pList[i]->pev->flags & FL_SPECTATOR)) && theEdict != pentIgnore)
//            if (theEdict != pentIgnore)
            {

                float t = NS_TraceLineAgainstEntity(pList[i]->entindex(), gpGlobals->time, theRayOrigin, theRayDirection);        

                if (t != AVH_INFINITY && t < ptr->flFraction)
                {
                    ptr->flFraction = t;
                    ptr->pHit       = theEdict;
                    ptr->iHitgroup  = 0; // NS doesn't use hit groups.
                }

            }

        }
    
        // Compute the end position.

        if (ptr->flFraction != 1.0)
        {
            VectorMA(theRayOrigin, ptr->flFraction, theRayDirection, ptr->vecEndPos);
        }
    
    }

}


void AvHSUServerTraceBullets(const Vector& inStart, const Vector& inEnd, IGNORE_MONSTERS inIgnoreMonsters, edict_t* inIgnoreEdict, TraceResult& outTraceResult, bool& outProtected)
{
	outProtected = false;

    // This is the old way that doesn't take into account skulk rotation.
    // UTIL_TraceLine(inStart, inEnd, inIgnoreMonsters, /*dont_ignore_glass,*/ inIgnoreEdict, &outTraceResult);
    
    // TEMP removed the skulk hitboxes since it's too risky for the LAN.
	// joev:  0000573
	// this was commented out meaning that it was just stock tracelines, not using Max M's superb hitbox collision code.
	// Now *all* hitboxes perform as expected and the crouched fade can be shot pretty much anywhere on the model
	// (allowing for about a 5% visual disparity) 
    AvHTraceLine(inStart, inEnd, inIgnoreMonsters, /*dont_ignore_glass,*/ inIgnoreEdict, &outTraceResult);
	// :joev
	CBaseEntity* theEntityHit = CBaseEntity::Instance(outTraceResult.pHit);
	
	// If we hit an entity that's hidden in the umbra, return that we didn't hit anything
	if(theEntityHit)
	{
		if(GetHasUpgrade(theEntityHit->pev->iuser4, MASK_UMBRA))
		{
			const int theUmbraEffectiveness = BALANCE_VAR(kUmbraEffectiveness);
			
			// Block most shots but not all
			if(RANDOM_LONG(0, theUmbraEffectiveness) != 0)
			{
				outProtected = true;
			}
		}
		else
		{
			// Check if bullets pass through an umbra cloud before hitting target
			// If so, don't hit the target
		}
	}
}

void AvHSUUpdateHiveTechology(AvHTeamNumber inTeamNumber, AvHMessageID inBuildID)
{
	AvHHive* theHive = NULL;
	bool theTechnologyAlreadySupported = false;
	
	FOR_ALL_ENTITIES(kesTeamHive, AvHHive*)
		if(theEntity && theEntity->GetIsActive() && (theEntity->pev->team == (int)inTeamNumber))
		{
			AvHMessageID theTechnology = theEntity->GetTechnology();
			if(theTechnology == inBuildID)
			{
				theTechnologyAlreadySupported = true;
				break;
			}
			else if(theTechnology == MESSAGE_NULL)
			{
				if(!theHive)
				{
					theHive = theEntity;
				}
			}
		}
		END_FOR_ALL_ENTITIES(kesTeamHive);
		
		if(!theTechnologyAlreadySupported && theHive)
		{
			theHive->SetTechnology(inBuildID);
		}
}

bool gIsDebugging = false;

bool AvHSUGetIsDebugging()
{
	return gIsDebugging;
}

void AvHSUSetIsDebugging(bool inState)
{
	gIsDebugging = inState;
}

void AvHSUAddDebugPoint(float inX, float inY, float inZ)
{
	CBaseEntity* theBaseEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(1));
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theBaseEntity);
	if(thePlayer)
	{
		thePlayer->AddDebugEnemyBlip(inX, inY, inZ);
	}
}

AvHUser3 AvHSUGetGroupTypeFromSelection(const EntityListType& inEntityListType)
{
	// By default, it's a generic marine
	AvHUser3 theUnifiedUser3 = AVH_USER3_NONE;

	// Loop through all the entities
	bool theFirstTime = true;
	bool theSuccess = false;

	for(EntityListType::const_iterator theIter = inEntityListType.begin(); theIter != inEntityListType.end(); theIter++, theSuccess)
	{
		theSuccess = true;

		CBaseEntity* theBaseEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(*theIter));
		if(theBaseEntity)
		{
			// If all the user3s are the same, then use that
			AvHUser3 theCurrentUser3 = (AvHUser3)(theBaseEntity->pev->iuser3);
			if(theFirstTime || (theCurrentUser3 == theUnifiedUser3))
			{
				theUnifiedUser3 = theCurrentUser3;
				theFirstTime = false;
			}
			else
			{
				theSuccess = false;
			}
		}
		else
		{
			theSuccess = false;
		}
	}

	if(!theSuccess)
	{
		theUnifiedUser3 = AVH_USER3_MARINE_PLAYER;
	}

	return theUnifiedUser3;
}

void AvHSURemoveEntityFromHotgroupsAndSelection(int inEntityIndex)
{
	CBaseEntity* theBaseEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(inEntityIndex));
	if(theBaseEntity)
	{
		// Get team
		AvHTeam* theTeam = GetGameRules()->GetTeam((AvHTeamNumber)theBaseEntity->pev->team);
		if(theTeam)
		{
			// Remove entity from any hotgroups on team
			for(int i=0; i < kNumHotkeyGroups; i++)
			{
				EntityListType theHotGroup = theTeam->GetGroup(i);
			
				EntityListType::iterator theIter = std::find(theHotGroup.begin(), theHotGroup.end(), inEntityIndex);
				if(theIter != theHotGroup.end())
				{
					// Remove it and update it
					theHotGroup.erase(theIter);
			
					theTeam->SetGroup(i, theHotGroup);
				}
			}

			// Now get commander and remove entity from their selection
			AvHPlayer* theCommander = theTeam->GetCommanderPlayer();
			if(theCommander)
			{
				theCommander->RemoveSelection(inEntityIndex);
			}
		}
	}
}

string AvHSUGetLocationText(CBaseEntity* inEntity)
{
	// Add in location name to chat message
	string theLocationString(" ");
	
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inEntity);
	if(thePlayer && thePlayer->GetIsRelevant())
	{
		string theLocationName;
		if(AvHSHUGetNameOfLocation(GetGameRules()->GetInfoLocations(), inEntity->pev->origin, theLocationName))
		{
			theLocationString = theLocationName;
		}
	}
	
	return theLocationString;
}

void AvHSUCreateUmbraCloud(vec3_t inOrigin, AvHTeamNumber inTeamNumber, CBaseEntity* inCreatingEntity)
{
	// Fire umbra cloud event
	PLAYBACK_EVENT_FULL(0, inCreatingEntity->edict(), gUmbraCloudEventID, 0, inOrigin, (float *)&g_vecZero, 1.0f, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
	
	// Create the umbra cloud that will provide gameplay effects
	AvHUmbraCloud* theUmbra = GetClassPtr((AvHUmbraCloud*)NULL );
	theUmbra->Spawn();
	
	VectorCopy(inOrigin, theUmbra->pev->origin);
	
	// Set team and owner
	theUmbra->pev->owner = inCreatingEntity->pev->owner;
	theUmbra->pev->team = inTeamNumber;
}

// Explode with force (players only)
void AvHSUExplosiveForce(const Vector& inOrigin, int inRadius, float inForceScalar, const CBaseEntity* inAttacker, const CBaseEntity* inIgnorePlayer)
{
	CBaseEntity* theEntity = NULL;
	while ((theEntity = UTIL_FindEntityInSphere(theEntity, inOrigin, inRadius)) != NULL)
	{
		const char* theClassName = STRING(theEntity->pev->classname);
		if(!AvHSUGetIsExternalClassName(theClassName))
		{
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
			float theScalar = 1.0f;
			if(thePlayer 
				&& thePlayer->GetCanBeAffectedByEnemies() 
				&& GetGameRules()->CanEntityDoDamageTo(inAttacker, thePlayer, &theScalar) 
				&& (thePlayer != inIgnorePlayer)
				&& thePlayer->pev->team != inAttacker->pev->team)
			{
				// Apply outward force to player
				Vector theForceVector;
				VectorSubtract(thePlayer->pev->origin, inOrigin, theForceVector);
				
				float theForceScalar = inForceScalar;
				float theDistanceToExplosion = theForceVector.Length();
				theDistanceToExplosion = min(max(theDistanceToExplosion, 0.0f), (float)inRadius);
				
				float thePercentForce = (1.0f - (theDistanceToExplosion/(float)inRadius));
				theForceScalar *= thePercentForce;
				
				theForceVector.Normalize();

				// tankefugl: 0000771
				if((!(thePlayer->pev->flags & FL_ONGROUND) || !thePlayer->pev->groundentity) || 
					((thePlayer->pev->bInDuck) || (thePlayer->pev->flags & FL_DUCKING)))
				{
					theForceScalar *= 0.33f; 
				}

				Vector thePreVelocity = thePlayer->pev->velocity;
				float theDamageForce = thePlayer->DamageForce(theForceScalar);
				Vector thePostVelocity = (thePlayer->pev->velocity + theForceVector *( theDamageForce ));

				// check and cap horisontal speed
				float theMaxHorisontalSpeed = BALANCE_VAR(kExplodeMaxHorisontalSpeed);
				Vector theHorisontalSpeed;
				theHorisontalSpeed[0] = thePostVelocity[0];
				theHorisontalSpeed[1] = thePostVelocity[1];
				theHorisontalSpeed[2] = 0;
				float theHorisontalSpeedLength = theHorisontalSpeed.Length();
				float theHorisontalFactor = theMaxHorisontalSpeed / theHorisontalSpeedLength;
				if (theHorisontalSpeedLength > theMaxHorisontalSpeed) {
					thePostVelocity[0] *= theHorisontalFactor;
					thePostVelocity[1] *= theHorisontalFactor;
				}

				// then vertical speed
				float theMaxVerticalSpeed = BALANCE_VAR(kExplodeMaxVerticalSpeed);
				if (fabs(thePostVelocity[2]) > theMaxVerticalSpeed)
				{
					thePostVelocity[2] = thePostVelocity[2]/fabs(thePostVelocity[2]) * theMaxVerticalSpeed;
				}

				// assign new speed
				thePlayer->pev->velocity = thePostVelocity;
				// tankefugl
			}
		}
	}
}

int AvHSUGetValveHull(int inHull)
{
	// Convert "our" hull into VALVe hull
	int theHull = inHull;
	switch(inHull)
	{
	case 0:
		theHull = 1;
		break;
	case 1:
		theHull = 3;
		break;
	case 2:
		theHull = 0;
		break;
	case 3:
		theHull = 2;
		break;
	default:
		ASSERT(false);
		break;
	}

	return theHull;
}

bool AvHSUGetIsEnoughRoomForHull(const vec3_t& inCenter, int inHull, edict_t* inIgnoreEntity, bool inIgnorePlayers, bool inIgnoreEntities)
{
	ASSERT(inHull >= 0);
	ASSERT(inHull <= 3);

	// Check hull against the world
	if(pmove && (pmove->numphysent > 0))
	{
        
        model_s* theWorldModel = pmove->physents[0].model;
		
        // Convert "our" hull into VALVe hull.

		int theHull = AvHSUGetValveHull(inHull);
        const hull_t* theHullPtr = theWorldModel->hulls + theHull;

		int thePointContents = NS_PointContents(theHullPtr, theHullPtr->firstclipnode, (float*)&inCenter);
		if(thePointContents == CONTENTS_SOLID)
		{
    		return false;
		}
    }

    if (!inIgnoreEntities)
    {

	    // If that succeeds, check hull size against entities (above check doesn't take ents into account)
	    
	    // Use input hull to get min and max vectors
	    vec3_t theMin;
	    vec3_t theMax;
	    
	    // Scale it down just a tad so we can morph on ramps.  The shared movement code should get us unstuck in these minor cases
	    float kScalar = 1;//.85f;
	    
	    switch(inHull)
	    {
	    case 0:
		    theMin = HULL0_MIN*kScalar;
		    theMax = HULL0_MAX*kScalar;
		    break;
	    case 1:
		    theMin = HULL1_MIN*kScalar;
		    theMax = HULL1_MAX*kScalar;
		    break;
	    case 2:
		    theMin = HULL2_MIN*kScalar;
		    theMax = HULL2_MAX*kScalar;
		    break;
	    case 3:
		    theMin = HULL3_MIN*kScalar;
		    theMax = HULL3_MAX*kScalar;
		    break;
	    }
	    
	    return AvHSHUGetIsAreaFree(inCenter, theMin, theMax, inIgnoreEntity, inIgnorePlayers);
    
    }

    return true;

}

void AvHSUPrintDevMessage(const string& inMessage, bool inForce)
{
	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)

	#ifndef DEBUG
		bool theSendMessage = inForce;
		if( !theSendMessage )
		{
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(theEntity->edict()));
			if(thePlayer && thePlayer->GetIsMember(PLAYERAUTH_DEVELOPER) )
			{
				theSendMessage = true;
			}
		}		
		if( !theSendMessage )
		{
			continue;
		}
	#endif

	ClientPrint(theEntity->pev, HUD_PRINTNOTIFY, inMessage.c_str());

	END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
}

bool AvHCheckLineOfSight(const Vector& vecStart, const Vector& vecEnd, edict_t* pentIgnore, IGNORE_MONSTERS igmon, edict_t* pEntTarget)
{

    TraceResult tr;
    UTIL_TraceLine(vecStart, vecEnd, igmon, ignore_glass, pentIgnore, &tr);

    edict_t* theLastHit = NULL;
    
    while ( 1 )
    {

        bool theSeeThrough = false;
        
        if (tr.flFraction < 1 && tr.pHit != NULL)
        {
            if (tr.pHit->v.rendermode != kRenderNormal)
            {
                theSeeThrough = true;
            }
            else if ( AvHSSUGetIsClassNameFadeable(STRING(tr.pHit->v.classname)) && (int)tr.pHit->v.fuser2 != 255 )
            {
                theSeeThrough = true;
            }
        }
            
        if (!theSeeThrough)
        {
            break;
        }

        edict_t* temp = tr.pHit;
        UTIL_TraceLine(tr.vecEndPos, vecEnd, igmon, ignore_glass, tr.pHit, &tr);
            
        // To avoid an infinite loop if multiple entities are overlapping.

        if (theLastHit == tr.pHit)
        {
            break;
        }

        theLastHit = temp;

    }

    if (pEntTarget != NULL)
    {
    
        const char* s = STRING(tr.pHit->v.classname);
        return tr.flFraction < 1 && tr.pHit == pEntTarget;
    }
    else
    {
        return tr.flFraction == 1;
    }

}
