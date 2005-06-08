//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHUmbraGun.cpp $
// $Date: 2002/11/22 21:28:17 $
//
//-------------------------------------------------------------------------------
// $Log: AvHUmbraGun.cpp,v $
// Revision 1.10  2002/11/22 21:28:17  Flayra
// - mp_consistency changes
//
// Revision 1.9  2002/08/16 02:49:14  Flayra
// - Umbra is now used by the level 4
//
// Revision 1.8  2002/07/24 19:09:18  Flayra
// - Linux issues
//
// Revision 1.7  2002/07/24 18:55:53  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.6  2002/07/24 18:45:43  Flayra
// - Linux and scripting changes
//
// Revision 1.5  2002/07/23 17:34:25  Flayra
// - Updated for new artwork
//
// Revision 1.4  2002/07/01 21:20:14  Flayra
// - Implemented umbra weapon
//
// Revision 1.3  2002/06/25 17:50:31  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.2  2002/06/03 16:39:10  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.1  2002/05/23 02:32:57  Flayra
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

LINK_ENTITY_TO_CLASS(kwUmbraGun, AvHUmbraGun);

#ifdef AVH_SERVER
LINK_ENTITY_TO_CLASS(kwUmbraProjectile, AvHUmbraProjectile);
LINK_ENTITY_TO_CLASS(kwUmbraCloud, AvHUmbraCloud);

AvHUmbraCloud::AvHUmbraCloud()
{
	this->mTimeHit = -1;
}

void AvHUmbraCloud::Precache(void)
{
	CBaseEntity::Precache();
}

void AvHUmbraCloud::Spawn(void)
{
	this->Precache();

	CBaseEntity::Spawn();
	
	this->pev->movetype = MOVETYPE_NONE;
	this->pev->classname = MAKE_STRING(kwsUmbraCloud);
	this->pev->iuser3 = AVH_USER3_UMBRA;
	this->pev->effects |= EF_NODRAW;

	SetThink(&AvHUmbraCloud::SUB_Remove);
	this->pev->nextthink = gpGlobals->time + BALANCE_IVAR(kUmbraCloudDuration);
}



void AvHUmbraProjectile::Spawn(void)
{
	this->Precache();

	CBaseEntity::Spawn();
	
	this->pev->movetype = MOVETYPE_FLY;
	this->pev->classname = MAKE_STRING(kwsUmbraProjectile);
	
	SET_MODEL(ENT(this->pev), kClientUmbraSprite);
	this->pev->solid = SOLID_BBOX;
	
	if(!GetGameRules()->GetDrawInvisibleEntities())
	{
		this->pev->effects = EF_NODRAW;
	}
	else
	{
		this->pev->frame = 0;
		this->pev->scale = 0.5;
		this->pev->rendermode = kRenderTransAlpha;
		this->pev->renderamt = 255;
	}
	
	UTIL_SetSize(this->pev, Vector( 0, 0, 0), Vector(0, 0, 0));

	SetTouch(&AvHUmbraProjectile::UmbraTouch);
}

void AvHUmbraProjectile::UmbraTouch(CBaseEntity* inOther)
{
	// Never hit the player who fired it
	if(inOther != CBaseEntity::Instance(this->pev->owner))
	{
		AvHSUCreateUmbraCloud(this->pev->origin, AvHTeamNumber(this->pev->team), this);

		// Remove the projectile
		SetTouch(NULL);

		UTIL_Remove(this);
	}
}

#endif



void AvHUmbraGun::Init()
{
	this->mRange = kUmbraRange;
	this->mDamage = 0;
}

int	AvHUmbraGun::GetBarrelLength() const
{
	return kUmbraBarrelLength;
}

float AvHUmbraGun::GetRateOfFire() const
{
	return BALANCE_FVAR(kUmbraROF);
}

int	AvHUmbraGun::GetDeployAnimation() const
{
	int theDeployAnimation = -1;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
	case AVH_WEAPON_BITE2:
		theDeployAnimation = 9;
		break;
	case AVH_WEAPON_SPIKE:
		theDeployAnimation = 8;
		break;
	}
	
	return theDeployAnimation;
}

int	AvHUmbraGun::GetIdleAnimation() const
{
	return 2;
}

int	AvHUmbraGun::GetShootAnimation() const
{
	return 5;
}

bool AvHUmbraGun::GetFiresUnderwater() const
{
	return true;
}

bool AvHUmbraGun::GetIsDroppable() const
{
	return false;
}

void AvHUmbraGun::FireProjectiles(void)
{
	#ifdef AVH_SERVER
	
	// Make sure we have enough points to shoot this thing
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
	ASSERT(thePlayer);

	// Create umbra projectile
	AvHUmbraProjectile* theUmbra = GetClassPtr((AvHUmbraProjectile*)NULL );
	theUmbra->Spawn();
	
	UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);
	
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc = this->m_pPlayer->GetGunPosition( ) + vecAiming;
	
	UTIL_SetOrigin(theUmbra->pev, vecSrc);
	
	VectorScale(vecAiming, kShootCloudVelocity, theUmbra->pev->velocity);
	
	// Set projectile team and owner
	theUmbra->pev->owner = ENT(this->m_pPlayer->pev);
	
	// Set Umbra's team :)
	theUmbra->pev->team = this->m_pPlayer->pev->team;
	
	#endif	
}

char* AvHUmbraGun::GetViewModel() const
{
	return kLevel3ViewModel;
}

void AvHUmbraGun::Precache()
{
	AvHAlienWeapon::Precache();

	PRECACHE_UNMODIFIED_SOUND(kUmbraFireSound);
	PRECACHE_UNMODIFIED_SOUND(kUmbraBlockedSound);
	PRECACHE_UNMODIFIED_MODEL(kUmbraSprite);
	PRECACHE_UNMODIFIED_MODEL(kClientUmbraSprite);

	this->mEvent = PRECACHE_EVENT(1, kUmbraShootEventName);
}

void AvHUmbraGun::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 

	Precache();

	this->m_iId = AVH_WEAPON_UMBRA;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsUmbraGun);

	SET_MODEL(ENT(this->pev), kNullModel);

	FallInit();// get ready to fall down.
} 

bool AvHUmbraGun::UsesAmmo(void) const
{
	return false;
}

