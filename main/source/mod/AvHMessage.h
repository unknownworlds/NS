//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Messages/actions (impulses) from players
//
// $Workfile: AvHMessage.h $
// $Date: 2002/09/25 20:48:56 $
//
//-------------------------------------------------------------------------------
// $Log: AvHMessage.h,v $
// Revision 1.30  2002/09/25 20:48:56  Flayra
// - Added 3 more sayings
//
// Revision 1.29  2002/09/23 22:23:14  Flayra
// - Added heavy armor and jetpacks
// - Removed power armor
// - Added siege upgrade
//
// Revision 1.28  2002/07/23 17:14:26  Flayra
// - Removed unused constants, new upgrades
//
// Revision 1.27  2002/07/08 17:11:42  Flayra
// - Added admin commands, explicity defined HL impulses, removed offensive upgrades
//
// Revision 1.26  2002/06/25 18:07:59  Flayra
// - Building rename, blinking change
//
// Revision 1.25  2002/06/10 19:59:25  Flayra
// - Update for new commander UI
//
// Revision 1.24  2002/06/03 16:52:18  Flayra
// - Renamed weapons factory and armory
//
// Revision 1.23  2002/05/28 17:54:19  Flayra
// - Added support for recycling
//
// Revision 1.22  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVHMESSAGE_H
#define AVHMESSAGE_H

