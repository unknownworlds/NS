#include "types.h"
#include "mod/AvHPlayer.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHSharedUtil.h"

extern int gLevelUpEventID;

bool AvHPlayer::GiveCombatModeUpgrade(AvHMessageID inMessageID, bool inInstantaneous)
{
	bool theSuccess = false;
	bool theEffectivePlayerClassChanged = false;	

	// Process every upgrade here
	AvHUser3 theUser3 = (AvHUser3)this->pev->iuser3;
	int thePlayerLevel = this->GetExperienceLevel();
	float theHealthPercentage = this->pev->health/AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, theUser3, thePlayerLevel);
	float theArmorPercentage = this->pev->armorvalue/AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, theUser3);
	bool thePreserveHealthArmorPercentage = true;
    CBasePlayerItem* theCreatedItem = NULL;

	// Marine upgrades
	if(this->GetIsMarine())
	{
		Vector thePlayerMinSize, thePlayerMaxSize;
		this->GetSize(thePlayerMinSize, thePlayerMaxSize);

		Vector thePlayersFeet = this->pev->origin;
		thePlayersFeet.z += thePlayerMinSize.z;

		switch(inMessageID)
		{
		case BUILD_SHOTGUN:
		case BUILD_GRENADE_GUN:
		case BUILD_HMG:
		case BUILD_WELDER: 
		case BUILD_MINES:
		case BUILD_JETPACK:
		case BUILD_HEAVY:
			//: spawn the weapon in the middle of nowhere to prevent anyone else from getting it.
			theCreatedItem = dynamic_cast<CBasePlayerItem*>(AvHSUBuildTechForPlayer(inMessageID, Vector(9999,9999,9999), this));
			
			 ASSERT(theCreatedItem);

			if((inMessageID == BUILD_JETPACK) || (inMessageID == BUILD_HEAVY) || (inMessageID == BUILD_HMG) || 
				(inMessageID == BUILD_SHOTGUN) || (inMessageID == BUILD_GRENADE_GUN))
			{
				theEffectivePlayerClassChanged = true;
			}
           
			theSuccess = true; 
			break;

		case BUILD_SCAN:
			AvHSUBuildTechForPlayer(inMessageID, thePlayersFeet, this);
			theSuccess = true;
			break;

//		case RESEARCH_DISTRESSBEACON:
//			AvHSUResearchStarted(this, inMessageID);
//			AvHSUResearchComplete(this, inMessageID);
//			theSuccess = true;
//			break;

		// Give upgrades
		case RESEARCH_ARMOR_ONE:
			this->GiveTeamUpgrade(RESEARCH_ARMOR_ONE);
			theSuccess = true;
			break;

		case RESEARCH_ARMOR_TWO:
			this->GiveTeamUpgrade(RESEARCH_ARMOR_ONE);
			this->GiveTeamUpgrade(RESEARCH_ARMOR_TWO);
			theSuccess = true;
			break;

		case RESEARCH_ARMOR_THREE:
			this->GiveTeamUpgrade(RESEARCH_ARMOR_ONE);
			this->GiveTeamUpgrade(RESEARCH_ARMOR_TWO);
			this->GiveTeamUpgrade(RESEARCH_ARMOR_THREE);
			theSuccess = true;
			break;
		
		case RESEARCH_WEAPONS_ONE:
			this->GiveTeamUpgrade(RESEARCH_WEAPONS_ONE);
			theSuccess = true;
			break;

		case RESEARCH_WEAPONS_TWO:
			this->GiveTeamUpgrade(RESEARCH_WEAPONS_ONE);
			this->GiveTeamUpgrade(RESEARCH_WEAPONS_TWO);
			theSuccess = true;
			break;

		case RESEARCH_WEAPONS_THREE:
			this->GiveTeamUpgrade(RESEARCH_WEAPONS_ONE);
			this->GiveTeamUpgrade(RESEARCH_WEAPONS_TWO);
			this->GiveTeamUpgrade(RESEARCH_WEAPONS_THREE);
			theSuccess = true;
			break;

        case RESEARCH_GRENADES:
            this->GiveNamedItem(kwsGrenade);
            // NOTE: Fall through below

		case RESEARCH_MOTIONTRACK:
			this->GiveTeamUpgrade(inMessageID);
			theSuccess = true;
			break;

		//case BUILD_AMMO:
		//case BUILD_HEALTH:
		case BUILD_RESUPPLY:
			// Find all friendly players nearby (right now this just resupplies current player)
			// Give them health and ammo, equal to # of current level
			AvHSUResupplyFriendliesInRange(1, this);
			thePreserveHealthArmorPercentage = false;
			theSuccess = true;

			// Add new tech node to allow us to buy it again
			//AvHTechNode theTechNode(BUILD_RESUPPLY, TECH_FOUR_LEVEL_TWO, TECH_FOUR_LEVEL_TWO, TECH_NULL, 0, 0, true);
			//this->mCombatNodes.AddTechNode(theTechNode);
			break;

        case BUILD_CAT:
            // Don't give out cat-packs every time
            AvHCatalyst::GiveCatalyst(this);
            theSuccess = true;
            break;
		}
	}
	else if(this->GetIsAlien())
	{
        theSuccess = this->ExecuteAlienMorphMessage(inMessageID, inInstantaneous);
        thePreserveHealthArmorPercentage = false;
	}

	if(thePreserveHealthArmorPercentage)
	{
		this->pev->health = theHealthPercentage*AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, theUser3, thePlayerLevel);
		this->pev->armorvalue = theArmorPercentage*AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, theUser3);
	}

	if(theEffectivePlayerClassChanged)
	{
		this->EffectivePlayerClassChanged();
	}

	return theSuccess;
}

