//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile:  $
// $Date: $
//
//-------------------------------------------------------------------------------
// $Log: $
//===============================================================================
#include "util/nowarnings.h"
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/player.h"
#include "dlls/weapons.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHAlienEquipment.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHAlienWeapons.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHServerUtil.h"
#include "util/MathUtil.h"

typedef vector<AvHPlayer*>			AvHPlayerListType;
typedef vector<AvHObservatory*>		AvHObservatoryListType;
typedef vector<AvHScan*>			AvHScanListType;
typedef vector<AvHSensoryChamber*>	AvHSensoryChamberListType;
typedef vector<AvHUmbraCloud*>		AvHUmbraCloudListType;

BaseEntityListType			gBaseEntityList;
AvHPlayerListType			gPlayerList;
AvHPlayerListType			gRelevantPlayerList;
AvHObservatoryListType		gObservatoryList;
AvHScanListType				gScanList;
AvHSensoryChamberListType	gSensoryChamberList;
AvHUmbraCloudListType		gUmbraCloudList;

const float kMovementVisibilityThreshold = 10.0f;

bool AvHSUGetInViewOfEnemy(CBaseEntity* inEntity, int& outSightedStatus)
{
	bool theInViewOfEnemy = false;

	if(inEntity->pev->iuser4 & MASK_TOPDOWN) 
	{
		// Top down players are never visible
	}
	else
	{
		if(GetGameRules()->GetDrawInvisibleEntities())
		{
			outSightedStatus |= MASK_VIS_SIGHTED;
			theInViewOfEnemy = true;
		}
		else if(GetGameRules()->GetIsCheatEnabled(kcDetectAll))
		{
			outSightedStatus |= MASK_VIS_DETECTED;
			theInViewOfEnemy = true;
		}
		else
		{
			AvHPlayer* theInPlayer = dynamic_cast<AvHPlayer*>(inEntity);
			AvHCloakable* theCloakable = dynamic_cast<AvHCloakable*>(inEntity);
			if(!theInPlayer || !theInPlayer->GetIsCloaked())
			{
				if(!theCloakable || (theCloakable->GetOpacity() > 0.0f))
				{
					// Loop through enemy players, check if we are in view of any of them
					for(AvHPlayerListType::iterator thePlayerIter = gPlayerList.begin(); thePlayerIter != gPlayerList.end(); thePlayerIter++)
					{
						if((*thePlayerIter)->GetTeam() != inEntity->pev->team)
						{
							// Commanders can't "see" enemies
							if(((*thePlayerIter)->GetUser3() != AVH_USER3_COMMANDER_PLAYER) && ((*thePlayerIter)->GetIsRelevant()))
							{
								if((*thePlayerIter)->GetIsEntityInSight(inEntity))
								{
									outSightedStatus |= MASK_VIS_SIGHTED;
									theInViewOfEnemy = true;
									break;
								}
							}
    					}
					}
				}

				// Loop through observatories, uncloaking and detecting all enemy players in range
				for(AvHObservatoryListType::iterator theObservatoryIter = gObservatoryList.begin(); theObservatoryIter != gObservatoryList.end(); theObservatoryIter++)
				{
					if((*theObservatoryIter)->pev->team != inEntity->pev->team && !(*theObservatoryIter)->GetIsRecycling() )
					{
						// Check that entity is in range of scan (only check XY distance, for commander's purposes)
						float theDistance = VectorDistance2D((*theObservatoryIter)->pev->origin, inEntity->pev->origin);
						if(theDistance < BALANCE_VAR(kObservatoryXYDetectionRadius))
						{
							outSightedStatus |= MASK_VIS_DETECTED;
							theInViewOfEnemy = true;

							if(theCloakable)
							{
								theCloakable->Uncloak();
							}
							break;
						}
					}
				}
			
				// Loop through all active scans on our team
				for(AvHScanListType::iterator theScanIter = gScanList.begin(); theScanIter != gScanList.end(); theScanIter++)
				{
					if((*theScanIter)->pev->team != inEntity->pev->team)
					{
						// Check that entity is in range of scan
						float theDistance = VectorDistance((*theScanIter)->pev->origin, inEntity->pev->origin);
						if(theDistance < BALANCE_VAR(kScanRadius))
						{
							outSightedStatus |= MASK_VIS_SIGHTED;
							theInViewOfEnemy = true;
							break;
						}
					}
				}
			}
		}
		
		// If not in sight, check for motion-tracking
		if(!theInViewOfEnemy)
		{
			bool theEnemyTeamHasMotionTracking = false;
			AvHTeamNumber teamA = GetGameRules()->GetTeamA()->GetTeamNumber();
			AvHTeamNumber teamB = GetGameRules()->GetTeamB()->GetTeamNumber();
			if((inEntity->pev->team == teamA) || (inEntity->pev->team == teamB))
			{
				AvHTeamNumber theEnemyTeamNumber = (inEntity->pev->team == teamA) ? teamB : teamA;
				AvHTeam* theEnemyTeam = GetGameRules()->GetTeam(theEnemyTeamNumber);
				if(theEnemyTeam)
				{
					if(theEnemyTeam->GetResearchManager().GetTechNodes().GetIsTechResearched(TECH_RESEARCH_MOTIONTRACK) || GetGameRules()->GetIsCombatMode())
					{
						// Motion-tracking doesn't pick up cloaked entities (players)
						bool theIsCloaked = false;
						AvHCloakable* theCloakable = dynamic_cast<AvHCloakable*>(inEntity);
						if(theCloakable && (theCloakable->GetOpacity() < 0.1f))
						{
							theIsCloaked = true;
						}

						float theVelocity = inEntity->pev->velocity.Length();
						
						//ELVEN - WE HAVE TO CHECK FOR EXISTANT OBSERVATORIES BEFORE WE CAN FLAG THIS.
						//voogru: Fixed combat mode problems & slight perfoamance issue (no need to loop thru every obs).
						bool obsExists = false;

						if(!GetGameRules()->GetIsCombatMode())
						{
							FOR_ALL_ENTITIES(kwsObservatory, AvHObservatory*)
								if(theEntity->GetIsBuilt())
								{
									obsExists = true;
									break;
								}
								END_FOR_ALL_ENTITIES(kwsObservatory)
						}
						else
						{
							obsExists = true;
						}

						if((theVelocity > kMovementVisibilityThreshold) && !theIsCloaked && obsExists)
						{
							outSightedStatus |= MASK_VIS_DETECTED;
							theInViewOfEnemy = true;
						}
					}
				}
			}
		}
	}

	return theInViewOfEnemy;
}

