//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHBalanceAnalysis.cpp $
// $Date: $
//
//-------------------------------------------------------------------------------
// $Log: $
//===============================================================================
#include "util/nowarnings.h"
#include "mod/AvHBalanceAnalysis.h"
#include "mod/AvHBalance.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHMarineWeaponConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include "math.h"

#ifdef AVH_CLIENT
#include "mod/AvHHud.h"
extern AvHHud gHUD;
#endif

AvHEconomy::AvHEconomy(const AvHTechNodes& inTechNodes, int inInitialResources, int inInitialTowers, bool inMarine, MessageIDListType inTargetTechList)
{
	mTechNodes = inTechNodes;
	mInitialResources = inInitialResources;
	mInitialTowers = inInitialTowers;
	mMarine = inMarine;
	mTargetTech = inTargetTechList;

	mResultTime = mResultTowers = mResultResources = 0;
}

void AvHEconomy::CalculateFastestTime()
{
	int theMinNodeAmount = 0;
	float theMinTime = 10000;

	// 7 nodes is realistic map maximum
	for(int i = 0; i < 7; i++)
	{
		this->Simulate(i);
		float theResultTime = (float)this->GetResultTime();

		if(theResultTime < theMinTime)
		{
			theMinTime = theResultTime;
			theMinNodeAmount = i;
		}
	}

	// Use the minimum results
	this->Simulate(theMinNodeAmount);
}

int	AvHEconomy::GetBuildNodeCost() const
{
	int theCost = 0;

	if(this->mMarine)
	{
		theCost = BALANCE_IVAR(kResourceTowerCost);
	}
	else
	{
		theCost = BALANCE_IVAR(kAlienResourceTowerCost);
	}

	return theCost;
}

int AvHEconomy::GetResultTime() const
{
	return this->mResultTime;
}

int	AvHEconomy::GetResultTowers() const
{
	return this->mResultTowers;
}

int	AvHEconomy::GetResultResources() const
{
	return this->mResultResources;
}

// Assumes nodes build instantly, doesn't account for travel time, assumes there is always an available node to be built
void AvHEconomy::Simulate(int inBuildXNodes)
{
	int theResourceInjectionInterval = BALANCE_IVAR(kFuncResourceInjectionTime);
	int theBuildNodeInterval = BALANCE_IVAR(kBalanceFuncResourceTime);

	// Start simulating
	int theTimeOfLastNode = 0;
	bool theIsDone = false;
	int theNumNodes = this->mInitialTowers;
	float theTimeOfNextResearchComplete = 0;

	// Initialize results
	this->mResultTowers = this->mInitialTowers;
	this->mResultResources = this->mInitialResources;
	this->mResultTime = 0;

	// Use copy of tech nodes
	AvHTechNodes theTechNodes = this->mTechNodes;
	MessageIDListType theTargetTech = this->mTargetTech;

	if(theResourceInjectionInterval > 0)
	{
		// While not done
		while(!theIsDone)
		{
			AvHMessageID theTargetTechID = (*theTargetTech.begin());
		
			// If we have more nodes to build and we have enough resources
			if(inBuildXNodes > 0)
			{
				// Build the node (increment num nodes, decrement resources)
				int theNodeCost = this->GetBuildNodeCost();
				if(this->mResultResources >= theNodeCost)
				{
					if(this->mResultTime > (theTimeOfLastNode + theBuildNodeInterval))
					{
						theTimeOfLastNode = this->mResultTime;
						this->mResultResources -= theNodeCost;
						this->mResultTowers++;
						inBuildXNodes--;
					}
				}
			}
		
			// Find bottleneck technology
			AvHMessageID theBottleNeck = theTechNodes.GetNextMessageNeededFor(theTargetTechID);
		
			if(theTimeOfNextResearchComplete == 0)
			{
				// If we have enough resources to research/buy it
				int theCost = 0;
				float theTime = 0.0f;
				bool theIsResearchable = false;
				if(theTechNodes.GetResearchInfo(theBottleNeck, theIsResearchable, theCost, theTime))
				{
					// Pay for it
					if(this->mResultResources >= theCost)
					{
						// Remember when we'll be done with it
						this->mResultResources -= theCost;
						theTimeOfNextResearchComplete = this->mResultTime + theTime;
					}
				}
				else
				{
					this->mResultResources = -1;
					theIsDone = true;
				}
			}
			// Is our bottleneck technology done?
			else if(this->mResultTime >= theTimeOfNextResearchComplete)
			{
				// Set it researched
				theTechNodes.SetResearchDone(theBottleNeck, true);
		
				// Is this technology our final tech?
				if(theBottleNeck == theTargetTechID)
				{
					MessageIDListType::iterator theFindIter = std::find(theTargetTech.begin(), theTargetTech.end(), theTargetTechID);
					if(theFindIter != theTargetTech.end())
					{
						theTargetTech.erase(theFindIter);
					}
		
				}
		
				if(theTargetTech.size() == 0)
				{
					// We're done
					theIsDone = true;
				}
				else
				{
					theTimeOfNextResearchComplete = 0;
				}
			}
		
			// Add resources from current towers to our resources
			this->mResultResources += this->mResultTowers*BALANCE_IVAR(kFuncResourceInjectionAmount);
			
			// Increment current time by interval
			this->mResultTime += theResourceInjectionInterval;
		}
	}
}