float AvHPlayer::GetExperience() const
{
	return this->mExperience;
}

int AvHPlayer::GetExperienceLevelsSpent() const
{
    return this->mExperienceLevelsSpent;
}

void AvHPlayer::SetExperienceLevelsSpent(int inSpentLevels)
{
    this->mExperienceLevelsSpent = inSpentLevels;
}

void AvHPlayer::AwardExperienceForObjective(float inHealthChange, AvHMessageID inMessageID)
{
    bool theAwardExperience = false;

    if(GetGameRules()->GetIsCombatMode())
    {
        switch(inMessageID)
        {
        case ALIEN_BUILD_HIVE:
        case BUILD_COMMANDSTATION:
            theAwardExperience = true;
            break;
        }
    }

    if(theAwardExperience)
    {
        int theMaxHealth = GetGameRules()->GetBaseHealthForMessageID(inMessageID);
        float thePercentageOfHealth = inHealthChange/theMaxHealth;
        int theCombatObjectiveExperienceScalar = BALANCE_VAR(kCombatObjectiveExperienceScalar);
        float theExperienceGained = thePercentageOfHealth*theCombatObjectiveExperienceScalar;
        this->SetExperience(this->GetExperience() + theExperienceGained);
    }
}

void AvHPlayer::SetExperience(float inExperience)
{
	if(GetGameRules()->GetIsCombatMode())
	{
		int theCurrentLevel = AvHPlayerUpgrade::GetPlayerLevel(this->GetExperience());
		
		this->mExperience = inExperience;

		// Update server player data in case we get disconnected
		AvHTeam* theTeam = this->GetTeamPointer();
		if(theTeam)
		{
			AvHServerPlayerData* theServerPlayerData = this->GetServerPlayerData();
			if(theServerPlayerData)
			{
				theServerPlayerData->SetExperience(this->mExperience);
			}
		}
		
		int theNewLevel = AvHPlayerUpgrade::GetPlayerLevel(this->GetExperience());
		
		if(theCurrentLevel != theNewLevel)
		{
			int theIsMarine = this->GetIsMarine();
			PLAYBACK_EVENT_FULL(0, this->edict(), gLevelUpEventID, 0, this->pev->origin, (float *)&g_vecZero, 0.0, 0.0, theIsMarine, 0, 0, 0 );
		
			this->EffectivePlayerClassChanged();

			// Give player health and armor back on level-up, to allow more soloing, heroics, and reduce dependence on hives/resupply
			AvHUser3 theUser3 = AvHUser3(this->pev->iuser3);
            float theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, theUser3, theCurrentLevel);
			float theHealthPercentage = this->pev->health/theMaxHealth;
			
            float theLevelUpHealthPercentage = BALANCE_VAR(kCombatLevelupHealthIncreasePercent)/100.0f;
            theHealthPercentage = min(theHealthPercentage + theLevelUpHealthPercentage, 1.0f);
			this->pev->health = theHealthPercentage*theMaxHealth;

            float theMaxArmor = AvHPlayerUpgrade::GetMaxArmorLevel(this->pev->iuser4, theUser3);
            float theArmorPercentage = this->pev->armorvalue/theMaxArmor;

            float theLevelUpArmorPercentage = BALANCE_VAR(kCombatLevelupArmorIncreasePercent)/100.0f;
            theArmorPercentage = min(theArmorPercentage + theLevelUpArmorPercentage, 1.0f);
			this->pev->armorvalue = theArmorPercentage*theMaxArmor;
			
			// Unlock tiers as player levels up
	//		if(theNewLevel >= 4)
	//		{
	//			this->mCombatNodes.SetResearchDone(COMBAT_TIER2_UNLOCK);
	//		}
	//		if(theNewLevel >= 7)
	//		{
	//			this->mCombatNodes.SetResearchDone(COMBAT_TIER3_UNLOCK);
	//		}
		}
	}
}

