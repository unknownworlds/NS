//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAlienTurret.h $
// $Date: 2002/07/23 16:57:05 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAlienTurret.h,v $
// Revision 1.4  2002/07/23 16:57:05  Flayra
// - Alien turret refactoring and fixing (the view offset in spawn() was causing them to always miss crouched players)
//
//===============================================================================
#ifndef AVH_ALIEN_TURRET_H
#define AVH_ALIEN_TURRET_H

#include "mod/AvHTurret.h"
#include "mod/AvHAlienEquipmentConstants.h"

#ifdef AVH_SERVER
class AvHSpike : public CBaseEntity
{
public:
	virtual void	Precache(void);
	
	virtual void	Spawn(void);
	
	void EXPORT		SpikeTouch(CBaseEntity *pOther);
	
	void EXPORT		SpikeDeath();
	
	void			SetDamage(float inDamage);
	
private:
	float			mDamage;
};
#endif


class AvHAlienTurret : public AvHTurret
{
public:
					AvHAlienTurret();
					AvHAlienTurret(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inUser3);

	virtual bool	Energize(float inEnergyAmount);
					
	virtual char*	GetDeploySound() const;
	char*			GetModelName() const;

	virtual	bool	GetIsOrganic() const;

	int				GetIdle1Animation() const;
	int				GetIdle2Animation() const;
	
	virtual int		GetPointValue(void) const;
	virtual int		GetXYRange() const;
	virtual int		GetVerticalFOV() const;
	virtual int		MoveTurret(void);
	virtual bool	GetBaseClassAnimatesTurret() const;

	virtual int		GetTakeDamageAnimation() const;

	virtual void	ResetEntity();
	virtual void	Precache(void);
	virtual void	SetNextAttack();
	virtual void	Shoot(const Vector &inOrigin, const Vector &inToEnemy, const Vector& inVecEnemyVelocity);
	virtual void	Spawn();

protected:
	virtual float	GetRateOfFire() const;
	
private:
	void EXPORT		PreBuiltThink();

	int				mEvent;	

	float			mEnergy;
};

#endif