void GetSimulatedAnalysisFor(MessageIDListType inMessageIDList, bool inMarine, StringList& outList, string inNote)
{
	outList.push_back(inNote);

	int theStartingResources = inMarine ? BALANCE_IVAR(kNumInitialMarinePoints) : BALANCE_IVAR(kNumInitialAlienPoints);

	// Get tech nodes
	const AvHTechNodes& theTechNodes = gHUD.GetTechNodes();
	
	AvHEconomy theEconomy(theTechNodes, theStartingResources, 1, inMarine, inMessageIDList);
	
	theEconomy.CalculateFastestTime();

	// Add message to log
	int theResultTime = theEconomy.GetResultTime();
	char theTimeString[128];
	sprintf(theTimeString, "%d:%.2d", theResultTime/60, theResultTime%60);

	int theNodesBuilt = theEconomy.GetResultTowers() - 1;
	int theResultResources = theEconomy.GetResultResources();
	string theLogMessage = "  " + MakeStringFromInt(theNodesBuilt) + " nodes built -> Time: " + string(theTimeString) + " Resources: " + MakeStringFromInt(theResultResources);
	outList.push_back(theLogMessage);
}

void GetSimulatedAnalysisFor(AvHMessageID inMessageID, bool inMarine, StringList& outList, string inNote)
{
	MessageIDListType theMessageIDList;
	theMessageIDList.push_back(inMessageID);
	GetSimulatedAnalysisFor(theMessageIDList, inMarine, outList, inNote);
}


//void GetHiveAnalysis(StringList& outList)
//{
//	// Assume gorges are all building res towers
//
//	// Find time that one gorge has enough to build hive
//
//	// Add hive build time
//}


int SimulateDamage(AvHUser3 inUser3, int inUser4, float inDamage, int inNumHives)
{
	int theNumBullets = 0;

	// Initialize health and armor
	float theArmorLevel = (float)(AvHPlayerUpgrade::GetMaxArmorLevel(inUser4, inUser3));
	float theHealth = (float)(AvHPlayerUpgrade::GetMaxHealth(inUser4, inUser3));
	bool theDone = false;

	// While lifeform is still alive
	if(inDamage > 0)
	{
		while(!theDone)
		{
			// Simulate bullet hit
			float theHealthToRemove = AvHPlayerUpgrade::CalculateDamageLessArmor(inUser3, inUser4, inDamage, theArmorLevel, NS_DMG_NORMAL, inNumHives);
		
			theHealth -= theHealthToRemove;
		
			// Increment number bullets
			theNumBullets++;

			// We're dead when our health cast to an int is 0 (for HL reasons, see CBaseEntity::TakeDamage for more info)
			if(((int)theHealth) <= 0)
			{
				theDone = true;
			}
		}
	}
	else
	{
		theNumBullets = -1;
	}

	return theNumBullets;
}

