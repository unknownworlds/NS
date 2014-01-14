//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHConstants.h$
// $Date: 2002/11/12 02:23:01 $
//
//-------------------------------------------------------------------------------
// $Log: AvHConstants.h,v $
// Revision 1.61  2002/11/12 02:23:01  Flayra
// - Renamed avhplayer for 3rd party compatibility (adminmod, etc.)
//
// Revision 1.60  2002/11/06 01:39:48  Flayra
// - Regeneration now heals a percentage
//
// Revision 1.59  2002/11/05 06:17:25  Flayra
// - Balance changes
//
// Revision 1.58  2002/11/03 04:50:19  Flayra
// - Hard-coded gameplay constants instead of putting in skill.cfg
//
// Revision 1.57  2002/10/24 21:23:01  Flayra
// - Reworked jetpacks
// - Added alien easter eggs
//
// Revision 1.56  2002/10/18 22:18:44  Flayra
// - Added alien easter egg sayings
// - Added sensory chamber sayings
// - Limit number of buildings in radius
// - Fixed motd.txt length crash
//
// Revision 1.55  2002/10/16 20:51:44  Flayra
// - Hive health while gestating
//
// Revision 1.54  2002/10/16 00:52:45  Flayra
// - Added "need order" alert
// - Added authentication mask
// - Updated alien building sounds
// - Removed a couple unneeded sounds
//
// Revision 1.53  2002/10/03 18:41:58  Flayra
// - Added alien HUD sounds
// - Added more order sounds
//
// Revision 1.52  2002/09/25 20:43:40  Flayra
// - Removed use order, sound update
//
// Revision 1.51  2002/09/23 22:12:07  Flayra
// - New CC sounds
// - Regular update
//
// Revision 1.50  2002/09/09 19:49:25  Flayra
// - Added deathmessage cheat
//
// Revision 1.49  2002/08/31 18:01:01  Flayra
// - Work at VALVe
//
// Revision 1.48  2002/08/16 02:33:57  Flayra
// - Regular update
//
// Revision 1.47  2002/08/09 00:56:18  Flayra
// - Added "adjustscore" cheat for testing new scoreboard
//
// Revision 1.46  2002/08/02 22:01:04  Flayra
// - Regular update
//
// Revision 1.45  2002/07/28 19:38:59  Flayra
// - Linux path fixes
//
// Revision 1.44  2002/07/26 23:04:08  Flayra
// - Generate numerical feedback for damage events
//
// Revision 1.43  2002/07/23 17:01:02  Flayra
// - Regular update
//
// Revision 1.42  2002/07/08 16:50:20  Flayra
// - Reworked team colors, #define a few more sounds, weapon spread
//
// Revision 1.41  2002/07/01 22:41:40  Flayra
// - Removed outdated overwatch target and tension events
//
// Revision 1.40  2002/07/01 21:25:26  Flayra
// - Added new alien weapons, added build ranges
//
// Revision 1.39  2002/06/25 17:53:19  Flayra
// - Regular update (cleanup, new entities, new classnames)
//
// Revision 1.38  2002/06/10 19:51:29  Flayra
// - Regular update
//
// Revision 1.37  2002/06/03 16:41:50  Flayra
// - Removed duplicate hive class name, added more player class types for scoreboard info
//
// Revision 1.36  2002/05/28 17:37:48  Flayra
// - Reinforcment refactoring, renamed role sounds to be less confusing
//
// Revision 1.35  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVHCONSTANTS_H
#define AVHCONSTANTS_H

#include "types.h"
#include "game_shared/teamconst.h"

// Basic constants
const   int								kMaxPlayers = 32;
const	int								kMaxEntities = 1500;
const	int								kSelectionStartRange = 50;
const	int								kSelectionEndRange = 8012;
const	int								kSelectionExtreme = 8012;
const	float							kSelectionNetworkConstant = 1000.0f;
const	float							kBuildInvalidRangeMultiplier = 2.0f;
const	float							kWorldPosNetworkConstant = 6.0f;
const	float							kNumericNetworkConstant = 100.0f;
const	int								kWorldMinimumZ = -4096;
const	int								kMaxGroundPlayerSpeed = 1200;
const	int								kNumHotkeyGroups = 5;
const	int								kSelectAllHotGroup = kNumHotkeyGroups + 1;

const	int								kNumRequestTypes = 3;
const	int								kPendingIdleSoldiers = (kNumHotkeyGroups + 1);
const	int								kPendingAmmoRequests = (kNumHotkeyGroups + 2);
const	int								kPendingHealthRequests = (kNumHotkeyGroups + 3);

const	int								kMaxHives = 3;
const	int								kMaxUpgradeCategories = 3;
const	int								kMaxUpgradeLevel = 3;

const	int								kFuncResourceMaxResources = 300;
//const	float							kPlayerResourceScalar = 2.0f; // 1.5 might be better
const	int								kVictoryIntermission = 12;
const	int								kResetPlayersPerSecond = 5;
const	float							kBuildingUseWarmupTime = 2.0f;
const   float                           kRedeemInvulnerableTime = 1.0f;

const	float							kMaxBuildingDropSlope = 1.0f;
const	float							kMaxEquipmentDropSlope = 1.0f;

// Used to be cl_forwardspeed, cl_backspeed, cl_sidespeed.
const   float                           kForwardSpeed = 1000.0f;
const   float                           kBackSpeed = 1000.0f;
const   float                           kSideSpeed = 1000.0f;

