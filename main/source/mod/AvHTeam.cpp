//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHTeam.cpp $
// $Date: 2002/11/26 20:35:00 $
//
//-------------------------------------------------------------------------------
// $Log: AvHTeam.cpp,v $
// Revision 1.55  2002/11/26 20:35:00  Flayra
// - Hurt fix
//
// Revision 1.54  2002/11/26 15:58:31  Flayra
// - When the alien team no longer has any growing or active hives, all aliens slowly take damage. This is done to speed up the end game and prevent hiding llamas.
//
// Revision 1.53  2002/11/22 23:26:59  Flayra
// - Don't play kill lingering aliens with cheats on (when testing)
//
// Revision 1.52  2002/11/22 21:15:37  Flayra
// - Remove owner of entities when player leaves the team
// - Do damage to aliens without hives
// - Fixed bug where a vote could affect a commander that logged in after a vote had been started.
//
// Revision 1.51  2002/11/15 23:31:26  Flayra
// - Added "ready" verification for tourny mode
//
// Revision 1.50  2002/11/12 02:29:33  Flayra
// - Added better standardized logging
//
// Revision 1.49  2002/11/06 03:08:56  Flayra
// - Undid +1 for resources, it's too drastic
//
// Revision 1.48  2002/11/06 02:23:49  Flayra
// - Removed faulty minimum trickle that was giving 1 extra resource point to both teams
//
// Revision 1.47  2002/11/06 01:39:04  Flayra
// - Show true resources going to team
//
// Revision 1.46  2002/11/03 04:52:55  Flayra
// - Hard-coded resources
//
// Revision 1.45  2002/10/24 21:43:29  Flayra
// - Alien easter eggs
// - Voting fix
//
// Revision 1.44  2002/10/19 21:28:38  Flayra
// - Debugging info for linux
//
// Revision 1.43  2002/10/19 21:09:56  Flayra
// - Debugging info for linux
//
// Revision 1.42  2002/10/19 20:56:06  Flayra
// - Debugging info for linux
//
// Revision 1.41  2002/10/19 20:19:26  Flayra
// - Debugging info for linux
//
// Revision 1.40  2002/10/19 20:04:14  Flayra
// - Debugging info for linux
//
// Revision 1.39  2002/10/18 22:23:04  Flayra
// - Added beginnings of alien easter eggs
// - Added "we need builders" alert
//
// Revision 1.38  2002/10/04 18:03:23  Flayra
// - Fixed bug where extra resources were sometimes being wasted on the alien team
//
// Revision 1.37  2002/10/03 19:09:55  Flayra
// - New resource model
// - Orders refactoring
// - Tech tree changes
// - Aliens always get initial points when joining
// - Play gamestart sound
// - New trait available trigger
// - Moved voting stuff to server variables
// - Slowed down hints
//
// Revision 1.36  2002/09/25 20:51:31  Flayra
// - Play commander-idle sounds less often
//
// Revision 1.35  2002/09/23 22:35:43  Flayra
// - Removed hive donation and put in new system for builders
// - Updated tech tree (jetpacks, heavy armor, moved phase)
// - Resource towers set as built on game start
// - Added tons of commander hints (low resource alerts, tell commander about pressing jump key, commander ejected, select troops and give orders, don't just sit there, etc.)
//
// Revision 1.34  2002/09/09 20:08:26  Flayra
// - Added commander voting
// - Added hive info
// - Changed how commander scoring works
//
// Revision 1.33  2002/08/31 18:01:03  Flayra
// - Work at VALVe
//
// Revision 1.32  2002/08/02 21:45:20  Flayra
// - Reworked alerts.
//
// Revision 1.31  2002/07/28 19:21:27  Flayra
// - Balance changes after/during RC4a
//
// Revision 1.30  2002/07/26 23:08:25  Flayra
// - Added numerical feedback
//
// Revision 1.29  2002/07/25 16:58:00  flayra
// - Linux changes
//
// Revision 1.28  2002/07/24 18:55:53  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.27  2002/07/23 17:32:23  Flayra
// - Resource model overhaul (closed system, random initial points), tech tree changes (new marine upgrades), free resource tower at nearest func_resource
//
// Revision 1.26  2002/07/10 14:45:26  Flayra
// - Fixed victory condition bug (bug #171)
//
// Revision 1.25  2002/07/08 17:19:42  Flayra
// - Added handicapping, map validity checking, reinforcements happen independently of teams now
//
// Revision 1.24  2002/07/01 21:24:07  Flayra
// - Brought siege back, removed alpha male resource model
//
// Revision 1.23  2002/06/25 18:21:33  Flayra
// - New enabled/disabled system for alien weapons, better victory detection, updated tech tree (removed old junk, added armory upgrade), fixed bug where a commander that leaves the game hogged the station, update resources less often (optimization)
//
// Revision 1.22  2002/06/03 17:00:33  Flayra
// - Removed mines and siege temporarily while being worked on, removed old code, removed redundant hive class name
//
// Revision 1.21  2002/05/28 18:09:51  Flayra
// - Track number of webs for team, fix for premature victory condition (again), fixed bug where alien upgrades weren't being cleared between levels, causing eventual overflows for alien players after many games, recycling support, spawn in command center when game starts
//
// Revision 1.20  2002/05/23 02:32:57  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "mod/AvHTeam.h"
#include "mod/AvHPlayer.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHServerVariables.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "game_shared/teamconst.h"
#include "mod/AvHBuildable.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHTitles.h"
#include "mod/AvHPlayerUpgrade.h"
#include "util/MathUtil.h"
#include "mod/AvHNetworkMessages.h"
#include "mod/AvHNexusServer.h"

extern int gPhaseInEventID;
extern cvar_t avh_votecasttime;
extern cvar_t avh_votedowntime;
extern cvar_t avh_votepercentneeded;
extern cvar_t avh_minvotesneeded;
extern cvar_t avh_initialalienpoints;
extern cvar_t avh_eastereggchance;

AvHTeam::AvHTeam(AvHTeamNumber inTeamNumber)
{
	this->Init();
	this->mTeamNumber = inTeamNumber;
	this->mResearchManager.SetTeamNumber(inTeamNumber);
}

void AvHTeam::Init()
{
	this->mTeamType = AVH_CLASS_TYPE_UNDEFINED;
	this->mTeamName = kInvalidTeamName;
	this->mTeamNumber = TEAM_IND;
	this->mCommander = -1;
	this->mCommanderWhenVoteStarted = -1;
	
	this->mPlayerList.clear();
	this->mResourceTowerList.clear();

	this->mLastResourceUpdateTime = -1;
	this->mLastCommandScoreUpdateTime = -1;
	this->mLastServerPlayerDataUpdateTime = -1;
	this->mLastPlayerUpdateTime = -1;
	this->mLastInjectionTime = 0;
	this->mLastHiveSpawnTime = 0;

	// Alerts
	this->mAlertList.clear();
	this->mTimeOfLastTeamHint = -1;
	this->mTimeLastHintUpdate = -1;

	this->mTeamResources = 0;
	this->mHandicap = 1.0f;

	this->mResearchManager.Reset();

	this->mAlienUpgrades.clear();
	this->mTeamWideUpgrades = 0;

	this->mNumWebStrands = 0;
	this->mIsReady = false;

	this->mTotalResourcesGathered = 0;
	this->mClientTotalResourcesGathered = 0;

	this->mMenuTechSlots = 0;

	this->mTimeReinforcementWaveComplete = -1;
}

bool AvHTeam::AddPlayer(int inPlayerIndex)
{
	bool theSuccess = false;
	
	if(!this->GetIsPlayerOnTeam(inPlayerIndex))
	{
		// Add player
		this->mPlayerList.push_back(inPlayerIndex);

		theSuccess = true;
	}
	
	return theSuccess;
}

bool AvHTeam::AddResourceTower(int inResourceTowerIndex)
{
	bool theSuccess = false;

	EntityListType::const_iterator theIter = std::find(this->mResourceTowerList.begin(), this->mResourceTowerList	.end(), inResourceTowerIndex);
	if(theIter == this->mResourceTowerList.end())
	{
		this->mResourceTowerList.push_back(inResourceTowerIndex);
		theSuccess = true;
	}

	return theSuccess;
}

bool AvHTeam::RemoveResourceTower(int inResourceTowerIndex)
{
	bool theSuccess = false;

	EntityListType::iterator theIter = std::find(this->mResourceTowerList.begin(), this->mResourceTowerList	.end(), inResourceTowerIndex);
	if(theIter != this->mResourceTowerList.end())
	{
		this->mResourceTowerList.erase(theIter);
		theSuccess = true;
	}

	return theSuccess;
}

bool AvHTeam::GetIsReady() const
{
	return this->mIsReady;
}

int	AvHTeam::GetMenuTechSlots() const
{
	return this->mMenuTechSlots;
}

void AvHTeam::SetIsReady(bool bIsReady)
{
	this->mIsReady = bIsReady;
}

int	AvHTeam::GetNumBuiltCommandStations() const
{
	return this->mNumBuiltCommandStations;
}

int	AvHTeam::GetNumActiveHives() const
{
	return this->mNumActiveHives;
}

float AvHTeam::GetMaxResources(AvHUser3 inUser3) const
{
	float theMaxResources = -1;

	if(this->mTeamType == AVH_CLASS_TYPE_ALIEN)
	{
		//if(inUser3 == AVH_USER3_ALIEN_PLAYER2)
		//{
			// Needed so builders can create hives (also means that having more builders diverts resources from combat/offense)
			theMaxResources = kMaxAlienResources;
		//}
		//else
		//{
		//	int theNumActiveHives = max(this->GetNumActiveHives(), 1);
		//	theMaxResources = (theNumActiveHives/(float)kMaxAlienHives)*kMaxAlienResources;
		//}
	}

	return theMaxResources;
}


bool AvHTeam::GetTeamHasAbilityToRespawn() const
{
	bool theAbilityToRespawn = false;
	
	// If game hasn't started
	if(!GetGameRules()->GetGameStarted())
	{
		// return true
		theAbilityToRespawn = true;
	}
	// else if we're a marine team
	else if(this->GetTeamType() == AVH_CLASS_TYPE_MARINE)
	{
		// Do we have any built infantry portals?
		FOR_ALL_ENTITIES(kwsInfantryPortal, AvHInfantryPortal*)
		if(theEntity->GetIsBuilt())
		{
			AvHTeamNumber theTeamNumber = (AvHTeamNumber)(theEntity->pev->team);
			if(theTeamNumber == this->mTeamNumber)
			{
				theAbilityToRespawn = true;
				break;
			}
		}
		END_FOR_ALL_ENTITIES(kwsInfantryPortal);
	}
	// else if we're an alien team
	else if(this->GetTeamType() == AVH_CLASS_TYPE_ALIEN)
	{
		// Do we have any active hives?
		FOR_ALL_ENTITIES(kesTeamHive, AvHHive*)
			if(theEntity->GetIsActive())
			{
				AvHTeamNumber theTeamNumber = (AvHTeamNumber)(theEntity->pev->team);
				if(theTeamNumber == this->mTeamNumber)
				{
					theAbilityToRespawn = true;
					break;
				}
			}
			END_FOR_ALL_ENTITIES(kesTeamHive);
	}
	
	return theAbilityToRespawn;
}

bool AvHTeam::GetHasAtLeastOneActivePlayer(bool* outHasAtLeastOnePlayerOnTeam) const
{
	bool theHasAtLeastOneActivePlayer = false;

	// Loop through all players on team
	for(EntityListType::const_iterator theIter = this->mPlayerList.begin(); theIter != this->mPlayerList.end(); theIter++)
	{
		const AvHPlayer* thePlayer = this->GetPlayerFromIndex(*theIter);
		ASSERT(thePlayer);
		
		if(outHasAtLeastOnePlayerOnTeam)
		{
			*outHasAtLeastOnePlayerOnTeam = true;
		}
		
		if((thePlayer->IsAlive() && !thePlayer->GetIsBeingDigested()) || (thePlayer->GetUser3() == AVH_USER3_COMMANDER_PLAYER))
		{
			theHasAtLeastOneActivePlayer = true;
			break;
		}
	}

	return theHasAtLeastOneActivePlayer;
}

