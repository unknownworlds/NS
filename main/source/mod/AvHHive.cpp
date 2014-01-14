//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHHive.cpp $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHHive.cpp,v $
// Revision 1.20  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.19  2002/11/06 01:38:37  Flayra
// - Added ability for buildings to be enabled and disabled, for turrets to be shut down
// - Damage refactoring (TakeDamage assumes caller has already adjusted for friendly fire, etc.)
//
// Revision 1.18  2002/11/03 04:50:26  Flayra
// - Hard-coded gameplay constants instead of putting in skill.cfg
//
// Revision 1.17  2002/10/24 21:26:37  Flayra
// - Fixed hive wound animation when dying
// - Hives now choose a random spawn point instead of the first
//
// Revision 1.16  2002/10/16 20:53:21  Flayra
// - Hives have more health while growing
//
// Revision 1.15  2002/10/16 00:57:19  Flayra
// - Fixed hive not going solid sometimes (when player was in the way I think when construction complete)
// - Fixed exploit where hives can be manually sped up (oops!)
//
// Revision 1.14  2002/10/03 18:50:27  Flayra
// - Trigger "hive complete" alert
// - Trigger "hive is dying" alert
//
// Revision 1.13  2002/09/23 22:16:44  Flayra
// - Removed resource donation at hives
//
// Revision 1.12  2002/09/09 19:52:57  Flayra
// - Animations play properly
// - Hive can be hit once it starts gestating (hive becomes solid when gestating, not when complete)
// - Respawn fixes
//
// Revision 1.11  2002/08/16 02:36:01  Flayra
// - New damage system
// - Fixed bug where hive was absorbing too much damage in armor
//
// Revision 1.10  2002/08/02 21:59:36  Flayra
// - New alert system
//
// Revision 1.9  2002/07/26 23:04:19  Flayra
// - Generate numerical feedback for damage events
//
// Revision 1.8  2002/07/23 17:06:09  Flayra
// - Added ability for aliens to donate their resources at the hive, bind technology to a hive (so builders can choose the route), fixed bug where death animation played repeatedly
//
// Revision 1.7  2002/07/08 17:03:04  Flayra
// - Refactored reinforcements
//
// Revision 1.6  2002/07/01 21:33:48  Flayra
// - Hives can no longer be "used" to speed construction, wound sounds play on CHAN_BODY
//
// Revision 1.5  2002/06/25 18:00:14  Flayra
// - Play sequence for non-active hives
//
// Revision 1.4  2002/06/03 16:47:49  Flayra
// - Hives are base buildables now (bug with allowing use to speed building), added other hive anims for hurt, death, bad-touch, fixed bug where hives didn't get full health when they were initially built (after being killed once)
//
// Revision 1.3  2002/05/28 17:46:05  Flayra
// - Mark hives as persistent so they aren't deleted on level cleanup, new hive sight support, reinforcement refactoring and fixes
//
// Revision 1.2  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================

#include "mod/AvHHive.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHAlienAbilityConstants.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHHulls.h"
#include "mod/AvHMovementUtil.h"
#include "mod/AvHSoundListManager.h"
#include "mod/AvHServerVariables.h"
#include "mod/AvHParticleConstants.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHPlayerUpgrade.h"
#include "util/MathUtil.h"
#include <vector>

extern AvHSoundListManager				gSoundListManager;
BOOL IsSpawnPointValid( CBaseEntity *pPlayer, CBaseEntity *pSpot );

LINK_ENTITY_TO_CLASS( keTeamHive, AvHHive );

extern int gRegenerationEventID;
const int kScaredAnimationIndex = 9;

AvHHive::AvHHive() : AvHBaseBuildable(TECH_HIVE, ALIEN_BUILD_HIVE, kesTeamHive, AVH_USER3_HIVE)
{
	// This value should be the default in the .fgd
	this->mMaxSpawnDistance = 2000;
	this->mMaxHitPoints = 0;
	this->mActive = false;
	this->mSolid = false;
	this->mSpawning = false;
	this->mTimeLastWoundSound = -1;
	this->mTechnology = MESSAGE_NULL;	
	this->mEnergy = 0.0f;
	this->mLastTimeScannedHives=-1.0f;
	this->mTimeEmergencyUseEnabled=-1.0f;
	this->mTeleportHiveIndex=-1;

}

bool AvHHive::CanBecomeActive() const
{
	return !this->mActive;// && (!this->mHasBeenKilled || !GetGameRules()->GetIsTournamentMode() || GetGameRules()->GetCheatsEnabled());
}

