//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHBaseBuildable.cpp$
// $Date: 2002/11/22 21:28:15 $
//
//-------------------------------------------------------------------------------
// $Log: AvHBaseBuildable.cpp,v $
// Revision 1.19  2002/11/22 21:28:15  Flayra
// - mp_consistency changes
//
// Revision 1.18  2002/11/15 04:47:11  Flayra
// - Regenerate now returns bool if healed or not, for def chamber tweak
//
// Revision 1.17  2002/11/12 02:22:35  Flayra
// - Removed draw damage from public build
// - Don't allow +use to speed research
//
// Revision 1.16  2002/11/06 01:38:24  Flayra
// - Added ability for buildings to be enabled and disabled, for turrets to be shut down
// - Damage refactoring (TakeDamage assumes caller has already adjusted for friendly fire, etc.)
//
// Revision 1.15  2002/10/24 21:21:49  Flayra
// - Added code to award attacker a frag when destroying a building but thought better of it
//
// Revision 1.14  2002/10/16 00:49:35  Flayra
// - Reworked build times so they are real numbers
//
// Revision 1.13  2002/10/03 18:38:56  Flayra
// - Fixed problem where regenerating builders via healing spray wasn't updating health ring
// - Allow buildings to play custom damage alerts (for towers)
//
// Revision 1.12  2002/09/23 22:10:14  Flayra
// - Removed progress bar when building
// - Removed vestiges of fading building as building
// - Changed point costs
//
// Revision 1.11  2002/09/09 19:49:09  Flayra
// - Buildables now play animations better, without interrupting previous anims
//
// Revision 1.10  2002/08/31 18:01:00  Flayra
// - Work at VALVe
//
// Revision 1.9  2002/08/16 02:32:45  Flayra
// - Added damage types
// - Added visual health for commander and marines
//
// Revision 1.8  2002/08/02 22:02:09  Flayra
// - New alert system
//
// Revision 1.7  2002/07/26 23:03:56  Flayra
// - Don't play "hurt/wound" sounds when we don't actually take damage (GetCanEntityDoDamageTo)
// - Generate numerical feedback for damage events
//
// Revision 1.6  2002/07/23 16:58:38  Flayra
// - Auto-build can't happen before game starts
//
// Revision 1.5  2002/07/01 21:15:46  Flayra
// - Added auto-build capability
//
// Revision 1.4  2002/06/25 17:31:24  Flayra
// - Regular update, don't assume anything about player building, renamed arsenal to armory
//
// Revision 1.3  2002/06/03 16:27:59  Flayra
// - Allow alien buildings to regenerate, renamed weapons factory and armory
//
// Revision 1.2  2002/05/28 17:37:27  Flayra
// - Added building recycling, mark mapper placed buildables so they aren't destroyed at the end of the round
//
// Revision 1.1  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHBaseBuildable.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHServerVariables.h"
#include "mod/AvHParticleConstants.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHSoundListManager.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include "dlls/animation.h"
#include "mod/AvHMovementUtil.h"

const int kBaseBuildableSpawnAnimation = 0;
const int kBaseBuildableDeployAnimation = 1;
const int kBaseBuildableIdle1Animation = 2;
const int kBaseBuildableIdle2Animation = 3;
const int kBaseBuildableResearchingAnimation = 4;
const int kBaseBuildableActiveAnimation = 5;
const int kBaseBuildableFireAnimation = 6;
const int kBaseBuildableTakeDamageAnimation = 7;
const int kBaseBuildableDieForwardAnimation = 8;
const int kBaseBuildableDieLeftAnimation = 9;
const int kBaseBuildableDieBackwardAnimation = 10;
const int kBaseBuildableDieRightAnimation = 11;
const int kBaseBuildableSpecialAnimation = 12;

extern int gRegenerationEventID;
extern AvHSoundListManager		gSoundListManager;

AvHBaseBuildable::AvHBaseBuildable(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inUser3) : AvHBuildable(inTechID), kStartAlpha(128), mAverageUseSoundLength(.5f)
{
	this->mClassName = inClassName;
	this->mMessageID = inMessageID;

	this->mBaseHealth = GetGameRules()->GetBaseHealthForMessageID(inMessageID);

	char* theModelName = AvHSHUGetBuildTechModelName(inMessageID);
	ASSERT(theModelName);
	this->mModelName = theModelName;

	this->mSelectID = inUser3;
	this->mTimeToConstruct = GetGameRules()->GetBuildTimeForMessageID(inMessageID);

	// Very important that this doesn't go in Init(), else mapper-placed structures disappear on map-reset
	this->mPersistent = false;

	this->Init();
}

void AvHBaseBuildable::Init()
{
	if(this->pev)
	{
		InitializeBuildable(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, this->mSelectID);
	}
	
	this->mTimeAnimationDone = 0;
	this->mLastAnimationPlayed = -1;
	this->mIsResearching = false;
	this->mTimeOfLastAutoHeal = -1;
	this->mInternalSetConstructionComplete = false;
	this->mKilled = false;
	this->mTimeOfLastDamageEffect = -1;
	this->mTimeOfLastDamageUpdate = -1;
	this->mTimeRecycleStarted = -1;
	this->mTimeRecycleDone = -1;

	SetThink(NULL);
}

const float kAnimateThinkTime = .1f;

void AvHBaseBuildable::AnimateThink()
{
	int theSequence = this->GetResearchAnimation();
	if(!this->mIsResearching)
	{
		// Play a random idle animation
		theSequence = this->GetIdleAnimation();
	}
	else
	{
		int a = 0;
	}

	this->PlayAnimationAtIndex(theSequence);

	// Set our next think
	float theUpdateTime = this->GetTimeForAnimation(theSequence);
	this->pev->nextthink = gpGlobals->time + theUpdateTime;
}

int	AvHBaseBuildable::BloodColor( void ) 
{ 
	int theBloodColor = DONT_BLEED;

	if(this->GetIsOrganic())
	{
		theBloodColor = BLOOD_COLOR_GREEN;
	}

	return theBloodColor; 
}

void AvHBaseBuildable::BuildableTouch(CBaseEntity* inEntity)
{
    if(inEntity->pev->team != this->pev->team)
    {
        this->Uncloak();
    }
}

void AvHBaseBuildable::CheckEnabledState()
{
}