bool AvHTeam::GetHasTeamLost() const
{
	bool theTeamHasLost = false;
	
	bool theHasAtLeastOnePlayerOnTeam = false;
	bool theHasAtLeastOneActivePlayer = this->GetHasAtLeastOneActivePlayer(&theHasAtLeastOnePlayerOnTeam);
	
	if(GetGameRules()->GetIsCombatMode())
	{
		if(this->GetTeamType() == AVH_CLASS_TYPE_ALIEN)
		{
			if((this->GetNumActiveHives() == 0) || !theHasAtLeastOnePlayerOnTeam)
			{
				theTeamHasLost = true;
			}
		}
		else if(this->GetTeamType() == AVH_CLASS_TYPE_MARINE)
		{
			if((this->GetNumBuiltCommandStations() == 0) || !theHasAtLeastOnePlayerOnTeam)
			{
				theTeamHasLost = true;
			}
		}
	}
	else
	{
		// We need at least one alive player, OR, at the ability to respawn
		bool theHasAbilityToRespawn = this->GetTeamHasAbilityToRespawn();
		if((!theHasAtLeastOneActivePlayer && !theHasAbilityToRespawn) || !theHasAtLeastOnePlayerOnTeam)
		{
			theTeamHasLost = true;
		}
	}

	if(theTeamHasLost)
	{
		//UTIL_LogPrintf("Team %d has lost.  theAtLeastOneAlivePlayer: %d, theAbilityToRespawn: %d, theHasAtLeastOnePlayerOnTeam: %d\n", this->mTeamNumber, theHasAtLeastOneAlivePlayer, theHasAbilityToRespawn, theHasAtLeastOnePlayerOnTeam);
		UTIL_LogPrintf("Team %d has lost.\n", this->mTeamNumber);
		
		//FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		//	UTIL_LogPrintf("	Victory player info: ent: %d, team: %d, role: %d, playmode: %d\n", theEntity->entindex(), theEntity->pev->team, (int)theEntity->GetRole(), (int)theEntity->GetPlayMode());
		//END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
	}
	
	return theTeamHasLost;
}

HiveInfoListType AvHTeam::GetHiveInfoList() const
{
	return this->mHiveInfoList;
}


const AvHResearchManager& AvHTeam::GetResearchManager() const
{
	return this->mResearchManager;
}

AvHResearchManager&	AvHTeam::GetResearchManager()
{
	return this->mResearchManager;
}

float AvHTeam::GetTotalResourcesGathered() const
{
	return this->mTotalResourcesGathered;
}

void AvHTeam::AddResourcesGathered(float inResources)
{
	this->mTotalResourcesGathered += inResources;
}

bool AvHTeam::GetIsPlayerOnTeam(int inPlayerIndex) const
{
	bool theSuccess = false;
	
	EntityListType::const_iterator theIter = std::find(this->mPlayerList.begin(), this->mPlayerList	.end(), inPlayerIndex);
	if(theIter != this->mPlayerList.end())
	{
		theSuccess = true;
	}
	
	return theSuccess;
}

AvHPlayer* AvHTeam::GetPlayerFromIndex(int inPlayerIndex)
{
	AvHPlayer* thePlayer = NULL;
	
	CBaseEntity* theBaseEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(inPlayerIndex));
	thePlayer = dynamic_cast<AvHPlayer*>(theBaseEntity);
	
	return thePlayer;
}

const AvHPlayer* AvHTeam::GetPlayerFromIndex(int inPlayerIndex) const
{
	const AvHPlayer* thePlayer = NULL;
	const CBaseEntity* theBaseEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(inPlayerIndex));
	thePlayer = dynamic_cast<const AvHPlayer*>(theBaseEntity);
	
	return thePlayer;
}

void AvHTeam::GetOrders(OrderListType& outOrderList) const
{
	outOrderList = this->mOrderList;
}

bool AvHTeam::GetDoesPlayerHaveOrder(int inPlayerIndex)
{
	bool thePlayerHasOrders = false;

	for(OrderListType::iterator theIter = this->mOrderList.begin(); theIter != this->mOrderList.end(); theIter++)
	{
		EntityInfo theReceiver = theIter->GetReceiver();
		if(theReceiver == inPlayerIndex)
		{
			thePlayerHasOrders = true;
		}
	}

	return thePlayerHasOrders;
}

//void AvHTeam::GetPlayersCompletingOrders(const EntityListType& outPlayerList) const
//{
//	outPlayerList = this->mPlayersCompletingOrder;
//}

void AvHTeam::SetOrder(const AvHOrder& inOrder)
{
	AvHChangeOrder(this->mOrderList, inOrder);
}

void AvHTeam::AddTechNode(AvHMessageID inMessageID, AvHTechID inTechID, AvHTechID inPrereq1, AvHTechID inPrereq2, bool inAllowMultiples, bool inResearched)
{
	int theCost = GetGameRules()->GetCostForMessageID(inMessageID);
	int theBuildTime = GetGameRules()->GetBuildTimeForMessageID(inMessageID);

	this->mResearchManager.AddTechNode(inMessageID, inTechID, inPrereq1, inPrereq2, theCost, theBuildTime, inResearched, inAllowMultiples);
}

void AvHTeam::InitializeTechNodes()
{
	// Clear them first
	this->mResearchManager.Reset();
	//this->mUpgradeCosts.clear();
	this->mTechSlotManager.Clear();

	// Depending on game mode, set tech tree
	
	if(GetGameRules()->GetIsCombatMode())
	{
		this->InitializeCombatTechNodes();
	}
	else
	{
		// else regular NS
		if(this->mTeamType == AVH_CLASS_TYPE_MARINE)
		{
			this->AddTechNode(MESSAGE_NULL, TECH_COMMAND_CENTER, TECH_NULL, TECH_NULL, false, true);
			this->AddTechNode(BUILD_RECYCLE, TECH_NULL, TECH_NULL);
			this->AddTechNode(BUILD_COMMANDSTATION, TECH_NULL, TECH_NULL);

			this->AddTechNode(BUILD_INFANTRYPORTAL, TECH_INFANTRYPORTAL, TECH_COMMAND_CENTER);
			//this->AddTechNode(RESEARCH_FASTER_REINFORCEMENTS, TECH_RESEARCH_FASTER_REINFORCEMENTS, TECH_INFANTRYPORTAL, TECH_OBSERVATORY, false, false);
			
			this->AddTechNode(MESSAGE_CANCEL, TECH_NULL, TECH_NULL, TECH_NULL, true);
			
			this->AddTechNode(BUILD_AMMO, TECH_NULL, TECH_NULL);
			this->AddTechNode(BUILD_HEALTH, TECH_NULL, TECH_NULL);
            this->AddTechNode(BUILD_CAT, TECH_NULL, TECH_RESEARCH_CATALYSTS, TECH_NULL, true, false);
			this->AddTechNode(BUILD_RESOURCES, TECH_COMMAND_CENTER, TECH_NULL, TECH_NULL, true, false);
			this->AddTechNode(BUILD_HEALTH, TECH_COMMAND_CENTER, TECH_NULL);

			// CC
			//this->AddTechNode(RESEARCH_HEALTH, TECH_RESEARCH_HEALTH, TECH_COMMAND_CENTER, TECH_NULL, false);
			
			// Turret factory route
			this->AddTechNode(BUILD_TURRET_FACTORY, TECH_TURRET_FACTORY);
			this->AddTechNode(BUILD_TURRET, TECH_NULL, TECH_TURRET_FACTORY);
			this->AddTechNode(RESEARCH_ELECTRICAL, TECH_RESEARCH_ELECTRICAL, TECH_TURRET_FACTORY, TECH_NULL, true);

			this->AddTechNode(TURRET_FACTORY_UPGRADE, TECH_ADVANCED_TURRET_FACTORY, TECH_TURRET_FACTORY, TECH_NULL, true);
			this->AddTechNode(BUILD_SIEGE, TECH_NULL, TECH_ADVANCED_TURRET_FACTORY);
			
			// Arms lab
			this->AddTechNode(BUILD_ARMSLAB, TECH_ARMSLAB, TECH_ARMORY);

			this->AddTechNode(RESEARCH_ARMOR_ONE, TECH_RESEARCH_ARMOR_ONE, TECH_ARMSLAB, TECH_NULL, false);
			this->AddTechNode(RESEARCH_ARMOR_TWO, TECH_RESEARCH_ARMOR_TWO, TECH_RESEARCH_ARMOR_ONE, TECH_NULL, false);
			this->AddTechNode(RESEARCH_ARMOR_THREE, TECH_RESEARCH_ARMOR_THREE, TECH_RESEARCH_ARMOR_TWO, TECH_NULL, false);

			this->AddTechNode(RESEARCH_WEAPONS_ONE, TECH_RESEARCH_WEAPONS_ONE, TECH_ARMSLAB, TECH_NULL, false);
			this->AddTechNode(RESEARCH_WEAPONS_TWO, TECH_RESEARCH_WEAPONS_TWO, TECH_RESEARCH_WEAPONS_ONE, TECH_NULL, false);
			this->AddTechNode(RESEARCH_WEAPONS_THREE, TECH_RESEARCH_WEAPONS_THREE, TECH_RESEARCH_WEAPONS_TWO, TECH_NULL, false);
            this->AddTechNode(RESEARCH_CATALYSTS, TECH_RESEARCH_CATALYSTS, TECH_ARMSLAB, TECH_NULL, false, false);

			this->AddTechNode(RESEARCH_HEAVYARMOR, TECH_RESEARCH_HEAVYARMOR, TECH_PROTOTYPE_LAB, TECH_NULL, false);
					
			// Prototype lab
			this->AddTechNode(BUILD_PROTOTYPE_LAB, TECH_PROTOTYPE_LAB, TECH_ARMSLAB, TECH_ADVANCED_ARMORY);
			this->AddTechNode(RESEARCH_JETPACKS, TECH_RESEARCH_JETPACKS, TECH_PROTOTYPE_LAB, TECH_NULL, false, false);

			// Armor add-ons
			this->AddTechNode(BUILD_JETPACK, TECH_NULL, TECH_RESEARCH_JETPACKS, TECH_PROTOTYPE_LAB);
			this->AddTechNode(BUILD_HEAVY, TECH_NULL, TECH_RESEARCH_HEAVYARMOR, TECH_PROTOTYPE_LAB);

			// Weapon factory route
			this->AddTechNode(BUILD_ARMORY, TECH_ARMORY);
			this->AddTechNode(ARMORY_UPGRADE, TECH_ADVANCED_ARMORY, TECH_ARMORY);
            this->AddTechNode(RESEARCH_GRENADES, TECH_RESEARCH_GRENADES, TECH_ARMORY, TECH_NULL, false, false);
			//this->AddTechNode(BUILD_NUKE_PLANT, TECH_NUKE_PLANT, TECH_ADVANCED_ARMORY);

			// Observatory/knowledge/phase gate route
			this->AddTechNode(BUILD_OBSERVATORY, TECH_OBSERVATORY, TECH_ARMORY);
			this->AddTechNode(BUILD_SCAN, TECH_NULL, TECH_OBSERVATORY);
			this->AddTechNode(RESEARCH_MOTIONTRACK, TECH_RESEARCH_MOTIONTRACK, TECH_OBSERVATORY, TECH_NULL, false);
			this->AddTechNode(RESEARCH_DISTRESSBEACON, TECH_RESEARCH_DISTRESSBEACON, TECH_OBSERVATORY, TECH_NULL);
			this->AddTechNode(RESEARCH_PHASETECH, TECH_RESEARCH_PHASETECH, TECH_INFANTRYPORTAL, TECH_OBSERVATORY, false);
			this->AddTechNode(BUILD_PHASEGATE, TECH_NULL, TECH_OBSERVATORY, TECH_RESEARCH_PHASETECH);
			
			// Weapons
			this->AddTechNode(BUILD_SHOTGUN, TECH_NULL, TECH_ARMORY);
			this->AddTechNode(BUILD_WELDER, TECH_NULL, TECH_ARMORY);
			this->AddTechNode(BUILD_MINES, TECH_NULL, TECH_ARMORY);
			
			this->AddTechNode(BUILD_HMG, TECH_NULL, TECH_ADVANCED_ARMORY);
			this->AddTechNode(BUILD_GRENADE_GUN, TECH_NULL, TECH_ADVANCED_ARMORY, TECH_NULL);
			//this->AddTechNode(BUILD_NUKE, TECH_NULL, TECH_NUKE_PLANT, TECH_ADVANCED_ARMORY);
			
			// Specials
			//this->AddTechNode(BUILD_MEDKIT, TECH_NULL, TECH_MEDLAB);

			// Add tech to allow resource adjustment (allow it to be researched multiple times)
			//this->AddTechNode(RESOURCE_UPGRADE, TECH_NULL, TECH_NULL, TECH_NULL);

			// Initialize tech slots for marines
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_COMMANDER_STATION, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, BUILD_RECYCLE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_TURRET_FACTORY, RESEARCH_ELECTRICAL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, TURRET_FACTORY_UPGRADE, MESSAGE_NULL, MESSAGE_NULL, BUILD_RECYCLE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_ADVANCED_TURRET_FACTORY, RESEARCH_ELECTRICAL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, BUILD_RECYCLE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_ARMORY, ARMORY_UPGRADE, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, RESEARCH_GRENADES, MESSAGE_NULL, MESSAGE_NULL, BUILD_RECYCLE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_ADVANCED_ARMORY, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, RESEARCH_GRENADES, MESSAGE_NULL, MESSAGE_NULL, BUILD_RECYCLE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_ARMSLAB, RESEARCH_WEAPONS_ONE, RESEARCH_WEAPONS_TWO, RESEARCH_WEAPONS_THREE, RESEARCH_CATALYSTS, RESEARCH_ARMOR_ONE, RESEARCH_ARMOR_TWO, RESEARCH_ARMOR_THREE, BUILD_RECYCLE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_PROTOTYPE_LAB, RESEARCH_JETPACKS, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, RESEARCH_HEAVYARMOR, MESSAGE_NULL, MESSAGE_NULL, BUILD_RECYCLE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_OBSERVATORY, BUILD_SCAN, RESEARCH_PHASETECH, MESSAGE_NULL, MESSAGE_NULL, RESEARCH_DISTRESSBEACON, RESEARCH_MOTIONTRACK, MESSAGE_NULL, BUILD_RECYCLE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_TURRET, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, BUILD_RECYCLE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_SIEGETURRET, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, BUILD_RECYCLE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_RESTOWER, RESEARCH_ELECTRICAL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, BUILD_RECYCLE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_INFANTRYPORTAL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, BUILD_RECYCLE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_PHASEGATE, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, MESSAGE_NULL, BUILD_RECYCLE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_MARINE_PLAYER));

			// Initialize tech slots for top-level menus.  Note that the data for these four menus is stored in the command station iuser1
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_MENU_BUILD, BUILD_RESOURCES, BUILD_INFANTRYPORTAL, BUILD_ARMORY, BUILD_COMMANDSTATION, BUILD_TURRET_FACTORY, BUILD_TURRET, BUILD_SIEGE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_MENU_BUILD_ADVANCED, BUILD_OBSERVATORY, BUILD_ARMSLAB, BUILD_PROTOTYPE_LAB, BUILD_PHASEGATE));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_MENU_ASSIST, BUILD_AMMO, BUILD_HEALTH, BUILD_CAT));
			this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_MENU_EQUIP, BUILD_MINES, BUILD_SHOTGUN, BUILD_HMG, BUILD_GRENADE_GUN, BUILD_WELDER, BUILD_JETPACK, BUILD_HEAVY));
			//this->mTechSlotManager.AddTechSlots(AvHTechSlots(AVH_USER3_MENU_ORDERS, BUILD_HEALTH));
		}
		else
		{
            // These upgrades are per alien
            this->AddTechNode(ALIEN_LIFEFORM_ONE, TECH_SKULK, TECH_NULL, TECH_NULL, true, true);
            this->AddTechNode(ALIEN_LIFEFORM_TWO, TECH_GORGE, TECH_NULL, TECH_NULL, true, true);
            this->AddTechNode(ALIEN_LIFEFORM_THREE, TECH_LERK, TECH_SKULK, TECH_NULL, true, true);
            this->AddTechNode(ALIEN_LIFEFORM_FOUR, TECH_FADE, TECH_LERK, TECH_NULL, true, true);
            this->AddTechNode(ALIEN_LIFEFORM_FIVE, TECH_ONOS, TECH_FADE, TECH_NULL, true, true);

            // Only gorge can build
            this->AddTechNode(ALIEN_BUILD_RESOURCES, TECH_ALIEN_RESOURCE_NODE, TECH_GORGE, TECH_NULL, true, false);
            this->AddTechNode(ALIEN_BUILD_HIVE, TECH_HIVE, TECH_GORGE, TECH_NULL, true, false);
            this->AddTechNode(ALIEN_BUILD_MOVEMENT_CHAMBER, TECH_MOVEMENT_CHAMBER, TECH_GORGE, TECH_NULL, true, false);
            this->AddTechNode(ALIEN_BUILD_DEFENSE_CHAMBER, TECH_DEFENSE_CHAMBER, TECH_GORGE, TECH_NULL, true, false);
            this->AddTechNode(ALIEN_BUILD_SENSORY_CHAMBER, TECH_SENSORY_CHAMBER, TECH_GORGE, TECH_NULL, true, false);
            this->AddTechNode(ALIEN_BUILD_OFFENSE_CHAMBER, TECH_OFFENSE_CHAMBER, TECH_GORGE, TECH_NULL, true, false);

            // Upgrades
            this->AddTechNode(ALIEN_EVOLUTION_ONE, TECH_DEFENSE_CHAMBER, TECH_NULL, TECH_NULL, true, false);
            this->AddTechNode(ALIEN_EVOLUTION_TWO, TECH_DEFENSE_CHAMBER, TECH_NULL, TECH_NULL, true, false);
            this->AddTechNode(ALIEN_EVOLUTION_THREE, TECH_DEFENSE_CHAMBER, TECH_NULL, TECH_NULL, true, false);

            this->AddTechNode(ALIEN_EVOLUTION_SEVEN, TECH_MOVEMENT_CHAMBER, TECH_NULL, TECH_NULL, true, false);
            this->AddTechNode(ALIEN_EVOLUTION_EIGHT, TECH_MOVEMENT_CHAMBER, TECH_NULL, TECH_NULL, true, false);
            this->AddTechNode(ALIEN_EVOLUTION_NINE, TECH_MOVEMENT_CHAMBER, TECH_NULL, TECH_NULL, true, false);

            this->AddTechNode(ALIEN_EVOLUTION_TEN, TECH_SENSORY_CHAMBER, TECH_NULL, TECH_NULL, true, false);
            this->AddTechNode(ALIEN_EVOLUTION_ELEVEN, TECH_SENSORY_CHAMBER, TECH_NULL, TECH_NULL, true, false);
            this->AddTechNode(ALIEN_EVOLUTION_TWELVE, TECH_SENSORY_CHAMBER, TECH_NULL, TECH_NULL, true, false);
		}
	}
}