bool AvHSUGetInRangeOfFriendlyPrimalScream(CBaseEntity* inEntity)
{
	bool inRangeOfPrimalScream = false;

	int theTeamNumber = inEntity->pev->team;
	if(theTeamNumber)
	{
		// If team is of type alien
		const AvHTeam* theTeam = GetGameRules()->GetTeam(AvHTeamNumber(theTeamNumber));
		if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN) && inEntity->IsAlive())
		{
			// Loop through all players on our team
			for(AvHPlayerListType::iterator theIter = gPlayerList.begin(); theIter != gPlayerList.end(); theIter++)
			{
				AvHPlayer* thePlayer = *theIter;

				// See if any of them are screaming
				if(thePlayer && (thePlayer->pev->team == theTeamNumber) && (thePlayer->GetIsScreaming()))
				{
					// Are they in range of us?
					float theDistance = VectorDistance(inEntity->pev->origin, thePlayer->pev->origin);
					if(theDistance < BALANCE_VAR(kPrimalScreamRange))
					{
						inRangeOfPrimalScream = true;
						break;
					}
				}
			}
		}
	}

	return inRangeOfPrimalScream;
}

bool AvHSUGetInRangeOfFriendlySensoryChamber(CBaseEntity* inEntity)
{
	bool inRangeOfSensoryChamber = false;
	
	int theTeamNumber = inEntity->pev->team;
	if(theTeamNumber)
	{
		// If team is of type alien
		const AvHTeam* theTeam = GetGameRules()->GetTeam(AvHTeamNumber(theTeamNumber));
		if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN) && inEntity->IsAlive())
		{
			// Loop through all SensoryChamber clouds on our team
			for(AvHSensoryChamberListType::const_iterator theIter = gSensoryChamberList.begin(); theIter != gSensoryChamberList.end(); theIter++)
			{
				AvHSensoryChamber* theChamber = *theIter;
				if(theChamber && (theChamber->pev->team == theTeamNumber))
				{
					// Are we in range?
					float theDistance = VectorDistance(inEntity->pev->origin, theChamber->pev->origin);
					if(theDistance < BALANCE_VAR(kSensoryChamberRange))
					{
						AvHBaseBuildable* theBuildable = dynamic_cast<AvHBaseBuildable*>(theChamber);
						if(theBuildable && theBuildable->GetIsBuilt())
						{
							inRangeOfSensoryChamber = true;
							break;
						}
					}
				}
			}
		}
	}
	
	return inRangeOfSensoryChamber;
}