void AvHBaseBuildable::ConstructUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	bool theSuccess = false;
	bool theIsBuilding = false;
	bool theIsResearching = false;
	float thePercentage = 0.0f;

	AvHSHUGetBuildResearchState(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, theIsBuilding, theIsResearching, thePercentage);
	
	// Only allow players to help along building, not researching
	if(theIsBuilding)
	{
		// Only allow users from same team as turret deployer
		float thePercentage = this->GetNormalizedBuildPercentage();
		if(pActivator->pev->team == this->pev->team && (thePercentage < 1.0f))
		{
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(pActivator);
			ASSERT(thePlayer);
	
			// Only soldiers and builders can build
			if(thePlayer->GetIsAbleToAct() && ((thePlayer->pev->iuser3 == AVH_USER3_MARINE_PLAYER) || (thePlayer->pev->iuser3 == AVH_USER3_ALIEN_PLAYER2)))
			{
                AvHBasePlayerWeapon* theWeapon = dynamic_cast<AvHBasePlayerWeapon*>(thePlayer->m_pActiveItem);
                if(!theWeapon || theWeapon->CanHolster())
                {
				    thePlayer->PlayerConstructUse();
	    
				    bool thePlaySound = false;
				    
				    // Ensure that buildings are never absolutely painful to create
				    int theBuildTime = max(GetGameRules()->GetBuildTimeForMessageID(this->mMessageID), 1);
	    
				    if(GetGameRules()->GetIsTesting() || GetGameRules()->GetCheatsEnabled())
				    {
					    theBuildTime = 2;
				    }
	    
				    // Make non-frame-rate dependent
				    const float kDefaultInterval = .1f;
				    float theTimeOfLastConstructUse = thePlayer->GetTimeOfLastConstructUse();
				    
				    float theInterval = min(max(gpGlobals->time - theTimeOfLastConstructUse, 0.0f), kDefaultInterval);
				    thePercentage += (theInterval/(float)theBuildTime);

				    thePlayer->SetTimeOfLastConstructUse(gpGlobals->time);
	    
				    if(gpGlobals->time > (this->mLastTimePlayedSound + this->mAverageUseSoundLength))
				    {
					    AvHSUPlayRandomConstructionEffect(thePlayer, this);
					    this->mLastTimePlayedSound = gpGlobals->time;
				    }
				    
				    // Given the number of constructors, what's chance of starting a new sound?
				    float theChanceForNewSound = (gpGlobals->frametime/(this->mAverageUseSoundLength));//  /2.0f));
				    float theRandomFloat = RANDOM_FLOAT(0.0f, 1.0f);
				    if(theRandomFloat < theChanceForNewSound)
				    {
					    AvHSUPlayRandomConstructionEffect(thePlayer, this);
				    }
				    
				    //if(RANDOM_LONG(0, 20) == 0)
				    //{
				    //	char theMessage[128];
				    //	sprintf(theMessage, "Time passed: %f, ticks: %d, rate: %f\n", theTimePassed, this->mPreThinkTicks, this->mPreThinkFrameRate);
				    //	UTIL_SayText(theMessage, this);
				    //}
					    
				    this->SetNormalizedBuildPercentage(thePercentage);

				    theSuccess = true;
                }
			}
		}
	}

	// Clear out +use sound when ineffective
	if(!theSuccess)
	{
		EMIT_SOUND(pActivator->edict(), CHAN_ITEM, "common/null.wav", 1.0, ATTN_NORM);
	}
}

bool AvHBaseBuildable::Energize(float inEnergyAmount)
{
	return false;
}

int	AvHBaseBuildable::GetBaseHealth() const
{
	return this->mBaseHealth;
}

char* AvHBaseBuildable::GetClassName() const
{
	return this->mClassName;
}

int AvHBaseBuildable::GetIdleAnimation() const
{
	int theAnimation = this->GetIdle1Animation();
	
	if(RANDOM_LONG(0, 1))
	{
		theAnimation = this->GetIdle2Animation();
	}

	return theAnimation;
}

char* AvHBaseBuildable::GetDeploySound() const
{
	return NULL;
}

bool AvHBaseBuildable::GetIsBuilt() const
{
    return this->mInternalSetConstructionComplete;
}

bool AvHBaseBuildable::GetIsOrganic() const
{
	return false;
}

char* AvHBaseBuildable::GetKilledSound() const
{
	return NULL;
}

float AvHBaseBuildable::GetNormalizedBuildPercentage() const
{
	//return this->pev->fuser1/kNormalizationNetworkFactor;

	bool theIsBuilding;
	bool theIsResearching;
	float thePercentage;
	AvHSHUGetBuildResearchState(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, theIsBuilding, theIsResearching, thePercentage);

	// Check for energy special case
	if(theIsBuilding && theIsResearching)
	{
		thePercentage = 1.0f;
	}

	return thePercentage;
}

float AvHBaseBuildable::GetTimeForAnimation(int inIndex) const
{
	return GetSequenceDuration(GET_MODEL_PTR(ENT(pev)), this->pev);
}

int AvHBaseBuildable::GetStartAlpha() const
{
	return kStartAlpha;
}

void AvHBaseBuildable::FireDeathTarget() const
{
	if(this->mTargetOnDeath != "")
	{
		FireTargets(this->mTargetOnDeath.c_str(), NULL, NULL, USE_TOGGLE, 0.0f);
	}
}

void AvHBaseBuildable::FireSpawnTarget() const
{
	if(this->mTargetOnSpawn != "")
	{
		FireTargets(this->mTargetOnSpawn.c_str(), NULL, NULL, USE_TOGGLE, 0.0f);
	}
}

void AvHBaseBuildable::KeyValue(KeyValueData* pkvd)
{
	// Any entity placed by the mapper is persistent
	this->SetPersistent();

	if(FStrEq(pkvd->szKeyName, "targetonspawn"))
	{
		this->mTargetOnSpawn = pkvd->szValue;
		pkvd->fHandled = TRUE;
	}
	else if(FStrEq(pkvd->szKeyName, "targetondeath"))
	{
		this->mTargetOnDeath = pkvd->szValue;
		pkvd->fHandled = TRUE;
	}
	else if(FStrEq(pkvd->szKeyName, "teamchoice"))
	{
		//this->mTeam = (AvHTeamNumber)(atoi(pkvd->szValue));
		this->pev->team = (AvHTeamNumber)(atoi(pkvd->szValue));
		
		pkvd->fHandled = TRUE;
	}
	else if(FStrEq(pkvd->szKeyName, "angles"))
	{
		// TODO: Insert code here
		//pkvd->fHandled = TRUE;
		int a = 0;
	}
	else
	{
		CBaseAnimating::KeyValue(pkvd);
	}
}

