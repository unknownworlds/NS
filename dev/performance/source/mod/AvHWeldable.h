//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHWeldable.h $
// $Date: 2002/10/03 18:49:43 $
//
//-------------------------------------------------------------------------------
// $Log: AvHWeldable.h,v $
// Revision 1.2  2002/10/03 18:49:43  Flayra
// - Changes for welding order completion
//
// Revision 1.1  2002/05/23 02:32:40  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_WELDABLE_H
#define AVH_WELDABLE_H

#include "mod/AvHEntities.h"

class AvHWeldable : public AvHBaseEntity
{
public:
					AvHWeldable();
	
	void			AddBuildTime(float inTime);

	virtual void	AddChecksum(Checksum& inChecksum);
	
	void			EndTrace(void);

	bool			GetCanBeWelded() const;

	bool			GetIsWelded() const;
		
	virtual float	GetNormalizedBuildPercentage() const;
	
	bool			GetWeldOpens() const;

	virtual void	KeyValue( KeyValueData* pkvd );
	
	virtual void	Killed( entvars_t *pevAttacker, int iGib );

	virtual void	NotifyUpgrade(AvHUpgradeMask inUpgradeMask);
	
	virtual void	Precache(void);

	virtual void	ResetEntity(void);
	
	virtual void	Spawn();
	
	void			StartTrace(void);
	//virtual int		TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType );

	//void EXPORT		WeldableThink(void);

	void EXPORT		WeldableTouch(CBaseEntity *pOther);

	void EXPORT		WeldableUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

private:
	void			Init();
	void			SetHealth();
	void			SetPEVFlagsFromState();
	void			TriggerBroken();
	void			TriggerFinished();
	void			TriggerUse();
	void			UpdateEntityState();
	
	bool			mWelded;
	bool			mWeldOpens;
	bool			mUseState;
	bool			mDestroyed;
	
	Materials		mMaterial;
	string			mTargetOnBreak;
	string			mTargetOnFinish;
	string			mTargetOnUse;
	float			mTimeBuilt;
	float			mBuildTime;
	float			mMaxHealth;
	float			mNonUpgradedMaxHealth;
	float			mThinkInterval;
	float			mTimeLastPlayerTouch;
};

#endif