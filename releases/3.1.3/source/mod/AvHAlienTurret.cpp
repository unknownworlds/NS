//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAlienTurret.cpp $
// $Date: 2002/11/22 21:28:15 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAlienTurret.cpp,v $
// Revision 1.8  2002/11/22 21:28:15  Flayra
// - mp_consistency changes
//
// Revision 1.7  2002/10/24 21:20:49  Flayra
// - Alien turrets now credit their owner with kills
//
// Revision 1.6  2002/09/23 22:08:14  Flayra
// - Removed damage upgrades for alien turrets (used to be needed for offensive upgrades)
//
// Revision 1.5  2002/07/24 18:45:40  Flayra
// - Linux and scripting changes
//
// Revision 1.4  2002/07/23 16:57:05  Flayra
// - Alien turret refactoring and fixing (the view offset in spawn() was causing them to always miss crouched players)
//
// Revision 1.3  2002/07/01 21:14:21  Flayra
// - Added auto-building, added damage upgrade (from primal scream), added vertical FOV (doesn't work yet)
//
// Revision 1.2  2002/06/03 16:24:08  Flayra
// - Moved chamber firing into an event, added turret constants
//
// Revision 1.1  2002/05/28 17:12:17  Flayra
// - Offensive chamber that shoots spit
//
//===============================================================================
#include "mod/AvHAlienTurret.h"
#include "mod/AvHConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHAlienWeapons.h"
#include "common/hldm.h"
#include "common/event_api.h"
#include "common/event_args.h"
#include "common/vector_util.h"
#include "mod/AvHGamerules.h"
#include "util/MathUtil.h"

// Temporary
#include "mod/AvHMarineTurret.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHConstants.h"
#include "mod/AvHPlayerUpgrade.h"



LINK_ENTITY_TO_CLASS(kwOffenseChamber, AvHAlienTurret);

#ifdef AVH_SERVER

LINK_ENTITY_TO_CLASS(kwSpikeProjectile, AvHSpike);

void AvHSpike::Precache(void)
{
	CBaseEntity::Precache();
	
	PRECACHE_UNMODIFIED_MODEL(kSpikeProjectileModel);
}

void AvHSpike::SetDamage(float inDamage)
{
	this->mDamage = inDamage;
}

void AvHSpike::Spawn()
{
	this->Precache();
	CBaseEntity::Spawn();
	
	this->pev->movetype = MOVETYPE_FLY;
	this->pev->classname = MAKE_STRING(kSpikeProjectileClassName);
	
	SET_MODEL(ENT(this->pev), kSpikeProjectileModel);
	this->pev->solid = SOLID_BBOX;
	this->mDamage = 0.0f;
	
	// Comment out effects line, uncomment next four, then comment out creation of temp entity in EV_SpikeGun to see server side Spike for testing
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
	
	SetTouch(&AvHSpike::SpikeTouch);
	
	// Enforce short range
	SetThink(&AvHSpike::SpikeDeath);
	this->pev->nextthink = gpGlobals->time + kSpikeLifetime;
}

void AvHSpike::SpikeDeath()
{
	// Kill the Spike entity
	UTIL_Remove(this);
	
	//	SetThink(SUB_Remove);
	//	this->pev->nextthink = gpGlobals->time + 0.01f;
}

void AvHSpike::SpikeTouch(CBaseEntity* pOther)
{
	CBaseEntity* theSpikeOwner = CBaseEntity::Instance(this->pev->owner);
	if((pOther != theSpikeOwner) && (theSpikeOwner != NULL))
	{
		float theScalar = 1.0f;
		if(GetGameRules()->CanEntityDoDamageTo(this, pOther, &theScalar))
		{
			float theDamage = this->mDamage*theScalar;

			// Give credit to the spike owner's owner (spike's owner is OC, OC's owner is player)
			CBaseEntity* theSpikeOwnerOwner = NULL;
			entvars_t* theSpikeOwnerOwnerEntVars = NULL;
			if(theSpikeOwner)
			{
				AvHTurret* theTurret = dynamic_cast<AvHTurret*>(theSpikeOwner);
				if(theTurret)
				{
					theSpikeOwnerOwner = theTurret->GetAttacker();
				}
			}

			if(!theSpikeOwnerOwner)
			{
				theSpikeOwnerOwner = theSpikeOwner;
			}

			if(theSpikeOwnerOwner)
			{
				theSpikeOwnerOwnerEntVars = theSpikeOwnerOwner->pev;
			}

			// Apply damage to receiver
			pOther->TakeDamage(theSpikeOwner->pev, theSpikeOwnerOwnerEntVars, theDamage, NS_DMG_LIGHT);
		}
		
		// Kill it off
		this->SpikeDeath();
	}
}
#endif


