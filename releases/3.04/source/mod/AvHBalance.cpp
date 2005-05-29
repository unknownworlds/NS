#include "build.h"
#include "util/nowarnings.h"
#include "mod/AvHConstants.h"

#ifdef AVH_SERVER
#include "extdll.h"
#include "dlls/util.h"
#include "cbase.h"
#include "player.h"
#include "weapons.h"
#include "mod/AvHGamerules.h"
#include "util/STLUtil.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHServerUtil.h"
#endif

#ifdef AVH_CLIENT
#include "cl_dll/hud.h"
#include "mod/AvHHud.h"
#endif

#ifdef AVH_PLAYTEST_BUILD

int AvHGetBalanceInt(const BalanceIntListType& inBalanceInts, const string& inName)
{
	int theBalanceValue = 0;

	// Look it up
	bool theFoundIt = false;

	if(inBalanceInts.size() > 0)
	{
		for(BalanceIntListType::const_iterator theIter = inBalanceInts.begin(); theIter != inBalanceInts.end(); theIter++)
		{
			if(theIter->first == inName)
			{
				theBalanceValue = theIter->second;
				theFoundIt = true;
				break;
			}
		}
	}

	if(!theFoundIt)
	{
		#ifdef AVH_SERVER
		char theErrorString[512];
		sprintf(theErrorString, "Couldn't find balance key %s\r\n", inName.c_str());
		ALERT(at_logged, theErrorString);
		#endif
	}

	return theBalanceValue;
}

float AvHGetBalanceFloat(const BalanceFloatListType& inBalanceFloats, const string& inName)
{
	float theBalanceValue = 0;
	
	// Look it up
	bool theFoundIt = false;

	if(inBalanceFloats.size() > 0)
	{
		for(BalanceFloatListType::const_iterator theIter = inBalanceFloats.begin(); theIter != inBalanceFloats.end(); theIter++)
		{
			if(theIter->first == inName)
			{
				theBalanceValue = theIter->second;
				theFoundIt = true;
				break;
			}
		}
	}
	
	if(!theFoundIt)
	{
		#ifdef AVH_SERVER
		char theErrorString[512];
		sprintf(theErrorString, "Couldn't find balance key %s\r\n", inName.c_str());
		ALERT(at_logged, theErrorString);
		#endif
	}
	
	return theBalanceValue;
}

// Lookup current value of variable
int GetBalanceInt(const string& inName)
{
	int theValue = 0;

	#ifdef AVH_SERVER
	// Look up variable via gamerules
	theValue = GetGameRules()->GetBalanceInt(inName);
	#endif

	#ifdef AVH_CLIENT
	const BalanceIntListType& theBalanceInts = gHUD.GetBalanceInts();
	theValue = AvHGetBalanceInt(theBalanceInts, inName);
	#endif

	return theValue;
}

float GetBalanceFloat(const string& inName)
{
	float theValue = 0.0f;

	#ifdef AVH_SERVER
	// Look up variable via gamerules
	theValue = GetGameRules()->GetBalanceFloat(inName);
	#endif

	#ifdef AVH_CLIENT
	const BalanceFloatListType& theBalanceFloats = gHUD.GetBalanceFloats();
	theValue = AvHGetBalanceFloat(theBalanceFloats, inName);
	#endif

	return theValue;
}
#endif

#ifdef AVH_SERVER

#ifdef AVH_PLAYTEST_BUILD

// Playtest functionality
int	AvHGamerules::GetBalanceInt(const string& inName) const
{
	int theBalanceValue = 0;

	theBalanceValue = AvHGetBalanceInt(this->mBalanceInts, inName);

	return theBalanceValue;
}

float AvHGamerules::GetBalanceFloat(const string& inName) const
{
	float theBalanceValue = 0;

	theBalanceValue = AvHGetBalanceFloat(this->mBalanceFloats, inName);
	
	return theBalanceValue;
}

