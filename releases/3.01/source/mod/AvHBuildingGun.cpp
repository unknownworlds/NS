//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHBuildingGun.cpp$
// $Date: 2002/07/24 18:55:51 $
//
//-------------------------------------------------------------------------------
// $Log: AvHBuildingGun.cpp,v $
// Revision 1.10  2002/07/24 18:55:51  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.9  2002/07/24 18:45:40  Flayra
// - Linux and scripting changes
//
// Revision 1.8  2002/06/25 17:43:21  Flayra
// - Removed this
//
// Revision 1.7  2002/06/10 19:47:16  Flayra
// - New level 2 view model
//
// Revision 1.6  2002/06/03 16:39:09  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.5  2002/05/28 17:37:38  Flayra
// - Don't perform max # check for offense chambers (only for upgrade buildings)
//
// Revision 1.4  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHAlienWeapons.h"
#include "mod/AvHPlayer.h"

#ifdef AVH_CLIENT
#include "cl_dll/eventscripts.h"
#include "cl_dll/in_defs.h"
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#endif

#include "common/hldm.h"
#include "common/event_api.h"
#include "common/event_args.h"
#include "common/vector_util.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHTitles.h"

#ifdef AVH_SERVER
#include "mod/AvHSharedUtil.h"
#endif

//int	AvHBuildingGun::GetBarrelLength() const
//{
//	return kBuildingGunBarrelLength;
//}
//
//int	AvHBuildingGun::GetDeployAnimation() const
//{
//	// Look at most recently used weapon and see if we can transition from it
//	int theDeployAnimation = 7;
//
//	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
//
//	switch(thePreviousID)
//	{
//	case AVH_WEAPON_SPIT:
//		theDeployAnimation = 4;
//		break;
//	case AVH_WEAPON_WEBSPINNER:
//	case AVH_WEAPON_RESOURCE_TOWER:
//	case AVH_WEAPON_OFFENSE_CHAMBER:
//	case AVH_WEAPON_DEFENSE_CHAMBER:
//	case AVH_WEAPON_SENSORY_CHAMBER:
//	case AVH_WEAPON_MOVEMENT_CHAMBER:
//	case AVH_WEAPON_HIVE:
//		theDeployAnimation = -1;
//		break;
//	}
//
//	return theDeployAnimation;
//}
//
//float AvHBuildingGun::GetDeployTime() const
//{
//	return 1.0f;
//}
//
//int	AvHBuildingGun::GetIdleAnimation() const
//{
//	int theBaseOffset = 0;
//	int theAnimation = theBaseOffset + UTIL_SharedRandomFloat(this->m_pPlayer->random_seed, 0, 2);
//	return theAnimation;
//}
//
//int	AvHBuildingGun::GetShootAnimation() const
//{
//	return 5;
//}
//
//bool AvHBuildingGun::GetFiresUnderwater() const
//{
//	return true;
//}
//
//bool AvHBuildingGun::GetIsDroppable() const
//{
//	return false;
//}
//	
//void AvHBuildingGun::Init()
//{
//	this->mRange = kBuildingGunRange;
//	this->mROF = kBuildingGunROF;
//}
//
//char* AvHBuildingGun::GetViewModel() const
//{
//	return kLevel2ViewModel;
//}
//
//void AvHBuildingGun::Precache(void)
//{
//	AvHAlienWeapon::Precache();
//	
//	PRECACHE_SOUND(kBuildingGunSound1);
//	PRECACHE_SOUND(kBuildingGunSound2);
//	
//	this->mEvent = PRECACHE_EVENT(1, kBuildingGunEventName);
//	//this->mEvent = PRECACHE_EVENT(1, kSpitGEventName);
//}
//
//void AvHBuildingGun::Spawn()
//{
//    AvHAlienWeapon::Spawn(); 
//}
//
//bool AvHBuildingGun::UsesAmmo(void) const
//{
//	return false;
//}
//
//CBaseEntity* AvHBuildingGun::CreateBuilding(const Vector& inLocation) const
//{
//	Vector theAngles;
//	CBaseEntity* theEntity = CBaseEntity::Create(this->GetBuildingClassName(), inLocation, theAngles);
//	ASSERT(theEntity);
//	return theEntity;
//}
//
//int	AvHBuildingGun::GetMaxAllowed() const
//{
//	return 3;
//}
//
//void AvHBuildingGun::FireProjectiles(void)
//{
//	#ifdef AVH_SERVER
//	// Make sure we have enough points to shoot this thing
//	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
//	ASSERT(thePlayer);
//
//	int theBuildingPointCost = this->GetBuildingPointCost();
//
//	// Make sure we haven't exceeded the limit
//	int theNumBuildings = 0;
//	FOR_ALL_ENTITIES(this->GetBuildingClassName(), CBaseEntity*)
//		if(theEntity->pev->team == thePlayer->pev->team)
//		{
//			theNumBuildings++;
//		}
//	END_FOR_ALL_ENTITIES(this->GetBuildingClassName());
//
//	int theMaxBuildings = this->GetMaxAllowed();
//	if(!strcmp(this->GetBuildingClassName(), kwsOffenseChamber))
//	{
//		theMaxBuildings = -1;
//	}
//
//	if((theMaxBuildings == -1) || (theNumBuildings < theMaxBuildings))
//	{
//		if(thePlayer->GetResources() > theBuildingPointCost)
//		{
//			// Now check to make sure the space is big enough to hold the building
//			UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);
//		
//			Vector theStart = this->m_pPlayer->GetGunPosition();
//			Vector theEnd = theStart + gpGlobals->v_forward*this->GetRange(); 
//		
//			// Collide with world to find potential build site
//			TraceResult theTR;
//			UTIL_TraceLine(theStart, theEnd, dont_ignore_monsters, thePlayer->edict(), &theTR);
//		
//			Vector theLocation = theTR.vecEndPos;
//		
//			// Check if collision point is valid for building
//			if(AvHSHUGetIsSiteValidForBuild(this->GetBuildingMessageID(), &theLocation, thePlayer->edict()))
//			//if(AvHSUIsAreaFree(vecSrc, this->GetBuildingRadius(), this->m_pPlayer))
//			{
//				// Decrement theBuildingPointCost points
//				thePlayer->SetResources(thePlayer->GetResources() - theBuildingPointCost);
//		
//				// Create the new building
//				CBaseEntity* theEntity = this->CreateBuilding(theLocation);
//		
//				AvHSUBuildingJustCreated(this->GetBuildingMessageID(), theEntity, thePlayer);
//				
//				// Set owner (this prevents collisions between the entity and it's owner though)
//				//theEntity->pev->owner = ENT(this->m_pPlayer->pev);
//				
//				// Set building's team
//				theEntity->pev->team = this->m_pPlayer->pev->team;
//			}
//		}
//		else
//		{
//			thePlayer->PlayHUDSound(HUD_SOUND_ALIEN_MORE);
//		}
//	}
//	else
//	{
//		thePlayer->SendMessage(kTooManyBuildings, true);
//	}
//#endif	
//}
//
//// Build distance for all buildings
//float AvHBuildingGun::GetRange() const
//{
//	return 48;
//}
//
//bool AvHBuildingGun::GetMustPressTriggerForEachShot() const
//{
//	return true;
//}
//
//
//
//LINK_ENTITY_TO_CLASS(kwResourceTowerGun, AvHResourceTowerGun);
//
//char* AvHResourceTowerGun::GetBuildingClassName() const
//{
//	return kwsAlienResourceTower;
//}
//
//AvHMessageID AvHResourceTowerGun::GetBuildingMessageID() const
//{
//	return ALIEN_BUILD_RESOURCES;	
//}
//
//int	AvHResourceTowerGun::GetBuildingPointCost() const
//{
//	int theCost = 0;
//
//	#ifdef AVH_SERVER
//	theCost = GetGameRules()->GetPointCostForMessageID(this->GetBuildingMessageID());
//	#endif
//
//	return theCost;
//}
//
//int AvHResourceTowerGun::GetBuildingRadius() const
//{
//	return 40;
//}
//
//int	AvHResourceTowerGun::GetMaxAllowed() const
//{
//	return -1;
//}
//
//void AvHResourceTowerGun::Spawn()
//{
//	AvHBuildingGun::Spawn(); 
//	
//	Precache();
//	
//	this->m_iId = AVH_WEAPON_RESOURCE_TOWER;
//	
//	  // Set our class name
//	this->pev->classname = MAKE_STRING(kwsResourceTowerGun);
//	
//	SET_MODEL(ENT(this->pev), kNullModel);
//	
//	FallInit();// get ready to fall down.
//}
//
//
//
//
//
//
//LINK_ENTITY_TO_CLASS(kwOffenseChamberGun, AvHOffenseChamberGun);
//
//char* AvHOffenseChamberGun::GetBuildingClassName() const
//{
//	return kwsOffenseChamber;
//}
//
//AvHMessageID AvHOffenseChamberGun::GetBuildingMessageID() const
//{
//	return ALIEN_BUILD_OFFENSE_CHAMBER;	
//}
//
//int	AvHOffenseChamberGun::GetBuildingPointCost() const
//{
//	return 5;
//}
//
//int AvHOffenseChamberGun::GetBuildingRadius() const
//{
//	return 40;
//}
//
//void AvHOffenseChamberGun::Spawn()
//{
//	AvHBuildingGun::Spawn(); 
//	
//	Precache();
//	
//	this->m_iId = AVH_WEAPON_OFFENSE_CHAMBER;
//	
//	// Set our class name
//	this->pev->classname = MAKE_STRING(kwsOffenseChamberGun);
//	
//	SET_MODEL(ENT(this->pev), kNullModel);
//	
//	FallInit();// get ready to fall down.
//}
//
//
//
//
//
//
//LINK_ENTITY_TO_CLASS(kwDefenseChamberGun, AvHDefenseChamberGun);
//
//char* AvHDefenseChamberGun::GetBuildingClassName() const
//{
//	return kwsDefenseChamber;
//}
//
//AvHMessageID AvHDefenseChamberGun::GetBuildingMessageID() const
//{
//	return ALIEN_BUILD_DEFENSE_CHAMBER;	
//}
//
//int	AvHDefenseChamberGun::GetBuildingPointCost() const
//{
//	return 5;
//}
//
//int AvHDefenseChamberGun::GetBuildingRadius() const
//{
//	return 40;
//}
//
//void AvHDefenseChamberGun::Spawn()
//{
//	AvHBuildingGun::Spawn(); 
//	
//	Precache();
//	
//	this->m_iId = AVH_WEAPON_DEFENSE_CHAMBER;
//	
//	// Set our class name
//	this->pev->classname = MAKE_STRING(kwsDefenseChamberGun);
//	
//	SET_MODEL(ENT(this->pev), kNullModel);
//	
//	FallInit();// get ready to fall down.
//}
//
//
//
//
//LINK_ENTITY_TO_CLASS(kwSensoryChamberGun, AvHSensoryChamberGun);
//
//char* AvHSensoryChamberGun::GetBuildingClassName() const
//{
//	return kwsSensoryChamber;
//}
//
//AvHMessageID AvHSensoryChamberGun::GetBuildingMessageID() const
//{
//	return ALIEN_BUILD_SENSORY_CHAMBER;	
//}
//
//int	AvHSensoryChamberGun::GetBuildingPointCost() const
//{
//	return 5;
//}
//
//int AvHSensoryChamberGun::GetBuildingRadius() const
//{
//	return 40;
//}
//
//void AvHSensoryChamberGun::Spawn()
//{
//	AvHBuildingGun::Spawn(); 
//	
//	Precache();
//	
//	this->m_iId = AVH_WEAPON_SENSORY_CHAMBER;
//	
//	// Set our class name
//	this->pev->classname = MAKE_STRING(kwsSensoryChamberGun);
//	
//	SET_MODEL(ENT(this->pev), kNullModel);
//	
//	FallInit();// get ready to fall down.
//}
//
//
//
//
//LINK_ENTITY_TO_CLASS(kwMovementChamberGun, AvHMovementChamberGun);
//
//char* AvHMovementChamberGun::GetBuildingClassName() const
//{
//	//return kwsDeployedTurret;
//	return kwsMovementChamber;
//}
//
//AvHMessageID AvHMovementChamberGun::GetBuildingMessageID() const
//{
//	return ALIEN_BUILD_MOVEMENT_CHAMBER;	
//}
//
//int	AvHMovementChamberGun::GetBuildingPointCost() const
//{
//	return 5;
//}
//
//int AvHMovementChamberGun::GetBuildingRadius() const
//{
//	return 40;
//}
//
//void AvHMovementChamberGun::Spawn()
//{
//	AvHBuildingGun::Spawn(); 
//	
//	Precache();
//	
//	this->m_iId = AVH_WEAPON_MOVEMENT_CHAMBER;
//	
//	// Set our class name
//	this->pev->classname = MAKE_STRING(kwsMovementChamberGun);
//	
//	SET_MODEL(ENT(this->pev), kNullModel);
//	
//	FallInit();// get ready to fall down.
//}







