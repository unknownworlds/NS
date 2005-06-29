//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAlienEquipment.h $
// $Date: 2002/10/16 00:46:51 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAlienEquipment.h,v $
// Revision 1.13  2002/10/16 00:46:51  Flayra
// - Movement chambers not useable until 3 seconds after built
// - Moved and removed some building sounds, added one for chamber death
// - Don't allow using movement chamber while gestating
//
// Revision 1.12  2002/09/09 19:47:47  Flayra
// - Alien buildings now animate
// - Sensory chamber no longer hurts players when touched
//
// Revision 1.11  2002/08/31 18:01:00  Flayra
// - Work at VALVe
//
// Revision 1.10  2002/08/16 02:32:09  Flayra
// - Added damage types
// - Swapped umbra and bile bomb
//
// Revision 1.9  2002/07/25 16:57:59  flayra
// - Linux changes
//
// Revision 1.8  2002/07/23 16:55:36  Flayra
// - Alien buildings now have an iuser3 type (used for "resource tower is under attack")
//
// Revision 1.7  2002/07/01 21:13:03  Flayra
// - Added auto-build to all alien buildings, added new parasiting ability to sensory chambers
//
// Revision 1.6  2002/06/03 16:22:45  Flayra
// - Movement chamber needs to be used to teleport, get points for building resource tower, defense chambers heal buildables too, improved sensory chamber (still has problems though), removed duplicate hive classname
//
// Revision 1.5  2002/05/28 17:11:39  Flayra
// - Added new turret instead of old offensive upgrades chamber, added secondary functions
//
// Revision 1.4  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_ALIEN_EQUIPMENT_H
#define AVH_ALIEN_EQUIPMENT_H

#include "util/nowarnings.h"
#include "dlls/weapons.h"
#include "mod/AvHBasePlayerWeapon.h"
#include "mod/AvHConstants.h"
#include "mod/AvHEntities.h"
#include "mod/AvHBuildable.h"
#include "mod/AvHEntities.h"
#include "mod/AvHTurret.h"

class AvHAlienResourceTower : public AvHResourceTower
{
public:
					AvHAlienResourceTower();

	virtual int		GetActiveAnimation() const;

	virtual char*	GetDeploySound() const;

	virtual bool	GetIsOrganic() const;
	
	virtual int		GetPointValue() const;

	virtual char*	GetModelName() const;

	virtual int		GetSequenceForBoundingBox() const;

	char*			GetActiveSoundList() const;

	void			Killed(entvars_t* pevAttacker, int iGib);

	virtual void	Materialize();

	void			Precache();

	virtual void	Spawn();

private:
	void EXPORT		AlienResourceThink();
	
};

class AvHAlienUpgradeBuilding : public AvHBaseBuildable
{
public:
					AvHAlienUpgradeBuilding(AvHMessageID inMessageID, AvHTechID inTechID, char* inClassName, AvHUser3 inUser3);

	virtual char*	GetDeploySound() const = 0;

	virtual bool	GetIsOrganic() const;

	virtual int		GetPointValue() const;
	
	virtual AvHAlienUpgradeCategory	GetUpgradeCategory() const = 0;

	virtual void	Killed(entvars_t* pevAttacker, int iGib);

	virtual void	Materialize();

	virtual void	Precache();

	virtual void	SetHasBeenBuilt();
	
	virtual void	Spawn();

private:
	void EXPORT		AlienBuildingThink();

};

//class AvHOffenseChamber : public AvHTurret
//{
//public:
//					AvHOffenseChamber();
//
//	virtual char*	GetActiveSound() const;
//	virtual char*	GetAlertSound() const;
//	virtual char*	GetPingSound() const;
//
//	virtual int		GetRange() const;
//
//	virtual void	Precache();
//
//	virtual void	Spawn();
//};

class AvHDefenseChamber : public AvHAlienUpgradeBuilding
{
public:
					AvHDefenseChamber();

	virtual char*	GetDeploySound() const;
	
	virtual char*	GetModelName() const;

	void EXPORT		RegenAliensThink();

	virtual void	SetHasBeenBuilt();

	virtual AvHAlienUpgradeCategory	GetUpgradeCategory() const;
};

class AvHSensoryChamber : public AvHAlienUpgradeBuilding
{
public:
					AvHSensoryChamber();

	virtual char*	GetDeploySound() const;
	
	bool			GetIsEntityInSight(CBaseEntity* inEntity);

	virtual char*	GetModelName() const;
	
	virtual AvHAlienUpgradeCategory	GetUpgradeCategory() const;

	virtual void	Precache();

	virtual void	SetHasBeenBuilt();

};

class AvHMovementChamber : public AvHAlienUpgradeBuilding
{
public:
					AvHMovementChamber();

	virtual char*	GetDeploySound() const;
	
	virtual char*	GetModelName() const;

	virtual void	Precache();

	virtual void	ResetEntity();

	virtual void	SetHasBeenBuilt();

	void EXPORT		EnergyAliensThink();

	void EXPORT		TeleportUse(CBaseEntity* inActivator, CBaseEntity* inCaller, USE_TYPE inUseType, float inValue);

	void EXPORT		TeleportUseThink();

	virtual AvHAlienUpgradeCategory	GetUpgradeCategory() const;

private:
	float			mLastTimeScannedHives;
	int				mTeleportHiveIndex;
};


#endif