const AvHTechTree& AvHTeam::GetTechNodes() const
{
	return this->mResearchManager.GetTechNodes();
}

AvHTechTree& AvHTeam::GetTechNodes()
{
	return this->mResearchManager.GetTechNodes();
}

const AvHTechSlotManager& AvHTeam::GetTechSlotManager() const
{
	return this->mTechSlotManager;
}

AvHTechSlotManager& AvHTeam::GetTechSlotManager()
{
	return this->mTechSlotManager;
}

// For marine teams, this means a player is trying to vote down the commander
bool AvHTeam::PlayerVote(int inPlayerIndex, string& outPlayerMessage)
{
	bool theSuccess = false;
	
	// If we are a marine team and we have a commander
	if((this->GetTeamType() == AVH_CLASS_TYPE_MARINE) && (this->GetCommander() != -1) && GetGameRules()->GetGameStarted())
	{
		// If player is allowed to vote (can only vote every x minutes)
		EntityListType::iterator theIterator = std::find(this->mVotingPlayers.begin(), this->mVotingPlayers.end(), inPlayerIndex);
		if(theIterator == this->mVotingPlayers.end())
		{
			AvHPlayer* theVotingPlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(inPlayerIndex)));
			if(theVotingPlayer)
			{
				theVotingPlayer->SendMessage(kVoteCast);
				
				// Increment vote
				this->mVotes++;
				this->mVotingPlayers.push_back(inPlayerIndex);
				theSuccess = true;

				AvHPlayer* theCommander = GetCommanderPlayer();
				if(theCommander)
				{
					theCommander->LogPlayerActionPlayer(theVotingPlayer, "votedown");
				}
			}
		}
		else
		{
			// Display message that player has already voted
			outPlayerMessage = kAlreadyVoted;
		}
	}
	else
	{
		outPlayerMessage = kVoteIllegal;
	}
	
	return theSuccess;
}

void AvHTeam::PlayFunHUDSoundOnce(AvHHUDSound inHUDSound)
{
	if(std::find(this->mFunSoundsPlayed.begin(), this->mFunSoundsPlayed.end(), inHUDSound) == this->mFunSoundsPlayed.end())
	{
		this->PlayHUDSoundForAlivePlayers(inHUDSound);
		this->mFunSoundsPlayed.push_back(inHUDSound);
	}
}

void AvHTeam::PlayHUDSoundForAlivePlayers(AvHHUDSound inHUDSound) const
{
	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
	if(theEntity->GetIsRelevant() && (theEntity->pev->team == this->mTeamNumber))
	{
		theEntity->PlayHUDSound(inHUDSound);
	}
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
}

// joev: Bug 0000767
void AvHTeam::PlayHUDSoundForAllPlayers(AvHHUDSound inHUDSound) const
{
	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		theEntity->PlayHUDSound(inHUDSound);
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
}
// :joev


// Assumes rank change has been approved from on high.  It just does the best job it can,
// it doesn't change any other ranks than the one indicated
bool AvHTeam::ProcessRankChange(AvHPlayer* inPlayer, AvHUser3 inOldUser3, AvHUser3 inNewUser3)
{
	bool theSuccess = false;

	// Someone just stepped up or down, change the player's squad
	int thePlayerIndex = inPlayer->entindex();

	// if player is now a soldier
	if(inNewUser3 == AVH_USER3_MARINE_PLAYER)
	{
		if(this->GetCommander() == thePlayerIndex)
		{
			this->SetCommander(-1);
		}
	}
	// else if player is now commander
	else if(inNewUser3 == AVH_USER3_COMMANDER_PLAYER)
	{
		// Set him as commander
		this->SetCommander(thePlayerIndex);
		
		theSuccess = true;
	}

	return theSuccess;
}

void AvHTeam::ResetGame()
{
	AvHTeamNumber theTeamNumber = this->mTeamNumber;
	AvHClassType theTeamType = this->mTeamType;
	
	this->Init();
	
	this->mTeamNumber = theTeamNumber;
	this->mTeamType = theTeamType;
	
	this->mLastInjectionTime = gpGlobals->time;
	this->mLastResourceUpdateTime = -1;
	this->mLastCommandScoreUpdateTime = -1;
	this->mStartingLocation.x = this->mStartingLocation.y = this->mStartingLocation.z = 0;
	this->mNumBuiltCommandStations = 0;
	this->mNumActiveHives = 0;
	
	if(this->mTeamType == AVH_CLASS_TYPE_ALIEN)
	{
		// Spawn the initial hives
		int theNumHives = GetGameRules()->GetGameplay().GetInitialHives();
		for(int i = 0; i < theNumHives; i++)
		{
			this->SpawnHive(&this->mStartingLocation, true);
		}

		if(GetGameRules()->GetIsCombatMode())
		{
			for(int i= 0; i < 3; i++)
			{
				this->AddTeamUpgrade(ALIEN_UPGRADE_CATEGORY_DEFENSE);
				this->AddTeamUpgrade(ALIEN_UPGRADE_CATEGORY_MOVEMENT);
				this->AddTeamUpgrade(ALIEN_UPGRADE_CATEGORY_SENSORY);
			}
		}
	}
	
	if(!GetGameRules()->GetIsCombatMode())
	{
		// Put down command station
		FOR_ALL_ENTITIES(kwsTeamCommand, AvHCommandStation*)
			if(theEntity->pev->team == (int)(this->mTeamNumber))
			{
				theEntity->SetConstructionComplete(true);
				DROP_TO_FLOOR(ENT(theEntity->pev));
				//PLAYBACK_EVENT_FULL(0, theEntity->edict(), gPhaseInEventID, 0, theEntity->pev->origin, (float *)&g_vecZero, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
				VectorCopy(theEntity->pev->origin, this->mStartingLocation);
			}
		END_FOR_ALL_ENTITIES(kwsTeamCommand)

		if(this->mTeamType == AVH_CLASS_TYPE_MARINE)
		{
			this->SetTeamResources(GetGameRules()->GetGameplay().GetInitialMarinePoints());
		}
	//	else if(this->mTeamType == AVH_CLASS_TYPE_ALIEN)
	//	{
	//		this->SetTeamResources(GetGameRules()->GetGameplay().GetInitialAlienPoints());
	//	}

		this->SpawnResourceTower();
	}
		
	this->mOrderList.clear();
	
	this->ResetServerPlayerData();
	//this->InitializeRandomResourceShares();

	this->mHiveInfoList.clear();
	this->mTimeOfLastStructureUpdate = -1;

	this->mVoteStarted = false;
	this->mPreviousVotes = this->mVotes = 0;
	this->mVotingPlayers.clear();
	this->mTimeVoteStarted = -1;
	this->mVotesNeeded = 0;


	this->mLowResourceAlerts.clear();
	this->mCommandersToldAboutJumpKey.clear();
	this->mFunSoundsPlayed.clear();
	this->mPlayedSeeDeadPeople = false;

	int theEasterEggChance = max(0.0f, avh_eastereggchance.value);
	this->mPlayFunSoundsThisGame = (RANDOM_LONG(0, theEasterEggChance) == 0);

	// Don't give hints right away
	this->mTimeOfLastTeamHint = gpGlobals->time;
	this->mTimeLastHintUpdate = -1;

	// Reset group info
	for(int i = 0; i < kNumHotkeyGroups; i++)
	{
		this->mGroups[i].clear();
		this->mGroupTypes[i] = AVH_USER3_NONE;
	}
	this->mSelectAllGroup.clear();
}

