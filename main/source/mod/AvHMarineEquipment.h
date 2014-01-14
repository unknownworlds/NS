//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHMarineEquipment.h $
// $Date: 2002/11/06 01:40:17 $
//
//-------------------------------------------------------------------------------
// $Log: AvHMarineEquipment.h,v $
// Revision 1.29  2002/11/06 01:40:17  Flayra
// - Turrets now need an active turret factory to keep firing
//
// Revision 1.28  2002/10/24 21:32:16  Flayra
// - All heavy armor to be given via console
// - Fix for AFKers on inf portals, also for REIN players when recycling portals
//
// Revision 1.27  2002/10/16 20:54:30  Flayra
// - Added phase gate sound
// - Fixed ghostly command station view model problem after building it
//
// Revision 1.26  2002/10/16 01:00:33  Flayra
// - Phasegates play looping sound, and stop it when they are destroyed
//
// Revision 1.25  2002/09/23 22:21:21  Flayra
// - Added jetpack and heavy armor
// - Added "cc online" sound
// - Turret factories now upgrade to advanced turret factories for siege
// - Added automatic resupply at armory, but removed it
// - Observatories scan in 2D now, to match commander range overlay
//
// Revision 1.24  2002/09/09 19:59:39  Flayra
// - Fixed up phase gates (no longer teleport you unless you have two, and they work properly now)
// - Refactored reinforcements
// - Fixed bug where secondary command station couldn't be built
//
// Revision 1.23  2002/07/26 23:05:54  Flayra
// - Numerical event feedback
// - Started to add sparks when buildings were hit but didn't know the 3D point to play it at
//
// Revision 1.22  2002/07/23 17:11:47  Flayra
// - Phase gates must be built and can be destroyed, observatories decloak aliens, hooks for fast reinforcements upgrade, nuke damage increased, commander banning
//
// Revision 1.21  2002/07/08 17:02:57  Flayra
// - Refactored reinforcements, updated entities for new artwork
//
// Revision 1.20  2002/06/25 18:04:43  Flayra
// - Renamed some buildings, armory is now upgraded to advanced armory
//
// Revision 1.19  2002/06/03 16:50:35  Flayra
// - Renamed weapons factory and armory, added ammo resupplying
//
// Revision 1.18  2002/05/28 17:51:34  Flayra
// - Tried to make nuke sound play, extended shake duration to sound length, reinforcement refactoring, mark command stations as mapper placed, so they aren't deleted on level cleanup, support for point-entity buildable command stations
//
// Revision 1.17  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVHMARINEEQUIPMENT_H
#define AVHMARINEEQUIPMENT_H

#include "util/nowarnings.h"
#include "dlls/weapons.h"
#include "mod/AvHBasePlayerWeapon.h"
#include "mod/AvHConstants.h"
#include "mod/AvHMarineWeapons.h"
#include "dlls/turret.h"
#include "mod/AvHEntities.h"
#include "dlls/ctripmine.h"
#include "mod/AvHReinforceable.h"

class AvHDeployedMine : public CBasePlayerItem
{
public:
	void EXPORT		ActiveThink();
	void EXPORT		ActiveTouch(CBaseEntity* inOther);
	void			Detonate();
	void			Precache(void);
	void EXPORT		PowerupThink();
	void			SetPlacer(entvars_t* inPlacer);
	void			Spawn(void);

	void			Killed(entvars_t* inAttacker, int inGib);
	int				TakeDamage(entvars_t *inInflictor, entvars_t *inAttacker, float inDamage, int inBitsDamageType);

private:
	void			DetonateIfOwnerInvalid();
	void			Explode(TraceResult* inTrace, int inBitsDamageType);
	void EXPORT		Smoke();

	Vector			mVecDir;
	Vector			mVecEnd;
	Vector			mOwnerOrigin;
	Vector			mOwnerAngles;

	float			mTimePlaced;
	float			mLastTimeTouched;
	bool			mDetonated;
	bool			mPoweredUp;
	EHANDLE			mOwner;
	entvars_t*		mPlacer;
};

class AvHPlayerEquipment : public CBasePlayerItem
{
public:
					AvHPlayerEquipment();

	virtual int		GetLifetime() const;

	virtual bool	GetIsPersistent() const;
	virtual void	SetPersistent();

	virtual void	KeyValue(KeyValueData* pkvd);

private:
	bool			mIsPersistent;
	int				mLifetime;
};

class AvHHealth : public AvHPlayerEquipment
{
public:
	// : 1017 GiveHealth now takes the amount as a paramater.
	static BOOL GiveHealth(CBaseEntity* inOther, float points);