int	AvHPlayer::GetExperienceLevel() const
{
	int theLevel = 1;
	
	if(GetGameRules()->GetIsCombatMode())
	{
		theLevel = AvHPlayerUpgrade::GetPlayerLevel(this->GetExperience());
	}
	
	return theLevel;
}

AvHTechTree& AvHPlayer::GetCombatNodes()
{
	return this->mCombatNodes;
}

MessageIDListType& AvHPlayer::GetPurchasedCombatUpgrades()
{
	return this->mPurchasedCombatUpgrades;
}

void AvHPlayer::SetCombatNodes(const AvHTechTree& inTechNodes)
{
	this->mCombatNodes = inTechNodes;
}

void AvHPlayer::GiveCombatUpgradesOnSpawn()
{
	// Save off previously-spent upgrades and respend them
	MessageIDListType theUpgrades = this->mGiveCombatUpgrades;

	// Need to run through these in order
	for(MessageIDListType::iterator theIter = theUpgrades.begin(); theIter != theUpgrades.end(); theIter++)
	{
		AvHMessageID theCurrentCombatUpgrade = *theIter;
		if(theCurrentCombatUpgrade != MESSAGE_NULL)
		{
            this->GiveCombatModeUpgrade(theCurrentCombatUpgrade, true);
		}
	}
}

void AvHPlayer::PurchaseCombatUpgrade(AvHMessageID inMessageID)
{
	this->mPurchasedCombatUpgrades.push_back(inMessageID);
	//this->mExperienceLevelsSpent++;

    // Remove any upgrades that this prempts
    bool theOneShot = false;
    switch(inMessageID)
    {
    case BUILD_AMMO:
    case BUILD_HEALTH:
    case BUILD_RESUPPLY:
    case BUILD_CAT:
    case BUILD_SCAN:
    //case BUILD_MINES:	
    //case RESEARCH_DISTRESSBEACON:
    	theOneShot = true;
    	break;
    }
    
    // Don't add it as a permanent upgrade to get every time we spawn
    if(!theOneShot)
    {
        this->mGiveCombatUpgrades.push_back(inMessageID);
    }

    this->RemoveCombatUpgradesPremptedBy(inMessageID);
}

