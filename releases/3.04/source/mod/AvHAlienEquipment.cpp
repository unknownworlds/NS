//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAlienEquipment.cpp $
// $Date: 2002/11/22 21:28:15 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAlienEquipment.cpp,v $
// Revision 1.17  2002/11/22 21:28:15  Flayra
// - mp_consistency changes
//
// Revision 1.16  2002/11/15 04:47:30  Flayra
// - Def chambers only up to three targets per tick
//
// Revision 1.15  2002/10/16 00:46:51  Flayra
// - Movement chambers not useable until 3 seconds after built
// - Moved and removed some building sounds, added one for chamber death
// - Don't allow using movement chamber while gestating
//
// Revision 1.14  2002/10/03 18:31:29  Flayra
// - Movement chambers now teleport you to the farthest hive
//
// Revision 1.13  2002/09/25 20:42:14  Flayra
// - Defensive chamber performance/gameplay tweak
//
// Revision 1.12  2002/09/23 22:07:11  Flayra
// - Removed alien building fading
// - Removed bug where builders got rewarded twice for building a structure
//
// Revision 1.11  2002/09/09 19:47:47  Flayra
// - Alien buildings now animate
// - Sensory chamber no longer hurts players when touched
//
// Revision 1.10  2002/08/31 18:01:00  Flayra
// - Work at VALVe
//
// Revision 1.9  2002/08/16 02:32:09  Flayra
// - Added damage types
// - Swapped umbra and bile bomb
//
//===============================================================================
#include "mod/AvHAlienEquipment.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHTeam.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHSharedUtil.h"
#include "util/MathUtil.h"
#include "mod/AvHAlienAbilityConstants.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHMovementUtil.h"

LINK_ENTITY_TO_CLASS(kwAlienResourceTower, AvHAlienResourceTower);

AvHAlienResourceTower::AvHAlienResourceTower() : AvHResourceTower(TECH_ALIEN_RESOURCE_NODE, ALIEN_BUILD_RESOURCES, kwsAlienResourceTower)
{
	this->SetAverageUseSoundLength(kAverageAlienUseSoundLength);
}

int	AvHAlienResourceTower::GetSequenceForBoundingBox() const
{
	return 2;
}

int	AvHAlienResourceTower::GetActiveAnimation() const
{
	int theAnim = AvHBaseBuildable::GetActiveAnimation();

	int theRandomNumber = RANDOM_LONG(0, 10);
	if(theRandomNumber == 10)
	{
		theAnim = 3;
	}
	else if(theRandomNumber > 7)
	{
		theAnim	= 2;
	}

	return theAnim;
}

char* AvHAlienResourceTower::GetDeploySound() const
{
    return kChamberDeploySound;
}

bool AvHAlienResourceTower::GetIsOrganic() const
{
	return true;
}

int	AvHAlienResourceTower::GetPointValue() const
{
	return 3;
}

char* AvHAlienResourceTower::GetModelName() const
{
	return kAlienResourceTowerModel;
}

char* AvHAlienResourceTower::GetActiveSoundList() const
{
	char* theActiveSoundList = NULL;

	// Don't play active sounds until kAlienResourceTowerSoundDelayTime seconds have passed, to prevent marines from knowing where aliens start
	int theTimeToWaitBeforeSounds = BALANCE_IVAR(kAlienResourceTowerSoundDelayTime);
	int theGameTime = GetGameRules()->GetGameTime();
	if((theGameTime > 0) && (theGameTime >= theTimeToWaitBeforeSounds))
	{
		theActiveSoundList = kAlienResourceTowerSoundList;
	}
	else
	{
		int a = 0;
	}

	return theActiveSoundList;
}

void AvHAlienResourceTower::Killed(entvars_t* pevAttacker, int iGib)
{
	EMIT_SOUND(this->edict(), CHAN_AUTO, kChamberDieSound, 1.0f, ATTN_NORM);

	AvHResourceTower::Killed(pevAttacker, iGib);
}

