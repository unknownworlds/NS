//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAlienAbilities.cpp $
// $Date: 2002/11/22 21:28:15 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAlienAbilities.cpp,v $
// Revision 1.13  2002/11/22 21:28:15  Flayra
// - mp_consistency changes
//
// Revision 1.12  2002/10/16 00:43:58  Flayra
// - Removed blink fail event
//
// Revision 1.11  2002/09/23 22:06:33  Flayra
// - Updated anims for new view model artwork
//
// Revision 1.10  2002/08/09 00:52:51  Flayra
// - Speed up deploying of charge and leap, removed old hard-coded number
//
// Revision 1.9  2002/07/26 23:03:08  Flayra
// - New artwork
//
// Revision 1.8  2002/07/23 16:54:36  Flayra
// - Updates for new viewmodel artwork
//
// Revision 1.7  2002/07/08 16:42:38  Flayra
// - Refactoring for cheat protection, moved blinking in here from separate class
//
// Revision 1.6  2002/06/25 17:25:56  Flayra
// - Regular update for leap and charge
//
// Revision 1.5  2002/06/10 19:49:06  Flayra
// - Updated with new alien view model artwork (with running anims)
//
// Revision 1.4  2002/06/03 16:20:11  Flayra
// - Proper anims for alien abilities
//
// Revision 1.3  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHAlienAbilities.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHAlienAbilityConstants.h"
#include "mod/AvHConstants.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHPlayer.h"

#ifdef AVH_CLIENT
#include "cl_dll/hud.h"
#include "mod/AvHHud.h"
extern int g_runfuncs;
void IN_Attack2Down();
void IN_Attack2Up();
bool CheckInAttack();
#endif

LINK_ENTITY_TO_CLASS(kwLeap, AvHLeap);

BOOL AvHLeap::Deploy()
{
	BOOL theSuccess = DefaultDeploy(kLevel1ViewModel, kLeapPModel, this->GetDeployAnimation(), this->GetAnimationExtension());

	// Super-fast deploy time
	this->m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + this->GetDeployTime();

	return theSuccess;
}

AvHMessageID AvHLeap::GetAbilityImpulse() const
{
	return ALIEN_ABILITY_LEAP;
}

int	AvHLeap::GetBarrelLength() const
{
	return 0;
}

int	AvHLeap::GetDeployAnimation() const
{
	return 6;
}

float AvHLeap::GetDeployTime() const
{
	// Supa-fast!
	return 0.0f;
}

int	AvHLeap::GetIdleAnimation() const
{
	// TODO: Add idle here
	return 0;
}

int AvHLeap::GetShootAnimation() const
{
	return 3;
}

bool AvHLeap::GetFiresUnderwater() const
{
	return true;
}
	
bool AvHLeap::GetIsDroppable() const
{
	return false;
}

void AvHLeap::Precache(void)
{
	AvHAlienAbilityWeapon::Precache();
	
	PRECACHE_UNMODIFIED_MODEL(kLevel1ViewModel);
	PRECACHE_UNMODIFIED_MODEL(kLeapPModel);
	
	PRECACHE_UNMODIFIED_SOUND(kLeapSound);
	PRECACHE_UNMODIFIED_SOUND(kLeapHitSound1);
	PRECACHE_UNMODIFIED_SOUND(kLeapKillSound);

	this->mLeapEvent = PRECACHE_EVENT(1, kLeapEventName);	
	this->mAbilityEvent = PRECACHE_EVENT(1, kAbilityEventName);
}

void AvHLeap::Spawn()
{
    AvHAlienAbilityWeapon::Spawn(); 
	
	Precache();
	
	this->m_iId = AVH_ABILITY_LEAP;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsLeap);

	SET_MODEL(ENT(this->pev), kNullModel);
	
	FallInit();// get ready to fall down.
}

float AvHLeap::GetRateOfFire(void) const
{
	return (float)BALANCE_VAR(kLeapROF);// * 0.5f;
}

bool AvHLeap::UsesAmmo(void) const
{
	return false;
}

void AvHLeap::SecondaryAttack()
{
#ifdef AVH_CLIENT
	this->FireProjectiles();
#endif
}