void AvHBaseBuildable::PlayAnimationAtIndex(int inIndex, bool inForce, float inFrameRate)
{
	// Only play animations on buildings that we have artwork for
	bool thePlayAnim = false;

	if(inIndex >= 0)
	{
		switch(this->mMessageID)
		{
		case BUILD_RESOURCES:
		case BUILD_ARMSLAB:
		case BUILD_COMMANDSTATION:
		case BUILD_INFANTRYPORTAL:
		case BUILD_TURRET_FACTORY:
		case TURRET_FACTORY_UPGRADE:
		case BUILD_ARMORY:
		case ARMORY_UPGRADE:
		case BUILD_OBSERVATORY:
		case BUILD_TURRET:
		case BUILD_SIEGE:
		case BUILD_PROTOTYPE_LAB:
		case ALIEN_BUILD_HIVE:
		case ALIEN_BUILD_RESOURCES:
		case ALIEN_BUILD_OFFENSE_CHAMBER:
		case ALIEN_BUILD_DEFENSE_CHAMBER:
		case ALIEN_BUILD_SENSORY_CHAMBER:
		case ALIEN_BUILD_MOVEMENT_CHAMBER:
			thePlayAnim = true;
		}
	}

	// Make sure we're not interrupting another animation
	if(thePlayAnim)
	{
		// Allow forcing of new animation, but it's better to complete current animation then interrupt it and play it again
		float theCurrentTime = gpGlobals->time;
		if((theCurrentTime >= this->mTimeAnimationDone) || (inForce && (inIndex != this->mLastAnimationPlayed)))
		{
			this->pev->sequence	= inIndex;
			this->pev->frame = 0;
			ResetSequenceInfo();

            this->pev->framerate = inFrameRate;

            // Set to last frame to play backwards
            if(this->pev->framerate < 0)
            {
                this->pev->frame = 255;
            }

			this->mLastAnimationPlayed = inIndex;
			float theTimeForAnim = this->GetTimeForAnimation(inIndex);
			this->mTimeAnimationDone = theCurrentTime + theTimeForAnim;
			
			// Recalculate size
			//Vector theMinSize, theMaxSize;
			//this->ExtractBbox(this->pev->sequence, (float*)&theMinSize, (float*)&theMaxSize);
			//UTIL_SetSize(this->pev, theMinSize, theMaxSize); 
		}
	}
}

void AvHBaseBuildable::SetNormalizedBuildPercentage(float inPercentage, bool inForceIfComplete)
{
	// Get previous build percentage so we can add hitpoints as structure is building.  This means that structures that are hurt while building finish hurt.
	bool theIsBuilding, theIsResearching;
	float theNormalizedBuildPercentage = 0.0f;
	AvHSHUGetBuildResearchState(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, theIsBuilding, theIsResearching, theNormalizedBuildPercentage);

	float theDiff = inPercentage - theNormalizedBuildPercentage;
	if(theDiff > 0)
	{
		this->pev->health += theDiff*(1.0f - kBaseHealthPercentage)*this->mBaseHealth;
		this->pev->health = min(max(0.0f, this->pev->health), (float)this->mBaseHealth);
	}
	else
	{
		int a = 0;
	}

	// Set new build state
	AvHSHUSetBuildResearchState(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, true, inPercentage);

	if(inPercentage >= 1.0f)
	{
		this->InternalSetConstructionComplete(inForceIfComplete);
	}
	
	this->HealthChanged();
}

void AvHBaseBuildable::UpdateOnRecycle()
{
	// empty, override to add events on recycle for buildings
}

void AvHBaseBuildable::StartRecycle()
{
	if(!GetHasUpgrade(this->pev->iuser4, MASK_RECYCLING))
	{
        int theRecycleTime = (GetGameRules()->GetCheatsEnabled() && !GetGameRules()->GetIsCheatEnabled(kcSlowResearch)) ? 2 : BALANCE_VAR(kRecycleTime);

        // Play recycle animation in reverse (would like to play them slower according to recycle time, but it doesn't work for all structures, seems dependent on # of keyframes)
        int theAnimation = this->GetRecycleAnimation();
        float theTimeForAnim = this->GetTimeForAnimation(theAnimation);
        float theFrameRate = -1;//-theTimeForAnim/theRecycleTime;
        this->PlayAnimationAtIndex(theAnimation, true, theFrameRate);
		
		// Schedule time to give points back
		SetThink(&AvHBaseBuildable::RecycleComplete);

		this->mTimeRecycleStarted = gpGlobals->time;
		
		this->mTimeRecycleDone = gpGlobals->time + theRecycleTime;
		
		this->pev->nextthink = this->mTimeRecycleDone;

		float theVolume = .5f;
		EMIT_SOUND(this->edict(), CHAN_AUTO, kBuildableRecycleSound, theVolume, ATTN_NORM);

        SetUpgradeMask(&this->pev->iuser4, MASK_RECYCLING);

		// run any events for this class on recycling the structure
		this->UpdateOnRecycle();

        // Remove tech immediately, so research or building isn't started using this tech
        this->TriggerRemoveTech();
	}
}

bool AvHBaseBuildable::GetIsRecycling() const
{
	return GetHasUpgrade(this->pev->iuser4, MASK_RECYCLING);
}

bool AvHBaseBuildable::GetIsTechActive() const
{
	bool theIsActive = false;

	if(this->GetIsBuilt() && (this->pev->health > 0) && !GetHasUpgrade(this->pev->iuser4, MASK_RECYCLING))
	{
		theIsActive = true;
	}

	return theIsActive;
}

int	AvHBaseBuildable::GetActiveAnimation() const
{
	return kBaseBuildableActiveAnimation;
}