bool AvHSUGetInRangeOfEnemySensoryChamber(CBaseEntity* inEntity)
{
	bool inRangeOfSensoryChamber = false;
	int theTeamNumber = inEntity->pev->team;
	if(theTeamNumber)
	{
		// If team is of type marine
		const AvHTeam* theTeam = GetGameRules()->GetTeam(AvHTeamNumber(theTeamNumber));
		if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_MARINE) && inEntity->IsAlive())
		{
			// Loop through all SensoryChamber clouds on our team
			for(AvHSensoryChamberListType::const_iterator theIter = gSensoryChamberList.begin(); theIter != gSensoryChamberList.end(); theIter++)
			{
				AvHSensoryChamber* theChamber = *theIter;
				if(theChamber && (theChamber->pev->team != theTeamNumber))
				{
					// Are we in range?
					float theDistance = VectorDistance(inEntity->pev->origin, theChamber->pev->origin);
					if(theDistance < BALANCE_VAR(kSensoryChamberRange))
					{
						AvHBaseBuildable* theBuildable = dynamic_cast<AvHBaseBuildable*>(theChamber);
						if(theBuildable && theBuildable->GetIsBuilt())
						{
							inRangeOfSensoryChamber = true;
							break;
						}
					}
				}
			}
		}
	}
	return inRangeOfSensoryChamber;
}

bool AvHSUGetInRangeOfFriendlyUmbra(CBaseEntity* inEntity)
{
	bool inRangeOfUmbra = false;

	int theTeamNumber = inEntity->pev->team;
	if(theTeamNumber)
	{
		// If team is of type alien
		const AvHTeam* theTeam = GetGameRules()->GetTeam(AvHTeamNumber(theTeamNumber));
		if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN) && inEntity->IsAlive())
		{
			// Loop through all umbra clouds on our team
			for(AvHUmbraCloudListType::iterator theIter = gUmbraCloudList.begin(); theIter != gUmbraCloudList.end(); theIter++)
			{
				AvHUmbraCloud* theUmbraCloud = *theIter;
				if(theUmbraCloud && (theUmbraCloud->pev->team == theTeamNumber))
				{
					// Are we in range?
					float theDistance = VectorDistance(inEntity->pev->origin, theUmbraCloud->pev->origin);
					if(theDistance < BALANCE_VAR(kUmbraCloudRadius))
					{
						inRangeOfUmbra = true;
						break;
					}
				}
			}
		}
	}

	return inRangeOfUmbra;
}

