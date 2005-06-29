//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHSpecials.cpp $
// $Date: 2002/10/24 21:42:50 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSpecials.cpp,v $
// Revision 1.15  2002/10/24 21:42:50  Flayra
// - Fix for allowing multiple upgrades of the same familiy
//
// Revision 1.14  2002/09/23 22:32:51  Flayra
// - Removed power armor
// - Added heavy armor and jetpacks
//
// Revision 1.13  2002/07/23 17:28:39  Flayra
// - New constants for phase gates and alien buildings, new marine upgrades
//
// Revision 1.12  2002/07/08 17:18:29  Flayra
// - Mark spawn points (can't remember why this is needed), updated comments, removed offensive upgrade code
//
// Revision 1.11  2002/06/25 18:19:17  Flayra
// - Removed old offensive upgrades
//
// Revision 1.10  2002/05/23 02:32:57  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "util/nowarnings.h"
#include "mod/AvHSpecials.h"
#include "localassert.h"
#include "mod/AvHConstants.h"

void InitializeBuildable(int& inUser3, int& inUser4, float& inFuser1, int inUser3ID)
{
	inUser3 = inUser3ID;

	inUser4 = 0;
	SetUpgradeMask(&inUser4, MASK_BUILDABLE);
	SetUpgradeMask(&inUser4, MASK_SELECTABLE);

	inFuser1 = 0.0f;
}


void ClearUpgradeMask(int* inPointer, AvHUpgradeMask inUpgradeMask)
{
	int* thePointer = (int*)inPointer;
	*thePointer &= ~inUpgradeMask;
}

bool GetHasUpgrade(int inUpgrade, AvHUpgradeMask inUpgradeMask)
{
	return inUpgrade & inUpgradeMask;
}

void SetUpgradeMask(int* inPointer, AvHUpgradeMask inUpgradeMask, bool inSet)
{
	if(inSet)
	{
		int* thePointer = (int*)inPointer;
		*thePointer |= inUpgradeMask;
	}
	else
	{
		ClearUpgradeMask(inPointer, inUpgradeMask);
	}
}

AvHUpgradeMask ProcessGenericUpgrade(int& inUpgradeVariable, AvHMessageID inUpgrade, bool inGive)
{
	AvHUpgradeMask theUpgradeMask = MASK_NONE;

	switch(inUpgrade)
	{
	case RESEARCH_WEAPONS_ONE:
		SetUpgradeMask(&inUpgradeVariable, MASK_UPGRADE_1, inGive);
		theUpgradeMask = MASK_UPGRADE_1;
		break;
		
	case RESEARCH_WEAPONS_TWO:
		SetUpgradeMask(&inUpgradeVariable, MASK_UPGRADE_2, inGive);
		theUpgradeMask = MASK_UPGRADE_2;
		break;
		
	case RESEARCH_WEAPONS_THREE:
		SetUpgradeMask(&inUpgradeVariable, MASK_UPGRADE_3, inGive);
		theUpgradeMask = MASK_UPGRADE_3;
		break;
		
	case RESEARCH_ARMOR_ONE:
		SetUpgradeMask(&inUpgradeVariable, MASK_UPGRADE_4, inGive);
		theUpgradeMask = MASK_UPGRADE_4;
		break;
		
	case RESEARCH_ARMOR_TWO:
		SetUpgradeMask(&inUpgradeVariable, MASK_UPGRADE_5, inGive);
		theUpgradeMask = MASK_UPGRADE_5;
		break;
		
	case RESEARCH_ARMOR_THREE:
		SetUpgradeMask(&inUpgradeVariable, MASK_UPGRADE_6, inGive);
		theUpgradeMask = MASK_UPGRADE_6;
		break;

	case RESEARCH_MOTIONTRACK:
		SetUpgradeMask(&inUpgradeVariable, MASK_UPGRADE_8, inGive);
		theUpgradeMask = MASK_UPGRADE_8;
		break;

    case BUILD_HEAVY:
        SetUpgradeMask(&inUpgradeVariable, MASK_UPGRADE_13, inGive);
        theUpgradeMask = MASK_UPGRADE_13;
        break;

    case BUILD_JETPACK:
        SetUpgradeMask(&inUpgradeVariable, MASK_UPGRADE_7, inGive);
        theUpgradeMask = MASK_UPGRADE_7;
        break;

	// Alien upgrades
	case ALIEN_EVOLUTION_ONE:
	case ALIEN_EVOLUTION_TWO:
	case ALIEN_EVOLUTION_THREE:
	case ALIEN_EVOLUTION_SEVEN:
	case ALIEN_EVOLUTION_EIGHT:
	case ALIEN_EVOLUTION_NINE:
	case ALIEN_EVOLUTION_TEN:
	case ALIEN_EVOLUTION_ELEVEN:
	case ALIEN_EVOLUTION_TWELVE:
		{
			if(!AvHGetAlienUpgradeMask(inUpgrade, theUpgradeMask))
			{
				ASSERT(false);
			}
			SetUpgradeMask(&inUpgradeVariable, theUpgradeMask, inGive);
		}
		break;
		
		//default:
		//	ASSERT(false);
		//	break;
	}
	return theUpgradeMask;
}


