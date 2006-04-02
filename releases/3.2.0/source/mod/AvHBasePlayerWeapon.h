//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHBasePlayerWeapon.h $
// $Date: 2002/10/03 18:39:24 $
//
//-------------------------------------------------------------------------------
// $Log: AvHBasePlayerWeapon.h,v $
// Revision 1.24  2002/10/03 18:39:24  Flayra
// - Added heavy view models
//
// Revision 1.23  2002/09/23 22:10:46  Flayra
// - Weapons now stick around the way they should (forever when dropped by commander, weapon stay time when dropped by player)
//
// Revision 1.22  2002/08/16 02:33:12  Flayra
// - Added damage types
//
// Revision 1.21  2002/07/26 01:52:03  Flayra
// - Linux support for FindFirst/FindNext
//
// Revision 1.20  2002/07/08 16:47:31  Flayra
// - Reworked bullet firing to add random spread (bug #236), temporarily hacked shotty player animation, removed old adrenaline, don't allow using weapons when invulnerable
//
// Revision 1.19  2002/07/01 21:17:13  Flayra
// - Removed outdated adrenaline concept, made ROF generic for primal scream
//
// Revision 1.18  2002/06/25 17:41:13  Flayra
// - Reworking for correct player animations and new enable/disable state
//
// Revision 1.17  2002/06/10 19:50:37  Flayra
// - First-pass at level 1 animated view model (different anims when running and walking)
//
// Revision 1.16  2002/06/03 16:29:53  Flayra
// - Added resupply (from arsenal), better animation support (for both view model and player model)
//
// Revision 1.15  2002/05/28 17:37:33  Flayra
// - Max entities fix, added animation empty fire
//
// Revision 1.14  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVHBASEPLAYERWEAPON_H
#define AVHBASEPLAYERWEAPON_H

#include "dlls/weapons.h"
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/monsters.h"
#include "dlls/weapons.h"
#include "dlls/nodes.h"
#include "dlls/player.h"
#include "dlls/soundent.h"
#include "dlls/gamerules.h"
#include "util/CString.h"
#include "mod/AvHBasePlayerWeaponConstants.h"
#include "mod/AvHConstants.h"
#include "mod/AvHSpecials.h"

const int kIdleAnimationOne = 0;
const int kIdleAnimationTwo = 1;
const int kReloadAnimation = 2;
const int kShootAnimation = 3;
const int kShootEmptyAnimation = 4;
const int kDeployAnimation = 5;

Vector UTIL_GetRandomSpreadDir(unsigned int inSeed, int inShotNumber, const Vector& inBaseDirection, const Vector& inRight, const Vector& inUp, const Vector& inSpread);
Vector UTIL_GetRandomSpreadDirFrom(unsigned int inSeed, int inShotNumber, const Vector& inBaseDirection, const Vector& inRight, const Vector& inUp, const Vector& inSpread, const Vector& inFromSpread);

class AvHBasePlayerWeapon : public CBasePlayerWeapon
{
public:
                    AvHBasePlayerWeapon();
	virtual void	PrintWeaponToClient(CBaseEntity *theAvHPlayer);
	virtual int		AddToPlayer( CBasePlayer *pPlayer );

    virtual float   ComputeAttackInterval() const;

	virtual void	DeductCostForShot(void);

	virtual BOOL	DefaultDeploy(char *szViewModel, char *szWeaponModel, int iAnim, char *szAnimExt, int skiplocal = 0, int body = 0);

	virtual int		DefaultReload(int iClipSize, int iAnim, float fDelay, int body = 0);

    virtual BOOL    Deploy();

	virtual char*	GetActiveViewModel() const;
	
	virtual char*	GetAnimationExtension() const;
	
	virtual int		GetBarrelLength() const = 0;

    virtual bool    GetCanBeResupplied() const;

	virtual int		GetClipSize() const;

    virtual bool    GetIsGunPositionValid() const;

    virtual int     GetPrimaryAmmoAmount() const;

	virtual int		GetDamageType() const;

	virtual int		GetDeployAnimation() const;

	virtual int		GetEmptyShootAnimation() const;

	virtual char*	GetDeploySound() const;

	virtual float	GetDeploySoundVolume() const;

	virtual bool	GetEnabledState() const;