CBaseEntity* AvHBaseBuildable::GetAttacker()
{
	CBaseEntity* theAttacker = this;
	
	AvHBuildable* theBuildable = dynamic_cast<AvHBuildable*>(this);
	if(theBuildable)
	{
		int theBuilderIndex = theBuildable->GetBuilder();
		CBaseEntity* theBuilderEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theBuilderIndex));
		if(theBuilderEntity)
		{
			theAttacker = theBuilderEntity;
		}
	}
	
	return theAttacker;
}

int	AvHBaseBuildable::GetDeployAnimation() const
{
	return kBaseBuildableDeployAnimation;
}

int	AvHBaseBuildable::GetIdle1Animation() const
{
	return kBaseBuildableIdle1Animation;
}

int	AvHBaseBuildable::GetIdle2Animation() const
{
	return kBaseBuildableIdle2Animation;
}

int AvHBaseBuildable::GetKilledAnimation() const
{
	return kBaseBuildableDieForwardAnimation;
}

AvHMessageID AvHBaseBuildable::GetMessageID() const
{
	return this->mMessageID;
}

int	AvHBaseBuildable::GetMoveType() const
{
	return MOVETYPE_TOSS;
}

bool AvHBaseBuildable::GetTriggerAlertOnDamage() const
{
	return true;
}

float AvHBaseBuildable::GetTimeAnimationDone() const
{
	return this->mTimeAnimationDone;
}

int	AvHBaseBuildable::GetResearchAnimation() const
{
	return kBaseBuildableResearchingAnimation;
}

// Play deploy animation backwards
int	AvHBaseBuildable::GetRecycleAnimation() const
{
    int theAnimation = -1;
    
    if(this->GetIsBuilt())
    {
        theAnimation = this->GetDeployAnimation();
    }

	return theAnimation;
}

char* AvHBaseBuildable::GetModelName() const
{
	return this->mModelName;
}

int	AvHBaseBuildable::GetSpawnAnimation() const
{
	return kBaseBuildableSpawnAnimation;
}

int	AvHBaseBuildable::GetTakeDamageAnimation() const
{
    int theAnimation = -1;
    
    if(this->GetIsBuilt())
    {
        theAnimation = kBaseBuildableTakeDamageAnimation;
    }

	return theAnimation;
}


AvHTeamNumber AvHBaseBuildable::GetTeamNumber() const
{
	return (AvHTeamNumber)this->pev->team;
}

void AvHBaseBuildable::Killed(entvars_t* pevAttacker, int iGib)
{
	AvHBaseBuildable::SetHasBeenKilled();

	this->mKilled = true;
    this->mInternalSetConstructionComplete = false;

	// puzl: 980
	// Less smoke for recycled buildings
	this->TriggerDeathAudioVisuals(iGib == GIB_RECYCLED);
	
	if(!this->GetIsOrganic())
	{
		// More sparks for recycled buildings
		int numSparks = ( iGib == GIB_RECYCLED ) ? 7 : 3;
		for ( int i=0; i < numSparks; i++ ) {
			Vector vecSrc = Vector( (float)RANDOM_FLOAT( pev->absmin.x, pev->absmax.x ), (float)RANDOM_FLOAT( pev->absmin.y, pev->absmax.y ), (float)0 );
			vecSrc = vecSrc + Vector( (float)0, (float)0, (float)RANDOM_FLOAT( pev->origin.z, pev->absmax.z ) );
			UTIL_Sparks(vecSrc);
		}
	}
	// :puzl
	this->TriggerRemoveTech();

	AvHSURemoveEntityFromHotgroupsAndSelection(this->entindex());

	if(pevAttacker)
	{
		const char* theClassName = STRING(this->pev->classname);
		AvHPlayer* inPlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(ENT(pevAttacker)));
		if(inPlayer && theClassName)
		{
			inPlayer->LogPlayerAction("structure_destroyed", theClassName);
			GetGameRules()->RewardPlayerForKill(inPlayer, this);
		}
	}
	
	if(this->GetIsPersistent())
	{
        this->SetInactive();
	}
	else
	{
		CBaseAnimating::Killed(pevAttacker, iGib);
	}
}

void AvHBaseBuildable::SetActive()
{
    this->pev->effects &= ~EF_NODRAW;
}

void AvHBaseBuildable::SetInactive()
{
	this->pev->health = 0;
    this->pev->effects |= EF_NODRAW;
    this->pev->solid = SOLID_NOT;
    this->pev->takedamage = DAMAGE_NO;
	SetUpgradeMask(&this->pev->iuser4, MASK_PARASITED, false);//voogru: remove parasite flag to prevent phantom parasites.
    //this->pev->deadflag = DEAD_DEAD;
    SetThink(NULL);
}

int	AvHBaseBuildable::ObjectCaps(void)
{
	return FCAP_CONTINUOUS_USE;
}

void AvHBaseBuildable::Precache(void)
{
	CBaseAnimating::Precache();

	char* theDeploySound = this->GetDeploySound();
	if(theDeploySound)
	{
		PRECACHE_UNMODIFIED_SOUND(theDeploySound);
	}
	char* theKilledSound = this->GetKilledSound();
	if(theKilledSound)
	{
		PRECACHE_UNMODIFIED_SOUND(theKilledSound);
	}
	
	PRECACHE_UNMODIFIED_MODEL(this->mModelName);
	
	PRECACHE_UNMODIFIED_SOUND(kBuildableRecycleSound);
	//PRECACHE_UNMODIFIED_SOUND(kBuildableHurt1Sound);
	//PRECACHE_UNMODIFIED_SOUND(kBuildableHurt2Sound);

	this->mElectricalSprite = PRECACHE_UNMODIFIED_MODEL(kElectricalSprite);
}

void AvHBaseBuildable::RecycleComplete()
{
	// Look at whether it has been built and health to determine how many points to give back
	float thePercentage = BALANCE_VAR(kRecycleResourcePercentage);

	if(!this->GetIsBuilt())
	{
		thePercentage = .8f;
	}

	// Make sure the building is still alive, can't get points back if it's dead
	if(this->pev->health <= 0)
	{
		thePercentage = 0.0f;
	}

	// Look up team
	AvHTeam* theTeam = GetGameRules()->GetTeam((AvHTeamNumber)this->pev->team);
	if(theTeam)
	{
		bool theIsEnergyTech = AvHSHUGetDoesTechCostEnergy(this->mMessageID);
		ASSERT(!theIsEnergyTech);

		float thePointsBack = GetGameRules()->GetCostForMessageID(this->mMessageID)*thePercentage;
		theTeam->SetTeamResources(theTeam->GetTeamResources() + thePointsBack);

        // Play "+ resources" event
        AvHSUPlayNumericEventAboveStructure(thePointsBack, this);

		// puzl: 980
		// Less smoke and more sparks  for recycled buildings
		this->Killed(this->pev, GIB_RECYCLED);
		// :puzl
	}
}

