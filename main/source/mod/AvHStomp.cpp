//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHStomp.cpp $
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
#include "mod/AvHHulls.h"
#include "mod/AvHSharedUtil.h"

LINK_ENTITY_TO_CLASS(kwStomp, AvHStomp);

#ifdef AVH_SERVER
LINK_ENTITY_TO_CLASS(kwStompProjectile, AvHStompProjectile);

AvHStompProjectile::AvHStompProjectile()
{
}
	
void AvHStompProjectile::Precache(void)
{
	CBaseEntity::Precache();
}

void AvHStompProjectile::Spawn(void)
{
	this->Precache();

	CBaseEntity::Spawn();
	
	this->pev->movetype = MOVETYPE_NOCLIP;
	this->pev->classname = MAKE_STRING(kwsStompProjectile);
	
	SET_MODEL(ENT(this->pev), kStompProjectileModel);
	this->pev->solid = SOLID_TRIGGER;
	
	if(!GetGameRules()->GetDrawInvisibleEntities())
	{
		this->pev->effects = EF_NODRAW;
	}
	else
	{
		this->pev->frame = 0;
		this->pev->scale = 1.0;
		this->pev->rendermode = kRenderTransAdd;
		this->pev->renderamt = kStompModelRenderAmount;
	}

	const int kBoxWidth = 50;
	UTIL_SetSize(this->pev, Vector(-kBoxWidth, -kBoxWidth, -kBoxWidth), Vector(kBoxWidth, kBoxWidth, kBoxWidth));
//	UTIL_SetSize(this->pev, Vector( 0, 0, 0), Vector(0, 0, 0));

	SetTouch(&AvHStompProjectile::StompTouch);

	SetThink(&CBaseEntity::SUB_Remove);
	this->pev->nextthink = gpGlobals->time + kStompProjectileLifetime;

	this->mStunTime = 0.0f;
}

void AvHStompProjectile::SetStunTime(float inStunTime)
{
	this->mStunTime = inStunTime;
}
	
void AvHStompProjectile::StompTouch(CBaseEntity* inOther)
{
	if(!AvHSUGetIsExternalClassName(STRING(inOther->pev->classname)))
	{
		// Stop when it hits the world
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inOther);
		if(thePlayer)
		{
			if(thePlayer->GetCanBeAffectedByEnemies() && (thePlayer->GetTeam() != this->pev->team))
			{
				// Stun them if they're not stunned already, to prevent perpetual stunning
				if(!thePlayer->GetIsStunned() && !GetHasUpgrade(thePlayer->pev->iuser4, MASK_TOPDOWN))
				{
					// Don't stun jetpackers
					if(!GetHasUpgrade(thePlayer->pev->iuser4, MASK_UPGRADE_7)) //&& !(thePlayer->pev->flags & FL_ONGROUND)))
					{
						// Do a traceline to make sure the world isn't blocking it (StompTouch doesn't seem to be called for CWorld collisions)
						TraceResult theTraceResult;
						UTIL_TraceLine(this->mSpawnLocation, inOther->pev->origin, ignore_monsters, ignore_glass, NULL, &theTraceResult);

						if(theTraceResult.flFraction == 1.0f)
						{
							if(thePlayer->SetIsStunned(true, this->mStunTime))
                            {
                                // Play effect at player's feet
							    Vector theMinSize, theMaxSize;
							    thePlayer->GetSize(theMinSize, theMaxSize);
							    
							    vec3_t theOrigin = thePlayer->pev->origin;
							    theOrigin.z += theMinSize.z;
							    
							    AvHSUPlayParticleEvent(kpsStompEffect, inOther->edict(), theOrigin);
                            }
						}
						else
						{
							CBaseEntity* theEntityHit = CBaseEntity::Instance(ENT(theTraceResult.pHit));
						}
					}
				}
			}
		}
	}
}
	
#endif



BOOL AvHStomp::CanHolster(void)
{
	return true;
}

void AvHStomp::Init()
{
	this->mRange = BALANCE_VAR(kStompRadius);
}

// Only allow it to be triggered when on the ground
BOOL AvHStomp::IsUseable(void)
{
	BOOL theSuccess = AvHAlienWeapon::IsUseable();

	if(theSuccess)
	{
		if(!this->m_pPlayer || !FBitSet(this->m_pPlayer->pev->flags, FL_ONGROUND))
		{
			theSuccess = false;
		}
	}

	return theSuccess;
}

int	AvHStomp::GetBarrelLength() const
{
	return kStompBarrelLength;
}

float AvHStomp::GetRateOfFire() const
{
    return BALANCE_VAR(kStompROF);
}

int	AvHStomp::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
	int theDeployAnimation = 7;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
	case AVH_WEAPON_CLAWS:
		theDeployAnimation = 7;
		break;
	case AVH_ABILITY_CHARGE:
		theDeployAnimation = 11;
		break;
	case AVH_WEAPON_DEVOUR:
		theDeployAnimation = 20;
		break;
	}
	
	return theDeployAnimation;
}

