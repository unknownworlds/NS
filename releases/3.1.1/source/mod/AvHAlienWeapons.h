//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAlienWeapons.h$
// $Date: 2002/10/16 00:48:10 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAlienWeapons.h,v $
// Revision 1.17  2002/10/16 00:48:10  Flayra
// - Removed unneeded sprite
//
// Revision 1.16  2002/08/16 02:32:09  Flayra
// - Added damage types
// - Swapped umbra and bile bomb
//
// Revision 1.15  2002/07/26 23:03:08  Flayra
// - New artwork
//
// Revision 1.14  2002/07/23 16:58:09  Flayra
// - Updates for new viewmodel artwork
//
// Revision 1.13  2002/07/08 16:45:15  Flayra
// - Refactoring for cheat protection, moved blinking in here from separate class
//
// Revision 1.12  2002/07/01 21:15:27  Flayra
// - Added new alien abilities (primal scream, babblers, bilebomb, umbra)
//
// Revision 1.11  2002/06/25 17:29:50  Flayra
// - Better default behavior, removed building weapons
//
// Revision 1.10  2002/06/10 19:50:01  Flayra
// - Updated with new alien view model artwork (with running anims)
//
// Revision 1.9  2002/06/03 16:27:05  Flayra
// - Animation constants and changes with new artwork
//
// Revision 1.8  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVHALIENWEAPONS_H
#define AVHALIENWEAPONS_H

#include "mod/AvHAlienWeapon.h"


#ifdef AVH_SERVER
class AvHSpit : public CBaseEntity
{
public:
	virtual void	Precache(void);

	virtual void	Spawn(void);

	void EXPORT		SpitTouch(CBaseEntity *pOther);

	void EXPORT		SpitDeath();

	void			SetDamage(float inDamage);

private:
	float			mDamage;
};
#endif

class AvHSpitGun : public AvHAlienWeapon 
{ 
public: 
				    AvHSpitGun()
				        { this->Init(); } 

	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;

	virtual int		GetDamageType() const;
	
	virtual int		GetDeployAnimation() const;

	virtual bool	GetFiresUnderwater() const;
	
	virtual bool	GetIsDroppable() const;

	int				GetItemInfo(ItemInfo *p) const;

	virtual int		GetShootAnimation() const;

	virtual char*	GetViewModel() const;

	virtual int		iItemSlot(void);

	virtual void	Precache(void);

	virtual void	Spawn();

	virtual bool	UsesAmmo(void) const;

protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};


class AvHClaws : public AvHAlienWeapon 
{ 
public: 
					AvHClaws()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;

    virtual bool    GetIsGunPositionValid() const;

    virtual float	GetRateOfFire() const;

	virtual int		GetDamageType() const;
	
	virtual int		GetDeployAnimation() const;
	
	virtual float	GetDeployTime() const;
		
	virtual bool	GetFiresUnderwater() const;

	virtual int		GetIdleAnimation() const;
		
	virtual bool	GetIsDroppable() const;
	
	int				GetItemInfo(ItemInfo *p) const;

	virtual bool	GetMustPressTriggerForEachShot() const
						{ return true;	}
	
	virtual int		iItemSlot(void);

	virtual int		GetShootAnimation() const;
		
	virtual char*	GetViewModel() const;
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;

	virtual BOOL	UseDecrement(void);
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};

class AvHSwipe : public AvHAlienWeapon 
{ 
public: 
					AvHSwipe()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;

    virtual bool    GetIsGunPositionValid() const;

    virtual float	GetRateOfFire() const;

	virtual int		GetDamageType() const;
	
	virtual bool	GetFiresUnderwater() const;
	
	virtual int		GetDeployAnimation() const;

	virtual int		GetIdleAnimation() const;

	virtual int		GetShootAnimation() const;
		
	virtual char*	GetViewModel() const;

	virtual bool	GetIsDroppable() const;
	
