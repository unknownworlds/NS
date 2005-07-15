//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: NS high-level game rules
//
// $Workfile: AvHGamerules.cpp $
// $Date: 2002/11/22 21:23:55 $
//
//-------------------------------------------------------------------------------
// $Log: AvHGamerules.cpp,v $
// Revision 1.79  2002/11/22 21:23:55  Flayra
// - Players can't switch teams after seeing a team
// - Adminmod fixes
// - Fixed bug where players were only doing friendly-fire damage to world entities (ie, 33% to doors and breakables)
// - Changed allowed team discrepancy from 2 to 1.  Affects casual model only.
// - Fixes for timelimit
// - A team now automatically wins if they have 4 or more players then another team (generally because the losing team starts quitting).  Affects casual mode only.
//
// Revision 1.78  2002/11/15 23:41:52  Flayra
// - Spectators get end-game victory sound and message
//
// Revision 1.77  2002/11/15 23:31:12  Flayra
// - Added "ready" verification for tourny mode
//
// Revision 1.76  2002/11/15 04:46:18  Flayra
// - Changes to for profiling and for improving AddToFullPack performance
//
// Revision 1.75  2002/11/12 18:44:10  Flayra
// - Allow team unbalancing in tourny mode (assume everyone knows what they're doing)
//
// Revision 1.74  2002/11/12 06:21:12  Flayra
// - More AdminMod fixes
//
// Revision 1.73  2002/11/12 02:24:50  Flayra
// - HLTV updates
// - Remove babblers on level clean up
// - Don't dynamic_cast all entities, it's causes AdminMod to crash
// - Log end-game stats in standard way
// - Potential fix for server overflow after big games
//
// Revision 1.72  2002/11/06 01:40:01  Flayra
// - Damage refactoring (TakeDamage assumes caller has already adjusted for friendly fire, etc.)
//
// Revision 1.71  2002/11/05 06:17:25  Flayra
// - Balance changes
//
// Revision 1.70  2002/11/03 04:49:44  Flayra
// - Moved constants into .dll out of .cfg
// - Particle systems update much less often, big optimization
// - Team balance fixes
//
// Revision 1.69  2002/10/28 20:34:53  Flayra
// - Reworked game reset slightly, to meter out network usage during game reset
//
// Revision 1.68  2002/10/24 21:25:42  Flayra
// - Skin fixes (random number generator returning all 0s at some stages of the engine)
// - Builders don't get points for building anymore
// - Removed code that was never getting called in ClientConnected (AvHPlayer isn't built yet)
// - Remove armor and jetpacks on round reset
// - Update particle systems again, for particle culling
// - Unbuilt hives show up on hive sight
//
// Revision 1.67  2002/10/20 21:10:41  Flayra
// - Optimizations
//
// Revision 1.66  2002/10/20 17:24:41  Flayra
// - Redid performance improvement (agh)
//
// Revision 1.65  2002/10/20 16:36:09  Flayra
// - Added #ifdef for network metering
// - Added particles back in
//
// Revision 1.64  2002/10/19 22:33:44  Flayra
// - Various server optimizations
//
// Revision 1.63  2002/10/18 22:19:11  Flayra
// - Add sensory chamber saying
//
// Revision 1.62  2002/10/16 20:52:46  Flayra
// - Fixed bug introduced with secondary weapons
//
// Revision 1.61  2002/10/16 00:56:22  Flayra
// - Prototyped curl support for NS stats
// - Added player auth stuff (server string)
// - Added concept of secondary weapons
// - Removed disconnect sound
// - Fixed MOTD
// - Added "order needed" alert
//
// Revision 1.60  2002/10/07 17:49:50  Flayra
// - Play marine building alerts properly
//
// Revision 1.59  2002/10/03 18:44:03  Flayra
// - Play disconnected sound quieter because of mass exodus effect
// - Added functionality to allow players to join a team and run around freely before game countdown starts.  Game resets when countdown starts.
// - Added new alien alerts
//
// Revision 1.58  2002/09/25 20:44:22  Flayra
// - Removed old LOS code
// - Added extra alerts, allow alien code to peacefully coexist
//
// Revision 1.57  2002/09/23 22:16:01  Flayra
// - Added game victory status logging
// - Fixed commander alerts
// - Added new alerts
// - Particle system changes, only send them down when connecting or map changes
//
// Revision 1.56  2002/09/09 19:51:13  Flayra
// - Removed gamerules dictating alien respawn time, now it's in server variable
//
// Revision 1.55  2002/08/31 18:01:01  Flayra
// - Work at VALVe
//
// Revision 1.54  2002/08/16 02:35:09  Flayra
// - Blips now update once per second, instead of once per player per second
//
// Revision 1.53  2002/08/09 00:58:35  Flayra
// - Removed error condition for map validity, allow marines to have only one primary weapon, adjust weapon weights
//
// Revision 1.52  2002/08/02 22:00:24  Flayra
// - New alert system that's not so annoying and is more helpful.  Tweaks for new help system.
//
// Revision 1.51  2002/07/25 16:57:59  flayra
// - Linux changes
//
// Revision 1.50  2002/07/24 18:45:41  Flayra
// - Linux and scripting changes
//
// Revision 1.49  2002/07/23 17:03:20  Flayra
// - Resource model changes, refactoring spawning to fix level 5 redemption bug without code duplication
//
// Revision 1.48  2002/07/10 14:40:48  Flayra
// - Profiling and performance tweaks
//
// Revision 1.47  2002/07/08 16:59:14  Flayra
// - Don't pick up empty weapons, added handicapping, check map validity when loaded, reset players just like all other entities, fixed spawn bug code where it was counting non-team spawns
//
// Revision 1.46  2002/07/01 21:32:43  Flayra
// - Visibility update now updates world for primal scream and umbra, don't update reliable network messages every tick (big optimization)
//
// Revision 1.45  2002/06/25 17:59:03  Flayra
// - Added timelimit (switches map after a game finishes), added info_locations, tried adding team balance (not sure it works yet), give resources for kills
//
// Revision 1.44  2002/06/10 19:54:29  Flayra
// - New minimap support, more attempts to fix picking up of alien weapons
//
// Revision 1.43  2002/06/03 16:45:20  Flayra
// - Breakables and buttons take damage like they should, points added for buildables correctly, weapon weights tweaked
//
// Revision 1.42  2002/05/28 17:40:32  Flayra
// - Minimap refactoring, hive sight "under attack", reinforcement refactoring, don't delete entities marked as permanent (it was deleting hives!), allow players to join the opposite team until this can be fixed for real (server retry gets around this code so this is just an inconvenience), things build fast with cheats
//
// Revision 1.41  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "util/nowarnings.h"
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/player.h"
#include "dlls/weapons.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHConstants.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHEntities.h"
#include "mod/AvHParticleTemplateServer.h"
#include "mod/AvHPlayer.h"
#include "dlls/client.h"
#include "dlls/game.h"
#include "dlls/util.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHServerVariables.h"
#include "mod/AvHMessage.h"
#include "mod/AvHSoundListManager.h"
#include "mod/AvHMovementUtil.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHTitles.h"
#include "mod/AvHParticleSystemManager.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHEntities.h"
#include "mod/AvHVoiceHelper.h"
#include "common/director_cmds.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHDramaticPriority.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHHulls.h"
#include "textrep/TRFactory.h"
#include <stdio.h>
#include "mod/NetworkMeter.h"
#include "mod/AvHScriptManager.h"
#include "mod/AvHCloakable.h"
#include "mod/AvHCommandConstants.h"
#include "mod/AvHAlert.h"
#include "mod/AvHParticleConstants.h"
#include "util/MathUtil.h"
#include "mod/AvHNetworkMessages.h"
#include "mod/AvHNexusServer.h"

// puzl: 0001073
#ifdef USE_OLDAUTH
AuthMaskListType						gAuthMaskList;
extern const char* kSteamIDPending;
extern const char* kSteamIDLocal;
extern const char* kSteamIDBot;
extern const char* kSteamIDInvalidID;
extern const char* kSteamIDDefault;
#endif

AvHSoundListManager						gSoundListManager;
extern AvHVoiceHelper					gVoiceHelper;
CVoiceGameMgr							g_VoiceGameMgr;
extern cvar_t							allow_spectators;
extern cvar_t							avh_autoconcede;
extern cvar_t							avh_limitteams;
//extern cvar_t							avh_teamsizehandicapping;
extern cvar_t							avh_team1damagepercent;
extern cvar_t							avh_team2damagepercent;
extern cvar_t							avh_team3damagepercent;
extern cvar_t							avh_team4damagepercent;
extern cvar_t							avh_drawinvisible;
extern cvar_t							avh_uplink;
extern cvar_t							avh_gametime;
extern cvar_t							avh_ironman;
extern cvar_t                           avh_mapvoteratio;

BOOL IsSpawnPointValid( CBaseEntity *pPlayer, CBaseEntity *pSpot );
inline int FNullEnt( CBaseEntity *ent ) { return (ent == NULL) || FNullEnt( ent->edict() ); }
//extern void ResetCachedEntities();

// Allow assignment within conditional
#pragma warning (disable: 4706)

// Quick way to define or undefine security
extern AvHParticleTemplateListServer	gParticleTemplateList;
extern cvar_t							avh_deathmatchmode;
extern cvar_t							avh_countdowntime;
extern int								gCommanderPointsAwardedEventID;
extern cvar_t							avh_networkmeterrate;

std::string gPlayerNames[128];
cvar_t* cocName;
cvar_t* cocExp;
int gStartPlayerID = 0;
int gServerTick = 0;
int gUpdateEntitiesTick = 0;
bool gServerUpdate = false;
float kPVSCoherencyTime = 1.0f;
extern int kNumReturn0;
extern int kNumReturn1;
extern int kNumCached;
extern int kNumComputed;
extern int kMaxE;
int kServerFrameRate = 0;

#ifdef PROFILE_BUILD
extern cvar_t avh_performance;
int kProfileRunConfig = 0xFFFFFFFF;
#endif

std::string GetLogStringForPlayer( edict_t *pEntity );

// SCRIPTENGINE:
#include "scriptengine/AvHLUA.h"
extern AvHLUA *gLUA;
// :SCRIPTENGINE

const AvHMapExtents& GetMapExtents()
{
	return GetGameRules()->GetMapExtents();
}

void InstallGameRules( void )
{
	SERVER_COMMAND( "exec game.cfg\n" );
	SERVER_EXECUTE( );

    AvHGamerules* theNewGamerules = new AvHGamerules;
	SetGameRules(theNewGamerules);
}

static AvHGamerules* sGameRules = NULL;

AvHGamerules* GetGameRules()
{
	if(!g_pGameRules)
	{
		InstallGameRules();
	}

	if(!sGameRules)
	{
		sGameRules = dynamic_cast<AvHGamerules*>(g_pGameRules);
	}
	ASSERT(sGameRules);

	return sGameRules;
}

void SetGameRules(AvHGamerules* inGameRules)
{
	sGameRules = inGameRules;
	g_pGameRules = inGameRules;
}

static float gSvCheatsLastUpdateTime;
AvHGamerules::AvHGamerules() : mTeamA(TEAM_ONE), mTeamB(TEAM_TWO)
{
	this->mGameStarted = false;
	this->mPreserveTeams = false;

	this->mTeamA.SetTeamType(AVH_CLASS_TYPE_MARINE);
	this->mTeamB.SetTeamType(AVH_CLASS_TYPE_ALIEN);
	gSvCheatsLastUpdateTime = -1.0f;
	this->mVictoryTime = -1;
	this->mMapMode = MAP_MODE_UNDEFINED;
	this->mLastParticleUpdate = -1;
	this->mLastNetworkUpdate = -1;
	this->mLastWorldEntityUpdate = -1;
	this->mLastMapChange = -1;
	this->mTimeOfLastPlaytestUpdate = -1;
	this->mTimeOfLastHandicapUpdate = -1;
	this->mMapGamma = kDefaultMapGamma;
	this->mCombatAttackingTeamNumber = TEAM_IND;
	this->mCheats.clear();
	this->mSpawnEntity = NULL;

    RegisterServerVariable(kvBlockScripts);
	RegisterServerVariable(kvTournamentMode);
    RegisterServerVariable(kvTeam1DamagePercent);
    RegisterServerVariable(kvTeam2DamagePercent);
    RegisterServerVariable(kvTeam3DamagePercent);
    RegisterServerVariable(kvTeam4DamagePercent);
    RegisterServerVariable("sv_cheats");

	g_VoiceGameMgr.Init(&gVoiceHelper, gpGlobals->maxClients);

	#ifdef DEBUG
	avh_drawinvisible.value = 1;
	#endif

	this->ResetGame();
}

AvHGamerules::~AvHGamerules()
{
	int a = 0;
}


int	AvHGamerules::AmmoShouldRespawn( CBasePlayerAmmo *pAmmo )
{
	return GR_AMMO_RESPAWN_NO;
}

int	AvHGamerules::WeaponShouldRespawn(CBasePlayerItem *pWeapon)
{
	return GR_WEAPON_RESPAWN_NO;
}

// puzl: 0001073
#ifdef USE_OLDAUTH //players are authorized by UPP now.
const AuthIDListType& AvHGamerules::GetServerOpList() const
{
	return this->mServerOpList;
}

bool AvHGamerules::PerformHardAuthorization(AvHPlayer* inPlayer) const
{
	bool theAuthorized = true;

	#ifdef AVH_SECURE_PRERELEASE_BUILD
	if(!this->GetIsClientAuthorizedToPlay(inPlayer->edict(), false, true))
	{
		char* theMessage = UTIL_VarArgs(
			"%s<%s> is not authorized to play on beta NS servers.\n",
			STRING(inPlayer->pev->netname),
			AvHSUGetPlayerAuthIDString(inPlayer->edict()).c_str()
			);
		ALERT(at_logged, theMessage);

		// Boot player off server
		inPlayer->Kick();

		theAuthorized = false;
	}
	#endif

	return theAuthorized;
}
#endif

// Sets the player up to join the team, though they may not respawn in immediately depending
// on the ruleset and the state of the game.  Assumes 1 or a 2 for team number
bool AvHGamerules::AttemptToJoinTeam(AvHPlayer* inPlayer, AvHTeamNumber inTeamToJoin, bool inDisplayErrorMessage)
{
	bool theSuccess = false;
	string theErrorString;

	// Check authorization in secure build
	if(!inPlayer->GetIsAuthorized(AUTH_ACTION_JOIN_TEAM,inTeamToJoin))
	{
		AvHNexus::handleUnauthorizedJoinTeamAttempt(inPlayer->edict(),inTeamToJoin);
	}
	else
// puzl: 0001073
#ifdef USE_OLDAUTH
	if(this->PerformHardAuthorization(inPlayer))
#endif
	{
		int teamA = this->mTeamA.GetTeamNumber();
		int teamB = this->mTeamB.GetTeamNumber();
		if( inTeamToJoin != teamA && inTeamToJoin != teamB && inTeamToJoin != TEAM_IND )
		{
			theErrorString = kTeamNotAvailable;
		}
		else if(inPlayer->GetTeam() != TEAM_IND)
		{
			theErrorString = kAlreadyOnTeam;
		}
		// Can't join teams during the victory intermission (required by AvHPlayer::InternalCommonThink() for gradual player reset)
		else if(this->mVictoryTeam != TEAM_IND)
		{
			int a = 0;
		}
		else if(this->GetCanJoinTeamInFuture(inPlayer, inTeamToJoin, theErrorString))
		{
            this->JoinTeam(inPlayer, inTeamToJoin, inDisplayErrorMessage, false);
			this->MarkDramaticEvent(kJoinTeamPriority, inPlayer->entindex());
			theSuccess = true;
		}

		// Print error message to HUD
		if(!theSuccess)
		{
			if(inDisplayErrorMessage)
			{
				// Display error string
				inPlayer->SendMessage(theErrorString.c_str());
			}
		}
		else
		{
			// joev: Bug 0000767
			// Tell the other players that this player is joining a team.
			if (!this->GetCheatsEnabled()) {
				AvHTeam* theTeam = GetTeam(inTeamToJoin);
				// ensure that the sound only plays if the game already has started
				if (this->mGameStarted == true) {
					theTeam->PlayHUDSoundForAlivePlayers(HUD_SOUND_PLAYERJOIN);
				}
				char* theMessage = UTIL_VarArgs("%s has joined the %s\n",STRING(inPlayer->pev->netname),theTeam->GetTeamPrettyName());
				UTIL_ClientPrintAll(HUD_PRINTTALK, theMessage);
				UTIL_LogPrintf( "%s joined team \"%s\"\n", GetLogStringForPlayer( inPlayer->edict() ).c_str(), AvHSUGetTeamName(inPlayer->pev->team) );
			}
			// :joev
		}
	}
	return theSuccess;
}

// Pick a team for the player to join.  Choose the team with less players.
// If both teams have the same number of player, pick a team randomly
void AvHGamerules::AutoAssignPlayer(AvHPlayer* inPlayer)
{
	int theTeamACount = this->mTeamA.GetPlayerCount();
	int theTeamBCount = this->mTeamB.GetPlayerCount();
	
	bool joinTeamA = (theTeamACount < theTeamBCount) || ( (theTeamACount == theTeamBCount) && RANDOM_LONG(0,1) );

	AvHTeamNumber theTeam = (joinTeamA ? this->mTeamA.GetTeamNumber() : this->mTeamB.GetTeamNumber());

	// Try to join the first team, but don't emit an error if it fails
	if(!this->AttemptToJoinTeam(inPlayer, theTeam, false))
	{
		// If it failed, try the other team and emit an error if it too fails
		theTeam = (joinTeamA ? this->mTeamB.GetTeamNumber() : this->mTeamA.GetTeamNumber());
		this->AttemptToJoinTeam(inPlayer, theTeam, true);
	}
}

void AvHGamerules::RewardPlayerForKill(AvHPlayer* inPlayer, CBaseEntity* inTarget, entvars_t* inInflictor)
{
	ASSERT(inPlayer);
	ASSERT(inTarget);

	// Doesn't count targets of TEAM_IND
	if(inPlayer->pev->team != inTarget->pev->team && (inTarget->pev->team != TEAM_IND))
	{
		// Team could be NULL if spectating and using cheats
		AvHTeam* theTeamPointer = inPlayer->GetTeamPointer();

		// Only award resources for killing players
		if(theTeamPointer && inTarget->IsPlayer())
		{
			if(!this->GetIsCombatMode())
			{
				int theResourceValue = 0;
				int theMin = BALANCE_VAR(kKillRewardMin);
				int theMax = BALANCE_VAR(kKillRewardMax);
				theResourceValue = RANDOM_LONG(theMin, theMax);

				if(theResourceValue > 0)
				{
					// Send killing player a message and sound, telling him he just got the kill and is getting the points
					AvHClassType theClassType = inPlayer->GetClassType();
					if(theClassType == AVH_CLASS_TYPE_MARINE)
					{
						inPlayer->SendMessageOnce(kMarinePointsAwarded, TOOLTIP);
					}
					else if(theClassType == AVH_CLASS_TYPE_ALIEN)
					{
						inPlayer->SendMessageOnce(kAlienPointsAwarded, TOOLTIP);
					}

					// Increment resource score in tourny mode
					theTeamPointer->AddResourcesGathered(theResourceValue);

					AvHSUPlayNumericEvent(theResourceValue, inTarget->edict(), inTarget->pev->origin, 0, kNumericalInfoResourcesEvent, inPlayer->pev->team);

					inPlayer->SetResources(inPlayer->GetResources() + theResourceValue, true);
				}
			}
			else
			{
                // Mine kills don't share experience
                bool theShareExperience = true;
                if(inInflictor)
                {
                    const char* theClassName = STRING(inInflictor->classname);
                    if(theClassName && FStrEq(theClassName, kwsDeployedMine))
                    {
                        theShareExperience = false;
                    }
                }

				AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inPlayer);
				ASSERT(thePlayer);
				this->AwardExperience(inPlayer, thePlayer->GetExperienceLevel(), theShareExperience);
			}
		}

		// Give points or frags for kill
		int thePointReward = inTarget->GetPointValue();
		if(thePointReward != 0)
		{
			inPlayer->SetScore(inPlayer->GetScore() + thePointReward);

			if(inTarget->IsPlayer())
				inPlayer->pev->frags += 1;
			inPlayer->EffectivePlayerClassChanged();
		}
	}
}