//// Remove the mask part and return the base.  This function should be changed when the user3 variable mask is changed 
//int AvHGetUser3Base(int inUser3)
//{
//	int theUser3 = inUser3;
//	theUser3 &= ~AVH_USER3_VIS_SIGHTED;
//	theUser3 &= ~AVH_USER3_VIS_DETECTED;
//	return theUser3;
//}

//int AvHGetUser4Base(int inUser4)
//{
//	int theUser4 = inUser4;
//	
//	// Return only the bottom byte
//	theUser4 &= 0x000000FF;
//	
//	return theUser4;
//}
//
//int AvHGetUser4Extra(int inUser4)
//{
//	int theUser4 = inUser4;
//	
//	// Return only the top bit
//	theUser4 &= 0xFFFFFF00;
//	theUser4 >> 8;
//	
//	return theUser4;
//}
//
//void AvHSetUser4(int& inUser4, int inBase, int inExtra)
//{
//	ASSERT(inBase < 0xFF);
//	ASSERT(inExtra < 0xFFFFFF);
//
//	inUser4 = (inBase & 0x000000FF);
//	inUser4 |= (inExtra << 8);
//}

bool AvHGetAlienUpgradeCategory(AvHMessageID inUpgrade, AvHAlienUpgradeCategory& outCategory)
{
	bool theSuccess = false;

	switch(inUpgrade)
	{
	case ALIEN_EVOLUTION_ONE:
	case ALIEN_EVOLUTION_TWO:
	case ALIEN_EVOLUTION_THREE:
		outCategory = ALIEN_UPGRADE_CATEGORY_DEFENSE;
		theSuccess = true;
		break;
	//case ALIEN_EVOLUTION_FOUR:
	//case ALIEN_EVOLUTION_FIVE:
	//case ALIEN_EVOLUTION_SIX:
	//	outCategory = ALIEN_UPGRADE_CATEGORY_OFFENSE;
	//	theSuccess = true;
	//	break;
	case ALIEN_EVOLUTION_SEVEN:
	case ALIEN_EVOLUTION_EIGHT:
	case ALIEN_EVOLUTION_NINE:
		outCategory = ALIEN_UPGRADE_CATEGORY_MOVEMENT;
		theSuccess = true;
		break;
	case ALIEN_EVOLUTION_TEN:
	case ALIEN_EVOLUTION_ELEVEN:
	case ALIEN_EVOLUTION_TWELVE:
		outCategory = ALIEN_UPGRADE_CATEGORY_SENSORY;
		theSuccess = true;
		break;
	}
	
	return theSuccess;
}

