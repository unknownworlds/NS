//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHHeavyMachineGun.cpp $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHHeavyMachineGun.cpp,v $
// Revision 1.23  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.22  2002/10/18 22:19:22  Flayra
// - Fixed bug where HMG never ran out of ammo
//
// Revision 1.21  2002/10/16 20:53:09  Flayra
// - Removed weapon upgrade sounds
//
// Revision 1.20  2002/10/16 00:56:44  Flayra
// - Removed velocity change and ground restrictions (too confusing and felt buggy)
//
// Revision 1.19  2002/10/03 18:46:20  Flayra
// - Added heavy view model
//
// Revision 1.18  2002/09/23 22:16:31  Flayra
// - HMG can only be fired from the ground, and slows down firer
//
// Revision 1.17  2002/08/09 01:02:00  Flayra
// - Made HMG faster to deploy
//
// Revision 1.16  2002/07/24 19:09:16  Flayra
// - Linux issues
//
// Revision 1.15  2002/07/24 18:45:41  Flayra
// - Linux and scripting changes
//
// Revision 1.14  2002/07/08 16:59:48  Flayra
// - Updated anims and timing for new artwork
//
// Revision 1.13  2002/07/01 21:33:06  Flayra
// - Tweaks for new artwork
//
// Revision 1.12  2002/06/25 17:50:59  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.11  2002/06/03 16:37:31  Flayra
// - Constants and tweaks to make weapon anims and times correct with new artwork, added different deploy times (this should be refactored a bit more)
//
// Revision 1.10  2002/05/28 17:44:58  Flayra
// - Tweak weapon deploy volume, as Valve's sounds weren't normalized
//
// Revision 1.9  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHMarineWeapons.h"
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
#include "mod/AvHMarineWeapons.h"
#include "dlls/util.h"

LINK_ENTITY_TO_CLASS(kwHeavyMachineGun, AvHHeavyMachineGun);
void V_PunchAxis( int axis, float punch );

void AvHHeavyMachineGun::Init()
{
	this->mRange = kHMGRange;
	this->mDamage = BALANCE_IVAR(kHMGDamage);
}

int	AvHHeavyMachineGun::GetBarrelLength() const
{
	return kHMGBarrelLength;
}

float AvHHeavyMachineGun::GetRateOfFire() const
{
	return BALANCE_FVAR(kHMGROF);
}

int	AvHHeavyMachineGun::GetDeployAnimation() const
{
	return 7;
}

char* AvHHeavyMachineGun::GetDeploySound() const
{
	return kHMGDeploySound;
}

float AvHHeavyMachineGun::GetDeployTime() const
{
	return .6f;
}

int AvHHeavyMachineGun::GetDamageType() const
{
	return NS_DMG_PIERCING;
}

bool AvHHeavyMachineGun::GetHasMuzzleFlash() const
{
	return true;
}

int	AvHHeavyMachineGun::GetIdleAnimation() const
{
	const int kMaxRandom = 30;
	int iAnim;
	int theRandomNum = UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 0, kMaxRandom);

	if(theRandomNum == 0)
	{
		// Pet machine gun every once in awhile
		iAnim = 1;
	}
	else if(theRandomNum < kMaxRandom/2)
	{
		iAnim = 0;
	}
	else
	{
		iAnim = 2;
	}
	
	return iAnim;
}


void AvHHeavyMachineGun::FireProjectiles(void)
{
	// Assumes we're on the ground
//	const float kSlowDownScalar = .2f;
//	vec3_t theXYVelocity;
//	VectorCopy(this->m_pPlayer->pev->velocity, theXYVelocity);
//
//	VectorScale(theXYVelocity, kSlowDownScalar, theXYVelocity);
//	theXYVelocity.z = this->m_pPlayer->pev->velocity.z;
//
//	VectorCopy(theXYVelocity, this->m_pPlayer->pev->velocity);

	AvHMarineWeapon::FireProjectiles();
}

char* AvHHeavyMachineGun::GetHeavyViewModel() const
{
	return kHMGHVVModel;
}

char* AvHHeavyMachineGun::GetPlayerModel() const
{
	return kHMGPModel;
}

Vector AvHHeavyMachineGun::GetProjectileSpread() const
{
	return kHMGSpread;
}

char* AvHHeavyMachineGun::GetViewModel() const
{
	return kHMGVModel;
}

char* AvHHeavyMachineGun::GetWorldModel() const
{
	return kHMGWModel;
}

int	AvHHeavyMachineGun::GetReloadAnimation() const
{
	return 3;
}

float AvHHeavyMachineGun::GetReloadTime(void) const
{
	return 6.3f;
}

int	AvHHeavyMachineGun::GetShootAnimation() const
{
	int theShootAnim = 4;

	if(this->m_iClip % 2)
	{
		theShootAnim = 5;
	}

	return theShootAnim;
}

int	AvHHeavyMachineGun::GetEmptyShootAnimation() const
{
	return 6;
}

void AvHHeavyMachineGun::Precache()
{
	AvHMarineWeapon::Precache();

	PRECACHE_UNMODIFIED_MODEL(kHMGEjectModel);

	PRECACHE_UNMODIFIED_SOUND(kHMGFireSound1);
	
	#ifdef AVH_UPGRADE_SOUNDS
	PRECACHE_UNMODIFIED_SOUND(kHMGFireSound2);
	PRECACHE_UNMODIFIED_SOUND(kHMGFireSound3);
	PRECACHE_UNMODIFIED_SOUND(kHMGFireSound4);
	#endif

	PRECACHE_UNMODIFIED_SOUND(kHMGReloadSound);
	
	this->mEvent = PRECACHE_EVENT(1, kHMGEventName);
}

bool AvHHeavyMachineGun::ProcessValidAttack(void)
{
	bool theValidAttack = false;

	if(AvHMarineWeapon::ProcessValidAttack() /*&& (this->m_pPlayer->pev->flags & FL_ONGROUND)*/)
	{
		theValidAttack = true;
	}

	return theValidAttack;
}


void AvHHeavyMachineGun::Spawn() 
{ 
    AvHMarineWeapon::Spawn(); 

	Precache();

	this->m_iId = AVH_WEAPON_HMG;
	this->m_iDefaultAmmo = BALANCE_IVAR(kHMGMaxClip);

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsHeavyMachineGun);

	SET_MODEL(ENT(this->pev), kHMGWModel);

	FallInit();// get ready to fall down.
} 
