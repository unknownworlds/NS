//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHEntities.h$
// $Date: 2002/11/12 02:23:22 $
//
//-------------------------------------------------------------------------------
// $Log: AvHEntities.h,v $
// Revision 1.32  2002/11/12 02:23:22  Flayra
// - Removed ancient egg
//
// Revision 1.31  2002/10/03 18:42:31  Flayra
// - func_resources now are destroyed if a resource tower is destroyed
//
// Revision 1.30  2002/09/23 22:12:36  Flayra
// - Resource towers give 3 points to team in general
//
// Revision 1.29  2002/07/23 17:01:33  Flayra
// - Updated resource model, removed old junk
//
// Revision 1.28  2002/07/10 14:40:04  Flayra
// - Fixed bug where .mp3s weren't being processed client-side
//
// Revision 1.27  2002/07/08 16:55:35  Flayra
// - Added max ensnare, can't remember why I'm tagging team starts, changed resources functions to floats (for proper handicapping)
//
// Revision 1.26  2002/06/25 17:54:20  Flayra
// - New info_location entity, make resource tower sounds very quiet
//
// Revision 1.25  2002/05/28 17:37:01  Flayra
// - Track number of web strands to enforce limit, reworking of webs in general, removed duplicate code for AvHResourceTower
//
// Revision 1.24  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_ENTITIES_H
#define AVH_ENTITIES_H

#include "util/nowarnings.h"
#include "extdll.h"
#include "util.h"
#include "dlls/cbase.h"
#include "mod/AvHConstants.h"
#include "dlls/func_break.h"
#include "mod/AvHSpecials.h"
#include "dlls/cbasedoor.h"
#include "dlls/effects.h"
#include "mod/AvHMapExtents.h"
#include "mod/AvHBuildable.h"
#include "mod/AvHPlayer.h"
#include "mod/AvHBaseBuildable.h"
#include "mod/AvHBaseInfoLocation.h"
#include "mod/AvHCommandConstants.h"

void ExplodeEntity(CBaseEntity* inEntity, Materials inMaterial);

class AvHBaseEntity : public CBaseEntity
{
public:
						AvHBaseEntity();

	AvHTeamNumber		GetTeamNumber() const;

	void EXPORT			FallThink(void);
	
	virtual void		KeyValue( KeyValueData* pkvd );

	virtual void		NotifyUpgrade(AvHUpgradeMask inUpgradeMask);

private:
	//AvHTeamNumber		mTeam;
};


class AvHClientCommandEntity : public AvHBaseEntity
{
public:
						AvHClientCommandEntity();

	void EXPORT			ClientCommandTouch( CBaseEntity *pOther );

	//virtual void		KeyValue( KeyValueData* pkvd );

	virtual const char*	GetClientCommand() = 0;

	virtual void		Spawn(void);

private:
};

class AvHJoinTeamEntity : public AvHClientCommandEntity
{
public:
	virtual const char*	GetClientCommand();
};

class AvHSpectateEntity : public AvHClientCommandEntity
{
public:
	virtual const char*	GetClientCommand()
							{ return kcSpectate; }

};

//class AvHLeaveGameEntity : public AvHClientCommandEntity
//{
//public:
//	virtual const char*	GetClientCommand()
//							{ return kcReadyRoom; }
//
//};

class AvHTeamStartEntity : public AvHBaseEntity
{
public:
	virtual void	KeyValue( KeyValueData* pkvd );

	virtual void	Spawn(void);
	
	
};

class AvHAutoAssignEntity : public AvHClientCommandEntity
{
public:
	virtual const char*	GetClientCommand()
							{ return kcAutoAssign; }

};
//
//class AvHBuildableAnimating : public CBaseAnimating
//{
//public:
//	void EXPORT			BuildUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
//
//	virtual int			GetMinHitPoints() const;
//
//	virtual int			GetMaxHitPoints() const;
//	
//	virtual bool		GetIsBuilt() const;
//
//	virtual void		SetIsBuilt();
//
//	virtual float		GetNormalizedBuildPercentage() const;
//
//	virtual void		SetNormalizedBuildPercentage(float inPercentage);
//
//	virtual void		Spawn(void);
//
//private:
//	float				mTimeLastUsed;
//	
//};


