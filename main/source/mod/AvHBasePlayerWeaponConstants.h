//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHBasePlayerWeaponConstants.h$
// $Date: 2002/07/01 21:17:38 $
//
//-------------------------------------------------------------------------------
// $Log: AvHBasePlayerWeaponConstants.h,v $
// Revision 1.18  2002/07/01 21:17:38  Flayra
// - Added babbler id
//
// Revision 1.17  2002/06/25 17:42:02  Flayra
// - Removed building weapons, added new weapons
//
// Revision 1.16  2002/06/03 16:29:53  Flayra
// - Added resupply (from arsenal), better animation support (for both view model and player model)
//
// Revision 1.15  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
// Revision 1.16  2002/05/14 19:03:01  Charlie
//===============================================================================
#ifndef AVHBASEPLAYERWEAPONCONSTANTS_H
#define AVHBASEPLAYERWEAPONCONSTANTS_H

typedef enum
{
	AVH_WEAPON_NONE = 0,

	// Alien weapons
	AVH_WEAPON_CLAWS,
	AVH_WEAPON_SPIT,
	AVH_WEAPON_SPORES,
	AVH_WEAPON_SPIKE,
	AVH_WEAPON_BITE,		// Level 1 bite
	AVH_WEAPON_BITE2,		// Level 3 bite
	AVH_WEAPON_SWIPE,
	AVH_WEAPON_WEBSPINNER,
	AVH_WEAPON_METABOLIZE,
	AVH_WEAPON_PARASITE,
	AVH_WEAPON_BLINK,
	AVH_WEAPON_DIVINEWIND,

	// Adding a new weapon?  Don't forget to add it's weight in AvHGamerules::GetWeightForItemAndAmmo(AvHWeaponID inWeapon, int inNumRounds)
	AVH_WEAPON_KNIFE,
	AVH_WEAPON_PISTOL,
	AVH_WEAPON_MG,
	AVH_WEAPON_SONIC,
	AVH_WEAPON_HMG,
	AVH_WEAPON_WELDER,
	AVH_WEAPON_MINE,
	AVH_WEAPON_GRENADE_GUN,

	// Abilities
	AVH_ABILITY_LEAP,
	AVH_ABILITY_CHARGE,

	AVH_WEAPON_UMBRA,
	AVH_WEAPON_PRIMALSCREAM,
	AVH_WEAPON_BILEBOMB,
	AVH_WEAPON_ACIDROCKET,
	AVH_WEAPON_HEALINGSPRAY,
	AVH_WEAPON_GRENADE,
	AVH_WEAPON_STOMP,
	AVH_WEAPON_DEVOUR,

	// Can't go over 32 (client.cpp, GetWeaponData())

	AVH_WEAPON_MAX
}
AvHWeaponID;

#define			kWeaponMinSize			Vector(-16, -16, 0)
#define			kWeaponMaxSize			Vector(16, 16, 28)

#define			kGenericWallpuff		"sprites/wallpuff.spr"
#define			kNullModel				"models/null.mdl"
#define			kGenericAnimExt			"mp5"

const float		kDeployIdleInterval		= 10.0;

#endif