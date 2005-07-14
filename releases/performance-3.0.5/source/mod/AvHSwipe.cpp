//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHSwipe.cpp $
// $Date: 2002/11/22 21:28:17 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSwipe.cpp,v $
// Revision 1.15  2002/11/22 21:28:17  Flayra
// - mp_consistency changes
//
// Revision 1.14  2002/11/06 01:38:37  Flayra
// - Added ability for buildings to be enabled and disabled, for turrets to be shut down
// - Damage refactoring (TakeDamage assumes caller has already adjusted for friendly fire, etc.)
//
// Revision 1.13  2002/08/16 02:48:10  Flayra
// - New damage model
//
// Revision 1.12  2002/07/24 19:09:18  Flayra
// - Linux issues
//
// Revision 1.11  2002/07/24 18:55:53  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.10  2002/07/24 18:45:43  Flayra
// - Linux and scripting changes
//
// Revision 1.9  2002/07/23 17:29:52  Flayra
// - Updated for new artwork
//
// Revision 1.8  2002/07/08 16:48:50  Flayra
// - Melee weapons don't play hit sounds or punchangle target if they didn't damage them
//
// Revision 1.7  2002/06/25 17:51:00  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.6  2002/06/03 16:39:10  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.5  2002/05/23 02:32:57  Flayra
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

LINK_ENTITY_TO_CLASS(kwSwipe, AvHSwipe);

int	AvHSwipe::GetBarrelLength() const
{
	return kSwipeBarrelLength;
}

bool AvHSwipe::GetIsGunPositionValid() const
{
    return true;
}

float AvHSwipe::GetRateOfFire() const
{
	return BALANCE_FVAR(kSwipeROF);
}

bool AvHSwipe::GetFiresUnderwater() const
{
	return true;
}

int	AvHSwipe::GetIdleAnimation() const
{
	// Only play the arm-scratching animation once in awhile, it's a special treat
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


bool AvHSwipe::GetIsDroppable() const
{
	return false;
}

int	AvHSwipe::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
	int theDeployAnimation = 5;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
	case AVH_WEAPON_SWIPE:
		theDeployAnimation = -1;
		break;

	case AVH_WEAPON_ACIDROCKET:
	case AVH_WEAPON_BILEBOMB:
		theDeployAnimation = 10;
		break;

	case AVH_WEAPON_BLINK:
	case AVH_WEAPON_METABOLIZE:
		theDeployAnimation = 12;
		break;
	}
	
	return theDeployAnimation;
}

int	AvHSwipe::GetShootAnimation() const
{
	int theAnimation = UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 3, 4);

	return theAnimation;
}

void AvHSwipe::Init()
{
	this->mRange = kSwipeRange;
	this->mDamage = BALANCE_IVAR(kSwipeDamage);
}

int AvHSwipe::GetDamageType() const
{
	return NS_DMG_NORMAL;
}

char* AvHSwipe::GetViewModel() const
{
	return kLevel4ViewModel;
}


void AvHSwipe::Precache(void)
{
	AvHAlienWeapon::Precache();
	
	PRECACHE_UNMODIFIED_SOUND(kSwipeSound1);
	PRECACHE_UNMODIFIED_SOUND(kSwipeSound2);
	PRECACHE_UNMODIFIED_SOUND(kSwipeSound3);
	PRECACHE_UNMODIFIED_SOUND(kSwipeSound4);

	PRECACHE_UNMODIFIED_SOUND(kSwipeHitSound1);
	PRECACHE_UNMODIFIED_SOUND(kSwipeHitSound2);
	PRECACHE_UNMODIFIED_SOUND(kSwipeKillSound);

	this->mEvent = PRECACHE_EVENT(1, kSwipeEventName);
}

void AvHSwipe::Spawn()
{
    AvHAlienWeapon::Spawn(); 
	
	Precache();
	
	this->m_iId = AVH_WEAPON_SWIPE;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsSwipe);
	
	SET_MODEL(ENT(this->pev), kNullModel);
	
	FallInit();// get ready to fall down.
	
}

bool AvHSwipe::UsesAmmo(void) const
{
	return false;
}

BOOL AvHSwipe::UseDecrement(void)
{
	return true;
}

void AvHSwipe::FireProjectiles(void)
{
	#ifdef AVH_SERVER

	// TODO: Check team

	float theDamage = this->mDamage*AvHPlayerUpgrade::GetAlienMeleeDamageUpgrade(this->m_pPlayer->pev->iuser4, AvHSHUGetIsWeaponFocusable(AvHWeaponID(this->m_iId)));
	
	// Do trace hull here
	CBaseEntity* pHurt = this->m_pPlayer->CheckTraceHullAttack(kSwipeRange, theDamage, this->GetDamageType());
	if(pHurt)
	{
		if(pHurt->pev->flags & (FL_MONSTER | FL_CLIENT))
		{
            AvHSUKnockPlayerAbout(CBaseEntity::Instance(this->m_pPlayer->edict()), pHurt, 300);
			
			int theSoundIndex = RANDOM_LONG(0, 1);
			char* theSoundToPlay = NULL;
			switch(theSoundIndex)
			{
			case 0:
				theSoundToPlay = kSwipeHitSound1;
				break;
			case 1:
				theSoundToPlay = kSwipeHitSound2;
				break;
			}
			
			if(pHurt->pev->health <= 0)
			{
				theSoundToPlay = kSwipeKillSound;
			}
			
			ASSERT(theSoundToPlay);

			EMIT_SOUND(ENT(pev), CHAN_WEAPON, theSoundToPlay, 1.0, ATTN_NORM);
		}
	}

	#endif	
}

