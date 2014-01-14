//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHPlayerUpgrade.cpp $
// $Date: 2002/11/05 06:17:26 $
//
//-------------------------------------------------------------------------------
// $Log: AvHPlayerUpgrade.cpp,v $
// Revision 1.25  2002/11/05 06:17:26  Flayra
// - Balance changes
//
// Revision 1.24  2002/10/20 02:36:14  Flayra
// - Regular update
//
// Revision 1.23  2002/10/18 22:22:04  Flayra
// - Toned down fade armor
//
// Revision 1.22  2002/10/04 18:03:43  Flayra
// - Soldier armor reduced back to 50 + 20 per upgrade (instead of 100 + 20).  Means two skulk bites to kill.
//
// Revision 1.21  2002/10/03 19:02:19  Flayra
// - Toned down primal scream
//
// Revision 1.20  2002/09/25 20:50:23  Flayra
// - Increased armor for soldiers to 100
// - Heavy armor now takes 100% of damage
//
// Revision 1.19  2002/09/23 22:02:56  Flayra
// - Fixed bug where damage upgrades were applying when they shouldn't (for aliens)
// - Added heavy armor
//
// Revision 1.18  2002/09/09 20:05:20  Flayra
// - More heavily armored cocooned aliens to allow them to morph in combat a bit more and for tension when encountered
//
// Revision 1.17  2002/08/31 18:01:02  Flayra
// - Work at VALVe
//
// Revision 1.16  2002/08/16 02:43:16  Flayra
// - Adjusted power armor slightly so it's not worse then regular armor at any point
//
// Revision 1.15  2002/07/28 19:21:28  Flayra
// - Balance changes after/during RC4a
//
// Revision 1.14  2002/07/23 17:18:47  Flayra
// - Level 1 armor toned down, power armor changes
//
// Revision 1.13  2002/07/01 21:43:56  Flayra
// - Added offensive upgrade back, when under the influence...of primal scream!
//
// Revision 1.12  2002/06/25 18:14:40  Flayra
// - Removed old offensive upgrades
//
// Revision 1.11  2002/06/03 16:55:49  Flayra
// - Toned down carapace and marine upgrades
//
// Revision 1.10  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHSpecials.h"
#include "util/Balance.h"
#include "common/damagetypes.h"

const int	kWeaponTracerDefault		= 0;
const int	kWeaponTracerLevelOne		= 6;
const int	kWeaponTracerLevelTwo		= 4;
const int	kWeaponTracerLevelThree		= 2;

float AvHPlayerUpgrade::GetAlienMeleeDamageUpgrade(int inUpgrade, bool inIncludeFocus)
{
	float theMultiplier = 1.0f;
	
	if(GetHasUpgrade(inUpgrade, MASK_BUFFED))
	{
		theMultiplier = 1.0f + (float)BALANCE_VAR(kPrimalScreamDamageModifier);
	}

    if(inIncludeFocus)
    {
        theMultiplier *= AvHPlayerUpgrade::GetFocusDamageUpgrade(inUpgrade);
    }
	
	return theMultiplier;
}

float AvHPlayerUpgrade::GetAlienRangedDamageUpgrade(int inUpgrade)
{
	float theMultiplier = 1.0f;
	
	if(GetHasUpgrade(inUpgrade, MASK_BUFFED))
	{
		theMultiplier = 1.0f + (float)BALANCE_VAR(kPrimalScreamDamageModifier);
	}

	return theMultiplier;
}

float AvHPlayerUpgrade::GetFocusDamageUpgrade(int inUpgrade)
{
	float theFocusDamageUpgrade = 1.0f;

	int theFocusLevel = AvHGetAlienUpgradeLevel(inUpgrade, MASK_UPGRADE_8);
	if(theFocusLevel > 0)
	{
		// Increase damage for each level of focus
		const float theFocusDamageUpgradePercentPerLevel = (float)BALANCE_VAR(kFocusDamageUpgradePercentPerLevel);
		theFocusDamageUpgrade += theFocusLevel*theFocusDamageUpgradePercentPerLevel;
	}

	return theFocusDamageUpgrade;
}