// Build site entities
//class AvHResource : public AvHBaseEntity
//class AvHResource : public AvHBaseBuildable //CBaseAnimating
//{
//public:
//						AvHResource(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inHealth, AvHSelectableUser4 inUser4);
//	
//	int					BloodColor( void );
//	virtual float		GetBuildRange(void) const;
//	virtual bool		GetIsActive(void) const;
//
//	virtual void		Killed( entvars_t *pevAttacker, int iGib );
//	virtual int			ObjectCaps(void);
//	virtual void		Precache(void);
//
//	virtual float		GetTimeLastContributed();
//	virtual void		SetTimeLastContributed(float inTime);
//
//	void EXPORT			ResourceThink(void);
//	void EXPORT			ResourceTouch( CBaseEntity *pOther );
//	//void EXPORT			ResourceUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
//
//	virtual void		SetConstructionComplete();
//	virtual void		Spawn(void);
//	virtual int			TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
//
//private:
//	//void				LoopSound(void) const;
//	//void				PlayBuildSound(void);
//	//void				StopBuildSound(void);
//
//	const float			mThinkInterval;
//	bool				mBuildSoundPlaying;
//	int					mBuildRange;
//	int					mResourceRating;
//	bool				mHasResource;
//	bool				mStartAlreadyBuilt;
//	int					mValidTeams;
//	float				mTimeLastContributed;
//};

class AvHSeeThrough : public CBaseEntity
{
public:
					AvHSeeThrough();
	
	virtual void	KeyValue( KeyValueData* pkvd );
	
	virtual void	Spawn();
	
private:
	int				mCommanderAlpha;
	int				mPlayerAlpha;
};

class AvHSeeThroughDoor : public CBaseDoor
{
public:
					AvHSeeThroughDoor();
	
	virtual void	KeyValue( KeyValueData* pkvd );
	
	virtual void	Spawn();
	
private:
	int				mCommanderAlpha;
	int				mPlayerAlpha;
};


class AvHNoBuild : public AvHBaseEntity
{
public:
					AvHNoBuild();
	
	virtual void	KeyValue( KeyValueData* pkvd );
	
	virtual void	Spawn();
	
private:
};


class AvHMP3Audio : public AvHBaseEntity
{
public:
					AvHMP3Audio();

	int				GetFadeDistance() const;
						
	virtual void	KeyValue( KeyValueData* pkvd );

	virtual void	Precache();
			
	virtual void	Spawn();

	void EXPORT		SpecialSoundUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	static void			ClearSoundNameList();
	static StringList	GetSoundNameList();

private:
	static StringList	sSoundList;

	string			mSoundName;
	int				mSoundVolume;
	int				mFadeDistance;
	bool			mUseState;
	bool			mLooping;
};

class AvHMapInfo : public AvHBaseEntity
{
public:
					AvHMapInfo();

	const AvHMapExtents&	GetMapExtents() const;

	virtual void	KeyValue( KeyValueData* pkvd );
	
	virtual void	Spawn();
	
private:
	AvHMapExtents	mMapExtents;
};

class AvHGameplay : public AvHBaseEntity
{
public:
					AvHGameplay();

	int				GetInitialHives() const;

	int				GetInitialAlienPoints() const;

	int				GetInitialMarinePoints() const;

	int				GetAlienRespawnCost() const;

	int				GetAlienRespawnTime() const;

	int				GetTowerInjectionTime() const;

	int				GetTowerInjectionAmount() const;

	int				GetMarineRespawnCost() const;

	AvHClassType	GetTeamOneType() const;
		
	AvHClassType	GetTeamTwoType() const;

	virtual void	KeyValue( KeyValueData* pkvd );

