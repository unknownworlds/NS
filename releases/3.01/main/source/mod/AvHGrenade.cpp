//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHGrenade.cpp $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHGrenade.cpp,v $
// Revision 1.6  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.5  2002/07/24 19:09:16  Flayra
// - Linux issues
//
// Revision 1.4  2002/06/03 16:37:56  Flayra
// - Added different deploy times (this should be refactored a bit more), refactored grenades
//
// Revision 1.3  2002/05/23 02:33:42  Flayra
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

#include "dlls/util.h"

LINK_ENTITY_TO_CLASS(kwGrenade, AvHGrenade);

const int		kGrenadeVelocity                = 800;
const int		kGrenadeRollVelocity            = 350;
const float		kGrenadeParentVelocityScalar    = .4f;
const float     kGrenadeGravity                 = .8f;
const float     kGrenadeElasticity              = 0.6f;

const float     kGrenadePrimeAnimationLength    = 2.3f;
const float     kGrenadeThrowTimeBeforeRelease	= .3f;
const float     kGrenadeThrowAnimationLength    = 1.5f;

BOOL AvHGrenade::Deploy()
{
	// This has three values:
	//   0 means inactive
	//   1 means trigger throw
	//	 -1 means throw has been recently triggered
    m_flStartThrow   =  0;

	// -1 means inactive
	// UTIL_TimeBase or higher means time to throw (>= 0)
    m_flReleaseThrow = -1;

    return AvHMarineWeapon::Deploy();
}

int	AvHGrenade::GetBarrelLength() const
{
    return kGRBarrelLength;
}

float AvHGrenade::GetRateOfFire() const
{
	return kGrenadePrimeAnimationLength + kGrenadeThrowTimeBeforeRelease;
}

bool AvHGrenade::GetCanBeResupplied() const
{
    return false;
}

void AvHGrenade::Init()
{
    this->mRange = kGGRange;
    this->mDamage = BALANCE_IVAR(kHandGrenadeDamage);
}

int AvHGrenade::GetDeployAnimation() const
{
    return 5;
}

char* AvHGrenade::GetDeploySound() const
{
    return kGRDeploySound;
}

float AvHGrenade::GetDeployTime() const
{
    return AvHBasePlayerWeapon::GetDeployTime();
}

bool AvHGrenade::GetFiresUnderwater() const
{
    return true;
}

char* AvHGrenade::GetHeavyViewModel() const
{
    return kGRHVVModel;
}

int	AvHGrenade::GetIdleAnimation() const
{
	int theAnim = -1;

	if( m_flStartThrow == 0 && m_flReleaseThrow == -1)
	{
		theAnim = UTIL_SharedRandomLong(this->m_pPlayer->random_seed, 0, 2);
	}

    return theAnim;
}

bool AvHGrenade::GetIsDroppable() const
{
    return false;
}

BOOL AvHGrenade::GetIsWeaponPrimed() const
{
    return false;
}

BOOL AvHGrenade::GetIsWeaponPriming() const
{
    return false;
}

bool AvHGrenade::GetMustPressTriggerForEachShot() const
{
    return false;
}

bool AvHGrenade::ShouldRollGrenade(void) const
{
	// If player is crouched, roll grenade instead
	return (this->m_pPlayer && FBitSet(this->m_pPlayer->pev->flags, FL_DUCKING));
}

int	AvHGrenade::GetShootAnimation() const
{
    
    int theAnimation = 4;

    // If player is crouched, play roll animation
    if(this->ShouldRollGrenade())
    {
        theAnimation = 7;
    }

    return theAnimation;

}

char* AvHGrenade::GetPlayerModel() const
{
    return kGRPModel;
}

int AvHGrenade::GetPrimeAnimation() const
{
        
    int theAnimation = 3;

    // If player is crouched, play roll animation
    if(this->m_pPlayer && FBitSet(this->m_pPlayer->pev->flags, FL_DUCKING))
    {
        theAnimation = 6;
    }

    return theAnimation;

}

char* AvHGrenade::GetPrimeSound() const
{
    return kGRPrimeSound;
}

