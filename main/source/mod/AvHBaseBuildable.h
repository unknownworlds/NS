//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHBaseBuildable.h$
// $Date: 2002/11/15 04:47:11 $
//
//-------------------------------------------------------------------------------
// $Log: AvHBaseBuildable.h,v $
// Revision 1.11  2002/11/15 04:47:11  Flayra
// - Regenerate now returns bool if healed or not, for def chamber tweak
//
// Revision 1.10  2002/11/06 01:38:24  Flayra
// - Added ability for buildings to be enabled and disabled, for turrets to be shut down
// - Damage refactoring (TakeDamage assumes caller has already adjusted for friendly fire, etc.)
//
// Revision 1.9  2002/10/03 18:38:56  Flayra
// - Fixed problem where regenerating builders via healing spray wasn't updating health ring
// - Allow buildings to play custom damage alerts (for towers)
//
// Revision 1.8  2002/09/23 22:10:14  Flayra
// - Removed progress bar when building
// - Removed vestiges of fading building as building
// - Changed point costs
//
// Revision 1.7  2002/09/09 19:49:09  Flayra
// - Buildables now play animations better, without interrupting previous anims
//
// Revision 1.6  2002/08/31 18:01:01  Flayra
// - Work at VALVe
//
// Revision 1.5  2002/08/16 02:32:45  Flayra
// - Added damage types
// - Added visual health for commander and marines
//
// Revision 1.4  2002/07/01 21:15:46  Flayra
// - Added auto-build capability
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
#ifndef AVH_BASE_BUILDABLE_H
#define AVH_BASE_BUILDABLE_H

#include "util/nowarnings.h"
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "mod/AvHConstants.h"
#include "dlls/cbase.h"
#include "mod/AvHMessage.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHPlayer.h"
#include "mod/AvHCloakable.h"
#include "mod/AvHBuildable.h"

class AvHBaseBuildable : public CBaseAnimating, public AvHBuildable, public AvHCloakable
{
public:
					AvHBaseBuildable(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inUser3 = AVH_USER3_NONE);

	void EXPORT		AnimateThink();

	virtual int		BloodColor(void);

    void EXPORT		BuildableTouch(CBaseEntity* inEntity);

	virtual void	CheckEnabledState();
					
	void EXPORT		ConstructUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	virtual bool	Energize(float inEnergyAmount);

	void EXPORT		FallThink(void);

	virtual void	FireDeathTarget() const;

	virtual void	FireSpawnTarget() const;

	virtual int		GetBaseHealth() const;

	virtual int		GetIdleAnimation() const;

	virtual bool	GetIsRecycling() const;
	
	virtual CBaseEntity* GetAttacker();

	virtual char*	GetClassName() const;

	virtual char*	GetDeploySound() const;

	virtual bool	GetIsBuilt(void) const;

	virtual bool	GetIsOrganic() const;

	virtual bool	GetIsTechActive() const;

	virtual char*	GetKilledSound() const;

	virtual int		GetActiveAnimation() const;

	virtual int		GetDeployAnimation() const;

	virtual int		GetIdle1Animation() const;

	virtual int		GetIdle2Animation() const;

    virtual bool	GetIsTechnologyAvailable(AvHMessageID inMessageID) const;

	virtual int		GetKilledAnimation() const;

	virtual AvHMessageID	GetMessageID() const;

	virtual int		GetMoveType() const;

	virtual int		GetSequenceForBoundingBox() const;

	virtual bool	GetTriggerAlertOnDamage() const;

	virtual float	GetTimeAnimationDone() const;

	virtual int		GetRecycleAnimation() const;

	virtual int		GetResearchAnimation() const;

	virtual int		GetSpawnAnimation() const;

	virtual int		GetTakeDamageAnimation() const;
		
	virtual char*	GetModelName() const;
	
	virtual float	GetNormalizedBuildPercentage() const;

	virtual float	GetTimeForAnimation(int inIndex) const;
	
	virtual void	SetResearching(bool inState);

	virtual bool	GetIsPersistent() const;
	
	virtual AvHTeamNumber	GetTeamNumber() const;

	virtual void	HealthChanged();
		
	virtual void	KeyValue(KeyValueData* pkvd);

	virtual void	Killed(entvars_t* pevAttacker, int iGib);

	virtual int		ObjectCaps(void);

    // Pass -1 to play an animation backwards
	virtual void	PlayAnimationAtIndex(int inIndex, bool inForce = false, float inFrameRate = 1.0f);
				
	virtual void	Precache(void);

	virtual bool	Regenerate(float inRegenerationAmount, bool inPlaySound = true, bool dcHealing = false);
	
	virtual void	ResetEntity();

	virtual void	SetConstructionComplete(bool inForce = false);

	virtual void	SetAverageUseSoundLength(float inLength);

	virtual void	StartRecycle();
				
	virtual void	Spawn();

	virtual int		TakeDamage(entvars_t* inInflictor, entvars_t* inAttacker, float inDamage, int inBitsDamageType);

	virtual void	TechnologyBuilt(AvHMessageID inMessageID);

	virtual void	WorldUpdate();

	virtual void	UpdateOnRecycle();

	virtual Vector EyePosition( );
protected:
	virtual bool	GetHasBeenKilled() const;

	int				GetStartAlpha() const;

	void			Init();

	void			InternalInitializeBuildable();

	void			InternalSetConstructionComplete(bool inForce = false);

	virtual void	Materialize();

	void EXPORT		RecycleComplete();
    
    // Called when a persistent buildable is killed
    virtual void	SetActive();
    virtual void    SetInactive();

	virtual void	SetNormalizedBuildPercentage(float inPercentage, bool inForceIfComplete = false);

	void			SetPersistent();

	void			SetSelectID(int inSelectID);

	virtual void	TriggerDeathAudioVisuals(bool isRecycled=false);

	void			UpdateAutoBuild(float inTimePassed);

	void			UpdateAutoHeal();

	void			UpdateDamageEffects();

	virtual void	UpdateTechSlots();

	int				mBaseHealth;
	AvHMessageID	mMessageID;
	
private:
	
	char*			mClassName;
	char*			mModelName;
	int				mSelectID;
	
	//float			mPercentageBuilt;
	float			mLastTimePlayedSound;
	float			mTimeToConstruct;
	const int		kStartAlpha;

	float			mAverageUseSoundLength;

	int				mLastAnimationPlayed;
	float			mTimeAnimationDone;

	// Marks entity to not be removed on level reset
	bool			mPersistent;
	int				mElectricalSprite;
	bool			mIsResearching;

	string			mTargetOnSpawn;
	string			mTargetOnDeath;

	float			mTimeOfLastAutoHeal;
	bool			mInternalSetConstructionComplete;

	float			mTimeOfLastDamageEffect;
	float			mTimeOfLastDamageUpdate;

	float			mTimeRecycleStarted;
	float			mTimeRecycleDone;
	float				mTimeOfLastDCRegeneration;
	bool			mKilled;

	bool			mGhost;
};

#endif