const BalanceIntListType& AvHGamerules::GetBalanceInts() const
{
	return this->mBalanceInts;
}

const BalanceFloatListType&	AvHGamerules::GetBalanceFloats() const
{
	return this->mBalanceFloats;
}

void AvHGamerules::BalanceChanged()
{
	this->mTeamOne.BalanceChanged();
	this->mTeamTwo.BalanceChanged();

	// Tell all players to update their weapons
	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		theEntity->SendWeaponUpdate();
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
}
#endif

static string theBalanceFileName = string(getModDirectory()) + "/Balance.txt";

#ifdef AVH_PLAYTEST_BUILD

void AvHGamerules::ReadBalanceData()
{
	this->mBalanceInts.clear();
	this->mBalanceFloats.clear();

    fstream inFile;
    inFile.open(theBalanceFileName.c_str(), ios::in);
	
    if(inFile.is_open())
    {
		const int kMaxLineLength = 512;
		char theLineCStr[kMaxLineLength];
		bool theIsDone = false;

		while(!theIsDone)
		{
			inFile.getline(theLineCStr, kMaxLineLength);
			
			string theLine(theLineCStr);

			if(theLine.length() > 0)
			{
				// Check for comment
				if(theLine[0] != '/')
				{
					StringVector theTokens;
					if(Tokenizer::split(theLine, " \t", theTokens) == 3)
					{
						// "define" "variablename" "value"
						string theStringName = theTokens[1];
						string theStringValue = theTokens[2];
					
						bool theIsFloat = (std::find(theStringValue.begin(), theStringValue.end(), '.') != theStringValue.end());
						if(theIsFloat)
						{
							float theFloatValue = MakeFloatFromString(theStringValue);
							this->mBalanceFloats[theStringName] = theFloatValue;
						}
						else
						{
							int theIntValue = MakeIntFromString(theStringValue);
							this->mBalanceInts[theStringName] = theIntValue;
						}
					}
					else
					{
						char theErrorString[512];
						sprintf(theErrorString, "Balance.txt line: %s couldn't be parsed.\r\n", theLine);
						ALERT(at_logged, theErrorString);
					}
				}
			}

			if(inFile.eof())
			{
				theIsDone = true;
			}
		}

		inFile.close();
	}
}

void AvHGamerules::RecordBalanceData()
{
    fstream theOutfile;
    theOutfile.open(theBalanceFileName.c_str(), ios::out);
    if(theOutfile.is_open())
	{
		// Write out header
		string theFirstLine("// Automatically generated file. Last created ");
		string theTimeDateString = AvHSHUGetTimeDateString();
		theOutfile << theFirstLine << theTimeDateString << string(".") << std::endl;

		// Now write out #defines for each int
		string theIntHeader("// Integer data");
		theOutfile << std::endl << theIntHeader << std::endl;

		for(BalanceIntListType::const_iterator theIntIter = this->mBalanceInts.begin(); theIntIter != this->mBalanceInts.end(); theIntIter++)
		{
			string theLine = "#define " + theIntIter->first + string(" ") + MakeStringFromInt(theIntIter->second);
			theOutfile << theLine << std::endl;
		}

		string theFloatHeader("// Float data (up to three decimal places)");
		theOutfile << std::endl << theFloatHeader << std::endl;

		// Now write out #defines for each float
		for(BalanceFloatListType::const_iterator theFloatIter = this->mBalanceFloats.begin(); theFloatIter != this->mBalanceFloats.end(); theFloatIter++)
		{
			string theLine = "#define " + theFloatIter->first + string(" ") + MakeStringFromFloat(theFloatIter->second, 2);
			theOutfile << theLine << std::endl;
		}
		
        theOutfile.close();
	}
}
#endif