void UpdateWorldEntity(CBaseEntity* inBaseEntity)
{
	// Visibility
	inBaseEntity->pev->iuser4 &= ~MASK_VIS_SIGHTED;
	inBaseEntity->pev->iuser4 &= ~MASK_VIS_DETECTED;
	
	if(AvHSUGetIsSubjectToVisibilityRules(inBaseEntity))
	{
		int theSightedStatus = 0;
		if(AvHSUGetInViewOfEnemy(inBaseEntity, theSightedStatus))
		{
			inBaseEntity->pev->iuser4 |= theSightedStatus;
			if(inBaseEntity->pev->classname == MAKE_STRING(kesParticlesCustom))
			{
				int a = 0;
			}
		}
	}
	
	// Don't clear buff flag on marines, as it means catalysts for them and they expire in AvHPlayer::InternalMarineThink
    AvHUser3 theUser3 = AvHUser3(inBaseEntity->pev->iuser3);
    if(theUser3 != AVH_USER3_MARINE_PLAYER)
    {
        inBaseEntity->pev->iuser4 &= ~MASK_BUFFED;
    }
	
    // Primal scream bonuses
	if(AvHSUGetInRangeOfFriendlyPrimalScream(inBaseEntity))
	{
		inBaseEntity->pev->iuser4 |= MASK_BUFFED;
	}
	

	// Deteted by sensory chambers
    if(theUser3 == AVH_USER3_MARINE_PLAYER )
   {
		if(AvHSUGetInRangeOfEnemySensoryChamber(inBaseEntity))
		{
			SetUpgradeMask(&inBaseEntity->pev->iuser4, MASK_SENSORY_NEARBY, true);
		}
		else
		{
			SetUpgradeMask(&inBaseEntity->pev->iuser4, MASK_SENSORY_NEARBY, false);
		}
	}

	// Cloaking near sensory chambers
	AvHCloakable* theCloakable = dynamic_cast<AvHCloakable*>(inBaseEntity);
	if(theCloakable )
	{
		if ( theUser3 != AVH_USER3_MARINE_PLAYER ) 
		{
			if(AvHSUGetInRangeOfFriendlySensoryChamber(inBaseEntity))
			{
				theCloakable->SetSpeeds(0.0f, 0.0f, 0.0f);
				theCloakable->Cloak();
				
				SetUpgradeMask(&inBaseEntity->pev->iuser4, MASK_SENSORY_NEARBY, true);
			}
			else
			{
				// Don't uncloak if we are cloaking via the upgrade
				int theCloakingLevel = AvHGetAlienUpgradeLevel(inBaseEntity->pev->iuser4, MASK_UPGRADE_7);
				if(theCloakingLevel == 0)
				{
					theCloakable->Uncloak();
				}
				
				SetUpgradeMask(&inBaseEntity->pev->iuser4, MASK_SENSORY_NEARBY, false);
			}
		}
		else 
		{
			theCloakable->Uncloak();
		}
	}
	
	// Umbra defense
	inBaseEntity->pev->iuser4 &= ~MASK_UMBRA;
	if(AvHSUGetInRangeOfFriendlyUmbra(inBaseEntity))
	{
		inBaseEntity->pev->iuser4 |= MASK_UMBRA;
	}
	
	// Update tech slots periodically so UI shows what's available
	AvHBaseBuildable* theBaseBuildable = dynamic_cast<AvHBaseBuildable*>(inBaseEntity);
	if(theBaseBuildable)
	{
		theBaseBuildable->WorldUpdate();
	}
}