void AvHHive::ConstructUse(CBaseEntity* inActivator, CBaseEntity* inCaller, USE_TYPE inUseType, float inValue)
{
	int a = 0;
}

void AvHHive::DonateUse(CBaseEntity* inActivator, CBaseEntity* inCaller, USE_TYPE inUseType, float inValue)
{
	// Player is trying to donate his resources to the pool
	if(this->GetIsActive())
	{
		AvHPlayer* inActivatingPlayer = dynamic_cast<AvHPlayer*>(inActivator);
		if(inActivatingPlayer && (inActivator->pev->team == this->pev->team))
		{
			// Take some resources, give some resources
			const float kResourcesToDonatePerUse = .4f;
			float theResourcesToGive = min(inActivatingPlayer->GetResources(), kResourcesToDonatePerUse);
			
			if(theResourcesToGive > 0.0f)
			{
				AvHTeam* theTeam = inActivatingPlayer->GetTeamPointer();
				if(theTeam)
				{
					inActivatingPlayer->SetResources(inActivatingPlayer->GetResources() - theResourcesToGive);
					theTeam->SetTeamResources(theTeam->GetTeamResources() + theResourcesToGive);

					if(g_engfuncs.pfnRandomLong(0, 20) == 0)
					{
						PLAYBACK_EVENT_FULL(0, this->edict(), gRegenerationEventID, 0, this->pev->origin, (float *)&g_vecZero, 1.0f, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );

						// Just say "resources donated"
						inActivatingPlayer->PlaybackNumericalEvent(kNumericalInfoResourcesDonatedEvent, 0);
					}
				}
			}
		}
	}
}

AvHTeamNumber AvHHive::GetTeamNumber() const
{
	return (AvHTeamNumber)this->pev->team;
}

bool AvHHive::GetIsActive() const
{
	return this->mActive;
}

bool AvHHive::GetIsOrganic() const
{
	return true;
}

bool AvHHive::GetIsSpawning() const
{
	return this->mSpawning;
}

int	AvHHive::GetMaxSpawnDistance() const
{
	return this->mMaxSpawnDistance;
}

int	AvHHive::GetMoveType() const
{
	return MOVETYPE_NONE;
}

float AvHHive::GetTimeLastContributed()
{
	return this->mTimeLastContributed;
}

void AvHHive::SetTimeLastContributed(float inTime)
{
	this->mTimeLastContributed = inTime;
}

int	AvHHive::GetIdle1Animation() const
{
	int theAnimation = -1;
	
	if(this->GetIsBuilt())
	{
		theAnimation = 2;
	}
	
	return theAnimation;
}

int	AvHHive::GetIdle2Animation() const
{
	int theAnimation = -1;
	
	if(this->GetIsBuilt())
	{
		theAnimation = 3;
	}
	
	return theAnimation;
}

int AvHHive::GetTakeDamageAnimation() const
{
	int theAnimation = -1;
	
	// Choose animation based on global time, so animation doesn't interrupt itself
	float theTime = gpGlobals->time;
	int theOffset = (int)(ceil(theTime) - theTime + .5f);

	if(this->GetIsActive())
	{
		// Play wound animation.  
		theAnimation = 5 + theOffset;
	}
    else
    {
        // Use still-building flinch anims
        theAnimation = 7 + theOffset;
    }

	return theAnimation;
}

int	AvHHive::GetPointValue(void) const
{
	return BALANCE_VAR(kScoringHiveValue);
}

int	AvHHive::GetSpawnAnimation() const
{
	return 0;
}

AvHMessageID AvHHive::GetTechnology() const
{
	return this->mTechnology;
}

void AvHHive::SetTechnology(AvHMessageID inMessageID)
{
	this->mTechnology = inMessageID;
}

void AvHHive::HiveAliveThink(void)
{
	// For some reason, velocity is non-zero when created (meant they were showing up on motion-tracking)
	this->pev->velocity = Vector(0, 0, 0);

	if(GetGameRules()->GetGameStarted())
	{
        if(!this->mActive)
        {
		    bool theIsBuilding, theIsResearching;
		    float thePercentage;
		    AvHSHUGetBuildResearchState(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, theIsBuilding, theIsResearching, thePercentage);
		    
		    float theBuildTime = GetGameRules()->GetBuildTimeForMessageID(this->GetMessageID());
		    float theBuildPercentage = kHiveAliveThinkInterval/theBuildTime;
		    
		    float theNewPercentage = min(thePercentage + theBuildPercentage, 1.0f);
            this->SetNormalizedBuildPercentage(theNewPercentage);
        }
        else
		{
			this->ProcessHealing();

            // Play idle anims
            AvHBaseBuildable::AnimateThink();
		}

		this->UpdateReinforcements();

		//this->UpdateUmbra();
	}
	
	// Set next think
	this->pev->nextthink = gpGlobals->time + kHiveAliveThinkInterval;
}