bool AvHGetAlienUpgradeCategoryFromMask(AvHUpgradeMask inUpgradeMask, AvHAlienUpgradeCategory& outCategory)
{
	bool theSuccess = false;

	switch(inUpgradeMask)
	{
	case MASK_UPGRADE_1:
	case MASK_UPGRADE_2:
	case MASK_UPGRADE_3:
		outCategory = ALIEN_UPGRADE_CATEGORY_DEFENSE;
		theSuccess = true;
		break;

//	case MASK_PLACEHOLDER1:
//	case MASK_PLACEHOLDER2:
//	case MASK_PLACEHOLDER3:
//		outCategory = ALIEN_UPGRADE_CATEGORY_OFFENSE;
//		theSuccess = true;
//		break;
		
	case MASK_UPGRADE_4:
	case MASK_UPGRADE_5:
	case MASK_UPGRADE_6:
		outCategory = ALIEN_UPGRADE_CATEGORY_MOVEMENT;
		theSuccess = true;
		break;
		
	case MASK_UPGRADE_7:
	case MASK_UPGRADE_8:
	case MASK_UPGRADE_9:
		outCategory = ALIEN_UPGRADE_CATEGORY_SENSORY;
		theSuccess = true;
		break;
	}
	
	return theSuccess;
}

int AvHGetAlienUpgradeLevel(int inUser4, AvHUpgradeMask inMask)
{
	int theLevel = 0;

	if(GetHasUpgrade(inUser4, inMask))
	{
		theLevel = 1;

		switch(inMask)
		{
		// Defensive upgrades
		case MASK_UPGRADE_1:
		case MASK_UPGRADE_2:
		case MASK_UPGRADE_3:
			if(GetHasUpgrade(inUser4, MASK_UPGRADE_10))
			{
				theLevel = 2;
			}
			if(GetHasUpgrade(inUser4, MASK_UPGRADE_11))
			{
				theLevel = 3;
			}
			break;
		
		// Movement upgrades
		case MASK_UPGRADE_4:
		case MASK_UPGRADE_5:
		case MASK_UPGRADE_6:
			if(GetHasUpgrade(inUser4, MASK_UPGRADE_12))
			{
				theLevel = 2;
			}
			if(GetHasUpgrade(inUser4, MASK_UPGRADE_13))
			{
				theLevel = 3;
			}
			break;
			
		// Sensory upgrades
		case MASK_UPGRADE_7:
		case MASK_UPGRADE_8:
		case MASK_UPGRADE_9:
			if(GetHasUpgrade(inUser4, MASK_UPGRADE_14))
			{
				theLevel = 2;
			}
			if(GetHasUpgrade(inUser4, MASK_UPGRADE_15))
			{
				theLevel = 3;
			}
			break;
		}
	}
	return theLevel;
}

bool AvHGetAlienUpgradeMask(AvHMessageID inUpgrade, AvHUpgradeMask& outUpgradeMask)
{
	bool theSuccess = false;
	
	switch(inUpgrade)
	{
	case ALIEN_EVOLUTION_ONE:
		outUpgradeMask = MASK_UPGRADE_1;
		theSuccess = true;
		break;
	case ALIEN_EVOLUTION_TWO:
		outUpgradeMask = MASK_UPGRADE_2;
		theSuccess = true;
		break;
	case ALIEN_EVOLUTION_THREE:
		outUpgradeMask = MASK_UPGRADE_3;
		theSuccess = true;
		break;
//	case ALIEN_EVOLUTION_FOUR:
//		outUpgradeMask = MASK_PLACEHOLDER1;
//		theSuccess = true;
//		break;
//	case ALIEN_EVOLUTION_FIVE:
//		outUpgradeMask = MASK_PLACEHOLDER2;
//		theSuccess = true;
//		break;
//	case ALIEN_EVOLUTION_SIX:
//		outUpgradeMask = MASK_PLACEHOLDER3;
//		theSuccess = true;
//		break;
	case ALIEN_EVOLUTION_SEVEN:
		outUpgradeMask = MASK_UPGRADE_4;
		theSuccess = true;
		break;
	case ALIEN_EVOLUTION_EIGHT:
		outUpgradeMask = MASK_UPGRADE_5;
		theSuccess = true;
		break;
	case ALIEN_EVOLUTION_NINE:
		outUpgradeMask = MASK_UPGRADE_6;
		theSuccess = true;
		break;
	case ALIEN_EVOLUTION_TEN:
		outUpgradeMask = MASK_UPGRADE_7;
		theSuccess = true;
		break;
	case ALIEN_EVOLUTION_ELEVEN:
		outUpgradeMask = MASK_UPGRADE_8;
		theSuccess = true;
		break;
	case ALIEN_EVOLUTION_TWELVE:
		outUpgradeMask = MASK_UPGRADE_9;
		theSuccess = true;
		break;
	}
	return theSuccess;
}


