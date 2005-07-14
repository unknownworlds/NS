//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAlienAbilities.h $
// $Date: 2002/09/23 22:06:33 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAlienAbilities.h,v $
// Revision 1.9  2002/09/23 22:06:33  Flayra
// - Updated anims for new view model artwork
//
// Revision 1.8  2002/08/09 00:52:57  Flayra
// - Speed up deploying of charge and leap, removed old hard-coded number
//
// Revision 1.7  2002/07/26 23:03:08  Flayra
// - New artwork
//
// Revision 1.6  2002/07/08 16:42:38  Flayra
// - Refactoring for cheat protection, moved blinking in here from separate class
//
// Revision 1.5  2002/06/25 17:25:56  Flayra
// - Regular update for leap and charge
//
// Revision 1.4  2002/06/03 16:20:11  Flayra
// - Proper anims for alien abilities
//
// Revision 1.3  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVHALIENABILITIES_H
#define AVHALIENABILITIES_H

#include "util/nowarnings.h"
#include "dlls/weapons.h"
#include "mod/AvHAlienWeapons.h" 

class AvHAlienAbilityWeapon : public AvHAlienWeapon
{
public:
	virtual AvHMessageID		GetAbilityImpulse() const = 0;
	int							mAbilityEvent;
};

class AvHLeap : public AvHAlienAbilityWeapon 
{ 
public: 
				    AvHLeap()
				        { this->Init(); } 

    virtual BOOL    Deploy();

	virtual int		GetBarrelLength() const;

	virtual int		GetDeployAnimation() const;

	virtual float	GetDeployTime() const;
		
	virtual bool	GetFiresUnderwater() const;

	virtual int		GetIdleAnimation() const;
	
	AvHMessageID	GetAbilityImpulse() const;
	
	virtual bool	GetIsDroppable() const;

	int				GetItemInfo(ItemInfo *p) const;

	virtual int		GetShootAnimation() const;

	virtual int		iItemSlot(void);

	virtual void	Precache(void);

	virtual void	Spawn();

	virtual bool	UsesAmmo(void) const;
	virtual BOOL	GetTakesEnergy() { return FALSE; }

protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};

class AvHBlinkGun : public AvHAlienAbilityWeapon
{ 
public: 
	AvHBlinkGun()
	{ this->Init(); } 
	
    virtual BOOL    Deploy();
	
	AvHMessageID	GetAbilityImpulse() const;
	
	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;
	
	virtual int		GetDeployAnimation() const;
	
	virtual bool	GetFiresUnderwater() const;

	virtual int		GetIdleAnimation() const;
	
	virtual bool	GetIsDroppable() const;
	
	int				GetItemInfo(ItemInfo *p) const;
	
	virtual bool	GetMustPressTriggerForEachShot() const;
	
	virtual int		GetShootAnimation() const;
	
	virtual char*	GetViewModel() const;
	
	virtual int		iItemSlot(void);
	
	virtual void	ItemPostFrame(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;
	virtual BOOL	GetTakesEnergy() { return FALSE; }
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
	
	float			mTimeOfNextBlinkEvent;
	
	int				mBlinkSuccessEvent;
};

class AvHCharge : public AvHAlienAbilityWeapon 
{ 
public: 
					AvHCharge()
						{ this->Init(); } 
	
    virtual BOOL    Deploy();
	
	AvHMessageID	GetAbilityImpulse() const;
	
	virtual int		GetBarrelLength() const;
	
	virtual int		GetDeployAnimation() const;

	virtual float	GetDeployTime() const;
		
	virtual bool	GetFiresUnderwater() const;
	
	virtual int		GetIdleAnimation() const;
		
	virtual bool	GetIsDroppable() const;
	
	int				GetItemInfo(ItemInfo *p) const;
	
	virtual int		GetShootAnimation() const;
	
	virtual int		iItemSlot(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;
    	
    virtual float	GetRateOfFire() const;
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};



#endif