void AvHTeam::SpawnResourceTower()
{
	string theResourceEntity;
	AvHMessageID theResourceEntityMessageID = MESSAGE_NULL;

	if(this->mTeamType == AVH_CLASS_TYPE_MARINE)
	{
		theResourceEntity = kwsResourceTower;
		theResourceEntityMessageID = BUILD_RESOURCES;
	}
	else if(this->mTeamType == AVH_CLASS_TYPE_ALIEN)
	{
		theResourceEntity = kwsAlienResourceTower;
		theResourceEntityMessageID = ALIEN_BUILD_RESOURCES;	
	}

	// Create a resource tower for the team, nearest their spawn
	Vector theLocationToBuild;
	
	// Look for func resource nearest to team start
	CBaseEntity* theNearestFuncResource = NULL;
	float theNearestDistance = -1;
	
	FOR_ALL_ENTITIES(kesFuncResource, AvHFuncResource*)
		if(!theEntity->GetIsOccupied())
		{
			float theDistance = VectorDistance(this->mStartingLocation, theEntity->pev->origin);
			if((theNearestDistance == -1) || (theDistance < theNearestDistance))
			{
				theNearestFuncResource = theEntity;
				theNearestDistance = theDistance;
			}
		}
	END_FOR_ALL_ENTITIES(kesFuncResource);

	if(theNearestFuncResource)
	{
		Vector theLocation = theNearestFuncResource->pev->origin;
		theLocation.z += 35;

		CBaseEntity* theBaseEntity = CBaseEntity::Create(theResourceEntity.c_str(), theLocation, AvHSUGetRandomBuildingAngles());
			
		theBaseEntity->pev->team = this->mTeamNumber;

		AvHSUBuildingJustCreated(theResourceEntityMessageID, theBaseEntity, NULL);

		// Set it complete immediately.  If not, aliens are slowed down noticeably because they need builders to tower building
		AvHResourceTower* theResourceTower = dynamic_cast<AvHResourceTower*>(theBaseEntity);
		ASSERT(theResourceTower);
		// The first resource tower is active immediately
		theResourceTower->SetActivateTime(0);

		//DROP_TO_FLOOR(ENT(theBaseBuildable->pev));
		theResourceTower->SetConstructionComplete(true);
	}
}

// Delete any server player data that has expired
void AvHTeam::ResetServerPlayerData()
{
	// Run through list
	for(AvHServerPlayerDataListType::iterator theIter = this->mServerPlayerData.begin(); theIter != this->mServerPlayerData.end(); /* no inc */)
	{
		// Delete it unless the player has been voted down
		if(theIter->second.GetTimeVotedDown() == -1)
		{
			AvHServerPlayerDataListType::iterator theTempIter = theIter;
			theTempIter++;
			this->mServerPlayerData.erase(theIter);
			theIter = theTempIter;
		}
		else
		{
			theIter++;
		}
	}
}

int	AvHTeam::GetDesiredSpawnCount() const
{
	int theDesiredSpawns = 0;

	if(this->mTeamType == AVH_CLASS_TYPE_MARINE)
	{
		// 16 for the marine start
		theDesiredSpawns = 16;
	}
	else if(this->mTeamType == AVH_CLASS_TYPE_ALIEN)
	{
		// 16 for each hive
		theDesiredSpawns = 16*3;
	}

	return theDesiredSpawns;
}

float AvHTeam::GetInitialPlayerPoints(edict_t* inEdict)
{
	float theInitialPoints = 0;

	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(inEdict));
	ASSERT(thePlayer);

	// Marines don't get any individual points, aliens get random amount
	// Use WONID to make sure player doesn't try to get more points by quitting and rejoining
	if(this->mTeamType == AVH_CLASS_TYPE_ALIEN)
	{
		if(GetGameRules()->GetArePlayersAllowedToJoinImmediately() /*&& !thePlayer->GetHasLeftReadyRoom()*/)
		{
			theInitialPoints = GetGameRules()->GetGameplay().GetInitialAlienPoints();
		}

		// If WON id already exists (because we already gave points out to that player)
		AvHServerPlayerData* theServerPlayerData = this->GetServerPlayerData(inEdict);
		if(theServerPlayerData && (theServerPlayerData->GetResources() != -1))
		{
			// Give them the number of points they last had
			theInitialPoints = theServerPlayerData->GetResources();
		}
	}

	return theInitialPoints;
}

float AvHTeam::GetInitialExperience(edict_t* inEdict)
{
	float theInitialExperience = 0.0f;

	//AvHServerPlayerData* theServerPlayerData = this->GetServerPlayerData(inEdict);
	//if(theServerPlayerData && (theServerPlayerData->GetExperience() > 0.0f))
	//{
	//	// Give them experience they last had
	//	theInitialExperience = theServerPlayerData->GetExperience();
	//}

	return theInitialExperience;
}

void AvHTeam::SetGameStarted(bool inGameStarted)
{
	if(inGameStarted)
	{
		if(this->mTeamType == AVH_CLASS_TYPE_ALIEN)
		{
			this->mLastHiveSpawnTime = gpGlobals->time;
		}
		this->mLastInjectionTime = gpGlobals->time;

		//#ifndef DEBUG
		// If team is alien, give them all game started hud sound
		FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
			if(theEntity->pev->team == this->mTeamNumber)
			{
				theEntity->PlayHUDSound(HUD_SOUND_GAMESTART);

				// Send Combat objective
				if(GetGameRules()->GetIsCombatMode())
				{
					string theMessage;
					if(GetGameRules()->GetCombatAttackingTeamNumber() == this->mTeamNumber)
					{
						theMessage = kYouAreAttacking;

					}
					else
					{
						theMessage = kYouAreDefending;
					}

					theEntity->SendMessage(theMessage.c_str(), NORMAL);
				}
			}
		END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
		//#endif
	}
}

void AvHTeam::KillCS()
{
	AvHCommandStation* theCS = NULL;
	
	FOR_ALL_ENTITIES(kwsTeamCommand, AvHCommandStation*)
		if(theEntity->pev->health > 0)
		{
			theEntity->TakeDamage(theEntity->pev, theEntity->pev, 20000, DMG_GENERIC);
		}
	END_FOR_ALL_ENTITIES(kwsTeamCommand)
}

void AvHTeam::KillHive()
{
	if(this->mTeamType == AVH_CLASS_TYPE_ALIEN)
	{
		AvHHive* theHive = AvHSUGetRandomActiveHive(this->mTeamNumber);
		if(theHive)
		{
			theHive->Killed(NULL, 0);
		}
	}
}

void AvHTeam::SpawnHive(Vector* outLocation, bool inForce)
{
	if(this->mTeamType == AVH_CLASS_TYPE_ALIEN)
	{
		AvHHive* theHive = AvHSUGetRandomActivateableHive(this->mTeamNumber);
		if(theHive)
		{
			if(theHive->StartSpawningForTeam(this->mTeamNumber, inForce))
			{
				theHive->SetConstructionComplete(true);
				this->mNumActiveHives++;

				if(outLocation)
				{
					VectorCopy(theHive->pev->origin, *outLocation);
				}
			}
			else
			{
				ALERT(at_console, "Error spawning hive. Is it blocked by something?\n");
			}
		}
	}
}

int	AvHTeam::GetCommander() const
{
	return this->mCommander;
}

AvHPlayer* AvHTeam::GetCommanderPlayer()
{
	AvHPlayer* theCommanderPlayer = NULL;

	if(this->mCommander > 0)
	{
		theCommanderPlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(this->mCommander)));
	}

	return theCommanderPlayer;
}


AvHTeamNumber AvHTeam::GetTeamNumber(void) const
{
	return this->mTeamNumber;
}

void AvHTeam::SetTeamNumber(AvHTeamNumber inTeamNumber)
{
	this->mTeamNumber = inTeamNumber;
}

AvHClassType AvHTeam::GetTeamType(void) const
{
	return this->mTeamType;
}

const char*	AvHTeam::GetTeamTypeString(void) const
{
	const char* theTeamString = "Unknown";

	switch(this->mTeamType)
	{
	case AVH_CLASS_TYPE_MARINE:
		theTeamString = "marine";
		break;
	case AVH_CLASS_TYPE_ALIEN:
		theTeamString = "alien";
		break;
	}

	return theTeamString;
}

void AvHTeam::AddTeamUpgrade(AvHAlienUpgradeCategory inCategory)
{
	int theNumUpgradeCategories = AvHGetNumUpgradesInCategoryInList(this->mAlienUpgrades, inCategory);

	// If we don't have an upgrade of this type, trigger an alert
	if(theNumUpgradeCategories == 0)
	{
		GetGameRules()->TriggerAlert(this->mTeamNumber, ALERT_NEW_TRAIT, -1);
	}

	this->mAlienUpgrades.push_back(inCategory);
}

AvHAlienUpgradeListType	AvHTeam::GetAlienUpgrades() const
{
	return this->mAlienUpgrades;
}

bool AvHTeam::RemoveAlienUpgradeCategory(AvHAlienUpgradeCategory inCategory)
{
	bool theRemoved = AvHRemoveUpgradeCategory(inCategory, this->mAlienUpgrades);
	return theRemoved;
}

int	AvHTeam::GetTeamWideUpgrades() const
{
	return this->mTeamWideUpgrades;
}

void AvHTeam::ProcessTeamUpgrade(AvHMessageID inMessageID, bool inGive)
{
	ProcessGenericUpgrade(this->mTeamWideUpgrades, inMessageID, inGive);
}

void AvHTeam::InitializeRandomResourceShares()
{
	this->mRandomResourceShares.clear();
	this->mTotalShareWeight = 0;

	// Populate random resource shares with random value (like cards in Bridge)
	for(int i = 0; i < kTotalShares; i++)
	{
		const int kSlope = 2;
		int theBoundaryOne = kTotalShares/3;
		int theBoundaryTwo = (2*kTotalShares)/3;
		int theCardValue = 1;

		if((i > theBoundaryOne) && (i < theBoundaryTwo))
		{
			theCardValue = 2*kSlope;
		}
		else if(i > theBoundaryTwo)
		{
			theCardValue = 3*kSlope;
		}

		this->mRandomResourceShares.push_back(theCardValue);
		this->mTotalShareWeight += theCardValue;
	}
}

float AvHTeam::WithdrawPointsViaRandomShares(int inPlayerIndex)
{
	int theNumSharesLeft = this->mRandomResourceShares.size();
	ASSERT(theNumSharesLeft >= kNumSharesPerPlayer);

	// Pick some random shares and add them up
	int theRandomShares = 0;

	for(int i = 0; i < kNumSharesPerPlayer; i++)
	{
		// Pick a random "card", add it's shares, remove card from pool
		int theRandomNumber = g_engfuncs.pfnRandomLong(0, this->mRandomResourceShares.size() - 1);
		RandomResourceShareListType::iterator theIter = this->mRandomResourceShares.begin() + theRandomNumber;

		theRandomShares = theRandomShares + *theIter;

		// Remember which "cards" we've given this player so we can add them back in later
		this->mPlayerShares[inPlayerIndex].push_back(*theIter);

		this->mRandomResourceShares.erase(theIter);
	}

	// Return this percentage of the team resources for this player
	float thePercentage = theRandomShares/(float)this->mTotalShareWeight;

	float theInitialPoints = thePercentage*this->GetTeamResources();
	this->SetTeamResources(this->GetTeamResources() - theInitialPoints);

	return theInitialPoints;
}

void AvHTeam::SetTeamType(AvHClassType inType)
{
	this->mTeamType = inType;

	switch(inType)
	{
	case AVH_CLASS_TYPE_MARINE:
		if( this->GetTeamNumber() == TEAM_ONE )
		{ this->mTeamName = kMarine1Team; }
		else
		{ this->mTeamName = kMarine2Team; }
		this->mTeamPrettyName = kMarinePrettyName;
		break;
	case AVH_CLASS_TYPE_ALIEN:
		if( this->GetTeamNumber() == TEAM_TWO )
		{ this->mTeamName = kAlien1Team; }
		else
		{ this->mTeamName = kAlien2Team; }
		this->mTeamPrettyName = kAlienPrettyName;
		break;
	default:
		this->mTeamName = kUndefinedTeam;
		this->mTeamPrettyName = kUndefPrettyName;
		break;
	}

	// Make sure team names aren't longer than MAX_TEAM_NAME
	ASSERT(this->mTeamName.length() < MAX_TEAM_NAME);
}

int	AvHTeam::GetNumWebStrands() const
{
	return this->mNumWebStrands;
}

void AvHTeam::SetNumWebStrands(int inNumWebStrands)
{
	this->mNumWebStrands = inNumWebStrands;
}

float AvHTeam::GetTeamResources() const
{
	return this->mTeamResources;
}

void AvHTeam::SetTeamResources(float inTeamResources)
{
	this->mTeamResources = inTeamResources;
}

float AvHTeam::GetHandicap() const
{
	return this->mHandicap;
}