void AvHAlienResourceTower::Precache()
{
	AvHResourceTower::Precache();

	PRECACHE_UNMODIFIED_SOUND(kChamberDieSound);
}

void AvHAlienResourceTower::AlienResourceThink()
{
	if(!this->GetIsBuilt())
	{
		this->UpdateAutoBuild(kAlienBuildingThinkInterval*kAutoBuildScalar);
		
		this->pev->nextthink = gpGlobals->time + kAlienBuildingThinkInterval;
	}
}

void AvHAlienResourceTower::Materialize()
{
	AvHResourceTower::Materialize();

	this->pev->iuser3 = AVH_USER3_ALIENRESTOWER;
	
	SetThink(&AvHAlienResourceTower::AlienResourceThink);
	this->pev->nextthink = gpGlobals->time + kAlienBuildingThinkInterval;
}


void AvHAlienResourceTower::Spawn()
{
	AvHResourceTower::Spawn();
}




AvHAlienUpgradeBuilding::AvHAlienUpgradeBuilding(AvHMessageID inMessageID, AvHTechID inTechID, char* inClassName, AvHUser3 inUser3) : AvHBaseBuildable(inTechID, inMessageID, inClassName, inUser3)
{
	this->SetAverageUseSoundLength(kAverageAlienUseSoundLength);
}

bool AvHAlienUpgradeBuilding::GetIsOrganic() const
{
	return true;
}

int AvHAlienUpgradeBuilding::GetPointValue() const
{
	return BALANCE_IVAR(kScoringAlienUpgradeChamberValue);
}

void AvHAlienUpgradeBuilding::Killed(entvars_t* pevAttacker, int iGib)
{
	//AvHSUExplodeEntity(this, matFlesh);
	EMIT_SOUND(this->edict(), CHAN_AUTO, kChamberDieSound, 1.0f, ATTN_NORM);
	
	if(this->GetIsBuilt())
	{
		// Get team
		AvHTeamNumber theTeamNumber = AvHTeamNumber(this->pev->team);
		AvHTeam* theTeam = GetGameRules()->GetTeam(theTeamNumber);
		ASSERT(theTeam);
		theTeam->RemoveAlienUpgradeCategory(this->GetUpgradeCategory());
	}

	AvHBaseBuildable::Killed(pevAttacker, iGib);
}

void AvHAlienUpgradeBuilding::AlienBuildingThink()
{
	if(!this->GetIsBuilt())
	{
		this->UpdateAutoBuild(kAlienBuildingThinkInterval*kAutoBuildScalar);
		
		this->pev->nextthink = gpGlobals->time + kAlienBuildingThinkInterval;
	}
	else
	{
		SetThink(&AvHBaseBuildable::AnimateThink);
		this->pev->nextthink = gpGlobals->time + .1f;
	}
}

void AvHAlienUpgradeBuilding::Precache()
{
	AvHBaseBuildable::Precache();
		
	PRECACHE_UNMODIFIED_SOUND(kChamberDieSound);
}

void AvHAlienUpgradeBuilding::Materialize()
{
	//Vector theMinSize, theMaxSize;
	//if(AvHSHUGetSizeForTech(this->GetMessageID(), theMinSize, theMaxSize))
	//{
	//	UTIL_SetSize(this->pev, theMinSize, theMaxSize);
	//}

	AvHBaseBuildable::Materialize();
	
	SetThink(&AvHAlienUpgradeBuilding::AlienBuildingThink);
	this->pev->nextthink = gpGlobals->time + kAlienBuildingThinkInterval;
	
	this->pev->rendermode = kRenderNormal;
	this->pev->renderamt = 0;
	
	//	SET_MODEL(ENT(this->pev), this->GetModelName());
	//	
	//	this->pev->solid = SOLID_BBOX;
	//	this->pev->movetype = MOVETYPE_TOSS;
	//	
	//	//UTIL_SetSize(pev, kResourceMinSize, kResourceMaxSize); 
	//	
	//    this->pev->classname = MAKE_STRING(this->GetClassName());
	//	
	//	this->pev->takedamage = DAMAGE_YES;
	//	SetBits(this->pev->flags, FL_MONSTER);
	//	
	//	this->pev->iuser3 = AVH_USER3_BUILDABLE;
	//	this->pev->fuser1 = 0.0f;
	//	//this->pev->iuser4 = AVH_USER4_RESTOWER;
	//	
	//	// Start animating
	//	this->pev->sequence	= 0;
	//	this->pev->frame = 0;
	//	ResetSequenceInfo();
	//	AvHSUSetCollisionBoxFromSequence(this->pev);
}

