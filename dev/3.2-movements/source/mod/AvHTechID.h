//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHTechID.h $
// $Date: 2002/09/23 22:35:52 $
//
//-------------------------------------------------------------------------------
// $Log: AvHTechID.h,v $
// Revision 1.8  2002/09/23 22:35:52  Flayra
// - Regular update
//
// Revision 1.7  2002/07/23 17:32:46  Flayra
// - Regular update
//
// Revision 1.6  2002/06/25 18:22:34  Flayra
// - Renamed buildings
//
// Revision 1.5  2002/06/03 17:01:06  Flayra
// - Renamed weapons factory and armory
//
// Revision 1.4  2002/05/23 02:32:57  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_TECH_ID_H
#define AVH_TECH_ID_H

typedef enum
{
	TECH_NULL = 0,

	// Build tech (unlocked when there is at least one building of this type)
	TECH_COMMAND_CENTER = 1,
	//TECH_REINFORCEMENTS = 2,
	TECH_TURRET_FACTORY = 3,
	//TECH_ADVANCED_TURRET_FACTORY = 4,
	TECH_ARMSLAB = 5,
	TECH_PROTOTYPE_LAB = 6,
	TECH_CHEMLAB = 7,
	TECH_ARMORY = 8,
	TECH_ADVANCED_ARMORY = 9,
	TECH_NUKE_PLANT = 10,
	TECH_RESOURCES = 11,
	TECH_OBSERVATORY = 12,
	TECH_PHASE_GATE = 13,
	TECH_MEDLAB = 14,
	TECH_INFANTRYPORTAL = 15,

	// Research tech (permanently unlocked after researching it)
	TECH_RESEARCH_ELECTRICAL = 19,
	TECH_RESEARCH_ARMOR_ONE = 20,
	TECH_RESEARCH_ARMOR_TWO = 21,
	TECH_RESEARCH_ARMOR_THREE = 22,
	TECH_RESEARCH_WEAPONS_ONE = 23,
	TECH_RESEARCH_WEAPONS_TWO = 24,
	TECH_RESEARCH_WEAPONS_THREE = 25,

	TECH_ADVANCED_TURRET_FACTORY = 26,

	TECH_RESEARCH_JETPACKS = 28,
	TECH_RESEARCH_HEAVYARMOR = 29,

	TECH_RESEARCH_DISTRESSBEACON = 30,
	TECH_RESEARCH_HEALTH = 31,
	// 32 is cancel, although there's no real reason to keep these contants lined up, just makes it easier to think about
	TECH_RESEARCH_MOTIONTRACK = 33,
	TECH_RESEARCH_PHASETECH = 34,

    TECH_RESEARCH_CATALYSTS = 35,
    TECH_RESEARCH_GRENADES = 36,

    // Alien tech
    TECH_HIVE = 39,
    TECH_ONE_HIVE = 40,
    TECH_TWO_HIVES = 41,
    TECH_THREE_HIVES = 42,
    TECH_SKULK = 43,
    TECH_GORGE = 44,
    TECH_LERK = 45,
    TECH_FADE = 46,
    TECH_ONOS = 47,
    TECH_MOVEMENT_CHAMBER = 48,
    TECH_DEFENSE_CHAMBER = 49,
    TECH_SENSORY_CHAMBER = 50,
    TECH_OFFENSE_CHAMBER = 51,
    TECH_ALIEN_RESOURCE_NODE = 52,
    
	// Generic tech levels for combat game mode
	TECH_ONE_LEVEL_ONE = 60,
	TECH_ONE_LEVEL_TWO = 61,
	TECH_ONE_LEVEL_THREE = 62,
	TECH_ONE_LEVEL_FOUR = 63,
	TECH_ONE_LEVEL_FIVE = 64,

	TECH_TWO_LEVEL_ONE = 65,
	TECH_TWO_LEVEL_TWO = 66,
	TECH_TWO_LEVEL_THREE = 67,
	TECH_TWO_LEVEL_FOUR = 68,
	TECH_TWO_LEVEL_FIVE = 69,

	TECH_THREE_LEVEL_ONE = 70,
	TECH_THREE_LEVEL_TWO = 71,
	TECH_THREE_LEVEL_THREE = 72,
	TECH_THREE_LEVEL_FOUR = 73,
	TECH_THREE_LEVEL_FIVE = 74,
	
	TECH_FOUR_LEVEL_ONE = 75,
	TECH_FOUR_LEVEL_TWO = 76,
	TECH_FOUR_LEVEL_THREE = 77,
	TECH_FOUR_LEVEL_FOUR = 78,
	TECH_FOUR_LEVEL_FIVE = 79,
	
	TECH_FIVE_LEVEL_ONE = 80,
	TECH_FIVE_LEVEL_TWO = 81,
	TECH_FIVE_LEVEL_THREE = 82,
	TECH_FIVE_LEVEL_FOUR = 83,
	TECH_FIVE_LEVEL_FIVE = 84,

	TECH_PLAYER_TIER_ONE = 85,
	TECH_PLAYER_TIER_TWO = 86,
	TECH_PLAYER_TIER_THREE = 87,

    // Gives a way to add a technology to the tech tree that will never become available
    TECH_PLAYER_UNAVAILABLE = 88
	
} AvHTechID;

#endif