bool AvHGetHasFreeUpgradeCategory(AvHAlienUpgradeCategory inUpgradeCategory, const AvHAlienUpgradeListType& inList, int inUser4, int* outNumFree)
{
	bool theSuccess = false;

	// Count how many upgrades of this category the player already has
	int theNumUpgradesAlreadyHave = AvHGetNumUpgradesInCategory(inUpgradeCategory, inUser4);

	// Count how many upgrades of this category in the list
	int theNumUpgradesInCategoryInList = min(kMaxUpgradeLevel, AvHGetNumUpgradesInCategoryInList(inList, inUpgradeCategory));

	if(theNumUpgradesAlreadyHave < theNumUpgradesInCategoryInList)
	{
		if(outNumFree)
		{
			*outNumFree = theNumUpgradesInCategoryInList - theNumUpgradesAlreadyHave;
		}
		theSuccess = true;
	}
	
	return theSuccess;
}

bool AvHGetHasUpgradeChoiceInCategory(AvHAlienUpgradeCategory inUpgradeCategory, const AvHAlienUpgradeListType& inList, int inUser4)
{
	bool theSuccess = false;
	
	// Count how many upgrades of this category the player already has
	int theNumUpgradesAlreadyHave = AvHGetNumUpgradesInCategory(inUpgradeCategory, inUser4);
	
	// Count how many upgrades of this category in the list
	int theNumUpgradesInCategoryInList = AvHGetNumUpgradesInCategoryInList(inList, inUpgradeCategory);

	// Can only choose upgrade when we have chosen none
	if((theNumUpgradesAlreadyHave == 0) && (theNumUpgradesInCategoryInList > 0))
	{
		theSuccess = true;
	}
	
	return theSuccess;
}



bool AvHGetIsAlien(int inUser3)
{
	bool theIsAlien = false;

	if(inUser3 == AVH_USER3_ALIEN_PLAYER1)
	{
		theIsAlien = true;
	}
	else if(inUser3 == AVH_USER3_ALIEN_PLAYER2)
	{
		theIsAlien = true;
	}
	else if(inUser3 == AVH_USER3_ALIEN_PLAYER3)
	{
		theIsAlien = true;
	}
	else if(inUser3 == AVH_USER3_ALIEN_PLAYER4)
	{
		theIsAlien = true;
	}
	else if(inUser3 == AVH_USER3_ALIEN_PLAYER5)
	{
		theIsAlien = true;
	}
	return theIsAlien;
}

int AvHGetNumUpgradesInCategory(AvHAlienUpgradeCategory inUpgradeCategory, int inUser4)
{
	int theNumUpgrades = 0;
	
	ASSERT(inUpgradeCategory != ALIEN_UPGRADE_CATEGORY_INVALID);
	
	switch(inUpgradeCategory)
	{
	case ALIEN_UPGRADE_CATEGORY_DEFENSE:
		theNumUpgrades = 0;
		theNumUpgrades += (GetHasUpgrade(inUser4, MASK_UPGRADE_1) || GetHasUpgrade(inUser4, MASK_UPGRADE_2) || GetHasUpgrade(inUser4, MASK_UPGRADE_3)) ? 1 : 0;
		if(GetHasUpgrade(inUser4, MASK_UPGRADE_10))
		{
			theNumUpgrades = 2;
		}
		else if(GetHasUpgrade(inUser4, MASK_UPGRADE_11))
		{
			theNumUpgrades = 3;
		}
		break;
//	case ALIEN_UPGRADE_CATEGORY_OFFENSE:
//		theNumUpgrades = 0;
//		theNumUpgrades += (GetHasUpgrade(inUser4, MASK_PLACEHOLDER1) || GetHasUpgrade(inUser4, MASK_PLACEHOLDER2) || GetHasUpgrade(inUser4, MASK_PLACEHOLDER3)) ? 1 : 0;
		// TODO: Add these when neeeded
		//theNumUpgrades += GetHasUpgrade(inUser4, MASK_UPGRADE_10) ? 1 : 0;
		//theNumUpgrades += GetHasUpgrade(inUser4, MASK_UPGRADE_11) ? 1 : 0;
//		break;
	case ALIEN_UPGRADE_CATEGORY_MOVEMENT:
		theNumUpgrades = 0;
		theNumUpgrades += (GetHasUpgrade(inUser4, MASK_UPGRADE_4) || GetHasUpgrade(inUser4, MASK_UPGRADE_5) || GetHasUpgrade(inUser4, MASK_UPGRADE_6)) ? 1 : 0;
		if(GetHasUpgrade(inUser4, MASK_UPGRADE_12))
		{
			theNumUpgrades = 2;
		}
		else if(GetHasUpgrade(inUser4, MASK_UPGRADE_13))
		{
			theNumUpgrades = 3;
		}
		break;
	case ALIEN_UPGRADE_CATEGORY_SENSORY:
		theNumUpgrades = 0;
		theNumUpgrades += (GetHasUpgrade(inUser4, MASK_UPGRADE_7) || GetHasUpgrade(inUser4, MASK_UPGRADE_8) || GetHasUpgrade(inUser4, MASK_UPGRADE_9)) ? 1 : 0;
		if(GetHasUpgrade(inUser4, MASK_UPGRADE_14))
		{
			theNumUpgrades = 2;
		}
		else if(GetHasUpgrade(inUser4, MASK_UPGRADE_15))
		{
			theNumUpgrades = 3;
		}
		break;
	}
	
	return theNumUpgrades;
}