	virtual void	Reset();

	virtual void	Spawn();
	
private:

	AvHClassType	mTeamOneType;
	AvHClassType	mTeamTwoType;
	
	int				mInitialHives;

	//int				mInitialAlienPoints;
	//int				mInitialMarinePoints;
	
	//int				mAlienRespawnCost;
	//int				mAlienRespawnTime;
	
	//int				mMarineResourceInjectionTime;
	//int				mMarineResourceInjectionAmount;
	
};

class AvHGamma : public AvHBaseEntity
{
public:
					AvHGamma();

	virtual void	KeyValue( KeyValueData* pkvd );

	virtual float	GetGamma() const;

	virtual void	Spawn();
					
private:
	float			mGammaScalar;

};


//class AvHCommandStation : public AvHBaseEntity
//{
//public:
//	void EXPORT			CommandTouch( CBaseEntity *pOther );
//
//	void EXPORT			CommandUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
//
//	virtual int			ObjectCaps(void);
//	
//	virtual void		Precache(void);
//
//	virtual void		ResetEntity();
//
//	virtual void		SetInactive();
//
//	virtual void		Spawn(void);
//
//	virtual int			TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
//	
//
//private:
//	void EXPORT			CommanderUsingThink(void);
//};
//

class TriggerPresence : public CBaseEntity
{
public:
	TriggerPresence();
	
	virtual void	KeyValue(KeyValueData* pkvd);
	
	virtual void	Precache(void);
	
	virtual void	Spawn(void);
	
	void EXPORT		TriggerThink();
	
	void EXPORT		TriggerTouch(CBaseEntity *pOther);
	
	virtual void	Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	
private:
	void			SetPresence(bool inPresence);
	
	// Settings
	string			mMaster;
	string			mTargetEnter;
	string			mTargetLeave;
	string			mMomentaryTarget;
	bool			mPlayersDontActivate;
	bool			mMonstersDontActivate;
	bool			mPushablesDontActivate;
	bool			mTeamOneOnly;
	bool			mTeamTwoOnly;

	float			mTimeBeforeLeave;
	float			mMomentaryOpenTime;
	float			mMomentaryCloseTime;
	
	// Run-time
	bool			mEnabled;
	bool			mPresence;
	float			mTimeOfLastTouch;
	float			mMomentaryValue;
};

class AvHTriggerRandom : public AvHBaseEntity
{
public:
	AvHTriggerRandom();
	
	virtual void		KeyValue(KeyValueData* pkvd);
	
	virtual void		ResetEntity();
	
	virtual void		Spawn(void);
	
	virtual void		Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	
private:
	void				SetNextTrigger();
	
	void EXPORT			TriggerTargetThink(void);
	
	// "template" variables (don't change with each game)
	float				mMaxFireTime;
	float				mMinFireTime;
	int					mWaitBeforeReset;
	
	bool				mStartOn;
	bool				mToggle;
	bool				mRemoveOnFire;
	
	StringList			mTargetList;
	string				mBalancedTarget;
	
	// State variables (change with each game)
	bool				mFiredAtLeastOnce;
	bool				mToggleableAndOn;
	float				mTimeOfLastActivation;
	float				mTimeOfLastTrigger;
};

class AvHTriggerScript : public AvHBaseEntity
{
public:
						AvHTriggerScript();
	
	virtual void		KeyValue(KeyValueData* pkvd);
	
	virtual void		ResetEntity();

	virtual void		Spawn();

	virtual void		Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	
private:
	void				Trigger();

	bool				mStartOn;
	bool				mTriggered;

	string				mScriptName;
};


//class AvHEgg : public CBaseAnimating
//{
//public:
//					AvHEgg();
//
//	virtual void	Hatch();
//					
//	virtual void	Killed( entvars_t *pevAttacker, int iGib );
//
//	virtual void	Precache(void);
//	
//	virtual void	Spawn();
//
//	virtual void	SpawnPlayer();
//
//	virtual int		TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType );
//
//	void EXPORT		EggThink(void);
//	
//private:
//};


