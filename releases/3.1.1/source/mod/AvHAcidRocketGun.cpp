//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAcidRocketGun.cpp $
// $Date: 2002/11/22 21:28:15 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAcidRocketGun.cpp,v $
// Revision 1.8  2002/11/22 21:28:15  Flayra
// - mp_consistency changes
//
// Revision 1.7  2002/10/16 20:50:53  Flayra
// - Fixed problem where projectile hit player
//
// Revision 1.6  2002/08/16 02:32:09  Flayra
// - Added damage types
// - Swapped umbra and bile bomb
//
// Revision 1.5  2002/07/24 18:55:51  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.4  2002/07/24 18:45:40  Flayra
// - Linux and scripting changes
//
// Revision 1.3  2002/07/23 16:54:23  Flayra
// - Updates for new viewmodel artwork
//
// Revision 1.2  2002/07/01 21:11:59  Flayra
// - Removed unneeded linking entity to classname
//
// Revision 1.1  2002/06/25 17:23:02  Flayra
// - Level 4 projectile
//
//===============================================================================
#include "mod/AvHAlienWeapons.h"
#include "mod/AvHPlayer.h"

#ifdef AVH_CLIENT
#include "cl_dll/eventscripts.h"
#include "cl_dll/in_defs.h"
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#include "common/hldm.h"
#include "common/event_api.h"
#include "common/event_args.h"
#endif

#include "common/vector_util.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHConstants.h"


LINK_ENTITY_TO_CLASS(kwAcidRocketGun, AvHAcidRocketGun);
void V_PunchAxis( int axis, float punch );


#ifdef AVH_SERVER

#include "mod/AvHGamerules.h"
#include "mod/AvHServerUtil.h"

void AvHAcidRocket::Precache(void)
{
	CBaseEntity::Precache();

	PRECACHE_UNMODIFIED_MODEL(kAcidRocketProjectileModel);
}

void AvHAcidRocket::SetDamage(float inDamage)
{
	this->mDamage = inDamage;
}

void AvHAcidRocket::Spawn()
{
	this->Precache();
	CBaseEntity::Spawn();
	
	this->pev->movetype = MOVETYPE_FLY;
	this->pev->classname = MAKE_STRING(kwsAcidRocket);
	
	SET_MODEL(ENT(this->pev), kAcidRocketProjectileModel);
	this->pev->solid = SOLID_BBOX;
	this->mDamage = 0.0f;
	
	// Comment out effects line, uncomment next four, then comment out creation of temp entity in EV_AcidGun to see server side Acid for testing
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
	
	//UTIL_SetSize(this->pev, Vector( 0, 0, 0), Vector(0, 0, 0));
//	UTIL_SetSize(this->pev, Vector( -16, -16, -16), Vector(16, 16, 16));
	//UTIL_SetSize(this->pev, Vector( -50, -50, -50), Vector(50, 50, 50));
	
	SetTouch(&AvHAcidRocket::AcidRocketTouch);

//	// Enforce short range
//	SetThink(AcidRocketDeath);
//	this->pev->nextthink = gpGlobals->time + kAcidRocketLifetime;
}

void AvHAcidRocket::AcidRocketDeath()
{
}

void AvHAcidRocket::AcidRocketTouch(CBaseEntity* pOther)
{
	edict_t* theRocketOwner = this->pev->owner;
	CBaseEntity* theAttacker = CBaseEntity::Instance(theRocketOwner);

	//float theForceScalar = BALANCE_VAR(kAcidRocketForceScalar)*this->mDamage;
	//AvHSUExplosiveForce(this->pev->origin, kAcidRocketRadius, theForceScalar, theAttacker);

	// Explode with splash damage
	RadiusDamage(this->pev->origin, this->pev, VARS(theRocketOwner), this->mDamage, BALANCE_VAR(kAcidRocketRadius), CLASS_NONE, NS_DMG_ACID);

	// Kill the Acid entity
	UTIL_Remove(this);
}
#endif




void AvHAcidRocketGun::Init()
{
	this->mRange = kAcidRocketRange;
	this->mDamage = BALANCE_VAR(kAcidRocketDamage);
}

void AvHAcidRocketGun::FireProjectiles(void)
{
	#ifdef AVH_SERVER
	
	// Spawn AcidRocket
	AvHAcidRocket* theAcidRocket = GetClassPtr((AvHAcidRocket*)NULL );
	theAcidRocket->Spawn();
	
	UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);

	Vector vecAiming = gpGlobals->v_forward;

	// TODO: Make rocket appear to come from arm
	Vector theRocketOrigin;
	VectorMA(this->m_pPlayer->GetGunPosition(), this->GetBarrelLength(), vecAiming, theRocketOrigin);
	
	UTIL_SetOrigin(theAcidRocket->pev, theRocketOrigin);

	// This needs to be the same as in EV_AcidRocketGun
	Vector theBaseVelocity;
	VectorScale(this->pev->velocity, kAcidRocketParentVelocityScalar, theBaseVelocity);
	
	Vector theStartVelocity;
	VectorMA(theBaseVelocity, kAcidRocketVelocity, vecAiming, theStartVelocity);

	VectorCopy(theStartVelocity, theAcidRocket->pev->velocity);

	// Set owner
	theAcidRocket->pev->owner = ENT(this->m_pPlayer->pev);

	// Set AcidRocket's team :)
	theAcidRocket->pev->team = this->m_pPlayer->pev->team;

	// Set amount of damage it will do
	float theDamage = this->mDamage*AvHPlayerUpgrade::GetAlienRangedDamageUpgrade(this->m_pPlayer->pev->iuser4);
	theAcidRocket->SetDamage(theDamage);

	#endif	
}

int	AvHAcidRocketGun::GetBarrelLength() const
{
	return kAcidRocketBarrelLength;
}

int	AvHAcidRocketGun::GetIdleAnimation() const
{
	return UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 14, 15);
}

float AvHAcidRocketGun::GetRateOfFire() const
{
    return BALANCE_VAR(kAcidRocketROF);
}

int	AvHAcidRocketGun::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
	int theDeployAnimation = 6;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
	case AVH_WEAPON_ACIDROCKET:
		theDeployAnimation = -1;
		break;

	case AVH_WEAPON_SWIPE:
	case AVH_WEAPON_BLINK:
		theDeployAnimation = 8;
		break;

	case AVH_WEAPON_METABOLIZE:
		theDeployAnimation = 13;
		break;
	}
	
	return theDeployAnimation;
}

bool AvHAcidRocketGun::GetFiresUnderwater() const
{
	return true;
}

bool AvHAcidRocketGun::GetIsDroppable() const
{
	return false;
}

bool AvHAcidRocketGun::GetMustPressTriggerForEachShot() const
{
	return true;
}

int	AvHAcidRocketGun::GetShootAnimation() const
{
	return 16;
}

char* AvHAcidRocketGun::GetViewModel() const
{
	return kLevel4ViewModel;
}

void AvHAcidRocketGun::Precache()
{
	AvHAlienWeapon::Precache();

	PRECACHE_UNMODIFIED_MODEL(kAcidRocketProjectileModel);
	PRECACHE_UNMODIFIED_SOUND(kAcidRocketFireSound);
	PRECACHE_UNMODIFIED_SOUND(kAcidRocketHitSound);

	this->mEvent = PRECACHE_EVENT(1, kAcidRocketShootEventName);
}


void AvHAcidRocketGun::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 

	Precache();

	this->m_iId = AVH_WEAPON_ACIDROCKET;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsAcidRocketGun);

	SET_MODEL(ENT(this->pev), kNullModel);

	FallInit();// get ready to fall down.
} 