	int				GetItemInfo(ItemInfo *p) const;

	virtual bool	GetMustPressTriggerForEachShot() const
						{ return true;	}
	
	virtual int		iItemSlot(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;
	
	virtual BOOL	UseDecrement(void);
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};


#ifdef AVH_SERVER
class AvHSporeProjectile : public CBaseEntity
{
public:
					AvHSporeProjectile();

	virtual void	Precache(void);

	virtual void	SetDamage(float inDamage);
	
	virtual void	Spawn(void);
	
	void EXPORT		SporeCloudThink();

	void EXPORT		SporeTouch(CBaseEntity *pOther);

private:
	float			mTimeHit;
	float			mDamage;

};
#endif

class AvHSpore : public AvHAlienWeapon 
{ 
public: 
					AvHSpore()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;

	virtual int		GetDamageType() const;
	
	virtual int		GetDeployAnimation() const;

	virtual float	GetDeployTime() const;
	
	virtual bool	GetFiresUnderwater() const;
	
	virtual int		GetIdleAnimation() const;

	virtual bool	GetIsDroppable() const;

	int				GetItemInfo(ItemInfo *p) const;
	
	virtual int		GetShootAnimation() const;

	virtual char*	GetViewModel() const;

	virtual int		iItemSlot(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};

class AvHBite : public AvHAlienWeapon 
{ 
public: 
					AvHBite()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;
    
    virtual bool    GetIsGunPositionValid() const;
    
    virtual float	GetRateOfFire() const;

	virtual char*	GetBiteSound() const;

	virtual int		GetDeployAnimation() const;

	virtual bool	GetFiresUnderwater() const;

	virtual int		GetIdleAnimation() const;

	virtual int		GetShootAnimation() const;
	
	virtual bool	GetIsDroppable() const;
	
	int				GetItemInfo(ItemInfo *p) const;

	virtual bool	GetMustPressTriggerForEachShot() const
						{ return true;	}

	virtual char*	GetViewModel() const;
	
	virtual int		iItemSlot(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;

	virtual BOOL	UseDecrement(void);
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};

class AvHBite2 : public AvHBite 
{
public:
                    AvHBite2()
                        { this->Init(); } 

	virtual int		GetBarrelLength() const;

    virtual bool    GetIsGunPositionValid() const;

    virtual float	GetRateOfFire() const;

	virtual int		GetDamageType() const;
		
	virtual int		GetDeployAnimation() const;

	virtual float	GetDeployTime() const;

	virtual int		GetIdleAnimation() const;

	int				GetItemInfo(ItemInfo *p) const;

	virtual int		GetShootAnimation() const;

	virtual char*	GetViewModel() const;

	virtual int		iItemSlot(void);

	virtual void	Precache();

	virtual void	Spawn();

protected:
    
	virtual void	Init();

};


class AvHSpikeGun : public AvHAlienWeapon 
{ 
public: 
					AvHSpikeGun()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;

	virtual int		GetDamageType() const;
	
	virtual int		GetDeployAnimation() const;

	virtual float	GetDeployTime() const;

	virtual int		GetIdleAnimation() const;

	virtual bool	GetFiresUnderwater() const;

	virtual bool	GetIsDroppable() const;

	int				GetItemInfo(ItemInfo *p) const;

	virtual int		GetShootAnimation() const;

	virtual char*	GetViewModel() const;
	
	virtual int		iItemSlot(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();

    //virtual void    WeaponIdle();
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();

	//int				mFramesSinceMoreAmmo;
};

class AvHHealingSpray : public AvHAlienWeapon 
{ 
public: 
					AvHHealingSpray()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;

	virtual int		GetDamageType() const;
	
	virtual int		GetDeployAnimation() const;

	virtual int		GetIdleAnimation() const;

	virtual int		GetShootAnimation() const;
	
	int				GetItemInfo(ItemInfo *p) const;

	virtual char*	GetViewModel() const;
	