void AvHHive::UpdateUmbra()
{
	bool theIsUnderAttack = GetGameRules()->GetIsEntityUnderAttack(this->entindex());
	if(theIsUnderAttack)
	{
		if(this->mTimeOfNextUmbra == -1)
		{
			this->mTimeOfNextUmbra = gpGlobals->time + RANDOM_LONG(5, 15);
		}
	}

	if((this->mTimeOfNextUmbra != -1) && (gpGlobals->time > this->mTimeOfNextUmbra))
	{
		// If we're under attack, sometimes create umbra at hive
		vec3_t theUmbraOrigin = this->pev->origin;
		
		// else create umbra at random spawn
//		if(!theIsUnderAttack)
//		{
//			CBaseEntity* theSpawnPoint = GetGameRules()->GetRandomHiveSpawnPoint(this, this->pev->origin, this->GetMaxSpawnDistance());
//			if(theSpawnPoint)
//			{
//				VectorCopy(theSpawnPoint->pev->origin, theUmbraOrigin);
//			}
//		}

		// Create umbra around it, play "scared" anim
		//this->CreateUmbra(theUmbraOrigin);

		this->PlayAnimationAtIndex(kScaredAnimationIndex, true);
	}
}

void AvHHive::KeyValue(KeyValueData* pkvd)
{
	this->SetPersistent();

	if(FStrEq(pkvd->szKeyName, "maxspawndistance"))
	{
		this->mMaxSpawnDistance = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		AvHBaseBuildable::KeyValue(pkvd);
	}
}

void AvHHive::TriggerDeathAudioVisuals(bool isRecycled)
{
    AvHSUPlayParticleEvent(kpsHiveDeath, this->edict(), this->pev->origin);
    
    AvHSUExplodeEntity(this, matFlesh);
    
    EMIT_SOUND(ENT(this->pev), CHAN_AUTO, kHiveDeathSound, 1.0, ATTN_IDLE);	

    // Play death animation (increment time just to make sure there's no idle anim played after killed and before death)
    const float kDeathAnimationLength = 1.2f;
    
    this->PlayAnimationAtIndex(10, true);
    
    // Then explode
    //SetThink(&AvHHive::DeathThink);
    //this->pev->nextthink = gpGlobals->time + kDeathAnimationLength; 

    this->FireDeathTarget();
}

void AvHHive::Precache(void)
{
	AvHBaseBuildable::Precache();
	
	PRECACHE_UNMODIFIED_SOUND(kHiveSpawnSound);
	PRECACHE_UNMODIFIED_SOUND(kHiveAmbientSound);
	PRECACHE_UNMODIFIED_SOUND(kHiveDeathSound);
	
	PRECACHE_UNMODIFIED_MODEL(kHiveModel);
	
	CBreakable::PrecacheAll();
}

