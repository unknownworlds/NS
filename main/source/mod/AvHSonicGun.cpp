//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHSonicGun.cpp $
// $Date: 2002/11/22 21:28:17 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSonicGun.cpp,v $
// Revision 1.26  2002/11/22 21:28:17  Flayra
// - mp_consistency changes
//
// Revision 1.25  2002/10/24 21:33:52  Flayra
// - Shotgun reloading fixes
//
// Revision 1.24  2002/10/16 20:53:10  Flayra
// - Removed weapon upgrade sounds
//
// Revision 1.23  2002/10/16 01:08:11  Flayra
// - Removed sounds that are being played by anim
//
// Revision 1.22  2002/10/03 18:47:18  Flayra
// - Added heavy view model
//
// Revision 1.21  2002/09/25 20:51:19  Flayra
// - Shotgun updating (oops, this fixes sound problems)
//
// Revision 1.20  2002/09/23 22:32:35  Flayra
// - Fixed problem where damage upgrades sometimes were given to aliens
//
// Revision 1.19  2002/08/31 18:01:03  Flayra
// - Work at VALVe
//
// Revision 1.18  2002/08/16 02:48:10  Flayra
// - New damage model
//
// Revision 1.17  2002/08/09 00:49:34  Flayra
// - Fixed shotgun, so it reloads each shell individually
//
// Revision 1.16  2002/07/24 19:09:17  Flayra
// - Linux issues
//
// Revision 1.15  2002/07/24 18:45:43  Flayra
// - Linux and scripting changes
//
// Revision 1.14  2002/07/08 17:08:52  Flayra
// - Tweaked for bullet spread
//
// Revision 1.13  2002/06/25 17:50:59  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.12  2002/06/03 16:37:31  Flayra
// - Constants and tweaks to make weapon anims and times correct with new artwork, added different deploy times (this should be refactored a bit more)
//
// Revision 1.11  2002/05/28 17:44:35  Flayra
// - Tweak weapon deploy volume, as Valve's sounds weren't normalized, started to use new reload method (reload each shell)
//
// Revision 1.10  2002/05/23 02:33:20  Flayra
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
#include "mod/AvHPlayerUpgrade.h"

// Anim key:
const int kShotgunAnimIdle = 0;
const int kShotgunAnimIdle2 = 1;
const int kShotgunAnimGotoReload = 2;
const int kShotgunAnimReloadShell = 3;
const int kShotgunAnimEndReload = 4;
const int kShotgunAnimShoot = 5;
const int kShotgunAnimShootEmpty = 6;
const int kShotgunAnimDraw = 7;
const float kShotgunAnimDrawLength = .9f;
const float kShotgunGotoReloadLength = 1.1f;
const float kShotgunReloadShellLength = .9f;
const float kShotgunEndReloadLength = 1.8f;

LINK_ENTITY_TO_CLASS(kwShotGun, AvHSonicGun);
void V_PunchAxis( int axis, float punch );

void AvHSonicGun::Init()
{
	this->mRange = kSGRange;
	this->mDamage = BALANCE_VAR(kSGDamage);
}

int	AvHSonicGun::GetBarrelLength() const
{
	return kSGBarrelLength;
}

float AvHSonicGun::GetRateOfFire() const
{
	return BALANCE_VAR(kSGROF);
}

int	AvHSonicGun::GetDamageType() const
{
	//return NS_DMG_PIERCING;
	return NS_DMG_NORMAL;
}

int	AvHSonicGun::GetDeployAnimation() const
{
	return kShotgunAnimDraw;
}

char* AvHSonicGun::GetDeploySound() const
{ 
	//return kSGDeploySound;
	return NULL;
}

float AvHSonicGun::GetDeployTime() const
{
	return kShotgunAnimDrawLength;
}

int	AvHSonicGun::GetEmptyShootAnimation() const
{
	return kShotgunAnimShootEmpty;
}

