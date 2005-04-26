//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHKnife.cpp $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHKnife.cpp,v $
// Revision 1.17  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.16  2002/11/06 01:38:37  Flayra
// - Added ability for buildings to be enabled and disabled, for turrets to be shut down
// - Damage refactoring (TakeDamage assumes caller has already adjusted for friendly fire, etc.)
//
// Revision 1.15  2002/10/03 18:45:33  Flayra
// - Added heavy view models
// - Added new flavor anim (knife flourish)
//
// Revision 1.14  2002/08/09 01:04:23  Flayra
// - Knife is faster to deploy
//
// Revision 1.13  2002/07/24 19:09:17  Flayra
// - Linux issues
//
// Revision 1.12  2002/07/24 18:55:52  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.11  2002/07/24 18:45:42  Flayra
// - Linux and scripting changes
//
// Revision 1.10  2002/07/08 16:48:42  Flayra
// - Melee weapons don't play hit sounds or punchangle target if they didn't damage them
//
// Revision 1.9  2002/06/25 17:50:59  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.8  2002/06/10 19:57:39  Flayra
// - Updated with new knife artwork
//
// Revision 1.7  2002/06/03 16:37:31  Flayra
// - Constants and tweaks to make weapon anims and times correct with new artwork, added different deploy times (this should be refactored a bit more)
//
// Revision 1.6  2002/05/28 17:44:58  Flayra
// - Tweak weapon deploy volume, as Valve's sounds weren't normalized
//
// Revision 1.5  2002/05/23 02:33:42  Flayra
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

#ifdef AVH_SERVER
#include "mod/AvHGamerules.h"
#include "mod/AvHServerUtil.h"
#endif

LINK_ENTITY_TO_CLASS(kwKnife, AvHKnife);
void V_PunchAxis( int axis, float punch );

int	AvHKnife::GetDeployAnimation() const
{
	return 4;
}

float AvHKnife::GetDeployTime() const
{
	return .25f;
}

char* AvHKnife::GetDeploySound() const
{
	return kKNDeploySound;
}

char* AvHKnife::GetHeavyViewModel() const
{
	return kKNHVVModel;
}

int	AvHKnife::GetIdleAnimation() const
{
	// Only play the poking-finger animation once in awhile and play the knife flourish once in a blue moon, it's a special treat
	int iAnim;
	int theRandomNum = UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 0, 200);

	if(theRandomNum == 0)
	{
		iAnim = 5;
	}
	else if(theRandomNum < 16)
	{
		iAnim = kIdleAnimationTwo;	
	}
	else
	{
		iAnim = kIdleAnimationOne;
	}

	return iAnim;
}

bool AvHKnife::GetMustPressTriggerForEachShot() const
{ 
	return true;
}

char* AvHKnife::GetPlayerModel() const
{
	return kKNPModel;
}

char* AvHKnife::GetViewModel() const
{
	return kKNVModel;
}

char* AvHKnife::GetWorldModel() const
{
	return kKNWModel;
}

void AvHKnife::Init()
{
	this->mRange = kKNRange;
	this->mDamage = BALANCE_IVAR(kKNDamage);
}

void AvHKnife::FireProjectiles(void)
{
#ifdef AVH_SERVER
	
	// TODO: Check team
	
	// Do trace hull here
	float theDamage = this->mDamage;
	CBaseEntity* pHurt = this->m_pPlayer->CheckTraceHullAttack(kKNRange, theDamage, DMG_SLASH);
	if(pHurt)
	{
		char* theSoundToPlay = NULL;

		if(pHurt->pev->flags & (FL_MONSTER | FL_CLIENT))
		{
            //AvHSUKnockPlayerAbout(CBaseEntity::Instance(this->m_pPlayer->edict()), pHurt, 300);
			
			int theSoundIndex = RANDOM_LONG(0, 2);
			switch(theSoundIndex)
			{
			case 0:
				theSoundToPlay = kKNHitSound1;
				break;
			case 1:
				theSoundToPlay = kKNHitSound2;
				break;
			}
		}
		else
		{
			theSoundToPlay = kKNHitWallSound;
		}

		if(theSoundToPlay)
		{
			EMIT_SOUND(ENT(pev), CHAN_WEAPON, theSoundToPlay, 1.0, ATTN_NORM);
		}
	}
	
#endif	
}

int	AvHKnife::GetBarrelLength() const
{
	return kKNBarrelLength;
}

float AvHKnife::GetRateOfFire() const
{
	return BALANCE_FVAR(kKNROF);
}

bool AvHKnife::GetFiresUnderwater() const
{
	return true;
}

bool AvHKnife::GetIsDroppable() const
{
	return false;
}

int	AvHKnife::GetShootAnimation() const
{
	// CS artwork
	//return 5 + UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 0, 2);
	return 2 + UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 0, 1);
}

BOOL AvHKnife::IsUseable(void)
{
	return true;
}

void AvHKnife::Precache()
{
	AvHMarineWeapon::Precache();

	PRECACHE_UNMODIFIED_SOUND(kKNFireSound1);
	PRECACHE_UNMODIFIED_SOUND(kKNFireSound2);
	PRECACHE_UNMODIFIED_SOUND(kKNHitSound1);
	PRECACHE_UNMODIFIED_SOUND(kKNHitSound2);
	PRECACHE_UNMODIFIED_SOUND(kKNHitWallSound);

	this->mEvent = PRECACHE_EVENT(1, kKNEventName);
}

void AvHKnife::Spawn() 
{ 
    AvHMarineWeapon::Spawn(); 

	Precache();

	this->m_iId = AVH_WEAPON_KNIFE;
	//this->m_iDefaultAmmo = kKNMaxClip;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsKnife);
	
	SET_MODEL(ENT(this->pev), kNullModel);
	
	FallInit();// get ready to fall down.
} 


bool AvHKnife::UsesAmmo(void) const
{
	return false;
}

BOOL AvHKnife::UseDecrement(void)
{
	return true;
}