	void Precache( void );
	void Spawn( void );
	void EXPORT Touch(CBaseEntity* inOther);
};

class AvHCatalyst : public AvHPlayerEquipment
{
public:
    static BOOL GiveCatalyst(CBaseEntity* inOther);
    
    void Precache( void );
    void Spawn( void );
    void EXPORT Touch(CBaseEntity* inOther);
};

class AvHGenericAmmo : public CBasePlayerAmmo
{
public:
	static BOOL GiveAmmo(CBaseEntity* inOther);

	BOOL AddAmmo( CBaseEntity *pOther );
	void Precache( void );
	void Spawn( void );

	void EXPORT Dropped(void);

private:
	bool mDropped;
};

class AvHHeavyArmor : public AvHPlayerEquipment
{
public:
	void Precache( void );
	void Spawn( void );
	void EXPORT Touch(CBaseEntity* inOther);
};

class AvHJetpack : public AvHPlayerEquipment
{
public:
	void Precache( void );
	void Spawn( void );
	void EXPORT Touch(CBaseEntity* inOther);
};

class AvHAmmoPack : public AvHPlayerEquipment
{
public:
	char m_szAmmoType[32];
	int m_iMaxAmmo;
	int	m_iAmmoAmt;
	int	m_iWeaponID; //weapon id this is for.
	float m_flNoTouch;	//Dont let anyone touch it while its falling

	void Precache( void );
	void Spawn( void );
	void EXPORT Touch(CBaseEntity* inOther);
};

class AvHScan : public CBaseAnimating
{
public:
					AvHScan();
	void			Precache(void);
	void			Spawn(void);
	void			ScanThink();

private:
	float			mTimeCreated;

};

class AvHMarineBaseBuildable : public AvHBaseBuildable
{
public:
							AvHMarineBaseBuildable(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inUser3);

	virtual char*			GetDeploySound() const;
	virtual char*			GetKilledSound() const;
	virtual int				GetPointValue() const;
    virtual int             GetTakeDamageAnimation() const;
	virtual void			ResetEntity();
	virtual int				TakeDamage(entvars_t* inInflictor, entvars_t* inAttacker, float inDamage, int inBitsDamageType);
	virtual bool			GetIsTechnologyAvailable(AvHMessageID inMessageID) const;
	virtual void			TechnologyBuilt(AvHMessageID inMessageID);

protected:
	void					SetEnergy(float inEnergy);
	float					mEnergy;
	
};

const float kPhaseGateIdleThink = 1.0f;

class AvHPhaseGate : public AvHMarineBaseBuildable
{
public:
					AvHPhaseGate();
	virtual int		GetIdleAnimation() const;
	bool			GetIsEnabled() const;
	virtual int		GetSequenceForBoundingBox() const;
	void			Killed(entvars_t* inAttacker, int inGib);
	void			Precache(void);
	void EXPORT		IdleThink();
	virtual void	ResetEntity();
	virtual void	SetHasBeenBuilt();
	void			SetTimeOfLastDeparture(float timeOfLastDeparture);
	bool			IsReadyToUse();
	bool			HasWarmedUp() const;
	void EXPORT		TeleportUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value);
	//void EXPORT		TeleportTouch(CBaseEntity *pOther);
	virtual void	UpdateOnRecycle(void);
	virtual void	UpdateOnRemove(void);

private:

    void            KillBuildablesTouchingPlayer(AvHPlayer* inPlayer, entvars_t* inInflictor);
    bool            GetAreTeammatesBlocking(AvHPlayer* thePlayer, const Vector& inOrigin) const;
	void			SetEnabled(bool inEnabledState);
	bool			mEnabled;
	bool			mHasWarmedUp;
	float			mTimeOfLastDeparture;
};

class AvHNuke : public AvHMarineBaseBuildable
{
public:
						AvHNuke();
	
	virtual void		Precache();
	
	void EXPORT			ActiveThink();

	void EXPORT			DeathThink();

	virtual char*		GetDeploySound() const;

	virtual char*		GetKilledSound() const;

	virtual void		Spawn();

private:
	bool				mActive;
	float				mTimeActivated;
	
};

class AvHInfantryPortal : public AvHMarineBaseBuildable, public AvHReinforceable
{
public:
					AvHInfantryPortal();

	virtual void	Killed(entvars_t* inAttacker, int inGib);

	virtual float	GetReinforceTime() const;

	void EXPORT		PortalThink();
	