// Name this guy for custom propagation, maybe
typedef int									EntityInfo;
typedef vector<EntityInfo>					EntityListType;
typedef pair<EntityInfo, float>				EntityFloatPairType;
typedef vector<EntityFloatPairType>			EntityFloatPairListType;
typedef map<int, EntityInfo>				EntityMap;
typedef vector<int>							IntList;

#define kAvHGameName					((const char*)("Natural Selection"))
#define kAvHGameAcronymn				((const char*)("NS"))
#define kInvalidTeamName				"Invalid team name"
#define kAvHPlayerClassName				"player"
#define	kwsWeldableClassName			"avhweldable"
#define kwsMapInfoClassName				"avhmapinfo"
#define kwsGameplayClassName			"avhgameplay"
#define kwsGammaClassName				"avhgamma"

#define	kNumAlienLifeforms				5
#define	kNumAlienUpgrades				12
#define	kNumAlienUpgradeCategories		3
#define kNumUpgradesInCategory			3
#define kNumUpgradeLevelsInCategory		3

#define kMaxAlienHives					3
#define kMaxAlienEnergy					100
#define kMaxAlienResources				100

typedef enum
{
	AVH_CLASS_TYPE_UNDEFINED = 0,
	AVH_CLASS_TYPE_MARINE = 1,
	AVH_CLASS_TYPE_ALIEN = 2,
	AVH_CLASS_TYPE_AUTOASSIGN = 3
} AvHClassType;

typedef enum
{
	MAP_MODE_UNDEFINED = 0,
	MAP_MODE_NS = 1,
	MAP_MODE_CS = 2,
	MAP_MODE_DM = 3,
	MAP_MODE_CO = 4
}
AvHMapMode;

#define kUndefinedTeam					"undefinedteam"
#define kMarine1Team					"marine1team"
#define kAlien1Team						"alien1team"
#define kMarine2Team					"marine2team"
#define kAlien2Team						"alien2team"
#define kSpectatorTeam					"spectatorteam"
#define kTeamString						"undefinedteam;marine1team;alien1team;marine2team;alien2team;spectatorteam"

#define kMarinePrettyName				"Frontiersmen"
#define kAlienPrettyName				"Kharaa"
#define kUndefPrettyName				"Undefined"

typedef enum
{
	AUTH_ACTION_JOIN_TEAM = 0,
	AUTH_ACTION_ADJUST_BALANCE = 1
} AvHAuthAction;

typedef enum
{
	PLAYMODE_UNDEFINED = 0,
	PLAYMODE_READYROOM = 1,
	PLAYMODE_PLAYING = 2,
	PLAYMODE_AWAITINGREINFORCEMENT = 3,	// Player is dead and waiting in line to get back in
	PLAYMODE_REINFORCING = 4,			// Player is in the process of coming back into the game
	PLAYMODE_OBSERVER = 5,
	PLAYMODE_REINFORCINGCOMPLETE = 6	// Combat only: 'press fire to respawn'
} AvHPlayMode;

typedef enum
{
	TEAM_IND = 0,
	TEAM_ONE = 1,
	TEAM_TWO = 2,
	TEAM_THREE = 3,
	TEAM_FOUR = 4,
	TEAM_SPECT = 5,
	TEAM_ACTIVE_BEGIN = 1,
	TEAM_ACTIVE_END = 5
} AvHTeamNumber;

typedef enum
{
	ALERT_NONE = 0,
	ALERT_PLAYER_ENGAGE = 1,
	ALERT_UNDER_ATTACK = 2,
	ALERT_RESEARCH_COMPLETE = 3,
	ALERT_UPGRADE_COMPLETE = 4,
	ALERT_LOW_RESOURCES = 5,
	ALERT_SOLDIER_NEEDS_AMMO = 6,
	ALERT_SOLDIER_NEEDS_HEALTH = 7,
	ALERT_PLAYER_DIED = 8,
	ALERT_SENTRY_FIRING = 9,
	ALERT_SENTRY_DAMAGED = 10,
	ALERT_HIVE_COMPLETE = 11,
	ALERT_HIVE_DYING = 12,
	ALERT_NEW_TRAIT = 13,
	ALERT_ORDER_NEEDED = 14,
	ALERT_ORDER_COMPLETE = 15,
	ALERT_HIVE_DEFEND = 16,
	ALERT_MAX_ALERTS = 17
} AvHAlertType;

typedef enum
{
	PLAYERCLASS_NONE = 0,
	PLAYERCLASS_ALIVE_MARINE,
	PLAYERCLASS_ALIVE_JETPACK_MARINE,
	PLAYERCLASS_ALIVE_HEAVY_MARINE,
	PLAYERCLASS_ALIVE_LEVEL1,
	PLAYERCLASS_ALIVE_LEVEL2,
	PLAYERCLASS_ALIVE_LEVEL3,
	PLAYERCLASS_ALIVE_LEVEL4,
	PLAYERCLASS_ALIVE_LEVEL5,
	PLAYERCLASS_ALIVE_DIGESTING,
	PLAYERCLASS_ALIVE_GESTATING,
	PLAYERCLASS_DEAD_MARINE,
	PLAYERCLASS_DEAD_ALIEN,
	PLAYERCLASS_COMMANDER,
	PLAYERCLASS_REINFORCING,
	PLAYERCLASS_SPECTATOR,
	PLAYERCLASS_REINFORCINGCOMPLETE
} AvHPlayerClass;