int AvHGamerules::IPointsForKill(CBasePlayer *pAttacker, CBasePlayer *pKilled)
{
	// Return 0 because we increment our points via RewardPlayerForKill()
	return 0;
}

void AvHGamerules::BuildableBuilt(AvHBuildable* inBuildable)
{
	// Get player owner from buildable
	edict_t* theEdict = g_engfuncs.pfnPEntityOfEntIndex(inBuildable->GetBuilder());
	if(theEdict != NULL)
	{
		CBaseEntity* theEntity = CBaseEntity::Instance(theEdict);
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
		if(thePlayer)
		{
			AvHBaseBuildable* theBaseBuildable = dynamic_cast<AvHBaseBuildable*>(inBuildable);
			if(theBaseBuildable)
			{
				thePlayer->AddPoints(theBaseBuildable->GetPointValue(), TRUE);
			}
		}
	}
}

void AvHGamerules::BuildableKilled(AvHBuildable* inBuildable)
{
}

void AvHGamerules::BuildMiniMap(AvHPlayer* inPlayer)
{
	const char* theCStrLevelName = STRING(gpGlobals->mapname);
	if(theCStrLevelName && !FStrEq(theCStrLevelName, ""))
	{
		this->mMiniMap.BuildMiniMap(theCStrLevelName, inPlayer, this->mMapExtents);
	}
}

BOOL AvHGamerules::CanHaveAmmo( CBasePlayer *pPlayer, const char *pszAmmoName, int iMaxCarry )
{
	BOOL theCanHaveIt = CHalfLifeTeamplay::CanHaveAmmo(pPlayer, pszAmmoName, iMaxCarry);
	return theCanHaveIt;
}

// The player is touching an CBasePlayerItem, do I give it to him?
BOOL AvHGamerules::CanHavePlayerItem(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon)
{
	BOOL theCanHaveIt = FALSE;

	// Allow it if we don't already have it
	if(CHalfLifeTeamplay::CanHavePlayerItem(pPlayer, pWeapon) && !pPlayer->HasItem(pWeapon))
	{
		// Don't allow players to have more then one primary weapon
		ItemInfo theItemInfo;
		pWeapon->GetItemInfo(&theItemInfo);
		int theWeaponFlags = theItemInfo.iFlags;

		// Check primary and secondary weapons (assumes we only have one, but should work even with both)
		int theCurrentFlag = (theWeaponFlags & (PRIMARY_WEAPON | SECONDARY_WEAPON));
		CBasePlayerItem* theCurrentItem = NULL;
		bool theHasWeaponWithFlag = pPlayer->HasItemWithFlag(theCurrentFlag, theCurrentItem);

		if(theHasWeaponWithFlag)
		{
			if(theCurrentItem->iWeight() < pWeapon->iWeight())
			{
				theCanHaveIt = TRUE;
			}
			else if(this->GetIsCombatMode())
			{
				theCanHaveIt = TRUE;
			}
		}
		else
		{
			theCanHaveIt = TRUE;
		}
	}

	return theCanHaveIt;
}

bool AvHGamerules::CanPlayerBeKilled(CBasePlayer* inPlayer)
{
	bool theCanBeKilled = false;

	// Don't allow players that are being digested to suicide
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inPlayer);
	if(thePlayer)
	{
		if(thePlayer->GetCanBeAffectedByEnemies() && this->GetGameStarted() && !thePlayer->GetIsBeingDigested())
		{
			theCanBeKilled = true;
		}
	}

	return theCanBeKilled;
}

void AvHGamerules::CalculateMapGamma()
{
	// Set defaults
	this->mCalculatedMapGamma = kDefaultMapGamma;

	// Fetch from map extents entity if the map has one
	FOR_ALL_ENTITIES(kwsGammaClassName, AvHGamma*)
		this->mMapGamma = theEntity->GetGamma();
	END_FOR_ALL_ENTITIES(kwsGammaClassName)

	this->mCalculatedMapGamma = true;
}

// puzl: 0001073
#ifdef USE_OLDAUTH
BOOL AvHGamerules::GetIsClientAuthorizedToPlay(edict_t* inEntity, bool inDisplayMessage, bool inForcePending) const
{
	BOOL theIsAuthorized = false;

	#ifndef AVH_SECURE_PRERELEASE_BUILD
	theIsAuthorized = true;
	#endif

	#ifdef AVH_SECURE_PRERELEASE_BUILD
	string theAuthID = AvHSUGetPlayerAuthIDString(inEntity);
	const char* thePlayerName = STRING(inEntity->v.netname);
	if(!strcmp(thePlayerName, ""))
	{
		thePlayerName = "unknown";
	}

    // Allow only select players to play
	int theSecurityMask = PLAYERAUTH_DEVELOPER | PLAYERAUTH_PLAYTESTER  |  PLAYERAUTH_CONTRIBUTOR;

	// If any of these bits are set, allow them to play
    int theAuthMask = 0;

    // Get the auth mask the cheap way if possible
    AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(inEntity));
    if(thePlayer)
    {
        theAuthMask = thePlayer->GetAuthenticationMask();
    }
    else
    {
        theAuthMask = GetGameRules()->GetAuthenticationMask(theAuthID);
    }

	if(theAuthMask & theSecurityMask)
	{
		if(inDisplayMessage)
		{
			char theAuthenticateString[512];
			sprintf(theAuthenticateString, "Player (%s -> %s) authenticated as privileged player.\r\n", thePlayerName, theAuthID.c_str());
			ALERT(at_logged, theAuthenticateString);
		}

		theIsAuthorized = true;
	}
	// Pending
	else if(theAuthID == kSteamIDPending)
	{
		if(!inForcePending)
		{
			// The player is authorized
			theIsAuthorized = true;
		}
	}
	// Local players or bots are always allowed
	else if((theAuthID == kSteamIDLocal) || (theAuthID == kSteamIDBot))
	{
		theIsAuthorized = true;
	}

	// Display message on failure
	if(!theIsAuthorized && inDisplayMessage)
	{
		char theAuthenticateString[512];
		sprintf(theAuthenticateString, "Player (%s -> %s) failed authentication.\r\n", thePlayerName, theAuthID.c_str());
		ALERT(at_logged, theAuthenticateString);
	}

	#endif

	return theIsAuthorized;
}
#endif

// puzl: 0001073
#ifdef USE_OLDAUTH
BOOL AvHGamerules::ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	bool theAllowedToConnect = true;
	BOOL theSuccess = false;

	#ifdef AVH_SECURE_PRERELEASE_BUILD
	this->UpdateUplink();
	#endif

	#ifdef AVH_SECURE_PRERELEASE_BUILD
	theAllowedToConnect = false;
	theAllowedToConnect = this->GetIsClientAuthorizedToPlay(pEntity, true, false);
	#endif

	if(theAllowedToConnect)
	{
		g_VoiceGameMgr.ClientConnected(pEntity);

		// Play connect sound
		EMIT_SOUND(pEntity, CHAN_AUTO, kConnectSound, 0.8, ATTN_NORM);

		theSuccess = CHalfLifeTeamplay::ClientConnected(pEntity, pszName, pszAddress, szRejectReason);
	}
	else
	{
		sprintf(szRejectReason, "Only authorized players can join beta NS servers.\n");
	}

	return theSuccess;
}

#else

BOOL AvHGamerules::ClientConnected( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ] )
{
	BOOL theSuccess = false;

	g_VoiceGameMgr.ClientConnected(pEntity);

	// Play connect sound
	EMIT_SOUND(pEntity, CHAN_AUTO, kConnectSound, 0.8, ATTN_NORM);

	theSuccess = CHalfLifeTeamplay::ClientConnected(pEntity, pszName, pszAddress, szRejectReason);
	return theSuccess;
}
#endif


void AvHGamerules::ClientDisconnected( edict_t *pClient )
{
	// Call down to base class
	CHalfLifeTeamplay::ClientDisconnected(pClient);

	// Play disconnect sound (don't play, it encourages other people to leave)
	//EMIT_SOUND(pClient, CHAN_AUTO, kDisconnectSound, 0.5, ATTN_NORM);

	// Remove him from whatever team he was on (does the player get killed first?)
	CBaseEntity* theEntity = CBaseEntity::Instance(ENT(pClient));
	AvHPlayer* theAvHPlayer = dynamic_cast<AvHPlayer*>(theEntity);
	if(theAvHPlayer)
	{
		theAvHPlayer->ClientDisconnected();
	}

	// trigger a votemap check
	this->RemovePlayerFromVotemap(theEntity->entindex());
}

void AvHGamerules::ClientKill( edict_t *pEntity )
{
}

void AvHGamerules::ClientUserInfoChanged(CBasePlayer *pPlayer, char *infobuffer)
{
	// NOTE: Not currently calling down to parent CHalfLifeTeamplay 
}

void AvHGamerules::ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib )
{
	CHalfLifeTeamplay::ChangePlayerTeam(pPlayer, pTeamName, bKill, bGib);
}


int	AvHGamerules::DeadPlayerAmmo( CBasePlayer *pPlayer )
{
	return GR_PLR_DROP_AMMO_NO;
}

int	AvHGamerules::DeadPlayerWeapons(CBasePlayer* inPlayer)
{
	int theReturnCode = GR_PLR_DROP_GUN_NO;

	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inPlayer);
	ASSERT(thePlayer);
	if(thePlayer->GetIsMarine())
	{
		theReturnCode = GR_PLR_DROP_GUN_ACTIVE;
	}

	return theReturnCode;
}

void AvHGamerules::DeathNotice(CBasePlayer* pVictim, entvars_t* pKiller, entvars_t* pInflictor)
{
	// TODO: do something here?  Call CHalfLifeMultiplay::DeathNotice() to get normal death notices back
	CHalfLifeTeamplay::DeathNotice(pVictim, pKiller, pInflictor);
}

void AvHGamerules::DeleteAndResetEntities()
{
	// Print reset message at console
	char theResetString[128];

	sprintf(theResetString, "Game reset started.\n");
	ALERT(at_logged, theResetString);

	//ResetCachedEntities();

	//FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
	//	theEntity->Reset();
	//END_FOR_ALL_ENTITIES(kAvHPlayerClassName)

	// Clear out alien weapons
	AvHSURemoveAllEntities(kwsBiteGun);
	AvHSURemoveAllEntities(kwsParasiteGun);
	AvHSURemoveAllEntities(kwsLeap);
	AvHSURemoveAllEntities(kwsDivineWind);
	AvHSURemoveAllEntities(kwsSpitGun);
	AvHSURemoveAllEntities(kwsHealingSpray);
	AvHSURemoveAllEntities(kwsBileBombGun);
	AvHSURemoveAllEntities(kwsWebSpinner);
	AvHSURemoveAllEntities(kwsSpikeGun);
	AvHSURemoveAllEntities(kwsSporeGun);
	AvHSURemoveAllEntities(kwsUmbraGun);
	AvHSURemoveAllEntities(kwsPrimalScream);
	AvHSURemoveAllEntities(kwsSwipe);
	AvHSURemoveAllEntities(kwsBlinkGun);
	AvHSURemoveAllEntities(kwsAcidRocketGun);
	AvHSURemoveAllEntities(kwsMetabolize);
	AvHSURemoveAllEntities(kwsClaws);
	AvHSURemoveAllEntities(kwsDevour);
	AvHSURemoveAllEntities(kwsStomp);
	AvHSURemoveAllEntities(kwsCharge);

	// Clear out marine weapons
	AvHSURemoveAllEntities(kwsMachineGun);
	AvHSURemoveAllEntities(kwsPistol);
	AvHSURemoveAllEntities(kwsShotGun);
	AvHSURemoveAllEntities(kwsHeavyMachineGun);
	AvHSURemoveAllEntities(kwsGrenadeGun);
	AvHSURemoveAllEntities(kwsMine);
	AvHSURemoveAllEntities(kwsWelder);
	AvHSURemoveAllEntities(kwsKnife);

	// Remove alien items and projectiles
	AvHSURemoveAllEntities(kwsAcidRocket);
	AvHSURemoveAllEntities(kwsBileBomb);
	AvHSURemoveAllEntities(kwsBabblerProjectile);
	AvHSURemoveAllEntities(kwsSpitProjectile);
	AvHSURemoveAllEntities(kwsSporeProjectile);
	AvHSURemoveAllEntities(kwsStomp);
	AvHSURemoveAllEntities(kwsUmbraCloud);
	AvHSURemoveAllEntities(kwsUmbraProjectile);
	AvHSURemoveAllEntities(kesTeamWebStrand);

	// Remove marine items and projectiles
	AvHSURemoveAllEntities(kwsDeployedMine);
	AvHSURemoveAllEntities(kwsHeavyArmor);
	AvHSURemoveAllEntities(kwsJetpack);
	AvHSURemoveAllEntities(kwsGrenade);
	AvHSURemoveAllEntities(kwsScan);
	AvHSURemoveAllEntities(kwsGenericAmmo);
	AvHSURemoveAllEntities(kwsHealth);
	AvHSURemoveAllEntities(kwsWelder);
	AvHSURemoveAllEntities(kwsCatalyst);
	AvHSURemoveAllEntities(kwsAmmoPack);

	// Remove all non-persistent entities marked buildable
	//AvHSUPrintDevMessage("FOR_ALL_BASEENTITIES: AvHGamerules::DeleteAndResetEntities\n");

	FOR_ALL_BASEENTITIES();
		AvHBaseBuildable* theBuildable = dynamic_cast<AvHBaseBuildable*>(theBaseEntity);
		if(GetHasUpgrade(theBaseEntity->pev->iuser4, MASK_BUILDABLE) && (!theBuildable || !theBuildable->GetIsPersistent()))
		{
			ASSERT(theBaseEntity->pev->iuser3 != AVH_USER3_HIVE);
			if(theBaseEntity->pev->iuser3 == AVH_USER3_HIVE)
			{
				int a = 0;
			}
			UTIL_Remove(theBaseEntity);
		}
	END_FOR_ALL_BASEENTITIES();

	// Delete all non-persistent base buildables.
	//AvHSUPrintDevMessage("FOR_ALL_BASEENTITIES: AvHGamerules::DeleteAndResetEntities#2\n");

	FOR_ALL_BASEENTITIES();
		AvHBaseBuildable* theBuildable = dynamic_cast<AvHBaseBuildable*>(theBaseEntity);
		if(theBuildable && !theBuildable->GetIsPersistent())
		{
			UTIL_Remove(theBuildable);
		}
	END_FOR_ALL_BASEENTITIES();

	// TODO: Remove decals

	this->ResetEntities();

	FireTargets(ktGameReset, NULL, NULL, USE_TOGGLE, 0.0f);

	sprintf(theResetString, "Game reset complete.\n");
	ALERT(at_logged, theResetString);
}

BOOL AvHGamerules::FAllowMonsters( void )
{
	return TRUE;
}

BOOL AvHGamerules::FPlayerCanRespawn( CBasePlayer *pPlayer )
{
	bool theCanRespawn = false;

	return theCanRespawn;
}

bool AvHGamerules::CanEntityDoDamageTo(const CBaseEntity* inAttacker, const CBaseEntity* inReceiver, float* outScalar)
{
	bool theCanDoDamage = false;

	if(inAttacker && inReceiver)
	{
		AvHTeamNumber theAttackerTeam = (AvHTeamNumber)inAttacker->pev->team;
		AvHTeamNumber theReceiverTeam = (AvHTeamNumber)inReceiver->pev->team;
		bool theTeamsAreDifferent = (theAttackerTeam != theReceiverTeam);
		bool theTeamsAreOpposing = theTeamsAreDifferent && (theAttackerTeam != TEAM_IND) && (theReceiverTeam != TEAM_IND);
		bool theGameHasStarted = this->GetGameStarted();
		bool theIsBreakable = (inReceiver->pev->iuser3 == AVH_USER3_BREAKABLE);
		bool theIsDoor = !strcmp(STRING(inReceiver->pev->classname), kesFuncDoor) || !strcmp(STRING(inReceiver->pev->classname), "func_door_rotating") || !strcmp(STRING(inReceiver->pev->classname), "func_button");
		bool theAttackerIsSiege = inAttacker->pev->classname == MAKE_STRING(kwsSiegeTurret);
		bool theAttackerIsMine = inAttacker->pev->classname == MAKE_STRING(kwsDeployedMine);
		bool theReceiverIsMine = inReceiver->pev->classname == MAKE_STRING(kwsDeployedMine);
        bool theIsFriendlyFireEnabled = friendlyfire.value;
		bool theAttackerIsWorld = false;
		bool theReceiverIsPlayer = false;
		bool theAttackerIsReceiver = false;
		float theScalar = 1.0f;
		bool theReceiverIsWorld = false;

		// Never dynamic_cast any entities that could be non-NS entities
		if(!AvHSUGetIsExternalClassName(STRING(inAttacker->pev->classname)))
		{
			theAttackerIsWorld = (dynamic_cast<const CWorld*>(inAttacker) != NULL);
		}

 		if(!AvHSUGetIsExternalClassName(STRING(inAttacker->pev->classname)))
 		{
			theReceiverIsPlayer = (dynamic_cast<const AvHPlayer *>(inReceiver) != NULL);
		}

		if(!AvHSUGetIsExternalClassName(STRING(inReceiver->pev->classname)))
		{
			theReceiverIsWorld = (dynamic_cast<const CWorld*>(inReceiver) != NULL);
		}

		if(!theReceiverIsWorld)
		{
			if((inAttacker == inReceiver) || (CBaseEntity::Instance(inAttacker->pev->owner) == inReceiver))
			{
				theAttackerIsReceiver = true;
			}

			// If we're in tournament mode and two teams are different, yes
			if(theGameHasStarted)
			{
				if(theTeamsAreOpposing || theIsFriendlyFireEnabled || theIsBreakable || theIsDoor || theAttackerIsWorld || theAttackerIsReceiver || ( theAttackerIsMine && theTeamsAreDifferent ))
				{
					theCanDoDamage = true;
					// Do less damage with friendly fire
					if(theAttackerTeam == theReceiverTeam)
					{
						theScalar = .33f;
					}

					if(theAttackerIsReceiver)
					{
						theScalar = .5f;
					}
				}
			}
		}

		// Mines never blow up friendly mines or anything else
		if(theAttackerIsMine)
		{
			// Check if teams are the same
			if( inAttacker->pev->team == inReceiver->pev->team )
			{
				theCanDoDamage = false;
			}
		}

		// Mines can't be blown up by anonymous sources (prevents mines from blowing up other mines)
		if(theReceiverIsMine && theAttackerIsWorld)
		{
			theCanDoDamage = false;
		}

		if(inReceiver->pev->takedamage == DAMAGE_NO)
		{
			theCanDoDamage = false;
		}
		if ( theAttackerIsSiege && theReceiverIsPlayer )
		{
			theCanDoDamage = false;
		}
		if(theCanDoDamage && outScalar)
		{
			*outScalar = theScalar;
		}
	}
	return theCanDoDamage;
}


BOOL AvHGamerules::FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
{
	BOOL theCanTakeDamage = TRUE;

	if(!this->GetDeathMatchMode() && pAttacker)
	{
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(pPlayer);
		if(thePlayer->GetInReadyRoom())
		{
			theCanTakeDamage = FALSE;
		}
		// Friendly fire only in tournament mode (allow players to damage themselves)
		else if((pPlayer != pAttacker) && this->PlayerRelationship(pPlayer, pAttacker) == GR_TEAMMATE)
		{
			if(!(friendlyfire.value))
			{
				theCanTakeDamage = FALSE;
			}
		}
	}

	return theCanTakeDamage;
}

void AvHGamerules::ComputeWorldChecksum(Checksum& outChecksum) const
{
	// Run through all entities in the world, adding their checksum
	//AvHSUPrintDevMessage("FOR_ALL_BASEENTITIES: AvHGamerules::ComputeWorldChecksum\n");

	FOR_ALL_BASEENTITIES();
		theBaseEntity->AddChecksum(outChecksum);
	END_FOR_ALL_BASEENTITIES();
}

