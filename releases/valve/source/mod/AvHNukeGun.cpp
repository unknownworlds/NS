//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHNukeGun.cpp $
// $Date: 2002/06/03 16:39:09 $
//
//-------------------------------------------------------------------------------
// $Log: AvHNukeGun.cpp,v $
// Revision 1.9  2002/06/03 16:39:09  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.8  2002/05/23 02:33:20  Flayra
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

LINK_ENTITY_TO_CLASS(kwNukeGun, AvHNukeGun);
void V_PunchAxis( int axis, float punch );

BOOL AvHNukeGun::Deploy()
{
	return DefaultDeploy(kNGVModel, kNGPModel, this->GetDeployAnimation(), kNGAnimExt);
}

void AvHNukeGun::Init()
{
	this->mRange = kNGRange;
	this->mDamage = kNGDamage;
	this->mROF = kNGROF;
}						   

int	AvHNukeGun::GetBarrelLength() const
{
	return kNGBarrelLength;
}

void AvHNukeGun::Precache()
{
	AvHMarineWeapon::Precache();

	PRECACHE_MODEL(kNGVModel);
	PRECACHE_MODEL(kNGWModel);
	PRECACHE_MODEL(kNGPModel);
	PRECACHE_MODEL(kNGEjectModel);

	PRECACHE_SOUND(kNGFireSound1);
	PRECACHE_SOUND(kNGFireSound2);
	PRECACHE_SOUND(kNGFireSound3);
	PRECACHE_SOUND(kNGFireSound4);
	PRECACHE_SOUND(kNGReloadSound);

	this->mEvent = PRECACHE_EVENT(1, kNGEventName);
}

void AvHNukeGun::Spawn() 
{ 
    AvHMarineWeapon::Spawn(); 

	Precache();

	this->m_iId = -1;// AVH_WEAPON_NUKE
	m_iDefaultAmmo = kNGMaxAmmo;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsNukeGun);

	SET_MODEL(ENT(this->pev), kNGWModel);

	FallInit();// get ready to fall down.
} 