// : 0001073
#ifdef USE_OLDAUTH
// This is a mask because players can have more then one of these
typedef enum
{
	PLAYERAUTH_NONE = 0,
	PLAYERAUTH_DEVELOPER = 1,
	PLAYERAUTH_GUIDE = 2,
	PLAYERAUTH_SERVEROP = 4,
	PLAYERAUTH_PLAYTESTER = 8,
	PLAYERAUTH_CONTRIBUTOR = 16,
	PLAYERAUTH_CHEATINGDEATH = 32,
	PLAYERAUTH_VETERAN = 64,
	PLAYERAUTH_BETASERVEROP = 128,
	PLAYERAUTH_PENDING = 256,
	PLAYERAUTH_CUSTOM = 512,
	PLAYERAUTH_UPP_MODE = 16384
} AvHPlayerAuthentication;
#else
// Mask replaced with explicit string set for GetIsMember function
const static string PLAYERAUTH_DEVELOPER("dev");
const static string PLAYERAUTH_PLAYTESTER("pt");
const static string PLAYERAUTH_GUIDE("guide");
const static string PLAYERAUTH_CONTRIBUTOR("const");
const static string PLAYERAUTH_SERVEROP("op");
const static string PLAYERAUTH_VETERAN("vet");
const static string PLAYERAUTH_BETASERVEROP("betaop");
#endif

typedef enum 
{
	HUD_SOUND_INVALID = 0,
	HUD_SOUND_POINTS_SPENT,
	HUD_SOUND_COUNTDOWN,
	HUD_SOUND_SELECT,
	HUD_SOUND_SQUAD1,
	HUD_SOUND_SQUAD2,
	HUD_SOUND_SQUAD3,
	HUD_SOUND_SQUAD4,
	HUD_SOUND_SQUAD5,
	HUD_SOUND_PLACE_BUILDING,
	HUD_SOUND_MARINE_RESEARCHCOMPLETE,
	HUD_SOUND_MARINE_SOLDIER_UNDER_ATTACK,
	HUD_SOUND_MARINE_BASE_UNDER_ATTACK,
	HUD_SOUND_MARINE_UPGRADE_COMPLETE,
	HUD_SOUND_MARINE_MORE,
	HUD_SOUND_MARINE_SOLDIERLOST,
	HUD_SOUND_MARINE_CCONLINE,
	HUD_SOUND_MARINE_CCUNDERATTACK,
	HUD_SOUND_MARINE_COMMANDER_EJECTED,
	HUD_SOUND_MARINE_RESOURCES_LOW,
	HUD_SOUND_MARINE_NEEDS_AMMO,
	HUD_SOUND_MARINE_NEEDS_HEALTH,
	HUD_SOUND_MARINE_NEEDS_ORDER,
	HUD_SOUND_MARINE_POINTS_RECEIVED,
	HUD_SOUND_MARINE_SOLDIER_LOST,
	HUD_SOUND_MARINE_SENTRYFIRING,
	HUD_SOUND_MARINE_SENTRYDAMAGED,
	HUD_SOUND_MARINE_GIVEORDERS,
	HUD_SOUND_MARINE_NEEDPORTAL,
	HUD_SOUND_MARINE_GOTOALERT,
	HUD_SOUND_MARINE_COMMANDERIDLE,
	HUD_SOUND_MARINE_ARMORYUPGRADING,

	HUD_SOUND_ALIEN_ENEMY_APPROACHES,
	HUD_SOUND_ALIEN_GAMEOVERMAN,
	HUD_SOUND_ALIEN_HIVE_ATTACK,
	HUD_SOUND_ALIEN_HIVE_COMPLETE,
	HUD_SOUND_ALIEN_HIVE_DYING,
	HUD_SOUND_ALIEN_LIFEFORM_ATTACK,
	HUD_SOUND_ALIEN_RESOURCES_LOW,
	HUD_SOUND_ALIEN_MESS,
	HUD_SOUND_ALIEN_MORE,
	HUD_SOUND_ALIEN_NEED_BETTER,
	HUD_SOUND_ALIEN_NEED_BUILDERS,
	HUD_SOUND_ALIEN_NEW_TRAIT,
	HUD_SOUND_ALIEN_NOW_DONCE,
	HUD_SOUND_ALIEN_POINTS_RECEIVED,
	HUD_SOUND_ALIEN_RESOURCES_ATTACK,
	HUD_SOUND_ALIEN_STRUCTURE_ATTACK,
	HUD_SOUND_ALIEN_UPGRADELOST,
	
	HUD_SOUND_ORDER_MOVE,
	HUD_SOUND_ORDER_ATTACK,
	HUD_SOUND_ORDER_BUILD,
	HUD_SOUND_ORDER_WELD,
	HUD_SOUND_ORDER_GUARD,
	HUD_SOUND_ORDER_GET,
	HUD_SOUND_ORDER_COMPLETE,
	
	HUD_SOUND_GAMESTART,
	HUD_SOUND_YOU_WIN,
	HUD_SOUND_YOU_LOSE,
	HUD_SOUND_TOOLTIP,
	// : bug 0000767
	HUD_SOUND_PLAYERJOIN,
	// :
	HUD_SOUND_MAX
} AvHHUDSound;

typedef vector<AvHHUDSound>		HUDSoundListType;

//typedef enum
//{
//	ARMOR_BASE = 1,
//	ARMOR_HEAVY = 2,
//	ARMOR_LIFESUPPORT = 4,
//	ARMOR_JETPACK = 8,
//	ARMOR_MOTIONTRACK = 16
//} AvHMarineArmor;