void AvHTeam::SetHandicap(float inHandicap)
{
	this->mHandicap = inHandicap;
}

const char*	AvHTeam::GetTeamName(void) const
{
	return this->mTeamName.c_str();
}

const char*	AvHTeam::GetTeamPrettyName(void) const
{
	return this->mTeamPrettyName.c_str();
}

void AvHTeam::SetCommander(int inPlayerNumber)
{
	if(inPlayerNumber != this->mCommander)
	{
		// Reset commander hints
		this->mTimeOfLastTeamHint = -1;

		this->mCommander = inPlayerNumber;
	}
}

void AvHTeam::SetTeamName(const char* inTeamName)
{
	this->mTeamName = inTeamName;
}

void AvHTeam::SetTeamPrettyName(const char* inTeamName)
{
	this->mTeamPrettyName = inTeamName;
}

int AvHTeam::GetPlayerCount(bool inCountOnlyDead) const
{
	int theNumPlayers = this->mPlayerList.size();

	if(inCountOnlyDead)
	{
		theNumPlayers = 0;

		FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		if((AvHTeamNumber)theEntity->pev->team == this->mTeamNumber)
		{
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
			ASSERT(thePlayer);

			if(!thePlayer->IsAlive())
			{
				theNumPlayers++;
			}
		}
		END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
	}

	return theNumPlayers;
}

bool AvHTeam::RemovePlayer(int inPlayerIndex)
{
	bool theSuccess = false;
	
	if(this->GetIsPlayerOnTeam(inPlayerIndex))
	{
		// Remove player
		EntityListType::iterator thePlayerIter = std::find(this->mPlayerList.begin(), this->mPlayerList	.end(), inPlayerIndex);
		if(thePlayerIter != this->mPlayerList.end())
		{
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(inPlayerIndex)));
			ASSERT(thePlayer);

			// Add cards back into pool
//			RandomResourceShareListType& theVector = this->mPlayerShares[inPlayerIndex];
//			for(RandomResourceShareListType::iterator theCardIter = theVector.begin(); theCardIter != theVector.end(); theCardIter++)
//			{
//				this->mRandomResourceShares.push_back(*theCardIter);
//			}

			// Make sure no entities have this player as an owner
			edict_t* thePlayerEdict = thePlayer->edict();
			FOR_ALL_BASEENTITIES();
				if(theBaseEntity->pev->owner == thePlayerEdict)
				{
					theBaseEntity->pev->owner = NULL;
				}

				AvHBuildable* theBuildable = dynamic_cast<AvHBuildable*>(theBaseEntity);
				if(theBuildable && (theBuildable->GetBuilder() == thePlayer->entindex()))
				{
					theBuildable->SetBuilder(-1);
				}
			END_FOR_ALL_BASEENTITIES();

			AvHSURemoveEntityFromHotgroupsAndSelection(inPlayerIndex);
			
//			theVector.clear();
			
			this->mPlayerList.erase(thePlayerIter);
			
			theSuccess = true;
		}
	}
	
	if(this->mCommander == inPlayerIndex)
	{
		this->SetCommander(-1);
	}
	
	return theSuccess;
}

void AvHTeam::TriggerAddTech(AvHTechID inTechID)
{
	if(!GetGameRules()->GetIsCombatMode())
	{
		this->mResearchManager.TriggerAddTech(inTechID);

		// Get list of completed research that depended on this tech
		TechNodeMap nodes = this->mResearchManager.GetResearchNodesDependentOn(inTechID);
		
		// For all researched nodes
		TechNodeMap::const_iterator current, end = nodes.end();
		for( current = nodes.begin(); current != end; ++current )
		{
			if( current->second->getIsResearched() )
			{
				GetGameRules()->ProcessTeamUpgrade( current->first, this->mTeamNumber, 0, true );
			}
		}
	}
}

void AvHTeam::TriggerRemoveTech(AvHTechID inTechID)
{
	if(!GetGameRules()->GetIsCombatMode())
	{
		// Run through world, looking for anything alive that could still be giving us this tech
		bool theFoundActiveTech = false;

		FOR_ALL_BASEENTITIES();
			AvHBuildable* theBuildable = dynamic_cast<AvHBuildable*>(theBaseEntity);
			if(theBuildable)
			{
				if((theBuildable->GetSupportsTechID(inTechID)) && (theBuildable->GetTeamNumber() == this->mTeamNumber))
				{
					if(theBuildable->GetIsTechActive())
					{
						theFoundActiveTech = true;
						break;
					}
				}
			}
		END_FOR_ALL_BASEENTITIES();

		if(!theFoundActiveTech)
		{
			// If we can't find anything, remove it from the tech nodes
			this->mResearchManager.TriggerRemoveTech(inTechID);

			// Get list of completed research that depended on this tech
			TechNodeMap nodes = this->mResearchManager.GetResearchNodesDependentOn(inTechID);

			// For all researched nodes
			TechNodeMap::iterator current, end = nodes.end();
			for( current = nodes.begin(); current != end; ++current )
			{
				if( current->second->getIsResearched() )
				{
					// Disable because of loss of this tech and remove them
					GetGameRules()->ProcessTeamUpgrade( current->first, this->mTeamNumber, 0, false );
				}
			}
		}
	}
}

void AvHTeam::Update()
{
    PROFILE_START()
	this->UpdateHints();
    PROFILE_END(kAvHTeamUpdateHints)

    PROFILE_START()
	this->UpdateInventoryEnabledState();
    PROFILE_END(kAvHTeamUpdateInventoryEnabledState)

    PROFILE_START()
	this->UpdateReinforcementWave();
    PROFILE_END(kAvHTeamUpdateReinforcementWave)

    PROFILE_START()
	this->UpdateOrders();
    PROFILE_END(kAvHTeamUpdateOrders)

    PROFILE_START()
	this->UpdateResearch();
    PROFILE_END(kAvHTeamUpdateResearch)

    PROFILE_START()
	this->UpdateResources();
    PROFILE_END(kAvHTeamUpdateResources)

    PROFILE_START()
	this->UpdateAlerts();
    PROFILE_END(kAvHTeamUpdateAlerts)

    PROFILE_START()
	this->UpdateServerPlayerData();
    PROFILE_END(kAvHTeamUpdateServerPlayerData)

    PROFILE_START()
	this->UpdateTeamStructures();
    PROFILE_END(kAvHTeamUpdateTeamStructures)

    PROFILE_START()
	this->UpdateVoting();
    PROFILE_END(kAvHTeamUpdateVoting)

    PROFILE_START()
	this->UpdatePlayers();
    PROFILE_END(kAvHTeamUpdatePlayers)
}

void AvHTeam::UpdateTeamStructures()
{
	const float kStructureUpdateRate = 1.0f;
	if((this->mTimeOfLastStructureUpdate == -1) || (gpGlobals->time > (this->mTimeOfLastStructureUpdate + kStructureUpdateRate)))
	{
		this->mTimeOfLastStructureUpdate = gpGlobals->time;

		if(this->GetTeamType() == AVH_CLASS_TYPE_ALIEN)
		{
			this->mHiveInfoList.clear();

			// Search for free hives, or hives on our team
			FOR_ALL_ENTITIES(kesTeamHive, AvHHive*)
				if((theEntity->pev->team == 0) || ((AvHTeamNumber)theEntity->pev->team == this->mTeamNumber))
				{
					AvHHiveInfo theHiveInfo;

					theHiveInfo.mPosX = theEntity->pev->origin.x;
					theHiveInfo.mPosY = theEntity->pev->origin.y;
					theHiveInfo.mPosZ = theEntity->pev->origin.z;

					if (theEntity->pev->max_health > 0) {
						theHiveInfo.mHealthPercentage = (int)(100 * theEntity->pev->health / theEntity->pev->max_health);
					} else {
						theHiveInfo.mHealthPercentage = 0;
					}

					int theEntityIndex = theEntity->entindex();

					// Fill in hive status
					int theStatus = kHiveInfoStatusBuilt;
					
					// Unbuilt hives
					if(theEntity->pev->team == 0)
					{
						theStatus = kHiveInfoStatusUnbuilt;
					}
					// building hives
					else if(!theEntity->GetIsBuilt())
					{
						float theNormalizedBuildPercentage = theEntity->GetNormalizedBuildPercentage();
						if(theNormalizedBuildPercentage > 0.0f)
						{
							theStatus = kHiveInfoStatusBuildingStage1;
							int theSteps = (int)(theNormalizedBuildPercentage*5.0f);
							theStatus += theSteps;
						}
						else
						{
							theStatus = kHiveInfoStatusUnbuilt;
						}
					}
					
					theHiveInfo.mStatus = theStatus;
					theHiveInfo.mTechnology = (int)(theEntity->GetTechnology());
					
					// Under attack?
					theHiveInfo.mUnderAttack = GetGameRules()->GetIsEntityUnderAttack(theEntityIndex);
					
					// Add onto the end, so we don't resend unnecessarily
					this->mHiveInfoList.insert(this->mHiveInfoList.end(), theHiveInfo);
				}
			END_FOR_ALL_ENTITIES(kesTeamHive);
				
			// Count number of active hives
			int theNumHives = 0;
			FOR_ALL_ENTITIES(kesTeamHive, AvHHive*)
				if(theEntity->GetIsActive())
				{
					AvHTeamNumber theTeamNumber = (AvHTeamNumber)(theEntity->pev->team);
					if(theTeamNumber == this->mTeamNumber)
					{
						theNumHives++;
					}
				}
			END_FOR_ALL_ENTITIES(kesTeamHive);
			this->mNumActiveHives = theNumHives;
		}
		else if(this->GetTeamType() == AVH_CLASS_TYPE_MARINE)
		{
			// Count # of command stations
			int theNumActiveCommandStations = 0;
			FOR_ALL_ENTITIES(kwsTeamCommand, AvHCommandStation*)
				if(theEntity->GetIsBuilt())
				{
					AvHTeamNumber theTeamNumber = (AvHTeamNumber)(theEntity->pev->team);
					if(theTeamNumber == this->mTeamNumber)
					{
						theNumActiveCommandStations++;
					}
				}
			END_FOR_ALL_ENTITIES(kwsTeamCommand);
			this->mNumBuiltCommandStations = theNumActiveCommandStations;
		}
	}
}

void AvHTeam::UpdatePlayers()
{
	//FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
	//	GetGameRules()->ClientCommand(theEntity, );
	//END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
		
	// Update every once in awhile
	const float kPlayerUpdateRate = 2.0f;
	if((this->mLastPlayerUpdateTime == -1) || (gpGlobals->time > (this->mLastPlayerUpdateTime + kPlayerUpdateRate)))
	{
		if(!GetGameRules()->GetCheatsEnabled() && !GetGameRules()->GetIsTournamentMode() && !GetGameRules()->GetIsCombatMode())
		{
			// If the team is alien and has no hives left
			if(this->mTeamType == AVH_CLASS_TYPE_ALIEN)
			{
				// Do we have any active hives?
				int theNumGrowingOrActiveHives = 0;
				FOR_ALL_ENTITIES(kesTeamHive, AvHHive*)
					AvHTeamNumber theTeamNumber = (AvHTeamNumber)(theEntity->pev->team);
				if(theTeamNumber == this->mTeamNumber)
				{
					if(theEntity->GetIsActive() || theEntity->GetIsSpawning())
					{
						theNumGrowingOrActiveHives++;
					}
				}
				END_FOR_ALL_ENTITIES(kesTeamHive);
				
				if(theNumGrowingOrActiveHives == 0)
				{
					CBaseEntity* theWorld = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(0));
					if(theWorld)
					{
						entvars_t* theInflictor = theWorld->pev;
						
						// Do some damage to remaining players
						FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
							if((theEntity->pev->team == this->mTeamNumber) && theEntity->IsAlive())
							{
								AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
								ASSERT(thePlayer);
								float theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(theEntity->pev->iuser4, theEntity->GetUser3(), thePlayer->GetExperienceLevel());
								float theDamage = theMaxHealth/8;
								
								theEntity->TakeDamage(theInflictor, theInflictor, theDamage, DMG_DROWN | DMG_IGNOREARMOR);
							}
							theEntity->PlayHUDSound(HUD_SOUND_COUNTDOWN);
							
						END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
					}
				}
			}
		}
		
		this->mLastPlayerUpdateTime = gpGlobals->time;
	}
	
	// Update team score if we're in tournament mode
	this->SendResourcesGatheredScore(true);
}