float AvHStomp::GetDeployTime() const
{
	return .6f;
}

void AvHStomp::GetEventAngles(Vector& outAngles) const
{
	//this->GetStompVelocity(outAngles);
	AvHAlienWeapon::GetEventAngles(outAngles);
}

void AvHStomp::GetEventOrigin(Vector& outOrigin) const
{
	this->GetStompOrigin(outOrigin);
}

bool AvHStomp::GetFiresUnderwater() const
{
	return false;
}

bool AvHStomp::GetIsDroppable() const
{
	return false;
}

int AvHStomp::GetShootAnimation() const
{
	return 24;
}

void AvHStomp::FireProjectiles(void)
{
#ifdef AVH_SERVER
	// Search for relevant enemy players in range (only on ground?)
//	CBaseEntity* theEntity = NULL;
//	while((theEntity = UTIL_FindEntityInSphere(theEntity, this->pev->origin, this->mRange)) != NULL)
//	{
//		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
//		if(thePlayer && (thePlayer != this->m_pPlayer))
//		{
//			if(thePlayer->GetIsRelevant() && (thePlayer->GetTeam() != this->m_pPlayer->pev->team))
//			{
//				// Stun them if they're not stunned already, to prevent perpetual stunning
//				if(!thePlayer->GetIsStunned() && !GetHasUpgrade(thePlayer->pev->iuser4, MASK_TOPDOWN))
//				{
//					// Don't stun flying jetpackers
//					if(!(GetHasUpgrade(thePlayer->pev->iuser4, MASK_UPGRADE_7) && !(thePlayer->pev->flags & FL_ONGROUND)))
//					{
//						thePlayer->SetIsStunned(true, BALANCE_VAR(kStompTime));
//					}
//				}
//			}
//		}
//	}

	// Create stomp projectile that flies forward, stunning enemies it touches
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
	ASSERT(thePlayer);
	
	AvHStompProjectile* theStomp = GetClassPtr((AvHStompProjectile*)NULL );
	theStomp->Spawn();

	this->GetStompOrigin(theStomp->pev->origin);
	UTIL_SetOrigin(theStomp->pev, theStomp->pev->origin);

	// Save starting point
	VectorCopy(theStomp->pev->origin, theStomp->mSpawnLocation);

	this->GetStompVelocity(theStomp->pev->velocity);
	
	// Set owner
	theStomp->pev->owner = ENT(this->m_pPlayer->pev);
	
	// Set Stomp's team :)
	theStomp->pev->team = this->m_pPlayer->pev->team;

	// Set stun time
	float theStunTime = BALANCE_VAR(kStompTime);

    if(AvHSHUGetIsWeaponFocusable(AvHWeaponID(this->m_iId)))
    {
        theStunTime *= AvHPlayerUpgrade::GetFocusDamageUpgrade(this->m_pPlayer->pev->iuser4);
    }
    
	theStomp->SetStunTime(theStunTime);

	// Play view shake here
	float theShakeAmplitude = 100;
	float theShakeFrequency = 100;
	float theShakeDuration = 1.0f;
	float theShakeRadius = 700;
	UTIL_ScreenShake(this->pev->origin, theShakeAmplitude, theShakeFrequency, theShakeDuration, this->mRange);
#endif	
}

void AvHStomp::GetStompOrigin(Vector& outOrigin) const
{
	VectorCopy(this->m_pPlayer->pev->origin, outOrigin);
	
	#ifdef AVH_SERVER
	float theZOffset = this->m_pPlayer->pev->mins.z;
	outOrigin.z += theZOffset;
	#endif
}

void AvHStomp::GetStompVelocity(Vector& outVelocity) const
{
	// Send shockwave in direction player is looking, but always make it go kStompProjectilVelocity
	UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);
	
	// Zero out z velocity so it stays on the ground
	Vector theAim = gpGlobals->v_forward;
	theAim.z = 0;
	
	VectorNormalize(theAim);

	VectorScale(theAim, kStompProjectileVelocity, outVelocity);
}



char* AvHStomp::GetViewModel() const
{
	return kLevel5ViewModel;
}

void AvHStomp::Precache()
{
	AvHAlienWeapon::Precache();

	PRECACHE_UNMODIFIED_SOUND(kStompFireSound);
	PRECACHE_UNMODIFIED_MODEL(kStompProjectileModel);

	this->mEvent = PRECACHE_EVENT(1, kStompShootEventName);
}

void AvHStomp::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 

	Precache();

	this->m_iId = AVH_WEAPON_STOMP;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsStomp);

	SET_MODEL(ENT(this->pev), kNullModel);

	FallInit();// get ready to fall down.
} 

bool AvHStomp::UsesAmmo(void) const
{
	return false;
}

