//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHDevour.cpp $
// $Date: $
//
//-------------------------------------------------------------------------------
// $Log: $
//===============================================================================
#include "mod/AvHAlienWeapons.h"
#include "mod/AvHPlayer.h"
#include "mod/AvHConstants.h"

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

LINK_ENTITY_TO_CLASS(kwDevour, AvHDevour);

BOOL AvHDevour::CanHolster(void)
{
	return true;
}

void AvHDevour::Init()
{
	this->mRange = BALANCE_VAR(kDevourRange);
}

int	AvHDevour::GetBarrelLength() const
{
	return 0;
}

float AvHDevour::GetRateOfFire() const
{
	return BALANCE_VAR(kDevourROF);
}

int	AvHDevour::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
	int theDeployAnimation = 8;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
	case AVH_WEAPON_CLAWS:
		theDeployAnimation = 8;
		break;
	case AVH_ABILITY_CHARGE:
		theDeployAnimation = 12;
		break;
	case AVH_WEAPON_STOMP:
		theDeployAnimation = 17;
		break;
	}
	
	return theDeployAnimation;
}

float AvHDevour::GetDeployTime() const
{
	return .6f;
}

bool AvHDevour::GetFiresUnderwater() const
{
	return true;
}

bool AvHDevour::GetIsCapableOfFiring() const
{
	bool theIsCapableOfFiring = false;

    if(!GetHasUpgrade(this->m_pPlayer->pev->iuser4, MASK_DIGESTING))
	{
		theIsCapableOfFiring = true;
	}

	return theIsCapableOfFiring;
}

int AvHDevour::GetIdleAnimation() const
{
	int theIdle = 4;
    
	if(GetHasUpgrade(this->m_pPlayer->pev->iuser4, MASK_DIGESTING))
	{
		theIdle = 26;
	}

	return theIdle;
}

bool AvHDevour::GetIsDroppable() const
{
	return false;
}

int AvHDevour::GetShootAnimation() const
{
	return 25;
}

void AvHDevour::FireProjectiles(void)
{
#ifdef AVH_SERVER
	// If we're not already digesting a player
	if(this->GetIsCapableOfFiring())
	{
		UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);
		
		// Look for enemy player in front of us
		Vector vecAiming = gpGlobals->v_forward;
		Vector vecSrc = this->m_pPlayer->GetGunPosition( ) + vecAiming;
		Vector vecEnd = vecSrc + vecAiming*this->mRange;

		TraceResult theTraceResult;
		UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, this->m_pPlayer->edict(), &theTraceResult);

		edict_t* theEntityHit = theTraceResult.pHit;
		AvHPlayer* theDigestee = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(theEntityHit));
		if(theDigestee && theDigestee->GetCanBeAffectedByEnemies() && GetGameRules()->CanEntityDoDamageTo(this->m_pPlayer, theDigestee))
		{
			// Never devour friends or gestating players
			if((theDigestee->GetTeam() != this->m_pPlayer->pev->team) && theDigestee->IsAlive() && (theDigestee->GetUser3() != AVH_USER3_ALIEN_EMBRYO))
			{
				AvHPlayer* theDigester = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
				ASSERT(theDigester);

				theDigester->StartDigestion(theDigestee->entindex());
			}
		}
	}
#endif	

	// Immediately play idle after attack animation is done (either "full" or "empty" idle)
	this->m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + this->GetRateOfFire();
}

char* AvHDevour::GetViewModel() const
{
	return kLevel5ViewModel;
}

void AvHDevour::Precache()
{
	AvHAlienWeapon::Precache();

	PRECACHE_UNMODIFIED_SOUND(kDevourFireSound);
	PRECACHE_UNMODIFIED_SOUND(kDevourSwallowSound);
	PRECACHE_UNMODIFIED_SOUND(kDevourCompleteSound);
	PRECACHE_UNMODIFIED_SOUND(kDevourCancelSound);

	this->mEvent = PRECACHE_EVENT(1, kDevourShootEventName);
}

bool AvHDevour::ProcessValidAttack(void)
{
	bool theReturnCode = AvHAlienWeapon::ProcessValidAttack();

	if(theReturnCode)
	{
		theReturnCode = this->GetIsCapableOfFiring();
	}

	return theReturnCode;
}


void AvHDevour::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 

	Precache();

	this->m_iId = AVH_WEAPON_DEVOUR;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsDevour);

	SET_MODEL(ENT(this->pev), kNullModel);

	FallInit();// get ready to fall down.
} 

bool AvHDevour::UsesAmmo(void) const
{
	return false;
}