int AvHSonicGun::GetGotoReloadAnimation() const
{
    return kShotgunAnimGotoReload;
}

float AvHSonicGun::GetGotoReloadAnimationTime() const
{
    return kShotgunGotoReloadLength;
}

int AvHSonicGun::GetShellReloadAnimation() const
{
    return kShotgunAnimReloadShell;
}

float AvHSonicGun::GetShellReloadAnimationTime() const
{
    return kShotgunReloadShellLength;
}

int AvHSonicGun::GetEndReloadAnimation() const
{
    return kShotgunAnimEndReload;
}

float AvHSonicGun::GetEndReloadAnimationTime() const
{
    return kShotgunEndReloadLength;
}

char* AvHSonicGun::GetHeavyViewModel() const
{
	return kSGHVVModel;
}

char* AvHSonicGun::GetPlayerModel() const
{
	return kSGPModel;
}

char* AvHSonicGun::GetViewModel() const
{
	return kSGVModel;
}

char* AvHSonicGun::GetWorldModel() const
{
	return kSGWModel;
}

Vector AvHSonicGun::GetProjectileSpread() const
{
	return kSGSpread;
}

void AvHSonicGun::FireProjectiles(void)
{
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	
	ASSERT(this->m_iPrimaryAmmoType >= 0);
	//int theNumBullets = min(kSGBulletsPerShot, this->m_iClientClip);

	float theDamageMultiplier;
	AvHPlayerUpgrade::GetWeaponUpgrade(this->m_pPlayer->pev->iuser3, this->m_pPlayer->pev->iuser4, &theDamageMultiplier);
	float theDamage = this->mDamage*theDamageMultiplier;
	
    // Fire the bullets and apply damage
	//this->m_pPlayer->FireBullets(kSGBulletsPerShot, vecSrc, vecAiming, this->GetProjectileSpread(), this->mRange, 0, 0, theDamage);
	this->m_pPlayer->FireBulletsPlayer(BALANCE_VAR(kSGBulletsPerShot), vecSrc, vecAiming, this->GetProjectileSpread(), this->mRange, BULLET_PLAYER_BUCKSHOT, 0, theDamage);
}

bool AvHSonicGun::GetHasMuzzleFlash() const
{
	return true;
}

int	AvHSonicGun::GetIdleAnimation() const
{
	int theAnimation = 0;

	int theRandomNum = UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 0, 1);

	switch(theRandomNum)
	{
	case 0:
		theAnimation = kShotgunAnimIdle;
		break;
	case 1:
		theAnimation = kShotgunAnimIdle2;
		break;
	}
	
	return theAnimation;
}

int AvHSonicGun::GetReloadAnimation() const
{
    return kShotgunAnimReloadShell;
}

float AvHSonicGun::GetReloadTime(void) const
{
	//return .3f;
	return .22f;
}

int	AvHSonicGun::GetShootAnimation() const
{
	return kShotgunAnimShoot;
}

void AvHSonicGun::Precache()
{
	AvHMarineWeapon::Precache();
	
	PRECACHE_UNMODIFIED_MODEL(kSGEjectModel);
	PRECACHE_UNMODIFIED_SOUND(kSGFireSound1);
	PRECACHE_UNMODIFIED_SOUND(kSGReloadSound);
	PRECACHE_UNMODIFIED_SOUND(kSGCockSound);
	
	PRECACHE_UNMODIFIED_MODEL(kGenericWallpuff);
	
	this->mEvent = PRECACHE_EVENT(1, kSGEventName);
}

void AvHSonicGun::Spawn() 
{ 
    AvHMarineWeapon::Spawn(); 
	
	Precache();
	
	this->m_iId = AVH_WEAPON_SONIC;
	this->m_iDefaultAmmo = BALANCE_VAR(kSGMaxClip);
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsShotGun);
	
	SET_MODEL(ENT(this->pev), kSGWModel);

	FallInit();// get ready to fall down.
} 