void AvHHive::ProcessHealing()
{
	// Regenerate nearby friendly aliens
	CBaseEntity* theEntity = NULL;
	const int theHiveHealRadius = BALANCE_VAR(kHiveHealRadius);

	while((theEntity = UTIL_FindEntityInSphere(theEntity, this->pev->origin, theHiveHealRadius)) != NULL)
	{
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
		if(thePlayer)
		{
			if(thePlayer->GetIsRelevant() && (thePlayer->GetTeam() == this->GetTeamNumber()) && !thePlayer->GetIsBeingDigested())
			{
                // Hive heals percentage of player health
                float theRegenPercentage = BALANCE_VAR(kHiveRegenerationPercentage);
                int theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(thePlayer->pev->iuser4, (AvHUser3)thePlayer->pev->iuser3, thePlayer->GetExperienceLevel());
                float theRegenAmount = (theRegenPercentage*theMaxHealth);
                thePlayer->Heal(theRegenAmount, true);
			}
		}
	}
	
	// Regenerate self
	bool theDidHeal = false;
	
	// If we aren't at full health, heal health
	if(this->pev->health < this->mMaxHitPoints)
	{
        float theHiveRegenAmount = BALANCE_VAR(kHiveRegenerationAmount);
        float theCombatModeScalar = /*GetGameRules()->GetIsCombatMode() ? (1.0f/BALANCE_VAR(kCombatModeTimeScalar)) :*/ 1.0f;

		this->pev->health = min((float)this->mMaxHitPoints, this->pev->health + theHiveRegenAmount*theCombatModeScalar);
		theDidHeal = true;
	}
	
	// Play regen event
	if(theDidHeal)
	{
		// Play regeneration event
		PLAYBACK_EVENT_FULL(0, this->edict(), gRegenerationEventID, 0, this->pev->origin, (float *)&g_vecZero, 1.0f, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
	}
}

void AvHHive::ResetEntity(void)
{
	AvHReinforceable::ResetEntity();
	AvHBaseBuildable::ResetEntity();

	this->ResetCloaking();

	this->SetInactive();

	this->pev->health = this->mMaxHitPoints;
	
	this->pev->takedamage = DAMAGE_NO;

	// Reset parasites, etc.
	this->pev->iuser4 = 0;
	this->SetPersistent();

	this->mTimeOfNextUmbra = -1;

	// Reset fuser1 progress
	this->pev->fuser1 = 0;
}

void AvHHive::ResetReinforcingPlayer(bool inSuccess)
{
	AvHReinforceable::ResetReinforcingPlayer(inSuccess);

	this->mEnergy = 0.0f;
}

bool AvHHive::SetSolid(bool inForce)
{
	// Check to make sure there aren't any players in the destination area
	CBaseEntity* pList[128];
	
	// Crank up the area just to be safe
	Vector theMinArea = this->pev->origin + kHiveMinSize;
	Vector theMaxArea = this->pev->origin + kHiveMaxSize;
	
	// TODO: If players are blocking this area for too long, spawn hive and kill them
	int theNumBlockingEntities = UTIL_EntitiesInBox(pList, 128, theMinArea, theMaxArea, FL_CLIENT);
	if((theNumBlockingEntities == 0) || inForce)
	{
		this->pev->solid = SOLID_BBOX;
		this->pev->movetype = MOVETYPE_NONE;
			
		UTIL_SetSize(this->pev, kHiveMinSize, kHiveMaxSize);
			
		//		pev->frame = 0;
		//		pev->body = 3;
		//		pev->sequence = 0;
		//		// ResetSequenceInfo( );
		//		pev->framerate = 0;
		//		
		//		UTIL_SetOrigin(pev, pev->origin);
		//		UTIL_SetSize(pev, Vector(-20, -20, 0), Vector(20, 20, 28) ); 
			
		SetTouch(&AvHHive::HiveTouch);

		this->mSolid = true;
	}

	return this->mSolid;
}

void AvHHive::SetHasBeenBuilt()
{
	AvHBuildable::SetHasBeenBuilt();

	GetGameRules()->TriggerAlert((AvHTeamNumber)this->pev->team, ALERT_HIVE_COMPLETE, this->entindex());

	// Make hive support any unassigned upgrade technologies (happens after a hive supporting a technology is destroyed and then rebuilt)
	AvHTeamNumber theTeam = (AvHTeamNumber)this->pev->team;
	AvHTeam* theTeamPointer = GetGameRules()->GetTeam(theTeam);
	if(theTeamPointer)
	{
		AvHAlienUpgradeListType theUpgrades = theTeamPointer->GetAlienUpgrades();

		if(AvHGetNumUpgradesInCategoryInList(theUpgrades, ALIEN_UPGRADE_CATEGORY_DEFENSE) > 0)
		{
			AvHSUUpdateHiveTechology(theTeam, ALIEN_BUILD_DEFENSE_CHAMBER);
		}

		if(AvHGetNumUpgradesInCategoryInList(theUpgrades, ALIEN_UPGRADE_CATEGORY_MOVEMENT) > 0)
		{
			AvHSUUpdateHiveTechology(theTeam, ALIEN_BUILD_MOVEMENT_CHAMBER);
		}

		if(AvHGetNumUpgradesInCategoryInList(theUpgrades, ALIEN_UPGRADE_CATEGORY_SENSORY) > 0)
		{
			AvHSUUpdateHiveTechology(theTeam, ALIEN_BUILD_SENSORY_CHAMBER);
		}
	}
}

bool AvHHive::StartSpawningForTeam(AvHTeamNumber inTeam, bool inForce)
{
	bool theSuccess = false;

	if(this->SetSolid(inForce))
	{
		this->pev->team = inTeam;
		this->pev->takedamage = DAMAGE_YES;

		this->pev->rendermode = kRenderNormal;
		this->pev->renderamt = 0;
		
		SetBits(pev->flags, FL_MONSTER);
		SetUpgradeMask(&this->pev->iuser4, MASK_BUILDABLE);
		
		this->mSpawning = true;
		this->pev->health = kBaseHealthPercentage*this->mBaseHealth;

		// Looping, growing animation
		this->pev->sequence	= 0;
		this->pev->frame = 0;
		ResetSequenceInfo();

		this->pev->nextthink = gpGlobals->time + kHiveAliveThinkInterval;
		SetThink(&AvHHive::HiveAliveThink);
		SetUse(&AvHHive::TeleportUse);

		theSuccess = true;
	}

	return theSuccess;
}


void AvHHive::Spawn()
{
	this->Precache();
	
	AvHBaseBuildable::Spawn();
    
	this->pev->classname = MAKE_STRING(kesTeamHive);
	//this->pev->movetype	= MOVETYPE_FLY;

	this->pev->movetype	= MOVETYPE_FLY;
	this->pev->solid = SOLID_NOT;
	this->pev->flags = 0;
	this->pev->iuser3 = AVH_USER3_HIVE;

	this->mMaxHitPoints = GetGameRules()->GetBaseHealthForMessageID(ALIEN_BUILD_HIVE);
	
	SET_MODEL( ENT(this->pev), kHiveModel);
	//this->pev->scale = 2;

//	this->pev->sequence = 0;
//	this->pev->frame = 0;
//	ResetSequenceInfo();

	this->ResetEntity();

}

void AvHHive::SetActive()
{
    AvHBaseBuildable::SetActive();

    if(!this->mActive)
    {
        this->mActive = true;
        this->mSpawning = false;
        
        this->mTimeLastContributed = gpGlobals->time;
        
        // Start animating
        this->pev->sequence	= 1;
        this->pev->frame = 0;
        ResetSequenceInfo();
        //AvHSUSetCollisionBoxFromSequence(this->pev);
        
        // Play spawn sound here
        EMIT_SOUND(ENT(this->pev), CHAN_AUTO, kHiveSpawnSound, 1.0, ATTN_IDLE);	
        
        // Note: this isn't being created for the first hive because this sound plays before the map is totally up
        UTIL_EmitAmbientSound(ENT(this->pev), this->pev->origin, kHiveAmbientSound, 1.0f, 2.0, 0, 100);
        
        this->FireSpawnTarget();
        
        //SetUse(&AvHHive::DonateUse);
    }
}

void AvHHive::SetInactive()
{
    AvHBaseBuildable::SetInactive();

    // Set this so hives can be drawn translucently at hive locations for aliens
    this->pev->effects &= ~EF_NODRAW;

    this->ResetReinforcingPlayer(false);

	this->mActive = false;
	this->mSpawning = false;
	this->mSolid = false;
	this->mTimeLastContributed = -1;
	this->mTimeEmergencyUseEnabled=-1.0f;
	this->mTechnology = MESSAGE_NULL;

	this->pev->health = 0;
	this->pev->takedamage = DAMAGE_NO;
	this->pev->dmgtime = gpGlobals->time;
	this->pev->solid = SOLID_NOT;
	this->pev->team = TEAM_IND; 
	SetUpgradeMask(&this->pev->iuser4, MASK_BUILDABLE, false);
	
	this->pev->rendermode = kRenderTransAlpha;
	this->pev->renderamt = 0;
	
	// Stop animation
	this->pev->sequence	= 0;
	this->pev->frame = 0;
	this->pev->framerate = 0;

	// No longer built at all
	this->pev->fuser1 = 0.0f;
	SetThink(NULL);
	SetUse(NULL);
	// Stop looping
	UTIL_EmitAmbientSound(ENT(this->pev), this->pev->origin, kHiveAmbientSound, 1.0f, .5, SND_STOP, 100);

	ClearBits (pev->flags, FL_MONSTER); // why are they set in the first place???

	SetTouch(NULL);
}

int	AvHHive::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	const float kWoundSoundInterval = 1.0f;

	int theReturnCode = 0;
	
	if(this->mActive || this->mSpawning)
	{
		//CBaseEntity* theAttackingEntity = CBaseEntity::Instance(pevAttacker);
		//if(GetGameRules()->CanEntityDoDamageTo(theAttackingEntity, this))
		//{
			theReturnCode = AvHBaseBuildable::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);

			if(theReturnCode > 0)
			{	
				const float kDyingThreshold = .4f;
				if(this->pev->health < kDyingThreshold*this->mMaxHitPoints)
				{
					GetGameRules()->TriggerAlert((AvHTeamNumber)this->pev->team, ALERT_HIVE_DYING, this->entindex());
				}
				else
				{
					if ( pevAttacker->team != this->pev->team ) 
						GetGameRules()->TriggerAlert((AvHTeamNumber)this->pev->team, ALERT_UNDER_ATTACK, this->entindex());
				}

				if((this->mTimeLastWoundSound == -1) || ((this->mTimeLastWoundSound + kWoundSoundInterval) < gpGlobals->time))
				{
					// Pick a random wound sound to play
					//int theIndex = RANDOM_LONG(0, kNumWoundSounds - 1);
					//char* theSoundToPlay = kWoundSoundList[theIndex];
					//EMIT_SOUND(ENT(this->pev), CHAN_AUTO, kHiveDeathSound, 1.0, ATTN_IDLE);	
					
					//EMIT_SOUND(ENT(this->pev), CHAN_AUTO, "misc/hive_wound1.wav", 1.0, ATTN_IDLE);	
					
					// Emit hive damaged sound
					gSoundListManager.PlaySoundInList(kHiveWoundSoundList, this, CHAN_BODY);
					
					this->mTimeLastWoundSound = gpGlobals->time;
				}
			}
		//}
	}
	
	return theReturnCode;
}