void AvHAlienUpgradeBuilding::Spawn()
{
	AvHBaseBuildable::Spawn();
}

void AvHAlienUpgradeBuilding::SetHasBeenBuilt()
{
	AvHBaseBuildable::SetHasBeenBuilt();

	AvHAlienUpgradeCategory theCategory = this->GetUpgradeCategory();
	
	// Find the team associated with the building
	AvHTeamNumber theTeamNumber = AvHTeamNumber(this->pev->team);
	AvHTeam* theTeam = GetGameRules()->GetTeam(theTeamNumber);
	ASSERT(theTeam);
	theTeam->AddTeamUpgrade(theCategory);

	//this->SetHasBeenBuilt();
	
	//this->pev->rendermode = kRenderNormal;
	//this->pev->renderamt = 0;
}
	

//LINK_ENTITY_TO_CLASS(kwOffenseChamber, AvHOffenseChamber);
//AvHOffenseChamber::AvHOffenseChamber() : AvHAlienUpgradeBuilding(ALIEN_BUILD_OFFENSE_CHAMBER, kwsOffenseChamber)
//{
//}
//
//char* AvHOffenseChamber::GetDeploySound() const
//{
//	return kChamberDeploySound;
//}
//
//char* AvHOffenseChamber::GetModelName() const
//{
//	return kOffenseChamberModel;
//}
//
//
//
//AvHAlienUpgradeCategory	AvHOffenseChamber::GetUpgradeCategory() const
//{
//	return ALIEN_UPGRADE_CATEGORY_OFFENSE;
//}

//LINK_ENTITY_TO_CLASS(kwOffenseChamber, AvHOffenseChamber);
//AvHOffenseChamber::AvHOffenseChamber() : AvHTurret(TECH_NULL, ALIEN_BUILD_OFFENSE_CHAMBER, kwsOffenseChamber, AVH_USER3_NONE)
//{
//}
//
//char* AvHOffenseChamber::GetActiveSound() const
//{
//	return NULL;
//}
//
//char* AvHOffenseChamber::GetAlertSound() const
//{
//	return NULL;
//}
//
//char* AvHOffenseChamber::GetPingSound() const
//{
//	return NULL;
//}

//int	AvHOffenseChamber::GetActiveAnimation() const;
//int	AvHOffenseChamber::GetDeployAnimation() const;
//int	AvHOffenseChamber::GetIdle1Animation() const;
//int	AvHOffenseChamber::GetIdle2Animation() const;
//int	AvHOffenseChamber::GetKilledAnimation() const;
//int	AvHOffenseChamber::GetSpawnAnimation() const;
//int	AvHOffenseChamber::GetTakeDamageAnimation() const;
//bool AvHOffenseChamber::GetIsValidTarget(CBaseEntity* inEntity) const;
//int	AvHOffenseChamber::IRelationship(CBaseEntity* inTarget);
	
//int	AvHOffenseChamber::GetRange() const
//{
//	return 100;
//}
//
//void AvHOffenseChamber::Shoot(Vector &vecSrc, Vector &vecDirToEnemy)
//{
//}
//
//void AvHOffenseChamber::Spawn()
//{
//	AvHTurret::Spawn();
//
//	// TODO: Set any other variables?
//}