class AvHWebStrand : public CBeam
{
public:
					AvHWebStrand();

	void			Break();

	virtual void	Killed(entvars_t *pevAttacker, int iGib);
					
	virtual void	Precache(void);

	void			Setup(const Vector& inPointOne, const Vector& inPointTwo);

	virtual void	Spawn(void);
	
	void EXPORT		StrandTouch( CBaseEntity *pOther );

	void EXPORT		StrandExpire();
	
	virtual int		TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );

private:
};

class AvHFuncResource : public CBaseAnimating
{
public:
					AvHFuncResource();

	bool			GetIsActive() const;

	bool			GetIsOccupied() const;

	int				GetParticleSystemIndex() const;

	void EXPORT		DrawUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);

	//int				ObjectCaps(void);

	virtual void	KeyValue(KeyValueData* pkvd);
	
	virtual void	Precache();
	
	virtual void	ResetEntity();

	virtual void	Spawn();

	virtual void	TriggerOccupy();

	virtual void	TriggerBuild();

	virtual void	TriggerDestroy();

private:
	void			DeleteParticleSystem();
	void			TurnOffParticleSystem();
	void			TurnOnParticleSystem();
	void EXPORT		FuncResourceThink();

	string			mTargetOnBuild;

	string			mTargetOnDestroy;

	bool			mOccupied;
	bool			mActive;

	int				mParticleSystemIndex;

	float			mLastTimeDrawnUpon;
	
};

class AvHFog : public CBaseEntity
{
public:
					AvHFog();

	virtual void	KeyValue( KeyValueData* pkvd );
					
	void EXPORT		FogTouch(CBaseEntity* inEntity);

	void			GetFogColor(int& outRed, int& outGreen, int& outBlue) const;
	float			GetFogEnd() const;
	float			GetFogStart() const;
	
	virtual void	Spawn();
	
private:
	int				mFogColor[3];
	float			mFogStart;
	float			mFogEnd;
	float			mFogExpireTime;
	
};


class AvHResourceTower : public AvHBaseBuildable
{
public:
					AvHResourceTower();

					// For subclassing
					AvHResourceTower(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inUser3 = AVH_USER3_NONE);
	
	void EXPORT		ResourceTowerThink(void);
	
	virtual AvHFuncResource*	GetHostResource() const;

	virtual int		GetIdleAnimation() const;

	virtual bool	GetIsActive() const;
	
	virtual int		GetResourceEntityIndex() const;

	virtual int		GetSequenceForBoundingBox() const;
	
	virtual int		GetPointValue() const;
		
	virtual int		GetTechLevel() const;

	virtual float	GetTimeLastContributed();

	virtual void	SetTimeLastContributed(float inTime);
	
	virtual void	Killed(entvars_t *pevAttacker, int iGib);
	
	virtual void	Precache(void);

	virtual void	SetActivateTime(int inTime);

	virtual void	SetHasBeenBuilt();
	
	virtual void	Spawn();
	
	virtual void	Upgrade();

	virtual char*	GetClassName() const;
	//virtual char*	GetHarvestSound() const;
	virtual char*	GetDeploySound() const;
	virtual char*	GetModelName() const;

	virtual char*	GetActiveSoundList() const;

private:
	void EXPORT		ActivateThink();
	
	virtual int		GetMaxHitPoints() const;
	
	float			mTimeOfLastSound;
	//float			mTimeOfNextActiveAnim;
	
	int				mTechLevel;
	
	int				mResourceEntityIndex;

	int				mActivateTime;

	float			mTimeLastContributed;

	bool			mScannedForFuncResource;
};


class AvHInfoLocation : public CBaseEntity, public AvHBaseInfoLocation
{
public:
	virtual void	KeyValue(KeyValueData* pkvd);

	virtual void	Spawn();

	virtual void	UpdateOnRemove(void);

};

#endif