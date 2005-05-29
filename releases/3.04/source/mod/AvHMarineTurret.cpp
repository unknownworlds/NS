//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Marine sentry turret.
//
// $Workfile: AvHMarineTurret.cpp$
// $Date: 2002/11/22 21:25:26 $
//
//-------------------------------------------------------------------------------
// $Log: AvHMarineTurret.cpp,v $
// Revision 1.11  2002/11/22 21:25:26  Flayra
// - Fixed turret factory abuse, where turrets became active after recycling the nearby turret factory before turret was fully contructed.
// - Fixed bug where siege turrets became re-activated after building a regular turret factory nearby.
// - mp_consistency changes
//
// Revision 1.10  2002/11/06 02:23:11  Flayra
// - Turrets now need an active turret factory to keep firing (work with advanced turret factories too)
//
// Revision 1.9  2002/11/06 01:39:21  Flayra
// - Turrets now need an active turret factory to be active
//
// Revision 1.8  2002/11/03 04:51:31  Flayra
// - Hard-coded gameplay constants instead of putting in skill.cfg
//
// Revision 1.7  2002/10/24 21:33:21  Flayra
// - Thought about crediting marine team for turret kills but decided against it
//
// Revision 1.6  2002/10/16 01:01:29  Flayra
// - Removed unneeded sounds
//
// Revision 1.5  2002/09/23 23:36:41  Flayra
// - Linux compatibility
//
// Revision 1.4  2002/09/23 22:22:30  Flayra
// - Marine turrets send "sentry firing" and "sentry taking damage" alerts
//
// Revision 1.3  2002/07/23 17:13:18  Flayra
// - Always draw muzzle flash, calculate range in 2D
//
// Revision 1.2  2002/07/01 21:37:26  Flayra
// - Removed turret range from skill.cfg (for visible building placement ranges)
//
// Revision 1.1  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHMarineTurret.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHParticleConstants.h"
#include "util/MathUtil.h"

LINK_ENTITY_TO_CLASS(kwDeployedTurret, AvHMarineTurret);


AvHMarineTurret::AvHMarineTurret() : AvHTurret(TECH_NULL, BUILD_TURRET, kwsDeployedTurret, AVH_USER3_TURRET)
{
}

AvHMarineTurret::AvHMarineTurret(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inUser3) : AvHTurret(inTechID, inMessageID, inClassName, inUser3)
{
}

void AvHMarineTurret::CheckEnabledState()
{
	bool theEnabledState = false;

	if(this->GetHasBeenBuilt() && !this->GetIsRecycling())
	{
		// Search for turret factories in range
		FOR_ALL_ENTITIES(kwsTurretFactory, AvHTurretFactory*)
			if((theEntity->pev->team == this->pev->team) && theEntity->GetIsBuilt() && !GetHasUpgrade(theEntity->pev->iuser4, MASK_RECYCLING))
			{
				// If they are a friendly, alive, turret factory
				float the2DDistance = VectorDistance2D(this->pev->origin, theEntity->pev->origin);

				// Enabled state is true
				if(the2DDistance <= BALANCE_IVAR(kTurretFactoryBuildDistance))
				{
					theEnabledState = true;
					break;
				}
			}
		END_FOR_ALL_ENTITIES(kwsTurretFactory)

		if(!theEnabledState)
		{
			FOR_ALL_ENTITIES(kwsAdvancedTurretFactory, AvHTurretFactory*)
			if((theEntity->pev->team == this->pev->team) && theEntity->GetIsBuilt() && !GetHasUpgrade(theEntity->pev->iuser4, MASK_RECYCLING))
			{
				// If they are a friendly, alive, turret factory
				float the2DDistance = VectorDistance2D(this->pev->origin, theEntity->pev->origin);
				
				// Enabled state is true
				if(the2DDistance <= BALANCE_IVAR(kTurretFactoryBuildDistance))
				{
					theEnabledState = true;
					break;
				}
			}
			END_FOR_ALL_ENTITIES(kwsAdvancedTurretFactory)
		}
	}
	
	// Set enabled state
	this->SetEnabledState(theEnabledState);
}

int AvHMarineTurret::GetSetEnabledAnimation() const
{
    return 3;
}

char* AvHMarineTurret::GetDeploySound() const
{
	return kTurretDeploy;
}

char* AvHMarineTurret::GetPingSound() const
{
	return kTurretPing;
}