bool AvHTeam::SendResourcesGatheredScore(bool inThisTeamOnly)
{
	bool theSentMessage = false;

	if(GetGameRules()->GetIsTournamentMode())
	{
		// Only send message to players on our team, or spectating players
		int theTeamScore = (int)this->GetTotalResourcesGathered();
		if(theTeamScore != this->mClientTotalResourcesGathered)
		{
			if(inThisTeamOnly)
			{
				for(EntityListType::iterator thePlayerIter = this->mPlayerList.begin(); thePlayerIter != this->mPlayerList.end(); thePlayerIter++)
				{
					AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(*thePlayerIter)));
					if(thePlayer)
					{
						NetMsg_TeamScore( thePlayer->pev, this->GetTeamName(), theTeamScore, 0 );
					}
				}
			}
			else
			{
				NetMsg_TeamScore( this->GetTeamName(), theTeamScore, 0 );
			}
						
			this->mClientTotalResourcesGathered = theTeamScore;

			theSentMessage = true;
		}
	}
	
	return theSentMessage;
}


void AvHTeam::UpdateVoting()
{
	string theMessageForPlayers;
	AvHHUDSound theSoundForPlayers = HUD_SOUND_INVALID;
	bool theSendOnlyToNonvotingPlayers = false;
	
	// If we have more then 1 vote against commander
	if(this->mVotes > 0)
	{
		// If a vote hasn't started
		if(!this->mVoteStarted)
		{
			// Tell all players on team that a vote has started
			theMessageForPlayers = kVoteStarted;
			theSendOnlyToNonvotingPlayers = true;
			
			// Start vote
			this->mVoteStarted = true;
			this->mCommanderWhenVoteStarted = this->mCommander;
			this->mTimeVoteStarted = gpGlobals->time;
		}
		else
		{
			bool theResetVote = false;

			// If commander logs out after vote started, cancel vote
			if(this->mCommander != this->mCommanderWhenVoteStarted)
			{
				theMessageForPlayers = kVoteCancelled;
				theResetVote = true;
			}
			else
			{
				// If we have enough votes to kick commander
				float theVotePercent = avh_votepercentneeded.value;
				int theMinVotesRequired = (int)avh_minvotesneeded.value;
				int theVotesNeeded = max((float)theMinVotesRequired, this->mPlayerList.size()*theVotePercent);
				
				// #545: If the voting has changed, indicate so in the HUD
				if ((this->mVotesNeeded != theVotesNeeded) || (this->mVotes != this->mPreviousVotes)) {

					// tankefugl: 0000545
					char theVoteMessage[512];
					sprintf(theVoteMessage, "Eject commander: %d of %d votes needed.", this->mVotes, theVotesNeeded);

					FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
					if(theEntity->pev->team == this->mTeamNumber)
					{
						UTIL_SayText(theVoteMessage, theEntity); // , theEntity->entindex());
					}
					END_FOR_ALL_ENTITIES(kAvHPlayerClassName);


					// UTIL_ClientPrintAll(HUD_PRINTTALK, theVoteMessage);
					UTIL_LogPrintf(theVoteMessage);

					this->mVotesNeeded = theVotesNeeded;
					this->mPreviousVotes = this->mVotes;
					// :tankefugl
				}

				if(this->mVotes >= theVotesNeeded)
				{
					// Kick commander
					AvHPlayer* theCommander = this->GetCommanderPlayer();
					if(theCommander)
					{
						theCommander->StopTopDownMode();
						theCommander->SetUser3(AVH_USER3_MARINE_PLAYER);
				
						// Remember when we were voted down, so they can't command again
						AvHServerPlayerData* theServerPlayerData = this->GetServerPlayerData(theCommander->edict());
						if(theServerPlayerData)
						{
							theServerPlayerData->SetTimeVotedDown(gpGlobals->time);
						}
						
						// Tell all players on team that commander has been ejected
						theMessageForPlayers = kVoteSucceeded;
						theSoundForPlayers = HUD_SOUND_MARINE_COMMANDER_EJECTED;
						theResetVote = true;
					}
				}
				else
				{
					// else if enough time has elapsed without a vote going through
					int theVoteCastTimeInSeconds = avh_votecasttime.value*60;
					if(gpGlobals->time > (this->mTimeVoteStarted + theVoteCastTimeInSeconds))
					{
						// Tell all players on team that the vote has failed
						theMessageForPlayers = kVoteFailed;
						theResetVote = true;
					}
				}
			}
			
			if(theResetVote)
			{
				// Reset vote (don't reset players that voted, they can only vote once per game)
				this->mVotes = 0;
				this->mVoteStarted = false;
				this->mTimeVoteStarted = -1;
				// clear the list of voting players to allow more than one vote each game (#545)
				this->mVotingPlayers.clear();
				this->mVotesNeeded = 0;
				this->mPreviousVotes = 0;
			}
		}
	}
	
	// Send message to players
	if(theMessageForPlayers != "")
	{
		// Send message to players on team
		FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
			if(theEntity->GetTeam() == this->GetTeamNumber())
			{
				int thePlayerIndex = theEntity->entindex();
				EntityListType::iterator theIterator = std::find(this->mVotingPlayers.begin(), this->mVotingPlayers.end(), thePlayerIndex);
				bool thePlayerHasVoted = (theIterator != this->mVotingPlayers.end());

				if(!theSendOnlyToNonvotingPlayers || !thePlayerHasVoted)
				{
					theEntity->SendMessage(theMessageForPlayers.c_str());

					// Play "commander has been ejected" sound for all players
					if(theSoundForPlayers != HUD_SOUND_INVALID)
					{
						theEntity->PlayHUDSound(theSoundForPlayers);
					}
				}
			}
		END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
	}
}

void AvHTeam::UpdateHints()
{
	if(GetGameRules()->GetGameStarted() && (this->GetPlayerCount() > 1) && !GetGameRules()->GetIsCombatMode())
	{
		const float kHintUpdateInterval = 2.0f;
		if((this->mTimeLastHintUpdate == -1) || (gpGlobals->time > (this->mTimeLastHintUpdate + kHintUpdateInterval)))
		{
			this->mTimeLastHintUpdate = gpGlobals->time;

			float kHintInterval = 20.0f;
			if(this->GetTeamType() == AVH_CLASS_TYPE_ALIEN)
			{
				kHintInterval = 30.0f;
			}
		
			if((this->mTimeOfLastTeamHint == -1) || (gpGlobals->time > (this->mTimeOfLastTeamHint + kHintInterval)))
			{
				bool thePlayedHint = false;
			
				// Update commander hints
				AvHPlayer* theCommanderPlayer = this->GetCommanderPlayer();
				if(theCommanderPlayer)
				{
					const float kGiveOrdersHintDelay = 20.0f;
					float theTimeAsCommander = gpGlobals->time - theCommanderPlayer->GetTimeStartedTopDown();
				
					if(theTimeAsCommander > kGiveOrdersHintDelay)
					{
						// Remove warning because tech tree doesn't currently require it
						//// Do we have zero infantry portals, and we haven't played "need infantry portal" sound for awhile
						//bool theTeamHasLiveInfantryPortal = false;
						//
						//FOR_ALL_ENTITIES(kwsInfantryPortal, AvHInfantryPortal*)
						//	if(theEntity->pev->team == this->mTeamNumber)
						//	{
						//		if(theEntity->GetIsBuilt())
						//		{
						//			theTeamHasLiveInfantryPortal = true;
						//			break;
						//		}
						//	}
						//END_FOR_ALL_ENTITIES(kwsInfantryPortal);
						//
						//if(!theTeamHasLiveInfantryPortal)
						//{
						//	// Play "need infantry portal" sound
						//	theCommanderPlayer->PlayHUDSound(HUD_SOUND_MARINE_NEEDPORTAL);
						//	thePlayedHint = true;
						//}
						
						if(!thePlayedHint)
						{
							// Did commander just join and hasn't given any orders?  Play "select your troops and give them orders".
							if(!theCommanderPlayer->GetHasGivenOrder())
							{
								theCommanderPlayer->PlayHUDSound(HUD_SOUND_MARINE_GIVEORDERS);
								thePlayedHint = true;
							}
						}
						
						if(!thePlayedHint)
						{
							// If we just played an alert, tell them how to go to it (but only tell every commander once)
							const float kMinAlertNotifyInterval = 5.0f;
							float theTimeOfLastAudioAlert = -1;
                            const AvHAlert* theLastAlert = this->GetLastAudioAlert();
                            if(theLastAlert)
                            {
                                theTimeOfLastAudioAlert = theLastAlert->GetTime();
                            }

							if((theTimeOfLastAudioAlert == -1) || ((gpGlobals->time - theTimeOfLastAudioAlert) < kMinAlertNotifyInterval))
							{
								int theCurrentCommander = theCommanderPlayer->entindex();
								if(std::find(this->mCommandersToldAboutJumpKey.begin(), this->mCommandersToldAboutJumpKey.end(), theCurrentCommander) == this->mCommandersToldAboutJumpKey.end())
								{
									theCommanderPlayer->PlayHUDSound(HUD_SOUND_MARINE_GOTOALERT);
									this->mCommandersToldAboutJumpKey.push_back(theCurrentCommander);
								}
							}
						}
						
						// Detect shell-shocked commander.  Play "do something" sound.
						if(!thePlayedHint)
						{
							const float kShellShockedInterval = 130.0f;
						
							float theTimeOfLastSignificantCommanderAction = theCommanderPlayer->GetTimeOfLastSignificantCommanderAction();
							float theTimeSinceLastSignificantAction = gpGlobals->time - theTimeOfLastSignificantCommanderAction;
						
							if((theTimeOfLastSignificantCommanderAction == -1) || (theTimeSinceLastSignificantAction > kShellShockedInterval))
							{
								theCommanderPlayer->PlayHUDSound(HUD_SOUND_MARINE_COMMANDERIDLE);
								thePlayedHint = true;
							}
						}
					}
				}
			
				// Update alien team hints
				if(this->mTeamType == AVH_CLASS_TYPE_ALIEN)
				{
					int theNumBuilders = 0;
					int theNumAliveTeamMembers = 0;
		
					// If we have no builders, tell the team
					FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
						if(theEntity->GetIsRelevant() && (theEntity->pev->team == this->mTeamNumber))
						{
							// If we have builders, or players that are evolving to builders...
							if(theEntity->GetUser3() == AVH_USER3_ALIEN_PLAYER2)
							{
								theNumBuilders++;
							}
							else if(theEntity->GetEvolution() == ALIEN_LIFEFORM_TWO)
							{
								theNumBuilders++;
							}
							else if((theEntity->GetUser3() == AVH_USER3_ALIEN_EMBRYO) && (theEntity->GetPreviousUser3() == AVH_USER3_ALIEN_PLAYER2))
							{
								AvHMessageID theEvolution = theEntity->GetEvolution();
								switch(theEvolution)
								{
								case ALIEN_EVOLUTION_ONE:
								case ALIEN_EVOLUTION_TWO:
								case ALIEN_EVOLUTION_THREE:
								case ALIEN_EVOLUTION_SEVEN:
								case ALIEN_EVOLUTION_EIGHT:
								case ALIEN_EVOLUTION_NINE:
								case ALIEN_EVOLUTION_TEN:
								case ALIEN_EVOLUTION_ELEVEN:
								case ALIEN_EVOLUTION_TWELVE:
									theNumBuilders++;
									break;
								}
							}
		
							if(theEntity->IsAlive())
							{
								theNumAliveTeamMembers++;
							}
						}
					END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
		
					// Only play if there is more then one person alive on team (player could be last one left or could be saving up)
					if((theNumBuilders == 0) && (theNumAliveTeamMembers > 1))
					{
						this->PlayHUDSoundForAlivePlayers(HUD_SOUND_ALIEN_NEED_BUILDERS);
						thePlayedHint = true;
					}
		
					// Now, fun easter eggs, but not in tourny mode
					if(!thePlayedHint && !GetGameRules()->GetIsTournamentMode() && this->mPlayFunSoundsThisGame)
					{
						// If a hive is under attack and the marines are way ahead, play "game over man"
		
						AvHHive* theRandomHive = AvHSUGetRandomActiveHive(this->mTeamNumber);
						if(theRandomHive)
						{
							const float kRadius = 600;
		
							int theClutterEntities = 0;
							int theDeadTeammates = 0;
							int theDeadPlayers = 0;
		
							CBaseEntity* theEntity = NULL;
							while((theEntity = UTIL_FindEntityInSphere(theEntity, theRandomHive->pev->origin, kRadius)) != NULL)
							{
								// If there are a ton of dead alien players, sometimes play "we need better players"
								if(theEntity->IsPlayer() && !theEntity->IsAlive())
								{
									if(theEntity->pev->team == this->mTeamNumber)
									{
										theDeadTeammates++;
									}
		
									theDeadPlayers++;
								}
								else
								{
									// If there are tons of aliens buildings nearby, play "place is a mess" sound for all aliens nearby
									AvHBaseBuildable* theBuildable = dynamic_cast<AvHBaseBuildable*>(theEntity);
									CBasePlayerItem* theItem = dynamic_cast<CBasePlayerItem*>(theEntity);
									if(theBuildable || theItem)
									{
										theClutterEntities++;
									}
								}
							}
		
							if((theDeadPlayers >= 14) && !this->mPlayedSeeDeadPeople)
							{
								EMIT_SOUND(theRandomHive->edict(), CHAN_AUTO, kAlienSeeDead, 1.0f, ATTN_NORM);
								this->mPlayedSeeDeadPeople;
								thePlayedHint = true;
							}
							else if(theDeadTeammates >= 8)
							{
								this->PlayFunHUDSoundOnce(HUD_SOUND_ALIEN_NEED_BETTER);
								thePlayedHint = true;
							}
							else if(theClutterEntities >= 25)
							{
								this->PlayFunHUDSoundOnce(HUD_SOUND_ALIEN_MESS);
								thePlayedHint = true;
							}
						}
		
						// If aliens have all three hives and all upgrades, play "donce"
						if(this->GetNumActiveHives() == kMaxHives)
						{
							if(this->GetAlienUpgrades().size() >= kMaxUpgradeCategories*kMaxUpgradeLevel)
							{
								this->PlayFunHUDSoundOnce(HUD_SOUND_ALIEN_NOW_DONCE);
								thePlayedHint = true;
							}
						}
		
						// If there are tons of bodies near a hive, play "I see dead people"
		
					}
				}
			
				if(thePlayedHint)
				{
					this->mTimeOfLastTeamHint = gpGlobals->time;
				}
			}
		}
	}
}