float AvHPlayerUpgrade::GetArmorValue(int inNumHives)
{
	// Each point of armor is work 1/x points of health
	float theArmorBonus = BALANCE_VAR(kArmorValueNonAlien);
	if(inNumHives == 3)
	{
		//float theArmorValueBase = 1.0f + (float)BALANCE_VAR(kArmorValueBase);
		//float theArmorValuePerHive = (float)BALANCE_VAR(kArmorValuePerHive);
		//inNumHives = min(inNumHives, kMaxHives);

		//theArmorBonus = (theArmorValueBase + inNumHives*theArmorValuePerHive);
		theArmorBonus+=kArmorValuePerHive;
	}

	// Smaller is better
	theArmorBonus = 1.0f/theArmorBonus;

	return theArmorBonus;
}

float AvHPlayerUpgrade::GetArmorAbsorption(AvHUser3 inUser3, int inUpgrade, int inNumHives)
{
	// A value of .2 means the armor Takes 80% of the damage, so the value gets smaller as it improves
	float theAbsorption = (float)BALANCE_VAR(kArmorAbsorptionBase);
	inNumHives = min(inNumHives, kMaxHives);//: prevent aliens taking negative damage if some mapper goon (or me :o) ) decides to put more than 3 hives on the map.

	// Heavy armor is the shiznit
	if(inUser3 == AVH_USER3_MARINE_PLAYER && GetHasUpgrade(inUpgrade, MASK_UPGRADE_13))
	{
		float theHeavyArmorAbsorbPercent = BALANCE_VAR(kHeavyArmorAbsorbPercent)/100.0f;
		ASSERT(theHeavyArmorAbsorbPercent >= 0.0f);
		ASSERT(theHeavyArmorAbsorbPercent <= 1.0f);

		theAbsorption = 1.0f - theHeavyArmorAbsorbPercent;
	}

  // Increase absorption at higher hive-levels to make sure armor is always used before player dies
    if(inNumHives==3)
    {
        switch(inUser3)
        {
        case AVH_USER3_ALIEN_PLAYER1:
        case AVH_USER3_ALIEN_PLAYER2:
        case AVH_USER3_ALIEN_PLAYER3:
        case AVH_USER3_ALIEN_PLAYER4:
        case AVH_USER3_ALIEN_PLAYER5:
        case AVH_USER3_ALIEN_EMBRYO:
            theAbsorption -= (float)BALANCE_VAR(kArmorAbsorptionPerExtraHive);
            break;
        }
    }

	ASSERT(theAbsorption >= 0.0f);
	ASSERT(theAbsorption <= 1.0f);

	return theAbsorption;
}

int AvHPlayerUpgrade::GetMaxHealth(int inUpgrade, AvHUser3 inUser3, int inLevel)
{
	int theMaxHealth = 0;
	int theHealthLevelIncrementPercent = BALANCE_VAR(kCombatLevelHealthIncrease);

	// TODO: Take into account upgrade if added
	
	switch(inUser3)
	{
	default:
	case AVH_USER3_MARINE_PLAYER:
	case AVH_USER3_COMMANDER_PLAYER:
		theMaxHealth = BALANCE_VAR(kMarineHealth);
		break;
		
	case AVH_USER3_ALIEN_PLAYER1:
		theMaxHealth = BALANCE_VAR(kSkulkHealth);
		break;
		
	case AVH_USER3_ALIEN_PLAYER2:
		theMaxHealth = BALANCE_VAR(kGorgeHealth);
		break;
		
	case AVH_USER3_ALIEN_PLAYER3:
		theMaxHealth = BALANCE_VAR(kLerkHealth);
		break;
		
	case AVH_USER3_ALIEN_PLAYER4:
		theMaxHealth = BALANCE_VAR(kFadeHealth);
		break;
		
	case AVH_USER3_ALIEN_PLAYER5:
		theMaxHealth = BALANCE_VAR(kOnosHealth);
		break;

	case AVH_USER3_ALIEN_EMBRYO:
		theMaxHealth = BALANCE_VAR(kGestateHealth);
		break;
	}
	
	//return (1.0f + (inLevel - 1)*theHealthLevelIncrementPercent)*theMaxHealth;
	return theMaxHealth + (inLevel - 1)*theHealthLevelIncrementPercent;
}