typedef enum
{
    // Special or misc. actions
    MESSAGE_NULL = 0,

    // Use an item or ability (these are currently forced to be less than 10 because it's
    // used as an index into some weapons/equipment array <sigh>)
    WEAPON_NEXT = 1,
    WEAPON_RELOAD = 2,
    WEAPON_DROP = 3,

	// Admin commands
	ADMIN_READYROOM = 5,
	ADMIN_VOTEDOWNCOMMANDER = 6,

    // Talk to your teammates
    SAYING_1 = 7,
    SAYING_2 = 8,
    SAYING_3 = 9,
    SAYING_4 = 10,
    SAYING_5 = 11,
    SAYING_6 = 12,
    SAYING_7 = 13,
    SAYING_8 = 14,
    SAYING_9 = 15,

	// Chat
	COMM_CHAT_PUBLIC = 16,
	COMM_CHAT_TEAM = 17,
	COMM_CHAT_NEARBY = 18,

	// Research
	RESEARCH_ARMOR_ONE = 20,
	RESEARCH_ARMOR_TWO = 21,
	RESEARCH_ARMOR_THREE = 22,
	RESEARCH_WEAPONS_ONE = 23,
	RESEARCH_WEAPONS_TWO = 24,
	RESEARCH_WEAPONS_THREE = 25,
	TURRET_FACTORY_UPGRADE = 26,
	BUILD_CAT = 27,
	RESEARCH_JETPACKS = 28,
	RESEARCH_HEAVYARMOR = 29,
	RESEARCH_DISTRESSBEACON = 30,
	MESSAGE_CANCEL = 32,
	RESEARCH_MOTIONTRACK = 33,
	RESEARCH_PHASETECH = 34,
	RESOURCE_UPGRADE = 35,
	RESEARCH_ELECTRICAL = 36,
    RESEARCH_GRENADES = 37,
	
	// Buildings
	BUILD_HEAVY = 38,
	BUILD_JETPACK = 39,
	BUILD_INFANTRYPORTAL = 40,
	BUILD_RESOURCES = 41,
	BUILD_TURRET_FACTORY = 43,
	BUILD_ARMSLAB = 45,
	BUILD_PROTOTYPE_LAB = 46,
	RESEARCH_CATALYSTS = 47,
	BUILD_ARMORY = 48,
	ARMORY_UPGRADE = 49,
	BUILD_NUKE_PLANT = 50,
	BUILD_OBSERVATORY = 51,
	RESEARCH_HEALTH = 52,

	BUILD_SCAN = 53,
	BUILD_PHASEGATE = 55,
	BUILD_TURRET = 56,
	BUILD_SIEGE = 57,
	BUILD_COMMANDSTATION = 58,
	
	// Weapons and items
	BUILD_HEALTH = 59,
	BUILD_AMMO = 60,
	BUILD_MINES = 61,
	BUILD_WELDER = 62,
	BUILD_UNUSED = 63,
	BUILD_SHOTGUN = 64,
	BUILD_HMG = 65,
	BUILD_GRENADE_GUN = 66,
	BUILD_NUKE = 67,

	// Misc
	BUILD_RECYCLE = 69,

	GROUP_CREATE_1 = 70,
	GROUP_CREATE_2 = 71,
	GROUP_CREATE_3 = 72,
	GROUP_CREATE_4 = 73,
	GROUP_CREATE_5 = 74,

	GROUP_SELECT_1 = 75,
	GROUP_SELECT_2 = 76,
	GROUP_SELECT_3 = 77,
	GROUP_SELECT_4 = 78,
	GROUP_SELECT_5 = 79,
	
	// Orders
	ORDER_REQUEST = 80,
	ORDER_ACK = 81,

	// Commander mode 
	COMMANDER_MOUSECOORD = 82,
	COMMANDER_MOVETO = 83,
	COMMANDER_SCROLL = 84,
	COMMANDER_DEFAULTORDER = 104,
	COMMANDER_SELECTALL = 105,
	COMMANDER_REMOVESELECTION = 96,
	
	// Sub-menus
	MENU_BUILD = 85,
	MENU_BUILD_ADVANCED = 86,
	MENU_ASSIST = 87,
	MENU_EQUIP = 88,
	MENU_SOLDIER_FACE = 89,

	// These messages aren't sent as impulses, but are used internally in the shared code.  They can be removed if necessary with some work.
	ALIEN_BUILD_RESOURCES = 90,
	ALIEN_BUILD_OFFENSE_CHAMBER = 91,
	ALIEN_BUILD_DEFENSE_CHAMBER = 92,
	ALIEN_BUILD_SENSORY_CHAMBER = 93,
	ALIEN_BUILD_MOVEMENT_CHAMBER = 94,
	ALIEN_BUILD_HIVE = 95,

	IMPULSE_LOGO = 99,
	IMPULSE_FLASHLIGHT = 100,
	IMPULSE_SPRAYPAINT = 201,
	IMPULSE_DEMORECORD = 204,

	// Alien menu items (there are assumptions that these are contiguous)
	ALIEN_EVOLUTION_ONE = 101,		// Carapace
	ALIEN_EVOLUTION_TWO = 102,		// Regeneration
	ALIEN_EVOLUTION_THREE = 103,	// Redemption

	ALIEN_EVOLUTION_SEVEN = 107,	// Celerity
	ALIEN_EVOLUTION_EIGHT = 108,	// Adrenaline
	ALIEN_EVOLUTION_NINE = 109,		// Silence
	ALIEN_EVOLUTION_TEN = 110,		// Cloaking
	ALIEN_EVOLUTION_ELEVEN = 111,	// Pheromones
	ALIEN_EVOLUTION_TWELVE = 112,	// Scent of fear

	// Alien lifeforms
	ALIEN_LIFEFORM_ONE = 113,
	ALIEN_LIFEFORM_TWO = 114,
	ALIEN_LIFEFORM_THREE = 115,
	ALIEN_LIFEFORM_FOUR = 116,
	ALIEN_LIFEFORM_FIVE = 117,

	// Unlock alien abilities in Combat mode
	BUILD_RESUPPLY = 31,
	ALIEN_HIVE_TWO_UNLOCK = 118,
	ALIEN_HIVE_THREE_UNLOCK = 126,
	COMBAT_TIER2_UNLOCK = 31,
	COMBAT_TIER3_UNLOCK = 54,
	
	// Alien abilities
	ALIEN_ABILITY_LEAP = 119,
	ALIEN_ABILITY_BLINK = 120,
	ALIEN_ABILITY_CHARGE = 122,

	COMMANDER_NEXTIDLE = 123,
	COMMANDER_NEXTAMMO = 124,
	COMMANDER_NEXTHEALTH = 125,

	MESSAGE_LAST = 127
	
	// NOTE: If this gets larger then a byte, AvHTechNode will have to change it's networking, possibly other code too
	// NOTE: When changing any of these values, make sure to update titles.txt, skill.cfg and dlls\game.cpp, and tech*s.spr
	
} AvHMessageID;

#endif