// Sets the template iuser3 for this buildable.  This is stored outside of the actual iuser3 because sometimes the pev isn't allocated yet.
void AvHBaseBuildable::SetSelectID(int inSelectID)
{
	this->mSelectID = inSelectID;
}

bool AvHBaseBuildable::Regenerate(float inRegenerationAmount, bool inPlaySound)
{
	bool theDidHeal = false;
	
	float theMaxHealth = this->mBaseHealth;

	if(!this->GetIsBuilt())
	{
		float theNormalizedBuildPercentage = this->GetNormalizedBuildPercentage();

		theMaxHealth = (kBaseHealthPercentage + theNormalizedBuildPercentage*(1.0f - kBaseHealthPercentage))*this->mBaseHealth;
	}

	// If we aren't at full health, heal health
	if(this->pev->health < theMaxHealth)
	{
		this->pev->health = min(theMaxHealth, this->pev->health + inRegenerationAmount);
		this->HealthChanged();
		theDidHeal = true;
	}
	
	// Play regen event
	if(theDidHeal)
	{
		if(inPlaySound)
		{
			// Play regeneration event
			PLAYBACK_EVENT_FULL(0, this->edict(), gRegenerationEventID, 0, this->pev->origin, (float *)&g_vecZero, 1.0f, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
		}
	}

	return theDidHeal;
}

void AvHBaseBuildable::ResetEntity()
{
	CBaseAnimating::ResetEntity();

	this->Init();

	this->Materialize();

	this->pev->effects = 0;

	// Build it if marked as starting built
	if(this->pev->spawnflags & 1)
		this->SetConstructionComplete(true);

	this->mKilled = false;
}

void AvHBaseBuildable::InternalSetConstructionComplete(bool inForce)
{
	if(!this->mInternalSetConstructionComplete || inForce)
	{
		// Fully built items are no longer marked as buildable
		SetUpgradeMask(&this->pev->iuser4, MASK_BUILDABLE, false);
		
		this->pev->rendermode = kRenderNormal;
		this->pev->renderamt = 255;
		
		this->SetHasBeenBuilt();

        this->SetActive();
        
        this->mInternalSetConstructionComplete = true;

		this->TriggerAddTech();
		
		char* theDeploySound = this->GetDeploySound();
		if(theDeploySound)
		{
			EMIT_SOUND(ENT(this->pev), CHAN_WEAPON, theDeploySound, 1, ATTN_NORM);
		}
		
		int theDeployAnimation = this->GetDeployAnimation();
		
		this->PlayAnimationAtIndex(theDeployAnimation, true);
	}
}

void AvHBaseBuildable::SetConstructionComplete(bool inForce)
{
	this->SetNormalizedBuildPercentage(1.0f, inForce);
}

void AvHBaseBuildable::SetAverageUseSoundLength(float inLength)
{
	this->mAverageUseSoundLength = inLength;
}

void AvHBaseBuildable::SetResearching(bool inState)
{
	int theSequence = this->GetResearchAnimation();

	if(!inState)
	{
		theSequence = this->GetIdleAnimation();
	}

	this->PlayAnimationAtIndex(theSequence, true);

	this->mIsResearching = inState;
}

// Requires mSelectID and mMessageID to be set
// Sets the pev user variables, mBaseHealth, pev->health and pev->armorvalue
void AvHBaseBuildable::InternalInitializeBuildable()
{
	// Always buildable
	InitializeBuildable(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, this->mSelectID);
	this->mBaseHealth = GetGameRules()->GetBaseHealthForMessageID(this->mMessageID);
	this->pev->health = this->mBaseHealth*kBaseHealthPercentage;
	this->pev->max_health = this->mBaseHealth;

	// Store max health in armorvalue
	//this->pev->armorvalue = GetGameRules()->GetBaseHealthForMessageID(this->mMessageID);
}

const float kFallThinkInterval = .1f;

void AvHBaseBuildable::Spawn()
{
	this->Precache();

	CBaseAnimating::Spawn();

	// Get building size in standard way
	SET_MODEL(ENT(this->pev), this->mModelName);

	pev->movetype = this->GetMoveType();
	pev->solid = SOLID_BBOX;
	
	UTIL_SetOrigin( pev, pev->origin );

	this->Materialize();

    SetTouch(&AvHBaseBuildable::BuildableTouch);

	if(this->pev->spawnflags & 1)
		this->SetConstructionComplete(true);
}


void AvHBaseBuildable::FallThink()
{
	pev->nextthink = gpGlobals->time + kFallThinkInterval;
	
	if ( pev->flags & FL_ONGROUND )
	{
		this->Materialize();

		// Start animating
		SetThink(&AvHBaseBuildable::AnimateThink);
		this->pev->nextthink = gpGlobals->time + kAnimateThinkTime;
	}
}

int	AvHBaseBuildable::GetSequenceForBoundingBox() const
{
	return 0;
}

void AvHBaseBuildable::Materialize()
{
	this->pev->solid = SOLID_BBOX;
	this->pev->movetype = this->GetMoveType();
	
	this->pev->classname = MAKE_STRING(this->mClassName);
	
	this->pev->takedamage = DAMAGE_YES;
	SetBits(this->pev->flags, FL_MONSTER);
	
	// Always buildable
	this->InternalInitializeBuildable();
	
	this->SetNormalizedBuildPercentage(0.0f);
	
	// NOTE: fuser2 is used for repairing structures

	Vector theMinSize, theMaxSize;
	//int theSequence = this->GetSequenceForBoundingBox();

	// Get height needed for model
	//this->ExtractBbox(theSequence, (float*)&theMinSize, (float*)&theMaxSize);
	//float theHeight = theMaxSize.z - theMinSize.z;

	AvHSHUGetSizeForTech(this->GetMessageID(), theMinSize, theMaxSize);

	UTIL_SetSize(pev, theMinSize, theMaxSize);

	this->PlayAnimationAtIndex(this->GetSpawnAnimation(), true);
		
	SetUse(&AvHBaseBuildable::ConstructUse);
}

int	AvHBaseBuildable::TakeDamage(entvars_t* inInflictor, entvars_t* inAttacker, float inDamage, int inBitsDamageType)
{
	if(GetGameRules()->GetIsCheatEnabled(kcHighDamage))
	{
		inDamage *= 50;
	}

	if(!inAttacker)
	{
		inAttacker = inInflictor;
	}
	
	if(!inInflictor)
	{
		inInflictor = inAttacker;
	}

	// Take into account handicap
	AvHTeam* theTeam = GetGameRules()->GetTeam(AvHTeamNumber(inAttacker->team));
	if(theTeam)
	{
		float theHandicap = theTeam->GetHandicap();
		inDamage *= theHandicap;
	}
	
	CBaseEntity* inInflictorEntity = CBaseEntity::Instance(inInflictor);
	float theDamage = 0;

	// Take half damage from piercing
	if(inBitsDamageType & NS_DMG_PIERCING)
	{
		inDamage /= 2.0f;
	}

	// Take double damage from blast
	if(inBitsDamageType & NS_DMG_BLAST)
	{
		inDamage *= 2.0f;
	}
	
	if((inBitsDamageType & NS_DMG_ORGANIC) && !this->GetIsOrganic())
	{
		inDamage = 0.0f;
	}

	theDamage = AvHPlayerUpgrade::CalculateDamageLessArmor((AvHUser3)this->pev->iuser3, this->pev->iuser4, inDamage, this->pev->armorvalue, inBitsDamageType, GetGameRules()->GetNumActiveHives((AvHTeamNumber)this->pev->team));
	if(theDamage > 0)
	{
        int theAnimationIndex = this->GetTakeDamageAnimation();
        if(theAnimationIndex >= 0)
        {
            this->PlayAnimationAtIndex(theAnimationIndex, true);
        }

        // Award experience to attacker
        CBaseEntity* theEntity = CBaseEntity::Instance(ENT(inAttacker));
        AvHPlayer* inAttacker = dynamic_cast<AvHPlayer*>(theEntity);
        if(inAttacker && (inAttacker->pev->team != this->pev->team))
        {
            inAttacker->AwardExperienceForObjective(theDamage, this->GetMessageID());
        }
	}
	
	int theReturnValue = 0;
	
	if(theDamage > 0.0f)
	{
		if(this->GetTriggerAlertOnDamage())
			GetGameRules()->TriggerAlert((AvHTeamNumber)this->pev->team, ALERT_UNDER_ATTACK, this->entindex());
		
		theDamage = CBaseAnimating::TakeDamage(inInflictor, inAttacker, inDamage, inBitsDamageType);

		bool theDrawDamage = (CVAR_GET_FLOAT(kvDrawDamage) > 0);

		if(theDrawDamage)
		{
			Vector theMinSize;
			Vector theMaxSize;
			AvHSHUGetSizeForTech(this->GetMessageID(), theMinSize, theMaxSize);
			
			Vector theStartPos = this->pev->origin;
			theStartPos.z += theMaxSize.z;
			
			// Draw for everyone (team is 0 after inDamage parameter)
			AvHSUPlayNumericEvent(-inDamage, this->edict(), theStartPos, 0, kNumericalInfoHealthEvent, 0);
		}
	}

	// Structures uncloak when damaged
	this->Uncloak();

	this->HealthChanged();
	
	return theDamage;
}

void AvHBaseBuildable::TechnologyBuilt(AvHMessageID inMessageID)
{
}

void AvHBaseBuildable::WorldUpdate()
{
	this->UpdateTechSlots();

	// Organic buildings heal themselves
	if(this->GetIsOrganic())
	{
		this->UpdateAutoHeal();
	}
	else
	{
		//this->UpdateDamageEffects();
	}

	// If we're electrified, set render mode
	if(GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_11))
	{
		// Base marine building
		const int kElectrifyRenderMode = kRenderFxGlowShell;
		const int kElectrifyRenderAmount = 40;

		this->pev->renderfx = kElectrifyRenderMode;
		this->pev->renderamt = kElectrifyRenderAmount;
		this->pev->rendercolor.x = kTeamColors[this->pev->team][0];
		this->pev->rendercolor.y = kTeamColors[this->pev->team][1];
		this->pev->rendercolor.z = kTeamColors[this->pev->team][2];

		// Check for enemy players/structures nearby
		CBaseEntity* theBaseEntity = NULL;
		int theNumEntsDamaged = 0;
		
		while(((theBaseEntity = UTIL_FindEntityInSphere(theBaseEntity, this->pev->origin, BALANCE_VAR(kElectricalRange))) != NULL) && (theNumEntsDamaged < BALANCE_VAR(kElectricalMaxTargets)))
		{
			// When "electric" cheat is enabled, shock all non-self entities, else shock enemies
			if((GetGameRules()->GetIsCheatEnabled(kcElectric) && (theBaseEntity != this)) || ((theBaseEntity->pev->team != this->pev->team) && theBaseEntity->IsAlive()))
			{
				// Make sure it's not blocked
				TraceResult theTraceResult;
				UTIL_TraceLine(this->pev->origin, theBaseEntity->pev->origin, ignore_monsters, dont_ignore_glass, this->edict(), &theTraceResult);
				if(theTraceResult.flFraction == 1.0f)
				{
					CBaseEntity* theAttacker = this->GetAttacker();
					ASSERT(theAttacker);

					if(theBaseEntity->TakeDamage(this->pev, theAttacker->pev, BALANCE_VAR(kElectricalDamage), DMG_GENERIC) > 0)
					{
							MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
								WRITE_BYTE(TE_BEAMENTPOINT);
								WRITE_SHORT(theBaseEntity->entindex());
								WRITE_COORD( this->pev->origin.x);
								WRITE_COORD( this->pev->origin.y);
								WRITE_COORD( this->pev->origin.z);
						
								WRITE_SHORT( this->mElectricalSprite );
								WRITE_BYTE( 0 ); // framestart
								WRITE_BYTE( (int)15); // framerate
								WRITE_BYTE( (int)(2) ); // life
								WRITE_BYTE( 60 );  // width
								WRITE_BYTE( 15 );   // noise
								WRITE_BYTE( (int)this->pev->rendercolor.x );   // r, g, b
								WRITE_BYTE( (int)this->pev->rendercolor.y );   // r, g, b
								WRITE_BYTE( (int)this->pev->rendercolor.z );   // r, g, b
								WRITE_BYTE( 200 );	// brightness
								WRITE_BYTE( 10 );	// speed
							MESSAGE_END();
						
							gSoundListManager.PlaySoundInList(kElectricSparkSoundList, this, CHAN_AUTO, .7f);
						
							UTIL_Sparks(theBaseEntity->pev->origin);
						
							theNumEntsDamaged++;
					}
				}
			}
		}
	}
}