int	AvHGamerules::GetBaseHealthForMessageID(AvHMessageID inMessageID) const
{
	int	theBaseHealth = 100;

	switch(inMessageID)
	{
		case MESSAGE_NULL:
		case BUILD_RECYCLE:
			theBaseHealth = 0;
			break;
		
		// Structures
		case BUILD_INFANTRYPORTAL:
			theBaseHealth = BALANCE_IVAR(kInfantryPortalHealth);
			break;
		case BUILD_RESOURCES:
			theBaseHealth = BALANCE_IVAR(kResourceTowerHealth);
			break;
		case BUILD_TURRET_FACTORY:
		case TURRET_FACTORY_UPGRADE:
			theBaseHealth = BALANCE_IVAR(kTurretFactoryHealth);
			break;
		case BUILD_ARMSLAB:
			theBaseHealth = BALANCE_IVAR(kArmsLabHealth);
			break;
		case BUILD_PROTOTYPE_LAB:
			theBaseHealth = BALANCE_IVAR(kPrototypeLabHealth);
			break;
		case BUILD_ARMORY:
			theBaseHealth = BALANCE_IVAR(kArmoryHealth);
			break;
		case ARMORY_UPGRADE:
			theBaseHealth = BALANCE_IVAR(kAdvArmoryHealth);
			break;
		case BUILD_OBSERVATORY:
			theBaseHealth = BALANCE_IVAR(kObservatoryHealth);
			break;

		case BUILD_PHASEGATE:
			theBaseHealth = BALANCE_IVAR(kPhaseGateHealth);
			break;
		case BUILD_TURRET:
			theBaseHealth = BALANCE_IVAR(kSentryHealth);
			break;
		case BUILD_SIEGE:
			theBaseHealth = BALANCE_IVAR(kSiegeHealth);
			break;
		case BUILD_COMMANDSTATION:
			theBaseHealth = BALANCE_IVAR(kCommandStationHealth);
			break;
		
		// Weapons and items
		case ALIEN_BUILD_RESOURCES:
			theBaseHealth = BALANCE_IVAR(kAlienResourceTowerHealth);
			break;
		case ALIEN_BUILD_OFFENSE_CHAMBER:
			theBaseHealth = BALANCE_IVAR(kOffenseChamberHealth);
			break;
		case ALIEN_BUILD_DEFENSE_CHAMBER:
			theBaseHealth = BALANCE_IVAR(kDefenseChamberHealth);
			break;
		case ALIEN_BUILD_SENSORY_CHAMBER:
			theBaseHealth = BALANCE_IVAR(kSensoryChamberHealth);
			break;
		case ALIEN_BUILD_MOVEMENT_CHAMBER:
			theBaseHealth = BALANCE_IVAR(kMovementChamberHealth);
			break;
		case ALIEN_BUILD_HIVE:
			theBaseHealth = BALANCE_IVAR(kHiveHealth);
			break;
	}
	
	return theBaseHealth;
}


