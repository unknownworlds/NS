//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHMarineWeapons.h $
// $Date: 2002/10/24 21:33:52 $
//
//-------------------------------------------------------------------------------
// $Log: AvHMarineWeapons.h,v $
// Revision 1.30  2002/10/24 21:33:52  Flayra
// - Shotgun reloading fixes
//
// Revision 1.29  2002/10/16 20:55:04  Flayra
// - Mine fixes
//
// Revision 1.28  2002/10/16 01:01:51  Flayra
// - Fixed mines being resupplied from armory
//
// Revision 1.27  2002/10/03 18:58:51  Flayra
// - Added heavy view models
//
// Revision 1.26  2002/09/23 22:22:52  Flayra
// - HMG firing restrictions
//
// Revision 1.25  2002/08/16 02:44:11  Flayra
// - New damage types
//
// Revision 1.24  2002/08/09 01:08:16  Flayra
// - Support for new shotgun reload
// - Regular update
//
// Revision 1.23  2002/07/26 23:06:09  Flayra
// - New welder artwork
//
// Revision 1.22  2002/07/08 17:10:47  Flayra
// - Bullet spread, new HMG artwork
//
// Revision 1.21  2002/07/01 21:38:09  Flayra
// - Tweaks for new artwork
//
// Revision 1.20  2002/06/25 18:07:25  Flayra
// - Refactoring, cleanup
//
// Revision 1.19  2002/06/10 19:59:04  Flayra
// - Updated with new knife artwork
//
// Revision 1.18  2002/06/03 16:52:00  Flayra
// - Constants and tweaks to make weapon anims and times correct with new artwork, added different deploy times (this should be refactored a bit more)
//
// Revision 1.17  2002/05/28 17:54:03  Flayra
// - Started to add special animations for shotgun
//
// Revision 1.16  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVHMARINEWEAPONS_H
#define AVHMARINEWEAPONS_H

#include "util/nowarnings.h"
#include "dlls/weapons.h"
#include "mod/AvHBasePlayerWeapon.h"
#include "mod/AvHConstants.h"
#include "mod/AvHMarineWeaponConstants.h"
#include "mod/AvHMarineWeapon.h"

//extern "C"
//{
//	void EV_MachineGun( struct event_args_s *args  );
//	void EV_SonicGun( struct event_args_s *args  );
//	void EV_HeavyMachineGun( struct event_args_s *args  );
//	void EV_GrenadeGun( struct event_args_s *args  );
//	void EV_NukeGun( struct event_args_s *args  );
//	void EV_Jetpack( struct event_args_s *args  );
//}

class AvHKnife : public AvHMarineWeapon 
{
public:
					AvHKnife()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;

	virtual int		GetDeployAnimation() const;

	virtual char*	GetDeploySound() const;

	virtual float	GetDeployTime() const;

	virtual bool	GetFiresUnderwater() const;

	virtual char*	GetHeavyViewModel() const;
	
	virtual int		GetIdleAnimation() const;

	virtual bool	GetIsDroppable() const;

	int				GetItemInfo(ItemInfo *p) const;

	bool			GetMustPressTriggerForEachShot() const;
		
	int				GetShootAnimation() const;

	virtual char*	GetPlayerModel() const;

	virtual char*	GetViewModel() const;

	virtual char*	GetWorldModel() const;
	
	virtual int		iItemSlot(void);

	virtual BOOL	IsUseable(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();

	virtual bool	UsesAmmo(void) const;
	
	virtual BOOL	UseDecrement(void);
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};

//#ifdef AVH_SERVER
//class AvHGrenadeProjectile : public CGrenade
//{
//public:
//	virtual void	Precache(void);
//	
//	virtual void	Spawn(void);
//	
//	void EXPORT		GrenThink();
//
//private:
//	float			mTimeCreated;
//};
//#endif


class AvHMachineGun : public AvHMarineWeapon 
{ 
public: 
				    AvHMachineGun()
				        { this->Init(); } 

	virtual int		GetBarrelLength() const;

	virtual int		GetDamageType() const;

	virtual char*	GetDeploySound() const;

	virtual float	GetDeployTime() const;

	virtual bool	GetHasMuzzleFlash() const;
	
	virtual char*	GetHeavyViewModel() const;

	int				GetItemInfo(ItemInfo *p) const;

	virtual char*	GetPlayerModel() const;

	virtual Vector	GetProjectileSpread() const;

	virtual char*	GetViewModel() const;

	virtual char*	GetWorldModel() const;

	float			GetReloadTime(void) const;

	virtual int		iItemSlot(void);

	virtual void	Precache(void);

	virtual void	Spawn();