bool AvHHive::GetCanReinforce() const
{
	return (this->GetIsBuilt() && this->IsAlive() && !GetGameRules()->GetIsCombatMode());
}

bool AvHHive::GetSpawnLocationForPlayer(CBaseEntity* inPlayer, Vector& outLocation) const
{
	bool theSuccess = false;

	CBaseEntity* theSpawnPoint = GetGameRules()->GetRandomHiveSpawnPoint(inPlayer, this->pev->origin, this->GetMaxSpawnDistance());
	if(theSpawnPoint)
	{
		outLocation = theSpawnPoint->pev->origin;
		theSuccess = true;
	}

	return theSuccess;
}

bool AvHHive::GetTriggerAlertOnDamage() const
{
	return false;
}

AvHTeamNumber AvHHive::GetReinforceTeamNumber() const
{
	return AvHBaseBuildable::GetTeamNumber();
}


//void AvHHive::UpdateReinforcements()
//{
//	// If hive is active 
//	if(this->GetIsActive())
//	{
//		// Test to make sure our reinforcing player is still valid
//		AvHPlayer* theReinforcingPlayer = this->GetReinforcingPlayer();
//		if(theReinforcingPlayer)
//		{
//			AvHPlayMode thePlayMode = theReinforcingPlayer->GetPlayMode();
//			if((theReinforcingPlayer->pev->team != this->pev->team) || (thePlayMode == PLAYMODE_UNDEFINED) || (thePlayMode == PLAYMODE_READYROOM) || (thePlayMode == PLAYMODE_OBSERVER))
//			{
//				this->mReinforcingPlayer = -1;
//			}
//		}
//
//		// If hive isn't spawning a player in currently
//		if(this->mReinforcingPlayer == -1)
//		{
//			// Find player on this team that's been waiting the longest
//			AvHPlayer* thePlayerToSpawn = NULL;
//			
//			FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*);
//			if(theEntity->GetTeam() == this->GetTeamNumber())
//			{
//				if(theEntity->GetPlayMode() == PLAYMODE_AWAITINGREINFORCEMENT)
//				{
//					if(!thePlayerToSpawn || (theEntity->GetTimeLastPlayModeSet() < thePlayerToSpawn->GetTimeLastPlayModeSet()))
//					{
//						thePlayerToSpawn = theEntity;
//						break;
//					}
//				}
//			}
//			END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
//			
//			// Spawn the one waiting the longest
//			if(thePlayerToSpawn)
//			{
//				// Set the player to be reinforcing
//				this->mReinforcingPlayer = thePlayerToSpawn->entindex();
//				
//				thePlayerToSpawn->SetPlayMode(PLAYMODE_REINFORCING);
//				
//				// Play hive animation, play effect for player?
//				this->pev->sequence = 4;
//				this->pev->frame = 0;
//				ResetSequenceInfo();
//			}
//		}
//		// else hive is spawning a player
//		else
//		{
//			// Is player still valid, or has he left the server/team?
//			AvHPlayer* thePlayer = this->GetReinforcingPlayer();
//			if(thePlayer && (thePlayer->GetTeam() == this->GetTeamNumber()))
//			{
//				if(thePlayer->GetPlayMode() == PLAYMODE_REINFORCING)
//				{
//					// Has enough time passed to bring the player in?
//					const float kHiveRespawnTime = GetGameRules()->GetAlienRespawnTime();
//					
//					if(gpGlobals->time > (thePlayer->GetTimeLastPlayModeSet() + kHiveRespawnTime))
//					{
//						this->ResetReinforcingPlayer(true);
//					
//						// Take away points from the player if possible
//						float theNewPointTotal = max(thePlayer->GetResources() - GetGameRules()->GetGameplay().GetAlienRespawnCost(), 0.0f);
//						thePlayer->SetResources(theNewPointTotal);
//					}
//				}
//				else
//				{
//					this->mReinforcingPlayer = -1;
//				}
//			}
//		}
//	}
//}

