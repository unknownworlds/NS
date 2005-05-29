//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHPrimalScream.cpp $
// $Date: 2002/11/22 21:28:17 $
//
//-------------------------------------------------------------------------------
// $Log: AvHPrimalScream.cpp,v $
// Revision 1.9  2002/11/22 21:28:17  Flayra
// - mp_consistency changes
//
// Revision 1.8  2002/07/24 19:09:17  Flayra
// - Linux issues
//
// Revision 1.7  2002/07/24 18:55:52  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.6  2002/07/24 18:45:42  Flayra
// - Linux and scripting changes
//
// Revision 1.5  2002/07/01 21:44:16  Flayra
// - Refactoring
//
// Revision 1.4  2002/06/25 18:14:54  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
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
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHPlayerUpgrade.h"

#ifdef AVH_SERVER
#include "mod/AvHGamerules.h"
#include "mod/AvHServerUtil.h"
#endif

#include "mod/AvHConstants.h"

LINK_ENTITY_TO_CLASS(kwPrimalScream, AvHPrimalScream);

int	AvHPrimalScream::GetBarrelLength() const
{
	return kPrimalScreamBarrelLength;
}

float AvHPrimalScream::GetRateOfFire() const
{
	return BALANCE_FVAR(kPrimalScreamROF);
}

bool AvHPrimalScream::GetFiresUnderwater() const
{
	return true;
}

int	AvHPrimalScream::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
	int theDeployAnimation = -1;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
	case AVH_WEAPON_BITE2:
		theDeployAnimation = 9;
		break;
	case AVH_WEAPON_SPIKE:
		theDeployAnimation = 8;
		break;
	}
	
	return theDeployAnimation;
}

float AvHPrimalScream::GetDeployTime() const
{
	return .6f;
}

int	AvHPrimalScream::GetIdleAnimation() const
{
	return 2;
}

int	AvHPrimalScream::GetShootAnimation() const
{
	return 6;
}

bool AvHPrimalScream::GetIsDroppable() const
{
	return false;
}
	
void AvHPrimalScream::Init()
{
	this->mRange = BALANCE_IVAR(kPrimalScreamRange);
}

char* AvHPrimalScream::GetViewModel() const
{
	return kLevel3ViewModel;
}


void AvHPrimalScream::Precache(void)
{
	AvHAlienWeapon::Precache();
	
	PRECACHE_UNMODIFIED_SOUND(kPrimalScreamSound);
	
	this->mEvent = PRECACHE_EVENT(1, kPrimalScreamShootEventName);
}

void AvHPrimalScream::Spawn()
{
    AvHAlienWeapon::Spawn(); 
	
	Precache();
	
	this->m_iId = AVH_WEAPON_PRIMALSCREAM;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsPrimalScream);
	
	SET_MODEL(ENT(this->pev), kNullModel);
	
	FallInit();// get ready to fall down.
	
}

bool AvHPrimalScream::UsesAmmo(void) const
{
	return false;
}

void AvHPrimalScream::FireProjectiles(void)
{
	#ifdef AVH_SERVER
	
	// Make sure we have enough points to shoot this thing
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
	ASSERT(thePlayer);

	thePlayer->StartScreaming();
				
	#endif	
}

