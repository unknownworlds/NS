//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Base NS turret, used for all types of automated enemy detection
//
// $Workfile: AvHTurret.cpp$
// $Date: 2002/11/22 21:24:59 $
//
//-------------------------------------------------------------------------------
// $Log: AvHTurret.cpp,v $
// Revision 1.12  2002/11/22 21:24:59  Flayra
// - Fixed turret factory abuse, where turrets became active after recycling the nearby turret factory before turret was fully contructed.
// - Fixed bug where siege turrets became re-activated after building a regular turret factory nearby.
//
// Revision 1.11  2002/11/06 01:39:21  Flayra
// - Turrets now need an active turret factory to be active
//
// Revision 1.10  2002/10/03 19:10:19  Flayra
// - Turret intervals sped up again
//
// Revision 1.9  2002/09/25 20:52:24  Flayra
// - Performance improvements
//
// Revision 1.8  2002/09/23 22:36:37  Flayra
// - Turrets now reacquire closer targets intermittently
//
// Revision 1.7  2002/08/31 18:01:03  Flayra
// - Work at VALVe
//
// Revision 1.6  2002/08/16 02:49:00  Flayra
// - New damage model
//
// Revision 1.5  2002/07/23 17:34:07  Flayra
// - Turrets track range in 2D, turrets can not require LOS if desired (for siege)
//
// Revision 1.4  2002/07/01 21:23:00  Flayra
// - Added generic vertical FOV to allow alien turrets to shoot very high and low
//
// Revision 1.3  2002/06/03 17:02:30  Flayra
// - Reduced turret volume
//
// Revision 1.2  2002/05/28 18:11:59  Flayra
// - Put in slower, randomish rate of fire for turrets for sound variance and drama, don't play ping if no ping sound specified (crashing with offensive tower)
//
// Revision 1.1  2002/05/23 02:32:57  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHTurret.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHCloakable.h"
#include "util/MathUtil.h"

const float kPingInterval = 3.0f;
const float kPingVolume = .4f;
const float kTurretThinkInterval = .05f;
const float kTurretSearchScalar = kTurretThinkInterval*(-90.0f/180)*M_PI;
const float kTurretUpdateEnemyInterval = .25f;

AvHTurret::AvHTurret(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inUser3) : AvHBaseBuildable(inTechID, inMessageID, inClassName, inUser3)
{
	this->Init();
}

void AvHTurret::Init()
{
	this->m_hEnemy = 0;
	this->m_flFieldOfView = 0;
	this->mTimeOfLastAttack = -1;
	this->mTimeOfNextAttack = -1;
    this->mTimeOfLastUpdateEnemy = -1;
	
	this->m_fTurnRate = 0;
	
	this->mGoalQuat = this->mCurQuat = Quat(0, 0, 0, 1);
	
	this->mNextPingTime = 0;
	
	this->mEnabled = false;
}

void AvHTurret::ResetEntity()
{
	AvHBaseBuildable::ResetEntity();

	this->Init();

	this->Setup();
}

char* AvHTurret::GetActiveSound() const
{
	return NULL;
}

char* AvHTurret::GetAlertSound() const
{
	return NULL;
}

char* AvHTurret::GetPingSound() const
{
	return NULL;
}

int AvHTurret::GetPointValue(void) const
{
	return BALANCE_IVAR(kScoringTurretValue);
}

int AvHTurret::GetSetEnabledAnimation() const
{
    return -1;
}

int	AvHTurret::GetActiveAnimation() const
{
	return 1;
}

int	AvHTurret::GetSpawnAnimation() const
{
	return 0;
}

int	AvHTurret::GetDeployAnimation() const
{
	return -1;
}

bool AvHTurret::GetEnabledState() const
{
	return this->mEnabled;
}

int	AvHTurret::GetTakeDamageAnimation() const
{
	return -1;
}

int	AvHTurret::GetIdle1Animation() const
{
	return -1;
}

int	AvHTurret::GetIdle2Animation() const
{
	return -1;
}

