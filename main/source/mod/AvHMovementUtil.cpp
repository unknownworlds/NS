//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHMovementUtil.cpp $
// $Date: 2002/10/24 21:34:02 $
//
//-------------------------------------------------------------------------------
// $Log: AvHMovementUtil.cpp,v $
// Revision 1.20  2002/10/24 21:34:02  Flayra
// - Less rockets
//
// Revision 1.19  2002/10/04 18:04:27  Flayra
// - Fixed floating gestation sacs
// - Aliens now fall all the way to ground during countdown (instead of floating and shaking)
//
// Revision 1.18  2002/10/03 18:59:04  Flayra
// - Refactored energy
//
// Revision 1.17  2002/09/09 20:00:10  Flayra
// - Balance changes
//
// Revision 1.16  2002/08/16 02:40:14  Flayra
// - Regular balance update
//
// Revision 1.15  2002/08/09 01:08:30  Flayra
// - Regular update
//
// Revision 1.14  2002/08/02 21:53:47  Flayra
// - Various energy tweaks, so melee attacks don't run out when attacking buildings as often, and so fliers can fly and bite/spike
//
// Revision 1.13  2002/07/23 17:14:45  Flayra
// - Energy updates
//
// Revision 1.12  2002/07/08 17:12:05  Flayra
// - Regular update
//
// Revision 1.11  2002/07/01 21:38:46  Flayra
// - Primal scream gives energy back faster, added energy usage for new weapons
//
// Revision 1.10  2002/06/25 18:08:40  Flayra
// - Energy costs tweaking, added new weapons, added charging
//
// Revision 1.9  2002/06/03 16:52:36  Flayra
// - Tweaked spike energy cost
//
// Revision 1.8  2002/05/28 17:54:45  Flayra
// - Tweaked costs for swipe and web
//
// Revision 1.7  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHMovementUtil.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHHulls.h"
#include "mod/AvHConstants.h"
#include "util/Balance.h"
#include "types.h"

int AvHMUGetHull(bool inIsDucking, int inUserVar)
{
	int theHull = 0;

	// For marines and level 4
	theHull = inIsDucking ? kHLCrouchHullIndex : kHLStandHullIndex;
	
	// Set the hull for our special sized players
	if(inUserVar == AVH_USER3_ALIEN_PLAYER1)
	{
		// Always use small hull, even when ducking
		theHull = kHLCrouchHullIndex;
	}
	else if(inUserVar == AVH_USER3_ALIEN_PLAYER2)
	{
		theHull = kHLCrouchHullIndex;
	}
	else if(inUserVar == AVH_USER3_ALIEN_PLAYER3)
	{
		theHull = kHLCrouchHullIndex;
	}
	else if(inUserVar == AVH_USER3_ALIEN_EMBRYO)
	{
		theHull = kHLCrouchHullIndex;
	}
	else if(inUserVar == AVH_USER3_ALIEN_PLAYER5)
	{
		// Use human hull when ducking, largest otherwise
		theHull = inIsDucking ? kHLStandHullIndex : kNSHugeHullIndex;
	}
	
	return theHull;
}

int AvHMUGetOriginOffsetForUser3(AvHUser3 inUser3)
{
	int theOffset = 20;

	if(inUser3 == AVH_USER3_ALIEN_PLAYER5)
	{
		theOffset = 40;
	}
	
	return theOffset;
}

int AvHMUGetOriginOffsetForMessageID(AvHMessageID inMessageID)
{
	AvHUser3 theUser3 = AVH_USER3_NONE;
	
	if(inMessageID == ALIEN_LIFEFORM_FIVE)
	{
		theUser3 = AVH_USER3_ALIEN_PLAYER5;
	}
	
	return AvHMUGetOriginOffsetForUser3(theUser3);
}

bool AvHMUGetCanDuck(int inUser3)
{
	bool theCanDuck = true;
	
	if((inUser3 == AVH_USER3_ALIEN_PLAYER1) || (inUser3 == AVH_USER3_ALIEN_PLAYER2) || (inUser3 == AVH_USER3_ALIEN_PLAYER3) || (inUser3 == AVH_USER3_ALIEN_EMBRYO) )
	{
		theCanDuck = false;
	}
	
	return theCanDuck;
}

