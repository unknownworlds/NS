//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHHive.h $
// $Date: 2002/10/24 21:26:44 $
//
//-------------------------------------------------------------------------------
// $Log: AvHHive.h,v $
// Revision 1.10  2002/10/24 21:26:44  Flayra
// - Fixed hive wound animation when dying
// - Hives now choose a random spawn point instead of the first
//
// Revision 1.9  2002/10/03 18:50:34  Flayra
// - Trigger "hive complete" alert
// - Trigger "hive is dying" alert
//
// Revision 1.8  2002/09/09 19:52:57  Flayra
// - Animations play properly
// - Hive can be hit once it starts gestating (hive becomes solid when gestating, not when complete)
// - Respawn fixes
//
// Revision 1.7  2002/08/16 02:36:01  Flayra
// - New damage system
// - Fixed bug where hive was absorbing too much damage in armor
//
// Revision 1.6  2002/07/23 17:06:09  Flayra
// - Added ability for aliens to donate their resources at the hive, bind technology to a hive (so builders can choose the route), fixed bug where death animation played repeatedly
//
// Revision 1.5  2002/07/08 17:03:04  Flayra
// - Refactored reinforcements
//
// Revision 1.4  2002/07/01 21:33:48  Flayra
// - Hives can no longer be "used" to speed construction, wound sounds play on CHAN_BODY
//
// Revision 1.3  2002/06/03 16:47:49  Flayra
// - Hives are base buildables now (bug with allowing use to speed building), added other hive anims for hurt, death, bad-touch, fixed bug where hives didn't get full health when they were initially built (after being killed once)
//
// Revision 1.2  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_HIVE_H
#define AVH_HIVE_H

#include "util/nowarnings.h"
#include "dlls/extdll.h"
#include "dlls/util.h"
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
#include "mod/AvHReinforceable.h"

#define kHiveHitEvent		"events/HiveHit.sc"

const float kHiveAliveThinkInterval = 1.0f;

//const int kNumWoundSounds	= 2;
//char** kWoundSoundList = 
//{
//	"sounds/misc/hive_wound1.wav",
//	"sounds/misc/hive_wound2.wav",
//}

class AvHHive : public AvHBaseBuildable, public AvHReinforceable
{
public:
					AvHHive();

	virtual bool	CanBecomeActive() const;

	void EXPORT		ConstructUse(CBaseEntity* inActivator, CBaseEntity* inCaller, USE_TYPE inUseType, float inValue);

	void EXPORT		DonateUse(CBaseEntity* inActivator, CBaseEntity* inCaller, USE_TYPE inUseType, float inValue);

	virtual bool	Energize(float inEnergyAmount);

	AvHTeamNumber	GetTeamNumber() const;

	int				GetIdle1Animation() const;

	int				GetIdle2Animation() const;

	virtual int		GetTakeDamageAnimation() const;

	virtual bool	GetIsActive() const;

	virtual bool	GetIsOrganic() const;
	
	virtual bool	GetIsSpawning() const;
	
	virtual int		GetMaxSpawnDistance() const;

	virtual int		GetMoveType() const;

	// From AvHReinforceable
	virtual void	CueRespawnEffect(AvHPlayer* inPlayer);
	
	virtual bool	GetCanReinforce() const;
	
	virtual bool	GetSpawnLocationForPlayer(CBaseEntity* inPlayer, Vector& outLocation) const;

	virtual bool	GetTriggerAlertOnDamage() const;
	
	virtual AvHTeamNumber	GetReinforceTeamNumber() const;
	
	

	//virtual CBaseEntity*	GetRandomSpawnPointForPlayer(CBaseEntity* inPlayer) const;

	virtual float	GetTimeLastContributed();

	virtual int		GetPointValue(void) const;

	virtual float	GetReinforceTime() const;
	
	virtual int		GetSpawnAnimation() const;

	AvHMessageID	GetTechnology() const;

	void			SetTechnology(AvHMessageID inMessageID);
	
	virtual void	SetTimeLastContributed(float inTime);

	virtual void	KeyValue( KeyValueData* pkvd );

	virtual void	Precache(void);

	virtual void	ResetEntity(void);

	void EXPORT		HiveTouch(CBaseEntity* inOther);

	virtual void	SetActive();

	virtual void	SetHasBeenBuilt();

	virtual void	SetInactive();

	virtual bool	StartSpawningForTeam(AvHTeamNumber inTeam, bool inForce = false);
					
	virtual int		TakeDamage(entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType);

    virtual void	TriggerDeathAudioVisuals();
		
	virtual void	Spawn();

	//virtual void	UpdateReinforcements();

protected:
	virtual void	ResetReinforcingPlayer(bool inSuccess);
					
private:
	void			CreateUmbra(vec3_t& inOrigin);
	void			UpdateUmbra();
	void EXPORT		HiveAliveThink(void);
	void			ProcessHealing();
	bool			SetSolid(bool inForce = false);
	AvHMessageID	mTechnology;
	
	int				mMaxSpawnDistance;
	int				mMaxHitPoints;

	bool			mActive;
	bool			mSolid;
	bool			mSpawning;
	float			mTimeLastContributed;
	float			mTimeLastWoundSound;
	float			mTimeOfNextUmbra;
	float			mEnergy;
};

#endif