int	AvHTurret::GetKilledAnimation() const
{
	return 5;
}

bool AvHTurret::GetRequiresLOS() const
{
	return true;
}

int	AvHTurret::GetDamageType() const
{
	// Turrets to half damage to heavy players
	return DMG_BULLET | DMG_NEVERGIB | NS_DMG_LIGHT;
}

int AvHTurret::IRelationship (CBaseEntity* inTarget)
{
	int theRelationship = R_NO;

	// Ignore the roaches for heaven's sake
	if(!FClassnameIs( inTarget->pev, "monster_cockroach" ))
	{
		// Don't shoot at cloaked players
		AvHCloakable* theCloakable = dynamic_cast<AvHCloakable*>(inTarget);
		if(theCloakable && theCloakable->GetIsCloaked())
		{
			theRelationship = R_NO;
		}
		else
		{
			// Shoot all monsters that aren't on our team
			CBaseMonster* theMonsterPointer = dynamic_cast<CBaseMonster*>(inTarget);
			if(theMonsterPointer && (theMonsterPointer->pev->team != this->pev->team))
			{
				theRelationship = R_DL;
			}
			else
			{
				// Look at own team vs. incoming team
				AvHTeamNumber inTeam = (AvHTeamNumber)inTarget->pev->team;
				if(inTeam != TEAM_IND)
				{
					if(inTeam == this->pev->team)
					{
						theRelationship = R_AL;
					}
					else
					{
						// Don't keep switching targets constantly
						theRelationship = R_DL;
					}
				}
				else
				{
					//theRelationship = CSentry::IRelationship(inTarget);
				}
			}
		}
	}
	
	return theRelationship;
}

bool AvHTurret::GetIsValidTarget(CBaseEntity* inEntity) const
{
	bool theTargetIsValid = false;
	
	if((inEntity->pev->team != this->pev->team) && (inEntity->pev->team != 0) && (inEntity->pev->takedamage))
	{
		float theDistanceToCurrentEnemy = AvHSUEyeToBodyXYDistance(this->pev, inEntity);
		if(theDistanceToCurrentEnemy <= this->GetXYRange())
		{
			// Players are targettable when 
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inEntity);
			if(!thePlayer || !thePlayer->GetIsCloaked() || !thePlayer->GetIsBeingDigested()) //added digestion - elven
			{
				// TODO: Check to be sure enemy is still visible
				theTargetIsValid = true;
			}
			else
			{
				AvHCloakable* theCloakable = dynamic_cast<AvHCloakable*>(inEntity);
				if(!theCloakable || (theCloakable->GetOpacity() > 0.0f))
				{
					theTargetIsValid = true;
				}
			}
		}
	}

	return theTargetIsValid;
}

void AvHTurret::Ping(void)
{
	// Make the pinging noise every second while searching
	if(this->mNextPingTime == 0)
	{
		this->mNextPingTime = gpGlobals->time + kPingInterval;
	}
	else if(this->mNextPingTime <= gpGlobals->time)
	{
		char* thePingSound = this->GetPingSound();
		if(thePingSound)
		{
			this->mNextPingTime = gpGlobals->time + kPingInterval;
			EMIT_SOUND(ENT(this->pev), CHAN_ITEM, thePingSound, kPingVolume, ATTN_STATIC);
			//EyeOn( );
		}
	}
	//	else if (m_eyeBrightness > 0)
	//	{
	//		EyeOff( );
	//	}
}

//=========================================================
// FInViewCone - returns true is the passed ent is in
// the caller's forward view cone. The dot product is performed
// in 2d, making the view cone infinitely tall. 
//=========================================================
BOOL AvHTurret::FInViewCone(CBaseEntity* inEntity)
{
	Vector2D	vec2LOS;
	float	flDot;
	
	UTIL_MakeVectors ( pev->angles );
	
	vec2LOS = ( inEntity->pev->origin - pev->origin ).Make2D();
	vec2LOS = vec2LOS.Normalize();
	
	flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );
	
	if ( flDot > this->m_flFieldOfView )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//=========================================================
