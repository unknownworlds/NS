//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHBite.cpp $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHBite.cpp,v $
// Revision 1.15  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.14  2002/11/06 01:38:37  Flayra
// - Added ability for buildings to be enabled and disabled, for turrets to be shut down
// - Damage refactoring (TakeDamage assumes caller has already adjusted for friendly fire, etc.)
//
// Revision 1.13  2002/08/16 02:33:13  Flayra
// - Added damage types
//
// Revision 1.12  2002/07/24 18:45:40  Flayra
// - Linux and scripting changes
//
// Revision 1.11  2002/07/08 16:48:26  Flayra
// - Melee weapons don't play hit sounds or punchangle target if they didn't damage them
//
// Revision 1.10  2002/07/01 21:21:15  Flayra
// - Removed adrenaline
//
// Revision 1.9  2002/06/25 17:50:59  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.8  2002/06/10 19:49:06  Flayra
// - Updated with new alien view model artwork (with running anims)
//
// Revision 1.7  2002/06/03 16:27:05  Flayra
// - Animation constants and changes with new artwork
//
// Revision 1.6  2002/05/23 02:34:00  Flayra
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

#include "mod/AvHSharedUtil.h"

LINK_ENTITY_TO_CLASS(kwBiteGun, AvHBite);
void V_PunchAxis( int axis, float punch );

void AvHBite::Init()
{
	this->mRange = BALANCE_VAR(kBiteRange);
	this->mDamage = BALANCE_VAR(kBiteDamage);
}

int	AvHBite::GetBarrelLength() const
{
	return kBiteBarrelLength;
}

bool AvHBite::GetIsGunPositionValid() const
{
    return true;
}
    
float AvHBite::GetRateOfFire() const
{
	return BALANCE_VAR(kBiteROF);
}

char* AvHBite::GetBiteSound() const
{
	return kBiteSound;
}

int	AvHBite::GetDeployAnimation() const
{
	// Only play deploy anim when changing to skulk
	int theDeployAnimation = 5;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
	case AVH_WEAPON_BITE:
	case AVH_WEAPON_PARASITE:
	case AVH_ABILITY_LEAP:
	case AVH_WEAPON_DIVINEWIND:
		theDeployAnimation = -1;
		break;
	}
	
	return theDeployAnimation;
}

int	AvHBite::GetIdleAnimation() const
{
	//int theBaseOffset = this->GetIsPlayerMoving() ? 5 : 2;
	//int theAnimation = theBaseOffset + UTIL_SharedRandomFloat(this->m_pPlayer->random_seed, 0, 1);

	// Must be odd
	int theAnimation = 0;
	const int kMax = 41;
	int theRandomNumber = UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 0, kMax);

	if(theRandomNumber == kMax)
	{
		// Return flavor anim
		theAnimation = 2;
	} 
	else if(theRandomNumber < (kMax-1)/2)
	{
		theAnimation = 0;
	}
	else
	{
		theAnimation = 1;
	}

	return theAnimation;
}

int	AvHBite::GetShootAnimation() const
{
	return 4;
}

char* AvHBite::GetViewModel() const
{
	return kLevel1ViewModel;
}

void AvHBite::Precache()
{
	AvHAlienWeapon::Precache();

	char* theBiteSound = this->GetBiteSound();
	if(theBiteSound)
	{
		PRECACHE_UNMODIFIED_SOUND(theBiteSound);
	}
	PRECACHE_UNMODIFIED_SOUND(kBiteHitSound1);
	PRECACHE_UNMODIFIED_SOUND(kBiteHitSound2);
	PRECACHE_UNMODIFIED_SOUND(kBiteKillSound);
	
	this->mEvent = PRECACHE_EVENT(1, kBiteEventName);
}


void AvHBite::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 
	
	this->Precache();
	
	this->m_iId = AVH_WEAPON_BITE;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsBiteGun);
	
	SET_MODEL(ENT(this->pev), kNullModel);
	
	FallInit();// get ready to fall down.
} 


void AvHBite::FireProjectiles(void)
{
#ifdef AVH_SERVER
	
	// TODO: Check team
	float theDamage = this->mDamage*AvHPlayerUpgrade::GetAlienMeleeDamageUpgrade(this->m_pPlayer->pev->iuser4, AvHSHUGetIsWeaponFocusable(AvHWeaponID(this->m_iId)));

	// Do trace hull here
	Vector theTestAiming = this->m_pPlayer->GetAutoaimVector( AUTOAIM_5DEGREES );
	
	CBaseEntity* pHurt = this->m_pPlayer->CheckTraceHullAttack(this->mRange, theDamage, this->GetDamageType());
	if(pHurt)
	{
		if(pHurt->pev->flags & (FL_MONSTER | FL_CLIENT))
		{
			AvHSUKnockPlayerAbout(CBaseEntity::Instance(this->m_pPlayer->edict()), pHurt, 225);

			int theSoundIndex = RANDOM_LONG(0, 1);
			char* theSoundToPlay = "";
			switch(theSoundIndex)
			{
			case 0:
				theSoundToPlay = kBiteHitSound1;
				break;
			case 1:
				theSoundToPlay = kBiteHitSound2;
				break;
			}
			
			if(pHurt->pev->health <= 0)
			{
				theSoundToPlay = kBiteKillSound;
			}
			
			EMIT_SOUND(ENT(pev), CHAN_WEAPON, theSoundToPlay, 1.0, ATTN_NORM);
		}
	}
#endif	
}

bool AvHBite::GetFiresUnderwater() const
{
	return true;
}

bool AvHBite::GetIsDroppable() const
{
	return false;
}

bool AvHBite::UsesAmmo(void) const
{
	return false;
}

BOOL AvHBite::UseDecrement(void)
{
	return true;
}