void AvHHive::CreateUmbra(vec3_t& inOrigin)
{
	AvHSUCreateUmbraCloud(inOrigin, AvHTeamNumber(this->pev->team), this);

	// Don't create another for a bit
	this->mTimeOfNextUmbra = -1;
}

void AvHHive::CueRespawnEffect(AvHPlayer* inPlayer)
{
	// Play hive animation, play effect for player?
	this->pev->sequence = 4;
	this->pev->frame = 0;
	ResetSequenceInfo();

	// Create umbra around spawning players, but not until after late-join period (to avoid a ton of umbras all at once)
	if(!GetGameRules()->GetArePlayersAllowedToJoinImmediately())
	{
		//this->CreateUmbra(inPlayer->pev->origin);
	}
}

float AvHHive::GetReinforceTime() const
{
	const float kMaxRespawnTime = BALANCE_VAR(kAlienRespawnTime);

	float theRespawnTime = (kMaxRespawnTime - kMaxRespawnTime*this->mEnergy);

	//  0000854 
	//  Decrease respawn wait time for aliens (NS: Classic)
	//	With one hive, for every player above six on the alien team, 
	//  reduce the per-player respawn wait time by two-thirds of a second. 
	//  With two hives, make the reduction one-third of a second. 
	//  With three (or more, in the case of weird custom maps) hives, do not apply it.

	AvHTeam* theTeam = GetGameRules()->GetTeam(GetTeamNumber());
	ASSERT(theTeam);

	int thePlayerModifier = theTeam->GetPlayerCount() - BALANCE_VAR(kAlienRespawnPlayerModifier);
	int theHiveCount = GetGameRules()->GetNumActiveHives(GetTeamNumber());
	
	if ( thePlayerModifier > 0 && theHiveCount < 3 ) 
	{
		float theTimeModifier = BALANCE_VAR(kAlienRespawnTimeModifier);

		// For one hive double the modifier
		if ( theHiveCount == 1 ) 
		{
			theTimeModifier *= 2.0f;
		}

		theRespawnTime -= theTimeModifier * (float)thePlayerModifier;
	}

	theRespawnTime = min(max(theRespawnTime, 0.0f), kMaxRespawnTime);

	return theRespawnTime;
}