void AvHTeam::UpdateCommanderScore()
{
	// The commander score is the average of his team score
	const float kUpdateScoreTime = 2.0f;
	float theCurrentTime = gpGlobals->time;

	int theCommander = this->GetCommander();
	if(theCommander > 0)
	{
		if((this->mLastCommandScoreUpdateTime == -1) || (theCurrentTime > this->mLastCommandScoreUpdateTime + kUpdateScoreTime))
		{
			int theScore = 0;
			int theNumPlayers = 0;
		
			// Run through players on team and average score
			FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
				// For each player, update alien inventory with this # of hives
				if(theEntity->GetTeam() == this->GetTeamNumber())
				{
					if(theEntity->entindex() != theCommander)
					{
						theScore += theEntity->GetScore();
						theNumPlayers++;
					}
				}
			END_FOR_ALL_ENTITIES(kAvHPlayerClassName);

			int theCommanderScore = (int)((theScore/(float)theNumPlayers) + .5f);

			AvHPlayer* theCommanderPlayer = this->GetCommanderPlayer();
			if(theCommanderPlayer)
			{
				theCommanderPlayer->SetScore(theCommanderScore);
			}
					
			this->mLastCommandScoreUpdateTime = theCurrentTime;
		}
	}
}

void AvHTeam::UpdateInventoryEnabledState()
{
	// Count # of hives
	int theNumHives = this->GetNumActiveHives();

	// Loop through players on team
	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		// For each player, update alien inventory with this # of hives
		if(theEntity->GetTeam() == this->GetTeamNumber())
		{
			if(GetGameRules()->GetIsCombatMode())
			{
				theNumHives = 1;
				
				MessageIDListType& thePurchasedCombatUpgrades = theEntity->GetPurchasedCombatUpgrades();
				MessageIDListType::iterator theIter = std::find(thePurchasedCombatUpgrades.begin(), thePurchasedCombatUpgrades.end(), ALIEN_HIVE_TWO_UNLOCK);

				//AvHTechNode theTechNode;
				//if(theEntity->GetCombatNodes().GetTechNode(ALIEN_HIVE_TWO_UNLOCK, theTechNode) && theTechNode.GetIsResearched())
				if(theIter != thePurchasedCombatUpgrades.end())
				{
					theNumHives++;

					//if(theEntity->GetCombatNodes().GetTechNode(ALIEN_HIVE_THREE_UNLOCK, theTechNode) && theTechNode.GetIsResearched())
					theIter = std::find(thePurchasedCombatUpgrades.begin(), thePurchasedCombatUpgrades.end(), ALIEN_HIVE_THREE_UNLOCK);
					if(theIter != thePurchasedCombatUpgrades.end())
					{
						theNumHives++;
					}
				}
			}

			theEntity->UpdateInventoryEnabledState(theNumHives);
		}
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
}

void AvHTeam::UpdateReinforcementWave()
{
    int theNumPlayersOnTeam = this->GetPlayerCount();
	int theNumDeadPlayers = this->GetPlayerCount(true);
	int theWaveSize = AvHSUCalcCombatSpawnWaveSize(theNumPlayersOnTeam, theNumDeadPlayers);

	// If we're in combat mode
	if(GetGameRules()->GetIsCombatMode() && (GetGameRules()->GetVictoryTeam() == TEAM_IND) && !GetGameRules()->GetIsIronMan())
	{
		// If reinforcement wave hasn't started
		if(this->mTimeReinforcementWaveComplete == -1)
		{
			// If team has at least one dead player, start wave
			for(EntityListType::const_iterator theIter = this->mPlayerList.begin(); theIter != this->mPlayerList.end(); theIter++)
			{
				const AvHPlayer* thePlayer = this->GetPlayerFromIndex(*theIter);
				ASSERT(thePlayer);
                float theWaveTime = AvHSUCalcCombatSpawnTime(this->mTeamType, theNumPlayersOnTeam, theNumDeadPlayers, 0);

                switch(thePlayer->GetPlayMode())
                {
                case PLAYMODE_AWAITINGREINFORCEMENT:
                case PLAYMODE_REINFORCING:
					// Set time reinforcement is complete
					this->mTimeReinforcementWaveComplete = gpGlobals->time + theWaveTime;
					break;
                }
			}
		}
		else
		{
            // Spawn back in a max of X players per wave (1/4 rounded up)
            int theNumRespawning = 0;

            EntityListType::const_iterator theIter;

            // Count number of people currently respawning
            for(theIter = this->mPlayerList.begin(); theIter != this->mPlayerList.end(); theIter++)
            {
                AvHPlayer* thePlayer = this->GetPlayerFromIndex(*theIter);
                ASSERT(thePlayer);
                
                if(thePlayer->GetPlayMode() == PLAYMODE_REINFORCING)
                {
                    theNumRespawning++;
                }
            }

            // Find the player that's been waiting the longest
            if(theNumRespawning < theWaveSize)
            {
                EntityListType::const_iterator theLongestWaitingPlayer = this->mPlayerList.end();
                float theLongestWaitingTime = -1;

                // Loop through players
                for(theIter = this->mPlayerList.begin(); theIter != this->mPlayerList.end(); theIter++)
                {
                    // While we don't allow any more to repsawn or we hit end of list 
                    AvHPlayer* thePlayer = this->GetPlayerFromIndex(*theIter);
                    ASSERT(thePlayer);
                    
                    if(thePlayer->GetPlayMode() == PLAYMODE_AWAITINGREINFORCEMENT)
                    {
                        float thePlayerWaitTime = (gpGlobals->time - thePlayer->GetTimeLastPlaying());
                        if((theLongestWaitingTime == -1) || (thePlayerWaitTime > theLongestWaitingTime))
                        {
                            theLongestWaitingPlayer = theIter;
                            theLongestWaitingTime = thePlayerWaitTime;
                        }
                    }
                }

                if(theLongestWaitingPlayer != this->mPlayerList.end())
                {
                    AvHPlayer* thePlayer = this->GetPlayerFromIndex(*theLongestWaitingPlayer);
                    ASSERT(thePlayer);
                    thePlayer->SetPlayMode(PLAYMODE_REINFORCING);
                }
            }

            // Respawn players when wave is complete
            if(gpGlobals->time > this->mTimeReinforcementWaveComplete)
            {
                for(theIter = this->mPlayerList.begin(); theIter != this->mPlayerList.end(); theIter++)
                {
                    // While we don't allow any more to repsawn or we hit end of list 
                    AvHPlayer* thePlayer = this->GetPlayerFromIndex(*theIter);
                    ASSERT(thePlayer);

                    if(thePlayer->GetPlayMode() == PLAYMODE_REINFORCING)
                    {
                        thePlayer->SetPlayMode(PLAYMODE_PLAYING);
                    }
                }

                // Reset wave
                this->mTimeReinforcementWaveComplete = -1;
            }
		}
	}
}

void AvHTeam::UpdateOrders()
{
	// For each order in our list
	for(OrderListType::iterator theOrderIter = this->mOrderList.begin(); theOrderIter != this->mOrderList.end(); /* no increment*/)
	{
		ASSERT(theOrderIter->GetReceiver() != -1 );
		if(theOrderIter->Update())
		{
			bool theOrderCancelled = theOrderIter->GetOrderCancelled();
			if(!theOrderCancelled)
			{
				EntityInfo theReceiver = theOrderIter->GetReceiver();
				if(theReceiver != -1 )
				{
					GetGameRules()->TriggerAlert(this->mTeamNumber, ALERT_ORDER_COMPLETE, theReceiver);
				}
			}
		}
		
		// Have any orders been completed for a bit?
		const float kExpireTime = 1.0f;
		if(!theOrderIter->GetOrderActive() && (theOrderIter->GetTimeOrderCompleted() != -1) && (gpGlobals->time > (theOrderIter->GetTimeOrderCompleted() + kExpireTime)))
		{
			this->mOrderList.erase(theOrderIter);
		}
		else
		{
			theOrderIter++;
		}

		// Is the order complete?
//	for(OrderListType::iterator theOrderIter = this->mOrderList.begin(); theOrderIter != this->mOrderList.end(); /* no increment */)
//	{
//		// Is the order complete?
//		bool theOrderCancelled = false;
//		if(theOrderIter->GetIsComplete(&theOrderCancelled))
//		{
//			if(!theOrderCancelled)
//			{
//				// For each receiver
//				EntityListType theReceivers = theOrderIter->GetReceivers();
//				for(EntityListType::iterator theReceiverIter = theReceivers.begin(); theReceiverIter != theReceivers.end(); theReceiverIter++)
//				{
//					// Get player.  This fails occasionally, due to selection issues (ie, a building or other non-player entity will be in here)
//					AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(*theReceiverIter)));
//					//ASSERT(thePlayer);
//					if(thePlayer)
//					{
//						// Set the order complete
//						thePlayer->PlayOrderComplete();
//					}
//				}
//				
//				// Set order complete for commander
//				int theCommanderIndex = this->GetCommander();
//				if(theCommanderIndex >= 0)
//				{
//					AvHPlayer* theCommander = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theCommanderIndex)));
//					if(theCommander)
//					{
//						theCommander->PlayOrderComplete();
//					}
//				}
//			}
//			
//			// Set the receivers to none because it's no longer relevant
//			AvHOrder theCompletedOrder(*theOrderIter);
//			theCompletedOrder.SetOrderCompleted();
//			
//			this->SetOrder(theCompletedOrder);
//		}
//		else
//		{
//			theOrderIter++;
//		}
	}
}

//void AvHTeam::UpdateReinforcements()
//{
//	// If this team is alien
//	if(this->mTeamType == AVH_CLASS_TYPE_ALIEN)
//	{
//		// For each hive on this team
//		FOR_ALL_ENTITIES(kesTeamHive, AvHHive*)
//			if(AvHTeamNumber(theEntity->pev->team) == this->mTeamNumber)
//			{
//				// Update reinforcements
//				theEntity->UpdateReinforcements();
//			}
//		END_FOR_ALL_ENTITIES(kesTeamHive);
//	}
//	// else if it's marine
//	else if(this->mTeamType == AVH_CLASS_TYPE_MARINE)
//	{
//		// For each infantry portal on this team
//		FOR_ALL_ENTITIES(kwsInfantryPortal, AvHInfantryPortal*)
//			if(AvHTeamNumber(theEntity->pev->team) == this->mTeamNumber)
//			{
//				// Update reinforcements
//				theEntity->UpdateReinforcements();
//			}
//		END_FOR_ALL_ENTITIES(kwsInfantryPortal);
//	}
//	else
//	{
//		ASSERT(false);
//	}
//}

void AvHTeam::UpdateResearch()
{
	this->mResearchManager.UpdateResearch();
	this->UpdateMenuTechSlots();
}