int	AvHGamerules::GetBuildTimeForMessageID(AvHMessageID inMessageID) const
{
	int	theBuildTime = 0;
	const float theCombatModeScalar = this->GetIsCombatMode() ? BALANCE_FVAR(kCombatModeTimeScalar) : 1.0f;
	const float theCombatModeGestationScalar = this->GetIsCombatMode() ? BALANCE_FVAR(kCombatModeGestationTimeScalar) : 1.0f;

	switch(inMessageID)
	{
		// Research
		case RESEARCH_ELECTRICAL:
			theBuildTime = BALANCE_IVAR(kElectricalUpgradeResearchTime);
			break;
		case RESEARCH_ARMOR_ONE:
			theBuildTime = BALANCE_IVAR(kArmorOneResearchTime);
			break;
		case RESEARCH_ARMOR_TWO:
			theBuildTime = BALANCE_IVAR(kArmorTwoResearchTime);
			break;
		case RESEARCH_ARMOR_THREE:
			theBuildTime = BALANCE_IVAR(kArmorThreeResearchTime);
			break;
		case RESEARCH_WEAPONS_ONE:
			theBuildTime = BALANCE_IVAR(kWeaponsOneResearchTime);
			break;
		case RESEARCH_WEAPONS_TWO:
			theBuildTime = BALANCE_IVAR(kWeaponsTwoResearchTime);
			break;
		case RESEARCH_WEAPONS_THREE:
			theBuildTime = BALANCE_IVAR(kWeaponsThreeResearchTime);
			break;

        case RESEARCH_CATALYSTS:
            theBuildTime = BALANCE_IVAR(kCatalystResearchTime);
            break;
            
        case RESEARCH_GRENADES:
            theBuildTime = BALANCE_IVAR(kGrenadesResearchTime);
            break;

		case TURRET_FACTORY_UPGRADE:
			theBuildTime = BALANCE_IVAR(kTurretFactoryUpgradeTime);
			break;

		case RESEARCH_JETPACKS:
			theBuildTime = BALANCE_IVAR(kJetpacksResearchTime);
			break;
		case RESEARCH_HEAVYARMOR:
			theBuildTime = BALANCE_IVAR(kHeavyArmorResearchTime);
			break;
		// Distress beacon (should be the same length as misc/distressbeacon.wav)
		case RESEARCH_DISTRESSBEACON:
			theBuildTime = BALANCE_IVAR(kDistressBeaconTime);
			break;

		case RESEARCH_HEALTH:
			theBuildTime = BALANCE_IVAR(kHealthResearchTime);
			break;

		case RESEARCH_MOTIONTRACK:
			theBuildTime = BALANCE_IVAR(kMotionTrackingResearchTime);
			break;

		case RESEARCH_PHASETECH:
			theBuildTime = BALANCE_IVAR(kPhaseTechResearchTime);
			break;

		// Structures
		case BUILD_INFANTRYPORTAL:
			theBuildTime = BALANCE_IVAR(kInfantryPortalBuildTime);
			break;
		case BUILD_RESOURCES:
			theBuildTime = BALANCE_IVAR(kResourceTowerBuildTime);
			break;
		case BUILD_TURRET_FACTORY:
			theBuildTime = BALANCE_IVAR(kTurretFactoryBuildTime);
			break;
		case BUILD_ARMSLAB:
			theBuildTime = BALANCE_IVAR(kArmsLabBuildTime);
			break;
		case BUILD_PROTOTYPE_LAB:
			theBuildTime = BALANCE_IVAR(kPrototypeLabBuildTime);
			break;
		case BUILD_ARMORY:
			theBuildTime = BALANCE_IVAR(kArmoryBuildTime);
			break;
		case ARMORY_UPGRADE:
			theBuildTime = BALANCE_IVAR(kArmoryUpgradeTime);
			break;
		case BUILD_OBSERVATORY:
			theBuildTime = BALANCE_IVAR(kObservatoryBuildTime);
			break;
		// Scan duration
		case BUILD_SCAN:
			theBuildTime = BALANCE_IVAR(kScanDuration);
			break;
	
		case BUILD_PHASEGATE:
			theBuildTime = BALANCE_IVAR(kPhaseGateBuildTime);
			break;
		case BUILD_TURRET:
			theBuildTime = BALANCE_IVAR(kSentryBuildTime);
			break;
		case BUILD_SIEGE:
			theBuildTime = BALANCE_IVAR(kSiegeBuildTime);
			break;
		case BUILD_COMMANDSTATION:
			theBuildTime = BALANCE_IVAR(kCommandStationBuildTime);
			break;
		
		// Weapons and items
		case ALIEN_BUILD_RESOURCES:
			theBuildTime = BALANCE_IVAR(kAlienResourceTowerBuildTime);
			break;
		case ALIEN_BUILD_OFFENSE_CHAMBER:
			theBuildTime = BALANCE_IVAR(kOffenseChamberBuildTime);
			break;
		case ALIEN_BUILD_DEFENSE_CHAMBER:
			theBuildTime = BALANCE_IVAR(kDefenseChamberBuildTime);
			break;
		case ALIEN_BUILD_SENSORY_CHAMBER:
			theBuildTime = BALANCE_IVAR(kSensoryChamberBuildTime);
			break;
		case ALIEN_BUILD_MOVEMENT_CHAMBER:
			theBuildTime = BALANCE_IVAR(kMovementChamberBuildTime);
			break;
		case ALIEN_BUILD_HIVE:
			theBuildTime = BALANCE_IVAR(kHiveBuildTime);
			break;

		// Alien menu items
		case ALIEN_EVOLUTION_ONE:
		case ALIEN_EVOLUTION_TWO:
		case ALIEN_EVOLUTION_THREE:
		case ALIEN_EVOLUTION_SEVEN:
		case ALIEN_EVOLUTION_EIGHT:
		case ALIEN_EVOLUTION_NINE:
		case ALIEN_EVOLUTION_TEN:
		case ALIEN_EVOLUTION_ELEVEN:
		case ALIEN_EVOLUTION_TWELVE:
        case ALIEN_HIVE_TWO_UNLOCK:
        case ALIEN_HIVE_THREE_UNLOCK:
			theBuildTime = max(1, BALANCE_IVAR(kEvolutionGestateTime)*theCombatModeScalar);
			break;
	
		// Alien lifeforms
		case ALIEN_LIFEFORM_ONE:
			theBuildTime = BALANCE_IVAR(kSkulkGestateTime)*theCombatModeGestationScalar;
			break;
		case ALIEN_LIFEFORM_TWO:
			theBuildTime = BALANCE_IVAR(kGorgeGestateTime)*theCombatModeGestationScalar;
			break;
		case ALIEN_LIFEFORM_THREE:
			theBuildTime = BALANCE_IVAR(kLerkGestateTime)*theCombatModeGestationScalar;
			break;
		case ALIEN_LIFEFORM_FOUR:
			theBuildTime = BALANCE_IVAR(kFadeGestateTime)*theCombatModeGestationScalar;
			break;
		case ALIEN_LIFEFORM_FIVE:
			theBuildTime = BALANCE_IVAR(kOnosGestateTime)*theCombatModeGestationScalar;
			break;
	}

	if(this->GetCheatsEnabled() && !this->GetIsCheatEnabled(kcSlowResearch))
	{
		theBuildTime = min(theBuildTime, 2.0f);
	}

    // For testing
    //theBuildTime = 10;
	
	return theBuildTime;
}

