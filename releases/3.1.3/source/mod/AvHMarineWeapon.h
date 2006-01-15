//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHMarineWeapon.h $
// $Date: 2002/10/03 18:58:15 $
//
//-------------------------------------------------------------------------------
// $Log: AvHMarineWeapon.h,v $
// Revision 1.3  2002/10/03 18:58:15  Flayra
// - Added heavy view models
//
// Revision 1.2  2002/06/25 17:50:59  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.1  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_MARINE_WEAPON_H
#define AVH_MARINE_WEAPON_H

#include "mod/AvHBasePlayerWeapon.h"

class AvHMarineWeapon : public AvHBasePlayerWeapon
{
public:
    virtual float	ComputeAttackInterval() const;

    virtual char*	GetActiveViewModel() const;

	bool			GetAllowedForUser3(AvHUser3 inUser3);
	
	virtual float	GetDeploySoundVolume() const;

	virtual char*	GetHeavyViewModel() const;

	virtual void    Precache();
	
};

class AvHReloadableMarineWeapon : public AvHMarineWeapon
{
public:
    virtual void	DeductCostForShot(void);

    virtual int		DefaultReload( int iClipSize, int iAnim, float fDelay );

    virtual int     GetGotoReloadAnimation() const = 0;
    
    virtual float   GetGotoReloadAnimationTime() const = 0;

    virtual int		GetShellReloadAnimation() const = 0;

    virtual float   GetShellReloadAnimationTime() const = 0;

    virtual int     GetEndReloadAnimation() const = 0;

    virtual float   GetEndReloadAnimationTime() const = 0;

    virtual void    Holster( int skiplocal);

    virtual void	Reload(void);
    
    virtual void	WeaponIdle(void);

protected:
    virtual void	Init();

private:
    int				mSpecialReload;
    float			mNextReload;
};

#endif