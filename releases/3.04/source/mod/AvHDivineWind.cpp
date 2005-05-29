//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHDivineWind.cpp$
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHDivineWind.cpp,v $
// Revision 1.11  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.10  2002/10/16 00:52:55  Flayra
// - Plays cool effect now
//
// Revision 1.9  2002/09/23 22:12:20  Flayra
// - Removed offensive upgrade awareness
//
// Revision 1.8  2002/08/16 02:44:10  Flayra
// - New damage types
//
// Revision 1.7  2002/07/24 19:09:16  Flayra
// - Linux issues
//
// Revision 1.6  2002/07/24 18:55:51  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.5  2002/07/24 18:45:41  Flayra
// - Linux and scripting changes
//
// Revision 1.4  2002/06/25 17:50:59  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.3  2002/06/10 19:49:06  Flayra
// - Updated with new alien view model artwork (with running anims)
//
// Revision 1.2  2002/06/03 16:27:06  Flayra
// - Animation constants and changes with new artwork
//
// Revision 1.1  2002/05/23 02:33:41  Flayra
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

#ifdef AVH_SERVER
#include "mod/AvHGamerules.h"
#include "mod/AvHServerUtil.h"
#endif

#include "mod/AvHParticleConstants.h"
#include "mod/AvHSharedUtil.h"

LINK_ENTITY_TO_CLASS(kwDivineWind, AvHDivineWind);
extern int gDivineWindStartEventID;

BOOL AvHDivineWind::CanHolster(void)
{
	return true;
}

void AvHDivineWind::Init()
{
	this->mRange = kDivineWindRange;
	this->mDamage = BALANCE_IVAR(kDivineWindDamage);
	this->mPrimed = false;
}

int	AvHDivineWind::GetBarrelLength() const
{
	return 0;
}

float AvHDivineWind::GetRateOfFire() const
{
	return BALANCE_FVAR(kDivineWindROF);
}

int	AvHDivineWind::GetDeployAnimation() const
{
	return 13;
}

bool AvHDivineWind::GetFiresUnderwater() const
{
	return true;
}

bool AvHDivineWind::GetIsDroppable() const
{
	return false;
}

int AvHDivineWind::GetShootAnimation() const
{
	return -1;
}

BOOL AvHDivineWind::IsUseable(void)
{
	BOOL theIsUseable = FALSE;
	
	if(AvHAlienWeapon::IsUseable())
	{
		theIsUseable = !this->mPrimed;
	}

	return theIsUseable;
}

void AvHDivineWind::Explode(void)
{
#ifdef AVH_SERVER
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
	ASSERT(thePlayer);
	
	if(thePlayer->GetCanBeAffectedByEnemies())
	{
		// Treat damage upgrade as modifier onto DivineWind
		//int theTracerFreq;
		//float theDamageMultiplier;
		//AvHPlayerUpgrade::GetWeaponUpgrade(thePlayer->pev->iuser4, &theDamageMultiplier, &theTracerFreq);
		
		// Explode!
		EMIT_SOUND(thePlayer->edict(), CHAN_AUTO, kDivineWindExplodeSound, 1.0f, ATTN_NORM);

		// Kill ourself (set team to 0 so we always take damage)
		this->pev->team = 0;
		thePlayer->TakeDamage(this->pev, this->pev, 1000, NS_DMG_NORMAL | DMG_ALWAYSGIB);

		// Add explosive force
		float theDamage = this->mDamage * AvHPlayerUpgrade::GetAlienRangedDamageUpgrade(this->m_pPlayer->pev->iuser4);

        if(AvHSHUGetIsWeaponFocusable(AvHWeaponID(this->m_iId)))
        {
            theDamage *= AvHPlayerUpgrade::GetFocusDamageUpgrade(this->m_pPlayer->pev->iuser4);
        }

		float theForceScalar = .05f*theDamage;
        int theRadius = BALANCE_IVAR(kDivineWindRadius);
		AvHSUExplosiveForce(this->pev->origin, theRadius, theForceScalar, thePlayer);

		// Make sure weapon team is the same as player team so we don't damage friends when friendly fire is off
		this->pev->team = thePlayer->pev->team;
		::RadiusDamage(thePlayer->pev->origin, this->pev, thePlayer->pev, theDamage, theRadius, CLASS_NONE, NS_DMG_NORMAL);
		
		// Shake view of those around us!
		float theShakeAmplitude = 30;
		float theShakeFrequency = 100;
		float theShakeDuration = 1.5f;
		float theShakeRadius = theRadius;
		UTIL_ScreenShake(thePlayer->pev->origin, theShakeAmplitude, theShakeFrequency, theShakeDuration, theShakeRadius);

		AvHSUPlayParticleEvent(kpsXenocide, this->edict(), this->pev->origin);

		// Increment our score so it doesn't count as suicide
		//thePlayer->pev->frags++;
	}
#endif
}

// This is currently the length of kDivineWindFireSound
const float kExplodeTime = 1.7f;

void AvHDivineWind::FireProjectiles(void)
{
#ifdef AVH_SERVER
	// If we haven't already been activated
	if(!this->mPrimed)
	{
		// Make it so weapon can't be switched away from
		this->mPrimed = true;

		// Play "about to go off" sound (this is the fire sound)

		// Set think to be a small time from now
		SetThink(&AvHDivineWind::Explode);
		this->pev->nextthink = gpGlobals->time + kExplodeTime;
	}
#endif	
}

char* AvHDivineWind::GetViewModel() const
{
	return kLevel1ViewModel;
}

void AvHDivineWind::Precache()
{
	AvHAlienWeapon::Precache();

	PRECACHE_UNMODIFIED_SOUND(kDivineWindFireSound);
	PRECACHE_UNMODIFIED_SOUND(kDivineWindExplodeSound);

	this->mEvent = PRECACHE_EVENT(1, kDivineWindShootEventName);
}

void AvHDivineWind::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 

	Precache();

	this->m_iId = AVH_WEAPON_DIVINEWIND;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsDivineWind);

	SET_MODEL(ENT(this->pev), kNullModel);

	FallInit();// get ready to fall down.
} 

bool AvHDivineWind::UsesAmmo(void) const
{
	return false;
}

