//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHSpores.cpp $
// $Date: 2002/11/22 21:28:17 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSpores.cpp,v $
// Revision 1.13  2002/11/22 21:28:17  Flayra
// - mp_consistency changes
//
// Revision 1.12  2002/09/23 22:33:21  Flayra
// - Spores no longer hurt buildings
//
// Revision 1.11  2002/08/16 02:48:10  Flayra
// - New damage model
//
// Revision 1.10  2002/07/24 19:09:18  Flayra
// - Linux issues
//
// Revision 1.9  2002/07/24 18:55:52  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.8  2002/07/24 18:45:43  Flayra
// - Linux and scripting changes
//
// Revision 1.7  2002/06/25 17:49:01  Flayra
// - Some refactoring, new view model artwork, removed old code
//
// Revision 1.6  2002/06/03 16:39:10  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.5  2002/05/23 02:32:57  Flayra
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

#include "mod/AvHAlienWeaponConstants.h"

LINK_ENTITY_TO_CLASS(kwSporeGun, AvHSpore);

#ifdef AVH_SERVER
LINK_ENTITY_TO_CLASS(kwSporeProjectile, AvHSporeProjectile);
extern int gSporeCloudEventID;

AvHSporeProjectile::AvHSporeProjectile()
{
	this->mTimeHit = -1;
	this->mDamage = 0.0f;
}

void AvHSporeProjectile::Precache(void)
{
	CBaseEntity::Precache();
	
	PRECACHE_UNMODIFIED_MODEL(kSporeSprite);
	PRECACHE_UNMODIFIED_MODEL(kClientSporeSprite);
}

void AvHSporeProjectile::SetDamage(float inDamage)
{
	this->mDamage = inDamage;
}

void AvHSporeProjectile::Spawn(void)
{
	this->Precache();
	CBaseEntity::Spawn();
	
	this->pev->movetype = MOVETYPE_FLY;
	this->pev->classname = MAKE_STRING(kwsSporeProjectile);
	
	SET_MODEL(ENT(this->pev), kClientSporeSprite);
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
	//UTIL_SetSize(this->pev, Vector( -50, -50, -50), Vector(50, 50, 50));
	
	SetTouch(&AvHSporeProjectile::SporeTouch);
}

void AvHSporeProjectile::SporeCloudThink()
{
	// Apply damage to all enemy players in radius
	CBaseEntity* theEntity = NULL;
	while ( (theEntity = UTIL_FindEntityInSphere( theEntity, this->pev->origin, BALANCE_VAR(kSporeCloudRadius))) != NULL )
	{
		// Only hurt players
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
		if(thePlayer)
		{
			ASSERT(this->pev->team != 0);
			if(theEntity->pev->team != this->pev->team)
			{
				// Don't do damage to heavy armor
				if(!thePlayer->GetHasHeavyArmor())
				{
					// Spores don't stack, so don't do damage too often
					float theTimeOfLastSporeDamage = thePlayer->GetTimeOfLastSporeDamage();
					float kTolerance = .05f;
					if((theTimeOfLastSporeDamage == -1) || (gpGlobals->time > (theTimeOfLastSporeDamage + BALANCE_VAR(kSporeCloudThinkInterval) - kTolerance)))
					{
						// Make sure a direct line can be traced from spores to target
						TraceResult theTraceResult;
						UTIL_TraceLine(this->pev->origin, theEntity->pev->origin, ignore_monsters, dont_ignore_glass, theEntity->edict(), &theTraceResult);
						
						if(theTraceResult.flFraction == 1.0f)
						{
							theEntity->TakeDamage(this->pev, VARS(this->pev->owner), this->mDamage, NS_DMG_NORMAL);

							thePlayer->SetTimeOfLastSporeDamage(gpGlobals->time);
						}
						else
						{
							// What did we hit?
							CBaseEntity* theEntity = CBaseEntity::Instance(theTraceResult.pHit);
						}
					}
				}
			
				// TODO: Fire event?
			}
		}
	}

	// Is it time to expire?
	if(gpGlobals->time > (this->mTimeHit + BALANCE_VAR(kSporeCloudTime)))
	{
		// if so, remove entity
		SetThink(&AvHSporeProjectile::SUB_Remove);
		this->pev->nextthink = gpGlobals->time + 0.01f;
	}
	else
	{
		// if not, set next think
		this->pev->nextthink = gpGlobals->time + BALANCE_VAR(kSporeCloudThinkInterval);
	}
}