void AvHPlayer::RemoveCombatUpgrade(AvHMessageID inMessageID)
{
    MessageIDListType::iterator theIter = std::find(this->mGiveCombatUpgrades.begin(), this->mGiveCombatUpgrades.end(), inMessageID);
    if(theIter != this->mGiveCombatUpgrades.end())
    {
        this->mGiveCombatUpgrades.erase(theIter);

        // Take away the upgrade
        this->GiveTeamUpgrade(inMessageID, false);
    }
}

void AvHPlayer::RemoveCombatUpgradesPremptedBy(AvHMessageID inMessageID)
{
    switch(inMessageID)
    {
    case BUILD_JETPACK:
        this->RemoveCombatUpgrade(BUILD_HEAVY);
        break;
    case BUILD_HEAVY:
        this->RemoveCombatUpgrade(BUILD_JETPACK);
        break;
    case BUILD_HMG:
        this->RemoveCombatUpgrade(BUILD_SHOTGUN);
        this->RemoveCombatUpgrade(BUILD_GRENADE_GUN);
        break;
    case BUILD_GRENADE_GUN:
        this->RemoveCombatUpgrade(BUILD_SHOTGUN);
        this->RemoveCombatUpgrade(BUILD_HMG);
        break;
    case ALIEN_LIFEFORM_TWO:
        this->RemoveCombatUpgrade(ALIEN_LIFEFORM_ONE);
        break;
    case ALIEN_LIFEFORM_THREE:
        this->RemoveCombatUpgrade(ALIEN_LIFEFORM_TWO);
        break;
    case ALIEN_LIFEFORM_FOUR:
        this->RemoveCombatUpgrade(ALIEN_LIFEFORM_THREE);
        break;
    case ALIEN_LIFEFORM_FIVE:
        this->RemoveCombatUpgrade(ALIEN_LIFEFORM_FOUR);
        break;

    //case BUILD_WELDER
    //    this->RemoveCombatUpgrade(BUILD_MINES);
    //    break;
    //case BUILD_MINES:
    //    this->RemoveCombatUpgrade(BUILD_WELDER);
    //    break;
    }
}

void AvHPlayer::ProcessCombatDeath()
{
	// Unspend lifeform (return to skulk, but get the levels spent on lifeform back)
	if(this->GetIsAlien())
	{
		AvHMessageID theLifeformID = MESSAGE_NULL;
		AvHUser3 theUser3 = this->GetUser3(false);
		if(theUser3 == AVH_USER3_ALIEN_EMBRYO)
		{
			// Use the last lifeform we were, unless we're evolving into a new one (assumes only one lifeform change per life)
			theUser3 = this->GetPreviousUser3(false);
			switch(this->mEvolution)
			{
			case ALIEN_LIFEFORM_TWO:
				theUser3 = AVH_USER3_ALIEN_PLAYER2;
				break;
			case ALIEN_LIFEFORM_THREE:
				theUser3 = AVH_USER3_ALIEN_PLAYER3;
				break;
			case ALIEN_LIFEFORM_FOUR:
				theUser3 = AVH_USER3_ALIEN_PLAYER4;
				break;
			case ALIEN_LIFEFORM_FIVE:
				theUser3 = AVH_USER3_ALIEN_PLAYER5;
				break;
			}
		}

		AvHSHUUser3ToMessageID(theUser3, theLifeformID);
		ASSERT(theLifeformID != MESSAGE_NULL);

		int theLifeformCost = GetGameRules()->GetCostForMessageID(theLifeformID);
		this->SetExperienceLevelsSpent(this->GetExperienceLevelsSpent() - theLifeformCost);

		this->RemoveCombatUpgrade(theLifeformID);

		// Make all lifeforms chooseable again
		this->SetLifeformCombatNodesAvailable(true);
	}
}