AvHAlienTurret::AvHAlienTurret() : AvHTurret(TECH_OFFENSE_CHAMBER, ALIEN_BUILD_OFFENSE_CHAMBER, kwsOffenseChamber, AVH_USER3_OFFENSE_CHAMBER)
{
}

AvHAlienTurret::AvHAlienTurret(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inUser3) : AvHTurret(inTechID, inMessageID, inClassName, inUser3)
{
}

// Energy from movement chambers subtracts off the rate of fire
bool AvHAlienTurret::Energize(float inEnergyAmount)
{
	bool theSuccess = false;

	if(this->GetIsBuilt() && (this->mEnergy < 1.0f))
	{
		if(this->m_hEnemy != NULL)
		{
			this->mEnergy = max(0.0f, min(1.0f, this->mEnergy + inEnergyAmount));
			theSuccess = true;
		}
	}

	return theSuccess;
}

char* AvHAlienTurret::GetDeploySound() const
{
	return kAlienTurretDeploy;
}

bool AvHAlienTurret::GetIsOrganic() const
{
	return true;
}

int AvHAlienTurret::GetPointValue(void) const
{
	return 2;
}

int	AvHAlienTurret::GetXYRange() const
{
	return 700;
}

void AvHAlienTurret::Precache()
{
	PRECACHE_UNMODIFIED_MODEL(kOffenseChamberModel);
	PRECACHE_UNMODIFIED_MODEL(kSpikeProjectileModel);
	PRECACHE_UNMODIFIED_SOUND(kAlienTurretFire1);
	PRECACHE_UNMODIFIED_SOUND(kAlienTurretDeploy);
	PRECACHE_UNMODIFIED_MODEL(kAlienTurretSprite);
	this->mEvent = PRECACHE_EVENT(1, kOffenseChamberEventName);
}

int	AvHAlienTurret::GetVerticalFOV() const
{
	return AvHTurret::GetVerticalFOV();
}

void AvHAlienTurret::PreBuiltThink()
{
	if(!this->GetIsBuilt())
		this->UpdateAutoBuild(kAlienBuildingThinkInterval*kAutoBuildScalar);
	else
		this->SetHasBeenBuilt();

	this->pev->nextthink = gpGlobals->time + kAlienBuildingThinkInterval;
}