int AvHGetNumUpgradesInCategoryInList(const AvHAlienUpgradeListType& inList, AvHAlienUpgradeCategory inUpgradeCategory)
{
	int theNumUpgradesInCategoryInList = 0;
	
	for(AvHAlienUpgradeListType::const_iterator theIter = inList.begin(); theIter != inList.end(); theIter++)
	{
		if(*theIter == inUpgradeCategory)
		{
			theNumUpgradesInCategoryInList++;
		}
	}
	return theNumUpgradesInCategoryInList;
	
}

void AvHAddHigherLevelUpgrades(const AvHAlienUpgradeListType& inList, int& inUser4)
{
	ASSERT(ALIEN_UPGRADE_CATEGORY_DEFENSE == 1);
	ASSERT(ALIEN_UPGRADE_CATEGORY_SENSORY == (ALIEN_UPGRADE_CATEGORY_MAX_PLUS_ONE - 1));
	
	for(int i = (int)(ALIEN_UPGRADE_CATEGORY_DEFENSE); i < (int)(ALIEN_UPGRADE_CATEGORY_MAX_PLUS_ONE); i++)
	{
		AvHAlienUpgradeCategory theCurrentCategory = (AvHAlienUpgradeCategory)(i);
		int theNumUpgradesInCategory = AvHGetNumUpgradesInCategory(theCurrentCategory, inUser4);
		int theNumUpgradesInCategoryInList = AvHGetNumUpgradesInCategoryInList(inList, theCurrentCategory);
		while(theNumUpgradesInCategoryInList > theNumUpgradesInCategory)
		{
			AvHAddUpgradeInCategory(theCurrentCategory, inUser4);
			theNumUpgradesInCategory += 1;
		}
	}
}

void AvHRemoveIrrelevantUpgrades(AvHAlienUpgradeListType& inList)
{
	// Remove any upgrades that aren't helping
	for(int i = (int)(ALIEN_UPGRADE_CATEGORY_DEFENSE); i < (int)(ALIEN_UPGRADE_CATEGORY_MAX_PLUS_ONE); i++)
	{
		AvHAlienUpgradeCategory theCurrentCategory = (AvHAlienUpgradeCategory)(i);

		int theNumUpgradeCategories = AvHGetNumUpgradesInCategoryInList(inList, theCurrentCategory);
		while(theNumUpgradeCategories > kNumUpgradeLevelsInCategory)
		{
			AvHRemoveUpgradeCategory(theCurrentCategory, inList);
			theNumUpgradeCategories = AvHGetNumUpgradesInCategoryInList(inList, theCurrentCategory);
		}
	}
}