// Location orders, global orders
typedef enum
{
	ORDERTYPE_UNDEFINED = 0,
	ORDERTYPEL_DEFAULT,
	ORDERTYPEL_MOVE,

	ORDERTYPET_ATTACK,
	ORDERTYPET_BUILD,
	ORDERTYPET_GUARD,
	ORDERTYPET_WELD,
	ORDERTYPET_GET,

	ORDERTYPEG_HOLD_POSITION,
	ORDERTYPEG_CODE_DEPLOY_MINES,
	ORDERTYPEG_CODE_GREEN,
	ORDERTYPEG_CODE_YELLOW,
	ORDERTYPEG_CODE_RED,

	ORDERTYPE_MAX
}
AvHOrderType;

typedef enum
{
	ORDERTARGETTYPE_UNDEFINED = 0,
	ORDERTARGETTYPE_GLOBAL = 1,
	ORDERTARGETTYPE_LOCATION = 2,
	ORDERTARGETTYPE_TARGET = 3
}
AvHOrderTargetType;

extern const char* getModDirectory(void);
extern const char* getModName(void);

#define kSpriteDirectory			"sprites"
#define kMiniMapSpritesDirectory	"sprites/minimaps"
#define	kTechTreeSpriteDirectory	"sprites/techtree"
#define kTechTreeSpritePrefix		"tech"
#define kScriptsDirectory			"scripts"
#define kSoundDirectory				"sound"
#define kMapDirectory				"maps"
#define kMusicDirectory				"music"
#define kBasePSName					"ns.ps"

// Entities
#define kesDeathMatchStart			"info_player_deathmatch"
#define kesTerroristStart			"info_player_deathmatch"
#define kesCounterTerroristStart	"info_player_start"

#define kesFuncDoor					"func_door"
#define kesFuncWall					"func_wall"
#define kesEnvSprite				"env_sprite"
#define kesFuncIllusionary			"func_illusionary"

#define kesReadyRoomStart			"info_player_start"
#define keTeamStart					info_team_start
#define kesTeamStart				"info_team_start"
#define keLeaveGame					info_leave_game
#define keJoinTeam					info_join_team
#define keSpectate					info_spectate
#define keAutoAssign				info_join_autoassign
#define keMapInfo					info_mapinfo
#define kesMapInfo					"info_mapinfo"
#define keGameplay					info_gameplay
#define kesGameplay					"info_gameplay"

//#define keGlow						glow
#define keGamma						env_gamma
#define kesGamma					"env_gamma"
#define keParticles					env_particles
#define kesParticles				"env_particles"
#define keParticlesCustom			env_particles_custom
#define kesParticlesCustom			"env_particles_custom"

//#define keResource					resource
#define	keWeldable					func_weldable
#define	kesWeldable					"func_weldable"

#define	keSeethrough				func_seethrough
#define	kesSeethrough				"func_seethrough"

#define	keSeethroughDoor			func_seethroughdoor
#define	kesSeethroughDoor			"func_seethroughdoor"

//#define keWaypoint					func_waypoint
//#define kesWaypoint					"func_waypoint"

#define keNoBuild					func_nobuild
#define kesNoBuild					"func_nobuild"

#define keFuncResource				func_resource
#define kesFuncResource				"func_resource"

#define keMP3Audio					target_mp3audio
#define kesMP3Audio					"target_mp3audio"

#define keFog						env_fog
#define kesFog						"env_fog"

#define keInfoLocation				info_location
#define kesInfoLocation				"info_location"

#define keTeamHive					team_hive
#define kesTeamHive					"team_hive"

#define keTeamEgg					team_egg
#define kesTeamEgg					"team_egg"

#define keTriggerRandom				trigger_random
#define kesTriggerRandom			"trigger_random"

#define keTriggerPresence			trigger_presence
#define kesTriggerPresence			"trigger_presence"

#define keTriggerScript				trigger_script
#define kesTriggerScript			"trigger_script"

#define keTeamWebStrand				team_webstrand
#define kesTeamWebStrand			"team_webstrand"

// Targets fired by game
#define ktGameStartedStatus			"gamestartedstatus"
#define ktGameReset					"gamereset"
									
// Weapons/Equipment				
#define kwsKnife					"weapon_knife"
#define kwKnife						weapon_knife
#define kwsGrenade					"weapon_grenade"
#define kwGrenade					weapon_grenade
#define kwsMachineGun				"weapon_machinegun"
#define kwMachineGun				weapon_machinegun
#define kwsPistol					"weapon_pistol"
#define kwPistol					weapon_pistol
#define kwsShotGun					"weapon_shotgun"
#define kwShotGun					weapon_shotgun
#define kwsHeavyMachineGun			"weapon_heavymachinegun"
#define kwHeavyMachineGun			weapon_heavymachinegun
#define kwsGrenadeGun				"weapon_grenadegun"
#define kwGrenadeGun				weapon_grenadegun
#define kwsNukeGun					"weapon_nukegun"
#define kwNukeGun					weapon_nukegun
#define kwsFlameGun					"weapon_flamegun"
#define kwFlameGun					weapon_flamegun

// Weapon that dispenses mines
#define kwsMine						"weapon_mine"
#define kwMine						weapon_mine

// Deployed mines
#define	kwDeployedMine				item_mine
#define	kwsDeployedMine				"item_mine"

#define kwsDeployedTurret			"turret"
#define kwDeployedTurret			turret

#define kwsHealth					"item_health"
#define kwHealth					item_health

#define kwsCatalyst                 "item_catalyst"
#define kwCatalyst                 item_catalyst

#define kwsGenericAmmo				"item_genericammo"
#define kwGenericAmmo				item_genericammo