void AvHSporeProjectile::SporeTouch(CBaseEntity *pOther)
{
	// Never hit the player who fired it
	if(pOther != CBaseEntity::Instance(this->pev->owner))
	{
		// If not in tourny mode, pass through friends
		//if(GetGameRules()->GetIsTournamentMode() || (pOther->pev->team != this->pev->team))
		//{
			// Tell the projectile to stop moving 
			VectorCopy(g_vecZero, this->pev->velocity);

			this->mTimeHit = gpGlobals->time;
			SetThink(&AvHSporeProjectile::SporeCloudThink);
			this->pev->nextthink = gpGlobals->time + BALANCE_VAR(kSporeCloudThinkInterval);

			SetTouch(NULL);
			
			// Fire spore cloud event
			PLAYBACK_EVENT_FULL(0, this->edict(), gSporeCloudEventID, 0, pOther->pev->origin, (float *)&g_vecZero, 1.0f, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
		//}
	}
}
#endif



int	AvHSpore::GetBarrelLength() const
{
	return kSporeBarrelLength;
}

float AvHSpore::GetRateOfFire() const
{
    return BALANCE_VAR(kSporeROF);
}

bool AvHSpore::GetFiresUnderwater() const
{
	return true;
}

bool AvHSpore::GetIsDroppable() const
{
	return false;
}
	
void AvHSpore::Init()
{
	this->mRange = kSporeRange;
	this->mDamage = BALANCE_VAR(kSporeDamage);
}

int	AvHSpore::GetDamageType() const
{
	return NS_DMG_NORMAL;
}

int	AvHSpore::GetIdleAnimation() const
{
	return 2;
}

int	AvHSpore::GetShootAnimation() const
{
	return 5;
}

int	AvHSpore::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
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

float AvHSpore::GetDeployTime() const
{
	return .6f;
}

char* AvHSpore::GetViewModel() const
{
	return kLevel3ViewModel;
}

void AvHSpore::Precache(void)
{
	AvHAlienWeapon::Precache();
	
	PRECACHE_UNMODIFIED_SOUND(kSporeFireSound);
	PRECACHE_UNMODIFIED_SOUND(kSporeCloudSound);
	
	this->mEvent = PRECACHE_EVENT(1, kSporeShootEventName);
}

void AvHSpore::Spawn()
{
    AvHAlienWeapon::Spawn(); 
	
	Precache();
	
	this->m_iId = AVH_WEAPON_SPORES;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsSporeGun);
	
	SET_MODEL(ENT(this->pev), kNullModel);
	
	FallInit();// get ready to fall down.
	
}

bool AvHSpore::UsesAmmo(void) const
{
	return false;
}

void AvHSpore::FireProjectiles(void)
{
	#ifdef AVH_SERVER
	// Make sure we have enough points to shoot this thing
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
	ASSERT(thePlayer);
	
	//if(thePlayer->GetResources() > kSporePointCost)
	//{
	//	// Decrement kSporePointCost points
	//	thePlayer->SetResources(thePlayer->GetResources() - kSporePointCost);
		
		// Create hidden projectile that creates a huge spore cloud where it hits
		AvHSporeProjectile* theSpore = GetClassPtr((AvHSporeProjectile*)NULL );
		theSpore->Spawn();
		
		UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);
		
		Vector vecAiming = gpGlobals->v_forward;
		Vector vecSrc = this->m_pPlayer->GetGunPosition( ) + vecAiming;
		
		UTIL_SetOrigin(theSpore->pev, vecSrc);
		
//		// This needs to be the same as in EV_SporeGun
//		Vector theBaseVelocity;
//		VectorScale(this->pev->velocity, kSporeParentVelocityScalar, theBaseVelocity);
//		
//		Vector theStartVelocity;
//		VectorMA(theBaseVelocity, kSporeVelocity, vecAiming, theStartVelocity);
//		
//		VectorCopy(theStartVelocity, theSpore->pev->velocity);

		VectorScale(vecAiming, kShootCloudVelocity, theSpore->pev->velocity);
		
		// Set owner
		theSpore->pev->owner = ENT(this->m_pPlayer->pev);
		
		// Set spore's team :)
		theSpore->pev->team = this->m_pPlayer->pev->team;

		// Set the spore damage
		float theDamage = this->mDamage*AvHPlayerUpgrade::GetAlienRangedDamageUpgrade(this->m_pPlayer->pev->iuser4);
		theSpore->SetDamage(theDamage);
	//}
	#endif	
}

