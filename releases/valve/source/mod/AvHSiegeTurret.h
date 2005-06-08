//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: The marine siege cannon
//
// $Workfile: AvHSiegeTurret.h$
// $Date: 2002/11/22 21:26:06 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSiegeTurret.h,v $
// Revision 1.7  2002/11/22 21:26:06  Flayra
// - Fixed turret factory abuse, where turrets became active after recycling the nearby turret factory before turret was fully contructed.
// - Fixed bug where siege turrets became re-activated after building a regular turret factory nearby.
// - mp_consistency changes
//
// Revision 1.6  2002/10/16 01:07:36  Flayra
// - Removed unused sounds
//
// Revision 1.5  2002/09/23 22:32:14  Flayra
// - Removed minimum range for siege
//
// Revision 1.4  2002/08/16 02:48:10  Flayra
// - New damage model
//
// Revision 1.3  2002/07/23 17:27:47  Flayra
// - Siege no longer requires LOS (so it's actual siege)
//
// Revision 1.2  2002/07/01 21:47:27  Flayra
// - Added siege shockwave effect, added view shaking effects
//
// Revision 1.1  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_SIEGE_TURRET_H
#define AVH_SIEGE_TURRET_H

#include "mod/AvHMarineTurret.h"

class AvHSiegeTurret : public AvHMarineTurret
{
public:
							AvHSiegeTurret();

	virtual void			CheckEnabledState();
	virtual char*			GetDeploySound() const;
	virtual char*			GetModelName() const;
	virtual char*			GetPingSound() const;
	virtual bool			GetRequiresLOS() const;

	virtual int				GetDamageType() const;
		
	virtual bool			GetIsValidTarget(CBaseEntity* inEntity) const;
	virtual int				GetPointValue(void) const;
	
	//virtual int				GetMinimumRange() const;
	virtual int				GetXYRange() const;
		
	virtual void			Precache(void);

	virtual void			ResetEntity();
	virtual void			Shoot(const Vector &inOrigin, const Vector &inDirToEnemy, const Vector& inVecEnemyVelocity);
	virtual void			Spawn();
		
private:
	float					mTimeLastFired;
	int						mShockwaveTexture;
};

#endif