bool AvHMUDeductAlienEnergy(float& ioFuser, float inNormAmount)
{
	bool theSuccess = false;

	if(AvHMUHasEnoughAlienEnergy(ioFuser, inNormAmount))
	{
		float theCurrentEnergy = ioFuser/kNormalizationNetworkFactor;

		theCurrentEnergy -= inNormAmount;
		theCurrentEnergy = max(theCurrentEnergy, 0.0f);
		ioFuser = theCurrentEnergy*kNormalizationNetworkFactor;

		theSuccess = true;
	}

	return theSuccess;
}

bool AvHMUGiveAlienEnergy(float& ioFuser, float inNormAmount)
{
	bool theSuccess = false;

	float theCurrentEnergy = ioFuser/kNormalizationNetworkFactor;

	if(theCurrentEnergy < 1.0f)
	{
		theCurrentEnergy += inNormAmount;
		theCurrentEnergy = min(max(theCurrentEnergy, 0.0f), 1.0f);
		ioFuser = theCurrentEnergy*kNormalizationNetworkFactor;
		theSuccess = true;
	}

	return theSuccess;
}

bool AvHMUGetEnergyCost(AvHWeaponID inWeaponID, float& outEnergyCost)
{
	bool theSuccess = false;

	float theCost = 0.0f;
	switch(inWeaponID)
	{
	case AVH_WEAPON_CLAWS:
		theCost = (float)BALANCE_VAR(kClawsEnergyCost);
		break;
	case AVH_WEAPON_SPIT:
		theCost = (float)BALANCE_VAR(kSpitEnergyCost);
		break;
	case AVH_WEAPON_SPORES:
		theCost = (float)BALANCE_VAR(kSporesEnergyCost);
		break;
	case AVH_WEAPON_SPIKE:
		theCost = (float)BALANCE_VAR(kSpikeEnergyCost);
		break;
	case AVH_WEAPON_BITE:
		theCost = (float)BALANCE_VAR(kBiteEnergyCost);
		break;
    case AVH_WEAPON_BITE2:
        theCost = (float)BALANCE_VAR(kBite2EnergyCost);
        break;
	case AVH_WEAPON_SWIPE:
		theCost = (float)BALANCE_VAR(kSwipeEnergyCost);
		break;
	case AVH_WEAPON_BLINK:
		theCost = (float)BALANCE_VAR(kBlinkEnergyCost);
		break;
	case AVH_WEAPON_WEBSPINNER:
		theCost = (float)BALANCE_VAR(kWebEnergyCost);
		break;
	case AVH_WEAPON_PARASITE:
		theCost = (float)BALANCE_VAR(kParasiteEnergyCost);
		break;
	case AVH_WEAPON_DIVINEWIND:
		theCost = (float)BALANCE_VAR(kDivineWindEnergyCost);
		break;
	case AVH_WEAPON_HEALINGSPRAY:
		theCost = (float)BALANCE_VAR(kHealingSprayEnergyCost);
		break;
	case AVH_WEAPON_METABOLIZE:
		theCost = (float)BALANCE_VAR(kMetabolizeEnergyCost);
		break;
	case AVH_WEAPON_UMBRA:
		theCost = (float)BALANCE_VAR(kUmbraEnergyCost);
		break;
	case AVH_WEAPON_PRIMALSCREAM:
		theCost = (float)BALANCE_VAR(kPrimalScreamEnergyCost);
		break;
	case AVH_WEAPON_BILEBOMB:
		theCost = (float)BALANCE_VAR(kBileBombEnergyCost);
		break;
	case AVH_WEAPON_ACIDROCKET:
		theCost = (float)BALANCE_VAR(kAcidRocketEnergyCost);
		break;
	case AVH_WEAPON_STOMP:
		theCost = (float)BALANCE_VAR(kStompEnergyCost);
		break;
	case AVH_WEAPON_DEVOUR:
		theCost = (float)BALANCE_VAR(kDevourEnergyCost);
		break;

	// Abilities
	case AVH_ABILITY_LEAP:
		theCost = (float)BALANCE_VAR(kLeapEnergyCost);
		break;
	case AVH_ABILITY_CHARGE:
		theCost = (float)BALANCE_VAR(kChargeEnergyCost);
		break;

	}

	outEnergyCost = theCost;

	if(theCost > 0.0f)
	{
		theSuccess = true;
	}

	return theSuccess;
}