void AvHPlayer::SetLifeformCombatNodesAvailable(bool inAvailable)
{
	MessageIDListType theLifeformList;
	theLifeformList.push_back(ALIEN_LIFEFORM_ONE);
	theLifeformList.push_back(ALIEN_LIFEFORM_TWO);
	theLifeformList.push_back(ALIEN_LIFEFORM_THREE);
	theLifeformList.push_back(ALIEN_LIFEFORM_FOUR);
	theLifeformList.push_back(ALIEN_LIFEFORM_FIVE);

	for(MessageIDListType::iterator theIter = theLifeformList.begin(); theIter != theLifeformList.end(); theIter++)
	{
		AvHMessageID theLifeformID = *theIter;
		this->mCombatNodes.SetIsResearchable(theLifeformID, inAvailable);
		this->mCombatNodes.SetResearchDone(theLifeformID, !inAvailable);
	}
}

bool AvHPlayer::ExecuteCombatMessage(AvHMessageID inMessageID, bool& outIsAvailable, bool inForce)
{
	bool theMessageExecuted = false;

    // Only explicitly deny messages in the tech tree, let the others fall back to NS-mode handling
    if(this->mCombatNodes.GetIsMessageInTechTree(inMessageID))
    {
	    if(this->mCombatNodes.GetIsMessageAvailable(inMessageID) || inForce)
	    {
		    if(this->GiveCombatModeUpgrade(inMessageID))
		    {
			    theMessageExecuted = true;
		    }
	    }
	    else
	    {
		    // Explicitly deny
		    outIsAvailable = false;
	    }
    }

	return theMessageExecuted;
}

bool AvHPlayer::GetHasCombatModeUpgrade(AvHMessageID inMessageID) const
{
	bool theHasUpgrade = false;

	MessageIDListType::const_iterator theIter = std::find(this->mPurchasedCombatUpgrades.begin(), this->mPurchasedCombatUpgrades.end(), inMessageID);
	if(theIter != this->mPurchasedCombatUpgrades.end())
	{
		theHasUpgrade = true;
	}

	return theHasUpgrade;
}