bool AvHGamerules::GetArePlayersAllowedToJoinImmediately(void) const
{
	bool thePlayerIsAllowedToJoinImmediately = false;

	// if the game hasn't started
	if(!this->mGameStarted)
	{
		thePlayerIsAllowedToJoinImmediately = true;
	}
	// if cheats are enabled
	else if(this->GetCheatsEnabled())
	{
		thePlayerIsAllowedToJoinImmediately = true;
	}
	else
	{
		// if it's not tournament mode and it's within x seconds of game start
		float theLateJoinSeconds = CVAR_GET_FLOAT(kvLateJoinTime)*60;

		if(!this->GetIsTournamentMode() && (gpGlobals->time < (this->mTimeGameStarted + theLateJoinSeconds)))
		{
			thePlayerIsAllowedToJoinImmediately = true;
		}
	}

	return thePlayerIsAllowedToJoinImmediately;
}

bool AvHGamerules::GetCanJoinTeamInFuture(AvHPlayer* inPlayer, AvHTeamNumber inTeamNumber, string& outString) const
{
	// You can switch teams, unless
	bool theCanJoinTeam = false;

	// You can always go back to ready room
	if(inTeamNumber == TEAM_IND)
	{
        theCanJoinTeam = true;
	}

	// You can always join before the game has started
	// Don't allow team switching once the game has started, or ever in tourny mode
	else if(inPlayer->GetTeam() != inTeamNumber)
	{
		// Server ops can ignore team balance
		if( inPlayer->GetIsMember(PLAYERAUTH_SERVEROP) )
        {
            theCanJoinTeam = true;
        }
        else
		{
			// Check to see if teams are wildly unbalanced
			AvHTeamNumber teamA = this->mTeamA.GetTeamNumber();
			AvHTeamNumber teamB = this->mTeamB.GetTeamNumber();
			AvHTeamNumber theOtherTeamNumber = (inTeamNumber == teamA) ? teamB : teamA;

			const AvHTeam* theTeam = this->GetTeam(inTeamNumber);
			const AvHTeam* theOtherTeam = this->GetTeam(theOtherTeamNumber);

			if( theTeam && theOtherTeam )
			{
				int theWouldBeNumPlayersOnTeam = theTeam->GetPlayerCount() + 1;
				int theWouldBeNumPlayersOnOtherTeam = theOtherTeam->GetPlayerCount();

				// Subtract ourselves off
				if(inPlayer->GetTeam() == theOtherTeamNumber)
				{
					theWouldBeNumPlayersOnOtherTeam--;
				}

				int theDiscrepancyAllowed = max(1.0f, avh_limitteams.value);
				if(((theWouldBeNumPlayersOnTeam - theWouldBeNumPlayersOnOtherTeam) <= theDiscrepancyAllowed) || this->GetIsTournamentMode() || this->GetCheatsEnabled())
				{
					// tankefugl: 0000953
					if (!(this->GetCheatsEnabled()) && !(inPlayer->JoinTeamCooledDown(BALANCE_VAR(kJoinTeamCooldown))))
						outString = kJoinTeamTooFast;
					else
					// :tankefugl
						theCanJoinTeam = true;
				}
				else
				{
					outString = kTooManyPlayersOnTeam;
				}
			}
		}
	}

	return theCanJoinTeam;
}

const AvHBaseInfoLocationListType& AvHGamerules::GetInfoLocations() const
{
	return mInfoLocations;
}

bool AvHGamerules::GetCheatsEnabled(void) const
{
	static float theCheatsEnabled = CVAR_GET_FLOAT( "sv_cheats" );
	if (gpGlobals->time > (gSvCheatsLastUpdateTime + 0.5f))
	{
		theCheatsEnabled = CVAR_GET_FLOAT( "sv_cheats" );
		gSvCheatsLastUpdateTime = gpGlobals->time;
	}
	return (theCheatsEnabled == 1.0f);
}

float AvHGamerules::GetFirstScanThinkTime() const 
{
	return this->GetIsCombatMode() ? 0.25f : 0.5f;
}

bool AvHGamerules::GetDrawInvisibleEntities() const
{
	bool theDrawInvisible = false;

	#ifdef DEBUG
	if(CVAR_GET_FLOAT(kvDrawInvisible) > 0)
	{
		theDrawInvisible = true;
	}
	#endif

	return theDrawInvisible;
}

bool AvHGamerules::GetDeathMatchMode(void) const
{
	bool theInDMMode = false;

	// deathmatch mode means there is no ready room
	if(avh_deathmatchmode.value == 1.0f)
	{
		theInDMMode = true;
	}

	return theInDMMode;
}

bool AvHGamerules::GetEntityExists(const char* inName) const
{
	bool theSuccess = false;

	edict_t* theEnt = FIND_ENTITY_BY_CLASSNAME(NULL, inName);
	if(!FNullEnt(theEnt))
	{
		theSuccess = true;
	}
	else
	{
		// Look in spawns
		for(SpawnListType::const_iterator theIter = this->mSpawnList.begin(); theIter != this->mSpawnList.end(); theIter++)
		{
			if(!strcmp(theIter->GetClassName().c_str(), inName))
			{
				theSuccess = true;
				break;
			}
		}
	}

	return theSuccess;
}

const char* AvHGamerules::GetGameDescription(void)
{
    static char sGameDescription[512];

    sprintf(sGameDescription, "%s %s", kAvHGameAcronymn, AvHSUGetGameVersionString());

    return sGameDescription;
}

//edict_t* AvHGamerules::GetPlayerSpawnSpot( CBasePlayer *pPlayer )
//{
//}

bool AvHGamerules::GetCountdownStarted() const
{
	return this->mStartedCountdown;
}

bool AvHGamerules::GetGameStarted() const
{
    return this->mGameStarted;
}

int AvHGamerules::GetGameTime() const
{
	int theGameTime = 0;

	if(this->GetGameStarted())
	{
		theGameTime = (gpGlobals->time - this->mTimeGameStarted);
	}

	return theGameTime;
}

float AvHGamerules::GetMapGamma()
{
	if(!this->mCalculatedMapGamma)
	{
		this->CalculateMapGamma();
	}

	return this->mMapGamma;
}

const AvHGameplay& AvHGamerules::GetGameplay() const
{
	return this->mGameplay;
}

bool AvHGamerules::GetIsTesting(void) const
{
	return CVAR_GET_FLOAT(kvTesting) > 0;
}

bool AvHGamerules::GetIsTournamentMode(void) const
{
	return (CVAR_GET_FLOAT(kvTournamentMode) == 1.0f);
}

bool AvHGamerules::GetIsCombatMode(void) const
{
	return (this->GetMapMode() == MAP_MODE_CO);
}

AvHTeamNumber AvHGamerules::GetCombatAttackingTeamNumber() const
{
	return this->mCombatAttackingTeamNumber;
}

bool AvHGamerules::GetIsNSMode(void) const
{
    return (this->GetMapMode() == MAP_MODE_NS);
}

bool AvHGamerules::GetIsScriptedMode(void) const
{
    return (this->GetMapMode() == MAP_MODE_NSC);
}

bool AvHGamerules::GetIsHamboneMode() const
{
    return this->GetIsCombatMode() && (CVAR_GET_FLOAT(kvIronMan) == 2);
}

bool AvHGamerules::GetIsIronMan(void) const
{
	return this->GetIsCombatMode() && (CVAR_GET_FLOAT(kvIronMan) == 1);
}

bool AvHGamerules::GetIsTrainingMode(void) const
{
	return (CVAR_GET_FLOAT(kvTrainingMode) == 1.0f);
}

AvHMapMode AvHGamerules::GetMapMode(void) const
{
	return this->mMapMode;
}

const AvHMapExtents& AvHGamerules::GetMapExtents()
{
	this->mMapExtents.CalculateMapExtents();

	return this->mMapExtents;
}

AvHEntityHierarchy& AvHGamerules::GetEntityHierarchy(AvHTeamNumber inTeam)
{
	if(inTeam == this->mTeamA.GetTeamNumber())
	{
		return this->mTeamAEntityHierarchy;
	}
	else if(inTeam == this->mTeamB.GetTeamNumber())
	{
		return this->mTeamBEntityHierarchy;
	}
	else
	{
		ASSERT(false);
	}
	return this->mTeamAEntityHierarchy;
}

bool AvHGamerules::GetIsPlayerSelectableByPlayer(AvHPlayer* inTargetPlayer, AvHPlayer* inByPlayer)
{
	ASSERT(inTargetPlayer);
	ASSERT(inByPlayer);

	bool thePlayerIsSelectable = false;

	if(inTargetPlayer && inTargetPlayer->IsAlive() && (inTargetPlayer != inByPlayer))
	{
		if(inTargetPlayer->pev->team == inByPlayer->pev->team)
		{
			if(!inTargetPlayer->IsObserver())
			{
				thePlayerIsSelectable = true;
			}
		}
	}

	return thePlayerIsSelectable;
}

int	AvHGamerules::GetServerTick() const
{
	return gServerTick;
}

const char*	AvHGamerules::GetSpawnEntityName(AvHPlayer* inPlayer) const
{
	// Come back to ready room by default (in case there is a bug, go back to ready room, don't crash)
	const char* theSpawnEntityName = kesReadyRoomStart;

	AvHClassType theClassType = inPlayer->GetClassType();

	// If there is no no avh start points, try to start up as CS.  If that doesn't look
	// right, always return DM spawns.
	if((this->mMapMode == MAP_MODE_NS) || (this->mMapMode == MAP_MODE_CO) || (this->mMapMode == MAP_MODE_NSC))
	{
		// The different cases:
		// Player just connected to server and hasn't done anything yet OR
		// Player is leaving game and going back to ready room
		if(theClassType == AVH_CLASS_TYPE_UNDEFINED)
		{
			// Use ready room spawn
			theSpawnEntityName = kesReadyRoomStart;
		}
		else
		{
			theSpawnEntityName = kesTeamStart;
		}
	}
	else if(this->mMapMode == MAP_MODE_CS)
	{
		switch(theClassType)
		{
		// "CT"
		case AVH_CLASS_TYPE_MARINE:
			theSpawnEntityName = kesCounterTerroristStart;
			inPlayer->SetPendingCommand(kcJoinTeamOne);
			break;

		// "T"
		case AVH_CLASS_TYPE_ALIEN:
			theSpawnEntityName = kesTerroristStart;
			inPlayer->SetPendingCommand(kcJoinTeamTwo);
			break;

		// Random
		case AVH_CLASS_TYPE_UNDEFINED:
		case AVH_CLASS_TYPE_AUTOASSIGN:
			if(RANDOM_LONG(0, 1) == 0)
			{
				theSpawnEntityName = kesCounterTerroristStart;
				inPlayer->SetPendingCommand(kcJoinTeamOne);
			}
			else
			{
				theSpawnEntityName = kesTerroristStart;
				inPlayer->SetPendingCommand(kcJoinTeamTwo);
			}
			break;
		}
	}
	else if(this->mMapMode == MAP_MODE_DM)
	{
		theSpawnEntityName = kesDeathMatchStart;
	}

	return theSpawnEntityName;
}

float AvHGamerules::GetTimeGameStarted() const
{
	return this->mTimeGameStarted;
}

int AvHGamerules::GetTimeLimit() const
{
	float theMinutes = CVAR_GET_FLOAT("mp_timelimit");

	if(this->GetIsCombatMode())
	{
		theMinutes = CVAR_GET_FLOAT(kvCombatTime);
	}

	int theTimeLimit = (int)(theMinutes*60);

	return theTimeLimit;
}

const AvHTeam* AvHGamerules::GetTeam(AvHTeamNumber inTeamNumber) const
{
	const AvHTeam* theTeam = NULL;

	if( this->mTeamA.GetTeamNumber() == inTeamNumber )
	{ theTeam = &this->mTeamA; }
	if( this->mTeamB.GetTeamNumber() == inTeamNumber )
	{ theTeam = &this->mTeamB; }
	return theTeam;
}

const AvHTeam* AvHGamerules::GetTeamA(void) const
{
	return &this->mTeamA;
}

const AvHTeam* AvHGamerules::GetTeamB(void) const
{
	return &this->mTeamB;
}

AvHTeam* AvHGamerules::GetTeam(AvHTeamNumber inTeamNumber)
{
	AvHTeam* theTeam = NULL;

	if( this->mTeamA.GetTeamNumber() == inTeamNumber )
	{ theTeam = &this->mTeamA; }
	if( this->mTeamB.GetTeamNumber() == inTeamNumber )
	{ theTeam = &this->mTeamB; }
	return theTeam;
}
AvHTeam* AvHGamerules::GetTeamA(void)
{
	return &this->mTeamA;
}

AvHTeam* AvHGamerules::GetTeamB(void)
{
	return &this->mTeamB;
}

AvHTeamNumber AvHGamerules::GetVictoryTeam() const
{
	return this->mVictoryTeam;
}

float AvHGamerules::GetVictoryTime() const
{
	return this->mVictoryTime;
}

// Assumes full encumbered weight around 50
int	AvHGamerules::GetMaxWeight(void) const
{
	return 30;
}

int	AvHGamerules::GetNumCommandersOnTeam(AvHTeamNumber inTeam)
{
	const AvHTeam* theTeam = this->GetTeam(inTeam);
	ASSERT(theTeam);

	int theNumCommanders = (theTeam->GetCommander() == -1 ? 0 : 1);
	return theNumCommanders;
}

int AvHGamerules::GetNumActiveHives(AvHTeamNumber inTeam) const
{
    int theNumHives = 0;

    const AvHTeam* theTeam = this->GetTeam(inTeam);
    if(theTeam)
    {
        theNumHives = theTeam->GetNumActiveHives();
    }

    return theNumHives;
}

int	AvHGamerules::GetNumEntities() const
{
	int theNumEntities = 0;

	//AvHSUPrintDevMessage("FOR_ALL_BASEENTITIES: AvHGamerules::GetNumEntities\n");

//	FOR_ALL_BASEENTITIES();
//		theNumEntities++;
//	END_FOR_ALL_BASEENTITIES();

    theNumEntities = g_engfuncs.pfnNumberOfEntities();

	return theNumEntities;
}

int	AvHGamerules::GetWeightForItemAndAmmo(AvHWeaponID inWeapon, int inNumRounds) const
{
	// Assumes full encumbered weight around 50

	int theWeight = 0;
	ASSERT(inNumRounds >= 0);

	switch(inWeapon)
	{
	case AVH_WEAPON_NONE:
		break;

	case AVH_WEAPON_MG:
		theWeight += 4;
		theWeight += (inNumRounds/100.0f)*1;
		break;

	case AVH_WEAPON_PISTOL:
		theWeight += 2;
		theWeight += (inNumRounds/8.0f)*.5;
		break;

	case AVH_WEAPON_KNIFE:
		// No weight for the knife
		break;

	case AVH_WEAPON_SONIC:
		theWeight += 5;
		theWeight += (inNumRounds/10.0f)*2;
		break;

	case AVH_WEAPON_HMG:
		theWeight += 7;
		theWeight += (inNumRounds/150.0f)*2;
		break;

	case AVH_WEAPON_GRENADE_GUN:
		theWeight += 7;
		theWeight += (inNumRounds/4.0f)*1;
		break;

	case AVH_WEAPON_WELDER:
		theWeight += 4;
		break;

//	case AVH_WEAPON_NUKE:
//		theWeight += 18;
//		theWeight += (inNumRounds/5.0f)*8;
//		break;

	//case AVH_WEAPON_FLAMER:
	//	theWeight += 10;
	//	break;

	case AVH_WEAPON_MINE:
		// Only judge by amount of ammo
		theWeight += inNumRounds;
		break;
	}

	return theWeight;
}

BOOL AvHGamerules::IsMultiplayer( void )
{
	return TRUE;
}

BOOL AvHGamerules::IsDeathmatch( void )
{
	return FALSE;
}

BOOL AvHGamerules::IsCoOp( void )
{
	return FALSE;
}

void AvHGamerules::InitHUD( CBasePlayer *pPlayer )
{
	// Call down to base class first (this may need to be changed later but make sure to update spectator cam)
	CHalfLifeTeamplay::InitHUD(pPlayer);
	// notify other clients of player joining the game
	UTIL_ClientPrintAll( HUD_PRINTNOTIFY, UTIL_VarArgs( "%s has joined the game\n", ( pPlayer->pev->netname && STRING(pPlayer->pev->netname)[0] != 0 ) ? STRING(pPlayer->pev->netname) : "unconnected" ) );
}


void GetMapNamesFromMapCycle(StringList& outMapNameList);

void AvHGamerules::InitializeMapVoteList()
{
    // Add each map
    this->mMapVoteList.clear();
    this->mPlayersVoted.clear();
	this->mPlayersVoteTime.clear();

    StringList theMapNames;
    GetMapNamesFromMapCycle(theMapNames);

    // Traverse list
    for(StringList::iterator theIter = theMapNames.begin(); theIter != theMapNames.end(); theIter++)
    {
        this->mMapVoteList.push_back( make_pair(*theIter, 0) );
    }
}

void AvHGamerules::PlayerGotWeapon(CBasePlayer *pPlayer, CBasePlayerItem *pWeapon)
{
	ItemInfo theItemInfo;
	pWeapon->GetItemInfo(&theItemInfo);
	int theWeaponFlags = theItemInfo.iFlags;
	//bool theDroppedWeapon = false;

	for(int i = 0; i < 2; i++)
	{
		int theCurrentFlag = (i == 0 ? PRIMARY_WEAPON : SECONDARY_WEAPON);

		CBasePlayerItem* theCurrentItem = NULL;
		bool theHasWeaponWithFlag = pPlayer->HasItemWithFlag(theCurrentFlag, theCurrentItem);

		if(((theWeaponFlags & theCurrentFlag) && pPlayer->HasItemWithFlag(theCurrentFlag, theCurrentItem)))
		{
			// Discard the one we have
			char theWeaponName[256];
			const char* theCurrentWeaponName = theCurrentItem->pszName();
			ASSERT(theCurrentWeaponName);

			strcpy(theWeaponName, theCurrentWeaponName);
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(pPlayer);
			if(thePlayer)
			{
				if(GetGameRules()->GetIsCombatMode())
				{
					thePlayer->RemovePlayerItem(theCurrentItem);
                    theCurrentItem->DestroyItem();
				}
				else
				{
					thePlayer->DropItem(theWeaponName);
				}
			}
			//theDroppedWeapon = true;
		}
	}

	//if(!theDroppedWeapon)
	//{
		CHalfLifeTeamplay::PlayerGotWeapon(pPlayer, pWeapon);
	//}

	if(GetGameRules()->GetIsCombatMode())
	{
		if(!AvHSUGetIsExternalClassName(STRING(pWeapon->pev->classname)))
		{
			AvHBasePlayerWeapon* theWeapon = dynamic_cast<AvHBasePlayerWeapon*>(pWeapon);
			if(theWeapon)
			{
				// Spawn with default ammo
				ItemInfo theItemInfo;
				if(theWeapon->UsesAmmo() && theWeapon->GetItemInfo(&theItemInfo) && theWeapon->GetCanBeResupplied())
				{
					//int theMaxPrimary = theItemInfo.iMaxAmmo1;
					int theStartAmmo = theItemInfo.iMaxClip*BALANCE_VAR(kCombatSpawnClips);

					// Add some to primary store
					pPlayer->m_rgAmmo[theWeapon->m_iPrimaryAmmoType] = theStartAmmo;
				}
			}
		}
	}
}

Vector AvHGamerules::GetSpawnAreaCenter(AvHTeamNumber inTeamNumber) const
{
	Vector theCenter(0, 0, 0);
	int theNumSpawns = 0;

	for(SpawnListType::const_iterator theIter = this->mSpawnList.begin(); theIter != this->mSpawnList.end(); theIter++)
	{
		if(theIter->GetTeamNumber() == inTeamNumber)
		{
			VectorAdd(theIter->GetOrigin(), theCenter, theCenter);
			theNumSpawns++;
		}
	}

	if(theNumSpawns > 0)
	{
		VectorScale(theCenter, 1.0f/theNumSpawns, theCenter);
	}

	return theCenter;
}