	virtual int		iItemSlot(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};

class AvHMetabolize : public AvHAlienWeapon 
{ 
public: 
					AvHMetabolize()
						{ this->Init(); } 
	
    virtual void	DeductCostForShot(void);

	virtual BOOL    Deploy();

	virtual int		GetBarrelLength() const;
        
    virtual float	GetRateOfFire() const;
	
	virtual int		GetDeployAnimation() const;
	
	virtual int		GetIdleAnimation() const;

	#ifdef AVH_SERVER
	virtual int		GetResourceCost() const;
	#endif
	
	virtual int		GetShootAnimation() const;
	
	int				GetItemInfo(ItemInfo *p) const;
	
	virtual char*	GetViewModel() const;
	
	virtual int		iItemSlot(void);
	
	virtual void	ItemPostFrame(void);

	virtual void	Precache(void);

	virtual void	Spawn();
	
protected: 
    virtual void    FireProjectiles(void);

    virtual void	Init();

    virtual void    SetAnimationAndSound(void);

	float			mTimeOfLastMetabolizeViewAnim;
};

class AvHWebSpinner : public AvHAlienWeapon 
{ 
public: 
					AvHWebSpinner()
						{ this->Init(); } 
	
    virtual BOOL    Deploy();
	
	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;

	virtual int		GetDeployAnimation() const;

	virtual float	GetDeployTime() const;
	
	virtual int		GetIdleAnimation() const;

	virtual int		GetShootAnimation() const;

	virtual bool	GetFiresUnderwater() const;

	#ifdef AVH_SERVER
	virtual bool	GetLastPoint(vec3_t& outLastPoint) const;

	virtual void	SetLastPoint(vec3_t& inLastPoint);
	#endif
	
	virtual bool	GetIsDroppable() const;
	
	int				GetItemInfo(ItemInfo *p) const;
	
	virtual char*	GetViewModel() const;

	virtual int		iItemSlot(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();

	#ifdef AVH_SERVER
	bool			mPlacedValidWebPointSinceDeploy;
	vec3_t			mLastValidWebPoint;
	//vec3_t			mLastValidWebNormal;
	#endif

};

#ifdef AVH_SERVER
class AvHWebProjectile : public CBaseEntity
{
public:
	virtual bool	CreateWeb();
	
	virtual void	Precache(void);
	
	virtual void	Spawn(void);
	
	void EXPORT		WebTouch(CBaseEntity *pOther);

private:
	bool			GetWebSpinner(AvHWebSpinner*& outSpinner);
	
};
#endif

class AvHBabblerGun : public AvHAlienWeapon 
{ 
public: 
					AvHBabblerGun()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;

	virtual int		GetDeployAnimation() const;

	virtual int		GetIdleAnimation() const;

	virtual int		GetShootAnimation() const;
	
	int				GetItemInfo(ItemInfo *p) const;

	virtual char*	GetViewModel() const;
	
	virtual int		iItemSlot(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};


class AvHPrimalScream : public AvHAlienWeapon 
{ 
public: 
					AvHPrimalScream()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;
	
	virtual bool	GetFiresUnderwater() const;
	
	virtual bool	GetIsDroppable() const;

	virtual int		GetDeployAnimation() const;
	
	virtual float	GetDeployTime() const;
	
	virtual int		GetIdleAnimation() const;

	virtual int		GetShootAnimation() const;
	
	int				GetItemInfo(ItemInfo *p) const;
	
	virtual char*	GetViewModel() const;