void AvHPlayer::InternalCombatThink()
{
	// Only update every so often
	if(GetGameRules()->GetIsCombatMode())
	{
        // Save our data in case we get kicked off
        AvHServerPlayerData* theServerPlayerData = this->GetServerPlayerData();
        if(theServerPlayerData)
        {
            theServerPlayerData->SetCombatNodes(this->mCombatNodes);
            theServerPlayerData->SetPurchasedCombatUpgrades(this->mPurchasedCombatUpgrades);
            theServerPlayerData->SetExperienceLevelsSpent(this->mExperienceLevelsSpent);
        }

        // If it's time for an update
        float theCurrentTime = gpGlobals->time;
        const float theCombatThinkInterval = BALANCE_VAR(kCombatThinkInterval);
        
		// Give support from a fake commander
		if(this->GetIsMarine() && this->GetIsRelevant() && !this->GetIsBeingDigested())
		{
			if(this->mTimeOfLastCombatThink == 0 || (theCurrentTime > (this->mTimeOfLastCombatThink + theCombatThinkInterval)))
			{
				// Only allow one upgrade per think
				bool theSuccess = false;

				// Does player have resupply upgrade?
				if(this->GetHasCombatModeUpgrade(BUILD_RESUPPLY))
				{
					// Do they need it?
					float theHealthPercentage = this->pev->health/AvHPlayerUpgrade::GetMaxHealth(this->pev->iuser4, (AvHUser3)this->pev->iuser3, this->GetExperienceLevel());
					// float theAmmoPercentage = this->GetCurrentWeaponAmmoPercentage(); // changed to fix #542
					bool theAmmoResupply = this->GetShouldResupplyAmmo(); 

					if((theHealthPercentage < BALANCE_VAR(kCombatResupplyHealthPercentage)) || theAmmoResupply) //(theAmmoPercentage < BALANCE_VAR(kCombatResupplyAmmoPercentage)))
					{
						// Resupply player
						this->GiveCombatModeUpgrade(BUILD_RESUPPLY);
						theSuccess = true;
					}
				}

                // Does player have resupply upgrade?
                if(this->GetHasCombatModeUpgrade(BUILD_CAT))
                {
                    // Catalyst player after he gets a kill
                    if(this->pev->frags > this->mSavedCombatFrags)
                    {
                        //if(RANDOM_LONG(0, 1) == 0)
                        //{
                            AvHCatalyst::GiveCatalyst(this);
                            theSuccess = true;
                        //}
                    }
                }
                this->mSavedCombatFrags = this->pev->frags;

				// Does player have scan upgrade?
				if(!theSuccess && this->GetHasCombatModeUpgrade(BUILD_SCAN) && !this->GetIsBeingDigested())
				{
					// Needed if there is a cloaked enemy nearby
					bool theCloakedEnemyNearby = false;

					// Look in sphere for cloakables
					CBaseEntity* theSphereEntity = NULL;
					while ((theSphereEntity = UTIL_FindEntityInSphere(theSphereEntity, this->pev->origin, BALANCE_VAR(kScanRadius))) != NULL)
					{
						if(!AvHSUGetIsExternalClassName(STRING(theSphereEntity->pev->classname)))
						{
							AvHCloakable* theCloakable = dynamic_cast<AvHCloakable*>(theSphereEntity);
							if(theCloakable && theCloakable->GetIsPartiallyCloaked() && (theSphereEntity->pev->team != this->pev->team))
							{
								//if(this->GetIsEntityInSight(theSphereEntity))
								//{
									theCloakedEnemyNearby = true;
									break;
								//}
							}
						}
					}

					// Lucky enough to receive?
					if(theCloakedEnemyNearby /*&& (RANDOM_LONG(0, 1) == 0)*/)
					{
						// Scan
						this->GiveCombatModeUpgrade(BUILD_SCAN);
						theSuccess = true;
					}
				}

//				// Does player have distress beacon?
//				if(!theSuccess && this->GetHasCombatModeUpgrade(RESEARCH_DISTRESSBEACON))
//				{
//					// Needed?
//					int theNumTeammatesWaitingToSpawn = 0;
//					FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*);
//					if(theEntity->GetTeam() == this->GetTeam())
//					{
//						if(theEntity->GetPlayMode() == PLAYMODE_AWAITINGREINFORCEMENT)
//						{
//							theNumTeammatesWaitingToSpawn++;
//						}
//					}
//					END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
//
//					// Lucky enough?
//					int theNumPlayersOnTeam = 0;
//					AvHTeam* theTeamPointer = this->GetTeamPointer();
//					if(theTeamPointer)
//					{
//						theNumPlayersOnTeam = theTeamPointer->GetPlayerCount();
//					}
//
//					float theDeadPercentage = (float)theNumTeammatesWaitingToSpawn/theNumPlayersOnTeam;
//					if(theDeadPercentage > BALANCE_VAR(kCombatDistressBeaconDeadPercentage))
//					{
//						// Distress!
//						this->GiveCombatModeUpgrade(RESEARCH_DISTRESSBEACON);
//						theSuccess = true;
//					}
//				}
				
				// Update time
				this->mTimeOfLastCombatThink = theCurrentTime;
			}
		}
        else if(this->GetIsAlien() && this->GetIsRelevant())
        {
            // Give aliens experience slowly over time
            if(this->mTimeOfLastCombatThink == 0 || (theCurrentTime > (this->mTimeOfLastCombatThink + theCombatThinkInterval)))
            {
                float theExperienceRate = BALANCE_VAR(kCombatExperienceAlienGrowthRate);
                float theExperienceGained = theCombatThinkInterval*theExperienceRate;
                this->SetExperience(this->GetExperience() + theExperienceGained);

                // Update time
                this->mTimeOfLastCombatThink = theCurrentTime;
            }
        }
	}
}


