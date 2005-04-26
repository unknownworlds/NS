//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHPlayerUpgrade.h $
// $Date: 2002/09/23 22:02:57 $
//
//-------------------------------------------------------------------------------
// $Log: AvHPlayerUpgrade.h,v $
// Revision 1.7  2002/09/23 22:02:57  Flayra
// - Fixed bug where damage upgrades were applying when they shouldn't (for aliens)
// - Added heavy armor
//
// Revision 1.6  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_PLAYER_UPGRADE_H
#define AVH_PLAYER_UPGRADE_H

#include "util/nowarnings.h"
#include "types.h"
#include "mod/AvHSpecials.h"

class AvHPlayerUpgrade
{
public:
	static float GetAlienMeleeDamageUpgrade(int inUpgrade, bool inIncludeFocus = false);
	static float GetAlienRangedDamageUpgrade(int inUpgrade);
	static float GetFocusDamageUpgrade(int inUpgrade);

	static float GetArmorValue(int inNumHives = -1);
	static float GetArmorAbsorption(AvHUser3 inUser3, int inUpgrade, int inNumHives = -1);
	static int GetMaxArmorLevel(int inUpgrade, AvHUser3 inUser3);
	static int GetMaxHealth(int inUpgrade, AvHUser3 inUser3, int inLevel = 1);
	static int GetArmorUpgrade(AvHUser3 inUser3, int inUpgrade, float* outArmorMultiplier = NULL);
	static float GetSilenceVolumeLevel(AvHUser3 inUser3, int inUpgrade);
	static int GetWeaponUpgrade(int inUser3, int inUpgrade, float* theDamageMultiplier = NULL, int* theTracerFreq = NULL, float* theSpread = NULL);
	static float CalculateDamageLessArmor(AvHUser3 inUser3, int inUser4, float flDamage, float& ioArmorValue, int bitsDamageType, int inNumHives = -1);
	static float GetExperienceForLevel(int inLevel);
	static int GetPlayerLevel(float inExperience);
	static float GetPercentToNextLevel(float inExperience);
};

#endif