#define	kwsHeavyArmor				"item_heavyarmor"
#define	kwHeavyArmor				item_heavyarmor

#define	kwsJetpack					"item_jetpack"
#define	kwJetpack					item_jetpack

#define	kwsAmmoPack					"item_ammopack"
#define	kwAmmoPack					item_ammopack

// Alien weapons
#define kwsParalysisGun				"weapon_paralysis"
#define kwParalysisGun				weapon_paralysis

#define kwsSpitGun					"weapon_spit"
#define kwSpitGun					weapon_spit

#define kwClaws						weapon_claws
#define kwsClaws					"weapon_claws"

#define kwSwipe						weapon_swipe
#define kwsSwipe					"weapon_swipe"

#define kwSporeGun					weapon_spore
#define kwsSporeGun					"weapon_spore"

// Alien projectiles
#define kwsSpitProjectile			"spitgunprojectile"
#define kwSpitProjectile			spitgunprojectile

#define kwSporeProjectile			sporegunprojectile
#define kwsSporeProjectile			"sporegunprojectile"

#define kwSpikeGun					weapon_spikegun
#define kwsSpikeGun					"weapon_spikegun"

#define kwBiteGun					weapon_bitegun
#define kwsBiteGun					"weapon_bitegun"

#define kwBite2Gun					weapon_bite2gun
#define kwsBite2Gun					"weapon_bite2gun"

#define kwResourceTowerGun			weapon_resourcetowergun
#define kwsResourceTowerGun			"weapon_resourcetowergun"

#define kwOffenseChamberGun			weapon_offensechambergun
#define kwsOffenseChamberGun		"weapon_offensechambergun"

#define kwDefenseChamberGun			weapon_defensechambergun
#define kwsDefenseChamberGun		"weapon_defensechambergun"

#define kwSensoryChamberGun			weapon_sensorychambergun
#define kwsSensoryChamberGun		"weapon_sensorychambergun"

#define kwMovementChamberGun		weapon_movementchambergun
#define kwsMovementChamberGun		"weapon_movementchambergun"

#define kwHiveGun					weapon_hivegun
#define kwsHiveGun					"weapon_hivegun"

#define kwHealingSpray				weapon_healingspray
#define kwsHealingSpray				"weapon_healingspray"

#define kwMetabolize				weapon_metabolize
#define kwsMetabolize				"weapon_metabolize"

#define kwWebSpinner				weapon_webspinner
#define kwsWebSpinner				"weapon_webspinner"

#define kwsWebProjectile			"webgunprojectile"
#define kwWebProjectile				webgunprojectile

#define kwBabblerGun				weapon_babblergun
#define kwsBabblerGun				"weapon_babblergun"

#define kwBabblerProjectile			weapon_babblerprojectile
#define kwsBabblerProjectile		"weapon_babblerprojectile"

#define kwPrimalScream				weapon_primalscream
#define kwsPrimalScream				"weapon_primalscream"

#define kwLeap						weapon_leap
#define kwsLeap						"weapon_leap"

#define kwAmplify					weapon_amplify
#define kwsAmplify					"weapon_amplify"

#define kwStomp						weapon_stomp
#define kwsStomp					"weapon_stomp"

#define kwStompProjectile			stompprojectile
#define kwsStompProjectile			"stompprojectile"
		  
#define kwDevour					weapon_devour
#define kwsDevour					"weapon_devour"

#define kwCharge					weapon_charge
#define kwsCharge					"weapon_charge"

#define kwsParasiteGun				"weapon_parasite"
#define kwParasiteGun				weapon_parasite

#define kwsUmbraCloud				"umbracloud"
#define kwUmbraCloud				umbracloud
#define kwsUmbraProjectile			"umbraprojectile"
#define kwUmbraProjectile			umbraprojectile

#define kwsUmbraGun					"weapon_umbra"
#define kwUmbraGun					weapon_umbra

#define kwsBlinkGun					"weapon_blink"
#define kwBlinkGun					weapon_blink

#define kwsDivineWind				"weapon_divinewind"
#define kwDivineWind				weapon_divinewind

#define kwBileBomb					weapon_bilebomb
#define kwsBileBomb					"weapon_bilebomb"

#define kwBileBombGun				weapon_bilebombgun
#define kwsBileBombGun				"weapon_bilebombgun"

#define kwAcidRocket				weapon_acidrocket
#define kwsAcidRocket				"weapon_acidrocket"

#define kwAcidRocketGun				weapon_acidrocketgun
#define kwsAcidRocketGun			"weapon_acidrocketgun"

// Debug item
#define kwsDebugEntity				"item_genericammo"

// Filenames
#define kMOTDName					"motd.txt"

// Tech node prefix (skill.cfg)
#define kTechCostPrefix				"avh_techcost_"
#define kTechHealthPrefix			"avh_techhealth_"
#define kTechTimePrefix				"avh_techtime_"

// Player models
#define	kNullModel					"models/null.mdl"
#define kReadyRoomModel				"models/player.mdl"
#define kMarineSoldierModel			"models/player/soldier/soldier.mdl"
#define kHeavySoldierModel			"models/player/heavy/heavy.mdl"
#define kMarineCommanderModel		"models/player/commander/commander.mdl"
#define kAlienLevelOneModel			"models/player/alien1/alien1.mdl"
#define kAlienLevelTwoModel			"models/player/alien2/alien2.mdl"
#define kAlienLevelThreeModel		"models/player/alien3/alien3.mdl"
#define kAlienLevelFourModel		"models/player/alien4/alien4.mdl"
#define kAlienLevelFiveModel		"models/player/alien5/alien5.mdl"
#define kAlienGestateModel		    "models/player/gestate/gestate.mdl"

