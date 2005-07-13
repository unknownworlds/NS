//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: The marine siege cannon
//
// $Workfile: AvHSiegeTurret.cpp$
// $Date: 2002/11/22 21:26:06 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSiegeTurret.cpp,v $
// Revision 1.12  2002/11/22 21:26:06  Flayra
// - Fixed turret factory abuse, where turrets became active after recycling the nearby turret factory before turret was fully contructed.
// - Fixed bug where siege turrets became re-activated after building a regular turret factory nearby.
// - mp_consistency changes
//
// Revision 1.11  2002/11/12 02:29:11  Flayra
// - Removed check for mp_testing with siege
//
// Revision 1.10  2002/11/03 04:52:18  Flayra
// - Removed server variables, hard-coded them
//
// Revision 1.9  2002/10/16 01:07:36  Flayra
// - Removed unused sounds
//
// Revision 1.8  2002/09/23 22:32:14  Flayra
// - Removed minimum range for siege
//
// Revision 1.7  2002/08/16 02:48:09  Flayra
// - New damage model
//
// Revision 1.6  2002/07/26 23:08:14  Flayra
// - Siege don't fire at babblers
//
// Revision 1.5  2002/07/24 18:55:52  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.4  2002/07/23 17:27:47  Flayra
// - Siege no longer requires LOS (so it's actual siege)
//
// Revision 1.3  2002/07/01 21:47:27  Flayra
// - Added siege shockwave effect, added view shaking effects
//
// Revision 1.2  2002/05/28 18:07:19  Flayra
// - Reduced tracking rate for siege
//
// Revision 1.1  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHSiegeTurret.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHPlayerUpgrade.h"
#include "util/MathUtil.h"

LINK_ENTITY_TO_CLASS(kwSiegeTurret, AvHSiegeTurret);

extern int		gSiegeHitEventID;
extern int		gSiegeViewHitEventID;

AvHSiegeTurret::AvHSiegeTurret() : AvHMarineTurret(TECH_NULL, BUILD_SIEGE, kwsSiegeTurret, AVH_USER3_SIEGETURRET)
{
	float theStartTime = RANDOM_FLOAT(0, BALANCE_VAR(kSiegeROF));
	this->mTimeLastFired = gpGlobals->time - theStartTime;
}

void AvHSiegeTurret::CheckEnabledState()
{
	bool theEnabledState = false;

	if(this->GetHasBeenBuilt() && !this->GetIsRecycling())
	{
		FOR_ALL_ENTITIES(kwsAdvancedTurretFactory, AvHTurretFactory*)
		if((theEntity->pev->team == this->pev->team) && theEntity->GetIsBuilt() && !GetHasUpgrade(theEntity->pev->iuser4, MASK_RECYCLING))
		{
			// If they are a friendly, alive, turret factory
			float the2DDistance = VectorDistance2D(this->pev->origin, theEntity->pev->origin);
			
			// Enabled state is true
			if(the2DDistance <= BALANCE_VAR(kTurretFactoryBuildDistance))
			{
				theEnabledState = true;
				break;
			}
		}
		END_FOR_ALL_ENTITIES(kwsAdvancedTurretFactory)
	}

	// Set enabled state
	this->SetEnabledState(theEnabledState);
}

bool AvHSiegeTurret::GetIsValidTarget(CBaseEntity* inEntity) const
{
	bool theTargetIsValid = false;
	
	if(AvHMarineTurret::GetIsValidTarget(inEntity))
	{
		if(!inEntity->IsPlayer() && !FStrEq(STRING(inEntity->pev->classname), kwsBabblerProjectile))
		{
			float theDistanceToCurrentEnemy = AvHSUEyeToBodyDistance(this->pev, inEntity);
			//if(theDistanceToCurrentEnemy >= this->GetMinimumRange())
			//{
				// We have to see it as well
				//Vector vecMid = this->pev->origin + this->pev->view_ofs;
				//Vector vecMidEnemy = inEntity->BodyTarget(vecMid);
				//if(FBoxVisible(this->pev, inEntity->pev, vecMidEnemy))
				//{

				// Entities must be sighted to be hit (in view of player or scanned)
				AvHSiegeTurret* thisTurret = const_cast<AvHSiegeTurret*>(this);
				if(GetHasUpgrade(inEntity->pev->iuser4, MASK_VIS_SIGHTED) || inEntity->FVisible(thisTurret))
				{
					theTargetIsValid = true;
				}
				//}
			//}
		}
	}
	return theTargetIsValid;
}

int AvHSiegeTurret::GetDamageType() const
{
	return NS_DMG_STRUCTURAL;
}

char* AvHSiegeTurret::GetDeploySound() const
{
	return kSiegeDeploy;
}

char* AvHSiegeTurret::GetPingSound() const
{
	return kSiegePing;
}

int AvHSiegeTurret::GetPointValue(void) const
{
	return BALANCE_VAR(kScoringSiegeValue);
}

//int	AvHSiegeTurret::GetMinimumRange() const
//{
//	return this->GetXYRange()*kSiegeTurretMinRangeScalar;
//}

char* AvHSiegeTurret::GetModelName() const
{
	return kSiegeTurretModel;
}

int	AvHSiegeTurret::GetXYRange() const
{
	return BALANCE_VAR(kSiegeTurretRange);
}

bool AvHSiegeTurret::GetRequiresLOS() const
{
	return false;
}

