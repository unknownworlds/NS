//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHParalysisGun.cpp $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHParalysisGun.cpp,v $
// Revision 1.11  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.10  2002/10/16 20:55:15  Flayra
// - Added visible paralysis projectile
//
// Revision 1.9  2002/09/23 22:23:55  Flayra
// - Removed damage upgrades for paralysis
// - Updated onos view model artwork
//
// Revision 1.8  2002/07/26 23:06:18  Flayra
// - New artwork
//
// Revision 1.7  2002/07/24 19:09:17  Flayra
// - Linux issues
//
// Revision 1.6  2002/07/24 18:55:52  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.5  2002/07/24 18:45:42  Flayra
// - Linux and scripting changes
//
// Revision 1.4  2002/06/25 17:50:59  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.3  2002/06/03 16:39:09  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.2  2002/05/23 02:33:20  Flayra
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
#include "mod/AvHGamerules.h"

LINK_ENTITY_TO_CLASS(kwParalysisGun, AvHParalysisGun);
extern int gParalysisStartEventID;

void AvHParalysisGun::Init()
{
	this->mRange = kParalysisRange;
	this->mDamage = kParalysisDamage;
	this->mROF = kParalysisROF;
}

int	AvHParalysisGun::GetBarrelLength() const
{
	return kParalysisBarrelLength;
}

int	AvHParalysisGun::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
	int theDeployAnimation = 10;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
	case AVH_WEAPON_CLAWS:
		theDeployAnimation = 4;
		break;
	case AVH_ABILITY_CHARGE:
		theDeployAnimation = 6;
		break;
	}
	
	return theDeployAnimation;
}

float AvHParalysisGun::GetDeployTime() const
{
	return .5f;
}

int	AvHParalysisGun::GetIdleAnimation() const
{
	return 2;
}

int	AvHParalysisGun::GetShootAnimation() const
{
	return 11;
}

bool AvHParalysisGun::GetFiresUnderwater() const
{
	return true;
}

bool AvHParalysisGun::GetIsDroppable() const
{
	return false;
}

void AvHParalysisGun::FireProjectiles(void)
{
#ifdef AVH_SERVER
	
	UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);
	
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc = this->m_pPlayer->GetGunPosition( ) + vecAiming;
	Vector vecEnd = vecSrc + vecAiming*kParalysisRange;
	
	const float kParalysisTime = 6;
	
	// Treat damage upgrade as modifier onto paralysis
	//int theTracerFreq;
	//float theDamageMultiplier;
	//AvHPlayerUpgrade::GetWeaponUpgrade(this->m_pPlayer->pev->iuser4, &theDamageMultiplier, &theTracerFreq);

	// Perform trace to hit victim
	TraceResult tr;
	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, dont_ignore_glass, this->m_pPlayer->edict(), &tr);
	CBaseEntity* theEntityHit = CBaseEntity::Instance(tr.pHit);
	if(theEntityHit)
	{
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntityHit);
		if(thePlayer)
		{
			// Check teams (hit friendly in tourny mode)
			if((thePlayer->pev->team != this->m_pPlayer->pev->team) || (GetGameRules()->GetIsTournamentMode()))
			{
				thePlayer->SetIsParalyzed(true, kParalysisTime/**theDamageMultiplier*/);

				// Play hit event
				PLAYBACK_EVENT_FULL(0, thePlayer->edict(), gParalysisStartEventID, 0, thePlayer->pev->origin, (float *)&g_vecZero, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0);
			}
		}
	}

	#endif	
}

char* AvHParalysisGun::GetViewModel() const
{
	return kLevel5ViewModel;
}


void AvHParalysisGun::Precache()
{
	AvHAlienWeapon::Precache();

	PRECACHE_UNMODIFIED_SOUND(kParalysisFireSound);
	PRECACHE_UNMODIFIED_SOUND(kParalysisHitSound);
	PRECACHE_UNMODIFIED_MODEL(kParalysisProjectileModel);

	this->mEvent = PRECACHE_EVENT(1, kParalysisShootEventName);
}

void AvHParalysisGun::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 

	Precache();

	this->m_iId = AVH_WEAPON_PARALYSIS;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsParalysisGun);

	SET_MODEL(ENT(this->pev), kNullModel);

	FallInit();// get ready to fall down.
} 

bool AvHParalysisGun::UsesAmmo(void) const
{
	return false;
}