void AvHTeam::UpdateResources()
{
	const AvHGameplay& theGameplay = GetGameRules()->GetGameplay();
	float kResourceUpdateInterval = theGameplay.GetTowerInjectionTime();
 
    if(GetGameRules()->GetIsHamboneMode())
    {
        kResourceUpdateInterval /= 2.0f;    
    }

    // Change here if teams should always get at least one resource
    float theResourcesGathered = 0.0f;

	float theCurrentTime = gpGlobals->time;
	if((this->mLastResourceUpdateTime == -1) || (theCurrentTime > (this->mLastResourceUpdateTime + kResourceUpdateInterval)))
	{
		// Add resources for each built-on resource on the team
		for(EntityListType::iterator theResourceTowerIter = this->mResourceTowerList.begin(); theResourceTowerIter != this->mResourceTowerList.end(); /* nothing */)
		{
			CBaseEntity* theResourceTowerEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(*theResourceTowerIter));
			AvHResourceTower* theResourceTower = dynamic_cast<AvHResourceTower*>(theResourceTowerEntity);
			if(theResourceTower && (theResourceTower->pev->team == this->mTeamNumber))
			{
				if(theResourceTower->GetIsActive())
				{
					// Has enough time elapsed?
					float theLastTimeResourcesContributed = theResourceTower->GetTimeLastContributed();
					float theCurrentTime = gpGlobals->time;
					
					if(theCurrentTime > (theLastTimeResourcesContributed + kResourceUpdateInterval))
					{
						// Take into account upgrade level stored in tower
						float theTowerContribution = theGameplay.GetTowerInjectionAmount();
						theResourcesGathered += theTowerContribution;
					
						// Decrement the amount of points the resources has
						theResourceTower->SetTimeLastContributed(theCurrentTime);
					
                        AvHSUPlayNumericEventAboveStructure(theTowerContribution, theResourceTower);
					}
				}

				theResourceTowerIter++;
			}
			else
			{
				// Remove it from team also
				theResourceTowerIter = this->mResourceTowerList.erase(theResourceTowerIter);
			}
		}
        
	    // If we're marines, add it to the team total
	    if(this->mTeamType == AVH_CLASS_TYPE_MARINE)
	    {
		    this->SetTeamResources(this->GetTeamResources() + theResourcesGathered);
	    }
	    else if(this->mTeamType == AVH_CLASS_TYPE_ALIEN)
	    {
		    // Else dump it into the pool, and split the whole pool up among all players on our team
            float theTeamResources = this->GetTeamResources() + theResourcesGathered;

		    int theNumPlayers = this->GetPlayerCount();
		    float theAmountForPlayer = theTeamResources/(float)theNumPlayers;

            // Clear resources, assuming all res will be given out.  Anything over will gob ack into team pool
            this->SetTeamResources(0.0f);
		    
		    for(EntityListType::iterator thePlayerIter = this->mPlayerList.begin(); thePlayerIter != this->mPlayerList.end(); thePlayerIter++)
		    {
			    AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(*thePlayerIter)));
			    ASSERT(thePlayer);

                // If the amount goes over the player's max, it goes back into team resources
                float theResourcesBefore = thePlayer->GetResources();
			    thePlayer->SetResources(thePlayer->GetResources() + theAmountForPlayer);
		    }
	    }
	    
	    // Increment total resources gathered
	    this->AddResourcesGathered(theResourcesGathered);

        this->mLastResourceUpdateTime = theCurrentTime;
    }
}
// puzl: 1041
// o Added back in steamid based authids 
#ifndef USE_OLDAUTH
#ifdef AVH_PLAYTEST_BUILD
// Function that is backwards-compatible with WON ids 
string AvHSUGetPlayerAuthIDString(edict_t* inPlayer)
{
	const char* kSteamIDInvalidID = "-1";
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
#endif
AvHServerPlayerData* AvHTeam::GetServerPlayerData(edict_t* inEdict)
{
#ifdef AVH_PLAYTEST_BUILD
	string theNetworkID = AvHSUGetPlayerAuthIDString(inEdict);
#else
	string theNetworkID = AvHNexus::getNetworkID(inEdict);
#endif
	return &this->mServerPlayerData[theNetworkID];
}
// :puzl
void AvHTeam::UpdateServerPlayerData()
{
	const float kServerPlayerDataUpdateInterval = 1.0f;
	float theCurrentTime = gpGlobals->time;
	if((this->mLastServerPlayerDataUpdateTime == -1) || (theCurrentTime > (this->mLastServerPlayerDataUpdateTime + kServerPlayerDataUpdateInterval)))
	{
		// Update all settings to remember keyed to their WONid
		FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		AvHServerPlayerData* theServerPlayerData = this->GetServerPlayerData(theEntity->edict());
		if(theServerPlayerData)
		{
			// Expire any commander bans
			float theCommanderVoteDownTime = avh_votedowntime.value;
			float theTimeVotedDown = theServerPlayerData->GetTimeVotedDown()*60;
			if(theTimeVotedDown > 0)
			{
				if(gpGlobals->time > (theTimeVotedDown + theCommanderVoteDownTime))
				{
					theServerPlayerData->SetTimeVotedDown(-1);
				}
			}
		}
		END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
	}
}

void AvHTeam::UpdateMenuTechSlots()
{
	if(this->mTeamType == AVH_CLASS_TYPE_MARINE)
	{
		// Update commander menus.  Get tech slots for each menu and add them all in one 32 long list
		this->mMenuTechSlots = 0;
		
		AvHGamerules* theGameRules = GetGameRules();
		const AvHTeam* theTeam = theGameRules->GetTeam(this->mTeamNumber);
		if(theTeam)
		{
			for(int j = 0; j < 4; j++)
			{
				AvHUser3 theUser3 = AVH_USER3_NONE;
				int theBaseIndex = 0;
				
				switch(j)
				{
				case 0:
					theUser3 = AVH_USER3_MENU_BUILD;
					theBaseIndex = 0;
					break;
				case 1:
					theUser3 = AVH_USER3_MENU_BUILD_ADVANCED;
					theBaseIndex = 8;
					break;
				case 2:
					theUser3 = AVH_USER3_MENU_ASSIST;
					theBaseIndex = 16;
					break;
				case 3:
					theUser3 = AVH_USER3_MENU_EQUIP;
					theBaseIndex = 24;
					break;
				}
				
				// For each slot, see if tech is available
				AvHTechSlots theTechSlots;
				if(theTeam->GetTechSlotManager().GetTechSlotList(theUser3, theTechSlots))
				{
					// Store results in full 32-bits of iuser1
					for(int i = 0; i < kNumTechSlots; i++)
					{
						int theCurrentMask = 1 << (theBaseIndex + i);
						
						AvHMessageID theMessage = theTechSlots.mTechSlots[i];
						if(theMessage != MESSAGE_NULL)
						{
							if(this->GetIsTechnologyAvailable(theMessage))
							{
								this->mMenuTechSlots |= theCurrentMask;
							}
						}
					}
				}
			}
		}
	}
}

bool AvHTeam::GetIsTechnologyAvailable(AvHMessageID inMessageID) const
{
	bool theTechnologyAvailable = this->GetResearchManager().GetIsMessageAvailable(inMessageID);
	return theTechnologyAvailable;
}




bool AvHTeam::GetLastAlert(AvHAlert& outAlert, bool inClearAlert, bool inAnyMessage, AvHMessageID* ioMessageID)
{
	float theLastAlertTime = -1;
	bool theSuccess = false;
	
	// Look for last alert
	AlertListType::iterator theLastAlertIter;
	AvHMessageID theOutputMessageID = MESSAGE_NULL;

	for(MessageAlertListType::iterator theIter = this->mAlertList.begin(); theIter != this->mAlertList.end(); theIter++)
	{
		if(inAnyMessage || (ioMessageID && (theIter->first == *ioMessageID)))
		{
			for(AlertListType::iterator theAlertIter = theIter->second.begin(); theAlertIter != theIter->second.end(); theAlertIter++)
			{
				float theCurrentTime = theAlertIter->GetTime();
				if(theCurrentTime > theLastAlertTime)
				{
					theLastAlertIter = theAlertIter;
					theLastAlertTime = theCurrentTime;
					theOutputMessageID = theIter->first;
					theSuccess = true;
				}
			}
		}
	}

	// If we found it, set the alert
	if(theSuccess)
	{
		outAlert = *theLastAlertIter;
		if(ioMessageID)
		{
			*ioMessageID = theOutputMessageID;
		}
	}

	// Clear the alert if specified
	if(inClearAlert)
	{
		for(MessageAlertListType::iterator theIter = this->mAlertList.begin(); theIter != this->mAlertList.end(); theIter++)
		{
			for(AlertListType::iterator theAlertIter = theIter->second.begin(); theAlertIter != theIter->second.end(); theAlertIter++)
			{
				if(theAlertIter == theLastAlertIter)
				{
					theIter->second.erase(theLastAlertIter);
					break;
				}
			}
		}
	}
	
	return theSuccess;
}

float AvHTeam::GetAudioAlertInterval() const
{
	float theAudioAlertInterval = BALANCE_VAR(kAudioAlertInterval);
	return theAudioAlertInterval;
}

void AvHTeam::AddAlert(const AvHAlert& inAlert, AvHMessageID inMessageID)
{
	this->mAlertList[inMessageID].push_back(inAlert);
}

const AvHAlert* AvHTeam::GetLastAudioAlert() const
{
	// Look for last audio alert
	const AvHAlert* theLastAudioAlert = NULL;
	float theLastTime = -1;

	for(MessageAlertListType::const_iterator theIter = this->mAlertList.begin(); theIter != this->mAlertList.end(); theIter++)
	{
		for(AlertListType::const_iterator theAlertIter = theIter->second.begin(); theAlertIter != theIter->second.end(); theAlertIter++)
		{
			if(theAlertIter->GetPlayedAudio())
			{
				float theCurrentTime = theAlertIter->GetTime();
				if(theCurrentTime > theLastTime)
				{
                    theLastAudioAlert = &(*theAlertIter);
					theLastTime = theCurrentTime;
				}
			}
		}
	}
	
	return theLastAudioAlert;
}

AlertListType AvHTeam::GetAlerts(AvHMessageID inMessageID) 
{
	return this->mAlertList[inMessageID];
}

void AvHTeam::UpdateAlerts()
{
	const float theAlertDuration = BALANCE_VAR(kAlertExpireTime);

	#ifdef AVH_PLAYTEST_BUILD
	// Run through our team, and alert everyone if there is a player that's no longer playing or on the server
	for(EntityListType::const_iterator thePlayerIter = this->mPlayerList.begin(); thePlayerIter != this->mPlayerList.end(); thePlayerIter++)
	{
		int thePlayerIndex = *thePlayerIter;
		bool theSendAlert = false;
		char theErrorMessage[512];

		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(*thePlayerIter)));
		if(!thePlayer)
		{
			theSendAlert = true;
			strcpy(theErrorMessage, "NULL player");
		}
		else
		{
			AvHPlayMode thePlayMode = thePlayer->GetPlayMode();
			if((thePlayMode != PLAYMODE_PLAYING) && (thePlayMode != PLAYMODE_AWAITINGREINFORCEMENT) && (thePlayMode != PLAYMODE_REINFORCING))
			{
				theSendAlert = true;
				sprintf(theErrorMessage, "Invalid play mode: %d\n", thePlayMode);
			}
			else if(AvHTeamNumber(thePlayer->pev->team) != this->mTeamNumber)
			{
				theSendAlert = true;
				sprintf(theErrorMessage, "on wrong team (%d instead of %d)", thePlayer->pev->team, this->mTeamNumber);
			}
		}

		if(theSendAlert)
		{
			const char* thePlayerName = "<null>";
			thePlayerName = STRING(thePlayer->pev->netname);
			
			char theFullErrorMesssage[512];
			sprintf(theFullErrorMesssage, "Invalid player bug detected <\"%s\">: %d", thePlayerName, theErrorMessage);
			UTIL_SayTextAll(theFullErrorMesssage, thePlayer);
		}
	}
	#endif
	
	// Run through alerts, and expire any past a certain age
	for(MessageAlertListType::iterator theIter = this->mAlertList.begin(); theIter != this->mAlertList.end(); ++theIter)
	{
		AlertListType& theAlertList = theIter->second;
		for(AlertListType::iterator theAlertIter = theAlertList.begin(); theAlertIter != theAlertList.end(); /* no inc */)
		{
			float theAlertTime = theAlertIter->GetTime();
			if(gpGlobals->time > (theAlertTime + theAlertDuration))
			{
				theAlertIter = theAlertList.erase(theAlertIter);
			}
			else
			{
				theAlertIter++;
			}
		}
	}
}

EntityListType AvHTeam::GetGroup(int inGroupNumber)
{
	ASSERT(inGroupNumber >= 0);
	ASSERT(inGroupNumber < kNumHotkeyGroups);

	return this->mGroups[inGroupNumber];
}

void AvHTeam::SetGroup(int inGroupNumber, EntityListType& inEntityListType)
{
	// Set group
	ASSERT(inGroupNumber >= 0);
	ASSERT(inGroupNumber < kNumHotkeyGroups);
	
	this->mGroups[inGroupNumber] = inEntityListType;

	// Update group type
	AvHUser3 theUser3 = AvHSUGetGroupTypeFromSelection(this->mGroups[inGroupNumber]);
	this->mGroupTypes[inGroupNumber] = theUser3;
}

AvHUser3 AvHTeam::GetGroupType(int inGroupNumber)
{
	ASSERT(inGroupNumber >= 0);
	ASSERT(inGroupNumber < kNumHotkeyGroups);

	return this->mGroupTypes[inGroupNumber];
}

EntityListType AvHTeam::GetSelectAllGroup()
{
	return this->mSelectAllGroup;
}

void AvHTeam::SetSelectAllGroup(EntityListType& inGroup)
{
	this->mSelectAllGroup = inGroup;
}
