//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Marine sentry turret.
//
// $Workfile: AvHMarineTurret.h$
// $Date: 2002/11/22 21:25:27 $
//
//-------------------------------------------------------------------------------
// $Log: AvHMarineTurret.h,v $
// Revision 1.6  2002/11/22 21:25:27  Flayra
// - Fixed turret factory abuse, where turrets became active after recycling the nearby turret factory before turret was fully contructed.
// - Fixed bug where siege turrets became re-activated after building a regular turret factory nearby.
// - mp_consistency changes
//
// Revision 1.5  2002/11/06 01:39:21  Flayra
// - Turrets now need an active turret factory to be active
//
// Revision 1.4  2002/10/16 01:01:30  Flayra
// - Removed unneeded sounds
//
// Revision 1.3  2002/09/23 22:22:30  Flayra
// - Marine turrets send "sentry firing" and "sentry taking damage" alerts
//
// Revision 1.2  2002/07/23 17:13:18  Flayra
// - Always draw muzzle flash, calculate range in 2D
//
// Revision 1.1  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_MARINE_TURRET_H
#define AVH_MARINE_TURRET_H

#include "mod/AvHTurret.h"

class AvHMarineTurret : public AvHTurret
{
public:
					AvHMarineTurret();
					AvHMarineTurret(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inUser3);

	virtual void	CheckEnabledState();
    virtual int     GetSetEnabledAnimation() const;
					
	virtual char*	GetDeploySound() const;
	char*			GetModelName() const;
	virtual char*	GetPingSound() const;
	
	virtual int		GetRecycleAnimation() const;
	virtual int		GetXYRange() const;
	
	virtual void	Precache(void);
	virtual void	SetEnabledState(bool inState, bool inForce = false);
	virtual void	Shoot(const Vector &inOrigin, const Vector &inDirToEnemy, const Vector& inVecEnemyVelocity);
	virtual void	Spawn();
	virtual int		TakeDamage(entvars_t* inInflictor, entvars_t* inAttacker, float inDamage, int inBitsDamageType);

};

#endif