int AvHPlayerUpgrade::GetMaxArmorLevel(int inUpgrade, AvHUser3 inUser3)
{
	int theMaxArmorLevel = 0;

	int theArmorLevel = AvHPlayerUpgrade::GetArmorUpgrade(inUser3, inUpgrade);
	//bool theHasAlienCarapace = GetHasUpgrade(inUpgrade, MASK_UPGRADE_1);
	bool theHasPowerArmor = GetHasUpgrade(inUpgrade, MASK_UPGRADE_10);
	bool theHasHeavyArmor = GetHasUpgrade(inUpgrade, MASK_UPGRADE_13);

	// Upgrade TODO: Take different levels into account?

	switch(inUser3)
	{
	default:
	case AVH_USER3_MARINE_PLAYER:
	case AVH_USER3_COMMANDER_PLAYER:
		//theMaxArmorLevel = 100 + theArmorLevel*20;
		theMaxArmorLevel = BALANCE_VAR(kMarineBaseArmor) + (int)((theArmorLevel/3.0f)*BALANCE_VAR(kMarineArmorUpgrade));
		if(theHasHeavyArmor)
		{
			theMaxArmorLevel = BALANCE_VAR(kMarineBaseHeavyArmor) + (int)((theArmorLevel/3.0f)*BALANCE_VAR(kMarineHeavyArmorUpgrade));
		}
		//if(theHasPowerArmor)
		//{
		//	theMaxArmorLevel *= 2;
		//}
		break;

	case AVH_USER3_ALIEN_PLAYER1:
		theMaxArmorLevel = BALANCE_VAR(kSkulkBaseArmor) + (int)((theArmorLevel/3.0f)*BALANCE_VAR(kSkulkArmorUpgrade));//(theHasAlienCarapace ? 30 : 10);
		break;

	case AVH_USER3_ALIEN_PLAYER2:
		theMaxArmorLevel = BALANCE_VAR(kGorgeBaseArmor) + (int)((theArmorLevel/3.0f)*BALANCE_VAR(kGorgeArmorUpgrade));//(theHasAlienCarapace ? 75 : 50);
		break;

	case AVH_USER3_ALIEN_PLAYER3:
		theMaxArmorLevel = BALANCE_VAR(kLerkBaseArmor) + (int)((theArmorLevel/3.0f)*BALANCE_VAR(kLerkArmorUpgrade));//(theHasAlienCarapace ? 75 : 50);
		break;																								

	case AVH_USER3_ALIEN_PLAYER4:
		theMaxArmorLevel = BALANCE_VAR(kFadeBaseArmor) + (int)((theArmorLevel/3.0f)*BALANCE_VAR(kFadeArmorUpgrade));//(theHasAlienCarapace ? 150 : 125);
		break;

	case AVH_USER3_ALIEN_PLAYER5:
		theMaxArmorLevel = BALANCE_VAR(kOnosBaseArmor) + (int)((theArmorLevel/3.0f)*BALANCE_VAR(kOnosArmorUpgrade));//(theHasAlienCarapace ? 200 : 150);
		break;

	case AVH_USER3_ALIEN_EMBRYO:
		theMaxArmorLevel = BALANCE_VAR(kGestateBaseArmor);
		break;

	}

	return theMaxArmorLevel;
}

// Returns the level: 0, 1, 2 or 3
int AvHPlayerUpgrade::GetArmorUpgrade(AvHUser3 inUser3, int inUpgrade, float* theArmorMultiplier)
{
	int theUpgradeLevel = 0;

	if(theArmorMultiplier)
		*theArmorMultiplier = 1.0f;

	bool theIsMarine = false;
	switch(inUser3)
	{
	case AVH_USER3_MARINE_PLAYER:
	case AVH_USER3_COMMANDER_PLAYER:
		theIsMarine = true;
		break;
	}

	if(theIsMarine)
	{
		if(GetHasUpgrade(inUpgrade, MASK_UPGRADE_6))
		{
			if(theArmorMultiplier)
				*theArmorMultiplier = 1.0f + (float)BALANCE_VAR(kMarineArmorLevelThree);
			theUpgradeLevel = 3;
		}
		else if(GetHasUpgrade(inUpgrade, MASK_UPGRADE_5))
		{
			if(theArmorMultiplier)
				*theArmorMultiplier = 1.0f + (float)BALANCE_VAR(kMarineArmorLevelTwo);
			
			theUpgradeLevel = 2;
		}
		else if(GetHasUpgrade(inUpgrade, MASK_UPGRADE_4))
		{
			if(theArmorMultiplier)
				*theArmorMultiplier = 1.0f + (float)BALANCE_VAR(kMarineArmorLevelOne);
			
			theUpgradeLevel = 1;
		}
    }
	else
	{
		if(GetHasUpgrade(inUpgrade, MASK_UPGRADE_1))
		{
			if(theArmorMultiplier)
			{
				*theArmorMultiplier = 1.0f + (float)BALANCE_VAR(kAlienArmorLevelOne);
			}
			theUpgradeLevel = 1;

			if(GetHasUpgrade(inUpgrade, MASK_UPGRADE_10))
			{
				if(theArmorMultiplier)
				{
					*theArmorMultiplier = 1.0f + (float)BALANCE_VAR(kAlienArmorLevelTwo);
				}
				
				theUpgradeLevel = 2;
			}
			if(GetHasUpgrade(inUpgrade, MASK_UPGRADE_11))
			{
				if(theArmorMultiplier)
				{
					*theArmorMultiplier = 1.0f + (float)BALANCE_VAR(kAlienArmorLevelThree);
				}
				
				theUpgradeLevel = 3;
			}
		}
	}
	
	return theUpgradeLevel;
}