bool AvHBaseBuildable::GetHasBeenKilled() const
{
	return this->mKilled;
}

bool AvHBaseBuildable::GetIsTechnologyAvailable(AvHMessageID inMessageID) const
{
	bool theTechnologyAvailable = false;
	
	const AvHTeam* theTeam = GetGameRules()->GetTeam((AvHTeamNumber)this->pev->team);
	if(theTeam)
	{
        // Don't allow electrical upgrade if we're already electrified
        if((inMessageID != RESEARCH_ELECTRICAL) || !GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_11))
        {
            theTechnologyAvailable = (theTeam->GetIsTechnologyAvailable(inMessageID) && this->GetIsBuilt() && !GetHasUpgrade(this->pev->iuser4, MASK_RECYCLING));

            // Enable recycle button for unbuilt structures
            if(!this->GetIsBuilt() && (inMessageID == BUILD_RECYCLE))
            {
                theTechnologyAvailable = true;
            }
        }
	}	
	return theTechnologyAvailable;
}


void AvHBaseBuildable::UpdateTechSlots()
{
	// Get tech slot for this structure
	AvHGamerules* theGameRules = GetGameRules();
	const AvHTeam* theTeam = theGameRules->GetTeam((AvHTeamNumber)this->pev->team);
	if(theTeam)
	{
		// Update tech slots
		AvHTechSlots theTechSlots;
		if(theTeam->GetTechSlotManager().GetTechSlotList((AvHUser3)this->pev->iuser3, theTechSlots))
		{
			// Clear the existing slots
			int theMasks[kNumTechSlots] = {MASK_UPGRADE_1, MASK_UPGRADE_2, MASK_UPGRADE_3, MASK_UPGRADE_4, MASK_UPGRADE_5, MASK_UPGRADE_6, MASK_UPGRADE_7, MASK_UPGRADE_8};
			
			// Each slot if we technology is available
			for(int i = 0; i < kNumTechSlots; i++)
			{
				int theCurrentMask = theMasks[i];
				this->pev->iuser4 &= ~theCurrentMask;
				
				AvHMessageID theMessage = theTechSlots.mTechSlots[i];
				if(theMessage != MESSAGE_NULL)
				{
					if(this->GetIsTechnologyAvailable(theMessage))
					{
						this->pev->iuser4 |= theCurrentMask;
					}
				}
			}
		}

		// Update recycling status bar
		if(GetHasUpgrade(this->pev->iuser4, MASK_RECYCLING))
		{
			float theNormalizedRecyclingFactor = (gpGlobals->time - this->mTimeRecycleStarted)/(this->mTimeRecycleDone - this->mTimeRecycleStarted);
			theNormalizedRecyclingFactor = min(max(theNormalizedRecyclingFactor, 0.0f), 1.0f);
			
			//theResearchEntity->pev->fuser1 = (kResearchFuser1Base + theNormalizedResearchFactor)*kNormalizationNetworkFactor;
			AvHSHUSetBuildResearchState(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, false, theNormalizedRecyclingFactor);
		}
	}
}