	virtual int		iItemSlot(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};


//class AvHBuildingGun : public AvHAlienWeapon 
//{ 
//public: 
//					AvHBuildingGun()
//						{ this->Init(); } 
//
//	virtual CBaseEntity*	CreateBuilding(const Vector& inLocation) const;
//					
//	virtual int		GetBarrelLength() const;
//
//	virtual char*	GetBuildingClassName() const = 0;
//	
//	virtual AvHMessageID	GetBuildingMessageID() const = 0;
//
//	virtual int		GetBuildingPointCost() const = 0;
//
//	virtual int		GetDeployAnimation() const;
//
//	virtual float	GetDeployTime() const;
//
//	virtual int		GetIdleAnimation() const;
//
//	virtual int		GetShootAnimation() const;
//
//	virtual int		GetMaxAllowed() const;
//
//	virtual int		GetBuildingRadius() const = 0;
//	
//	virtual bool	GetFiresUnderwater() const;
//	
//	virtual bool	GetIsDroppable() const;
//
//	virtual bool	GetMustPressTriggerForEachShot() const;
//
//	virtual float	GetRange() const;
//
//	virtual char*	GetViewModel() const;
//	
//	virtual void	Precache(void);
//	
//	virtual void	Spawn() = 0;
//	
//	virtual bool	UsesAmmo(void) const;
//
//protected: 
//    virtual void    FireProjectiles(void);
//	
//    virtual void	Init();
//};
//
//class AvHResourceTowerGun : public AvHBuildingGun
//{
//public:
//
//	virtual AvHMessageID	GetBuildingMessageID() const;
//
//	virtual char*	GetBuildingClassName() const;
//	
//	virtual int		GetBuildingPointCost() const;
//
//	virtual int		GetBuildingRadius() const;
//
//	virtual int		GetItemInfo(ItemInfo *p);
//	
//	virtual int		GetMaxAllowed() const;
//
//	virtual int		iItemSlot(void);
//
//	virtual void	Spawn();
//	
//};
//
//
//class AvHOffenseChamberGun : public AvHBuildingGun
//{
//public:
//	
//	virtual AvHMessageID	GetBuildingMessageID() const;
//	
//	virtual char*	GetBuildingClassName() const;
//	
//	virtual int		GetBuildingPointCost() const;
//	
//	virtual int		GetBuildingRadius() const;
//	
//	virtual int		GetItemInfo(ItemInfo *p);
//	
//	virtual int		iItemSlot(void);
//	
//	virtual void	Spawn();
//	
//};
//
//
//class AvHDefenseChamberGun : public AvHBuildingGun
//{
//public:
//	
//	virtual AvHMessageID	GetBuildingMessageID() const;
//	
//	virtual char*	GetBuildingClassName() const;
//	
//	virtual int		GetBuildingPointCost() const;
//	
//	virtual int		GetBuildingRadius() const;
//	
//	virtual int		GetItemInfo(ItemInfo *p);
//	
//	virtual int		iItemSlot(void);
//	
//	virtual void	Spawn();
//	
//};
//
//class AvHSensoryChamberGun : public AvHBuildingGun
//{
//public:
//	
//	virtual AvHMessageID	GetBuildingMessageID() const;
//	
//	virtual char*	GetBuildingClassName() const;
//	
//	virtual int		GetBuildingPointCost() const;
//	
//	virtual int		GetBuildingRadius() const;
//	
//	virtual int		GetItemInfo(ItemInfo *p);
//	
//	virtual int		iItemSlot(void);
//	
//	virtual void	Spawn();
//	
//};
//
//class AvHMovementChamberGun : public AvHBuildingGun
//{
//public:
//	
//	virtual AvHMessageID	GetBuildingMessageID() const;
//	
//	virtual char*	GetBuildingClassName() const;
//	
//	virtual int		GetBuildingPointCost() const;
//	
//	virtual int		GetBuildingRadius() const;
//	
//	virtual int		GetItemInfo(ItemInfo *p);
//	
//	virtual int		iItemSlot(void);
//	
//	virtual void	Spawn();
//	
//};
//
//class AvHHiveGun : public AvHAlienWeapon 
//{
//public:
//
//	virtual int		GetBarrelLength() const;
//
//	virtual bool	GetFiresUnderwater() const;
//	
//	virtual bool	GetIsDroppable() const;
//	
//	virtual int		GetItemInfo(ItemInfo *p);
//
//	virtual char*	GetViewModel() const;
//
//	virtual int		iItemSlot(void);
//	
//	virtual void	Precache(void);
//
//	virtual void	Spawn();
//
//	virtual bool	UsesAmmo(void) const;
//	
//protected:
//    virtual void    FireProjectiles(void);
//
//    virtual void	Init();
//	
//};

class AvHParalysisGun : public AvHAlienWeapon 
{ 
public: 
					AvHParalysisGun()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;