void AvHGamerules::PerformMapValidityCheck()
{
	// Perform check to see that we have enough of all the entities
	int theNumReadyRoomSpawns = 0;
	int theNumTeamASpawns = 0;
	int theNumTeamBSpawns = 0;
//	CBaseEntity* theSpawn = NULL;
//	while((theSpawn = UTIL_FindEntityByClassname(theSpawn, kesReadyRoomStart)) != NULL)
//	{
//		theNumReadyRoomSpawns++;
//	}

	for(SpawnListType::const_iterator theIter = this->mSpawnList.begin(); theIter != this->mSpawnList.end(); theIter++)
	{
		if(!strcmp(theIter->GetClassName().c_str(), kesReadyRoomStart))
		{
			theNumReadyRoomSpawns++;
		}
		else if(theIter->GetTeamNumber() == this->mTeamA.GetTeamNumber())
		{
			theNumTeamASpawns++;
		}
		else if(theIter->GetTeamNumber() == this->mTeamB.GetTeamNumber())
		{
			theNumTeamBSpawns++;
		}
	}

	int theNumDesiredReadyRoomSpawns = 32;
	int theNumDesiredTeamASpawns = this->mTeamA.GetDesiredSpawnCount();
	int theNumDesiredTeamBSpawns = this->mTeamB.GetDesiredSpawnCount();

	if((theNumReadyRoomSpawns < theNumDesiredReadyRoomSpawns) || (theNumTeamASpawns < theNumDesiredTeamASpawns) || (theNumTeamBSpawns < theNumDesiredTeamBSpawns))
	{
		ALERT(at_logged, "Map validity check failure: %d/%d ready room spawns, %d/%d team A spawns, %d/%d team B spawns.\n", theNumReadyRoomSpawns, theNumDesiredReadyRoomSpawns, theNumTeamASpawns, theNumDesiredTeamASpawns, theNumTeamBSpawns, theNumDesiredTeamBSpawns);
	}
	else
	{
		ALERT(at_logged, "Map validity check success.\n");
	}
}

void AvHGamerules::PostWorldPrecacheInitParticles()
{
	// When a player first connects, send down all particle info
	ASSERT(gParticleTemplateList.GetCreatedTemplates());

	// Make sure client clears out all particle systems first
	int theNumberTemplates = gParticleTemplateList.GetNumberTemplates();
	for(int i = 0; i < theNumberTemplates; i++)
	{
		AvHParticleTemplate* theTemplate = gParticleTemplateList.GetTemplateAtIndex(i);
		gParticleTemplateList.LinkToEntities(theTemplate);
	}
}

void AvHGamerules::PreWorldPrecacheInitParticles()
{
	// Reset templates and everything
	AvHParticleSystemManager::Instance()->Reset();

	// Load up the particle systems from modname.ps then levelname.ps
	TRDescriptionList theDescriptionList;
	string theLevelBaseSystemFile = string(getModDirectory()) + "/" + kBasePSName;
    if(TRFactory::ReadDescriptions(theLevelBaseSystemFile, theDescriptionList))
	{
		gParticleTemplateList.CreateTemplates(theDescriptionList);
	}
	theDescriptionList.clear();

	// TODO: the level name isn't populated yet for some reason
	const char* theCStrLevelName = STRING(gpGlobals->mapname);
	if(theCStrLevelName && !FStrEq(theCStrLevelName, ""))
	{
		string theLevelName = theCStrLevelName;
		string theLevelParticleSystemFile = string(getModDirectory()) + string("/") + theLevelName + string(".ps");
		if(TRFactory::ReadDescriptions(theLevelParticleSystemFile, theDescriptionList))
		{
			gParticleTemplateList.CreateTemplates(theDescriptionList);
		}
	}
}

// Emit line to log indicating victory status
void AvHGamerules::TallyVictoryStats() const
{
	int theGameLength = (int)(gpGlobals->time - this->mTimeGameStarted);
	int theGameLengthMinutes = theGameLength/60;
	int theGameLengthSeconds = theGameLength % 60;

	const char* thePrintableMapName = "ns_?";
	const char* theMapName = STRING(gpGlobals->mapname);
	if(theMapName)
	{
		thePrintableMapName = theMapName;
	}

	const AvHTeam* theVictoryTeam = &this->mTeamA;
	const AvHTeam* theLosingTeam = &this->mTeamB;

	if(this->mVictoryTeam == this->mTeamB.GetTeamNumber())
	{
		theVictoryTeam = &this->mTeamB;
		theLosingTeam = &this->mTeamA;
	}

	ALERT(at_logged, "Team \"%d\" scored \"%d\" with \"%d\" players\n", this->mTeamA.GetTeamNumber(), this->mTeamA.GetTotalResourcesGathered(), this->mTeamA.GetPlayerCount());
	ALERT(at_logged, "Team \"%d\" scored \"%d\" with \"%d\" players\n", this->mTeamB.GetTeamNumber(), this->mTeamB.GetTotalResourcesGathered(), this->mTeamB.GetPlayerCount());

	if(!this->mVictoryDraw)
	{
		ALERT(at_logged, "(map_name \"%s\") (game_version \"%s\") (game_time \"%02d:%02d\") (victory_team \"%s\") (losing_team \"%s\") (winning_teamsize \"%d\") (losing_teamsize \"%d\")\n", thePrintableMapName, AvHSUGetGameVersionString(), theGameLengthMinutes, theGameLengthSeconds, theVictoryTeam->GetTeamTypeString(), theLosingTeam->GetTeamTypeString(), theVictoryTeam->GetPlayerCount(), theLosingTeam->GetPlayerCount());
	}
	else
	{
		ALERT(at_logged, "(map_name \"%s\") (game_version \"%s\") (game_time \"%02d:%02d\") (victory_team \"draw\") (losing_team \"draw\")\n", thePrintableMapName, AvHSUGetGameVersionString(), theGameLengthMinutes, theGameLengthSeconds);
	}
}

void AvHGamerules::InitializeTechNodes()
{
	this->mTeamA.InitializeTechNodes();
	this->mTeamB.InitializeTechNodes();
}

void AvHGamerules::PlayerDeathEnd(AvHPlayer* inPlayer)
{
	ASSERT(inPlayer);

	if(inPlayer->GetPlayMode() == PLAYMODE_PLAYING)
	{
		inPlayer->pev->nextthink = -1;
		inPlayer->SetPlayMode(PLAYMODE_AWAITINGREINFORCEMENT, true);
	}
}

void AvHGamerules::PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	// Tell gamerules
	CHalfLifeTeamplay::PlayerKilled(pVictim, pKiller, pInflictor);

	CBaseEntity* theBaseKiller = CBaseEntity::Instance(pKiller);
	AvHPlayer* theKiller = dynamic_cast<AvHPlayer*>(theBaseKiller);
	if(!theKiller)
	{
		CBaseEntity* theOwner = CBaseEntity::Instance(theBaseKiller->pev->owner);
		theKiller = dynamic_cast<AvHPlayer*>(theOwner);
	}

	if(theKiller)
	{
		this->RewardPlayerForKill(theKiller, pVictim, pInflictor);
	}
}


// From CHalfLifeMultiplay::PlayerSpawn
void AvHGamerules::PlayerSpawn( CBasePlayer *pPlayer )
{
	AvHPlayer*		theAvHPlayer = dynamic_cast<AvHPlayer*>(pPlayer);
	BOOL			addDefault;
	CBaseEntity*	pWeaponEntity = NULL;

	pPlayer->pev->weapons |= (1<<WEAPON_SUIT);

	addDefault = TRUE;

	while ( pWeaponEntity = UTIL_FindEntityByClassname( pWeaponEntity, "game_player_equip" ))
	{
		pWeaponEntity->Touch( pPlayer );
		addDefault = FALSE;
	}

	if ( addDefault )
	{
		theAvHPlayer->pev->team = theAvHPlayer->GetTeam();
		//theAvHPlayer->InitializeFromTeam();
		//this->GetTeam(theAvHPlayer->GetTeam())->AddPlayer(theAvHPlayer->entindex(), theAvHPlayer->GetRole());
	}
}

void AvHGamerules::PlayerThink( CBasePlayer *pPlayer )
{
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(pPlayer);

	// Update base
	CHalfLifeTeamplay::PlayerThink(pPlayer);
}

// Reset all players, remove weapons off the ground, etc.
void AvHGamerules::PostWorldPrecacheReset(bool inNewMap)
{
    EntityListType theJoinedTeamA;
    EntityListType theJoinedTeamB;
    EntityListType theSpectating;

	// If we're preserving teams
	if(this->mPreserveTeams)
	{
		// Remember the players that had already joined
		FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
			int theEntityIndex = theEntity->entindex();

			if(theEntity->pev->team == this->mTeamA.GetTeamNumber())
			{
				theJoinedTeamA.push_back(theEntityIndex);
			}
			else if(theEntity->pev->team == this->mTeamB.GetTeamNumber())
			{
				theJoinedTeamB.push_back(theEntityIndex);
			}
			else if(theEntity->pev->team == TEAM_IND)
			{
				if(theEntity->GetPlayMode() == PLAYMODE_OBSERVER)
				{
					theSpectating.push_back(theEntityIndex);
				}
			}

			theEntity->SendMessageNextThink(kGameStarting);

		END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
	}

	// Stop running all scripts
	AvHScriptManager::Instance()->Reset();

	this->DeleteAndResetEntities();

	gServerTick = 0;
	gServerUpdate = false;
	this->mTimeUpdatedScripts = -1;

	// Reset game on teams
	this->mGameplay.Reset();

	// Find gameplay entity if it exists
	FOR_ALL_ENTITIES(kwsGameplayClassName, AvHGameplay*)
		this->mGameplay = *theEntity;
	END_FOR_ALL_ENTITIES(kwsMapInfoClassName)

	this->mTeamA.ResetGame();
	this->mTeamB.ResetGame();

	this->mTeamAEntityHierarchy.Clear();
	this->mTeamBEntityHierarchy.Clear();

	// Set up both sides
	this->mTeamA.SetTeamType(this->mGameplay.GetTeamAType());
	this->mTeamB.SetTeamType(this->mGameplay.GetTeamBType());

	// Set team numbers differently if teams are the same, so their colors change
	// Marines vs. marines will be team 1 vs, team 3, aliens vs. aliens will be team 2 vs. team 4
	if(this->mTeamA.GetTeamType() == this->mTeamB.GetTeamType())
	{
		if(this->mTeamA.GetTeamType() == AVH_CLASS_TYPE_MARINE)
		{
			this->mTeamA.SetTeamNumber(TEAM_ONE);
			this->mTeamB.SetTeamNumber(TEAM_THREE);
			this->mTeamB.SetTeamName(kMarine2Team);
			this->mTeamB.SetTeamPrettyName(kMarinePrettyName);
		}
		else
		{
			this->mTeamA.SetTeamNumber(TEAM_TWO);
			this->mTeamA.SetTeamName(kAlien1Team);
			this->mTeamA.SetTeamPrettyName(kAlienPrettyName);
			this->mTeamB.SetTeamNumber(TEAM_FOUR);
			this->mTeamB.SetTeamName(kAlien2Team);
			this->mTeamB.SetTeamPrettyName(kAlienPrettyName);
		}
	}

	this->InitializeTechNodes();

	this->PostWorldPrecacheInitParticles();

	// Set round as not started
	this->InternalResetGameRules();

	this->mEntitiesUnderAttack.clear();

	this->mMapExtents.ResetMapExtents();

	this->mCalculatedMapGamma = false;

	// TODO: Clear min/max map sizes?  Others?

	// Find info location entities, store our own internal representation, then delete them for efficiency
	if(inNewMap || (this->mSpawnList.size() == 0))
	{
		this->mInfoLocations.clear();

		FOR_ALL_ENTITIES(kesInfoLocation, AvHInfoLocation*)
			AvHBaseInfoLocation theLocation(*theEntity);
			this->mInfoLocations.push_back(theLocation);
			UTIL_Remove(theEntity);
		END_FOR_ALL_ENTITIES(kesInfoLocation);

		// Clear spawns on a new map only
		this->mSpawnList.clear();
		if(this->mSpawnEntity)
		{
			UTIL_Remove(this->mSpawnEntity);
		}

		FOR_ALL_ENTITIES(kesReadyRoomStart, CPointEntity*)
			string theClassName(STRING(theEntity->pev->classname));
			this->RegisterSpawnPoint(theClassName, theEntity->pev->origin, theEntity->pev->angles, TEAM_IND);
			UTIL_Remove(theEntity);
		END_FOR_ALL_ENTITIES(kesReadyRoomStart);

		FOR_ALL_ENTITIES(kesTeamStart, AvHTeamStartEntity*)
			string theClassName(STRING(theEntity->pev->classname));
			this->RegisterSpawnPoint(theClassName, theEntity->pev->origin, theEntity->pev->angles, theEntity->GetTeamNumber());
			UTIL_Remove(theEntity);
		END_FOR_ALL_ENTITIES(kesTeamStart);

		// Create dummy entity we can use to return from SelectSpawnPoint
		this->mSpawnEntity = CBaseEntity::Create(kesReadyRoomStart, Vector(), Vector());
		ASSERT(this->mSpawnEntity);

		// Added by mmcguire.
        this->mSpawnEntity->pev->effects |= EF_NODRAW;

        // Clear map voting on a level change
        this->mMapVoteList.clear();
        this->mPlayersVoted.clear();
		this->mPlayersVoteTime.clear();
	}

	// Must happen after processing spawn entities
	this->RecalculateMapMode();

	// Loop through all players that are playing and respawn them

	bool theJustResetGameAtCountdownStart = false;

	// Now tell those players to all join again
	EntityListType::iterator theIter, end = theJoinedTeamA.end();
	for(theIter = theJoinedTeamA.begin(); theIter != end; ++theIter)
	{
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(*theIter)));
		if(thePlayer)
		{
			GetGameRules()->JoinTeam(thePlayer, this->mTeamA.GetTeamNumber(), false, true);
		}
		theJustResetGameAtCountdownStart = true;
	}
	theJoinedTeamA.clear();

	// Now tell those players to all join again
	end = theJoinedTeamB.end();
	for(theIter = theJoinedTeamB.begin(); theIter != end; ++theIter)
	{
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(*theIter)));
		if(thePlayer)
		{
			GetGameRules()->JoinTeam(thePlayer, this->mTeamB.GetTeamNumber(), false, true);
		}
		theJustResetGameAtCountdownStart = true;
	}
	theJoinedTeamB.clear();

	// Rejoin spectators
	for(theIter = theSpectating.begin(); theIter != theSpectating.end(); theIter++)
	{
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(*theIter)));
		if(thePlayer)
		{
			thePlayer->SetPlayMode(PLAYMODE_OBSERVER);
		}
		theJustResetGameAtCountdownStart = true;
	}
	theSpectating.clear();

	if(theJustResetGameAtCountdownStart)
	{
		this->mTimeCountDownStarted = this->mSavedTimeCountDownStarted;
		this->mStartedCountdown = true;
	}

	// SCRIPTENGINE: Load map and execute OnLoad
	gLUA->Init();
	if (gLUA->LoadLUAForMap(STRING(gpGlobals->mapname)))
		gLUA->OnLoad();

	//gVoiceHelper.Reset();
}

void AvHGamerules::JoinTeam(AvHPlayer* inPlayer, AvHTeamNumber inTeamToJoin, bool inDisplayErrorMessage, bool inForce)
{
    // Report new player status
	int thePrevTeam = inPlayer->pev->team;
	inPlayer->pev->team = inTeamToJoin;

	inPlayer->InitializeFromTeam();

	AvHServerPlayerData* theServerPlayerData = inPlayer->GetServerPlayerData();

	bool thePlayerCanJoinImmediately = this->GetArePlayersAllowedToJoinImmediately() && this->SelectSpawnPoint(inPlayer) || inForce;

	if(theServerPlayerData
		&& theServerPlayerData->GetHasJoinedTeam()
		&& GetGameRules()->GetGameStarted()
		&& !inForce
		&& inTeamToJoin != TEAM_IND
		&& !GetGameRules()->GetCheatsEnabled())
		thePlayerCanJoinImmediately = false;

	if(thePlayerCanJoinImmediately)
	{
		inPlayer->SetPlayMode(PLAYMODE_PLAYING);
	}
	else
	{
		inPlayer->SetPlayMode(PLAYMODE_AWAITINGREINFORCEMENT);
		inPlayer->SendMessage(kJoinSoon);
	}

	if(inTeamToJoin != TEAM_IND)
	{
		UTIL_LogPrintf( "%s joined team \"%s\"\n", GetLogStringForPlayer( inPlayer->edict() ).c_str(), AvHSUGetTeamName(inPlayer->pev->team) );
		if(theServerPlayerData)
			theServerPlayerData->SetHasJoinedTeam(true);
	}

	// SCRIPTENGINE: Join team
	if (this->GetIsScriptedMode())
		if (thePrevTeam != inTeamToJoin)
			gLUA->OnJointeam(inPlayer->entindex(), inTeamToJoin);
	// :SCRIPTENGINE
}

// This is called before any entities are spawned, every time the map changes, including the first time
void AvHGamerules::PreWorldPrecacheReset()
{
	gParticleTemplateList.Clear();
	AvHMP3Audio::ClearSoundNameList();
	this->PreWorldPrecacheInitParticles();
}

void AvHGamerules::ProcessRespawnCostForPlayer(AvHPlayer* inPlayer)
{
	AvHTeam* theTeam = inPlayer->GetTeamPointer();
	if(theTeam)
	{
		AvHClassType theTeamType = theTeam->GetTeamType();
		if(theTeamType == AVH_CLASS_TYPE_ALIEN)
		{
		}
		else if(theTeamType == AVH_CLASS_TYPE_MARINE)
		{
			this->MarkDramaticEvent(kReinforcementsPriority, inPlayer);
		}
	}
	// No team?  Player died in the ready room...fix?
}

void AvHGamerules::ProcessTeamUpgrade(AvHMessageID inUpgrade, AvHTeamNumber inNumber, int inEntity, bool inGive)
{
	AvHTeam* theTeam = this->GetTeam(inNumber);
	if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_MARINE))
	{
		// Add or remove the upgrade from every entity that's on the team
		FOR_ALL_BASEENTITIES();
		if(theBaseEntity->pev->team == inNumber)
		{
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theBaseEntity);
			if(thePlayer)
			{
				thePlayer->GiveTeamUpgrade(inUpgrade, inGive);
			}
			else
			{
				ProcessGenericUpgrade(theBaseEntity->pev->iuser4, inUpgrade, inGive);
			}
		}
		END_FOR_ALL_BASEENTITIES();

		// Store it in the team too, so new players and entities can be initialized properly
		theTeam->ProcessTeamUpgrade(inUpgrade, inGive);
	}
}

bool AvHGamerules::ReadyToStartCountdown()
{
	bool theReadyToStartCountdown = false;

	bool theAtLeastOneOnTeamA = this->mTeamA.GetPlayerCount() > 0;
	bool theAtLeastOneOnTeamB = this->mTeamB.GetPlayerCount() > 0;

	if(this->GetCheatsEnabled())
	{
		if(theAtLeastOneOnTeamA || theAtLeastOneOnTeamB)
		{
			theReadyToStartCountdown = true;
		}
	}
	else if(theAtLeastOneOnTeamA && theAtLeastOneOnTeamB)
	{
		//need to ready up for tournament mode; otherwise just need players on each team
		if( !this->GetIsTournamentMode() || ( this->mTeamA.GetIsReady() && this->mTeamB.GetIsReady() ) )
		{
			theReadyToStartCountdown = true;
		}
	}

	return theReadyToStartCountdown;
}

void AvHGamerules::RecalculateHandicap()
{
    const float kHandicapMax = 100.0f;

	// Teams can enforce their own handicaps if they want (cap to valid values)
	float handicaps[4];
	handicaps[0] = max(min(kHandicapMax, avh_team1damagepercent.value), 0.0f);
	handicaps[1] = max(min(kHandicapMax, avh_team2damagepercent.value), 0.0f);
	handicaps[2] = max(min(kHandicapMax, avh_team3damagepercent.value), 0.0f);
	handicaps[3] = max(min(kHandicapMax, avh_team4damagepercent.value), 0.0f);

    // Set handicap scalars
	this->mTeamA.SetHandicap(handicaps[this->mTeamA.GetTeamNumber()-1]/kHandicapMax);
	this->mTeamB.SetHandicap(handicaps[this->mTeamB.GetTeamNumber()-1]/kHandicapMax);

	avh_team1damagepercent.value = handicaps[0];
	avh_team2damagepercent.value = handicaps[1];
	avh_team3damagepercent.value = handicaps[2];
	avh_team4damagepercent.value = handicaps[3];
}

// Called when dedicated server exits
void AvHGamerules::ServerExit()
{
	AvHNexus::shutdown();
}