void AvHTeam::InitializeCombatTechNodes()
{
	// If team is marine
	if(this->mTeamType == AVH_CLASS_TYPE_MARINE)
	{
		this->AddTechNode(RESEARCH_WEAPONS_ONE, TECH_ONE_LEVEL_ONE, TECH_NULL, TECH_NULL, false);
		this->AddTechNode(RESEARCH_WEAPONS_TWO, TECH_ONE_LEVEL_TWO, TECH_ONE_LEVEL_ONE, TECH_NULL, false);
		this->AddTechNode(RESEARCH_WEAPONS_THREE, TECH_NULL, TECH_ONE_LEVEL_TWO, TECH_NULL, false);
		this->AddTechNode(BUILD_SHOTGUN, TECH_ONE_LEVEL_THREE, TECH_ONE_LEVEL_ONE, TECH_NULL, false);
		this->AddTechNode(BUILD_GRENADE_GUN, TECH_NULL, TECH_ONE_LEVEL_THREE, TECH_NULL, false);
		this->AddTechNode(BUILD_HMG, TECH_NULL, TECH_ONE_LEVEL_THREE, TECH_NULL, false);
		
		this->AddTechNode(RESEARCH_ARMOR_ONE, TECH_TWO_LEVEL_ONE, TECH_NULL, TECH_NULL, false);
		this->AddTechNode(RESEARCH_ARMOR_TWO, TECH_TWO_LEVEL_TWO, TECH_TWO_LEVEL_ONE, TECH_NULL, false);
		this->AddTechNode(RESEARCH_ARMOR_THREE, TECH_NULL, TECH_TWO_LEVEL_TWO, TECH_NULL, false);
		this->AddTechNode(BUILD_JETPACK, TECH_NULL, TECH_TWO_LEVEL_TWO, TECH_NULL, false);
		this->AddTechNode(BUILD_HEAVY, TECH_NULL, TECH_TWO_LEVEL_TWO, TECH_NULL, false);
		
		this->AddTechNode(BUILD_SCAN, TECH_THREE_LEVEL_ONE, TECH_NULL, TECH_NULL, false, false);
        this->AddTechNode(RESEARCH_MOTIONTRACK, TECH_THREE_LEVEL_ONE, TECH_NULL, TECH_NULL, false);
        //this->AddTechNode(RESEARCH_DISTRESSBEACON, TECH_THREE_LEVEL_ONE, TECH_NULL, TECH_NULL, true, false);

        this->AddTechNode(BUILD_RESUPPLY, TECH_THREE_LEVEL_ONE, TECH_NULL, TECH_NULL, false, false);
        this->AddTechNode(BUILD_CAT, TECH_THREE_LEVEL_ONE, TECH_NULL, TECH_NULL, false, false);
		
		this->AddTechNode(BUILD_WELDER, TECH_THREE_LEVEL_ONE, TECH_NULL, TECH_NULL, false, false);
		this->AddTechNode(BUILD_MINES, TECH_THREE_LEVEL_ONE, TECH_NULL, TECH_NULL, false, false);
        this->AddTechNode(RESEARCH_GRENADES, TECH_THREE_LEVEL_ONE, TECH_NULL, TECH_NULL, false, false);
	}
	else
	{
        
        // Deny skulks so that player can't "re-evolve" to skulks.        
        this->AddTechNode(ALIEN_LIFEFORM_ONE, TECH_NULL, TECH_PLAYER_UNAVAILABLE, TECH_NULL, false);
        
        this->AddTechNode(ALIEN_LIFEFORM_TWO, TECH_ONE_LEVEL_ONE, TECH_NULL, TECH_NULL, false);
		this->AddTechNode(ALIEN_LIFEFORM_THREE, TECH_ONE_LEVEL_TWO, TECH_NULL, TECH_NULL, false);
		this->AddTechNode(ALIEN_LIFEFORM_FOUR, TECH_ONE_LEVEL_THREE, TECH_NULL, TECH_NULL, false);
		this->AddTechNode(ALIEN_LIFEFORM_FIVE, TECH_ONE_LEVEL_FOUR, TECH_NULL, TECH_NULL, false);
				
		this->AddTechNode(ALIEN_HIVE_TWO_UNLOCK, TECH_TWO_LEVEL_ONE, TECH_NULL, TECH_NULL, false);
		this->AddTechNode(ALIEN_HIVE_THREE_UNLOCK, TECH_THREE_LEVEL_TWO, TECH_TWO_LEVEL_ONE, TECH_NULL, false);
				
		this->AddTechNode(ALIEN_EVOLUTION_ONE, TECH_THREE_LEVEL_ONE, TECH_NULL, TECH_NULL, false);
		this->AddTechNode(ALIEN_EVOLUTION_TWO, TECH_THREE_LEVEL_ONE, TECH_NULL, TECH_NULL, false);
		this->AddTechNode(ALIEN_EVOLUTION_THREE, TECH_THREE_LEVEL_ONE, TECH_NULL, TECH_NULL, false);
				
		this->AddTechNode(ALIEN_EVOLUTION_TEN, TECH_FOUR_LEVEL_ONE, TECH_NULL, TECH_NULL, false);
		this->AddTechNode(ALIEN_EVOLUTION_TWELVE, TECH_FOUR_LEVEL_ONE, TECH_NULL, TECH_NULL, false);
		this->AddTechNode(ALIEN_EVOLUTION_ELEVEN, TECH_FOUR_LEVEL_ONE, TECH_NULL, TECH_NULL, false);
				
		this->AddTechNode(ALIEN_EVOLUTION_SEVEN, TECH_FIVE_LEVEL_ONE, TECH_NULL, TECH_NULL, false);
		this->AddTechNode(ALIEN_EVOLUTION_EIGHT, TECH_FIVE_LEVEL_ONE, TECH_NULL, TECH_NULL, false);
		this->AddTechNode(ALIEN_EVOLUTION_NINE, TECH_FIVE_LEVEL_ONE, TECH_NULL, TECH_NULL, false);
	}
}