// FInViewCone - returns true is the passed vector is in
// the caller's forward view cone. The dot product is performed
// in 2d, making the view cone infinitely tall. 
//=========================================================
BOOL AvHTurret::FInViewCone(Vector* inOrigin)
{
	Vector2D	vec2LOS;
	float		flDot;

	UTIL_MakeVectors ( pev->angles );
	
	vec2LOS = ( *inOrigin - pev->origin ).Make2D();
	vec2LOS = vec2LOS.Normalize();
	
	flDot = DotProduct (vec2LOS , gpGlobals->v_forward.Make2D() );
	
	if ( flDot > this->m_flFieldOfView )
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}

}

// This function takes a lot of CPU, so make sure it's not called often!  Don't call this function directly, use UpdateEnemy instead whenever possible.
CBaseEntity* AvHTurret::FindBestEnemy()
{
    PROFILE_START()
	CBaseEntity* theEntityList[100];
	
	int theMaxRange = this->GetXYRange();
	
	Vector delta = Vector(theMaxRange, theMaxRange, theMaxRange);
	CBaseEntity* theCurrentEntity = NULL;
	CBaseEntity* theBestPlayer = NULL;
	CBaseEntity* theBestStructure = NULL;

	float theCurrentEntityRange = 100000;
	
	// Find only monsters/clients in box, NOT limited to PVS
	int theCount = UTIL_EntitiesInBox(theEntityList, 100, this->pev->origin - delta, this->pev->origin + delta, FL_CLIENT | FL_MONSTER);
	for(int i = 0; i < theCount; i++ )
	{
		theCurrentEntity = theEntityList[i];
		if((theCurrentEntity != this) && theCurrentEntity->IsAlive())
		{
			// the looker will want to consider this entity
			// don't check anything else about an entity that can't be seen, or an entity that you don't care about.
			if(this->IRelationship(theCurrentEntity ) != R_NO && FInViewCone(theCurrentEntity) && !FBitSet(theCurrentEntity->pev->flags, FL_NOTARGET))
			{
				AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theCurrentEntity);
				if(!thePlayer || thePlayer->GetCanBeAffectedByEnemies())
				{
					if(this->GetIsValidTarget(theCurrentEntity))
					{
						// Find nearest enemy
						float theRangeToTarget = VectorDistance2D(this->pev->origin, theCurrentEntity->pev->origin);
						if(theRangeToTarget < theCurrentEntityRange)
						{
                            // FVisible is expensive, so defer until necessary
                            if(!this->GetRequiresLOS() || FVisible(theCurrentEntity))
                            {
                                theCurrentEntityRange = theRangeToTarget;
								if ( thePlayer ) 
								{
									theBestPlayer = theCurrentEntity;
								}
								else
								{
									theBestStructure = theCurrentEntity;
								}
                            }
						}
					}
				}
			}
		}
	}
    PROFILE_END(kAvHTurretFindBestEnemy);

	return (theBestPlayer != NULL ) ? theBestPlayer : theBestStructure;
}

// Return degrees from center turret can aim up or down
int	AvHTurret::GetVerticalFOV() const
{
	return 30;
}

void AvHTurret::UpdateEnemy()
{
	// If enabled
	if(this->mEnabled)
	{
		// If time to find new enemy
        float theCurrentTime = gpGlobals->time;

		if((this->mTimeOfLastUpdateEnemy == -1) || (theCurrentTime > (this->mTimeOfLastUpdateEnemy + kTurretUpdateEnemyInterval)))
		{
			// Find new best enemy
			this->m_hEnemy = this->FindBestEnemy();
            this->mTimeOfLastUpdateEnemy = theCurrentTime;
		}
	}
	else
	{
		// Clear current enemy
		this->m_hEnemy = NULL;
	}
}