void AvHBaseBuildable::TriggerDeathAudioVisuals(bool isRecycled)
{
	AvHClassType theTeamType = AVH_CLASS_TYPE_UNDEFINED;
	AvHTeam* theTeam = GetGameRules()->GetTeam((AvHTeamNumber)this->pev->team);
	if(theTeam)
	{
		theTeamType = theTeam->GetTeamType();
	}
	
	switch(theTeamType)
	{
	case AVH_CLASS_TYPE_ALIEN:
		AvHSUPlayParticleEvent(kpsChamberDeath, this->edict(), this->pev->origin);
		break;
		
	case AVH_CLASS_TYPE_MARINE:
		// lots of smoke
		// puzl: 980
		// Less smoke for recycled buildings
		int smokeScale = isRecycled ? 15 : 25;
		MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
		WRITE_BYTE( TE_SMOKE );
		WRITE_COORD( RANDOM_FLOAT( pev->absmin.x, pev->absmax.x ) );
		WRITE_COORD( RANDOM_FLOAT( pev->absmin.y, pev->absmax.y ) );
		WRITE_COORD( RANDOM_FLOAT( pev->absmin.z, pev->absmax.z ) );
		WRITE_SHORT( g_sModelIndexSmoke );
		WRITE_BYTE( smokeScale ); // scale * 10
		WRITE_BYTE( 10 ); // framerate
		MESSAGE_END();
		break;
	}
	
	char* theKilledSound = this->GetKilledSound();
	if(theKilledSound)
	{
		EMIT_SOUND(ENT(this->pev), CHAN_AUTO, theKilledSound, 1.0, ATTN_IDLE);
	}
}