	virtual int		GetDeployAnimation() const;

	virtual float	GetDeployTime() const;

	virtual int		GetIdleAnimation() const;

	virtual int		GetShootAnimation() const;
	
	virtual bool	GetFiresUnderwater() const;
	
	virtual bool	GetIsDroppable() const;
	
	int				GetItemInfo(ItemInfo *p) const;
	
	virtual char*	GetViewModel() const;

	virtual int		iItemSlot(void);

	virtual void	Precache(void);
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};


class AvHParasiteGun : public AvHAlienWeapon 
{ 
public: 
					AvHParasiteGun()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;

	virtual int		GetDamageType() const;
		
	virtual int		GetDeployAnimation() const;

	virtual bool	GetFiresUnderwater() const;
	
	virtual bool	GetIsDroppable() const;

	virtual	int		GetShootAnimation() const;
	
	int				GetItemInfo(ItemInfo *p) const;

	virtual char*	GetViewModel() const;
	
	virtual int		iItemSlot(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};

#ifdef AVH_SERVER
class AvHUmbraCloud : public CBaseEntity
{
public:
					AvHUmbraCloud();

	virtual void	Precache(void);

	virtual void	Spawn(void);
	
private:
	float			mTimeHit;
};

class AvHUmbraProjectile : public CBaseEntity
{
public:
	virtual void	Spawn(void);
	
	void EXPORT		UmbraTouch(CBaseEntity* inOther);
	
};
#endif


class AvHUmbraGun : public AvHAlienWeapon 
{ 
public: 
					AvHUmbraGun()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;
 
	virtual int		GetDeployAnimation() const;
		
	virtual bool	GetFiresUnderwater() const;

	virtual int		GetIdleAnimation() const;

	virtual int		GetShootAnimation() const;
		
	virtual bool	GetIsDroppable() const;
	
	int				GetItemInfo(ItemInfo *p) const;
	
	virtual char*	GetViewModel() const;

	virtual int		iItemSlot(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};


class AvHDivineWind : public AvHAlienWeapon 
{ 
public: 
					AvHDivineWind()
						{ this->Init(); } 

	virtual BOOL	CanHolster(void);
					
	virtual int		GetBarrelLength() const;

	virtual float	GetRateOfFire() const;

	virtual int		GetDeployAnimation() const;
	
	virtual bool	GetFiresUnderwater() const;
	
	virtual bool	GetIsDroppable() const;

	virtual int		GetShootAnimation() const;
	
	int				GetItemInfo(ItemInfo *p) const;

	virtual char*	GetViewModel() const;
	
	virtual int		iItemSlot(void);
	
	virtual BOOL	IsUseable(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;

	void EXPORT		Explode(void);
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();

	bool			mPrimed;
};

#ifdef AVH_SERVER
class AvHBileBomb : public CBaseEntity
{
public:
	virtual void	Spawn(void);

	void EXPORT		BileBombTouch(CBaseEntity *pOther);

	virtual void	Precache(void);

	void			SetDamage(float inDamage);

private:
	float			mDamage;
	//float			mExplodeSprite;
};
#endif

class AvHBileBombGun : public AvHAlienWeapon 
{ 
public: 
					AvHBileBombGun()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;

    virtual float   GetRateOfFire() const;

	virtual int		GetDamageType() const;