void SimulateWeaponVsLifeform(float inDamage, string& inPrependString, string& inLifeformName, AvHUser3 inLifeform, StringList& outList)
{
	for(int theNumHives = 1; theNumHives <= 3; theNumHives++)
	//int theNumHives = 1;
	{
		for(int i = 0; i < 4; i++)
		{
			float theDamage = inDamage;

			switch(i)
			{
			case 1:
				theDamage *= (1.0f + (float)BALANCE_FVAR(kWeaponDamageLevelOne));
				break;
			case 2:
				theDamage *= (1.0f + (float)BALANCE_FVAR(kWeaponDamageLevelTwo));
				break;
			case 3:
				theDamage *= (1.0f + (float)BALANCE_FVAR(kWeaponDamageLevelThree));
				break;
			}
			
			string theResult = string(inPrependString + " L" + MakeStringFromInt(i) + " (" + MakeStringFromFloat(theDamage) + ") vs. " + MakeStringFromInt(theNumHives) + "-hive " + inLifeformName + ": ");
			theResult += MakeStringFromInt(SimulateDamage(inLifeform, MASK_NONE, theDamage, theNumHives));
			theResult += "/" + MakeStringFromInt(SimulateDamage(inLifeform, MASK_UPGRADE_1, theDamage, theNumHives));
			theResult += "/" + MakeStringFromInt(SimulateDamage(inLifeform, MASK_UPGRADE_1 | MASK_UPGRADE_10, theDamage, theNumHives));
			theResult += "/" + MakeStringFromInt(SimulateDamage(inLifeform, MASK_UPGRADE_1 | MASK_UPGRADE_11, theDamage, theNumHives));
			
			outList.push_back(theResult);
		}
	}
}

void SimulateWeaponVsLifeforms(float inDamage, string& inPrependString, StringList& outList)
{
	SimulateWeaponVsLifeform(inDamage, inPrependString, string("Skulk"), AVH_USER3_ALIEN_PLAYER1, outList);
	SimulateWeaponVsLifeform(inDamage, inPrependString, string("Gorge"), AVH_USER3_ALIEN_PLAYER2, outList);
	SimulateWeaponVsLifeform(inDamage, inPrependString, string("Lerk"), AVH_USER3_ALIEN_PLAYER3, outList);
	SimulateWeaponVsLifeform(inDamage, inPrependString, string("Fade"), AVH_USER3_ALIEN_PLAYER4, outList);
	SimulateWeaponVsLifeform(inDamage, inPrependString, string("Onos"), AVH_USER3_ALIEN_PLAYER5, outList);
	SimulateWeaponVsLifeform(inDamage, inPrependString, string("Marine"), AVH_USER3_MARINE_PLAYER, outList);
}