void AvHSiegeTurret::Precache(void)
{
	AvHMarineTurret::Precache();
	
	PRECACHE_UNMODIFIED_MODEL(kSiegeTurretModel);
	PRECACHE_UNMODIFIED_SOUND(kSiegeTurretFire1);
	PRECACHE_UNMODIFIED_SOUND(kSiegeDeploy);
	PRECACHE_UNMODIFIED_SOUND(kSiegePing);
	PRECACHE_UNMODIFIED_SOUND(kSiegeHitSound1);
	PRECACHE_UNMODIFIED_SOUND(kSiegeHitSound2);
	this->mShockwaveTexture = PRECACHE_UNMODIFIED_MODEL(kSiegeTurretShockWave);
}


void AvHSiegeTurret::Shoot(const Vector &inOrigin, const Vector &inToEnemy, const Vector& inVecEnemyVelocity)
{
	// Only fire once every few seconds...this is hacky but there's no way to override think functions so it must be done
	// I wish it was easier to change the update rate but it's not so...
	if((gpGlobals->time - this->mTimeLastFired) > BALANCE_VAR(kSiegeROF))
	{
		// Find enemy player in range, ignore walls and everything else
		if(this->m_hEnemy != NULL)
		{
			edict_t* theEnemyEdict = this->m_hEnemy->edict();
			entvars_t* theEnemyEntVars = this->m_hEnemy->pev;
			CBaseEntity* theEnemyEntity = (CBaseEntity*)(this->m_hEnemy);

			if(this->GetIsValidTarget(this->m_hEnemy) && theEnemyEdict && theEnemyEntVars && theEnemyEntity)
			{
				// Apply damage, taking upgrade into account
				float theDamageMultiplier;
				AvHPlayerUpgrade::GetWeaponUpgrade(this->pev->iuser3, this->pev->iuser4, &theDamageMultiplier);
				float theDamage = theDamageMultiplier*BALANCE_VAR(kSiegeDamage);

				// Play view shake, because a big gun is going off
				float theShakeAmplitude = 20;
				float theShakeFrequency = 80;
				float theShakeDuration = .3f;
				float theShakeRadius = 240;
				UTIL_ScreenShake(this->pev->origin, theShakeAmplitude, theShakeFrequency, theShakeDuration, theShakeRadius);

				float theSiegeSplashRadius = kSiegeSplashRadius;
				
				// Play fire sound
				EMIT_SOUND_DYN(ENT(this->pev), CHAN_AUTO, kSiegeTurretFire1, 1.0, ATTN_NORM, 0, PITCH_NORM);
				
				this->pev->effects |= EF_MUZZLEFLASH;
				
				// Send normal effect to all
				PLAYBACK_EVENT_FULL(0, theEnemyEdict, gSiegeHitEventID, 0, theEnemyEntVars->origin, theEnemyEntVars->angles, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );

				// Play view shake where it hits as well
				theShakeAmplitude = 60;
				theShakeFrequency = 120;
				theShakeDuration = 1.0f;
				theShakeRadius = 650;
				UTIL_ScreenShake(theEnemyEntVars->origin, theShakeAmplitude, theShakeFrequency, theShakeDuration, theShakeRadius);
				
				if(theEnemyEntity->IsPlayer())
				{
					// Send personal view shake to recipient only (check for splash here, pass param to lessen effect for others?)
					// TODO: Use upgrade level to parameterize screen shake and fade?
					PLAYBACK_EVENT_FULL(FEV_HOSTONLY, theEnemyEdict, gSiegeViewHitEventID, 0, theEnemyEntVars->origin, theEnemyEntVars->angles, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
					
					Vector theFadeColor;
					theFadeColor.x = 255;
					theFadeColor.y = 100;
					theFadeColor.z = 100;
					UTIL_ScreenFade(this->m_hEnemy, theFadeColor, .3f, 0.0f, 255, FFADE_OUT);
				}

				// blast circles
				MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, theEnemyEntVars->origin );
					WRITE_BYTE( TE_BEAMCYLINDER );
					WRITE_COORD( theEnemyEntVars->origin.x);
					WRITE_COORD( theEnemyEntVars->origin.y);
					WRITE_COORD( theEnemyEntVars->origin.z + 16);
					WRITE_COORD( theEnemyEntVars->origin.x);
					WRITE_COORD( theEnemyEntVars->origin.y);
					WRITE_COORD( theEnemyEntVars->origin.z + 16 + theSiegeSplashRadius / .2); // reach damage radius over .3 seconds
					WRITE_SHORT( this->mShockwaveTexture );
					WRITE_BYTE( 0 ); // startframe
					WRITE_BYTE( 0 ); // framerate
					WRITE_BYTE( 2 ); // life
					WRITE_BYTE( 16 );  // width
					WRITE_BYTE( 0 );   // noise
				
					// Write color
					WRITE_BYTE(188);
					WRITE_BYTE(220);
					WRITE_BYTE(255);
				
					WRITE_BYTE( 255 );		//brightness
					WRITE_BYTE( 0 );		// speed
				MESSAGE_END();

				// Finally, do damage (do damage after sending effects because m_hEnemy seems to be going to NULL)
				::RadiusDamage(theEnemyEntVars->origin, this->pev, this->GetAttacker()->pev, theDamage, theSiegeSplashRadius, CLASS_NONE, this->GetDamageType());
			}
			else
			{
				this->m_hEnemy = NULL;
			}
		}

		this->mTimeLastFired = gpGlobals->time;
	}
}

void AvHSiegeTurret::ResetEntity()
{
	AvHMarineTurret::ResetEntity();

	this->mTimeLastFired = -1;
}

void AvHSiegeTurret::Spawn()
{
	AvHMarineTurret::Spawn();

	this->m_fTurnRate = M_PI/3.0f;
}