LINK_ENTITY_TO_CLASS(kwDefenseChamber, AvHDefenseChamber);
AvHDefenseChamber::AvHDefenseChamber() : AvHAlienUpgradeBuilding(ALIEN_BUILD_DEFENSE_CHAMBER, TECH_DEFENSE_CHAMBER, kwsDefenseChamber, AVH_USER3_DEFENSE_CHAMBER)
{
}

char* AvHDefenseChamber::GetDeploySound() const
{
	return kChamberDeploySound;
}

char* AvHDefenseChamber::GetModelName() const
{
	return kDefenseChamberModel;
}

void AvHDefenseChamber::SetHasBeenBuilt()
{
	AvHAlienUpgradeBuilding::SetHasBeenBuilt();
	
	SetThink(&AvHDefenseChamber::RegenAliensThink);
	this->pev->nextthink = gpGlobals->time + BALANCE_IVAR(kDefenseChamberThinkInterval);
}

void AvHDefenseChamber::RegenAliensThink()
{
	// Loop through all players
	CBaseEntity* theBaseEntity = NULL;
	int theNumEntsHealed = 0;
	
	while(((theBaseEntity = UTIL_FindEntityInSphere(theBaseEntity, this->pev->origin, BALANCE_IVAR(kDefensiveChamberHealRange))) != NULL) && (theNumEntsHealed < BALANCE_IVAR(kAlienChamberMaxPlayers)))
	{
		if(theBaseEntity->pev->team == this->pev->team)
		{
			AvHBaseBuildable* theBuildable = dynamic_cast<AvHBaseBuildable*>(theBaseEntity);
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theBaseEntity);
			if(thePlayer && thePlayer->IsAlive())
			{
				if(thePlayer->Heal(BALANCE_IVAR(kDefensiveChamberRegenAmount)))
				{
					theNumEntsHealed++;
				}
			}
			else if(theBuildable && theBuildable->GetIsBuilt() && (theBuildable != this))
			{
				if(theBuildable->Regenerate(BALANCE_IVAR(kDefensiveChamberRegenAmount)))
				{
					theNumEntsHealed++;
				}
			}
		}
	}
	
	// Set next think
	this->pev->nextthink = gpGlobals->time + BALANCE_IVAR(kDefenseChamberThinkInterval);
	
	// Play a random idle animation
	int theIdle = this->GetIdle1Animation();
	
	if(RANDOM_LONG(0, 1))
	{
		theIdle = this->GetIdle2Animation();
	}
	
	this->PlayAnimationAtIndex(theIdle);
}


AvHAlienUpgradeCategory	AvHDefenseChamber::GetUpgradeCategory() const
{
	return ALIEN_UPGRADE_CATEGORY_DEFENSE;
}




LINK_ENTITY_TO_CLASS(kwSensoryChamber, AvHSensoryChamber);
AvHSensoryChamber::AvHSensoryChamber() : AvHAlienUpgradeBuilding(ALIEN_BUILD_SENSORY_CHAMBER, TECH_SENSORY_CHAMBER, kwsSensoryChamber, AVH_USER3_SENSORY_CHAMBER)
{
}
	
char* AvHSensoryChamber::GetDeploySound() const
{
	return kChamberDeploySound;
}

bool AvHSensoryChamber::GetIsEntityInSight(CBaseEntity* inEntity)
{
	bool theEntityIsInSight = false;

	// Do a traceline from sensory chamber to entity
	TraceResult tr;
	UTIL_TraceLine(this->pev->origin, inEntity->pev->origin, ignore_monsters, dont_ignore_glass, this->edict(), &tr);
	if(tr.flFraction == 1.0f)
	{
		theEntityIsInSight = true;
	}

	return theEntityIsInSight;
}

char* AvHSensoryChamber::GetModelName() const
{
	return kSensoryChamberModel;
}



AvHAlienUpgradeCategory	AvHSensoryChamber::GetUpgradeCategory() const
{
	return ALIEN_UPGRADE_CATEGORY_SENSORY;
}

