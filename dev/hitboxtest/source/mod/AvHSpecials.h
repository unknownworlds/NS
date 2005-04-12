//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Used to describe NS entity state on the client, server and in shared-code
//
// $Workfile: AvHSpecials.h $
// $Date: 2002/11/15 04:41:53 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSpecials.h,v $
// Revision 1.33  2002/11/15 04:41:53  Flayra
// - Added user3 to cull spawns
//
// Revision 1.32  2002/11/03 04:52:42  Flayra
// - Added new user3s, to track entities for AddToFullPack (probably not needed)
//
// Revision 1.31  2002/09/23 22:32:51  Flayra
// - Removed power armor
// - Added heavy armor and jetpacks
//
// Revision 1.30  2002/08/16 02:48:33  Flayra
// - Removed overwatch, added ensnared flag
//
// Revision 1.29  2002/08/02 21:47:00  Flayra
// - Added max iuser3
//
// Revision 1.28  2002/07/23 17:28:39  Flayra
// - New constants for phase gates and alien buildings, new marine upgrades
//
// Revision 1.27  2002/07/08 17:18:29  Flayra
// - Mark spawn points (can't remember why this is needed), updated comments, removed offensive upgrade code
//
// Revision 1.26  2002/07/01 21:24:56  Flayra
// - Regular update
//
// Revision 1.25  2002/06/25 18:18:56  Flayra
// - Removed offensive upgrades, added charging, new alien upgrade system
//
// Revision 1.24  2002/06/03 17:00:48  Flayra
// - Renamed weapons factory and armory
//
// Revision 1.23  2002/05/28 18:08:25  Flayra
// - Added recycling and persistent masks
//
// Revision 1.22  2002/05/23 02:32:57  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_SPECIALS_H
#define AVH_SPECIALS_H

#include "types.h"
#include "mod/AvHMessage.h"
#include "mod/AvHConstants.h"

// Only one of these allowed per entity, stored in pev->iuser3.
typedef enum
{
	AVH_USER3_NONE = 0,
	AVH_USER3_MARINE_PLAYER,
	AVH_USER3_COMMANDER_PLAYER,
	AVH_USER3_ALIEN_PLAYER1,
	AVH_USER3_ALIEN_PLAYER2,
	AVH_USER3_ALIEN_PLAYER3,
	AVH_USER3_ALIEN_PLAYER4,
	AVH_USER3_ALIEN_PLAYER5,
	AVH_USER3_ALIEN_EMBRYO,
	AVH_USER3_SPAWN_TEAMONE,
	AVH_USER3_SPAWN_TEAMTWO,
	AVH_USER3_PARTICLE_ON,				// only valid for AvHParticleEntity: entindex as int in fuser1, template index stored in fuser2
	AVH_USER3_PARTICLE_OFF,				// only valid for AvHParticleEntity: particle system handle in fuser1
	AVH_USER3_WELD,						// float progress (0 - 100) stored in fuser1
	AVH_USER3_ALPHA,					// fuser1 indicates how much alpha this entity toggles to in commander mode, fuser2 for players
	AVH_USER3_MARINEITEM,				// Something a friendly marine can pick up
	AVH_USER3_WAYPOINT,
	AVH_USER3_HIVE,
	AVH_USER3_NOBUILD,
	AVH_USER3_USEABLE,
	AVH_USER3_AUDIO_ON,
	AVH_USER3_AUDIO_OFF,
	AVH_USER3_FUNC_RESOURCE,
	AVH_USER3_COMMANDER_STATION,
	AVH_USER3_TURRET_FACTORY, 
	AVH_USER3_ARMORY, 
	AVH_USER3_ADVANCED_ARMORY,
	AVH_USER3_ARMSLAB,
	AVH_USER3_PROTOTYPE_LAB, 
	AVH_USER3_OBSERVATORY,
	AVH_USER3_CHEMLAB,
	AVH_USER3_MEDLAB,
	AVH_USER3_NUKEPLANT,
	AVH_USER3_TURRET,
	AVH_USER3_SIEGETURRET,
	AVH_USER3_RESTOWER,
	AVH_USER3_PLACEHOLDER,
	AVH_USER3_INFANTRYPORTAL,
	AVH_USER3_NUKE,
	AVH_USER3_BREAKABLE,
	AVH_USER3_UMBRA,
	AVH_USER3_PHASEGATE,
	AVH_USER3_DEFENSE_CHAMBER,
	AVH_USER3_MOVEMENT_CHAMBER,
	AVH_USER3_OFFENSE_CHAMBER,
	AVH_USER3_SENSORY_CHAMBER,
	AVH_USER3_ALIENRESTOWER,
	AVH_USER3_HEAVY,
	AVH_USER3_JETPACK,
	AVH_USER3_ADVANCED_TURRET_FACTORY,
	AVH_USER3_SPAWN_READYROOM,
	AVH_USER3_CLIENT_COMMAND,
	AVH_USER3_FUNC_ILLUSIONARY,
	AVH_USER3_MENU_BUILD,
	AVH_USER3_MENU_BUILD_ADVANCED,
	AVH_USER3_MENU_ASSIST,
	AVH_USER3_MENU_EQUIP,
	AVH_USER3_MINE,
	AVH_USER3_UNKNOWN,
	AVH_USER3_MAX
} AvHUser3;