bool AvHHive::Energize(float inEnergyAmount)
{
	bool theSuccess = false;

	// Only energize when a player is in the cue
	if(this->GetIsBuilt() && this->GetReinforcingPlayer())
	{
		if(this->mEnergy < 1.0f)
		{
			this->mEnergy += inEnergyAmount;
			this->mEnergy = min(max(0.0f, this->mEnergy), 1.0f);
			theSuccess = true;
		}
	}

	return theSuccess;
}

void AvHHive::HiveTouch(CBaseEntity* inOther)
{
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inOther);
	if(thePlayer && (thePlayer->pev->team != this->pev->team))
	{
		if(this->GetIsActive())
		{
			// Play scared animation, it recoils from human touch
			this->PlayAnimationAtIndex(kScaredAnimationIndex, true);
		}
	}

    AvHBaseBuildable::BuildableTouch(inOther);
}

void AvHHive::SetEmergencyUse() {
	if ( !this->GetEmergencyUse() ) {
		GetGameRules()->TriggerAlert((AvHTeamNumber)this->pev->team, ALERT_HIVE_DEFEND, this->entindex());
		this->mTimeEmergencyUseEnabled=gpGlobals->time;
	}
}

bool AvHHive::GetEmergencyUse() const {
	return ( this->mTimeEmergencyUseEnabled > gpGlobals->time - 5.0f ); //BALANCE_VAR(kHiveEmergencyInterval)
}