	virtual int		GetDeployAnimation() const;

	virtual int		GetShootAnimation() const;
		
	int				GetItemInfo(ItemInfo *p) const;
	
	virtual char*	GetViewModel() const;

	virtual int		iItemSlot(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};

#ifdef AVH_SERVER
class AvHAcidRocket : public CBaseEntity
{
public:
	virtual void	Precache(void);

	virtual void	Spawn(void);

	void EXPORT		AcidRocketTouch(CBaseEntity *pOther);

	void EXPORT		AcidRocketDeath();

	void			SetDamage(float inDamage);

private:
	float			mDamage;
};
#endif


class AvHAcidRocketGun : public AvHAlienWeapon 
{ 
public: 
					AvHAcidRocketGun()
						{ this->Init(); } 
	
	virtual int		GetBarrelLength() const;
	virtual float   GetRateOfFire() const;

	virtual int		GetDeployAnimation() const;
		
	virtual bool	GetFiresUnderwater() const;

	virtual bool	GetIsDroppable() const;

	virtual int		GetIdleAnimation() const;

	int				GetItemInfo(ItemInfo *p) const;

	virtual bool	GetMustPressTriggerForEachShot() const;

	virtual int		GetShootAnimation() const;
		
	virtual char*	GetViewModel() const;
	
	virtual int		iItemSlot(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();
};

class AvHDevour : public AvHAlienWeapon 
{ 
public: 
					AvHDevour()
						{ this->Init(); } 
	
	virtual BOOL	CanHolster(void);
 
	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;

	virtual bool	GetIsCapableOfFiring() const;
	
	virtual int		GetDeployAnimation() const;

	virtual float	GetDeployTime() const;
	
	virtual bool	GetFiresUnderwater() const;
	
	virtual int		GetIdleAnimation() const;

	virtual bool	GetIsDroppable() const;

	virtual int		GetShootAnimation() const;
	
	int				GetItemInfo(ItemInfo *p) const;
	
	virtual char*	GetViewModel() const;
	
	virtual int		iItemSlot(void);

	virtual void	Precache(void);
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;
	
protected: 
    virtual void    FireProjectiles(void);
	
    virtual void	Init();

    virtual bool    ProcessValidAttack(void);

};

#ifdef AVH_SERVER
class AvHStompProjectile : public CBaseEntity
{
public:
	AvHStompProjectile();
	
	virtual void	Precache(void);

	virtual void	SetStunTime(float inStunTime);

	virtual void	Spawn(void);
	
	void EXPORT		StompTouch(CBaseEntity* inOther);

	Vector			mSpawnLocation;

private:
	float			mStunTime;
	
};
#endif

class AvHStomp : public AvHAlienWeapon 
{ 
public: 
	AvHStomp()
	{ this->Init(); } 

	virtual BOOL	CanHolster(void);
	
	virtual int		GetBarrelLength() const;

    virtual float	GetRateOfFire() const;
	
	virtual int		GetDeployAnimation() const;
	
	virtual float	GetDeployTime() const;

	virtual void	GetEventAngles(Vector& outAngles) const;

	virtual void	GetEventOrigin(Vector& outOrigin) const;

	virtual bool	GetFiresUnderwater() const;
	
	virtual bool	GetIsDroppable() const;
	
	virtual int		GetShootAnimation() const;
	
	int				GetItemInfo(ItemInfo *p) const;
	
	virtual char*	GetViewModel() const;
	
	virtual int		iItemSlot(void);

	virtual BOOL	IsUseable(void);
	
	virtual void	Precache(void);
	
	virtual void	Spawn();
	
	virtual bool	UsesAmmo(void) const;
	
protected: 
	virtual void	GetStompOrigin(Vector& outOrigin) const;

	virtual void	GetStompVelocity(Vector& outVelocity) const;

    virtual void    FireProjectiles(void);
	
    virtual void	Init();
	
};



#endif


