//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHMetabolize.cpp $
// $Date: $
//
//-------------------------------------------------------------------------------
// $Log: $
//===============================================================================
#include "mod/AvHAlienWeapons.h"
#include "mod/AvHPlayer.h"

#ifdef AVH_CLIENT
#include "cl_dll/eventscripts.h"
#include "cl_dll/in_defs.h"
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#endif

#ifdef AVH_SERVER
#include "mod/AvHBaseBuildable.h"
#endif

#include "mod/AvHMovementUtil.h"
#include "common/hldm.h"
#include "common/event_api.h"
#include "common/event_args.h"
#include "common/vector_util.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHConstants.h"
#include "mod/AvHSharedUtil.h"

#include "pm_shared/pm_defs.h"
extern playermove_t*	pmove;

LINK_ENTITY_TO_CLASS(kwMetabolize, AvHMetabolize);

void AvHMetabolize::Init()
{
	this->mRange = 0;
//	this->mTimeOfLastMetabolizeViewAnim = -1;
}

BOOL AvHMetabolize::Deploy()
{
//	this->mTimeOfLastMetabolizeViewAnim = -1;

	return AvHAlienWeapon::Deploy();
}

int	AvHMetabolize::GetBarrelLength() const
{
	return 0;
}

float AvHMetabolize::GetRateOfFire() const
{
    return BALANCE_VAR(kMetabolizeROF);
}

#ifdef AVH_SERVER
int	AvHMetabolize::GetResourceCost() const
{
	return BALANCE_VAR(kMetabolizeResourceCost);
}
#endif

int	AvHMetabolize::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
	int theDeployAnimation = 7;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
	case AVH_WEAPON_BLINK:
	case AVH_WEAPON_METABOLIZE:
		theDeployAnimation = -1;
		break;

	case AVH_WEAPON_SWIPE:
		theDeployAnimation = 9;
		break;
		
	case AVH_WEAPON_ACIDROCKET:
	case AVH_WEAPON_BILEBOMB:
		theDeployAnimation = 11;
		break;
	}
	
	return theDeployAnimation;
}

int	AvHMetabolize::GetIdleAnimation() const
{
	int theAnimation = UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 17, 18);
	return theAnimation;
}

void AvHMetabolize::ItemPostFrame(void)
{
	AvHAlienWeapon::ItemPostFrame();

	float theClientTimePassedThisTick = (pmove->cmd.msec/1000.0f);
//	this->mTimeOfLastMetabolizeViewAnim -= theClientTimePassedThisTick;
}

int	AvHMetabolize::GetShootAnimation() const
{
	int theAnimation = UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 19, 20);
	return theAnimation;
}

char* AvHMetabolize::GetViewModel() const
{
	return kLevel4ViewModel;
}

void AvHMetabolize::Precache()
{
	AvHAlienWeapon::Precache();

	PRECACHE_UNMODIFIED_SOUND(kMetabolizeFireSound1);
	PRECACHE_UNMODIFIED_SOUND(kMetabolizeFireSound2);
	PRECACHE_UNMODIFIED_SOUND(kMetabolizeFireSound3);
	
	this->mEvent = PRECACHE_EVENT(1, kMetabolizeEventName);
}


void AvHMetabolize::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 
	
	this->Precache();
	
	this->m_iId = AVH_WEAPON_METABOLIZE;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsMetabolize);
	
	SET_MODEL(ENT(this->pev), kNullModel);
	
	FallInit();// get ready to fall down.
} 

void AvHMetabolize::FireProjectiles(void)
{
	#ifdef AVH_SERVER
//	// Don't allow moving while metabolizing
//    if(this->m_pPlayer->pev->flags & FL_ONGROUND)
//	{
//		VectorScale(this->m_pPlayer->pev->velocity, 0.3, this->m_pPlayer->pev->velocity);
//	}

    // Get health back
    float theFocusAmount = 1.0f;
    if(AvHSHUGetIsWeaponFocusable(AvHWeaponID(this->m_iId)))
    {
        theFocusAmount *= AvHPlayerUpgrade::GetFocusDamageUpgrade(this->m_pPlayer->pev->iuser4);
    }

	// Get health back
	int theRegenAmount = BALANCE_VAR(kMetabolizeHealAmount)*theFocusAmount;
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
	ASSERT(thePlayer);
	thePlayer->Heal(theRegenAmount);

	// Adjusts player's max speed
	thePlayer->SetTimeOfMetabolizeEnd(gpGlobals->time + this->GetRateOfFire());
	#endif	
}

void AvHMetabolize::DeductCostForShot(void)
{
    // Add energy back after it's deducted to make sure extra isn't wasted
    AvHAlienWeapon::DeductCostForShot();

    // Get energy back
    float theFocusAmount = 1.0f;
    if(AvHSHUGetIsWeaponFocusable(AvHWeaponID(this->m_iId)))
    {
        theFocusAmount *= AvHPlayerUpgrade::GetFocusDamageUpgrade(this->m_pPlayer->pev->iuser4);
    }

    float theEnergyAmount = BALANCE_VAR(kMetabolizeEnergyAmount)*theFocusAmount;
	AvHMUGiveAlienEnergy(this->m_pPlayer->pev->fuser3, theEnergyAmount);
}

void AvHMetabolize::SetAnimationAndSound(void)
{
	// Only play view model anim every so often
//	const float kAnimInterval = 3.0f;
//	if(this->mTimeOfLastMetabolizeViewAnim < 0.0f)
//	{
//		this->SendWeaponAnim(19);
//		this->mTimeOfLastMetabolizeViewAnim = UTIL_WeaponTimeBase() + kAnimInterval;
//	}

	AvHAlienWeapon::SetAnimationAndSound();
}