void AvHTurret::ActiveThink(void)
{
    PROFILE_START()

	// Advance model frame
	StudioFrameAdvance();

	// Find enemy, or reacquire dead enemy
	this->UpdateEnemy();

	// If we have a valid enemy
	if(!FNullEnt(this->m_hEnemy))
	{
		// If enemy is in FOV
		Vector theVecMid = this->pev->origin + this->pev->view_ofs;
		//AvHSUPlayParticleEvent("JetpackEffect", this->edict(), theVecMid);

		CBaseEntity* theEnemyEntity = this->m_hEnemy;
		Vector theVecMidEnemy = theEnemyEntity->BodyTarget(theVecMid);

		//AvHSUPlayParticleEvent("JetpackEffect", theEnemyEntity->edict(), theVecMidEnemy);

		// calculate dir and dist to enemy
		Vector theVecDirToEnemy = theVecMidEnemy - theVecMid;
		Vector theAddition = theVecMid + theVecDirToEnemy;

		Vector theVecLOS = theVecDirToEnemy.Normalize();

		// Update our goal angles to direction to enemy
		Vector theVecDirToEnemyAngles;
		VectorAngles(theVecDirToEnemy, theVecDirToEnemyAngles);

		// Set goal quaternion
		this->mGoalQuat = Quat(theVecDirToEnemyAngles);

		// Is the turret looking at the target yet?
		float theRadians = (this->GetVerticalFOV()/180.0f)*3.1415f;
		float theCosVerticalFOV = cos(theRadians);

		Vector theCurrentAngles;
		this->mCurQuat.GetAngles(theCurrentAngles);
		UTIL_MakeAimVectors(theCurrentAngles);

		if(DotProduct(theVecLOS, gpGlobals->v_forward) > theCosVerticalFOV)
		{
			// If enemy is visible
			bool theEnemyVisible = FBoxVisible(this->pev, this->m_hEnemy->pev, theVecMidEnemy) || !this->GetRequiresLOS();
			if(theEnemyVisible && this->m_hEnemy->IsAlive())
			{
				// If it's time to attack
				if((this->mTimeOfNextAttack == -1) || (gpGlobals->time >= this->mTimeOfNextAttack))
				{
					// Shoot and play shoot animation
					Shoot(theVecMid, theVecDirToEnemy, theEnemyEntity->pev->velocity);
					
					this->PlayAnimationAtIndex(this->GetActiveAnimation());
					
					// Set time for next attack
					this->SetNextAttack();
				}
				// spin the barrel when acquired but not firing
				else if(this->GetBaseClassAnimatesTurret())
				{
					this->pev->sequence = 2;
					ResetSequenceInfo();
				}
			}
		}

		// Set next active think
		this->pev->nextthink = gpGlobals->time + kTurretThinkInterval;
	}
	// else we have no enemy, go back to search think
	else
	{
		SetThink(&AvHTurret::SearchThink);
		this->pev->nextthink = gpGlobals->time + kTurretThinkInterval;
	}

	this->TurretUpdate();

    PROFILE_END(kAvHTurretActiveThink)
}

bool AvHTurret::GetBaseClassAnimatesTurret() const
{
	return true;
}

void AvHTurret::SearchThink(void)
{
    PROFILE_START()
        
	if(this->GetBaseClassAnimatesTurret())
	{
		this->pev->sequence = 2;
		ResetSequenceInfo();
		StudioFrameAdvance();
	}
	this->pev->nextthink = gpGlobals->time + kTurretThinkInterval;
	
	this->Ping();
	
	// If we have a target and we're still healthy
	if(this->m_hEnemy != NULL)
	{
		if(!this->m_hEnemy->IsAlive() )
		{
			this->m_hEnemy = NULL;// Dead enemy forces a search for new one
		}
	}
	
	// Acquire Target
    this->UpdateEnemy();

	// If we've found a target, spin up the barrel and start to attack
	if(this->m_hEnemy != NULL)
	{
		//this->m_flSpinUpTime = 0;
		SetThink(&AvHTurret::ActiveThink);
	}
//	else
//	{
//		// generic hunt for new victims
//		this->m_vecGoalAngles.y = (this->m_vecGoalAngles.y + 0.1*this->m_fTurnRate);
//		if(this->m_vecGoalAngles.y >= 360)
//		{
//			m_vecGoalAngles.y -= 360;
//		}
//	}

	this->TurretUpdate();
    
    PROFILE_END(kTurretSearchThink)
}