//typedef enum
//{
//	AVH_USER4_PARTICLE_VISIBLE,			// only valid for AvHParticleEntity: particle system handle in fuser1
//	AVH_USER4_PARTICLE_NOTVISIBLE,		// only valid for AvHParticleEntity: particle system handle in fuser1
//}
//AvHParticleUser4;

//typedef enum
//{
//	AVH_USER4_NONE = 0,
//	AVH_USER4_COMMANDER_STATION = 1,
//	AVH_USER4_TURRET_FACTORY = 2, 
//	AVH_USER4_WEAPON_FACTORY = 3, 
//	AVH_USER4_ADVANCED_WEAPON_FACTORY = 4, 
//	AVH_USER4_ARMORY = 5, 
//	AVH_USER4_PROTOTYPE_LAB = 6, 
//	AVH_USER4_OBSERVATORY = 7,
//	AVH_USER4_CHEMLAB = 8,
//	AVH_USER4_MEDLAB = 9,
//	AVH_USER4_NUKEPLANT = 10,
//	AVH_USER4_TURRET = 11,
//	AVH_USER4_RESTOWER = 12,
//	AVH_USER4_CAMTOWER = 13,
//	AVH_USER4_INFANTRYPORTAL = 14
//}
//AvHSelectableUser4;

// AvHSpecials, only one per entity, stored in pev->iuser4.  

// Stored in iuser4.  Some entities don't use these values, but most do.  The ones that don't include:
// AVH_USER3_AUDIO_OFF
// AVH_USER3_AUDIO_ON
typedef enum
{
	MASK_NONE = 0,
	MASK_VIS_SIGHTED = 1,					// This means this is an entity that can be seen by at least one member of the opposing team.  Assumes commanders can never be seen.
	MASK_VIS_DETECTED = 2,					// This entity has been detected by the other team but isn't currently seen
	MASK_BUILDABLE = 4,						// This entity is buildable
	MASK_UPGRADE_1 = 8,						// Marine weapons 1, armor, marine basebuildable slot #0
	MASK_UPGRADE_2 = 16,					// Marine weapons 2, regen, marine basebuildable slot #1
	MASK_UPGRADE_3 = 32,					// Marine weapons 3, redemption, marine basebuildable slot #2
	MASK_UPGRADE_4 = 64,					// Marine armor 1, speed, marine basebuildable slot #3
	MASK_UPGRADE_5 = 128,					// Marine armor 2, adrenaline, marine basebuildable slot #4
	MASK_UPGRADE_6 = 256,					// Marine armor 3, silence, marine basebuildable slot #5
	MASK_UPGRADE_7 = 512,					// Marine jetpacks, Cloaking, marine basebuildable slot #6
	MASK_UPGRADE_8 = 1024,					// Pheromone, motion-tracking, marine basebuildable slot #7
	MASK_UPGRADE_9 = 2048,					// Scent of fear, exoskeleton
	MASK_UPGRADE_10 = 4096,					// Defensive level 2, power armor
	MASK_UPGRADE_11 = 8192,					// Defensive level 3, electrical defense
	MASK_UPGRADE_12 = 16384,				// Movement level 2, 
	MASK_UPGRADE_13 = 32768,				// Movement level 3, marine heavy armor
	MASK_UPGRADE_14 = 65536,				// Sensory level 2
	MASK_UPGRADE_15 = 131072,				// Sensory level 3
	MASK_ALIEN_MOVEMENT = 262144,			// Onos is charging
	MASK_WALLSTICKING = 524288,				// Flag for wall-sticking
	MASK_BUFFED = 1048576,                  // Alien is in range of active primal scream, or marine is under effects of catalyst
	MASK_UMBRA = 2097152,
	MASK_DIGESTING = 4194304,				// When set on a visible player, player is digesting.  When set on invisible player, player is being digested
	MASK_RECYCLING = 8388608,
	MASK_TOPDOWN = 16777216,
	MASK_PLAYER_STUNNED = 33554432,			// Player has been stunned by stomp
	MASK_ENSNARED = 67108864,
	MASK_ALIEN_EMBRYO = 268435456,
	MASK_SELECTABLE = 536870912,
	MASK_PARASITED = 1073741824,
	MASK_SENSORY_NEARBY = 2147483648
} AvHUpgradeMask;