int AvHRemoveExcessUpgrades(const AvHAlienUpgradeListType& inList, int& inUser4)
{
	// Count number of upgrades in each category, removing any in excess
	ASSERT(ALIEN_UPGRADE_CATEGORY_DEFENSE == 1);
	ASSERT(ALIEN_UPGRADE_CATEGORY_SENSORY == (ALIEN_UPGRADE_CATEGORY_MAX_PLUS_ONE - 1));
	
	int theNumberRemoved = 0;
	
	for(int i = (int)(ALIEN_UPGRADE_CATEGORY_DEFENSE); i < (int)(ALIEN_UPGRADE_CATEGORY_MAX_PLUS_ONE); i++)
	{
		AvHAlienUpgradeCategory theCurrentCategory = (AvHAlienUpgradeCategory)(i);
		int theNumUpgradesInCategory = AvHGetNumUpgradesInCategory(theCurrentCategory, inUser4);
		int theNumUpgradesInCategoryInList = AvHGetNumUpgradesInCategoryInList(inList, theCurrentCategory);
		while(theNumUpgradesInCategory > theNumUpgradesInCategoryInList)
		{
			AvHRemoveUpgradeInCategory(theCurrentCategory, inUser4);
			theNumUpgradesInCategory -= 1;
			
			theNumberRemoved++;
		}
	}
	
	return theNumberRemoved;
}

void AvHAddUpgradeInCategory(AvHAlienUpgradeCategory inUpgradeCategory, int& inUser4)
{
	int theNumUpgradesInCategory = AvHGetNumUpgradesInCategory(inUpgradeCategory, inUser4);
	switch(inUpgradeCategory)
	{
	case ALIEN_UPGRADE_CATEGORY_DEFENSE:
		if(theNumUpgradesInCategory == 1)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_10);
			SetUpgradeMask(&inUser4, MASK_UPGRADE_11, false);
		}
		else if(theNumUpgradesInCategory == 2)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_10, false);
			SetUpgradeMask(&inUser4, MASK_UPGRADE_11);
		}
		break;
	case ALIEN_UPGRADE_CATEGORY_MOVEMENT:
		if(theNumUpgradesInCategory == 1)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_12);
			SetUpgradeMask(&inUser4, MASK_UPGRADE_13, false);
		}
		else if(theNumUpgradesInCategory == 2)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_12, false);
			SetUpgradeMask(&inUser4, MASK_UPGRADE_13);
		}
		break;
	case ALIEN_UPGRADE_CATEGORY_SENSORY:
		if(theNumUpgradesInCategory == 1)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_14);
			SetUpgradeMask(&inUser4, MASK_UPGRADE_15, false);
		}
		else if(theNumUpgradesInCategory == 2)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_14, false);
			SetUpgradeMask(&inUser4, MASK_UPGRADE_15);
		}
		break;
	}
}

void AvHRemoveUpgradeInCategory(AvHAlienUpgradeCategory inUpgradeCategory, int& inUser4)
{
	int theNumUpgradesInCategory = AvHGetNumUpgradesInCategory(inUpgradeCategory, inUser4);
	switch(inUpgradeCategory)
	{
	case ALIEN_UPGRADE_CATEGORY_DEFENSE:
		if(theNumUpgradesInCategory == 1)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_1, false);
			SetUpgradeMask(&inUser4, MASK_UPGRADE_2, false);
			SetUpgradeMask(&inUser4, MASK_UPGRADE_3, false);
		}
		else if(theNumUpgradesInCategory == 2)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_10, false);
		}
		else if(theNumUpgradesInCategory == 3)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_11, false);
		}
		break;
//	case ALIEN_UPGRADE_CATEGORY_OFFENSE:
//		if(theNumUpgradesInCategory == 1)
//		{
//			SetUpgradeMask(&inUser4, MASK_PLACEHOLDER1, false);
//			SetUpgradeMask(&inUser4, MASK_PLACEHOLDER2, false);
//			SetUpgradeMask(&inUser4, MASK_PLACEHOLDER3, false);
//		}
		// TODO: Add this when needed