void AvHHive::TeleportUse(CBaseEntity* inActivator, CBaseEntity* inCaller, USE_TYPE inUseType, float inValue)
{
	if ( this->GetIsSpawning() ) {
		this->SetEmergencyUse();
		return;
	}

	const float kHiveScanInterval = 1.0f;

	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inActivator);


	if(thePlayer && (thePlayer->pev->team == this->pev->team) && (thePlayer->GetUser3() != AVH_USER3_ALIEN_EMBRYO) && thePlayer->GetCanUseHive() )
	{
		vector<int> theHives;
		vector<int> theHivesUnderAttack;
		if((this->mLastTimeScannedHives == -1) || (gpGlobals->time > (this->mLastTimeScannedHives + kHiveScanInterval)))
		{
			this->mTeleportHiveIndex = -1;
			float theFarthestDistance = 0.0f; //sqrt((kMaxMapDimension*2)*(kMaxMapDimension*2));
			// Loop through the hives for this team, look for the farthest one (hives under attack take precedence)
			FOR_ALL_ENTITIES(kesTeamHive, AvHHive*)
				if((theEntity->pev->team == this->pev->team) && theEntity != this )
				{
					bool theHiveIsUnderAttack = GetGameRules()->GetIsEntityUnderAttack(theEntity->entindex());
					// allow teleport to any built hive, or unbuilt hives under attack.
					if(!theEntity->GetIsSpawning() || ( theEntity->GetIsSpawning() && ( theHiveIsUnderAttack || theEntity->GetEmergencyUse()) ) )
					{
						theHives.push_back(theEntity->entindex());
						if ( theHiveIsUnderAttack ) 
							theHivesUnderAttack.push_back(theEntity->entindex());
					}
				}
			END_FOR_ALL_ENTITIES(kesTeamHive)
		
			this->mLastTimeScannedHives = gpGlobals->time;
		}

		vector<int> *tmpPtr=&theHives;
		if ( theHivesUnderAttack.size() > 0 )
			tmpPtr=&theHivesUnderAttack;

		if ( tmpPtr->size() > 0 ) {
			int myIndex=this->entindex();
			for ( int i=0; i < tmpPtr->size(); i++ ) {
				int hiveIndex=(*tmpPtr)[i];
				if ( hiveIndex > myIndex ) {
					this->mTeleportHiveIndex=hiveIndex;
					break;
				}
			}
			if ( this->mTeleportHiveIndex == -1 ) {
				this->mTeleportHiveIndex=(*tmpPtr)[0];
			}
		}

		// If we have a valid hive index, jump the player to it
		if(this->mTeleportHiveIndex != -1)
		{
			// Play sound at this entity
			EMIT_SOUND(this->edict(), CHAN_AUTO, kAlienSightOnSound, 1.0f, ATTN_NORM);
		
			// Move him to it!
			AvHHive* theHive = NULL;
			AvHSUGetEntityFromIndex(this->mTeleportHiveIndex, theHive);
			if(theHive)
			{
				CBaseEntity* theSpawnEntity = GetGameRules()->GetRandomHiveSpawnPoint(thePlayer, theHive->pev->origin, theHive->GetMaxSpawnDistance());
				if(theSpawnEntity)
				{
					Vector theMinSize;
					Vector theMaxSize;
					thePlayer->GetSize(theMinSize, theMaxSize);

					int theOffset = AvHMUGetOriginOffsetForUser3(AvHUser3(thePlayer->pev->iuser3));
					Vector theOriginToSpawn = theSpawnEntity->pev->origin;
					theOriginToSpawn.z += theOffset;

					if(AvHSUGetIsEnoughRoomForHull(theOriginToSpawn, AvHMUGetHull(false, thePlayer->pev->iuser3), thePlayer->edict()))
					{
						thePlayer->SetTimeOfLastHiveUse(gpGlobals->time);
						thePlayer->SetPosition(theOriginToSpawn);
						thePlayer->pev->velocity = Vector(0, 0, 0);
						thePlayer->TriggerUncloak();
						// Play teleport sound before and after
						EMIT_SOUND(inActivator->edict(), CHAN_AUTO, kAlienSightOffSound, 1.0f, ATTN_NORM);
					}
				}
			}
		}
	}
}