char* AvHGrenade::GetViewModel() const
{
    return kGRVModel;
}

char* AvHGrenade::GetWorldModel() const
{
    return kGRWModel;
}

void AvHGrenade::Holster(int skiplocal)
{
    
    // Important that this goes first to avoid infinite recursion when removing
    // the item from the player.

    AvHMarineWeapon::Holster(skiplocal);

    m_flStartThrow = 0;
    m_flReleaseThrow = -1;

    if(!this->m_iClip)
    {
        SetThink(&AvHGrenade::DestroyItem);
        this->pev->nextthink = gpGlobals->time + 0.1f;
    }

    EMIT_SOUND(ENT(m_pPlayer->pev), CHAN_WEAPON, "common/null.wav", 1.0, ATTN_NORM);

}


BOOL AvHGrenade::IsUseable(void)
{
    // TODO:
    return TRUE;
}


void AvHGrenade::PrimaryAttack(void)
{
 
    if (this->ProcessValidAttack())
    {

        if (!this->mAttackButtonDownLastFrame)
        {
            this->PlaybackEvent(this->mStartEvent);
            this->mAttackButtonDownLastFrame = true;
        }
    		
	    if (m_flStartThrow == 0)
	    {
		    m_flStartThrow = 1;
			
            this->PlaybackEvent(this->mEvent, this->GetPrimeAnimation());
            
            // Set the animation and sound.
	            
	        this->m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
	        this->m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

	        this->m_pPlayer->SetAnimation(PLAYER_PRIME);

			// Don't idle/fire until we've finished prime animation
            m_flTimeWeaponIdle = UTIL_WeaponTimeBase() + kGrenadePrimeAnimationLength;
	    }
    }
}

void AvHGrenade::FireProjectiles(void)
{
}

int AvHGrenade::GetEmptyShootAnimation() const
{
    return -1;
}

float AvHGrenade::GetWeaponPrimeTime() const
{ 
    return BALANCE_FVAR(kHandGrenadePrimeTime); 
}


BOOL AvHGrenade::PlayEmptySound()
{
    // None
    return 0;
}

void AvHGrenade::Precache(void)
{
    AvHMarineWeapon::Precache();
    
    PRECACHE_UNMODIFIED_SOUND(kGRFireSound1);
    PRECACHE_UNMODIFIED_SOUND(kGRDeploySound);
    PRECACHE_UNMODIFIED_SOUND(kGRExplodeSound);
    PRECACHE_UNMODIFIED_SOUND(kGRHitSound);
    
    this->mEvent = PRECACHE_EVENT(1, kGREventName);
}

bool AvHGrenade::Resupply()
{
    return false;
}

void AvHGrenade::SetNextIdle(void)
{
    // Idle is treated specially for grenade
    if( m_flStartThrow == 0 && m_flReleaseThrow == -1)
    {
        AvHMarineWeapon::SetNextIdle();
    }
}

void AvHGrenade::Spawn()
{
    AvHMarineWeapon::Spawn();
    
    Precache();
    
    this->m_iId = AVH_WEAPON_GRENADE;
    this->m_iDefaultAmmo = 1;
    
    // Set our class name
    this->pev->classname = MAKE_STRING(kwsGrenade);
    
    SET_MODEL(ENT(this->pev), kGRWModel);
    
    FallInit();// get ready to fall down.
}

bool AvHGrenade::UsesAmmo(void) const
{
    return true;
}

BOOL AvHGrenade::UseDecrement(void)
{
    return true;
}

