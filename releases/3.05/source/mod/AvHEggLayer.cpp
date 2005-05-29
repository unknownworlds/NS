//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHEggLayer.cpp $
// $Date: 2002/07/24 19:09:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHEggLayer.cpp,v $
// Revision 1.7  2002/07/24 19:09:16  Flayra
// - Linux issues
//
// Revision 1.6  2002/07/24 18:55:51  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.5  2002/06/03 16:39:09  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.4  2002/05/23 02:33:41  Flayra
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
#include "mod/AvHServerUtil.h"

LINK_ENTITY_TO_CLASS(kwEggLayer, AvHEggLayer);

BOOL AvHEggLayer::Deploy()
{
	return DefaultDeploy(kEggLayerVModel, kEggLayerPModel, this->GetDeployAnimation(), kEggLayerAnimExt);
}


int	AvHEggLayer::GetBarrelLength() const
{
	return kEggLayerBarrelLength;
}

bool AvHEggLayer::GetFiresUnderwater() const
{
	return true;
}

bool AvHEggLayer::GetIsDroppable() const
{
	return false;
}
	
void AvHEggLayer::Init()
{
	this->mRange = kEggLayerRange;
	this->mROF = kEggLayerROF;
}

void AvHEggLayer::Precache(void)
{
	AvHAlienWeapon::Precache();
	
	PRECACHE_MODEL(kEggLayerVModel);
	PRECACHE_MODEL(kEggLayerPModel);
	PRECACHE_MODEL(kAlienGunWModel);

	PRECACHE_SOUND(kEggLaySound1);
	PRECACHE_SOUND(kEggLaySound2);
	
	this->mEvent = PRECACHE_EVENT(1, kEggLayerShootEventName);
	//this->mEvent = PRECACHE_EVENT(1, kSpitGEventName);
}

void AvHEggLayer::Spawn()
{
    AvHAlienWeapon::Spawn(); 
	
	Precache();
	
	this->m_iId = AVH_WEAPON_EGGLAYER;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsEggLayer);
	
	SET_MODEL(ENT(this->pev), kAlienGunWModel);
	
	FallInit();// get ready to fall down.
	
}

bool AvHEggLayer::UsesAmmo(void) const
{
	return false;
}

void AvHEggLayer::FireProjectiles(void)
{
	#ifdef AVH_SERVER
	// Make sure we have enough points to shoot this thing
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
	ASSERT(thePlayer);
	
	if(thePlayer->GetResources() > kEggLayerPointCost)
	{
		// Now check to make sure the space is big enough to hold the egg
		UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);
		
		//Vector vecAiming = gpGlobals->v_forward;
		Vector vecSrc = this->m_pPlayer->GetGunPosition();// + vecAiming;

		if(AvHSUIsAreaFree(vecSrc, kEggRadius, this->m_pPlayer))
		{
			// Decrement kEggLayerPointCost points
			thePlayer->SetResources(thePlayer->GetResources() - kEggLayerPointCost);
			
			AvHEgg* theEgg = GetClassPtr((AvHEgg*)NULL );
			theEgg->Spawn();
			
			theEgg->pev->angles.x = 0;
			theEgg->pev->angles.z = 0;
			//theEgg->pev->velocity = gpGlobals->v_forward * 300 + gpGlobals->v_forward * 100;

			UTIL_SetOrigin(theEgg->pev, vecSrc);
			
			// Set owner
			//theEgg->pev->owner = ENT(this->m_pPlayer->pev);
			
			// Set egg's team
			theEgg->pev->team = this->m_pPlayer->pev->team;
		}
	}
	#endif	
}