void AvHTurret::TurretUpdate()
{
	this->MoveTurret();
}

void AvHTurret::SetNextAttack()
{
	this->mTimeOfNextAttack = gpGlobals->time + this->GetRateOfFire();
}

int AvHTurret::MoveTurret(void)
{
	ASSERT(this->m_fTurnRate > 0);

	// We have an enemy, track towards goal angles
	if(this->m_hEnemy != NULL)
	{
		float theRate = this->m_fTurnRate*kTurretThinkInterval;
		this->mCurQuat = ConstantRateLerp(this->mCurQuat, this->mGoalQuat, theRate);
	}
	// generic hunt for new victims
	else
	{
		// Create transformation quat that will rotate current quat
		float axis[3] = { 0.0f, 0.0f, 1.0f};
		Quat rot(kTurretSearchScalar, axis);

		this->mCurQuat = rot*this->mCurQuat;

		// Reset height
	}

	Vector theAngles;
	this->mCurQuat.GetAngles(theAngles);

	//SetBoneController(0, m_vecCurAngles.y - pev->angles.y );
	SetBoneController(0, theAngles.y - pev->angles.y );
	SetBoneController(1, -theAngles.x);

	return 0;
}

void AvHTurret::CheckEnabledState()
{
    this->SetEnabledState(true, true);
}

void AvHTurret::SetHasBeenBuilt()
{
	AvHBuildable::SetHasBeenBuilt();
	
    this->CheckEnabledState();
}

void AvHTurret::SetEnabledState(bool inState, bool inForce)
{
	if(!GetHasUpgrade(this->pev->iuser4, MASK_RECYCLING))
	{
		if((inState != this->mEnabled) || inForce)
		{
			this->mEnabled = inState;

            if(this->GetBaseClassAnimatesTurret())
            {
                int theEnabledAnimation = this->GetSetEnabledAnimation();
                if(theEnabledAnimation >= 0)
                {
                    float theSpeed = this->mEnabled ? 1.0f : -1.0f;
                    this->PlayAnimationAtIndex(theEnabledAnimation, true, theSpeed);
                }
            }
			
			if(!this->mEnabled)
			{
				this->m_hEnemy = NULL;
				SetThink(NULL);

			}
			else
			{
                float theTimeToAnimate = max(gpGlobals->time + kTurretThinkInterval, this->GetTimeAnimationDone());
				SetThink(&AvHTurret::SearchThink);
				this->pev->nextthink = theTimeToAnimate;
			}
		}
	}
}

float AvHTurret::GetRateOfFire() const
{
	float theVariance = RANDOM_FLOAT(0, 0.2);
	float theBaseROF = BALANCE_FVAR(kTurretBaseRateOfFire);
	return theBaseROF + theVariance;
}

void AvHTurret::Setup()
{
	this->pev->nextthink = gpGlobals->time + 1;
	this->pev->frame = 0;
	this->pev->takedamage = DAMAGE_AIM;
	this->mNextPingTime = 0;
	this->m_flFieldOfView = VIEW_FIELD_FULL;
	this->m_fTurnRate = BALANCE_FVAR(kTurretTrackingRate);
	
	// This is the visual difference between model origin and gun barrel, it's needed to orient the barrel and hit targets properly
	this->pev->view_ofs.z = 48;
	
	SetBits(this->pev->flags, FL_MONSTER);
	
	this->SetBoneController(0, 0);
	this->SetBoneController(1, 0);

	this->mEnabled = false;
}

void AvHTurret::Spawn()
{
	AvHBaseBuildable::Spawn();

	this->Setup();
}