void AvHGamerules::VoteMap(int inPlayerIndex, int inMapIndex)
{
	// check to remove votemap spam
	map< int, float >::iterator thePlayerVoteTime = this->mPlayersVoteTime.find(inPlayerIndex);

	if (thePlayerVoteTime != this->mPlayersVoteTime.end())
	{
		if (thePlayerVoteTime->second + 3.0f > gpGlobals->time)
			return;

		thePlayerVoteTime->second = gpGlobals->time;
	}
	else
	{
		this->mPlayersVoteTime.insert(pair < int, float >(inPlayerIndex, gpGlobals->time));
	}

    if(avh_mapvoteratio.value != -1)
    {
        if(this->mMapVoteList.size() == 0)
        {
            this->InitializeMapVoteList();
        }

        // If this is a valid map
		if((inMapIndex > 0) && (inMapIndex <= (signed)this->mMapVoteList.size()))
		{

			PlayerMapVoteListType::iterator theMappedPlayer = this->mPlayersVoted.find(inPlayerIndex);
			
            // Increment votes for map
            MapVoteListType::iterator theIter = (MapVoteListType::iterator)&this->mMapVoteList[inMapIndex-1];
            int theVotes = ++theIter->second;

			// If player has already voted, decrement previous map and update which map the player has voted
			if(theMappedPlayer != this->mPlayersVoted.end()) {
				((MapVoteListType::iterator)&this->mMapVoteList[theMappedPlayer->second - 1])->second--;
				theMappedPlayer->second = inMapIndex;
			}
			// else, remember the "new" player's vote
			else
			{
				this->mPlayersVoted.insert(pair < int, int >(inPlayerIndex, inMapIndex));
			}

            // Tell everyone
            CBaseEntity* theVotingPlayer = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(inPlayerIndex));
            ASSERT(theVotingPlayer);
            char* theMessage = UTIL_VarArgs("%s executed votemap %d (%s %d/%d)\n", STRING(theVotingPlayer->pev->netname), inMapIndex, theIter->first.c_str(), theIter->second, this->GetVotesNeededForMapChange());
            UTIL_ClientPrintAll(HUD_PRINTTALK, theMessage);
			UTIL_LogPrintf( "%s executed votemap %d (%s %d/%d)\n", GetLogStringForPlayer( theVotingPlayer->edict() ).c_str(), inMapIndex, theIter->first.c_str(), theIter->second, this->GetVotesNeededForMapChange());

            // Does this map enough votes to change?
            if(theVotes >= this->GetVotesNeededForMapChange())
            {
                // Changelevel now
                char theLevelName[256];
                strcpy(theLevelName, theIter->first.c_str());
                CHANGE_LEVEL(theLevelName, NULL);
            }
			
		}
	}
}

// remove a player's vote from the votemap list and trigger a mapchange check
void AvHGamerules::RemovePlayerFromVotemap(int inPlayerIndex)
{
	PlayerMapVoteListType::iterator theMappedPlayer = this->mPlayersVoted.find(inPlayerIndex);

	// If player has voted, decrement the map voted for
	if(theMappedPlayer != this->mPlayersVoted.end()) {
		((MapVoteListType::iterator)&this->mMapVoteList[theMappedPlayer->second - 1])->second--;
		this->mPlayersVoted.erase(inPlayerIndex);
	}

	// trigger mapchange check
	int theVotesNeeded = this->GetVotesNeededForMapChange();
	for(MapVoteListType::iterator theMapIterator = this->mMapVoteList.begin(); 
		theMapIterator != this->mMapVoteList.end(); 
		theMapIterator++)
	{
		if (theMapIterator->second >= theVotesNeeded)
		{
            // Changelevel now
            char theLevelName[256];
            strcpy(theLevelName, theMapIterator->first.c_str());
            CHANGE_LEVEL(theLevelName, NULL);
			break;
		}
	}
}

bool AvHGamerules::GetMapVoteStrings(StringList& outMapVoteList)
{
    bool theSuccess = false;

    if(avh_mapvoteratio.value != -1)
    {
        if(this->mMapVoteList.size() == 0)
        {
            this->InitializeMapVoteList();
        }

        int theMapIndex = 1;
        outMapVoteList.clear();

        for(MapVoteListType::iterator theIter = this->mMapVoteList.begin(); theIter != this->mMapVoteList.end(); theIter++)
        {
            string theOutputString = MakeStringFromInt(theMapIndex) + ") " + theIter->first + " (" + MakeStringFromInt(theIter->second) + "/" + MakeStringFromInt(this->GetVotesNeededForMapChange()) + ")\n";
            outMapVoteList.push_back(theOutputString);
            theMapIndex++;
        }

        theSuccess = true;
    }

    return theSuccess;
}

int AvHGamerules::GetVotesNeededForMapChange() const
{
    int theNumVotes = -1;

    float theMapVoteRatio = avh_mapvoteratio.value;
    if(theMapVoteRatio > 0)
    {
        theMapVoteRatio = min(max(0.0f, theMapVoteRatio), 1.0f);
        theNumVotes = max(theMapVoteRatio*this->GetNumberOfPlayers(), 1.0f);
    }

    return theNumVotes;
}

void AvHGamerules::RegisterSpawnPoint(const string& inClassName, const Vector& inOrigin, const Vector& inAngles, const AvHTeamNumber& inTeamNumber)
{
	AvHSpawn theSpawnPoint(inClassName, inOrigin, inAngles, inTeamNumber);
	this->mSpawnList.push_back(theSpawnPoint);
}

void AvHGamerules::RespawnPlayer(AvHPlayer* inPlayer)
{
	bool thePlayerCanRespawn = this->FPlayerCanRespawn(inPlayer);
	ASSERT(thePlayerCanRespawn);

	this->ProcessRespawnCostForPlayer(inPlayer);

	// Clear alien upgrades, they must be repurchased.  Soldier upgrades stick.
	AvHTeam* theTeam = inPlayer->GetTeamPointer();
	if(theTeam)
	{
		if(theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN)
		{
			inPlayer->pev->iuser4 = 0;
		}

		respawn(inPlayer->pev, !(inPlayer->m_afPhysicsFlags & PFLAG_OBSERVER));// don't copy a corpse if we're in deathcam.
		inPlayer->pev->nextthink = -1;

		// Bring us back in the game
		if(theTeam->GetTeamType() == AVH_CLASS_TYPE_MARINE)
		{
			inPlayer->SetUser3(AVH_USER3_MARINE_PLAYER, true);
		}
		else if(theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN)
		{
			inPlayer->SetUser3(AVH_USER3_ALIEN_PLAYER1, true);
		}
	}
}

void AvHGamerules::ResetGame(bool inPreserveTeams)
{
	// Reset game rules
	this->mFirstUpdate = true;
	this->mPreserveTeams = inPreserveTeams;
	gSvCheatsLastUpdateTime = -1.0f;
}

void AvHGamerules::RecalculateMapMode( void )
{
	// Recalculate map mode we're in.  MAP_MODE_UNDEFINED means just try to spawn somewhere.

	// Check for AVH entities
	if((this->GetEntityExists(kesReadyRoomStart)) && (this->GetEntityExists(kesTeamStart)))
	{
		const char* theCStrLevelName = STRING(gpGlobals->mapname);
		if(theCStrLevelName && (strlen(theCStrLevelName) > 3))
		{
			if(!strnicmp(theCStrLevelName, "ns_", 3))
			{
				if(this->GetEntityExists(kesTeamHive))
				{
					if(this->GetEntityExists(kwsTeamCommand))
					{
						this->mMapMode = MAP_MODE_NS;
						this->PerformMapValidityCheck();
					}
				}
			}
			else if(!strnicmp(theCStrLevelName, "co_", 3))
			{
				this->mMapMode = MAP_MODE_CO;
			}
			else if(!strnicmp(theCStrLevelName, "nsc_", 4))
			{
				this->mMapMode = MAP_MODE_NSC;
			}
		}
	}
	// Check for CS entities
	else if(this->GetEntityExists(kesTerroristStart) && this->GetEntityExists(kesCounterTerroristStart))
	{
		this->mMapMode = MAP_MODE_CS;
	}
	else if(this->GetEntityExists(kesDeathMatchStart))
	{
		this->mMapMode = MAP_MODE_DM;
	}
}

bool AvHGamerules::RoamingAllowedForPlayer(CBasePlayer* inPlayer) const
{
	AvHPlayer*		thePlayer = dynamic_cast<AvHPlayer*>(inPlayer);
	bool			theRoamingAllowed = false;

	if(thePlayer)
	{
		if((thePlayer->GetPlayMode() == PLAYMODE_OBSERVER))
		{
			theRoamingAllowed = true;
		}

		const AvHTeam* theTeam = this->GetTeam(thePlayer->GetTeam());
		if(theTeam)
		{
			if(theTeam->GetPlayerCount() == 0)
			{
				theRoamingAllowed = true;
			}
		}
		else
		{
			AvHTeamNumber teamA = this->mTeamA.GetTeamNumber();
			AvHTeamNumber teamB = this->mTeamB.GetTeamNumber();
			theTeam = this->GetTeam( (thePlayer->GetTeam() == teamA ? teamB : teamA) );
			if(theTeam)
			{
				if(theTeam->GetPlayerCount() == 0)
				{
					theRoamingAllowed = true;
				}
			}
		}
	}

	return theRoamingAllowed;
}

BOOL AvHGamerules::FShouldSwitchWeapon(CBasePlayer* inPlayer, CBasePlayerItem* inWeapon)
{
	BOOL theShouldSwitch = CHalfLifeTeamplay::FShouldSwitchWeapon(inPlayer, inWeapon);
	if(theShouldSwitch)
	{
		AvHBasePlayerWeapon* theWeapon = dynamic_cast<AvHBasePlayerWeapon*>(inWeapon);
		if(theWeapon && !theWeapon->GetIsCapableOfFiring())
		{
			theShouldSwitch = false;
		}
	}
	return theShouldSwitch;
}

void AvHGamerules::MarkDramaticEvent(int inPriority, CBaseEntity* inPrimaryEntity, bool inDramatic, CBaseEntity* inSecondaryEntity) const
{
	ASSERT(inPrimaryEntity);
	short inPrimaryEntityIndex = ENTINDEX(inPrimaryEntity->edict());
	short inSecondaryEntityIndex = !inSecondaryEntity ? 0 : ENTINDEX(inSecondaryEntity->edict());

	this->MarkDramaticEvent(inPriority, inPrimaryEntityIndex, inDramatic, inSecondaryEntityIndex);
}

void AvHGamerules::MarkDramaticEvent(int inPriority, short inPrimaryEntityIndex, bool inDramatic, short inSecondaryEntityIndex) const
{
	ASSERT(inPriority >= kMinDramaticPriority);
	ASSERT(inPriority <= kMaxDramaticPriority);

	MESSAGE_BEGIN(MSG_SPEC, SVC_DIRECTOR);
		WRITE_BYTE(DIRECTOR_MESSAGE_SIZE);
		WRITE_BYTE(DRC_CMD_EVENT);
		WRITE_SHORT(inPrimaryEntityIndex);
		WRITE_SHORT(inSecondaryEntityIndex);

		int thePriority = inPriority;
		if(inDramatic)
		{
			thePriority |= DRC_FLAG_DRAMATIC;
		}

		WRITE_LONG(thePriority);
	MESSAGE_END();
}

void AvHGamerules::MarkDramaticEvent(int inPriority, short inPrimaryEntityIndex, bool inDramatic, entvars_t* inSecondaryEntity) const
{
	// Changed by mmcguire. Sometimes the attacker is NULL (e.g. when a
	// player creates a turret and then leaves the game), so we need to
	// handle that case.
	short secondaryEntityIndex = 0;

	if (inSecondaryEntity != NULL)
	{
		secondaryEntityIndex = OFFSET(inSecondaryEntity);
	}

	this->MarkDramaticEvent(inPriority, inPrimaryEntityIndex, secondaryEntityIndex);
}

void AvHGamerules::ResetEntities()
{
	// Now reset all the world entities and mark useable ones with AVH_USER3_USEABLE
	//AvHSUPrintDevMessage("FOR_ALL_BASEENTITIES: AvHGamerules::ResetEntities\n");

	FOR_ALL_BASEENTITIES();

	// Reset the entity.  Assumes that players in the ready room have already been reset recently.
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theBaseEntity);
	if(thePlayer && (thePlayer->GetPlayMode() == PLAYMODE_READYROOM))
	{
		int theUser3 = thePlayer->pev->iuser3;
		int theUser4 = thePlayer->pev->iuser4;
		int thePlayMode = thePlayer->pev->playerclass;
		int thePlayerTeam = thePlayer->pev->team;
		int theSolidType = thePlayer->pev->solid;

		thePlayer->ResetEntity();

		thePlayer->pev->iuser3 = theUser3;
		thePlayer->pev->iuser4 = theUser4;
		thePlayer->pev->playerclass = thePlayMode;
		thePlayer->pev->team = thePlayerTeam;
		thePlayer->pev->solid = theSolidType;
	}
	else
	{
		theBaseEntity->ResetEntity();
	}

	// Don't mark commander stations as useable in this case
	AvHCommandStation* theCommandStation = dynamic_cast<AvHCommandStation*>(theBaseEntity);
	if(!theCommandStation)
	{
		int	theObjectCaps = theBaseEntity->ObjectCaps();
		if(theObjectCaps & (FCAP_IMPULSE_USE | FCAP_CONTINUOUS_USE | FCAP_ONOFF_USE))
		{
			// After playing once, this is no longer zero
			if(theBaseEntity->pev->iuser3 == 0)
			{
				theBaseEntity->pev->iuser3 = AVH_USER3_USEABLE;

				// Now also mark the target entity as useable!
				if (!FStringNull(theBaseEntity->pev->target))
				{
					CBaseEntity* theTarget = NULL;

					while(theTarget = UTIL_FindEntityByTargetname(theTarget, STRING(theBaseEntity->pev->target)))
					{
						theTarget->pev->iuser3 = AVH_USER3_USEABLE;
					}
				}
			}
		}
	}
	END_FOR_ALL_BASEENTITIES();
}

void AvHGamerules::InternalResetGameRules()
{
	if(AvHNexus::isRecordingGame())
	{
		AvHNexus::cancelGame();
	}
	this->mGameStarted = false;
	this->mTimeCountDownStarted = 0;
	this->mTimeGameStarted = -1;
	this->mTimeOfLastGameTimeUpdate = -1;
    this->mTimeOfLastHLTVProxyUpdate = -1;
	this->mTimeSentCountdown = 0;
	this->mTimeLastWontStartMessageSent = 0;
	this->mStartedCountdown = false;
	this->mSentCountdownMessage = false;
	this->mTimeWorldReset = gpGlobals->time;
	this->mCombatAttackingTeamNumber = TEAM_IND;
	gSvCheatsLastUpdateTime = -1.0f;
	if(this->mLastMapChange == -1)
	{
		this->mLastMapChange = gpGlobals->time;
	}

	this->mVictoryTeam = TEAM_IND;
	this->mVictoryDraw = false;
	this->mVictoryTime = 0;
	this->mCheats.clear();
	this->mLastParticleUpdate = -1;
	this->mLastNetworkUpdate = -1;
	this->mLastWorldEntityUpdate = -1;
	this->mLastCloakableUpdate = -1;
	this->mLastVictoryUpdate = -1;
// puzl: 0001073
#ifdef USE_OLDAUTH //under UPP, we don't want to reestablish a connection with each new game
	this->mUpdatedUplink = false;
#endif

}

void AvHGamerules::CopyDataToSpawnEntity(const AvHSpawn& inSpawnEntity) const
{
	VectorCopy(inSpawnEntity.GetOrigin(), this->mSpawnEntity->pev->origin);
	VectorCopy(inSpawnEntity.GetAngles(), this->mSpawnEntity->pev->angles);
	VectorCopy(inSpawnEntity.GetAngles(), this->mSpawnEntity->pev->v_angle);
	this->mSpawnEntity->pev->team = inSpawnEntity.GetTeamNumber();
}

// Look for spawns in radius
CBaseEntity* AvHGamerules::GetRandomHiveSpawnPoint(CBaseEntity* inPlayer, const Vector& inOrigin, float inMaxDistance) const
{
	CBaseEntity*			theSpawnPoint = NULL;

	typedef vector<int>		EntityIndexListType;
	EntityIndexListType		theSpawnIndexList;

	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inPlayer);
	ASSERT(thePlayer);

	// Loop through all spawns, looking for valid spawns within range
	int theIndex = 0;
	SpawnListType::const_iterator theIter;
	for(theIter = this->mSpawnList.begin(); theIter != this->mSpawnList.end(); theIter++, theIndex++)
	{
		// Found one in range?  Add it's index to the list
		if(theIter->GetTeamNumber() == AvHTeamNumber(thePlayer->pev->team))
		{
			Vector theSpawnOrigin = theIter->GetOrigin();
			float theDistanceToSpawn = VectorDistance(theSpawnOrigin, inOrigin);
			if(theDistanceToSpawn <= inMaxDistance)
			{
				int theOffset = AvHMUGetOriginOffsetForUser3(AvHUser3(thePlayer->pev->iuser3));
				theSpawnOrigin.z += theOffset;

				if(AvHSUGetIsEnoughRoomForHull(theSpawnOrigin, AvHMUGetHull(false, inPlayer->pev->iuser3), inPlayer->edict()))
				{
					theSpawnIndexList.push_back(theIndex);
				}
			}
		}
	}

	// If index list has more then one entry
	int theNumValidSpawns = theSpawnIndexList.size();
	if(theNumValidSpawns > 0)
	{
		// Pick random index
		int theRandomIndex = RANDOM_LONG(0, theNumValidSpawns - 1);
		ASSERT(theRandomIndex >= 0);
		ASSERT(theRandomIndex < theNumValidSpawns);
		ASSERT(this->mSpawnEntity);

		// Copy data into spawn entity
		int theSpawnIndex = theSpawnIndexList[theRandomIndex];
		const AvHSpawn& theSpawn = this->mSpawnList[theSpawnIndex];
		this->CopyDataToSpawnEntity(theSpawn);

		// Return it
		theSpawnPoint = this->mSpawnEntity;
	}

	return theSpawnPoint;
}
bool AvHGamerules::CanPlayerBeacon(CBaseEntity *inPlayer) 
{
	bool result=true;
	SpawnListType::const_iterator theSpawnIter;

	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inPlayer);
	ASSERT(thePlayer);
	AvHTeamNumber thePlayerTeamNumber = thePlayer->GetTeam();
	for(theSpawnIter = this->mSpawnList.begin(); theSpawnIter != this->mSpawnList.end(); theSpawnIter++)
	{
		string theClassName = theSpawnIter->GetClassName();
		if(theClassName == kesTeamStart)
		{
			if(theSpawnIter->GetTeamNumber() == thePlayerTeamNumber && 
				VectorDistance(theSpawnIter->GetOrigin(), inPlayer->pev->origin) < BALANCE_VAR(kDistressBeaconRange))
			{
				result=false;
			}
		}
	}
	return result;
}

// puzl: 0001073
#ifdef USE_OLDAUTH
unsigned int gTimeLastUpdatedUplink = 0;
void AvHGamerules::UpdateUplink()
{
	#ifdef AVH_SECURE_PRERELEASE_BUILD
	avh_uplink.value = 1;
	#endif

	// If authentication is enabled
	if(!this->mUpdatedUplink && (avh_uplink.value > 0))
	{
		// Only allow it once every day -> 500 servers == num queries per hour = 500*75k =  1,500k per hour -> just over a 1 gig a month
		unsigned int theCurrentSystemTime = AvHSUTimeGetTime();
		int theUpdateUplinkInterval = (60*60*1000)*24;

		#ifdef AVH_SECURE_PRERELEASE_BUILD
		theUpdateUplinkInterval = (60*60*1000)/30; // every 30 minutes or so while testing
		#endif

        #ifdef DEBUG
        theUpdateUplinkInterval = 0;
        #endif

		if((gTimeLastUpdatedUplink == 0) || (theCurrentSystemTime > (gTimeLastUpdatedUplink + theUpdateUplinkInterval)))
		{
			// Initialize it
			ALERT(at_logged, "Contacting www.natural-selection.org...\n");
			this->InitializeAuthentication();
			//this->DisplayVersioning();
		}
		else
		{
			//ALERT(at_logged, "You must wait longer before uplinking again.\n");
		}
	}

	// If it just turned off, clear auth masks
	if(this->mUpdatedUplink && !avh_uplink.value)
	{
		gAuthMaskList.clear();
		ALERT(at_logged, "Authentication disabled.\n");
	}

	this->mUpdatedUplink = avh_uplink.value;
}
#endif