// Returns the level: 0, 1, 2 or 3
int AvHPlayerUpgrade::GetWeaponUpgrade(int inUser3, int inUpgrade, float* outDamageMultiplier, int* outTracerFreq, float* outSpread)
{
	int theUpgradeLevel = 0;
	
	if(outDamageMultiplier)
		*outDamageMultiplier = 1.0f;

	if(outTracerFreq)
		*outTracerFreq = kWeaponTracerDefault;

	// Only marines and marine items can get damage upgrades
	switch(inUser3)
	{
	case AVH_USER3_MARINE_PLAYER:
	case AVH_USER3_TURRET:
	case AVH_USER3_SIEGETURRET:
	case AVH_USER3_MARINEITEM:
    case AVH_USER3_MINE:
	case AVH_USER3_NUKE:
		// Apply extra damage for upgrade
		if(GetHasUpgrade(inUpgrade, MASK_UPGRADE_3))
		{
			if(outDamageMultiplier)
				*outDamageMultiplier *= (1.0f + (float)BALANCE_VAR(kWeaponDamageLevelThree));
			if(outTracerFreq)
				*outTracerFreq = kWeaponTracerLevelThree;
		
			theUpgradeLevel = 3;
		} 
		else if(GetHasUpgrade(inUpgrade, MASK_UPGRADE_2))
		{
			if(outDamageMultiplier)
				*outDamageMultiplier *= (1.0f + (float)BALANCE_VAR(kWeaponDamageLevelTwo));
			if(outTracerFreq)
				*outTracerFreq = kWeaponTracerLevelTwo;
			theUpgradeLevel = 2;
		}
		else if(GetHasUpgrade(inUpgrade, MASK_UPGRADE_1))
		{
			if(outDamageMultiplier)
				*outDamageMultiplier *= (1.0f + (float)BALANCE_VAR(kWeaponDamageLevelOne));
		
			if(outTracerFreq)
				*outTracerFreq = kWeaponTracerLevelOne;
		
			theUpgradeLevel = 1;
		}
		break;
	}
	
	return theUpgradeLevel;
}

float AvHPlayerUpgrade::GetSilenceVolumeLevel(AvHUser3 inUser3, int inUpgrade)
{
	int theSilenceLevel = 0;

	switch(inUser3)
	{
	case AVH_USER3_ALIEN_PLAYER1:
	case AVH_USER3_ALIEN_PLAYER2:
	case AVH_USER3_ALIEN_PLAYER3:
	case AVH_USER3_ALIEN_PLAYER4:
	case AVH_USER3_ALIEN_PLAYER5:
	case AVH_USER3_ALIEN_EMBRYO:
		theSilenceLevel = AvHGetAlienUpgradeLevel(inUpgrade, MASK_UPGRADE_6);
		break;
	}
	
	//float theSilenceVolumeFactor = (1 - (theSilenceLevel/3.0f));

	float theSilenceVolumeFactor = 1.0f;
	switch(theSilenceLevel)
	{
	case 1:
		theSilenceVolumeFactor = (float)BALANCE_VAR(kSilenceLevel1Volume);
		break;
	case 2:
		theSilenceVolumeFactor = (float)BALANCE_VAR(kSilenceLevel2Volume);
		break;
	case 3:
		theSilenceVolumeFactor = (float)BALANCE_VAR(kSilenceLevel3Volume);
		break;
	}

	return theSilenceVolumeFactor;
}