void AvHLeap::FireProjectiles(void)
{
#ifdef AVH_SERVER
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
	if(thePlayer)
	{
		thePlayer->TriggerUncloak();
	}
#endif
#ifdef AVH_CLIENT
	if(g_runfuncs)
	{
		//IN_Attack2Down();
		//CBasePlayerWeapon::SendWeaponAnim(3);
		gHUD.SetAlienAbility(this->GetAbilityImpulse());
	}
#endif
}

void AvHLeap::Init()
{

}

LINK_ENTITY_TO_CLASS(kwCharge, AvHCharge);

BOOL AvHCharge::Deploy()
{
	BOOL theSuccess = DefaultDeploy(kLevel5ViewModel, kNullModel, this->GetDeployAnimation(), this->GetAnimationExtension());
	
	// Super-fast deploy time
	this->m_pPlayer->m_flNextAttack = UTIL_WeaponTimeBase() + this->GetDeployTime();
	
	return theSuccess;
}

AvHMessageID AvHCharge::GetAbilityImpulse() const
{
	return ALIEN_ABILITY_CHARGE;
}

int	AvHCharge::GetBarrelLength() const
{
	return 0;
}

int	AvHCharge::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
	int theDeployAnimation = 5;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
	case AVH_WEAPON_CLAWS:
		theDeployAnimation = 5;
		break;

	case AVH_WEAPON_DEVOUR:
		theDeployAnimation = 18;
		break;

	case AVH_WEAPON_STOMP:
		theDeployAnimation = 15;
		break;
	}
	
	return theDeployAnimation;
}

float AvHCharge::GetDeployTime() const
{
	return 0.0f; //.6f;
}

bool AvHCharge::GetFiresUnderwater() const
{
	return false;
}

int AvHCharge::GetIdleAnimation() const
{
	return 1;
}

bool AvHCharge::GetIsDroppable() const
{
	return false;
}

int AvHCharge::GetShootAnimation() const
{
	return 22;
}

void AvHCharge::Precache(void)
{
	AvHAlienAbilityWeapon::Precache();
	
	PRECACHE_UNMODIFIED_MODEL(kLevel5ViewModel);
	PRECACHE_UNMODIFIED_MODEL(kNullModel);
	
	PRECACHE_UNMODIFIED_SOUND(kChargeSound2);
	PRECACHE_UNMODIFIED_SOUND(kChargeKillSound);
	
	this->mEvent = PRECACHE_EVENT(1, kChargeEventName);
	this->mAbilityEvent = PRECACHE_EVENT(1, kAbilityEventName);
}

void AvHCharge::Spawn()
{
    AvHAlienAbilityWeapon::Spawn(); 
	
	Precache();
	
	this->m_iId = AVH_ABILITY_CHARGE;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsCharge);
	
	SET_MODEL(ENT(this->pev), kNullModel);
	
	FallInit();// get ready to fall down.
}

bool AvHCharge::UsesAmmo(void) const
{
	return false;
}

void AvHCharge::SecondaryAttack()
{
#ifdef AVH_CLIENT
	this->FireProjectiles();
#endif
}

void AvHCharge::FireProjectiles(void)
{
#ifdef AVH_CLIENT
	if (CheckInAttack())
		IN_Attack2Down();
	else
		IN_Attack2Up();

	//gHUD.SetAlienAbility(this->GetAbilityImpulse());
#endif

	// Event is played back.  Mark pmove with proper flag so the alien Charges forward.
	//PLAYBACK_EVENT_FULL(0, this->m_pPlayer->edict(), this->mAbilityEvent, 0, this->m_pPlayer->pev->origin, (float *)&g_vecZero, 0.0, 0.0, this->GetAbilityImpulse(), 0, 1, 0 );
	
	// Send fire anim
	//SendWeaponAnim(5);
	//this->PlaybackEvent(this->mWeaponAnimationEvent, 5);
}

void AvHCharge::Init()
{
}

float AvHCharge::GetRateOfFire() const
{
	// Approximate length of charge sound
    return 1.0f;
}

