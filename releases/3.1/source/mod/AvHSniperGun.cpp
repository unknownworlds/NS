//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHSniperGun.cpp $
// $Date: 2002/07/24 18:55:52 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSniperGun.cpp,v $
// Revision 1.4  2002/07/24 18:55:52  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.3  2002/06/03 16:39:10  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.2  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHAlienWeapons.h"
#include "mod/AvHPlayer.h"
#include "cl_dll/eventscripts.h"
#include "cl_dll/in_defs.h"
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#include "common/hldm.h"
#include "common/event_api.h"
#include "common/event_args.h"
#include "cl_dll/vector_util.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHGamerules.h"

LINK_ENTITY_TO_CLASS(kwSniperGun, AvHSniperGun);

BOOL AvHSniperGun::Deploy()
{
	return DefaultDeploy(kSniperVModel, kSniperPModel, this->GetDeployAnimation(), kSniperAnimExt);
}

void AvHSniperGun::Init()
{
	this->mRange = kSniperRange;
	this->mDamage = kSniperDamage;
	this->mROF = kSniperROF;
}

int	AvHSniperGun::GetBarrelLength() const
{
	return kSniperBarrelLength;
}

bool AvHSniperGun::GetFiresUnderwater() const
{
	return true;
}

bool AvHSniperGun::GetIsDroppable() const
{
	return false;
}

void AvHSniperGun::FireProjectiles(void)
{
	#ifdef AVH_SERVER
	
	UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);

	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc = this->m_pPlayer->GetGunPosition( ) + vecAiming;

	// TODO: 

	#endif	
}

void AvHSniperGun::Precache()
{
	AvHAlienWeapon::Precache();

	PRECACHE_MODEL(kSniperVModel);
	PRECACHE_MODEL(kAlienGunWModel);

	PRECACHE_SOUND(kSniperFireSound);

	this->mEvent = PRECACHE_EVENT(1, kSniperShootEventName);
}

void AvHSniperGun::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 

	Precache();

	this->m_iId = AVH_WEAPON_SNIPER;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsSniperGun);

	SET_MODEL(ENT(this->pev), kAlienGunWModel);

	FallInit();// get ready to fall down.
} 

bool AvHSniperGun::UsesAmmo(void) const
{
	return false;
}


