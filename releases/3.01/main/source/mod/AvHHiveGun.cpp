//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHHiveGun.cpp $
// $Date: 2002/07/24 19:09:17 $
//
//-------------------------------------------------------------------------------
// $Log: AvHHiveGun.cpp,v $
// Revision 1.8  2002/07/24 19:09:17  Flayra
// - Linux issues
//
// Revision 1.7  2002/07/24 18:55:52  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.6  2002/07/24 18:45:41  Flayra
// - Linux and scripting changes
//
// Revision 1.5  2002/06/25 18:00:56  Flayra
// - Removed this
//
// Revision 1.4  2002/06/10 19:47:16  Flayra
// - New level 2 view model
//
// Revision 1.3  2002/06/03 16:39:09  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.2  2002/05/23 02:33:42  Flayra
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
#include "util/MathUtil.h"
#include "mod/AvHTitles.h"
#include "mod/AvHHive.h"

#ifdef AVH_SERVER
#include "mod/AvHSharedUtil.h"
#endif

//LINK_ENTITY_TO_CLASS(kwHiveGun, AvHHiveGun);
//
//int	AvHHiveGun::GetBarrelLength() const
//{
//	return kBuildingGunBarrelLength;
//}
//
//bool AvHHiveGun::GetFiresUnderwater() const
//{
//	return true;
//}
//
//bool AvHHiveGun::GetIsDroppable() const
//{
//	return false;
//}
//
//void AvHHiveGun::FireProjectiles(void)
//{
//#ifdef AVH_SERVER
//	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
//	ASSERT(thePlayer);
//	
//	// See if there is an inactive hive within range
//	UTIL_MakeVectors(thePlayer->pev->v_angle);
//	
//	Vector theStart = thePlayer->GetGunPosition();
//	Vector theEnd = theStart + gpGlobals->v_forward*this->GetRange(); 
//
//	// Collide with world to find potential build site
//	TraceResult theTR;
//	UTIL_TraceLine(theStart, theEnd, dont_ignore_monsters, thePlayer->edict(), &theTR);
//	
//	Vector theLocation = theTR.vecEndPos;
//
//	// Do we have enough points?
//	int thePointCost = GetGameRules()->GetPointCostForMessageID(ALIEN_BUILD_HIVE);
//	if(thePlayer->GetResources() >= thePointCost)
//	{
//		if(AvHSHUGetIsSiteValidForBuild(ALIEN_BUILD_HIVE, &theLocation, thePlayer->edict()))
//		{
//			// Get the hive at this location
//			CBaseEntity* theBaseEntity = NULL;
//			AvHHive* theNearestHive = NULL;
//
//			// Find the nearest hive
//			while((theBaseEntity = UTIL_FindEntityByClassname(theBaseEntity, kesTeamHive)) != NULL)
//			{
//				if(theBaseEntity)
//				{
//					AvHHive* theCurrentHive = dynamic_cast<AvHHive*>(theBaseEntity);
//					if(theCurrentHive)
//					{
//						float theCurrentDistance = VectorDistance(theLocation, theCurrentHive->pev->origin);
//						if(!theNearestHive || (theCurrentDistance < VectorDistance(theLocation, theNearestHive->pev->origin)))
//						{
//							theNearestHive = theCurrentHive;
//						}
//					}
//				}
//			}
//
//			if(theNearestHive)
//			{
//				// If so, set it as growing
//				theNearestHive->StartSpawningForTeam(thePlayer->GetTeam());
//
//				// Decrement points
//				thePlayer->SetResources(thePlayer->GetResources() - thePointCost);
//			}
//		}
//		else
//		{
//			thePlayer->SendMessage(kHelpTextEmptyHiveNotNearby, true);
//		}
//	}
//	else
//	{
//		thePlayer->PlayHUDSound(HUD_SOUND_ALIEN_MORE);
//	}
//#endif
//}
//
//void AvHHiveGun::Init()
//{
//	this->mRange = kBuildingGunRange;
//	this->mROF = kBuildingGunROF;
//}	
//
//char* AvHHiveGun::GetViewModel() const
//{
//	return kLevel2ViewModel;
//}
//
//void AvHHiveGun::Precache(void)
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
//
//void AvHHiveGun::Spawn()
//{
//	Precache();
//
//	AvHAlienWeapon::Spawn();
//	
//	this->m_iId = AVH_WEAPON_HIVE;
//	
//	// Set our class name
//	this->pev->classname = MAKE_STRING(kwsHiveGun);
//	
//	SET_MODEL(ENT(this->pev), kNullModel);
//	
//	FallInit();// get ready to fall down.
//}
//
//bool AvHHiveGun::UsesAmmo(void) const
//{
//	return false;
//}