void AvHGamerules::UpdateWorldEntities()
{
	// Prepare for many calls to AvHSUGetInViewOfEnemy
	ASSERT(gPlayerList.size() == 0);
	ASSERT(gRelevantPlayerList.size() == 0);
	ASSERT(gObservatoryList.size() == 0);
	ASSERT(gScanList.size() == 0);
	ASSERT(gSensoryChamberList.size() == 0);
	ASSERT(gUmbraCloudList.size() == 0);
	ASSERT(gBaseEntityList.size() == 0);
    
    PROFILE_START()
	
	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		gPlayerList.push_back(theEntity);
		if(theEntity->GetIsRelevant(true))
		{
			gRelevantPlayerList.push_back(theEntity);
		}
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
		
	FOR_ALL_ENTITIES(kwsObservatory, AvHObservatory*)
		if(theEntity->GetIsBuilt())
		{
			gObservatoryList.push_back(theEntity);
		}
	END_FOR_ALL_ENTITIES(kwsObservatory)
		
	FOR_ALL_ENTITIES(kwsScan, AvHScan*)
		gScanList.push_back(theEntity);
	END_FOR_ALL_ENTITIES(kwsScan)
		
	FOR_ALL_ENTITIES(kwsSensoryChamber, AvHSensoryChamber*)
		gSensoryChamberList.push_back(theEntity);
	END_FOR_ALL_ENTITIES(kwsSensoryChamber)

	FOR_ALL_ENTITIES(kwsUmbraCloud, AvHUmbraCloud*)
		gUmbraCloudList.push_back(theEntity);
	END_FOR_ALL_ENTITIES(kwsUmbraCloud)

    PROFILE_END(kUpdateWorldEntitiesBuildLists)
    
	//AvHSUPrintDevMessage("FOR_ALL_BASEENTITIES: AvHGamerules::UpdateWorldEntities\n");

    if(GET_RUN_CODE(2048))
    {
		PROFILE_START()
        FOR_ALL_BASEENTITIES()
			UpdateWorldEntity(theBaseEntity);
            gBaseEntityList.push_back(theBaseEntity);
        END_FOR_ALL_BASEENTITIES()
        PROFILE_END(kUpdateWorldEntitiesUpdateWorldEntities)
    }

	// Rebuild this->mTeamAEntityHierarchy and this->mTeamBEntityHierarchy if changed
    PROFILE_START()
	this->mTeamAEntityHierarchy.BuildFromTeam(&this->mTeamA, gBaseEntityList);
	this->mTeamBEntityHierarchy.BuildFromTeam(&this->mTeamB, gBaseEntityList);
    PROFILE_END(kUpdateWorldEntitiesBuildEntityHierarchies)

	// Update blips
    if(GET_RUN_CODE(1024))
    {
        PROFILE_START()
	    for(AvHPlayerListType::iterator theIter = gPlayerList.begin(); theIter != gPlayerList.end(); theIter++)
	    {	
		    // Reset their blips
		    (*theIter)->ClearBlips();
	    }
	    
	    // For all entities in the world
	    for(BaseEntityListType::iterator theBaseIter = gBaseEntityList.begin(); theBaseIter != gBaseEntityList.end(); theBaseIter++)
	    {
		    CBaseEntity* theBaseEntity = *theBaseIter;

		    // If entity has a team	(allow hives so aliens can find hive locations)
            bool theIsHive = (theBaseEntity->pev->iuser3 == AVH_USER3_HIVE);
		    if((theBaseEntity->pev->team != 0) || theIsHive)
		    {
                // Only process players, parasited entities and hives, as they are the only things that ever show up as blips
                int theEntIndex = theBaseEntity->entindex();

                bool theIsParasited = GetHasUpgrade(theBaseEntity->pev->iuser4, MASK_PARASITED);
				bool theIsNearSensory = GetHasUpgrade(theBaseEntity->pev->iuser4, MASK_SENSORY_NEARBY);
                bool theIsBuildable = dynamic_cast<AvHBaseBuildable*>(theBaseEntity);

                if(((theEntIndex > 0) && (theEntIndex <= kMaxPlayers)) || theIsNearSensory || theIsHive || theIsParasited || theIsBuildable)
                {
			        // For all relevant players in list
			        for(AvHPlayerListType::iterator thePlayerIterator = gRelevantPlayerList.begin(); thePlayerIterator != gRelevantPlayerList.end(); thePlayerIterator++)
			        {
				        // Call ProcessEntityBlip
				        (*thePlayerIterator)->ProcessEntityBlip(theBaseEntity);
			        }
                }
		    }
	    }

        PROFILE_END(kUpdateWorldEntitiesUpdateBlips)
    }

	// End after many calls to AvHSUGetInViewOfEnemy
	gPlayerList.clear();
	gRelevantPlayerList.clear();
	gObservatoryList.clear();
	gScanList.clear();
	gSensoryChamberList.clear();
	gUmbraCloudList.clear();
	gBaseEntityList.clear();
}