	virtual void	GetEventOrigin(Vector& outOrigin) const;

	virtual void	GetEventAngles(Vector& outAngles) const;

	virtual bool	GetIsFiring() const;

	virtual bool	GetMustPressTriggerForEachShot() const
						{ return false;	}

	virtual bool	GetHasMuzzleFlash() const;

	virtual bool	GetIsCapableOfFiring() const;
	
	virtual bool	GetIsPlayerMoving() const;

	virtual bool	GetFiresUnderwater() const;

	virtual int		GetIdleAnimation() const;

	virtual Vector	GetProjectileSpread() const;

	virtual float	GetRateOfFire() const;

	virtual char*	GetPlayerModel() const;

    virtual char*   GetPrimeSound() const;

    virtual float	GetPrimeSoundVolume() const;

	virtual int		GetShootAnimation() const;

	virtual char*	GetViewModel() const;

	virtual char*	GetWorldModel() const;

	virtual bool	GetIsDroppable() const;
	
	virtual int		GetMaxClipsCouldReceive();

    // Priming functions
    virtual float   GetWeaponPrimeTime() const;
    virtual void    PrimeWeapon();
    virtual BOOL    GetIsWeaponPrimed() const;
    virtual BOOL    GetIsWeaponPriming() const;
    virtual int     GetPrimeAnimation() const;

	virtual float	GetRange() const;

	virtual int		GetReloadAnimation() const;

	virtual vec3_t	GetWorldBarrelPoint() const;

	virtual void	Holster( int skiplocal = 0 );

	virtual void	ItemPostFrame( void );	// called each frame by the player PostThink

	virtual void    Precache();

	virtual void    PrimaryAttack();

    virtual void    Reload();

    virtual bool	Resupply();

	virtual void	SendWeaponAnim(int inAnimation, int skiplocal = 1, int body = 0);

	// Only to be used on server
	#ifdef AVH_SERVER
	virtual int		GetGroundLifetime() const;
	
	virtual void	SetGroundLifetime(int inGroundLifetime);

	virtual int		GetResourceCost() const;

	virtual void	SetOverwatchState(bool inState);

	virtual void	UpdateInventoryEnabledState(int inNumActiveHives);

	// Methods for placing in the map
	void EXPORT		VirtualDestroyItem(void);
	virtual int		GetLifetime() const;
	virtual bool	GetIsPersistent() const;
	virtual void	SetPersistent();
	virtual void	KeyValue(KeyValueData* pkvd);
	virtual void	ResetEntity();
	virtual void	VirtualMaterialize(void);
	#endif

	virtual void    SecondaryAttack();

	virtual void	Spawn();

	virtual bool	UsesAmmo(void) const;

	virtual BOOL	UseDecrement(void);

	virtual void    WeaponIdle();

protected:
	//BOOL			Deploy(char *szViewModel, char *szWeaponModel, int iAnim, char *szAnimExt, float inNextAttackTime, int skiplocal = 0);
		
	virtual bool	GetAllowedForUser3(AvHUser3 inUser3) = 0;

	virtual int		GetShotsInClip() const;

	virtual AvHWeaponID		GetPreviousWeaponID() const;

    virtual bool    ProcessValidAttack(void);
	virtual float	GetReloadTime(void) const;
	virtual float	GetTimePassedThisTick() const;

    virtual void    PlaybackEvent(unsigned short inEvent, int inIparam2 = 0, int inFlags = FEV_NOTHOST) const;
    
    virtual void    SetAnimationAndSound(void);

    virtual void    FireProjectiles(void);

    virtual void    SetNextAttack(void);

	virtual void	SetNextIdle(void);

	unsigned short          mEvent;
	unsigned short			mWeaponAnimationEvent;
	unsigned short          mStartEvent;
	unsigned short          mEndEvent;

	float					mRange;
	float					mDamage;
	bool					mAttackButtonDownLastFrame;
	float					mTimeOfLastResupply;
    
    float                   mTimeOfLastPrime;
    bool                    mWeaponPrimeStarted;
	
	#ifdef AVH_SERVER
	bool					mInOverwatch;
	bool					mIsPersistent;
	int						mLifetime;
	#endif
	
    // sounds               
    CStringList				mFireSounds;
};

#endif