void AvHSensoryChamber::Precache()
{
	AvHAlienUpgradeBuilding::Precache();

	//PRECACHE_UNMODIFIED_SOUND(kParasiteHitSound);
}

void AvHSensoryChamber::SetHasBeenBuilt()
{
	AvHAlienUpgradeBuilding::SetHasBeenBuilt();
}





LINK_ENTITY_TO_CLASS(kwMovementChamber, AvHMovementChamber);
AvHMovementChamber::AvHMovementChamber() : AvHAlienUpgradeBuilding(ALIEN_BUILD_MOVEMENT_CHAMBER, TECH_MOVEMENT_CHAMBER, kwsMovementChamber, AVH_USER3_MOVEMENT_CHAMBER)
{
	this->mLastTimeScannedHives = -1;
	this->mTeleportHiveIndex = -1;
}

char* AvHMovementChamber::GetDeploySound() const
{
	return kChamberDeploySound;
}

char* AvHMovementChamber::GetModelName() const
{
	return kMovementChamberModel;
}

void AvHMovementChamber::ResetEntity()
{
	AvHAlienUpgradeBuilding::ResetEntity();

	this->mLastTimeScannedHives = -1;
	this->mTeleportHiveIndex = -1;
}


AvHAlienUpgradeCategory	AvHMovementChamber::GetUpgradeCategory() const
{
	return ALIEN_UPGRADE_CATEGORY_MOVEMENT;
}

void AvHMovementChamber::Precache()
{
	AvHAlienUpgradeBuilding::Precache();

	PRECACHE_UNMODIFIED_SOUND(kAlienSightOffSound);
	PRECACHE_UNMODIFIED_SOUND(kAlienSightOnSound);
	PRECACHE_UNMODIFIED_SOUND(kAlienEnergySound);
}

void AvHMovementChamber::SetHasBeenBuilt()
{
	AvHAlienUpgradeBuilding::SetHasBeenBuilt();
	
	// TODO: Include a "warm-up" time so movement chambers don't teleport the builder immediately
	//SetThink(&AvHMovementChamber::TeleportUseThink);

	SetThink(&AvHMovementChamber::EnergyAliensThink);
	this->pev->nextthink = gpGlobals->time + BALANCE_IVAR(kMovementChamberThinkInterval);
}

void AvHMovementChamber::EnergyAliensThink()
{
	// Don't teleport until it's "warmed up"
	SetUse(&AvHMovementChamber::TeleportUse);

	// Loop through all players
	CBaseEntity* theBaseEntity = NULL;
	int theNumEntsProcessed = 0;
	
	while(((theBaseEntity = UTIL_FindEntityInSphere(theBaseEntity, this->pev->origin, BALANCE_IVAR(kMovementChamberEnergyRange))) != NULL) && (theNumEntsProcessed < BALANCE_IVAR(kAlienChamberMaxPlayers)))
	{
		if(theBaseEntity->pev->team == this->pev->team)
		{
			float theEnergizeAmount = BALANCE_FVAR(kMovementChamberEnergyAmount);
			AvHBaseBuildable* theBuildable = dynamic_cast<AvHBaseBuildable*>(theBaseEntity);
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theBaseEntity);
			if(thePlayer && thePlayer->IsAlive())
			{
				if(thePlayer->Energize(theEnergizeAmount))
				{
					theNumEntsProcessed++;
				}
			}
			// Energize alien buildables
//			else if(theBuildable)
//			{
//				if(theBuildable->Energize(theEnergizeAmount))
//				{
//					theNumEntsProcessed++;
//				}
//			}
		}
	}

	// Play an animation (spin the arms if energizing)
	int theIdle = this->GetIdle2Animation();

	// Play sound
	if(theNumEntsProcessed > 0)
	{
		EMIT_SOUND(this->edict(), CHAN_AUTO, kAlienEnergySound, 1.0f, ATTN_NORM);

		theIdle = this->GetIdle1Animation();
	}

	this->PlayAnimationAtIndex(theIdle);
	
	// Set next think
	this->pev->nextthink = gpGlobals->time + BALANCE_IVAR(kMovementChamberThinkInterval);
}



