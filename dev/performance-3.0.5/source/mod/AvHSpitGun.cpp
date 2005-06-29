//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHSpitGun.cpp $
// $Date: 2002/11/22 21:28:17 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSpitGun.cpp,v $
// Revision 1.14  2002/11/22 21:28:17  Flayra
// - mp_consistency changes
//
// Revision 1.13  2002/08/16 02:48:10  Flayra
// - New damage model
//
// Revision 1.12  2002/07/24 19:09:17  Flayra
// - Linux issues
//
// Revision 1.11  2002/07/24 18:55:52  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.10  2002/07/24 18:45:43  Flayra
// - Linux and scripting changes
//
// Revision 1.9  2002/06/25 17:51:00  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.8  2002/06/10 19:47:16  Flayra
// - New level 2 view model
//
// Revision 1.7  2002/06/03 16:39:10  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.6  2002/05/23 02:32:57  Flayra
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
#endif

#include "mod/AvHSharedUtil.h"

LINK_ENTITY_TO_CLASS(kwSpitGun, AvHSpitGun);


#ifdef AVH_SERVER

LINK_ENTITY_TO_CLASS(kwSpitProjectile, AvHSpit);

void AvHSpit::Precache(void)
{
	CBaseEntity::Precache();

	PRECACHE_UNMODIFIED_MODEL(kSpitGunSprite);
}

void AvHSpit::SetDamage(float inDamage)
{
	this->mDamage = inDamage;
}

void AvHSpit::Spawn()
{
	this->Precache();
	CBaseEntity::Spawn();
	
	this->pev->movetype = MOVETYPE_FLY;
	this->pev->classname = MAKE_STRING(kSpitClassName);
	
	SET_MODEL(ENT(this->pev), kSpitGunSprite);
	this->pev->solid = SOLID_BBOX;
	this->mDamage = 0.0f;
	
	// Comment out effects line, uncomment next four, then comment out creation of temp entity in EV_SpitGun to see server side spit for testing
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
	UTIL_SetSize(this->pev, Vector( -kSpitSize, -kSpitSize, -kSpitSize), Vector(kSpitSize, kSpitSize, kSpitSize));
	//UTIL_SetSize(this->pev, Vector( -50, -50, -50), Vector(50, 50, 50));
	
	SetTouch(&AvHSpit::SpitTouch);

	// Enforce short range
	SetThink(&AvHSpit::SpitDeath);
	this->pev->nextthink = gpGlobals->time + kSpitLifetime;
}

void AvHSpit::SpitDeath()
{
	// Kill the spit entity
	UTIL_Remove(this);

//	SetThink(SUB_Remove);
//	this->pev->nextthink = gpGlobals->time + 0.01f;
}

void AvHSpit::SpitTouch(CBaseEntity* pOther)
{
	// Never hit the player who fired it
	if(pOther != CBaseEntity::Instance(this->pev->owner))
	{
		float theScalar = 1.0f;
		if(GetGameRules()->CanEntityDoDamageTo(this, pOther, &theScalar) || (pOther->pev->team == TEAM_IND))
		{
			float theDamage = this->mDamage*theScalar;

			// Apply damage to receiver
			pOther->TakeDamage(this->pev, VARS(this->pev->owner), theDamage, DMG_ACID);
		}

		// Kill it off
		this->SpitDeath();
	}
}
#endif

void AvHSpitGun::Init()
{
	this->mRange = kSpitGRange;
	this->mDamage = BALANCE_IVAR(kSpitGDamage);
}

int	AvHSpitGun::GetBarrelLength() const
{
	return kSpitGBarrelLength;
}

float AvHSpitGun::GetRateOfFire() const
{
	return BALANCE_FVAR(kSpitROF);
}

int	AvHSpitGun::GetDamageType() const
{
	return NS_DMG_NORMAL;
}

int	AvHSpitGun::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
	int theDeployAnimation = 7;

	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();

	switch(thePreviousID)
	{
	case AVH_WEAPON_SPIT:
		theDeployAnimation = -1;
		break;

	// Healing spray and web look the same
	case AVH_WEAPON_HEALINGSPRAY:
	case AVH_WEAPON_WEBSPINNER:
		theDeployAnimation = 7;
		break;
	}

	return theDeployAnimation;
}

bool AvHSpitGun::GetFiresUnderwater() const
{
	return true;
}

bool AvHSpitGun::GetIsDroppable() const
{
	return false;
}

int	AvHSpitGun::GetShootAnimation() const
{
	return 3;
}

void AvHSpitGun::FireProjectiles(void)
{
#ifdef AVH_SERVER
	
	// Spawn spit
	AvHSpit* theSpit = GetClassPtr((AvHSpit*)NULL );
	theSpit->Spawn();
	
	UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);

	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc = this->m_pPlayer->GetGunPosition( ) + vecAiming;
	
	UTIL_SetOrigin(theSpit->pev, vecSrc);

	// This needs to be the same as in EV_SpitGun
	Vector theBaseVelocity;
	VectorScale(this->pev->velocity, kSpitParentVelocityScalar, theBaseVelocity);
	
	Vector theStartVelocity;
	VectorMA(theBaseVelocity, kSpitVelocity, vecAiming, theStartVelocity);

	VectorCopy(theStartVelocity, theSpit->pev->velocity);

	// Set owner
	theSpit->pev->owner = ENT(this->m_pPlayer->pev);

	// Set spit's team :)
	theSpit->pev->team = this->m_pPlayer->pev->team;

	// Set amount of damage it will do
    float theFocusScalar = 1.0f;

    if(AvHSHUGetIsWeaponFocusable(AvHWeaponID(this->m_iId)))
    {
        theFocusScalar = AvHPlayerUpgrade::GetFocusDamageUpgrade(this->m_pPlayer->pev->iuser4);
    }

	float theDamage = this->mDamage*AvHPlayerUpgrade::GetAlienRangedDamageUpgrade(this->m_pPlayer->pev->iuser4)*theFocusScalar;
	theSpit->SetDamage(theDamage);

	#endif	
}

char* AvHSpitGun::GetViewModel() const
{
	return kLevel2ViewModel;
}


void AvHSpitGun::Precache()
{
	AvHAlienWeapon::Precache();

	PRECACHE_UNMODIFIED_MODEL(kNullModel);

	PRECACHE_UNMODIFIED_SOUND(kSpitGFireSound1);
	PRECACHE_UNMODIFIED_SOUND(kSpitGFireSound2);

	PRECACHE_UNMODIFIED_MODEL(kSpitGunSprite);
	PRECACHE_UNMODIFIED_SOUND(kSpitHitSound1);
	PRECACHE_UNMODIFIED_SOUND(kSpitHitSound2);

	this->mEvent = PRECACHE_EVENT(1, kSpitGEventName);
}

void AvHSpitGun::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 

	Precache();

	this->m_iId = AVH_WEAPON_SPIT;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsSpitGun);

	SET_MODEL(ENT(this->pev), kNullModel);

	FallInit();// get ready to fall down.
} 

bool AvHSpitGun::UsesAmmo(void) const
{
	return false;
}


