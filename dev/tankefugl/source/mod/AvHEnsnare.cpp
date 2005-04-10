//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHEnsnare.cpp $
// $Date: 2002/07/24 19:09:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHEnsnare.cpp,v $
// Revision 1.7  2002/07/24 19:09:16  Flayra
// - Linux issues
//
// Revision 1.6  2002/07/24 18:55:52  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.5  2002/06/03 16:39:09  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.4  2002/05/23 02:33:41  Flayra
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
#include "mod/AvHAlienWeaponConstants.h"

LINK_ENTITY_TO_CLASS(kwEnsnareGun, AvHEnsnare);


#ifdef AVH_SERVER
LINK_ENTITY_TO_CLASS(kwEnsnareGunProjectile, AvHEnsnareProjectile);
extern int gEnsnareHitEventID;

void AvHEnsnareProjectile::Precache(void)
{
	CBaseEntity::Precache();
	
	PRECACHE_MODEL(kEnsnareSprite);
}

void AvHEnsnareProjectile::Spawn(void)
{
	this->Precache();
	CBaseEntity::Spawn();
	
	this->pev->movetype = MOVETYPE_FLY;
	this->pev->classname = MAKE_STRING(kwsEnsnareGunProjectile);
	
	SET_MODEL(ENT(this->pev), kEnsnareSprite);
	this->pev->solid = SOLID_BBOX;
	
	// Comment out effects line, uncomment next four, then comment out creation of temp entity in EV_ShootEnsnare to see server side ensnare for testing
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
	
	SetTouch(EnsnareTouch);
}

void AvHEnsnareProjectile::EnsnareTouch(CBaseEntity *pOther)
{
	// Never hit the player who fired it
	if(pOther != CBaseEntity::Instance(this->pev->owner))
	{
		// If not in tourny mode, pass through friends
		if(pOther->pev->team != this->pev->team)
		{
			// Kill the ensnare projectile
			SetThink(SUB_Remove);
			this->pev->nextthink = gpGlobals->time + 0.01f;

			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(pOther);
			if(thePlayer)
			{
				PLAYBACK_EVENT_FULL(0, this->edict(), gEnsnareHitEventID, 0, pOther->pev->origin, (float *)&g_vecZero, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
				thePlayer->SetIsEnsnared();
			}
		}
	}
}
#endif


BOOL AvHEnsnare::Deploy()
{
	return DefaultDeploy(kEnsnareVModel, kEnsnarePModel, this->GetDeployAnimation(), kEnsnareAnimExt);
}

int	AvHEnsnare::GetBarrelLength() const
{
	return kEnsnareBarrelLength;
}

bool AvHEnsnare::GetFiresUnderwater() const
{
	return true;
}

bool AvHEnsnare::GetIsDroppable() const
{
	return false;
}
	
void AvHEnsnare::Init()
{
	this->mRange = kEnsnareRange;
	this->mDamage = kEnsnareDamage;
	this->mROF = kEnsnareROF;
}

void AvHEnsnare::Precache(void)
{
	AvHAlienWeapon::Precache();
	
	PRECACHE_MODEL(kEnsnareVModel);
	PRECACHE_MODEL(kEnsnarePModel);
	PRECACHE_MODEL(kAlienGunWModel);
	
	PRECACHE_SOUND(kEnsnareFireSound);
	PRECACHE_SOUND(kEnsnareHitSound);
	
	this->mEvent = PRECACHE_EVENT(1, kEnsnareShootEventName);
	//this->mEvent = PRECACHE_EVENT(1, kSpitGEventName);
}

void AvHEnsnare::Spawn()
{
    AvHAlienWeapon::Spawn(); 
	
	Precache();
	
	this->m_iId = AVH_WEAPON_ENSNARE;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsEnsnareGun);
	
	SET_MODEL(ENT(this->pev), kAlienGunWModel);
	
	FallInit();// get ready to fall down.
	
}

bool AvHEnsnare::UsesAmmo(void) const
{
	return false;
}

void AvHEnsnare::FireProjectiles(void)
{
	#ifdef AVH_SERVER
	// Make sure we have enough points to shoot this thing
//	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
//	ASSERT(thePlayer);
//	
//	if(thePlayer->GetResources() > kEnsnarePointCost)
//	{
//		// Decrement kEnsnarePointCost points
//		thePlayer->SetResources(thePlayer->GetResources() - kEnsnarePointCost);
		
		// Create hidden projectile that creates a huge Ensnare cloud where it hits
		AvHEnsnareProjectile* theEnsnare = GetClassPtr((AvHEnsnareProjectile*)NULL );
		theEnsnare->Spawn();
		
		UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);
		
		Vector vecAiming = gpGlobals->v_forward;
		Vector vecSrc = this->m_pPlayer->GetGunPosition( ) + vecAiming;
		
		UTIL_SetOrigin(theEnsnare->pev, vecSrc);
		
		// This needs to be the same as in EV_EnsnareGun
		Vector theBaseVelocity;
		VectorScale(this->pev->velocity, kEnsnareParentVelocityScalar, theBaseVelocity);
		
		Vector theStartVelocity;
		VectorMA(theBaseVelocity, kEnsnareVelocity, vecAiming, theStartVelocity);
		
		VectorCopy(theStartVelocity, theEnsnare->pev->velocity);
		
		// Set owner
		theEnsnare->pev->owner = ENT(this->m_pPlayer->pev);
		
		// Set Ensnare's team :)
		theEnsnare->pev->team = this->m_pPlayer->pev->team;
//	}
	#endif	
}