edict_t* AvHGamerules::SelectSpawnPoint(CBaseEntity* inPlayer, const string& inSpawnEntityName) const
{
	bool theDone = false;
	edict_t* theResult = NULL;
	CBaseEntity* theSpot = NULL;

	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inPlayer);
	ASSERT(thePlayer);

	// Handle alien spawns differently than others.
	AvHTeamNumber theTeamNumber = thePlayer->GetTeam();
	const AvHTeam* theTeam = this->GetTeam(theTeamNumber);
	if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN) && (inSpawnEntityName == kesTeamStart))
	{
		// Get a random active hive
		AvHHive* theHive = AvHSUGetRandomActiveHive(theTeamNumber);
		if(theHive)
		{
			theSpot = this->GetRandomHiveSpawnPoint(inPlayer, theHive->pev->origin, theHive->GetMaxSpawnDistance());
		}

		// If we still didn't find one and cheats are on, try a spawn not near a hive
		if(!theSpot && this->GetCheatsEnabled())
		{
			theSpot = this->SelectRandomSpawn(inPlayer, inSpawnEntityName);
		}
	}
	else
	{
		theSpot = this->SelectRandomSpawn(inPlayer, inSpawnEntityName);
	}

	if(!FNullEnt(theSpot))
	{
		int theOffset = AvHMUGetOriginOffsetForUser3(AvHUser3(thePlayer->pev->iuser3));
		Vector theSpawnOrigin = theSpot->pev->origin;
		theSpawnOrigin.z += theOffset;

		if(AvHSUGetIsEnoughRoomForHull(theSpawnOrigin, AvHMUGetHull(false, inPlayer->pev->iuser3), inPlayer->edict()))
		{
			theResult = theSpot->edict();
			theDone = true;
		}
	}

	if(!theDone)
	{
		char theErrorString[256];
		sprintf(theErrorString, "AvHGamerules::SelectSpawnPoint(): all spawns full\n");
		ALERT(at_logged, theErrorString);
		theResult = NULL;
	}

	return theResult;
}

void AvHGamerules::UpdateHLTVProxy()
{
    // Check if HLTV proxy is connected to server
    bool theHLTVProxyConnected = false;

    // Every so often, send out team for all players (needed for late joiners)
    for (int i = 1; i <= gpGlobals->maxClients; i++ )
    {
        CBasePlayer *plr = (CBasePlayer*)UTIL_PlayerByIndex( i );
        if ( plr && (plr->pev->flags & FL_PROXY))
        {
            theHLTVProxyConnected = true;
            break;
        }
    }

    // Update proxy if connected
    if(theHLTVProxyConnected)
    {
        const float kHLTVProxyUpdateTime = 6.0f;

        if((this->mTimeOfLastHLTVProxyUpdate == -1) || (gpGlobals->time > (this->mTimeOfLastHLTVProxyUpdate + kHLTVProxyUpdateTime)))
        {
            //char theMessage[256];
            //sprintf(theMessage, "AvHGamerules::UpdateHLTVProxy...\n");
            //ALERT(at_console, theMessage);

            // Every so often, send out team for all players (needed for late joiners)
            for (int i = 1; i <= gpGlobals->maxClients; i++ )
            {
                CBasePlayer *plr = (CBasePlayer*)UTIL_PlayerByIndex( i );
                if ( plr && GetGameRules()->IsValidTeam( plr->TeamID() ) )
                {
					NetMsgSpec_TeamInfo( plr->entindex(), plr->TeamID() );
                }
            }

            // Send messages to HLTV viewers
			NetMsgSpec_SetGammaRamp( GetGameRules()->GetMapGamma() );

            this->mTimeOfLastHLTVProxyUpdate = gpGlobals->time;
        }
    }
}

void AvHGamerules::UpdatePlaytesting()
{
	if(this->GetGameStarted() && !this->GetIsCombatMode())
	{
		const int theActiveNodesMessageUpdateTime = BALANCE_VAR(kActiveNodesMessageUpdateTime);
		if((theActiveNodesMessageUpdateTime > 0) && !this->GetIsTournamentMode())
		{
			if((this->mTimeOfLastPlaytestUpdate == -1) || (gpGlobals->time > (this->mTimeOfLastPlaytestUpdate + theActiveNodesMessageUpdateTime)))
			{
				int theTeamATowers = 0;
				int theTeamBTowers = 0;

				FOR_ALL_BASEENTITIES();
				AvHResourceTower* theResourceTower = dynamic_cast<AvHResourceTower*>(theBaseEntity);
				if(theResourceTower && theResourceTower->GetIsActive())
				{
					if(theResourceTower->pev->team == this->mTeamA.GetTeamNumber())
					{
						theTeamATowers++;
					}
					else if(theResourceTower->pev->team == this->mTeamB.GetTeamNumber())
					{
						theTeamBTowers++;
					}
				}
				END_FOR_ALL_BASEENTITIES();

				// Count how many active towers each team has, and tell the world
				char* theMessage = UTIL_VarArgs( "Active resource nodes:  TeamA: %d  TeamB: %d\n", theTeamATowers, theTeamBTowers);
				UTIL_ClientPrintAll(HUD_PRINTTALK, theMessage);
				UTIL_LogPrintf(theMessage);

				this->mTimeOfLastPlaytestUpdate = gpGlobals->time;
			}
		}
	}
}

edict_t* AvHGamerules::SelectSpawnPoint(CBaseEntity* inPlayer) const
{
	const char*		theSpawnName = kesReadyRoomStart;
	edict_t*		theResult = NULL;

	// Get name of spawn entity from player
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inPlayer);
	if(thePlayer)
	{
		// Set name of spawn point accordingly
		theSpawnName = this->GetSpawnEntityName(thePlayer);
	}

	theResult = this->SelectSpawnPoint(inPlayer, theSpawnName);

   	return theResult;
}

const char*	AvHGamerules::SetDefaultPlayerTeam(CBasePlayer *pPlayer)
{
	// By default, do nothing.  We start in the ready room, we don't join a team
	return NULL;
}

CBaseEntity* AvHGamerules::SelectRandomSpawn(CBaseEntity* inPlayer, const string& inSpawnName) const
{
	CBaseEntity* theSpawn = NULL;
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inPlayer);
	ASSERT(thePlayer);
	AvHTeamNumber thePlayerTeamNumber = thePlayer->GetTeam();

	// Count how many spawns of this type
	int theNumSpawns = 0;

	SpawnListType::const_iterator theSpawnIter;

	for(theSpawnIter = this->mSpawnList.begin(); theSpawnIter != this->mSpawnList.end(); theSpawnIter++)
	{
		string theClassName = theSpawnIter->GetClassName();
		if(theClassName == inSpawnName)
		{
			if(theSpawnIter->GetTeamNumber() == thePlayerTeamNumber)
			{
				theNumSpawns++;
			}
		}
	}

	bool theSuccess = false;
	int theSpawnListSize = this->mSpawnList.size();
	if(theSpawnListSize > 0)
	{
        int theNumTries = 0;

		do
		{
            int theOffset = RANDOM_LONG(0, theSpawnListSize-1);

			//theSpawnIter = &this->mSpawnList[theCurrentOffset];
			const AvHSpawn& theSpawn = this->mSpawnList[theOffset];
			if(theSpawn.GetClassName() == inSpawnName)
			{
				if(theSpawn.GetTeamNumber() == thePlayerTeamNumber)
				{
					// Copy data into current spawn point
					ASSERT(this->mSpawnEntity);

					this->CopyDataToSpawnEntity(theSpawn);

					if(IsSpawnPointValid(inPlayer, this->mSpawnEntity))
					{
						theSuccess = true;
					}
				}
			}

        // Put a limit to the number of tries, to avoid an infinite loop when mapper didn't create the right type of spawns
		} while((theNumTries++ < 100) && !theSuccess);
	}

	if(theSuccess)
	{
		theSpawn = this->mSpawnEntity;
	}
	else
	{
		theSpawn = NULL;
	}

	return theSpawn;
}

void AvHGamerules::SetGameStarted(bool inGameStarted)
{
	if(!this->mGameStarted && inGameStarted)
	{
		FireTargets(ktGameStartedStatus, NULL, NULL, USE_TOGGLE, 0.0f);
		AvHNexus::startGame();
	}
	this->mGameStarted = inGameStarted;
	this->mTimeGameStarted = gpGlobals->time;

	// Choose a random defending team in Combat
	if(this->GetIsCombatMode())
	{
		int theTeamIndex = 1;// + (rand()%2);
		AvHTeamNumber theAttackingTeamNumber = AvHTeamNumber(theTeamIndex);
		this->mCombatAttackingTeamNumber = theAttackingTeamNumber;
	}

	this->mTeamA.SetGameStarted(inGameStarted);
	this->mTeamB.SetGameStarted(inGameStarted);

   	// SCRIPTENGINE: OnStart
	if (this->GetIsScriptedMode())
		gLUA->OnStarted();
	// :SCRIPTENGINE
}

void AvHGamerules::SendMOTDToClient( edict_t *client )
{
	// read from the MOTD.txt file
	int				length, char_count = 0;
	char*			pFileList;
	char*			aFileList = pFileList = (char*)LOAD_FILE_FOR_ME( kMOTDName, &length );

	AvHPlayer*		thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(client));
	ASSERT(thePlayer);

	// send the server name
	NetMsg_ServerName( thePlayer->pev, string( CVAR_GET_STRING("hostname") ) );

	UTIL_ShowMessage(pFileList, thePlayer);

	FREE_FILE( aFileList );
}

int AvHGamerules::GetNumberOfPlayers() const
{
	int theNumberOfPlayers = 0;

	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
	// GetIsRelevant()?
	//if(!inPlayingGame || (inEntity->GetPlayMode() == PLAYMODE_PLAYING))
	//{
	if(UTIL_IsValidEntity(theEntity->edict()))
	{
		theNumberOfPlayers++;
	}
	//}
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName);

	return theNumberOfPlayers;
}

char gLastKnownMapName[256] = "";

void AvHGamerules::Think(void)
{
	PROFILE_START()

	float theTime = gpGlobals->time;
	float theTimePassed = gpGlobals->frametime;

	if(this->mFirstUpdate)
	{
		const char* theCStrLevelName = STRING(gpGlobals->mapname);
		bool theNewMap = !gLastKnownMapName || !theCStrLevelName || strcmp(theCStrLevelName, gLastKnownMapName);

		PROFILE_START();
		this->PostWorldPrecacheReset(theNewMap);
		PROFILE_END(kUpdatePrecacheResetProfile);

		strcpy(gLastKnownMapName, theCStrLevelName);

		// Tell all HUDs to reset
		NetMsg_GameStatus_State( kGameStatusReset, this->mMapMode );

   		// SCRIPTENGINE: Execute OnStart
		if (this->GetIsScriptedMode())
			gLUA->OnStart();

		this->mFirstUpdate = false;
	}

	// Handle queued network messages
	#ifdef USE_NETWORK_METERING
	const float kNetworkUpdateInterval = .1f;
	if((this->mLastNetworkUpdate == -1) || (theTime > (this->mLastNetworkUpdate + kNetworkUpdateInterval)))
	{
		PROFILE_START();
		int theBytesPerSecond = 100000;//(int)(avh_networkmeterrate.value);
		NetworkMeter::Instance()->SetBufferAmount(theBytesPerSecond);
		NetworkMeter::Instance()->ProcessQueuedMessages();
		PROFILE_END(kUpdateNetworkMeterProfile);
	}
	#endif

	#ifdef PROFILE_BUILD
	kProfileRunConfig = (int)(avh_performance.value);
	#endif

	PROFILE_START();
	AvHNexus::processResponses();
	this->RecalculateHandicap();
// puzl: 0001073
#ifdef USE_OLDAUTH
	this->UpdateUplink();
#endif
	this->UpdatePlaytesting();
    this->UpdateHLTVProxy();
	PROFILE_END(kUpdateMisc);

	const float kWorldEntityUpdateInterval = 1.0f;
	if((this->mLastWorldEntityUpdate == -1) || (theTime > (this->mLastWorldEntityUpdate + kWorldEntityUpdateInterval)))
	{
		if(GET_RUN_CODE(1))
		{
			// Update world entities
			PROFILE_START()
			this->UpdateWorldEntities();
			this->mLastWorldEntityUpdate = theTime;
			PROFILE_END(kUpdateWorldEntitiesProfile)
		}

		// Don't need to update cheats every tick, as they can be expensive
		this->UpdateCheats();
	}

	this->mMiniMap.Process();

	PROFILE_START();
	g_VoiceGameMgr.Update(gpGlobals->frametime);
	PROFILE_END(kUpdateVoiceManagerProfile);

	const float kParticleUpdateInterval = 2.0f;
	if((this->mLastParticleUpdate == -1) || (theTime > (this->mLastParticleUpdate + kParticleUpdateInterval)))
	{
		if(GET_RUN_CODE(2))
		{
			PROFILE_START();
			AvHParticleSystemManager::Instance()->Update(theTimePassed);
			this->mLastParticleUpdate = theTime;
			PROFILE_END(kUpdateParticleSystemManager)
		}

		#ifdef DEBUG
		int theUseCaching = BALANCE_VAR(kDebugServerCaching);
		if(theUseCaching)
		{
			PROFILE_START()

			char theMessage[128];
			int theReturnTotal = kNumReturn0 + kNumReturn1;
			float theNumReturnedPercentage = 0.0f;
			float theNumCachedPercentage = 0.0f;
			float theComputedPercentage = 0.0f;

			if(theReturnTotal > 0)
			{
				theNumReturnedPercentage = kNumReturn1/((float)theReturnTotal);
				theNumCachedPercentage = kNumCached/((float)theReturnTotal);
				theComputedPercentage = kNumComputed/((float)theReturnTotal);
			}

			kPVSCoherencyTime = BALANCE_VAR(kDebugPVSCoherencyTime);

			sprintf(theMessage, "Percentage entities propagated: %f (%d/%d) (compute percentage: %f) (pvs time: %f)\n", theNumReturnedPercentage, kNumReturn1, theReturnTotal, theComputedPercentage, kPVSCoherencyTime);
			UTIL_LogPrintf(theMessage);

			PROFILE_END(kUpdateDebugShowEntityLog)
		}
		#endif

		kServerFrameRate = 0;
		kNumReturn0 = kNumReturn1 = kNumCached = kNumComputed = 0;
	}
	else
	{
		int a = 0;
	}

	this->UpdateServerCommands();
	this->UpdateGameTime();

    if(GET_RUN_CODE(4))
    {
	    if(!this->GetGameStarted())
	    {
		    if(!this->GetIsTournamentMode())
		    {
			    this->UpdateTimeLimit();
		    }

		    this->UpdateCountdown(theTime);

		    PROFILE_START();
		    // Try to join any players that are waiting for a spot (bypass cost of reinforcements before game starts)
		    // Players are put into reinforcement mode if there are no available spawns
		    FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
			    if(theEntity->GetPlayMode() == PLAYMODE_AWAITINGREINFORCEMENT)
			    {
				    AvHTeamNumber theTeamNumber = theEntity->GetTeam();
				    if(theTeamNumber != TEAM_IND)
				    {
					    this->AttemptToJoinTeam(theEntity, theTeamNumber, false);
				    }
			    }
		    END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
		    PROFILE_END(kUpdateReinforcementsProfile);
	    }
	    else
        {
		    //this->ProcessTeamUpgrades();
		    this->UpdateEntitiesUnderAttack();

		    // Has a side won?
		    if(this->mVictoryTeam != TEAM_IND)
		    {
			    // Let players bask in their victory, don't update the world normally
			    PROFILE_START()
			    this->UpdateVictory();
			    PROFILE_END(kUpdateVictoryProfile)
		    }
		    else
		    {
			    PROFILE_START()
			    this->mTeamA.Update();
			    PROFILE_END(kTeamOneUpdate)

			    PROFILE_START()
			    this->mTeamB.Update();
			    PROFILE_END(kTeamTwoUpdate)
		    }

		    PROFILE_START()
		    // Don't update every frame for performance reasons
		    const float kVictoryUpdateInterval = 1.0f;
		    if((this->mLastVictoryUpdate == -1) || (theTime > (this->mLastVictoryUpdate + kVictoryUpdateInterval)))
		    {
			    this->UpdateVictoryStatus();
			    this->mLastVictoryUpdate = theTime;
		    }
		    PROFILE_END(kUpdateVictoryStatusProfile)
	    }
    }

    gServerTick++;
	if((gServerTick % 5) == 0)
	{
		gServerUpdate = true;
	}
	else
	{
		gServerUpdate = false;
	}
    PROFILE_END(kUpdateGamerulesProfile)

}

void AvHGamerules::RegisterServerVariable(const char* inName)
{
    mServerVariableList.push_back(inName);
}

int AvHGamerules::GetNumServerVariables() const
{
    return mServerVariableList.size();
}

const std::string& AvHGamerules::GetServerVariable(int i) const
{
    return mServerVariableList[i];
}

bool AvHGamerules::GetIsEntityUnderAttack(int inEntityIndex) const
{
	bool theEntityIsUnderAttack = false;

	// If entity is in list, it's being attacked
	for(EntityUnderAttackListType::const_iterator theIter = this->mEntitiesUnderAttack.begin(); theIter != this->mEntitiesUnderAttack.end(); theIter++)
	{
		if(inEntityIndex == theIter->first)
		{
			theEntityIsUnderAttack = true;
			break;
		}
	}

	return theEntityIsUnderAttack;
}