void AvHGamerules::AwardExperience(AvHPlayer* inPlayer, int inTargetLevel, bool inAwardFriendliesInRange)
{
	PlayerListType thePlayerList;
	thePlayerList.push_back(inPlayer);

	if(inAwardFriendliesInRange)
	{
		// Award experience to player, and any other players nearby
		int theExperienceRadius = BALANCE_VAR(kCombatFriendlyNearbyRange);

		// Make list of players to split it between.  If a player is at full experience, extra is wasted.
		CBaseEntity* theEntity = NULL;
		while ((theEntity = UTIL_FindEntityInSphere(theEntity, inPlayer->pev->origin, theExperienceRadius)) != NULL)
		{
			const char* theClassName = STRING(theEntity->pev->classname);
			if(!AvHSUGetIsExternalClassName(theClassName))
			{
				AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
				if(thePlayer && (thePlayer != inPlayer) && (thePlayer->pev->team == inPlayer->pev->team) && thePlayer->GetIsRelevant() && !thePlayer->GetIsBeingDigested())
				{
					thePlayerList.push_back(thePlayer);
				}
			}
		}
	}
	
	ASSERT(thePlayerList.size() > 0);

	float theExperienceFactor = GetGameRules()->GetIsIronMan() ? BALANCE_VAR(kCombatIronManExperienceScalar) : 1.0f;

	int theExperienceToAward = BALANCE_VAR(kCombatExperienceBaseAward) + inTargetLevel*BALANCE_VAR(kCombatExperienceLevelAward);

	float theExperienceForEach = (theExperienceToAward/(float)thePlayerList.size() + BALANCE_VAR(kCombatExperienceCrowdAward))*theExperienceFactor;

	for(PlayerListType::iterator thePlayerIter = thePlayerList.begin(); thePlayerIter != thePlayerList.end(); thePlayerIter++)
	{
		AvHPlayer* theCurrentPlayer = (*thePlayerIter);
		theCurrentPlayer->SetExperience(theCurrentPlayer->GetExperience() + theExperienceForEach);
	}
}