//#define kAlienAbilitiesGrantedSound	"misc/a-abilities_granted.wav"
//#define kAlienAbilitiesLostSound	"misc/a-abilities_lost.wav"
#define	kAlienBuildingSound1		"misc/a-build1.wav"
#define	kAlienBuildingSound2		"misc/a-build2.wav"
#define kDistressBeaconSound		"misc/distressbeacon.wav"

#define kBuildableRecycleSound		"misc/b_recycle.wav"
#define kBuildableHurt1Sound		"misc/b_hurt1.wav"
#define kBuildableHurt2Sound		"misc/b_hurt2.wav"
#define kElectricalSprite			"sprites/lgtning.spr"

// Model names for key
#define kSoldierName				"soldier"
#define kHeavyName					"heavy"
#define kCommanderName				"commander"
#define kAlien1Name					"alien1"
#define kAlien2Name					"alien2"
#define kAlien3Name					"alien3"
#define kAlien4Name					"alien4"
#define kAlien5Name					"alien5"
#define kAlienGestationName			"gestate"

// Sound lists
#define kPlayerLevelAttackSoundList	"player/role%d_attack"
#define kPlayerLevelDieSoundList	"player/role%d_die"
#define kPlayerLevelIdleSoundList	"player/role%d_idle"
#define kPlayerLevelMoveSoundList	"player/role%d_move"
#define kPlayerLevelPainSoundList	"player/role%d_pain"
#define kPlayerLevelSpawnSoundList	"player/role%d_spawn"
#define kPlayerLevelWoundSoundList	"player/role%d_wound"
#define kHiveWoundSoundList			"misc/hive_wound"

#define kFallPainSoundFormat	"player/pl_fallpain3-%d.wav"


#define	kDigestingSound				"player/digesting.wav"

// Not quite a sound list
//#define kHiveWoundSoundPrefix		"misc/hive_wound"

#define kPieSelectForwardSound		"hud/select_node_forward.wav"
#define kPieSelectBackwardSound		"hud/select_node_backward.wav"
#define kPieSelectForwardAlienSound		"hud/select_node_forward-a.wav"
#define kPieSelectBackwardAlienSound	"hud/select_node_backward-a.wav"
#define kSelectSound				"hud/select.wav"
#define kSelectAlienSound			"hud/select-a.wav"
#define kSquad1Sound				"hud/squad1.wav"
#define kSquad2Sound				"hud/squad2.wav"
#define kSquad3Sound				"hud/squad3.wav"
#define kSquad4Sound				"hud/squad4.wav"
#define kSquad5Sound				"hud/squad5.wav"

#define kMarineSquad1Sound			"hud/m-squad1.wav"
#define kMarineSquad2Sound			"hud/m-squad2.wav"
#define kMarineSquad3Sound			"hud/m-squad3.wav"
#define kMarineSquad4Sound			"hud/m-squad4.wav"
#define kMarineSquad5Sound			"hud/m-squad5.wav"

#define kPlaceBuildingSound			"hud/place_building.wav"
#define kCountdownSound				"hud/countdown.wav"
#define kPointsSpentSound			"hud/points_spent.wav"
#define kAlienPointsReceivedSound	"hud/alien_points_received.wav"
#define kMarinePointsReceivedSound	"hud/marine_points_received.wav"

#define kMarineResearchComplete		"hud/marine_research_complete.wav"
#define kMarineSoldierUnderAttack	"hud/marine_soldierunderattack.wav"
#define kMarineCCOnline1			"hud/marine_cconline1.wav"
#define kMarineCCOnline2			"hud/marine_cconline2.wav"
#define kMarineCCUnderAttack1		"hud/marine_ccunderattack1.wav"
#define kMarineCCUnderAttack2		"hud/marine_ccunderattack2.wav"
#define kMarineCommanderEjected		"hud/marine_commander_ejected.wav"
#define kMarineBaseUnderAttack1		"hud/marine_baseattack1.wav"
#define kMarineBaseUnderAttack2		"hud/marine_baseattack2.wav"
#define kMarineMoreResources		"hud/marine_more.wav"
#define kMarineLowResources			"hud/marine_lowresources.wav"
#define kMarineNeedsAmmo1			"hud/marine_needsammo1.wav"
#define kMarineNeedsAmmo2			"hud/marine_needsammo2.wav"
#define kMarineNeedsHealth1			"hud/marine_needshealth1.wav"
#define kMarineNeedsHealth2			"hud/marine_needshealth2.wav"
#define kMarineNeedsOrder1			"hud/marine_needsorder1.wav"
#define kMarineNeedsOrder2			"hud/marine_needsorder2.wav"
#define kMarineSoldierLost1			"hud/marine_soldierlost1.wav"
#define kMarineSoldierLost2			"hud/marine_soldierlost2.wav"
#define kMarineSentryFiring1		"hud/marine_sentryfiring1.wav"
#define kMarineSentryFiring2		"hud/marine_sentryfiring2.wav"
#define kMarineSentryTakingDamage1	"hud/marine_sentrytakingdamage1.wav"
#define kMarineSentryTakingDamage2	"hud/marine_sentrytakingdamage2.wav"
#define kMarineUpgradeComplete		"hud/marine_upgradecomplete.wav"
#define kMarineGiveOrders			"hud/marine_giveorders.wav"
#define kMarineNeedPortal1			"hud/marine_needportal1.wav"
#define kMarineNeedPortal2			"hud/marine_needportal2.wav"
#define kMarineGotoAlert			"hud/marine_gotoalert.wav"
#define kMarineCommanderIdle1		"hud/marine_commanderidle1.wav"
#define kMarineCommanderIdle2		"hud/marine_commanderidle2.wav"
#define kMarineArmoryUpgrading		"hud/marine_armoryupgrading.wav"

