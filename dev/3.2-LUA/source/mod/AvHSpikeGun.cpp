//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHSpikeGun.cpp $
// $Date: 2002/11/22 21:28:17 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSpikeGun.cpp,v $
// Revision 1.11  2002/11/22 21:28:17  Flayra
// - mp_consistency changes
//
// Revision 1.10  2002/09/23 22:33:11  Flayra
// - Fixed problem where spikes were playing inappropriate ricochet sounds (and too many of them)
//
// Revision 1.9  2002/08/16 02:48:10  Flayra
// - New damage model
//
// Revision 1.8  2002/07/24 19:09:17  Flayra
// - Linux issues
//
// Revision 1.7  2002/07/24 18:45:43  Flayra
// - Linux and scripting changes
//
// Revision 1.6  2002/06/25 17:50:59  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.5  2002/06/03 16:39:10  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.4  2002/05/23 02:32:57  Flayra
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

LINK_ENTITY_TO_CLASS(kwSpikeGun, AvHSpikeGun);
void V_PunchAxis( int axis, float punch );

void AvHSpikeGun::Init()
{
	this->mRange = kSpikeRange;
	this->mDamage = BALANCE_VAR(kSpikeDamage);

	//this->mFramesSinceMoreAmmo = 0;
}

void AvHSpikeGun::FireProjectiles(void)
{
	Vector vecSrc = this->m_pPlayer->GetGunPosition();
	Vector vecAiming = this->m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	
    // Fire the bullets and apply damage
    int theRange = this->mRange;
    float theDamage = this->mDamage*AvHPlayerUpgrade::GetAlienRangedDamageUpgrade(this->m_pPlayer->pev->iuser4);

	// NOTE: If these values change, update them in the EV_SpikeGun as well
	//this->m_pPlayer->FireBullets(1, vecSrc, vecAiming, VECTOR_CONE_3DEGREES, theRange, BULLET_MONSTER_9MM, 0, theDamage);
	this->m_pPlayer->FireBulletsPlayer(1, vecSrc, vecAiming, kSpikeSpread, theRange, BULLET_MONSTER_9MM, 0, theDamage, this->m_pPlayer->pev, this->m_pPlayer->random_seed);
}

int	AvHSpikeGun::GetBarrelLength() const
{
	return kSpikeBarrelLength;
}

float AvHSpikeGun::GetRateOfFire() const
{
	return BALANCE_VAR(kSpikeROF);
}

int	AvHSpikeGun::GetDamageType() const
{
	return NS_DMG_NORMAL;
}

int	AvHSpikeGun::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
	int theDeployAnimation = 13;

	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();

	switch(thePreviousID)
	{
	case AVH_WEAPON_BITE2:
		theDeployAnimation = 9;
		break;

	case AVH_WEAPON_UMBRA:
	case AVH_WEAPON_SPORES:
	case AVH_WEAPON_PRIMALSCREAM:
		theDeployAnimation = 11;
		break;
	}

	return theDeployAnimation;
}

float AvHSpikeGun::GetDeployTime() const
{
	return .6f;
}

bool AvHSpikeGun::GetFiresUnderwater() const
{
	return true;
}

int	AvHSpikeGun::GetIdleAnimation() const
{
	return 0;
}

bool AvHSpikeGun::GetIsDroppable() const
{
	return false;
}

int	AvHSpikeGun::GetShootAnimation() const
{
	return 3;
}

char* AvHSpikeGun::GetViewModel() const
{
	return kLevel3ViewModel;
}

void AvHSpikeGun::Precache()
{
	AvHAlienWeapon::Precache();

	PRECACHE_UNMODIFIED_SOUND(kSpikeFireSound);
	PRECACHE_UNMODIFIED_MODEL(kSpikeProjectileModel);
	PRECACHE_UNMODIFIED_MODEL(kSpikeGunHitSprite);

	this->mEvent = PRECACHE_EVENT(1, kSpikeShootEventName);
}


void AvHSpikeGun::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 

	Precache();

	this->m_iId = AVH_WEAPON_SPIKE;
	//this->m_iDefaultAmmo = kSpikeMaxClip;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsSpikeGun);

	SET_MODEL(ENT(this->pev), kNullModel);

	FallInit();// get ready to fall down.
} 

// Ammo comes back on it's own
//void AvHSpikeGun::WeaponIdle()
//{
//	if(this->m_iClip < kSpikeMaxClip)
//	{
//		if(++this->mFramesSinceMoreAmmo >= 6)
//		{
//			this->m_iClip++;
//			this->mFramesSinceMoreAmmo = 0;
//		}
//	}
//}
