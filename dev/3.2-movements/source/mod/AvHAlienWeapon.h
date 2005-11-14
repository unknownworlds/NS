//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAlienWeapon.h $
// $Date: 2002/09/23 22:08:59 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAlienWeapon.h,v $
// Revision 1.6  2002/09/23 22:08:59  Flayra
// - Updates to allow marine weapons stick around
//
// Revision 1.5  2002/08/16 02:32:09  Flayra
// - Added damage types
// - Swapped umbra and bile bomb
//
// Revision 1.4  2002/06/25 17:29:56  Flayra
// - Better default behavior
//
// Revision 1.3  2002/06/03 16:25:10  Flayra
// - Switch alien weapons quickly, renamed CheckValidAttack()
//
// Revision 1.2  2002/05/28 17:37:14  Flayra
// - Reduced times where alien weapons are dropped and picked up again (but didn't eliminate, they are still being instantiated somewhere)
//
// Revision 1.1  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_ALIEN_WEAPON_H
#define AVH_ALIEN_WEAPON_H

#include "util/nowarnings.h"
#include "dlls/weapons.h"
#include "mod/AvHBasePlayerWeapon.h"
#include "mod/AvHConstants.h"
#include "mod/AvHMessage.h"

class AvHAlienWeapon : public AvHBasePlayerWeapon 
{
public:
	AvHAlienWeapon();

    virtual float   ComputeAttackInterval() const;

	virtual void	DeductCostForShot(void);
	
	// Cannot ever drop alien weapons
	virtual void	Drop(void);

	bool			GetAllowedForUser3(AvHUser3 inUser3);

	virtual int		GetDamageType() const;
		
	virtual float	GetDeployTime() const;
		
	virtual float	GetEnergyForAttack() const;

	virtual bool	GetFiresUnderwater() const;

	virtual int		GetGroundLifetime() const;

	virtual bool	GetIsDroppable() const;

    virtual bool    GetIsGunPositionValid() const;

	virtual BOOL	IsUseable(void);

	virtual void	Precache(void);

    virtual bool	ProcessValidAttack(void);

	virtual void	Spawn(void);
	
	virtual bool	UsesAmmo(void) const;

	virtual BOOL	UseDecrement(void);
	virtual BOOL	GetTakesEnergy() { return true; }

private:
	float&			GetEnergyLevel();

};

#endif