void AvHAlienTurret::Shoot(const Vector &inOrigin, const Vector &inToEnemy, const Vector& inVecEnemyVelocity)
{
	// Spawn spike
	AvHSpike* theSpike = GetClassPtr((AvHSpike*)NULL );
	theSpike->Spawn();

	// Make it invisible
	if(!GetGameRules()->GetDrawInvisibleEntities())
	{
		theSpike->pev->effects = EF_NODRAW;
	}
	else
	{
		theSpike->pev->effects = 0;
		theSpike->pev->frame = 0;
		theSpike->pev->scale = 0.5;
		theSpike->pev->rendermode = kRenderTransAlpha;
		theSpike->pev->renderamt = 255;
	}

	// Predict where enemy will be when the spike hits and shoot that way
	float theTimeToReachEnemy = inToEnemy.Length()/(float)kOffenseChamberSpikeVelocity;
	Vector theEnemyPosition;
	VectorAdd(this->pev->origin, inToEnemy, theEnemyPosition);

	float theVelocityLength = inVecEnemyVelocity.Length();
	Vector theEnemyNormVelocity = inVecEnemyVelocity.Normalize();

	// Don't always hit very fast moving targets (jetpackers)
	const float kVelocityFactor = .7f;

	Vector thePredictedPosition;
	VectorMA(theEnemyPosition, theVelocityLength*kVelocityFactor*theTimeToReachEnemy, theEnemyNormVelocity, thePredictedPosition);

	Vector theOrigin = inOrigin;
	
	//Vector theDirToEnemy = inDirToEnemy.Normalize();

	Vector theDirToPredictedEnemy;
	VectorSubtract(thePredictedPosition, this->pev->origin, theDirToPredictedEnemy);
	Vector theDirToEnemy = theDirToPredictedEnemy.Normalize();

	//Vector theAttachOrigin, theAttachAngles;
	//GetAttachment(0, theAttachOrigin, theAttachAngles);
	
	//UTIL_SetOrigin(theSpike->pev, theStartPos);
	//VectorCopy(theStartPos, theSpike->pev->origin);
	VectorCopy(inOrigin, theSpike->pev->origin);

	// Pass this velocity to event
	int theVelocityScalar = kOffenseChamberSpikeVelocity;

	Vector theInitialVelocity;
	VectorScale(theDirToEnemy, theVelocityScalar, theInitialVelocity);
	
	// Set spike owner to OC so it doesn't collide with it
	theSpike->pev->owner = this->edict();

	// Set Spike's team :)
	theSpike->pev->team = this->pev->team;

	VectorCopy(theInitialVelocity, theSpike->pev->velocity);

	// Set amount of damage it will do
	theSpike->SetDamage(BALANCE_VAR(kOffenseChamberDamage));

	// Take into account network precision
	Vector theNetworkDirToEnemy;
	VectorScale(theDirToEnemy, 100.0f, theNetworkDirToEnemy);

	PLAYBACK_EVENT_FULL(0, this->edict(), this->mEvent, 0, theOrigin, theNetworkDirToEnemy, 1.0f, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );

	// Play attack anim
	this->PlayAnimationAtIndex(6, true);

	this->Uncloak();
}

bool AvHAlienTurret::GetBaseClassAnimatesTurret() const
{
	return false;
}

int AvHAlienTurret::MoveTurret(void)
{
	// Set animation, without overriding
	int theAnimation = this->GetIdle1Animation();
	if(RANDOM_LONG(0, 1) == 0)
	{
		theAnimation = this->GetIdle2Animation();
	}
	this->PlayAnimationAtIndex(theAnimation, false);
	
	return AvHTurret::MoveTurret();
}


int	AvHAlienTurret::GetIdle1Animation() const
{
	int theAnimation = -1;
	
	if(this->GetIsBuilt())
	{
		if(RANDOM_LONG(0, 5) == 0)
		{
			theAnimation = 4;
		}
		else
		{
			theAnimation = 2;
		}
	}
	
	return theAnimation;
}

int	AvHAlienTurret::GetIdle2Animation() const
{
	int theAnimation = -1;
	
	if(this->GetIsBuilt())
	{
		theAnimation = 3;
	}
	
	return theAnimation;
}

int	AvHAlienTurret::GetTakeDamageAnimation() const
{
    int theAnimation = -1;
    
    if(this->GetIsBuilt())
    {
        theAnimation = 7;
    }

	return theAnimation;
}

char* AvHAlienTurret::GetModelName() const
{
	return kOffenseChamberModel;
}

float AvHAlienTurret::GetRateOfFire() const
{
	return .5f + RANDOM_FLOAT(0.0f, (1.0f - this->mEnergy));
}

void AvHAlienTurret::ResetEntity()
{
	AvHTurret::ResetEntity();

	this->mEnergy = 0;
}

void AvHAlienTurret::Spawn()
{
	this->mEnergy = 0.0f;

	AvHTurret::Spawn();

	SetThink(&AvHAlienTurret::PreBuiltThink);
	this->pev->nextthink = gpGlobals->time + kAlienBuildingThinkInterval;
}

void AvHAlienTurret::SetNextAttack()
{
	AvHTurret::SetNextAttack();

	this->mEnergy = 0.0f;
}