	virtual float	GetRateOfFire() const;

protected: 
    virtual void	Init();
};

class AvHPistol : public AvHMarineWeapon 
{ 
public: 
					AvHPistol()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;

    virtual float   GetRateOfFire() const;

	virtual int		GetDeployAnimation() const;

	virtual char*	GetDeploySound() const;

	virtual float	GetDeployTime() const;

	virtual int		GetEmptyShootAnimation() const;
	
	virtual bool	GetHasMuzzleFlash() const;
	
	virtual char*	GetHeavyViewModel() const;

	int				GetItemInfo(ItemInfo *p) const;

	bool			GetMustPressTriggerForEachShot() const;

	virtual char*	GetPlayerModel() const;

	virtual Vector	GetProjectileSpread() const;
	
	virtual char*	GetViewModel() const;

	virtual char*	GetWorldModel() const;

	virtual int		GetReloadAnimation() const;

	virtual int		GetShootAnimation() const;
		
	virtual int		iItemSlot(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
protected: 
    virtual void	Init();

	virtual float	GetReloadTime(void) const;

};

class AvHSonicGun : public AvHReloadableMarineWeapon 
{ 
public: 
				    AvHSonicGun() 
				        { this->Init(); } 

	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;

	virtual int		GetDamageType() const;
	
	virtual int		GetDeployAnimation() const;

	virtual char*	GetDeploySound() const;

	virtual float	GetDeployTime() const;

	virtual int		GetEmptyShootAnimation() const;

	virtual bool	GetHasMuzzleFlash() const;

	virtual char*	GetHeavyViewModel() const;
	
	int				GetIdleAnimation() const;
		
	int				GetItemInfo(ItemInfo *p) const;
	
	virtual Vector	GetProjectileSpread() const;
	
	virtual char*	GetPlayerModel() const;

	virtual char*	GetViewModel() const;

	virtual char*	GetWorldModel() const;

	virtual int		GetReloadAnimation() const;

	virtual int		GetShootAnimation() const;

	virtual int		iItemSlot(void);

	virtual void	Precache(void);

	virtual void	Spawn();

    // pure functions from AvHReloadableMarineWeapon
    virtual int     GetGotoReloadAnimation() const;
    
    virtual float   GetGotoReloadAnimationTime() const;
    
    virtual int		GetShellReloadAnimation() const;
    
    virtual float   GetShellReloadAnimationTime() const;
    
    virtual int     GetEndReloadAnimation() const;
    
    virtual float   GetEndReloadAnimationTime() const;

protected: 
	virtual void	FireProjectiles(void);
	
	virtual float	GetReloadTime(void) const;

    virtual void	Init();
};


class AvHHeavyMachineGun : public AvHMarineWeapon 
{ 
public: 
					AvHHeavyMachineGun()
						{ this->Init(); } 

	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;

	virtual int		GetDamageType() const;

	virtual int		GetDeployAnimation() const;

	virtual char*	GetDeploySound() const;

	virtual float	GetDeployTime() const;

	virtual int		GetEmptyShootAnimation() const;

	virtual bool	GetHasMuzzleFlash() const;

	virtual char*	GetHeavyViewModel() const;
	
	virtual int		GetIdleAnimation() const;
		
	int				GetItemInfo(ItemInfo *p) const;

	virtual char*	GetPlayerModel() const;

	virtual Vector	GetProjectileSpread() const;
	
	virtual int		GetReloadAnimation() const;

	virtual int		GetShootAnimation() const;
	
	virtual char*	GetViewModel() const;

	virtual char*	GetWorldModel() const;

	virtual int		iItemSlot(void);

	virtual void	Precache(void);

	virtual void	Spawn();

protected: 
	virtual float	GetReloadTime(void) const;

    virtual void	FireProjectiles(void);
	
    virtual void	Init();

    virtual bool    ProcessValidAttack(void);
};


class AvHGrenadeGun : public AvHMarineWeapon 
{ 
public: 
				    AvHGrenadeGun()
						{ this->Init(); } 

	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;

	virtual int		GetDeployAnimation() const;

	virtual char*	GetDeploySound() const;

	virtual int		GetEmptyShootAnimation() const;

	virtual void	GetEventOrigin(Vector& outOrigin) const;

	virtual void	GetEventAngles(Vector& outAngles) const;

	virtual bool	GetHasMuzzleFlash() const;
	
	virtual char*	GetHeavyViewModel() const;
	
	virtual int		GetIdleAnimation() const;

	int				GetItemInfo(ItemInfo *p) const;
	
	virtual char*	GetPlayerModel() const;

	virtual int		GetReloadAnimation() const;

	virtual int		GetShootAnimation() const;

	virtual char*	GetViewModel() const;