// IMPORTANT: Keep this mask up to date as upgrades change and move around
const int kUpgradeBitMask = MASK_UPGRADE_1 | MASK_UPGRADE_2 | MASK_UPGRADE_3 | MASK_UPGRADE_4 | MASK_UPGRADE_5 | MASK_UPGRADE_6 /*| MASK_UPGRADE_7*/ | MASK_UPGRADE_8 | MASK_UPGRADE_9 | MASK_UPGRADE_10 | MASK_UPGRADE_11 | MASK_UPGRADE_12 /*| MASK_UPGRADE_13 */ | MASK_UPGRADE_14 | MASK_UPGRADE_15;

const int kMaxUpgradesTypes = 3;

typedef enum
{
	ALIEN_UPGRADE_CATEGORY_INVALID = 0, 
	ALIEN_UPGRADE_CATEGORY_DEFENSE,
	ALIEN_UPGRADE_CATEGORY_OFFENSE,
	ALIEN_UPGRADE_CATEGORY_MOVEMENT,
	ALIEN_UPGRADE_CATEGORY_SENSORY,
	ALIEN_UPGRADE_CATEGORY_MAX_PLUS_ONE,
} AvHAlienUpgradeCategory;

typedef vector<AvHAlienUpgradeCategory>		AvHAlienUpgradeListType;

//// Server ways to get/set upgrade masks
//#ifdef AVH_SERVER
//class AvHPlayer;
//bool GetHasUpgrade(AvHPlayer* inPlayer, AvHUpgradeMask inUpgrade);
//void SetUpgradeMask(AvHPlayer* inPlayer, AvHUpgradeMask inUpgrade);
//#endif
//
//// Client ways to get/set upgrade masks
//#ifdef AVH_CLIENT
//#include <assert.h>
//#include "mathlib.h"
//#include "const.h"
//#include "usercmd.h"
//#include "pm_defs.h"
//#include "pm_shared.h"
//#include "pm_movevars.h"
//#include "pm_debug.h"
//bool GetHasUpgrade(playermove_t* inPlayerMove, AvHUpgradeMask inUpgrade);
//void SetUpgradeMask(playermove_t* inPlayerMove, AvHUpgradeMask inUpgrade);
//#endif

void InitializeBuildable(int& inUser3, int& inUser4, float& inFuser1, int inUser3ID);
bool GetHasUpgrade(int inUpgrade, AvHUpgradeMask inUpgradeMask);
void SetUpgradeMask(int* inPointer, AvHUpgradeMask inUpgradeMask, bool inSet = true);
AvHUpgradeMask ProcessGenericUpgrade(int& inUpgradeVariable, AvHMessageID inUpgrade, bool inGive = true);

//int AvHGetUser3Base(int inUser3);
//
//int AvHGetUser4Base(int inUser4);
//int AvHGetUser4Extra(int inUser4);
//void AvHSetUser4(int& inUser4, int inBase, int inExtra);

bool AvHGetAlienUpgradeCategory(AvHMessageID inUpgrade, AvHAlienUpgradeCategory& outCategory);
bool AvHGetAlienUpgradeCategoryFromMask(AvHUpgradeMask inUpgradeMask, AvHAlienUpgradeCategory& outCategory);

bool AvHGetAlienUpgradeMask(AvHMessageID inUpgrade, AvHUpgradeMask& outUpgradeMask);
int AvHGetAlienUpgradeLevel(int inUser4, AvHUpgradeMask inMask);

bool AvHGetHasFreeUpgradeCategory(AvHAlienUpgradeCategory inUpgradeCategory, const AvHAlienUpgradeListType& inList, int inUser4, int* outNumFree = NULL);
bool AvHGetHasUpgradeChoiceInCategory(AvHAlienUpgradeCategory inUpgradeCategory, const AvHAlienUpgradeListType& inList, int inUser4);
bool AvHGetIsAlien(int inUser3);
int AvHGetNumUpgradesInCategory(AvHAlienUpgradeCategory inUpgradeCategory, int inUser4);
int AvHGetNumUpgradesInCategoryInList(const AvHAlienUpgradeListType& inList, AvHAlienUpgradeCategory inUpgradeCategory);
void AvHAddHigherLevelUpgrades(const AvHAlienUpgradeListType& inList, int& inUser4);
void AvHRemoveIrrelevantUpgrades(AvHAlienUpgradeListType& inList);
int AvHRemoveExcessUpgrades(const AvHAlienUpgradeListType& inList, int& inUser4);
void AvHAddUpgradeInCategory(AvHAlienUpgradeCategory inUpgradeCategory, int& inUser4);
void AvHRemoveUpgradeInCategory(AvHAlienUpgradeCategory inUpgradeCategory, int& inUser4);
bool AvHRemoveUpgradeCategory(AvHAlienUpgradeCategory inUpgradeCategory, AvHAlienUpgradeListType& inList);

//bool AvHGetTechSlotList(const string& inClassName, TechSlotList& outTechSlotList);
bool AvHGetTechSlotEnabled(int inSlot, int inUser4);
void AvHSetTechSlotEnabledState(int inSlot, int* inUser4, bool inEnabled);
#endif