#define kAlienEnemyApproaches1		"hud/alien_enemyapproaches1.wav"
#define kAlienEnemyApproaches2		"hud/alien_enemyapproaches2.wav"
#define kAlienGameOverMan			"hud/alien_gameoverman.wav"
#define kAlienHiveAttack			"hud/alien_hiveattack.wav"
#define kAlienHiveComplete1			"hud/alien_hivecomplete1.wav"
#define kAlienHiveComplete2			"hud/alien_hivecomplete2.wav"
#define kAlienHiveDying1			"hud/alien_hivedying1.wav"
#define kAlienHiveDying2			"hud/alien_hivedying2.wav"
#define kAlienLifeformAttack1		"hud/alien_lifeformattack1.wav"
#define kAlienLifeformAttack2		"hud/alien_lifeformattack2.wav"
#define kAlienLowResources			"hud/alien_lowresources.wav"
#define kAlienMess					"hud/alien_mess.wav"
#define kAlienMoreResources1		"hud/alien_more1.wav"
#define kAlienMoreResources2		"hud/alien_more2.wav"
#define kAlienNeedBetter			"hud/alien_needbetter.wav"
#define kAlienNeedBuilders1			"hud/alien_needbuilders1.wav"
#define kAlienNeedBuilders2			"hud/alien_needbuilders2.wav"
#define kAlienNewTrait1				"hud/alien_newtrait1.wav"
#define kAlienNewTrait2				"hud/alien_newtrait2.wav"
#define kAlienNowDonce				"hud/alien_now.wav"
#define kAlienResourceAttack1		"hud/alien_resourceattack1.wav"
#define kAlienResourceAttack2		"hud/alien_resourceattack2.wav"
#define kAlienSeeDead				"hud/alien_seedead.wav"
#define kAlienStructureAttack1		"hud/alien_structureattack1.wav"
#define kAlienStructureAttack2		"hud/alien_structureattack2.wav"
#define kAlienUpgradeLost			"hud/alien_upgrade_lost.wav"

#define kSoundOrderMove1			"hud/marine_order_move1.wav"
#define kSoundOrderMove2			"hud/marine_order_move2.wav"
#define kSoundOrderMove3			"hud/marine_order_move3.wav"
#define kSoundOrderMove4			"hud/marine_order_move4.wav"
#define kSoundOrderAttack			"hud/marine_order_attack.wav"
#define kSoundOrderBuild			"hud/marine_order_build.wav"
#define kSoundOrderWeld				"hud/marine_order_weld.wav"
#define kSoundOrderGuard			"hud/marine_order_guard.wav"
#define	kSoundOrderGet				"hud/marine_order_get.wav"
#define	kSoundOrderComplete1		"hud/marine_order_complete1.wav"
#define	kSoundOrderComplete2		"hud/marine_order_complete2.wav"
#define	kSoundOrderComplete3		"hud/marine_order_complete3.wav"
#define	kSoundOrderComplete4		"hud/marine_order_complete4.wav"
#define	kSoundOrderComplete5		"hud/marine_order_complete5.wav"
#define	kSoundOrderComplete6		"hud/marine_order_complete6.wav"

#define kAlienGameStart1			"hud/alien_gamestart1.wav"
#define kAlienGameStart2			"hud/alien_gamestart2.wav"
#define kMarineGameStart1			"hud/marine_gamestart1.wav"
#define kMarineGameStart2			"hud/marine_gamestart2.wav"
#define kYouWinSound				"hud/you_win.wav"
#define kYouLoseSound				"hud/you_lose.wav"
#define kTooltipSound				"hud/tooltip.wav"
#define	kMyHiveEasterEgg			"hud/alien_myhive.wav"

// Tech category names				
#define kWeaponTechCategory			"WeaponCategory"
#define kArmorTechCategory			"ArmorCategory"
#define kBuildTechCategory			"BuildCategory"
#define kRadioTechCategory			"RadioCategory"

// Tech button components are the category plus this suffix
//#define kMessageButtonsSuffix		"Buttons"
#define kTechNodeLabelPrefix		"TechNodeLabel_"
#define kTechNodeHelpPrefix			"TechNodeHelp_"
#define kPrerequisitePrefix			"Prerequisite"
#define kUser3Name					"User3Name_"
#define kUser3Description			"User3Desc_"
#define kUser3FriendlyDescription	"User3FriendlyDesc_"
#define kUser3CommanderDescription	"User3CommanderDesc_"
#define kBlipStatusPrefix			"BlipStatus_"
#define kMessageButtonCost			"Cost"
#define kNotFullyBuilt				"NotFullyBuilt"
#define kPointsSuffix				"Points"

const float kSquadHierarchyScaleFactor = .0001f;
const float kCommanderHierarchyScaleFactor = .0002f;

// Sayings
#define kSoldierSayingList			"vox/ssay%d"
#define kCommanderSayingList		"vox/csay%d"
#define kAlienSayingList			"vox/asay%d"

#define kSoldierOrderRequestList	"vox/sreq"
#define kSoldierOrderAckList		"vox/sack"