	virtual char*	GetWorldModel() const;

	virtual int		iItemSlot(void);

	virtual void	Precache(void);

	virtual void	Spawn();

protected: 
    virtual void	FireProjectiles(void);
	
	virtual float	GetReloadTime(void) const;

    virtual void	Init();
};

class AvHWelder : public AvHMarineWeapon
{
public:
					AvHWelder()
						{ this->Init(); } 

	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;

	int				GetItemInfo(ItemInfo *p) const;
	
	virtual void	Holster( int skiplocal = 0 );

	virtual int		GetDeployAnimation() const;

	virtual float	GetDeployTime() const;

	virtual char*	GetHeavyViewModel() const;
	
	virtual char*	GetPlayerModel() const;

	virtual int		GetShootAnimation() const;
		
	virtual char*	GetViewModel() const;

	virtual char*	GetWorldModel() const;

	virtual int		iItemSlot(void);
	
	virtual BOOL	IsUseable(void);

	virtual void	Precache(void);
	
	virtual void	Spawn();

	virtual bool	UsesAmmo(void) const;

	virtual void    WeaponIdle();

	bool			GetIsWelding()						{ return mIsWelding; }
	void			SetIsWelding(bool bWelding)			{ mIsWelding = bWelding;} 
	
protected: 
	bool			mIsWelding;
    virtual void	Init();

    virtual void    FireProjectiles(void);

	#ifdef AVH_SERVER
	bool			RepairTarget(CBaseEntity* inEntity, float inROF);
	#endif
	
	void EXPORT		WelderThink();

};

class AvHMine : public AvHMarineWeapon
{
public:
					AvHMine()
						{ this->Init(); } 
	
    virtual void    DeductCostForShot(void);

	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;

    virtual bool    GetCanBeResupplied() const;
    
	virtual int		GetDeployAnimation() const;

	virtual bool	GetFiresUnderwater() const;
	
	virtual char*	GetHeavyViewModel() const;
	
	int				GetItemInfo(ItemInfo *p) const;

	virtual char*	GetPlayerModel() const;

	virtual char*	GetWorldModel() const;
	
	virtual char*	GetViewModel() const;

	virtual int		GetShootAnimation() const;

	virtual void	Holster(int skiplocal = 0);
	
	virtual int		iItemSlot(void);

	virtual BOOL	PlayEmptySound(void);
		
	virtual void	Precache(void);

    virtual bool	Resupply();
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;
	
	virtual BOOL	UseDecrement(void);
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();

	virtual bool    ProcessValidAttack(void);

#ifdef AVH_SERVER
	bool			GetDropLocation(Vector& outLocation, Vector* outAngles = NULL) const;
#endif
};

class AvHGrenade : public AvHMarineWeapon
{
public:
                    AvHGrenade()
                        { this->Init(); } 

    virtual BOOL    Deploy();
    
    virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;
 
    virtual bool    GetCanBeResupplied() const;
    
    virtual int		GetDeployAnimation() const;
    
    virtual char*	GetDeploySound() const;
    
    virtual float	GetDeployTime() const;
    
    virtual int		GetEmptyShootAnimation() const;

    virtual bool	GetFiresUnderwater() const;
    
    virtual char*	GetHeavyViewModel() const;
    
    virtual int		GetIdleAnimation() const;
    
    virtual bool	GetIsDroppable() const;

    virtual BOOL    GetIsWeaponPrimed() const;

    virtual BOOL    GetIsWeaponPriming() const;
    
    int				GetItemInfo(ItemInfo *p) const;
    
    bool			GetMustPressTriggerForEachShot() const;
    
    int				GetShootAnimation() const;
    
    virtual char*	GetPlayerModel() const;

    virtual int     GetPrimeAnimation() const;

    virtual char*   GetPrimeSound() const;
    
    virtual char*	GetViewModel() const;

    virtual float   GetWeaponPrimeTime() const;

    virtual char*	GetWorldModel() const;

    virtual void	Holster(int skiplocal = 0);
    
    virtual int		iItemSlot(void);
    
    virtual BOOL	IsUseable(void);
    
	virtual void	ItemPostFrame(void);

    virtual BOOL	PlayEmptySound(void);

    virtual void	Precache(void);

    virtual bool	Resupply();

    virtual void	Spawn();
    
    virtual bool	UsesAmmo(void) const;
    
    virtual BOOL	UseDecrement(void);

    virtual void    WeaponIdle();

    virtual void    PrimaryAttack(void);
    
protected: 

			void	CreateProjectile(void);

    virtual void    FireProjectiles(void);
    
    virtual void	Init();

    virtual void	SetNextIdle(void);

			bool	ShouldRollGrenade(void) const;

};


#endif