int	AvHGamerules::GetCostForMessageID(AvHMessageID inMessageID) const
{
	// This is point cost or energy cost in NS, or number of levels in Combat
	int	theCost = 0;

	if(this->GetIsCombatMode())
    {
        switch(inMessageID)
        {
		case ALIEN_LIFEFORM_TWO:
        case ALIEN_EVOLUTION_ONE:
        case ALIEN_EVOLUTION_TWO:
        case ALIEN_EVOLUTION_THREE:
        case ALIEN_EVOLUTION_SEVEN:
        case ALIEN_EVOLUTION_EIGHT:
        case ALIEN_EVOLUTION_NINE:
        case ALIEN_EVOLUTION_TEN:
        case ALIEN_EVOLUTION_TWELVE:
        case BUILD_RESUPPLY:
        case RESEARCH_ARMOR_ONE:
        case RESEARCH_ARMOR_TWO:
        case RESEARCH_ARMOR_THREE:
        case RESEARCH_WEAPONS_ONE:
        case RESEARCH_WEAPONS_TWO:
        case RESEARCH_WEAPONS_THREE:
        case BUILD_CAT:
        case RESEARCH_MOTIONTRACK:
        case RESEARCH_GRENADES:
        case BUILD_SCAN:
        case BUILD_MINES:
        case BUILD_WELDER:
        case BUILD_SHOTGUN:
        case BUILD_HMG:
        case BUILD_GRENADE_GUN:
        case ALIEN_HIVE_TWO_UNLOCK:
            theCost = 1;
            break;

		case ALIEN_LIFEFORM_THREE:
        case BUILD_HEAVY:
        case BUILD_JETPACK:
        case ALIEN_HIVE_THREE_UNLOCK:
		case ALIEN_EVOLUTION_ELEVEN:
            theCost = 2;
            break;

        case ALIEN_LIFEFORM_FOUR:
			theCost = 3;
			break;

        case ALIEN_LIFEFORM_FIVE:
			theCost = 4;
			break;
        }
    }
    else
	{
		switch(inMessageID)
		{
			// Research
			case RESEARCH_ELECTRICAL:
				theCost = BALANCE_IVAR(kElectricalUpgradeResearchCost);
				break;
			case RESEARCH_ARMOR_ONE:
				theCost = BALANCE_IVAR(kArmorOneResearchCost);
				break;
			case RESEARCH_ARMOR_TWO:
				theCost = BALANCE_IVAR(kArmorTwoResearchCost);
				break;
			case RESEARCH_ARMOR_THREE:
				theCost = BALANCE_IVAR(kArmorThreeResearchCost);
				break;
			case RESEARCH_WEAPONS_ONE:
				theCost = BALANCE_IVAR(kWeaponsOneResearchCost);
				break;
			case RESEARCH_WEAPONS_TWO:
				theCost = BALANCE_IVAR(kWeaponsTwoResearchCost);
				break;
			case RESEARCH_WEAPONS_THREE:
				theCost = BALANCE_IVAR(kWeaponsThreeResearchCost);
				break;
            case RESEARCH_CATALYSTS:
                theCost = BALANCE_IVAR(kCatalystResearchCost);
                break;
            case RESEARCH_GRENADES:
                theCost = BALANCE_IVAR(kGrenadesResearchCost);
                break;
			case TURRET_FACTORY_UPGRADE:
				theCost = BALANCE_IVAR(kTurretFactoryUpgradeCost);
				break;

			case RESEARCH_JETPACKS:
				theCost = BALANCE_IVAR(kJetpacksResearchCost);
				break;
			case RESEARCH_HEAVYARMOR:
				theCost = BALANCE_IVAR(kHeavyArmorResearchCost);
				break;
			case RESEARCH_HEALTH:
				theCost = BALANCE_IVAR(kHealthResearchCost);
				break;
			case RESEARCH_MOTIONTRACK:
				theCost = BALANCE_IVAR(kMotionTrackingResearchCost);
				break;
			case RESEARCH_PHASETECH:
				theCost = BALANCE_IVAR(kPhaseTechResearchCost);
				break;
			case RESEARCH_DISTRESSBEACON:
				theCost = BALANCE_IVAR(kDistressBeaconCost);
				break;
		
			// Structures
			case BUILD_HEAVY:
				theCost = BALANCE_IVAR(kHeavyArmorCost);
				break;
			case BUILD_JETPACK:
				theCost = BALANCE_IVAR(kJetpackCost);
				break;
			case BUILD_INFANTRYPORTAL:
				theCost = BALANCE_IVAR(kInfantryPortalCost);
				break;
			case BUILD_RESOURCES:
				theCost = BALANCE_IVAR(kResourceTowerCost);
				break;
			case BUILD_TURRET_FACTORY:
				theCost = BALANCE_IVAR(kTurretFactoryCost);
				break;
			case BUILD_ARMSLAB:
				theCost = BALANCE_IVAR(kArmsLabCost);
				break;
			case BUILD_PROTOTYPE_LAB:
				theCost = BALANCE_IVAR(kPrototypeLabCost);
				break;
			case BUILD_ARMORY:
				theCost = BALANCE_IVAR(kArmoryCost);
				break;
			case ARMORY_UPGRADE:
				theCost = BALANCE_IVAR(kArmoryUpgradeCost);
				break;
			case BUILD_OBSERVATORY:
				theCost = BALANCE_IVAR(kObservatoryCost);
				break;
		
			case BUILD_PHASEGATE:
				theCost = BALANCE_IVAR(kPhaseGateCost);
				break;
			case BUILD_TURRET:
				theCost = BALANCE_IVAR(kSentryCost);
				break;
			case BUILD_SIEGE:
				theCost = BALANCE_IVAR(kSiegeCost);
				break;
			case BUILD_COMMANDSTATION:
				theCost = BALANCE_IVAR(kCommandStationCost);
				break;
			
			// Weapons and items
			case BUILD_HEALTH:
				theCost = BALANCE_IVAR(kHealthCost);
				break;
			case BUILD_AMMO:
				theCost = BALANCE_IVAR(kAmmoCost);
				break;
            case BUILD_CAT:
                theCost = BALANCE_IVAR(kCatalystCost);
				break;
			case BUILD_MINES:
				theCost = BALANCE_IVAR(kMineCost);
				break;
			case BUILD_WELDER:
				theCost = BALANCE_IVAR(kWelderCost);
				break;
			case BUILD_SHOTGUN:
				theCost = BALANCE_IVAR(kShotgunCost);
				break;
			case BUILD_HMG:
				theCost = BALANCE_IVAR(kHMGCost);
				break;
			case BUILD_GRENADE_GUN:
				theCost = BALANCE_IVAR(kGrenadeLauncherCost);
				break;
		
			case ALIEN_BUILD_RESOURCES:
				theCost = BALANCE_IVAR(kAlienResourceTowerCost);
				break;
			case ALIEN_BUILD_OFFENSE_CHAMBER:
				theCost = BALANCE_IVAR(kOffenseChamberCost);
				break;
			case ALIEN_BUILD_DEFENSE_CHAMBER:
				theCost = BALANCE_IVAR(kDefenseChamberCost);
				break;
			case ALIEN_BUILD_SENSORY_CHAMBER:
				theCost = BALANCE_IVAR(kSensoryChamberCost);
				break;
			case ALIEN_BUILD_MOVEMENT_CHAMBER:
				theCost = BALANCE_IVAR(kMovementChamberCost);
				break;
			case ALIEN_BUILD_HIVE:
				theCost = BALANCE_IVAR(kHiveCost);
				break;

			// Alien menu items
			case ALIEN_EVOLUTION_ONE:
			case ALIEN_EVOLUTION_TWO:
			case ALIEN_EVOLUTION_THREE:
			case ALIEN_EVOLUTION_SEVEN:
			case ALIEN_EVOLUTION_EIGHT:
			case ALIEN_EVOLUTION_NINE:
			case ALIEN_EVOLUTION_TEN:
			case ALIEN_EVOLUTION_ELEVEN:
			case ALIEN_EVOLUTION_TWELVE:
				theCost = BALANCE_IVAR(kEvolutionCost);
				break;
		
			// Alien lifeforms
			case ALIEN_LIFEFORM_ONE:
				theCost = BALANCE_IVAR(kSkulkCost);
				break;
			case ALIEN_LIFEFORM_TWO:
				theCost = BALANCE_IVAR(kGorgeCost);
				break;
			case ALIEN_LIFEFORM_THREE:
				theCost = BALANCE_IVAR(kLerkCost);
				break;
			case ALIEN_LIFEFORM_FOUR:
				theCost = BALANCE_IVAR(kFadeCost);
				break;
			case ALIEN_LIFEFORM_FIVE:
				theCost = BALANCE_IVAR(kOnosCost);
				break;

			// Energy costs
			case BUILD_SCAN:
				theCost = BALANCE_IVAR(kScanEnergyCost);
				break;
		}
	}
	
	return theCost;
}

#endif