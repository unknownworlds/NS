//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHClaws.cpp $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHClaws.cpp,v $
// Revision 1.19  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.18  2002/11/06 01:38:37  Flayra
// - Added ability for buildings to be enabled and disabled, for turrets to be shut down
// - Damage refactoring (TakeDamage assumes caller has already adjusted for friendly fire, etc.)
//
// Revision 1.17  2002/09/23 22:10:56  Flayra
// - New onos view model artwork
//
// Revision 1.16  2002/08/16 02:33:13  Flayra
// - Added damage types
//
// Revision 1.15  2002/07/26 23:03:08  Flayra
// - New artwork
//
// Revision 1.14  2002/07/24 18:55:51  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.13  2002/07/24 18:45:40  Flayra
// - Linux and scripting changes
//
// Revision 1.12  2002/07/08 16:48:26  Flayra
// - Melee weapons don't play hit sounds or punchangle target if they didn't damage them
//
// Revision 1.11  2002/07/01 21:21:15  Flayra
// - Removed adrenaline
//
// Revision 1.10  2002/06/25 17:50:59  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.9  2002/06/03 16:39:09  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.8  2002/05/23 02:34:00  Flayra
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

LINK_ENTITY_TO_CLASS(kwClaws, AvHClaws);

int	AvHClaws::GetBarrelLength() const
{
	return kSpitGBarrelLength;
}

bool AvHClaws::GetIsGunPositionValid() const
{
    return true;
}

float AvHClaws::GetRateOfFire() const
{
	return BALANCE_FVAR(kClawsROF);
}

int	AvHClaws::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
	int theDeployAnimation = 9;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
	case AVH_ABILITY_CHARGE:
		theDeployAnimation = 9;
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

float AvHClaws::GetDeployTime() const
{
	return .5f;
}

int AvHClaws::GetIdleAnimation() const
{
	return 0;
}

int	AvHClaws::GetShootAnimation() const
{
	return 21;
}


bool AvHClaws::GetFiresUnderwater() const
{
	return true;
}
	
bool AvHClaws::GetIsDroppable() const
{
	return false;
}

void AvHClaws::Init()
{
	this->mRange = kClawsRange;
	this->mDamage = BALANCE_IVAR(kClawsDamage);
}

int AvHClaws::GetDamageType() const
{
	return NS_DMG_BLAST;
}

char* AvHClaws::GetViewModel() const
{
	return kLevel5ViewModel;
}

void AvHClaws::Precache(void)
{
	AvHAlienWeapon::Precache();
	
	PRECACHE_UNMODIFIED_SOUND(kClawsSound1);
	PRECACHE_UNMODIFIED_SOUND(kClawsSound2);
	PRECACHE_UNMODIFIED_SOUND(kClawsSound3);

	PRECACHE_UNMODIFIED_SOUND(kClawsHitSound1);
	PRECACHE_UNMODIFIED_SOUND(kClawsHitSound2);
	PRECACHE_UNMODIFIED_SOUND(kClawsKillSound);
	
	this->mEvent = PRECACHE_EVENT(1, kClawsEventName);
}

void AvHClaws::Spawn()
{
    AvHAlienWeapon::Spawn(); 
	
	Precache();
	
	this->m_iId = AVH_WEAPON_CLAWS;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsClaws);
	
	SET_MODEL(ENT(this->pev), kNullModel);
	
	FallInit();// get ready to fall down.
	
}

bool AvHClaws::UsesAmmo(void) const
{
	return false;
}

BOOL AvHClaws::UseDecrement(void)
{
	return true;
}

void AvHClaws::FireProjectiles(void)
{
	#ifdef AVH_SERVER

	// TODO: Check team

	float theDamage = this->mDamage*AvHPlayerUpgrade::GetAlienMeleeDamageUpgrade(this->m_pPlayer->pev->iuser4, AvHSHUGetIsWeaponFocusable(AvHWeaponID(this->m_iId)));
	
	// Do trace hull here
	CBaseEntity* pHurt = this->m_pPlayer->CheckTraceHullAttack(kClawsRange, theDamage, this->GetDamageType());
	if(pHurt)
	{
		if(pHurt->pev->flags & (FL_MONSTER | FL_CLIENT))
		{
            AvHSUKnockPlayerAbout(CBaseEntity::Instance(this->m_pPlayer->edict()), pHurt, 300);
			
			int theSoundIndex = RANDOM_LONG(0, 1);
			char* theSoundToPlay = "";
			switch(theSoundIndex)
			{
			case 0:
				theSoundToPlay = kClawsHitSound1;
				break;
			case 1:
				theSoundToPlay = kClawsHitSound2;
				break;
			}
			
			if(pHurt->pev->health <= 0)
			{
				theSoundToPlay = kClawsKillSound;
			}

			// Throw nearby players around!
			float theForceScalar = theDamage*.2f;
			CBaseEntity* theAttacker = this->m_pPlayer;
			AvHSUExplosiveForce(pHurt->pev->origin, 100, theForceScalar, theAttacker, theAttacker);
			
			// Played in event now
			//EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, theSoundToPlay, 1.0, ATTN_NORM, 0, 100 + theAdrenalineFactor*30 + RANDOM_LONG(-3,3) );
		}
	}

	#endif	
}