float AvHMUGetWalkSpeedFactor(AvHUser3 inUser3)
{
	float theMoveSpeed = .1f;
	
	switch(inUser3)
	{
	case AVH_USER3_MARINE_PLAYER:
		theMoveSpeed = .095f;
		break;
	case AVH_USER3_ALIEN_PLAYER1:
		//theMoveSpeed = .04f;
		theMoveSpeed = .14f;
		break;
	case AVH_USER3_ALIEN_PLAYER2:
		theMoveSpeed = .08f;
		break;
	case AVH_USER3_ALIEN_PLAYER3:
		theMoveSpeed = .11f;
		break;
	case AVH_USER3_ALIEN_PLAYER4:
		theMoveSpeed = .09f;
		break;
	case AVH_USER3_ALIEN_PLAYER5:
		theMoveSpeed = .09f;
		break;
	}
	
	return theMoveSpeed;
}

bool AvHMUHasEnoughAlienEnergy(float& ioFuser, float inNormAmount)
{
	bool theSuccess = false;
	
	float theCurrentEnergy = ioFuser/kNormalizationNetworkFactor;
	if(theCurrentEnergy >= inNormAmount)
	{
		theSuccess = true;
	}
	
	return theSuccess;
}

void AvHMUUpdateAlienEnergy(float inTimePassed, int inUser3, int inUser4, float& ioFuser)
{
	if(	(inUser3 == AVH_USER3_ALIEN_PLAYER1) ||
		(inUser3 == AVH_USER3_ALIEN_PLAYER2) ||
		(inUser3 == AVH_USER3_ALIEN_PLAYER3) ||
		(inUser3 == AVH_USER3_ALIEN_PLAYER4) ||
		(inUser3 == AVH_USER3_ALIEN_PLAYER5))
	{
		if(!GetHasUpgrade(inUser4, MASK_PLAYER_STUNNED))
		{
			// Percentage (0-1) per second
			float theAlienEnergyRate = (float)BALANCE_VAR(kAlienEnergyRate);
			//float kFadeChargingDeplectionRate = -2.8f*kAlienEnergyRate;
			float kChargingDepletionRate = -BALANCE_VAR(kChargingEnergyScalar)*theAlienEnergyRate;

			const float kMultiplier = GetHasUpgrade(inUser4, MASK_BUFFED) ? (1.0f + BALANCE_VAR(kPrimalScreamEnergyFactor)) : 1.0f;
			float theEnergyRate = theAlienEnergyRate*kMultiplier;

			float theUpgradeFactor = 1.0f;
			int theNumLevels = AvHGetAlienUpgradeLevel(inUser4, MASK_UPGRADE_5);
			if(theNumLevels > 0)
			{

				theUpgradeFactor += theNumLevels*BALANCE_VAR(kAdrenalineEnergyPercentPerLevel);
			}

			float theCurrentEnergy = ioFuser/kNormalizationNetworkFactor;
			
			float theNewEnergy = theCurrentEnergy + inTimePassed*theAlienEnergyRate*theUpgradeFactor;

			// If we're charging, reduce energy
			if(GetHasUpgrade(inUser4, MASK_ALIEN_MOVEMENT))
			{
				if(inUser3 == AVH_USER3_ALIEN_PLAYER4)
				{
//					theNewEnergy += inTimePassed*kFadeChargingDeplectionRate;
				}
				else
				{
					theNewEnergy += inTimePassed*kChargingDepletionRate;
				}
			}
			
			theNewEnergy = min(max(theNewEnergy, 0.0f), 1.0f);
			
			ioFuser = theNewEnergy*kNormalizationNetworkFactor;
		}
	}
}

void AvHMUUpdateJetpackEnergy(bool inIsJetpacking, float theTimePassed, float& ioJetpackEnergy)
{
	if(inIsJetpacking)
	{
		ioJetpackEnergy -= theTimePassed*kJetpackEnergyLossRate;
	}
	else
	{
		ioJetpackEnergy += theTimePassed*kJetpackEnergyGainRate;
	}
	
	ioJetpackEnergy = min(1.0f, ioJetpackEnergy);
	ioJetpackEnergy = max(0.0f, ioJetpackEnergy);
}
