//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Base NS turret, used for all types of automated enemy detection
//
// $Workfile: AvHTurret.h$
// $Date: 2002/11/22 21:24:59 $
//
//-------------------------------------------------------------------------------
// $Log: AvHTurret.h,v $
// Revision 1.8  2002/11/22 21:24:59  Flayra
// - Fixed turret factory abuse, where turrets became active after recycling the nearby turret factory before turret was fully contructed.
// - Fixed bug where siege turrets became re-activated after building a regular turret factory nearby.
//
// Revision 1.7  2002/11/06 01:39:21  Flayra
// - Turrets now need an active turret factory to be active
//
// Revision 1.6  2002/08/16 02:49:00  Flayra
// - New damage model
//
// Revision 1.5  2002/07/23 17:34:07  Flayra
// - Turrets track range in 2D, turrets can not require LOS if desired (for siege)
//
// Revision 1.4  2002/07/01 21:23:01  Flayra
// - Added generic vertical FOV to allow alien turrets to shoot very high and low
//
// Revision 1.3  2002/06/03 17:02:22  Flayra
// - Experimented with pushable/buildable siege turrets
//
// Revision 1.2  2002/05/28 18:11:59  Flayra
// - Put in slower, randomish rate of fire for turrets for sound variance and drama, don't play ping if no ping sound specified (crashing with offensive tower)
//
// Revision 1.1  2002/05/23 02:32:57  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_TURRET_H
#define AVH_TURRET_H

#include "mod/AvHBaseBuildable.h"
#include "util/Quat.h"

//#include "mod/AvHPushableBuildable.h"
// This class behaves like CSentry does, but clean and extensible, and with a base class of AvHBaseBuildable
//class AvHTurret : public AvHPushableBuildable//public AvHBaseBuildable
class AvHTurret : public AvHBaseBuildable
{
public:
					AvHTurret(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inUser3);

    virtual void	CheckEnabledState();
    
	virtual char*	GetActiveSound() const;
	virtual char*	GetAlertSound() const;
	virtual char*	GetPingSound() const;

	virtual int		GetDamageType() const;

	virtual int		GetActiveAnimation() const;
	virtual int		GetDeployAnimation() const;
	virtual bool	GetEnabledState() const;
	virtual int		GetIdle1Animation() const;
	virtual int		GetIdle2Animation() const;
	virtual int		GetKilledAnimation() const;
	virtual bool	GetRequiresLOS() const;
	virtual int		GetPointValue(void) const;
    virtual int     GetSetEnabledAnimation() const;
	virtual int		GetSpawnAnimation() const;
	virtual int		GetTakeDamageAnimation() const;
	virtual int		GetVerticalFOV() const;

	virtual bool	GetIsValidTarget(CBaseEntity* inEntity) const;

	CBaseEntity*	FindBestEnemy();
	virtual int		IRelationship(CBaseEntity* inTarget);
	
	virtual void	SetNextAttack();
	
	// Must override these
	virtual int		GetXYRange() const = 0;
	virtual void	Shoot(const Vector &vecSrc, const Vector &vecDirToEnemy, const Vector& inVecEnemyVelocity) = 0;
    virtual void	Ping(void);
	virtual int		MoveTurret(void);
	virtual bool	GetBaseClassAnimatesTurret() const;

	virtual void	ResetEntity();
	virtual void	SetHasBeenBuilt();
	virtual void	SetEnabledState(bool inState, bool inForce = false);
	virtual void	Spawn();

protected:
	virtual float	GetRateOfFire() const;
	virtual void	UpdateEnemy();
	void			TurretUpdate();
	void			Init();
	void			Setup();

	BOOL			FInViewCone(CBaseEntity* inEntity);
	BOOL			FInViewCone(Vector* inOrigin);
		
	EXPORT void		ActiveThink(void);
	EXPORT void		SearchThink(void);

	EHANDLE			m_hEnemy;
	float			m_flFieldOfView;
	float			mTimeOfLastAttack;
	float			mTimeOfNextAttack;
    float           mTimeOfLastUpdateEnemy;

	// turn rate in radians
	float			m_fTurnRate;		

	// movement
	Quat			mCurQuat;
	Quat			mGoalQuat;
	
	float			mNextPingTime;

	bool			mEnabled;

};

#endif