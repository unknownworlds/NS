//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHFlameGun.cpp $
// $Date: 2002/06/03 16:39:09 $
//
//-------------------------------------------------------------------------------
// $Log: AvHFlameGun.cpp,v $
// Revision 1.7  2002/06/03 16:39:09  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.6  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHMarineWeapons.h"
#include "mod/AvHPlayer.h"
#include "cl_dll/eventscripts.h"
#include "cl_dll/in_defs.h"
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#include "common/hldm.h"
#include "common/event_api.h"
#include "common/event_args.h"
#include "cl_dll/vector_util.h"
#include "mod/AvHMarineWeapons.h"

LINK_ENTITY_TO_CLASS(kwFlameGun, AvHFlameGun);
void V_PunchAxis( int axis, float punch );

BOOL AvHFlameGun::Deploy()
{
	return DefaultDeploy(kFGVModel, kFGPModel, this->GetDeployAnimation(), kFGAnimExt);
}

void AvHFlameGun::Init()
{
	this->mRange = kFGRange;
	this->mDamage = kFGDamage;
	this->mROF = kFGROF;
}

int	AvHFlameGun::GetBarrelLength() const
{
	return kFGBarrelLength;
}

int AvHFlameGun::GetItemInfo(ItemInfo *p)
{
	p->pszName = STRING(pev->classname);
	p->pszAmmo1 = "FGAmmo";
	p->iMaxAmmo1 = kFGMaxAmmo;
	p->pszAmmo2 = NULL;
	p->iMaxAmmo2 = -1;
	p->iMaxClip = kFGMaxClip;
	p->iFlags = ITEM_FLAG_NOAUTOSWITCHEMPTY;
	p->iSlot = 1;
	p->iPosition = 5;

	//p->iId = m_iId = AVH_WEAPON_FLAMER;
	p->iId = m_iId = AVH_WEAPON_GRENADE_GUN;
	ASSERT(false);
		
	p->iWeight = 10;

	return 1;
}

int	AvHFlameGun::iItemSlot(void)
{
	return 2;
}

void AvHFlameGun::Precache()
{
	AvHMarineWeapon::Precache();

	PRECACHE_MODEL(kFGVModel);
	PRECACHE_MODEL(kFGWModel);
	PRECACHE_MODEL(kFGPModel);
	PRECACHE_MODEL(kFGEjectModel);

	PRECACHE_SOUND(kFGFireSound1);
	PRECACHE_SOUND(kFGFireSound2);
	PRECACHE_SOUND(kFGFireSound3);
	PRECACHE_SOUND(kFGFireSound4);

	PRECACHE_SOUND(kFGReloadSound);

	this->mEvent = PRECACHE_EVENT(1, kFGEventName);
}

void AvHFlameGun::Spawn() 
{ 
    AvHMarineWeapon::Spawn(); 

	Precache();

	//this->m_iId = AVH_WEAPON_FLAMER;
	ASSERT(false);
	this->m_iDefaultAmmo = kFGMaxClip;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsFlameGun);

	SET_MODEL(ENT(this->pev), kFGWModel);

	FallInit();// get ready to fall down.
} 