void AvHGamerules::TriggerAlert(AvHTeamNumber inTeamNumber, AvHAlertType inAlertType, int inEntIndex, AvHMessageID inMessageID)
{
	AvHTeam* theTeam = this->GetTeam(inTeamNumber);
	if(theTeam)
	{
		// Don't play audio alerts too often.  This also allows neat tactics where players can time strikes to prevent the other team from instant notification of an alert, ala RTS
		float theTimeOfLastAlert = -1;
        AvHAlertType theLastAlertType = ALERT_NONE;

        const AvHAlert* theLastAlert = theTeam->GetLastAudioAlert();
        if(theLastAlert)
        {
            theTimeOfLastAlert = theLastAlert->GetTime();
            theLastAlertType = theLastAlert->GetAlertType();
        }

        // Play alerts when enough time has passed
        bool theIsRepeatAlert = (inAlertType == theLastAlertType);
        const float kBaseAlertInterval = theTeam->GetAudioAlertInterval();
        float theAlertInterval = theIsRepeatAlert ? 2.0f*kBaseAlertInterval : kBaseAlertInterval;

        bool theAlertIntervalTimePassed = (gpGlobals->time - theTimeOfLastAlert > theAlertInterval);
		bool thePlayAlertSound = ((theTimeOfLastAlert == -1) || theAlertIntervalTimePassed);

        // Always play urgent alerts, but only when they're new (so you don't get 'hive under attack' twice in quick succession)
        if(AvHSUGetIsUrgentAlert(inAlertType) && (!AvHSUGetIsOftRepeatedAlert(inAlertType) && !theIsRepeatAlert))
        {
            thePlayAlertSound = true;
        }

		// Add it, remembering if we triggered sound for it
		AvHAlert theNewAlert(inAlertType, gpGlobals->time, inEntIndex);
		theNewAlert.SetPlayedAudio(thePlayAlertSound);
		theTeam->AddAlert(theNewAlert, inMessageID);

		if(thePlayAlertSound)
		{
			AvHHUDSound theSound = HUD_SOUND_INVALID;
			CBaseEntity* theAlertEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(inEntIndex));

			// If team type is marines, play sound for commander
			AvHClassType theTeamType = theTeam->GetTeamType();
			if(theTeamType == AVH_CLASS_TYPE_MARINE)
			{
				// Get commander for team, if any
				int theCommanderIndex = theTeam->GetCommander();

				if(this->GetIsTesting())
				{
					theCommanderIndex = 1;
				}

				if(theCommanderIndex != -1)
				{
					CBaseEntity* theBaseEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theCommanderIndex));
					AvHPlayer* theCommander = dynamic_cast<AvHPlayer*>(theBaseEntity);
					if(theCommander /*&& thePlayAudioAlert*/)
					{
						if(inAlertType == ALERT_RESEARCH_COMPLETE)
						{
							theSound = HUD_SOUND_MARINE_RESEARCHCOMPLETE;
						}
						else if(inAlertType == ALERT_UPGRADE_COMPLETE)
						{
							theSound = HUD_SOUND_MARINE_UPGRADE_COMPLETE;
						}
						else if(inAlertType == ALERT_LOW_RESOURCES)
						{
							theSound = HUD_SOUND_MARINE_RESOURCES_LOW;
						}
						// Special sound for the CC, always play it no matter where the commander is
						else if(inAlertType == ALERT_UNDER_ATTACK)
						{
							if(dynamic_cast<AvHCommandStation*>(theAlertEntity))
							{
								theSound = HUD_SOUND_MARINE_CCUNDERATTACK;
							}
							else
							{
								theSound = HUD_SOUND_MARINE_BASE_UNDER_ATTACK;
							}
						}
						else if(inAlertType == ALERT_ORDER_COMPLETE)
						{
							theSound = HUD_SOUND_ORDER_COMPLETE;
						}
						// Check for positional alerts (only send them if they are out of commander's sight)
						else
						{
							if(theAlertEntity)
							{
								vec3_t theDistanceToAlert;
								VectorSubtract(theCommander->pev->origin, theAlertEntity->pev->origin, theDistanceToAlert);
								float theXYDistanceToAlert = theDistanceToAlert.Length2D();

								if(theXYDistanceToAlert > this->GetMapExtents().GetTopDownCullDistance())
								{
									if(inAlertType == ALERT_PLAYER_ENGAGE)
									{
										theSound = HUD_SOUND_MARINE_SOLDIER_UNDER_ATTACK;
									}
									else if(inAlertType == ALERT_UNDER_ATTACK)
									{
										theSound = HUD_SOUND_MARINE_BASE_UNDER_ATTACK;
									}
									else if(inAlertType == ALERT_SOLDIER_NEEDS_AMMO)
									{
										theSound = HUD_SOUND_MARINE_NEEDS_AMMO;
									}
									else if(inAlertType == ALERT_SOLDIER_NEEDS_HEALTH)
									{
										theSound = HUD_SOUND_MARINE_NEEDS_HEALTH;
									}
									else if(inAlertType == ALERT_ORDER_NEEDED)
									{
										theSound = HUD_SOUND_MARINE_NEEDS_ORDER;
									}
									else if(inAlertType == ALERT_PLAYER_DIED)
									{
										theSound = HUD_SOUND_MARINE_SOLDIER_LOST;
									}
									else if(inAlertType == ALERT_SENTRY_FIRING)
									{
										theSound = HUD_SOUND_MARINE_SENTRYFIRING;
									}
									else if(inAlertType == ALERT_SENTRY_DAMAGED)
									{
										theSound = HUD_SOUND_MARINE_SENTRYDAMAGED;
									}
								}
							}
						}

						if(theSound != HUD_SOUND_INVALID)
						{
							if (theAlertEntity != NULL)
                            {
                                // Added by mmcguire.
                                theCommander->PlayHUDSound(theSound, theAlertEntity->pev->origin[0], theAlertEntity->pev->origin[1]);
                            }
                            else
                            {
                                theCommander->PlayHUDSound(theSound);
                            }
						}
					}
				}
				else
				{
					// Play "command station under attack" sound for all players on team
					if(inAlertType == ALERT_UNDER_ATTACK)
					{
						if(dynamic_cast<AvHCommandStation*>(theAlertEntity))
						{
							// For all players on this team, play the sound
							FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
								if((theEntity->pev->team == inTeamNumber) || this->GetIsTesting() || (theEntity->GetIsSpectatingTeam(inTeamNumber)))
								{
									theEntity->PlayHUDSound(HUD_SOUND_MARINE_CCUNDERATTACK);
								}
							END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
						}
					}
				}
			}
			// Else it's aliens, play hive under attack sound
			else if(theTeamType == AVH_CLASS_TYPE_ALIEN)
			{
				// When adding alien alerts, don't always play audio notification, it gets annoying
				if(thePlayAlertSound)
				{
					// Tell everyone where attack is
					if(inAlertType == ALERT_PLAYER_ENGAGE)
					{
						theSound = HUD_SOUND_ALIEN_LIFEFORM_ATTACK;
					}
					else if(inAlertType == ALERT_UNDER_ATTACK)
					{
						// If it's a resource tower
						if(theAlertEntity)
						{
							switch(theAlertEntity->pev->iuser3)
							{
							case AVH_USER3_ALIENRESTOWER:
								theSound = HUD_SOUND_ALIEN_RESOURCES_ATTACK;
								break;
							case AVH_USER3_HIVE:
								theSound = HUD_SOUND_ALIEN_HIVE_ATTACK;
								break;
							default:
								theSound = HUD_SOUND_ALIEN_STRUCTURE_ATTACK;
								break;
							}
						}
					}
					else if(inAlertType == ALERT_HIVE_COMPLETE)
					{
						theSound = HUD_SOUND_ALIEN_HIVE_COMPLETE;
					}
					else if(inAlertType == ALERT_HIVE_DYING)
					{
						theSound = HUD_SOUND_ALIEN_HIVE_DYING;
					}
					else if(inAlertType == ALERT_LOW_RESOURCES)
					{
						theSound = HUD_SOUND_ALIEN_RESOURCES_LOW;
					}
					else if(inAlertType == ALERT_NEW_TRAIT)
					{
						theSound = HUD_SOUND_ALIEN_NEW_TRAIT;
					}

					if(theSound != HUD_SOUND_INVALID)
					{
						int a = 0;

						// For all players on this team, play the sound
						FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
							if((theEntity->pev->team == inTeamNumber) || this->GetIsTesting() || (theEntity->GetIsSpectatingTeam(inTeamNumber)))
							{
								theEntity->PlayHUDSound(theSound);
							}
						END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
					}
				}
			}
			else
			{
				ASSERT(false);
			}
		}

		// Add entity to our list of entities that are under attack
		if(((inAlertType == ALERT_UNDER_ATTACK) || (inAlertType == ALERT_PLAYER_ENGAGE) || (inAlertType == ALERT_HIVE_DYING)) && (inEntIndex > 0))
		{
			// This will update current time longer if continually attacked
			const float kUnderAttackDuration = 5.0f;
			this->mEntitiesUnderAttack[inEntIndex] = gpGlobals->time + kUnderAttackDuration;
		}
	}
}

bool AvHGamerules::GetIsCheatEnabled(const string& inCheatName) const
{
	bool theCheatIsEnabled = false;

	bool theAllowCheats = this->GetCheatsEnabled();

	#ifdef AVH_PLAYTEST_BUILD
	theAllowCheats = true;
	#endif

	if(theAllowCheats)
	{
		StringList::const_iterator theIter = std::find(this->mCheats.begin(), this->mCheats.end(), inCheatName);
		if(theIter != this->mCheats.end())
		{
			theCheatIsEnabled = true;
		}
	}

	return theCheatIsEnabled;
}

void AvHGamerules::SetCheatEnabled(const string& inCheatName, bool inEnabledState)
{
	if(this->GetCheatsEnabled())
	{
		StringList::iterator theIter = std::find(this->mCheats.begin(), this->mCheats.end(), inCheatName);
		if(theIter == this->mCheats.end())
		{
			if(inEnabledState)
			{
				this->mCheats.push_back(inCheatName);
			}
		}
		else
		{
			if(!inEnabledState)
			{
				this->mCheats.erase(theIter);
			}
		}
	}
}

void AvHGamerules::UpdateCheats()
{
	if(this->GetCheatsEnabled())
	{
		// If bigdig is enabled
		if(this->GetIsCheatEnabled(kcBigDig))
		{
			// Run through all buildables, set them to complete
			//AvHSUPrintDevMessage("FOR_ALL_BASEENTITIES: AvHGamerules::UpdateCheats\n");

			FOR_ALL_BASEENTITIES();
				AvHBuildable* theBuildable = dynamic_cast<AvHBuildable*>(theBaseEntity);
				if(theBuildable && !theBuildable->GetHasBeenBuilt() && !GetHasUpgrade(theBaseEntity->pev->iuser4, MASK_RECYCLING) && (theBaseEntity->pev->team != TEAM_IND))
				{
					theBuildable->SetConstructionComplete(true);
				}
//				AvHDeployedTurret* theTurret = dynamic_cast<AvHDeployedTurret*>(theBaseEntity);
//				if(theTurret && !theTurret->GetIsBuilt())
//				{
//					theTurret->SetConstructionComplete();
//				}
			END_FOR_ALL_BASEENTITIES();
		}
	}
}

void AvHGamerules::UpdateCountdown(float inTime)
{
	if (this->GetIsScriptedMode())
	{
		// this->SetGameStarted(true);
		// TODO: SCRIPTENGINE START
		return;
	}

	const float kTimeWontStartInterval = 8.0f;
	int kSecondsToCountdown = 5;

	#ifdef DEBUG
	kSecondsToCountdown = 1;
	#endif

	// Time to start counting down?
	if(this->ReadyToStartCountdown())
	{
		if(!this->mStartedCountdown)
		{
			//FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
			//	theEntity->SendMessageNextThink(kGameStarting);
			//END_FOR_ALL_ENTITIES(kAvHPlayerClassName)

			// If there is at least one person on one team, start counting
			this->mTimeCountDownStarted = inTime;
			this->mStartedCountdown = true;

			// Reset server and respawn everyone
			this->ResetGame(true);

			this->mSavedTimeCountDownStarted = this->mTimeCountDownStarted;
		}
	}
	else
	{
		if(!this->GetIsTrainingMode())
		{
			if((this->mTimeLastWontStartMessageSent == 0) || (inTime > (this->mTimeLastWontStartMessageSent + kTimeWontStartInterval)))
			{
				const char* theMessage = kGameWontStart;
				if(this->GetIsTournamentMode())
				{
					theMessage = kGameWontStartUntilReady;
				}

				FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
					theEntity->SendMessageOnce(theMessage, TOOLTIP);
				END_FOR_ALL_ENTITIES(kAvHPlayerClassName)

				this->mTimeLastWontStartMessageSent = inTime;
			}
		}
	}

	if(this->mStartedCountdown)
	{
		if(inTime - this->mTimeCountDownStarted > (avh_countdowntime.value*60 - kSecondsToCountdown) && !this->mSentCountdownMessage)
		{
			// Send update to everyone
			NetMsg_UpdateCountdown( kSecondsToCountdown );

			this->mTimeSentCountdown = gpGlobals->time;
			this->mSentCountdownMessage = true;
		}

		if(this->mSentCountdownMessage)
		{
			if(gpGlobals->time - this->mTimeSentCountdown >= kSecondsToCountdown)
			{
				this->SetGameStarted(true);
			}
		}
	}
}

void AvHGamerules::UpdateEntitiesUnderAttack()
{
	// Expire entities under attack
	for(EntityUnderAttackListType::iterator theIter = this->mEntitiesUnderAttack.begin(); theIter != this->mEntitiesUnderAttack.end(); /* no increment*/)
	{
		if(gpGlobals->time >= theIter->second)
		{
			EntityUnderAttackListType::iterator theTempIter = theIter;
			theTempIter++;
			this->mEntitiesUnderAttack.erase(theIter);
			theIter = theTempIter;
		}
		else
		{
			theIter++;
		}
	}
}

void AvHGamerules::SendGameTimeUpdate(bool inReliable)
{
	//  Send down game time to clients periodically
	NetMsg_GameStatus_Time( kGameStatusGameTime, this->mMapMode, this->GetGameTime(), this->GetTimeLimit(), this->mCombatAttackingTeamNumber, inReliable );

	this->mTimeOfLastGameTimeUpdate = gpGlobals->time;
}

void AvHGamerules::UpdateGameTime()
{
	// Send periodic game time updates to everyone (needed for HLTV spectators)
	if(this->GetGameStarted() && (this->GetVictoryTeam() == TEAM_IND))
	{
		// Only reason why this isn't longer is to make sure late-joiners see time on scoreboard after reasonable period, without causing needless bandwidth
		const float kGameTimeUpdateInterval = 8;
		if((this->mTimeOfLastGameTimeUpdate == -1) || (gpGlobals->time > (this->mTimeOfLastGameTimeUpdate + kGameTimeUpdateInterval)))
		{
			this->SendGameTimeUpdate();
		}
	}
}


void AvHGamerules::UpdateScripts()
{
	const float kUpdateScriptsInterval = 1.0f;

	// If server scripts are enabled
	if(CVAR_GET_FLOAT(kvServerScripts))
	{
//		const char* theCStrLevelName = STRING(gpGlobals->mapname);
//		if(theCStrLevelName && !FStrEq(theCStrLevelName, ""))
//		{
//			string theLevelName = theCStrLevelName;
//			string theLevelScript = string(kModDirectory) + string("/") + kScriptsDirectory + string("/") + theLevelName + string(".bin");
//
//			if((this->mTimeUpdatedScripts == -1) || (gpGlobals->time > this->mTimeUpdatedScripts + kUpdateScriptsInterval))
//			{
//				// Execute the script
//				AvHLUADoFile(theLevelScript.c_str());
//			}
//
//		}
		AvHScriptManager::Instance()->Update(gpGlobals->time);
	}
}

void AvHGamerules::UpdateServerCommands()
{
	//float theAirAccelerate = CVAR_GET_FLOAT("sv_airaccelerate");
	//float theAirMove = CVAR_GET_FLOAT("sv_airmove");

	// TODO: Disguises these strings somehow to prevent hacking?
	SERVER_COMMAND("sv_airaccelerate 10\n");
	SERVER_COMMAND("sv_airmove 1\n");
}

void AvHGamerules::UpdateTimeLimit()
{
	// Only update time limit after world has been reset.  Give some time to make sure all clients get the game status message.
	#ifdef DEBUG
	const float kIntermissionTime = 2;
	#else
	const float kIntermissionTime = 5;
	#endif

	if(gpGlobals->time > (this->mTimeWorldReset + kIntermissionTime))
	{
		float theTimeLimit = timelimit.value * 60;

		if((theTimeLimit > 0) && (gpGlobals->time >= (theTimeLimit + this->mLastMapChange)))
		{
			this->ChangeLevel();
		}
	}
}

void AvHGamerules::UpdateVictory()
{
	if((gpGlobals->time - this->mVictoryTime) > (kVictoryIntermission + kMaxPlayers/kResetPlayersPerSecond))
	{
		this->ResetGame();
	}
}

void AvHGamerules::UpdateVictoryStatus(void)
{
	bool theCheckVictoryWithCheats = !this->GetCheatsEnabled() || this->GetIsCheatEnabled(kcEndGame1) || this->GetIsCheatEnabled(kcEndGame2);

	// SCRIPTENGINE VICTORY
	if (this->GetIsScriptedMode())
	{
		AvHObjectiveState teamAstate, teamBstate;
		teamAstate = this->mTeamA.GetObjectiveManager()->GetObjectivesState();
		teamBstate = this->mTeamB.GetObjectiveManager()->GetObjectivesState();
		if (teamAstate != OBJECTIVE_INDETERMINED || teamBstate != OBJECTIVE_INDETERMINED)
		{
			// one team is victorious
			this->mVictoryTime = gpGlobals->time;
			if (teamAstate == teamBstate)
			{
				this->mVictoryTeam = TEAM_SPECT;
				this->mVictoryDraw = true;
			}
			else if (teamAstate == OBJECTIVE_COMPLETED || teamBstate == OBJECTIVE_FAILED)
				this->mVictoryTeam = this->mTeamA.GetTeamNumber();
			else if (teamAstate == OBJECTIVE_FAILED || teamBstate == OBJECTIVE_COMPLETED)
				this->mVictoryTeam = this->mTeamB.GetTeamNumber();
		}
		else
		{
			// Execute LUA callback OnVictoryCheck
			AvHTeamNumber vicTeam = gLUA->OnVictoryCheck();
			if (vicTeam != TEAM_IND)
			{
				this->mVictoryTime = gpGlobals->time;
				this->mVictoryTeam = vicTeam;
			}
		}
		// Execute LUA callback OnVictory
		if (this->mVictoryTeam != TEAM_IND)
			gLUA->OnVictory(this->mVictoryTeam);

	} 
	else
	if((this->mVictoryTeam == TEAM_IND) && this->mGameStarted && theCheckVictoryWithCheats && !this->GetIsTrainingMode())
	{
		char* theVictoryMessage = NULL;
		bool theTeamALost = this->mTeamA.GetHasTeamLost();
		bool theTeamBLost = this->mTeamB.GetHasTeamLost();
		bool theDrawGame = false;

		int theTeamAPlayers = this->mTeamA.GetPlayerCount();
		int theTeamBPlayers = this->mTeamB.GetPlayerCount();

		bool theTimeLimitHit = false;
		int theTimeLimitSeconds = this->GetTimeLimit();

		if(this->GetIsCombatMode())
		{
			if(this->GetIsIronMan())
			{
				theTimeLimitSeconds = CVAR_GET_FLOAT(kvIronManTime)*60;
			}
		}

		if((theTimeLimitSeconds > 0) && ((gpGlobals->time - GetGameRules()->GetTimeGameStarted()) > theTimeLimitSeconds))
		{
			theTimeLimitHit = true;
		}

		if(this->GetIsCheatEnabled(kcEndGame1))
		{
			theTeamBLost = true;
		}
		else if(this->GetIsCheatEnabled(kcEndGame2))
		{
			theTeamALost = true;
		}
		else if(this->GetIsCombatMode())
		{
			// Alien victory if time limit is hit
			if(theTimeLimitHit)
			{
				if(this->mCombatAttackingTeamNumber == this->mTeamA.GetTeamNumber())
				{
					theTeamALost = true;
				}
				else if(this->mCombatAttackingTeamNumber == this->mTeamB.GetTeamNumber())
				{
					theTeamBLost = true;
				}
			}
		}
		else if(this->GetIsTournamentMode() && !this->GetIsCombatMode())
		{
			// If timelimit has elapsed in tourny mode, the victor is the team with the most resources
			if(theTimeLimitHit)
			{
				// Don't count fractional resources.  If it's that close, it was a tie.
				int theTeamAResources = this->mTeamA.GetTotalResourcesGathered();
				int theTeamBResources = this->mTeamB.GetTotalResourcesGathered();
				if(theTeamAResources > theTeamBResources)
				{
					theTeamBLost = true;
				}
				else if(theTeamBResources > theTeamAResources)
				{
					theTeamALost = true;
				}
				else
				{
					// It's a draw
					theDrawGame = true;
				}
			}
		}
		else if(this->GetIsIronMan())
		{
			if(theTimeLimitHit)
			{
				// Aliens win.  If both teams are the same, it's a draw.
				if(this->mTeamA.GetTeamType() == this->mTeamB.GetTeamType())
				{
					theDrawGame = true;
				}
				else
				{
					if(this->mTeamA.GetTeamType() == AVH_CLASS_TYPE_ALIEN)
					{
						theTeamBLost = true;
					}
					else if(this->mTeamB.GetTeamType() == AVH_CLASS_TYPE_ALIEN)
					{
						theTeamALost = true;
					}
				}
			}
			else if(!this->mTeamA.GetHasAtLeastOneActivePlayer())
			{
				theTeamALost = true;
			}
			else if(!this->mTeamB.GetHasAtLeastOneActivePlayer())
			{
				theTeamBLost = true;
			}
		}
		else if(!this->GetIsTournamentMode())
		{
			// Check for automatic concession, if teams are suddenly unbalanced (generally because a team realizes it's futile and leaves)
			const int kAutoConcedeNumPlayers = (int)(avh_autoconcede.value);
			if(kAutoConcedeNumPlayers > 0)
			{
				if((theTeamAPlayers - theTeamBPlayers) >= kAutoConcedeNumPlayers)
				{
					theTeamBLost = true;
				}
				else if((theTeamBPlayers - theTeamAPlayers) >= kAutoConcedeNumPlayers)
				{
					theTeamALost = true;
				}
			}
		}

		if((theTeamALost && theTeamBLost) || theDrawGame)
		{
			// Draw!
			this->mVictoryTime = gpGlobals->time;
			this->mVictoryTeam = TEAM_SPECT;
			this->mVictoryDraw = true;
			theVictoryMessage = kGameDraw;
			theDrawGame = true;
		}

		if(theTeamALost)
		{
			// If there is a victory, set mVictoryTime and mVictoryTeam
			this->mVictoryTime = gpGlobals->time;
			this->mVictoryTeam = this->mTeamB.GetTeamNumber();
			theVictoryMessage = kTeamTwoWon;
		}
		else if(theTeamBLost)
		{
			// If there is a victory, set mVictoryTime and mVictoryTeam
			this->mVictoryTime = gpGlobals->time;
			this->mVictoryTeam = this->mTeamA.GetTeamNumber();
			theVictoryMessage = kTeamOneWon;
		}

		//if(!theAtLeastOneTeamOneMemberLeft || !theAtLeastOneTeamTwoMemberLeft)
		if(this->mVictoryTeam != TEAM_IND)
		{
			this->TallyVictoryStats();
			AvHNexus::finishGame();

			FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
				AvHTeam* theTeam = theEntity->GetTeamPointer();
				AvHHUDSound theHUDSound = HUD_SOUND_YOU_LOSE;
				if((theTeam && (theTeam->GetTeamNumber() == this->mVictoryTeam)) || (theEntity->GetPlayMode() == PLAYMODE_OBSERVER) || (theEntity->pev->team == TEAM_IND) || theDrawGame)
				{
					theHUDSound = HUD_SOUND_YOU_WIN;
				}
				else if(!this->GetIsCombatMode())
				{
					// Fade losers out to black
					Vector theFadeColor;
					theFadeColor.x = 0;
					theFadeColor.y = 0;
					theFadeColor.z = 0;
					UTIL_ScreenFade(theEntity, theFadeColor, 1.0f, 0.0f, 255, FFADE_OUT | FFADE_STAYOUT);
				}
				theEntity->PlayHUDSound(theHUDSound);
				theEntity->SendMessage(theVictoryMessage, CENTER);

				// Final game time update to all clients have same winning time
				this->SendGameTimeUpdate(true);

				// Send final score to everyone if needed
				this->mTeamA.SendResourcesGatheredScore(false);
				this->mTeamB.SendResourcesGatheredScore(false);

			END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
			// Tell everyone that the game ended
			NetMsg_GameStatus_State( kGameStatusEnded, this->mMapMode );
		}
	}
}

