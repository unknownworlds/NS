//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose:
//
// $Workfile: AvHMachineGun.cpp $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHMachineGun.cpp,v $
// Revision 1.23  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.22  2002/10/16 20:53:09  Flayra
// - Removed weapon upgrade sounds
//
// Revision 1.21  2002/10/03 18:46:11  Flayra
// - Added heavy view model
//
// Revision 1.20  2002/08/31 18:01:02  Flayra
// - Work at VALVe
//
// Revision 1.19  2002/08/16 02:44:11  Flayra
// - New damage types
//
// Revision 1.18  2002/08/09 01:04:32  Flayra
// - MG is faster to deploy
//
// Revision 1.17  2002/07/26 23:05:15  Flayra
// - Tweak
//
// Revision 1.16  2002/07/24 19:09:17  Flayra
// - Linux issues
//
// Revision 1.15  2002/07/24 18:45:42  Flayra
// - Linux and scripting changes
//
// Revision 1.14  2002/07/08 17:08:52  Flayra
// - Tweaked for bullet spread
//
// Revision 1.13  2002/06/25 17:50:59  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.12  2002/06/03 16:37:31  Flayra
// - Constants and tweaks to make weapon anims and times correct with new artwork, added different deploy times (this should be refactored a bit more)
//
// Revision 1.11  2002/05/28 17:44:58  Flayra
// - Tweak weapon deploy volume, as Valve's sounds weren't normalized
//
// Revision 1.10  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHMarineWeapons.h"
#include "mod/AvHPlayer.h"

#ifdef AVH_CLIENT
#include "cl_dll/eventscripts.h"
#include "cl_dll/in_defs.h"
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#endif

#include "common/hldm.h"
#include "common/event_api.h"
#include "common/event_args.h"
#include "common/vector_util.h"
#include "mod/AvHMarineWeapons.h"
#include "dlls/util.h"

LINK_ENTITY_TO_CLASS(kwMachineGun, AvHMachineGun);
void V_PunchAxis( int axis, float punch );

void AvHMachineGun::Init()
{
	this->mRange = kMGRange;
	this->mDamage = BALANCE_VAR(kMGDamage);
}

int	AvHMachineGun::GetBarrelLength() const
{
	return kMGBarrelLength;
}

float AvHMachineGun::GetRateOfFire() const
{
	return BALANCE_VAR(kMGROF);
}

int	AvHMachineGun::GetDamageType() const
{
	//return NS_DMG_PIERCING;
	return NS_DMG_NORMAL;
}

char* AvHMachineGun::GetDeploySound() const
{
	return kMGDeploySound;
}

float AvHMachineGun::GetDeployTime() const
{
	return .4f;
}

bool AvHMachineGun::GetHasMuzzleFlash() const
{
	return true;
}

char* AvHMachineGun::GetHeavyViewModel() const
{
	return kMGHVVModel;
}

char* AvHMachineGun::GetPlayerModel() const
{
	return kMGPModel;
}

char* AvHMachineGun::GetViewModel() const
{
	return kMGVModel;
}

char* AvHMachineGun::GetWorldModel() const
{
	return kMGWModel;
}

float AvHMachineGun::GetReloadTime(void) const
{
	return 3.0f;
}

Vector AvHMachineGun::GetProjectileSpread() const
{
	return kMGSpread;
}

void AvHMachineGun::Precache()
{
	AvHMarineWeapon::Precache();

	PRECACHE_UNMODIFIED_MODEL(kMGEjectModel);

	PRECACHE_UNMODIFIED_SOUND(kMGFireSound1);
	PRECACHE_UNMODIFIED_SOUND(kMGReloadSound);

	PRECACHE_UNMODIFIED_MODEL(kGenericWallpuff);

	this->mEvent = PRECACHE_EVENT(1, kMGEventName);
}


void AvHMachineGun::Spawn()
{
    AvHMarineWeapon::Spawn();

	Precache();

	this->m_iId = AVH_WEAPON_MG;
	this->m_iDefaultAmmo = BALANCE_VAR(kMGMaxClip)*(BALANCE_VAR(kMarineSpawnClips) + 1);

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsMachineGun);

	SET_MODEL(ENT(this->pev), kMGWModel);

	FallInit();// get ready to fall down.
}