float AvHPlayerUpgrade::CalculateDamageLessArmor(AvHUser3 inUser3, int inUser4, float flDamage, float& ioArmorValue, int bitsDamageType, int inNumHives)
{
	// if we're alien, if we have the armor upgrade, we take less damage off the top
//	if(AvHGetIsAlien(inUser3))
//	{
//		int theArmorUpgradeLevel = AvHGetAlienUpgradeLevel(inUser4, MASK_UPGRADE_1);
//		if((theArmorUpgradeLevel > 0) && ((int)(ioArmorValue) > 0))
//		{
//			float thePercentageOffTop = .1f*theArmorUpgradeLevel;
//			flDamage -= flDamage*thePercentageOffTop; 
//		}
//	}
	
	float flRatio = AvHPlayerUpgrade::GetArmorAbsorption(inUser3, inUser4, inNumHives);
	float flBonus = AvHPlayerUpgrade::GetArmorValue(inNumHives);
	
	// Level 1 aliens don't take falling damage, ever
	if((inUser3 == AVH_USER3_ALIEN_PLAYER1) && (bitsDamageType & DMG_FALL))
	{
		flDamage = 0.0f;
	}
	
	// Armor. 
	if (ioArmorValue && !(bitsDamageType & (DMG_FALL | DMG_DROWN)) )// armor doesn't protect against fall or drown damage!
	{
		float flNew = flDamage*flRatio;
		float flArmor = (flDamage - flNew)*flBonus;
		
		// Does this use more armor than we have?
		if (flArmor > ioArmorValue)
		{
			flArmor = ioArmorValue;
			flArmor *= (1/flBonus);
			flNew = flDamage - flArmor;
			ioArmorValue = 0;
		}
		else
		{
			ioArmorValue -= flArmor;
			if ( bitsDamageType & (NS_DMG_ACID) )
			{
				ioArmorValue -= flArmor;
			}
		}
	
		flDamage = flNew;
	}
	
	return flDamage;
}

const int kCombatMinLevel = 1;
const int kCombatMaxLevel = 10;

float AvHPlayerUpgrade::GetExperienceForLevel(int inLevel)
{
	float theExperienceForLevel = 0.0f;
	int theLevel = 1;

    int theCombatBaseExperience = BALANCE_VAR(kCombatBaseExperience);
	float theCombatLevelExperienceModifier = (float)BALANCE_VAR(kCombatLevelExperienceModifier);

	while((theLevel < inLevel) && (theCombatLevelExperienceModifier > 0))
	{
		theExperienceForLevel += (1.0f + (theLevel-1)*theCombatLevelExperienceModifier)*theCombatBaseExperience;
		theLevel++;
	}
	
	return theExperienceForLevel;
}

int AvHPlayerUpgrade::GetPlayerLevel(float inExperience)
{
	int thePlayerLevel = 1;

    int theCombatBaseExperience = BALANCE_VAR(kCombatBaseExperience);
	float theCombatLevelExperienceModifier = (float)BALANCE_VAR(kCombatLevelExperienceModifier);

	while((inExperience > 0) && (theCombatLevelExperienceModifier > 0.0f))
	{
		inExperience -= (1.0f + (thePlayerLevel - 1)*theCombatLevelExperienceModifier)*theCombatBaseExperience;

		if(inExperience > 0)
		{
			thePlayerLevel++;
		}
	}

	thePlayerLevel = max(min(thePlayerLevel, kCombatMaxLevel), kCombatMinLevel);

	return thePlayerLevel;
}

float AvHPlayerUpgrade::GetPercentToNextLevel(float inExperience)
{
	int theCurrentLevel = AvHPlayerUpgrade::GetPlayerLevel(inExperience);
	int theNextLevel = min(max(theCurrentLevel + 1, kCombatMinLevel), kCombatMaxLevel);

	float theExperienceForCurrentLevel = AvHPlayerUpgrade::GetExperienceForLevel(theCurrentLevel);
	float theExperienceForNextLevel = AvHPlayerUpgrade::GetExperienceForLevel(theNextLevel);

	float thePercent = (inExperience - theExperienceForCurrentLevel)/(theExperienceForNextLevel - theExperienceForCurrentLevel);

	thePercent = min(max(0.0f, thePercent), 1.0f);

	return thePercent;
}