int	AvHGamerules::GetBaseHealthForMessageID(AvHMessageID inMessageID) const
{
	int health = 100;

	switch(inMessageID)
	{
	case BUILD_INFANTRYPORTAL:			health = BALANCE_VAR(kInfantryPortalHealth); break;
	case BUILD_RESOURCES:				health = BALANCE_VAR(kResourceTowerHealth); break;
	case BUILD_TURRET_FACTORY:			health = BALANCE_VAR(kTurretFactoryHealth); break;
	case TURRET_FACTORY_UPGRADE:		health = BALANCE_VAR(kTurretFactoryHealth); break;
	case BUILD_ARMSLAB:					health = BALANCE_VAR(kArmsLabHealth); break;
	case BUILD_PROTOTYPE_LAB:			health = BALANCE_VAR(kArmsLabHealth); break;
	case BUILD_ARMORY:					health = BALANCE_VAR(kArmoryHealth); break;
	case ARMORY_UPGRADE:				health = BALANCE_VAR(kAdvArmoryHealth); break;
	case BUILD_OBSERVATORY:				health = BALANCE_VAR(kObservatoryHealth); break;
	case BUILD_PHASEGATE:				health = BALANCE_VAR(kPhaseGateHealth); break;
	case BUILD_TURRET:					health = BALANCE_VAR(kSentryHealth); break;
	case BUILD_SIEGE:					health = BALANCE_VAR(kSiegeHealth); break;
	case BUILD_COMMANDSTATION:			health = BALANCE_VAR(kCommandStationHealth); break;
	case ALIEN_BUILD_RESOURCES:			health = BALANCE_VAR(kAlienResourceTowerHealth); break;
	case ALIEN_BUILD_OFFENSE_CHAMBER:	health = BALANCE_VAR(kOffenseChamberHealth); break;
	case ALIEN_BUILD_DEFENSE_CHAMBER:	health = BALANCE_VAR(kDefenseChamberHealth); break;
	case ALIEN_BUILD_SENSORY_CHAMBER:	health = BALANCE_VAR(kSensoryChamberHealth); break;
	case ALIEN_BUILD_MOVEMENT_CHAMBER:	health = BALANCE_VAR(kMovementChamberHealth); break;
	case ALIEN_BUILD_HIVE:				health = BALANCE_VAR(kHiveHealth); break;
	}
	return health;
}


int	AvHGamerules::GetBuildTimeForMessageID(AvHMessageID inMessageID) const
{
	float time = 0.0f;
	const float CO_Scalar = this->GetIsCombatMode() ? BALANCE_VAR(kCombatModeTimeScalar) : 1.0f;
	const float CO_GScalar = this->GetIsCombatMode() ? BALANCE_VAR(kCombatModeGestationTimeScalar) : 1.0f;

	switch(inMessageID)
	{
		// Marine Research
		case RESEARCH_ELECTRICAL:		time = BALANCE_VAR(kElectricalUpgradeResearchTime); break;
		case RESEARCH_ARMOR_ONE:		time = BALANCE_VAR(kArmorOneResearchTime); break;
		case RESEARCH_ARMOR_TWO:		time = BALANCE_VAR(kArmorTwoResearchTime); break;
		case RESEARCH_ARMOR_THREE:		time = BALANCE_VAR(kArmorThreeResearchTime); break;
		case RESEARCH_WEAPONS_ONE:		time = BALANCE_VAR(kWeaponsOneResearchTime); break;
		case RESEARCH_WEAPONS_TWO:		time = BALANCE_VAR(kWeaponsTwoResearchTime); break;
		case RESEARCH_WEAPONS_THREE:	time = BALANCE_VAR(kWeaponsThreeResearchTime); break;
        case RESEARCH_CATALYSTS:		time = BALANCE_VAR(kCatalystResearchTime); break;
        case RESEARCH_GRENADES:			time = BALANCE_VAR(kGrenadesResearchTime); break;
		case RESEARCH_JETPACKS:			time = BALANCE_VAR(kJetpacksResearchTime); break;
		case RESEARCH_HEAVYARMOR:		time = BALANCE_VAR(kHeavyArmorResearchTime); break;
		case RESEARCH_DISTRESSBEACON:	time = BALANCE_VAR(kDistressBeaconTime); break;
		case RESEARCH_HEALTH:			time = BALANCE_VAR(kHealthResearchTime); break;
		case RESEARCH_MOTIONTRACK:		time = BALANCE_VAR(kMotionTrackingResearchTime); break;
		case RESEARCH_PHASETECH:		time = BALANCE_VAR(kPhaseTechResearchTime); break;

		// Marine Structures
		case BUILD_INFANTRYPORTAL:		time = BALANCE_VAR(kInfantryPortalBuildTime); break;
		case BUILD_RESOURCES:			time = BALANCE_VAR(kResourceTowerBuildTime); break;
		case BUILD_TURRET_FACTORY:		time = BALANCE_VAR(kTurretFactoryBuildTime); break;
		case BUILD_ARMSLAB:				time = BALANCE_VAR(kArmsLabBuildTime); break;
		case BUILD_PROTOTYPE_LAB:		time = BALANCE_VAR(kPrototypeLabBuildTime); break;
		case BUILD_ARMORY:				time = BALANCE_VAR(kArmoryBuildTime); break;
		case ARMORY_UPGRADE:			time = BALANCE_VAR(kArmoryUpgradeTime); break;
		case BUILD_OBSERVATORY:			time = BALANCE_VAR(kObservatoryBuildTime); break;
		case BUILD_PHASEGATE:			time = BALANCE_VAR(kPhaseGateBuildTime); break;
		case BUILD_TURRET:				time = BALANCE_VAR(kSentryBuildTime); break;
		case BUILD_SIEGE:				time = BALANCE_VAR(kSiegeBuildTime); break;
		case BUILD_COMMANDSTATION:		time = BALANCE_VAR(kCommandStationBuildTime); break;
		case TURRET_FACTORY_UPGRADE:	time = BALANCE_VAR(kTurretFactoryUpgradeTime); break;

		// Scan Duration
		case BUILD_SCAN:				time = BALANCE_VAR(kScanDuration); break;
		
		// Alien Structures
		case ALIEN_BUILD_RESOURCES:			time = BALANCE_VAR(kAlienResourceTowerBuildTime); break;
		case ALIEN_BUILD_OFFENSE_CHAMBER:	time = BALANCE_VAR(kOffenseChamberBuildTime); break;
		case ALIEN_BUILD_DEFENSE_CHAMBER:	time = BALANCE_VAR(kDefenseChamberBuildTime); break;
		case ALIEN_BUILD_SENSORY_CHAMBER:	time = BALANCE_VAR(kSensoryChamberBuildTime); break;
		case ALIEN_BUILD_MOVEMENT_CHAMBER:	time = BALANCE_VAR(kMovementChamberBuildTime); break;
		case ALIEN_BUILD_HIVE:				time = BALANCE_VAR(kHiveBuildTime); break;

		// Alien Evolutions
		case ALIEN_EVOLUTION_ONE:			time = BALANCE_VAR(kEvolutionGestateTime)*CO_Scalar;	break;
		case ALIEN_EVOLUTION_TWO:			time = BALANCE_VAR(kEvolutionGestateTime)*CO_Scalar;	break;
		case ALIEN_EVOLUTION_THREE:			time = BALANCE_VAR(kEvolutionGestateTime)*CO_Scalar;	break;
		case ALIEN_EVOLUTION_SEVEN:			time = BALANCE_VAR(kEvolutionGestateTime)*CO_Scalar;	break;
		case ALIEN_EVOLUTION_EIGHT:			time = BALANCE_VAR(kEvolutionGestateTime)*CO_Scalar;	break;
		case ALIEN_EVOLUTION_NINE:			time = BALANCE_VAR(kEvolutionGestateTime)*CO_Scalar;	break;
		case ALIEN_EVOLUTION_TEN:			time = BALANCE_VAR(kEvolutionGestateTime)*CO_Scalar;	break;
		case ALIEN_EVOLUTION_ELEVEN:		time = BALANCE_VAR(kEvolutionGestateTime)*CO_Scalar;	break;
		case ALIEN_EVOLUTION_TWELVE:		time = BALANCE_VAR(kEvolutionGestateTime)*CO_Scalar;	break;
        case ALIEN_HIVE_TWO_UNLOCK:			time = BALANCE_VAR(kEvolutionGestateTime)*CO_Scalar;	break;
        case ALIEN_HIVE_THREE_UNLOCK:		time = BALANCE_VAR(kEvolutionGestateTime)*CO_Scalar;	break;
	
		// Alien Lifeforms
		case ALIEN_LIFEFORM_ONE:		time = BALANCE_VAR(kSkulkGestateTime)*CO_GScalar; break;
		case ALIEN_LIFEFORM_TWO:		time = BALANCE_VAR(kGorgeGestateTime)*CO_GScalar; break;
		case ALIEN_LIFEFORM_THREE:		time = BALANCE_VAR(kLerkGestateTime)*CO_GScalar; break;
		case ALIEN_LIFEFORM_FOUR:		time = BALANCE_VAR(kFadeGestateTime)*CO_GScalar; break;
		case ALIEN_LIFEFORM_FIVE:		time = BALANCE_VAR(kOnosGestateTime)*CO_GScalar; break;
	}

	if( time > 0 )
	{
		time = max( time, 1.0f ); //for cases where combat scalars would  result in fractional seconds
	}

	if(this->GetCheatsEnabled() && !this->GetIsCheatEnabled(kcSlowResearch))
	{
		time = min( time, 2.0f );
	}

	return floor( time );
}

int	AvHGamerules::GetCostForMessageID(AvHMessageID inMessageID) const
{
	// This is point cost or energy cost in NS, or number of levels in Combat
	int	cost = 0;

	if(this->GetIsCombatMode())
    {
        switch(inMessageID)
        {
		case ALIEN_LIFEFORM_TWO:
        case ALIEN_EVOLUTION_ONE:
        case ALIEN_EVOLUTION_TWO:
        case ALIEN_EVOLUTION_THREE:
        case ALIEN_EVOLUTION_SEVEN:
        case ALIEN_EVOLUTION_EIGHT:
        case ALIEN_EVOLUTION_NINE:
        case ALIEN_EVOLUTION_TEN:
        case ALIEN_EVOLUTION_TWELVE:
        case BUILD_RESUPPLY:
        case RESEARCH_ARMOR_ONE:
        case RESEARCH_ARMOR_TWO:
        case RESEARCH_ARMOR_THREE:
        case RESEARCH_WEAPONS_ONE:
        case RESEARCH_WEAPONS_TWO:
        case RESEARCH_WEAPONS_THREE:
        case BUILD_CAT:
        case RESEARCH_MOTIONTRACK:
        case RESEARCH_GRENADES:
        case BUILD_SCAN:
        case BUILD_MINES:
        case BUILD_WELDER:
        case BUILD_SHOTGUN:
        case BUILD_HMG:
        case BUILD_GRENADE_GUN:
        case ALIEN_HIVE_TWO_UNLOCK:
            cost = 1;
            break;

		case ALIEN_LIFEFORM_THREE:
        case BUILD_HEAVY:
        case BUILD_JETPACK:
        case ALIEN_HIVE_THREE_UNLOCK:
		case ALIEN_EVOLUTION_ELEVEN:
            cost = 2;
            break;

        case ALIEN_LIFEFORM_FOUR:
			cost = 3;
			break;

        case ALIEN_LIFEFORM_FIVE:
			cost = 4;
			break;
        }
    }
    else
	{
		switch(inMessageID)
		{
			// Marine Research
			case RESEARCH_ELECTRICAL:		cost = BALANCE_VAR(kElectricalUpgradeResearchCost); break;
			case RESEARCH_ARMOR_ONE:		cost = BALANCE_VAR(kArmorOneResearchCost); break;
			case RESEARCH_ARMOR_TWO:		cost = BALANCE_VAR(kArmorTwoResearchCost); break;
			case RESEARCH_ARMOR_THREE:		cost = BALANCE_VAR(kArmorThreeResearchCost); break;
			case RESEARCH_WEAPONS_ONE:		cost = BALANCE_VAR(kWeaponsOneResearchCost); break;
			case RESEARCH_WEAPONS_TWO:		cost = BALANCE_VAR(kWeaponsTwoResearchCost); break;
			case RESEARCH_WEAPONS_THREE:	cost = BALANCE_VAR(kWeaponsThreeResearchCost); break;
            case RESEARCH_CATALYSTS:		cost = BALANCE_VAR(kCatalystResearchCost); break;
            case RESEARCH_GRENADES:			cost = BALANCE_VAR(kGrenadesResearchCost); break;
			case TURRET_FACTORY_UPGRADE:	cost = BALANCE_VAR(kTurretFactoryUpgradeCost); break;
			case RESEARCH_JETPACKS:			cost = BALANCE_VAR(kJetpacksResearchCost); break;
			case RESEARCH_HEAVYARMOR:		cost = BALANCE_VAR(kHeavyArmorResearchCost); break;
			case RESEARCH_HEALTH:			cost = BALANCE_VAR(kHealthResearchCost); break;
			case RESEARCH_MOTIONTRACK:		cost = BALANCE_VAR(kMotionTrackingResearchCost); break;
			case RESEARCH_PHASETECH:		cost = BALANCE_VAR(kPhaseTechResearchCost); break;
			case RESEARCH_DISTRESSBEACON:	cost = BALANCE_VAR(kDistressBeaconCost); break;
		
			// Marine Structures
			case BUILD_HEAVY:				cost = BALANCE_VAR(kHeavyArmorCost); break;
			case BUILD_JETPACK:				cost = BALANCE_VAR(kJetpackCost); break;
			case BUILD_INFANTRYPORTAL:		cost = BALANCE_VAR(kInfantryPortalCost); break;
			case BUILD_RESOURCES:			cost = BALANCE_VAR(kResourceTowerCost); break;
			case BUILD_TURRET_FACTORY:		cost = BALANCE_VAR(kTurretFactoryCost); break;
			case BUILD_ARMSLAB:				cost = BALANCE_VAR(kArmsLabCost); break;
			case BUILD_PROTOTYPE_LAB:		cost = BALANCE_VAR(kPrototypeLabCost); break;
			case BUILD_ARMORY:				cost = BALANCE_VAR(kArmoryCost); break;
			case ARMORY_UPGRADE:			cost = BALANCE_VAR(kArmoryUpgradeCost); break;
			case BUILD_OBSERVATORY:			cost = BALANCE_VAR(kObservatoryCost); break;
			case BUILD_PHASEGATE:			cost = BALANCE_VAR(kPhaseGateCost); break;
			case BUILD_TURRET:				cost = BALANCE_VAR(kSentryCost); break;
			case BUILD_SIEGE:				cost = BALANCE_VAR(kSiegeCost); break;
			case BUILD_COMMANDSTATION:		cost = BALANCE_VAR(kCommandStationCost); break;
			
			// Marine Equipment
			case BUILD_HEALTH:					cost = BALANCE_VAR(kHealthCost); break;
			case BUILD_AMMO:					cost = BALANCE_VAR(kAmmoCost); break;
            case BUILD_CAT:						cost = BALANCE_VAR(kCatalystCost); break;
			case BUILD_MINES:					cost = BALANCE_VAR(kMineCost); break;
			case BUILD_WELDER:					cost = BALANCE_VAR(kWelderCost); break;
			case BUILD_SHOTGUN:					cost = BALANCE_VAR(kShotgunCost); break;
			case BUILD_HMG:						cost = BALANCE_VAR(kHMGCost); break;
			case BUILD_GRENADE_GUN:				cost = BALANCE_VAR(kGrenadeLauncherCost); break;

			// Alien Structures
			case ALIEN_BUILD_RESOURCES:			cost = BALANCE_VAR(kAlienResourceTowerCost); break;
			case ALIEN_BUILD_OFFENSE_CHAMBER:	cost = BALANCE_VAR(kOffenseChamberCost); break;
			case ALIEN_BUILD_DEFENSE_CHAMBER:	cost = BALANCE_VAR(kDefenseChamberCost); break;
			case ALIEN_BUILD_SENSORY_CHAMBER:	cost = BALANCE_VAR(kSensoryChamberCost); break;
			case ALIEN_BUILD_MOVEMENT_CHAMBER:	cost = BALANCE_VAR(kMovementChamberCost); break;
			case ALIEN_BUILD_HIVE:				cost = BALANCE_VAR(kHiveCost); break;

			// Alien Evolutions
			case ALIEN_EVOLUTION_ONE:			cost = BALANCE_VAR(kEvolutionCost); break;
			case ALIEN_EVOLUTION_TWO:			cost = BALANCE_VAR(kEvolutionCost); break;
			case ALIEN_EVOLUTION_THREE:			cost = BALANCE_VAR(kEvolutionCost); break;
			case ALIEN_EVOLUTION_SEVEN:			cost = BALANCE_VAR(kEvolutionCost); break;
			case ALIEN_EVOLUTION_EIGHT:			cost = BALANCE_VAR(kEvolutionCost); break;
			case ALIEN_EVOLUTION_NINE:			cost = BALANCE_VAR(kEvolutionCost); break;
			case ALIEN_EVOLUTION_TEN:			cost = BALANCE_VAR(kEvolutionCost); break;
			case ALIEN_EVOLUTION_ELEVEN:		cost = BALANCE_VAR(kEvolutionCost); break;
			case ALIEN_EVOLUTION_TWELVE:		cost = BALANCE_VAR(kEvolutionCost); break;
		
			// Alien Lifeforms
			case ALIEN_LIFEFORM_ONE:			cost = BALANCE_VAR(kSkulkCost); break;
			case ALIEN_LIFEFORM_TWO:			cost = BALANCE_VAR(kGorgeCost); break;
			case ALIEN_LIFEFORM_THREE:			cost = BALANCE_VAR(kLerkCost); break;
			case ALIEN_LIFEFORM_FOUR:			cost = BALANCE_VAR(kFadeCost); break;
			case ALIEN_LIFEFORM_FIVE:			cost = BALANCE_VAR(kOnosCost); break;

			// Energy Costs
			case BUILD_SCAN:					cost = BALANCE_VAR(kScanEnergyCost); break;
		}
	}
	
	return cost;
}

void AvHGamerules::BalanceChanged()
{
	// Tell all players to update their weapons
	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		theEntity->SendWeaponUpdate();
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
}