void AvHBAComputeAnalysis(StringList& outList)
{
	// Num bile bombs to kill a resource tower
	int theBileBombDamage = BALANCE_IVAR(kBileBombDamage);
	int theResourceTowerHealth = BALANCE_IVAR(kResourceTowerHealth);
	int theNumShots = (int)(ceil((float)theResourceTowerHealth/theBileBombDamage));
	outList.push_back("# bile bombs to kill resource tower: " + MakeStringFromInt(theNumShots));

	// Bite vs. resource tower
	int theBiteDamage = BALANCE_IVAR(kBiteDamage);
	theNumShots = (int)(ceil((float)theResourceTowerHealth/theBiteDamage));
	float theTime = theNumShots*BALANCE_FVAR(kBiteROF);
	outList.push_back("# bites to kill resource tower: " + MakeStringFromInt(theNumShots) + " (time: " + MakeStringFromFloat(theTime) + ")");

	// Shotgun vs. hive
	int theSGDamage = BALANCE_IVAR(kSGDamage);
	int theNumSGPellets = BALANCE_IVAR(kSGBulletsPerShot);
	int theHiveHealth = BALANCE_IVAR(kHiveHealth);
	theNumShots = (int)(ceil((float)theHiveHealth/((float)theSGDamage*theNumSGPellets)));
	outList.push_back("# shotgun blasts to kill hive: " + MakeStringFromInt(theNumShots));

	// Grenades vs. alien resource tower (blast damage)
	int theGGDamage = 2*BALANCE_IVAR(kGrenadeDamage);
	int theAlienTowerHealth = BALANCE_IVAR(kAlienResourceTowerHealth);
	theNumShots = (int)(ceil((float)theAlienTowerHealth/theGGDamage));
	outList.push_back("# grenades to kill alien resource tower: " + MakeStringFromInt(theNumShots));

	// Machine gun vs. alien res tower
	int theMGDamage = BALANCE_IVAR(kMGDamage);
	theNumShots = (int)(ceil((float)theAlienTowerHealth/theMGDamage));
	outList.push_back("# mg bullets to kill alien resource tower: " + MakeStringFromInt(theNumShots));

	GetSimulatedAnalysisFor(BUILD_HMG, true, outList, "First HMG:");
	GetSimulatedAnalysisFor(BUILD_GRENADE_GUN, true, outList, "First GL:");
	GetSimulatedAnalysisFor(BUILD_JETPACK, true, outList, "First jetpack:");
	GetSimulatedAnalysisFor(BUILD_HEAVY, true, outList, "First heavy armor:");
	GetSimulatedAnalysisFor(BUILD_SIEGE, true, outList, "First ASC:");
	GetSimulatedAnalysisFor(RESEARCH_MOTIONTRACK, true, outList, "Motion-tracking:");

	MessageIDListType theTech;
	theTech.push_back(BUILD_PHASEGATE);
	theTech.push_back(BUILD_PHASEGATE);
	GetSimulatedAnalysisFor(theTech, true, outList, "Two phase gates:");
	theTech.clear();

	theTech.push_back(RESEARCH_ARMOR_THREE);
	theTech.push_back(RESEARCH_WEAPONS_THREE);
	GetSimulatedAnalysisFor(theTech, true, outList, "Level 3 armor + weapons:");
	theTech.clear();

	for(int i = 0; i < BALANCE_IVAR(kBalanceAverageTeamSize); i++)
	{
		theTech.push_back(BUILD_SHOTGUN);
	}
	GetSimulatedAnalysisFor(theTech, true, outList, "Full squad of shotguns:");
	theTech.clear();

	for(i = 0; i < BALANCE_IVAR(kBalanceAverageTeamSize); i++)
	{
		theTech.push_back(BUILD_HMG);
	}
	GetSimulatedAnalysisFor(theTech, true, outList, "Full squad of HMGs:");
	theTech.clear();
	
	//GetSimulatedAnalysisFor(ALIEN_BUILD_HIVE, false, outList, "Second hive:");
	//GetSimulatedAnalysisFor(ALIEN_LIFEFORM_FIVE, false, outList, "First Onos:");

//	GetHiveAnalysis(outList);
//
//	GetMGSkulkAnalysis(outList);

	// HMG vs. hive
	SimulateWeaponVsLifeforms((float)BALANCE_IVAR(kMGDamage), string("mg"), outList);
	SimulateWeaponVsLifeforms((float)BALANCE_IVAR(kHGDamage), string("pistol"), outList);
	SimulateWeaponVsLifeforms((float)BALANCE_IVAR(kSGDamage), string("shotty"), outList);
	SimulateWeaponVsLifeforms((float)BALANCE_IVAR(kHMGDamage), string("hmg"), outList);
	SimulateWeaponVsLifeforms((float)BALANCE_IVAR(kMineDamage), string("mine"), outList);
	SimulateWeaponVsLifeforms((float)BALANCE_IVAR(kGrenadeDamage), string("gl"), outList);
	SimulateWeaponVsLifeforms((float)BALANCE_IVAR(kBiteDamage), string("bite"), outList);

//	outList.push_back("MG vs. Skulk: 1/2/3/4");
//	outList.push_back("MG vs. Skulk (cara 1): 2/3/4/5");
//	outList.push_back("MG vs. Skulk (cara 2): 3/4/5/6");

	outList.push_back("");
}