//void AvHMarineTurret::ConstructUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
//{
//	// Only allow users from same team as turret deployer
//	if(pActivator->pev->team == this->pev->team)
//	{
//		if(this->mPercentageBuilt < 1.0f)
//		{
//			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(pActivator);
//			ASSERT(thePlayer);
//			if(thePlayer->HolsterWeaponToUse())
//			{
//				bool thePlaySound = false;
//
//				if(this->mPercentageBuilt < 1.0f)
//				{
//					//thePlayer->TriggerProgressBar(this->entindex(), 1);
//					
//					float theConstructionRate = kConstructorRate;
//					if(GetGameRules()->GetIsTesting() || GetGameRules()->GetIsDemoing())
//					{
//						theConstructionRate *= 4.0f;
//					}
//					
//					this->mPercentageBuilt += (this->mThinkInterval/1.0f)*theConstructionRate;
//				}
//				
//				if(gpGlobals->time > (this->mLastTimePlayedSound + kAverageSoundLength))
//				{
//					AvHSUPlayRandomConstructionEffect(thePlayer, this);
//					this->mLastTimePlayedSound = gpGlobals->time;
//
//					//if(this->mPercentageBuilt == 1.0f)
//					//{
//					//	thePlayer->SetCarriedResources(thePlayer->GetCarriedResources() + 1);
//					//}
//				}
//				
//				// Given the number of constructors, what's chance of starting a new sound?
//				float theChanceForNewSound = (this->mThinkInterval/(kAverageSoundLength/2.0f));
//				float theRandomFloat = RANDOM_FLOAT(0.0f, 1.0f);
//				if(theRandomFloat < theChanceForNewSound)
//				{
//					AvHSUPlayRandomConstructionEffect(thePlayer, this);
//					
//					//if(this->mPercentageBuilt == 1.0f)
//					//{
//					//	thePlayer->SetCarriedResources(thePlayer->GetCarriedResources() + 1);
//					//}
//				}
//				
//				this->pev->rendermode = kRenderTransTexture;
//				this->pev->renderamt = this->mStartAlpha + this->mPercentageBuilt*(255 - this->mStartAlpha);
//				if(this->mPercentageBuilt >= 1.0f)
//				{
//					this->SetConstructionComplete();
//				}
//				
//				//this->pev->fuser1 = this->mPercentageBuilt*kNormalizationNetworkFactor;
//				AvHSHUSetBuildResearchState(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, true, this->mPercentageBuilt);
//			}
//		}
//	}
//}

int	AvHMarineTurret::GetXYRange() const
{
	return BALANCE_IVAR(kTurretRange);
}

void AvHMarineTurret::Precache()
{
	PRECACHE_UNMODIFIED_SOUND(kTurretFire1);
	PRECACHE_UNMODIFIED_SOUND(kTurretFire2);
	PRECACHE_UNMODIFIED_SOUND(kTurretFire3);
	PRECACHE_UNMODIFIED_SOUND(kTurretFire4);
	PRECACHE_UNMODIFIED_SOUND(kTurretPing);
	PRECACHE_UNMODIFIED_SOUND(kTurretDeploy);
	
	PRECACHE_UNMODIFIED_SOUND(kTurretBuild1);
	PRECACHE_UNMODIFIED_SOUND(kTurretBuild2);
	PRECACHE_UNMODIFIED_SOUND(kTurretBuild3);
	PRECACHE_UNMODIFIED_SOUND(kTurretBuild4);
	PRECACHE_UNMODIFIED_SOUND(kTurretBuild5);
}

void AvHMarineTurret::SetEnabledState(bool inState, bool inForce)
{
	if((inState != this->GetEnabledState()) || inForce)
	{
		AvHTurret::SetEnabledState(inState, inForce);
		
		// Power down
		char* theDeploySound = this->GetDeploySound();
		if(theDeploySound)
		{
			EMIT_SOUND(ENT(this->pev), CHAN_WEAPON, theDeploySound, 1, ATTN_NORM);
		}
	}
}

int AvHMarineTurret::GetRecycleAnimation() const
{
	return this->GetSpawnAnimation();
}

void AvHMarineTurret::Shoot(const Vector &inOrigin, const Vector &inToEnemy, const Vector& inVecEnemyVelocity)
{
	AvHTeam* theTeam = GetGameRules()->GetTeam(AvHTeamNumber(this->pev->team));
	ASSERT(theTeam);

	float theDamageModifier;
	int theTracerFreq;
	int theUpgradeLevel = AvHPlayerUpgrade::GetWeaponUpgrade(this->pev->iuser3, /*theTeam->GetTeamWideUpgrades()*/this->pev->iuser4, &theDamageModifier, &theTracerFreq);

	float theDamage = (float)BALANCE_IVAR(kSentryDamage)*theDamageModifier;		

	int theDamageType = this->GetDamageType();
	Vector theDirToEnemy = inToEnemy.Normalize();
	this->FireBullets(1, inOrigin, theDirToEnemy, VECTOR_CONE_3DEGREES, this->GetXYRange(), BULLET_MONSTER_MP5, theTracerFreq, theDamage, this->GetAttacker()->pev, theDamageType);
		
	const char* theSoundToPlay = kTurretFire1;
	
	switch(theUpgradeLevel)
	{
	case 1:	theSoundToPlay = kTurretFire2;break;
	case 2:theSoundToPlay = kTurretFire3;break;
	case 3:theSoundToPlay = kTurretFire4;break;
	}
	
	int thePitch = RANDOM_LONG(50, 150);
	EMIT_SOUND_DYN(ENT(this->pev), CHAN_WEAPON, theSoundToPlay, 1.0, ATTN_NORM, 0, thePitch);
	
	pev->effects = pev->effects | EF_MUZZLEFLASH;

	int theRandomSmoke = RANDOM_LONG(0, 3);
	if(theRandomSmoke == 0)
	{
		AvHSUPlayParticleEvent(kpsSmokePuffs, this->edict(), inOrigin);
	}

	GetGameRules()->TriggerAlert((AvHTeamNumber)this->pev->team, ALERT_SENTRY_FIRING, this->entindex());
}

void AvHMarineTurret::Spawn()
{
	AvHTurret::Spawn();

    // Sentries and sieges need TFs nearby to become active
    this->CheckEnabledState();
}

int AvHMarineTurret::TakeDamage(entvars_t* inInflictor, entvars_t* inAttacker, float inDamage, int inBitsDamageType)
{
	GetGameRules()->TriggerAlert((AvHTeamNumber)this->pev->team, ALERT_SENTRY_DAMAGED, this->entindex());

	return AvHTurret::TakeDamage(inInflictor, inAttacker, inDamage, inBitsDamageType);
}

char* AvHMarineTurret::GetModelName() const
{
	return kDeployedTurretModel;
}