// Other sounds
#define kJetpackSound				"misc/jetpack.wav"
//#define kAdrenalineSound			"misc/adren.wav"
#define kGestationSound				"misc/gestate.wav"
#define kConnectSound				"misc/connect.wav"
//#define kDisconnectSound			"misc/disconnect.wav"
#define kEmptySound					"weapons/357_cock1.wav"
#define kInvalidSound				"misc/invalid.wav"
#define kLevelUpMarineSound			"misc/levelup.wav"
#define kLevelUpAlienSound			"misc/a-levelup.wav"
// : bug 0000767
#define kPlayerJoinedSound			"player/jointeam.wav"
// :

// Events
#define kJetpackEvent				"events/Jetpack.sc"
#define kStartOverwatchEvent		"events/StartOverwatch.sc"
#define kEndOverwatchEvent			"events/EndOverwatch.sc"

#define	kStopVoiceSoundEvent		"events/StopVoice.sc"

#define kRegenerationEvent			"events/Regeneration.sc"
#define kStartCloakEvent			"events/StartCloak.sc"
#define kEndCloakEvent				"events/EndCloak.sc"

#define kWallJumpEvent				"events/WallJump.sc"
#define kFlightEvent				"events/Flight.sc"
#define kTeleportEvent				"events/Teleport.sc"
#define kSiegeHitEvent				"events/SiegeHit.sc"
#define kSiegeViewHitEvent			"events/SiegeViewHit.sc"
#define kPhaseInEvent				"events/PhaseIn.sc"
#define kCommanderPointsAwardedEvent	"events/CommandPoints.sc"
#define kEmptySoundEvent			"events/Empty.sc"
#define kNumericalInfoEvent			"events/NumericalInfo.sc"
#define kAlienSightOnEvent			"events/AlienSightOn.sc"
#define kAlienSightOffEvent			"events/AlienSightOff.sc"
#define kInvalidActionEvent			"events/InvalidAction.sc"
#define kParticleEvent				"events/Particle.sc"
#define kDistressBeaconEvent		"events/DistressBeacon.sc"
#define kWeaponAnimationEvent		"events/WeaponAnimation.sc"
#define kLevelUpEvent				"events/LevelUp.sc"
#define	kAbilityEventName			"events/Ability.sc"

// Targets
#define	kTargetCommandStationUseTeamOne				"commandstationuse1"
#define	kTargetCommandStationUseTeamTwo				"commandstationuse2"

#define	kTargetCommandStationLogoutTeamOne			"commandstationlogout1"
#define	kTargetCommandStationLogoutTeamTwo			"commandstationlogout2"

#define	kTargetCommandStationDestroyedTeamOne		"commandstationdestroyed1"
#define	kTargetCommandStationDestroyedTeamTwo		"commandstationdestroyed2"

#define kReadyNotification			"ready"
#define kNotReadyNotification		"notready"

const int	kOverwatchBreakingVelocity					= 5;
const float kOverwatchAcquireTime						= 6.0f;
const float	kOverwatchLostTargetTime					= 4.0f;
const float	kOverwatchKeepFiringAfterMissingTargetTime	= 1.0f;
const float	kSpeakingTime								= 4.0f;
const float	kEnemySightedTime							= 2.0f;

const int	kDefaultViewHeight							= 0;
const float kDefaultMinMapExtent						= -4000;
const float kDefaultMaxMapExtent						= 4000;

// Energy constants for marine structures
const float kMarineStructureEnergyRate = .33f;
const float kMarineStructureMaxEnergy = 100;

const int	kShellRenderAmount							= 50;
const int	kInvulShellRenderAmount						= 15;

// This is one less then 4096 because we need top bit for sign.
// Only allow map dimensions of this size or lower
const float kMaxMapDimension							= 4095;

const float kDefaultMapGamma							= 1.0f;

const float kNormalizationNetworkFactor					= 1000.0f;
const float kHotKeyNetworkFactor						= 100.0f;

const int kDetectionDistance							= 500;
const float kMaxRelevantCullDistance					= 1024;

const int kHiveXYDistanceTolerance						= 400;
const float	kBaseHealthPercentage						= .5f;

const float kHUDSoundVolume								= .3f;

#define	kHotKeyPrefix									"hotkey"

// How many directions can bullets travel in within spread vector?
//const int kBulletSpreadGranularity						= 15;

const int		iNumberOfTeamColors = 6;
extern int		kTeamColors[iNumberOfTeamColors][3];
extern float	kFTeamColors[iNumberOfTeamColors][3];

// Random resource constants
const int		kNumSharesPerPlayer = 3;
const int		kTotalShares = MAX_PLAYERS_PER_TEAM*kNumSharesPerPlayer;

const int		kNumericalInfoResourcesEvent			= 0;
const int		kNumericalInfoHealthEvent				= 1;
const int		kNumericalInfoResourcesDonatedEvent		= 2;
const int		kNumericalInfoAmmoEvent					= 3;

const int		kGameStatusReset						= 0;
const int		kGameStatusResetNewMap					= 1;
const int		kGameStatusEnded						= 2;
const int		kGameStatusGameTime						= 3;
const int		kGameStatusUnspentLevels				= 4;

// Max message length is 192, take into account rest of message
const int		kMaxPlayerSendMessageLength				= 160;

const int		kTopDownYaw = 90;
const int		kTopDownPitch = 0;
const int		kTopDownRoll = -90;

// Extra fudge factor to make sure players don't get stuck when respawning
const int		kRespawnFudgeFactorHeight = 1;

#endif