//		else if(theNumUpgradesInCategory == 2)
//		{
//			SetUpgradeMask(&inUser4, MASK_UPGRADE_10, false);
//		}
//		else if(theNumUpgradesInCategory == 3)
//		{
//			SetUpgradeMask(&inUser4, MASK_UPGRADE_11, false);
//		}
		break;
	case ALIEN_UPGRADE_CATEGORY_MOVEMENT:
		if(theNumUpgradesInCategory == 1)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_4, false);
			SetUpgradeMask(&inUser4, MASK_UPGRADE_5, false);
			SetUpgradeMask(&inUser4, MASK_UPGRADE_6, false);
		}
		else if(theNumUpgradesInCategory == 2)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_12, false);
		}
		else if(theNumUpgradesInCategory == 3)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_13, false);
		}
		break;
	case ALIEN_UPGRADE_CATEGORY_SENSORY:
		if(theNumUpgradesInCategory == 1)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_7, false);
			SetUpgradeMask(&inUser4, MASK_UPGRADE_8, false);
			SetUpgradeMask(&inUser4, MASK_UPGRADE_9, false);
		}
		else if(theNumUpgradesInCategory == 2)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_14, false);
		}
		else if(theNumUpgradesInCategory == 3)
		{
			SetUpgradeMask(&inUser4, MASK_UPGRADE_15, false);
		}
		break;
	}
}

bool AvHRemoveUpgradeCategory(AvHAlienUpgradeCategory inUpgradeCategory, AvHAlienUpgradeListType& inList)
{
	bool theSuccess = false;
	
	AvHAlienUpgradeListType::iterator theIter = std::find(inList.begin(), inList.end(), inUpgradeCategory);
	if(theIter != inList.end())
	{
		inList.erase(theIter);
		theSuccess = true;
	}

	return theSuccess;
}

// Changing this?  Make sure to change AvHCommanderModeHandler::ActionButtonActivated also.
bool AvHGetTechSlotEnabled(int inSlot, int inUser4)
{
	bool theSlotIsEnabled = false;

	switch(inSlot)
	{
	case 0:
		theSlotIsEnabled = GetHasUpgrade(inUser4, MASK_UPGRADE_1);
		break;
	case 1:
		theSlotIsEnabled = GetHasUpgrade(inUser4, MASK_UPGRADE_2);
		break;
	case 2:
		theSlotIsEnabled = GetHasUpgrade(inUser4, MASK_UPGRADE_3);
		break;
	case 3:
		theSlotIsEnabled = GetHasUpgrade(inUser4, MASK_UPGRADE_4);
		break;
	case 4:
		theSlotIsEnabled = GetHasUpgrade(inUser4, MASK_UPGRADE_5);
		break;
	case 5:
		theSlotIsEnabled = GetHasUpgrade(inUser4, MASK_UPGRADE_6);
		break;
	case 6:
		theSlotIsEnabled = GetHasUpgrade(inUser4, MASK_UPGRADE_7);
		break;
	case 7:
		theSlotIsEnabled = GetHasUpgrade(inUser4, MASK_UPGRADE_8);
		break;
	}

	return theSlotIsEnabled;
}

// Changing this?  Make sure to change AvHCommanderModeHandler::ActionButtonActivated also.
void AvHSetTechSlotEnabledState(int inSlot, int* inUser4, bool inEnabled)
{
	switch(inSlot)
	{
	case 0:
		SetUpgradeMask(inUser4, MASK_UPGRADE_1, inEnabled);
		break;
	case 1:
		SetUpgradeMask(inUser4, MASK_UPGRADE_2, inEnabled);
		break;
	case 2:
		SetUpgradeMask(inUser4, MASK_UPGRADE_3, inEnabled);
		break;
	case 3:
		SetUpgradeMask(inUser4, MASK_UPGRADE_4, inEnabled);
		break;
	case 4:
		SetUpgradeMask(inUser4, MASK_UPGRADE_5, inEnabled);
		break;
	case 5:
		SetUpgradeMask(inUser4, MASK_UPGRADE_6, inEnabled);
		break;
	case 6:
		SetUpgradeMask(inUser4, MASK_UPGRADE_7, inEnabled);
		break;
	case 7:
		SetUpgradeMask(inUser4, MASK_UPGRADE_8, inEnabled);
		break;
	}
}
