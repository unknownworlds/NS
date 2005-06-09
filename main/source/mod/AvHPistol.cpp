//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose:
//
// $Workfile: AvHPistol.cpp $
// $Date: 2002/11/22 21:28:17 $
//
//-------------------------------------------------------------------------------
// $Log: AvHPistol.cpp,v $
// Revision 1.13  2002/11/22 21:28:17  Flayra
// - mp_consistency changes
//
// Revision 1.12  2002/10/16 20:53:09  Flayra
// - Removed weapon upgrade sounds
//
// Revision 1.11  2002/10/03 18:46:58  Flayra
// - Added heavy view model
//
// Revision 1.10  2002/08/09 01:09:00  Flayra
// - Made pistol faster to deploy
//
// Revision 1.9  2002/07/24 19:09:17  Flayra
// - Linux issues
//
// Revision 1.8  2002/07/24 18:45:42  Flayra
// - Linux and scripting changes
//
// Revision 1.7  2002/07/08 17:08:52  Flayra
// - Tweaked for bullet spread
//
// Revision 1.6  2002/06/25 17:50:59  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.5  2002/06/03 16:37:31  Flayra
// - Constants and tweaks to make weapon anims and times correct with new artwork, added different deploy times (this should be refactored a bit more)
//
// Revision 1.4  2002/05/28 17:44:58  Flayra
// - Tweak weapon deploy volume, as Valve's sounds weren't normalized
//
// Revision 1.3  2002/05/23 02:33:20  Flayra
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

LINK_ENTITY_TO_CLASS(kwPistol, AvHPistol);
void V_PunchAxis( int axis, float punch );

void AvHPistol::Init()
{
	this->mRange = kHGRange;
	this->mDamage = BALANCE_VAR(kHGDamage);
}

int	AvHPistol::GetBarrelLength() const
{
	return kHGBarrelLength;
}

float AvHPistol::GetRateOfFire() const
{
    return BALANCE_VAR(kHGROF);
}

int	AvHPistol::GetDeployAnimation() const
{
	return 6;
}

char* AvHPistol::GetDeploySound() const
{
	return kHGDeploySound;
}

float AvHPistol::GetDeployTime() const
{
	return .35f;
}

int	AvHPistol::GetEmptyShootAnimation() const
{
	return 5;
}

char* AvHPistol::GetHeavyViewModel() const
{
	return kHGHVVModel;
}

char* AvHPistol::GetPlayerModel() const
{
	return kHGPModel;
}

Vector AvHPistol::GetProjectileSpread() const
{
	return kHGSpread;
}

char* AvHPistol::GetViewModel() const
{
	return kHGVModel;
}

char* AvHPistol::GetWorldModel() const
{
	return kHGWModel;
}

int	AvHPistol::GetReloadAnimation() const
{
	int theReloadAnimation = 2;

	// If empty, use empty reload
	bool theHasAmmo = (this->m_iClip > 0);// || (this->m_pPlayer->m_rgAmmo[this->m_iPrimaryAmmoType] > 0);
	if(!theHasAmmo)
	{
		theReloadAnimation = 3;
	}

	return theReloadAnimation;
}


int	AvHPistol::GetShootAnimation() const
{
	return 4;
}

bool AvHPistol::GetHasMuzzleFlash() const
{
	return true;
}

bool AvHPistol::GetMustPressTriggerForEachShot() const
{
	return true;
}

float AvHPistol::GetReloadTime(void) const
{
	return 3.0f;
}

void AvHPistol::Precache()
{
	AvHMarineWeapon::Precache();

	PRECACHE_UNMODIFIED_MODEL(kHGEjectModel);
	PRECACHE_UNMODIFIED_SOUND(kHGFireSound1);
	PRECACHE_UNMODIFIED_SOUND(kHGReloadSound);
	PRECACHE_UNMODIFIED_MODEL(kGenericWallpuff);

	this->mEvent = PRECACHE_EVENT(1, kHGEventName);
}


void AvHPistol::Spawn()
{
    AvHMarineWeapon::Spawn();

	Precache();

	this->m_iId = AVH_WEAPON_PISTOL;
	this->m_iDefaultAmmo = BALANCE_VAR(kHGMaxClip)*(BALANCE_VAR(kMarineSpawnClips) + 1);

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsPistol);

	SET_MODEL(ENT(this->pev), kHGWModel);

	FallInit();// get ready to fall down.
}