	virtual void	Precache();

	virtual void	ResetEntity();

	virtual void	SetHasBeenBuilt();
	
	// From AvHReinforceable
	virtual void	CueRespawnEffect(AvHPlayer* inPlayer);
	
	virtual bool	GetCanReinforce() const;
	
	virtual bool	GetSpawnLocationForPlayer(CBaseEntity* inPlayer, Vector& outLocation) const;
	
	virtual AvHTeamNumber	GetReinforceTeamNumber() const;

	virtual void	UpdateOnRecycle(void);
	virtual void	UpdateOnRemove(void);

	virtual int	    GetIdleAnimation() const;
	virtual int	    GetIdle1Animation() const;
	virtual int	    GetIdle2Animation() const;

	// virtual int		GetDeployAnimation() const;
	// virtual int		GetSpawnAnimation() const;

protected:

	virtual void	ResetReinforcingPlayer(bool inSuccess);
	
};

class AvHCommandStation : public AvHMarineBaseBuildable
{
public:
						AvHCommandStation();

	void EXPORT			CommandTouch( CBaseEntity *pOther );
	
	void EXPORT			CommandUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	virtual int			GetIdleAnimation() const;

    virtual bool	    GetIsTechnologyAvailable(AvHMessageID inMessageID) const;
    
	virtual char*		GetKilledSound() const;

	virtual int			GetPointValue() const;

    virtual void        Killed( entvars_t *pevAttacker, int iGib );

	virtual void		Materialize();

	virtual int			ObjectCaps(void);
	
	virtual void		Precache(void);
	
	virtual void		SetHasBeenBuilt();

	virtual void		SetInactive();
	
	virtual void		Spawn(void);
	
	virtual int			TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType );
	
private:
	void EXPORT			ActivateThink(void);
	void EXPORT			CommanderUsingThink(void);
	void				EjectCommander();

	int					mCommanderAtThisStation;
	float				mTimeToPlayOnlineSound;
	
};


class AvHTurretFactory : public AvHMarineBaseBuildable
{
public:
	AvHTurretFactory();

	virtual int		GetIdle1Animation() const;

	virtual int		GetIdle2Animation() const;

	virtual int		GetResearchAnimation() const;
	
	virtual bool	GetSupportsTechID(AvHTechID	inTechID) const;

	virtual void	SetHasBeenBuilt();

	virtual void	TriggerAddTech() const;

	virtual void	TriggerRemoveTech() const;

	virtual void	Upgrade();

private:
	virtual void	CheckTurretEnabledState() const;
	
};

class AvHArmory : public AvHMarineBaseBuildable
{
public:
	AvHArmory();

	virtual int		GetActiveAnimation() const;

	virtual int		GetIdle1Animation() const;
	
	virtual int		GetIdle2Animation() const;

	virtual int		GetResearchAnimation() const;

	virtual int		GetSequenceForBoundingBox() const;

	virtual bool	GetSupportsTechID(AvHTechID	inTechID) const;
		
	virtual void	Precache();

	//void EXPORT		ResupplyThink();
	void EXPORT		ResupplyUse(CBaseEntity* inActivator, CBaseEntity* inCaller, USE_TYPE inUseType, float inValue);

	virtual void	SetHasBeenBuilt();

	virtual void	Upgrade();

};

class AvHArmsLab : public AvHMarineBaseBuildable
{
public:
	AvHArmsLab();

	virtual int		GetResearchAnimation() const;

	virtual int		GetSequenceForBoundingBox() const;

};

class AvHPrototypeLab : public AvHMarineBaseBuildable
{
public:
	AvHPrototypeLab();
};

class AvHObservatory : public AvHMarineBaseBuildable
{
public:
					AvHObservatory();
	virtual int		GetSequenceForBoundingBox() const;
	virtual void	Materialize();
	void EXPORT		ObservatoryThink();
	virtual void	ResetEntity();
	virtual void	SetHasBeenBuilt();
	virtual void	Spawn();

	virtual int		GetActiveAnimation() const;
	virtual int		GetIdle1Animation() const;
	virtual int		GetIdle2Animation() const;
	virtual int		GetResearchAnimation() const;
		
};

//class AvHChemLab : public AvHMarineBaseBuildable
//{
//public:
//	AvHChemLab();
//};
//
//class AvHMedLab : public AvHMarineBaseBuildable
//{
//public:
//	AvHMedLab();
//};
//
//class AvHNukePlant : public AvHMarineBaseBuildable
//{
//public:
//	AvHNukePlant();
//};

#endif