void AvHBaseBuildable::UpdateAutoBuild(float inTimePassed)
{
	if(GetGameRules()->GetGameStarted())
	{
		// TF2 snippet for making sure players don't get stuck in buildings
		if(this->pev->solid == SOLID_NOT)
		{
			//trace_t tr;
			//UTIL_TraceHull(this->pev->origin, this->pev->origin, this->pev->mins, this->pev->maxs, this->pev, &tr);
			//if(!tr.startsolid && !tr.allsolid )
			//if(AvHSHUGetIsAreaFree(this->pev->origin, this->pev->mins, this->pev->maxs, this->edict()))

			// Check point contents for corner points
			float theMinX = this->pev->origin.x + this->pev->mins.x;
			float theMinY = this->pev->origin.y + this->pev->mins.y;
			float theMinZ = this->pev->origin.z + this->pev->mins.z;
			float theMaxX = this->pev->origin.x + this->pev->maxs.x;
			float theMaxY = this->pev->origin.y + this->pev->maxs.y;
			float theMaxZ = this->pev->origin.z + this->pev->maxs.z;

			// Do tracelines between the corners, to make sure there's no geometry inside the box
			Vector theMinVector(theMinX, theMinY, theMinZ);
			Vector theMaxVector(theMaxX, theMaxY, theMaxZ);
			if(AvHSHUTraceLineIsAreaFree(theMinVector, theMaxVector, this->edict()))
			{
			  theMinVector = Vector(theMaxX, theMinY, theMinZ);
			  theMaxVector = Vector(theMinX, theMaxY, theMaxZ);
				if(AvHSHUTraceLineIsAreaFree(theMinVector, theMaxVector, this->edict()))
				{
				  theMinVector = Vector(theMaxX, theMaxY, theMinZ);
				  theMaxVector = Vector(theMinX, theMinY, theMaxZ);
					if(AvHSHUTraceLineIsAreaFree(theMinVector, theMaxVector, this->edict()))
					{
						this->pev->solid = SOLID_BBOX;

						// Relink into world (not sure if this is necessary)
						UTIL_SetOrigin(this->pev, this->pev->origin);
					}
				}
			}
		}
		else
		{
			// If it's not fully built, build more
			bool theIsBuilding, theIsResearching;
			float thePercentage;
			AvHSHUGetBuildResearchState(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, theIsBuilding, theIsResearching, thePercentage);
			
			float theBuildTime = GetGameRules()->GetBuildTimeForMessageID(this->GetMessageID());
			float theBuildPercentage = inTimePassed/theBuildTime;
			
			float theNewPercentage = min(thePercentage + theBuildPercentage, 1.0f);
			this->SetNormalizedBuildPercentage(theNewPercentage);
			
//			// Increase built time if not fully built
//			if(!this->GetHasBeenBuilt() && (theNewPercentage >= 1.0f))
//			{
//				this->SetConstructionComplete();
//			}
////			else
////			{
////				this->pev->rendermode = kRenderTransTexture;
////				int theStartAlpha = this->GetStartAlpha();
////				this->pev->renderamt = theStartAlpha + theNewPercentage*(255 - theStartAlpha);
////			}
//
//			AvHSHUSetBuildResearchState(this->pev->iuser3, this->pev->iuser4, this->pev->fuser1, true, theNewPercentage);
			
			// TODO: Heal self?
			// TODO: Play ambient sounds?
		}
	}
}

void AvHBaseBuildable::UpdateAutoHeal()
{
	if(GetGameRules()->GetGameStarted() && this->GetIsBuilt())
	{
		if((this->mTimeOfLastAutoHeal != -1) && (gpGlobals->time > this->mTimeOfLastAutoHeal))
		{
			float theMaxHealth = GetGameRules()->GetBaseHealthForMessageID(this->GetMessageID());
			if(this->pev->health < theMaxHealth)
			{
				float theTimePassed = (gpGlobals->time - this->mTimeOfLastAutoHeal);
				float theHitPointsToGain = theTimePassed*BALANCE_VAR(kOrganicStructureHealRate);
			
				this->pev->health += theHitPointsToGain;
				this->pev->health = min(this->pev->health, theMaxHealth);
			
				this->HealthChanged();
			}
		}

		this->mTimeOfLastAutoHeal = gpGlobals->time;
	}
}

void AvHBaseBuildable::UpdateDamageEffects()
{
	if(GetGameRules()->GetGameStarted() && this->GetIsBuilt())
	{
		// Add special effects for structures that are hurt or almost dead
		float theMaxHealth = GetGameRules()->GetBaseHealthForMessageID(this->GetMessageID());
		float theHealthScalar = this->pev->health/theMaxHealth;
		float theTimeInterval = max(gpGlobals->time - this->mTimeOfLastDamageUpdate, .1f);

		const float kParticleSystemLifetime = 5.0f;
		int theAverageSoundInterval = -1;
		
		// If we're at 25% health or less, emit black smoke
		if(gpGlobals->time > (this->mTimeOfLastDamageEffect + kParticleSystemLifetime))
		{
			if(theHealthScalar < .25f)
			{
				AvHSUPlayParticleEvent(kpsBuildableLightDamage, this->edict(), this->pev->origin);
				this->mTimeOfLastDamageEffect = gpGlobals->time;
				theAverageSoundInterval = 3;
			}
			// If we're at 50% health or less, emit light smoke
			else if(theHealthScalar < .5f)
			{
				AvHSUPlayParticleEvent(kpsBuildableLightDamage, this->edict(), this->pev->origin);
				this->mTimeOfLastDamageEffect = gpGlobals->time;
				theAverageSoundInterval = 5;
			}
		}
		
		// If we're at less then 75% health, spark occasionally
		if(theHealthScalar < .75f)
		{
			int theRandomChance = RANDOM_LONG(0, (float)8/theTimeInterval);
			if(theRandomChance == 0)
			{
				UTIL_Sparks(this->pev->origin);
				UTIL_Sparks(this->pev->origin);

				const char* theHurtSoundToPlay = kBuildableHurt1Sound;
				if(RANDOM_LONG(0, 1) == 1)
				{
					theHurtSoundToPlay = kBuildableHurt2Sound;
				}
				
				float theVolume = .3f;
				EMIT_SOUND(this->edict(), CHAN_AUTO, theHurtSoundToPlay, theVolume, ATTN_NORM);
			}
		}
		
		this->mTimeOfLastDamageUpdate = gpGlobals->time;
	}
}


void AvHBaseBuildable::HealthChanged()
{
	int theMaxHealth = this->mBaseHealth;//this->pev->armorvalue;
	int theCurrentHealth = this->pev->health;
		
	float theNewHealthPercentage = (float)theCurrentHealth/theMaxHealth;
	this->pev->fuser2 = theNewHealthPercentage*kNormalizationNetworkFactor;
}

bool AvHBaseBuildable::GetIsPersistent() const
{
	return this->mPersistent;
}

void AvHBaseBuildable::SetPersistent()
{
	this->mPersistent = true;
}