void AvHMovementChamber::TeleportUse(CBaseEntity* inActivator, CBaseEntity* inCaller, USE_TYPE inUseType, float inValue)
{
	const float kHiveScanInterval = 1.0f;

	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inActivator);
	if(thePlayer && (thePlayer->pev->team == this->pev->team) && (thePlayer->GetUser3() != AVH_USER3_ALIEN_EMBRYO))
	{
		if((this->mLastTimeScannedHives == -1) || (gpGlobals->time > (this->mLastTimeScannedHives + kHiveScanInterval)))
		{
			this->mTeleportHiveIndex = -1;
			float theFarthestDistance = 0.0f; //sqrt((kMaxMapDimension*2)*(kMaxMapDimension*2));
			bool theIsDone = false;
		
			// Loop through the hives for this team, look for the farthest one (hives under attack take precedence)
			FOR_ALL_ENTITIES(kesTeamHive, AvHHive*)
				if((theEntity->pev->team == this->pev->team) && !theIsDone)
				{
					bool theHiveIsUnderAttack = GetGameRules()->GetIsEntityUnderAttack(theEntity->entindex());

					if(theEntity->GetIsActive() || theHiveIsUnderAttack)
					{
						float theCurrentDistance = VectorDistance(theEntity->pev->origin, inActivator->pev->origin);
						bool theHiveIsFarther = (theCurrentDistance > theFarthestDistance);

						// Undefined which attacked hive is chosen if multiples are under attack
						if((this->mTeleportHiveIndex == -1) || theHiveIsFarther || theHiveIsUnderAttack)
						{
							this->mTeleportHiveIndex = theEntity->entindex();
							theFarthestDistance = theCurrentDistance;

							// If there's a hive under attack, we're done
							if(theHiveIsUnderAttack)
							{
								theIsDone = true;
							}
						}
					}
				}
			END_FOR_ALL_ENTITIES(kesTeamHive)
		
			this->mLastTimeScannedHives = gpGlobals->time;
		}
		
		// If we have a valid hive index, jump the player to it
		if(this->mTeleportHiveIndex != -1)
		{
			// Play sound at this entity
			EMIT_SOUND(this->edict(), CHAN_AUTO, kAlienSightOnSound, 1.0f, ATTN_NORM);
		
			// Move him to it!
			AvHHive* theHive = NULL;
			AvHSUGetEntityFromIndex(this->mTeleportHiveIndex, theHive);
			if(theHive)
			{
				CBaseEntity* theSpawnEntity = GetGameRules()->GetRandomHiveSpawnPoint(thePlayer, theHive->pev->origin, theHive->GetMaxSpawnDistance());
				if(theSpawnEntity)
				{
					Vector theMinSize;
					Vector theMaxSize;
					thePlayer->GetSize(theMinSize, theMaxSize);

					int theOffset = AvHMUGetOriginOffsetForUser3(AvHUser3(thePlayer->pev->iuser3));
					Vector theOriginToSpawn = theSpawnEntity->pev->origin;
					theOriginToSpawn.z += theOffset;

					if(AvHSUGetIsEnoughRoomForHull(theOriginToSpawn, AvHMUGetHull(false, thePlayer->pev->iuser3), thePlayer->edict()))
					{
						thePlayer->SetPosition(theOriginToSpawn);
						thePlayer->pev->velocity = Vector(0, 0, 0);

						// Play teleport sound before and after
						EMIT_SOUND(inActivator->edict(), CHAN_AUTO, kAlienSightOffSound, 1.0f, ATTN_NORM);
					}
				}
			}
		}
	}
}

void AvHMovementChamber::TeleportUseThink()
{
	// Set our new use, don't set next think
	SetUse(&AvHMovementChamber::TeleportUse);

	// Start animating
	SetThink(&AvHBaseBuildable::AnimateThink);
	this->pev->nextthink = gpGlobals->time + .1f;
}