void AvHGrenade::CreateProjectile()
{
	#ifdef AVH_SERVER

	// Set position and velocity like we do in client event
	vec3_t theStartPosition;
	UTIL_MakeVectors(this->m_pPlayer->pev->v_angle + this->m_pPlayer->pev->punchangle);
	VectorMA(this->m_pPlayer->GetGunPosition(), kGRBarrelLength, gpGlobals->v_forward, theStartPosition);
	
	// Offset it to the right a bit, so it emanates from your hand instead of the center of your body
	VectorMA(theStartPosition, 5, gpGlobals->v_right, theStartPosition);
	VectorMA(theStartPosition, 8, gpGlobals->v_up, theStartPosition);
	
	// Inherit player velocity for extra skill and finesse
	
	Vector theVelocity;
	Vector theInheritedVelocity;
	VectorScale(this->m_pPlayer->pev->velocity, kGrenadeParentVelocityScalar, theInheritedVelocity);
	
	if(!this->ShouldRollGrenade())
	{
		VectorMA(theInheritedVelocity, kGrenadeVelocity, gpGlobals->v_forward, theVelocity);
	}
	else
	{
		Vector theTossVelocity(0, 0, 40);
		VectorAdd(theInheritedVelocity, theTossVelocity, theInheritedVelocity);
		VectorMA(theInheritedVelocity, kGrenadeRollVelocity, gpGlobals->v_forward, theVelocity);
	}

	// How to handle this?  Only generate entity on server, but we should do SOMETHING on the client, no?
	CGrenade* theGrenade = AvHSUShootServerGrenade(this->m_pPlayer->pev, theStartPosition, theVelocity, BALANCE_IVAR(kGrenDetonateTime), true);
	ASSERT(theGrenade);

	theGrenade->pev->dmg = this->mDamage;

	// Make the grenade not very bouncy
	theGrenade->pev->gravity = kGrenadeGravity;
	theGrenade->pev->friction = 1 - kGrenadeElasticity;

	SET_MODEL(ENT(theGrenade->pev), this->GetWorldModel());

	theGrenade->pev->avelocity.x = RANDOM_LONG(-300, -200);
	
	// Rotate the grenade to the orientation it would be if it was thrown.
	VectorCopy(m_pPlayer->pev->angles, theGrenade->pev->angles);
	theGrenade->pev->angles[1] += 100;

	#endif
}

void AvHGrenade::ItemPostFrame(void)
{
	AvHMarineWeapon::ItemPostFrame();
	float theTimeBase = UTIL_WeaponTimeBase();

	if(this->m_flReleaseThrow > theTimeBase)
	{
		float theClientTimePassedThisTick = this->GetTimePassedThisTick();
		this->m_flReleaseThrow = max(this->m_flReleaseThrow - theClientTimePassedThisTick, theTimeBase);
	}
}

void AvHGrenade::WeaponIdle()
{

    if ( m_flTimeWeaponIdle > UTIL_WeaponTimeBase() )
    {
		return;
    }

	if (m_flStartThrow == 1)
	{
		// Throw it
		this->PlaybackEvent(this->mEvent, GetShootAnimation());

		// Set the animation and sound.
		this->m_pPlayer->m_iWeaponVolume = NORMAL_GUN_VOLUME;
		this->m_pPlayer->m_iWeaponFlash = NORMAL_GUN_FLASH;

		this->m_pPlayer->SetAnimation(PLAYER_ATTACK1);

		// Set time to shoot projectile, so it looks right with throw animation
		float theTimeToCreateGrenade = UTIL_WeaponTimeBase() + kGrenadeThrowTimeBeforeRelease;
		m_flReleaseThrow = theTimeToCreateGrenade;

		m_flStartThrow = -1;
	}
	else if ((m_flStartThrow == -1) && (m_flReleaseThrow == UTIL_WeaponTimeBase()))
	{
		this->CreateProjectile();

		this->DeductCostForShot();

		// Finish throw animation
		float theAnimationEnd = UTIL_WeaponTimeBase() + kGrenadeThrowAnimationLength;
		m_flNextSecondaryAttack = theAnimationEnd;
		m_flNextPrimaryAttack = theAnimationEnd;
		m_flTimeWeaponIdle = theAnimationEnd;

		// We've finished the throw, don't do it again (set both inactive)
        m_flStartThrow = 0;
		m_flReleaseThrow = -1;
	}
    else
    {
        AvHMarineWeapon::WeaponIdle();

		if(!this->m_iClip)
        {
            this->m_pPlayer->SelectLastItem();
        }
    }
}
