//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHBite2.cpp $
// $Date: 2002/08/16 02:33:13 $
//
//-------------------------------------------------------------------------------
// $Log: AvHBite2.cpp,v $
// Revision 1.3  2002/08/16 02:33:13  Flayra
// - Added damage types
//
// Revision 1.2  2002/07/24 18:45:40  Flayra
// - Linux and scripting changes
//
// Revision 1.1  2002/06/25 17:24:09  Flayra
// - Level 3 bite attack
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
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHConstants.h"

LINK_ENTITY_TO_CLASS(kwBite2Gun, AvHBite2);
void V_PunchAxis( int axis, float punch );

void AvHBite2::Init()
{
	this->mRange = kBite2Range;
	this->mDamage = BALANCE_VAR(kBite2Damage);
}

bool AvHBite2::GetIsGunPositionValid() const
{
    return true;
}

int	AvHBite2::GetBarrelLength() const
{
	return kBite2BarrelLength;
}
    
float AvHBite2::GetRateOfFire() const
{
	return BALANCE_VAR(kBite2ROF);
}

int	AvHBite2::GetDeployAnimation() const
{
	// Look at most recently used weapon and see if we can transition from it
	int theDeployAnimation = 1;

	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();

	switch(thePreviousID)
	{
	case AVH_WEAPON_UMBRA:
	case AVH_WEAPON_SPORES:
	//case AVH_WEAPON_PRIMALSCREAM:
		theDeployAnimation = 12;
		break;
	}

	return theDeployAnimation;
}

int AvHBite2::GetDamageType() const
{
	return AvHBite::GetDamageType();
}

float AvHBite2::GetDeployTime() const
{
	return .2f;
}

int	AvHBite2::GetIdleAnimation() const
{
	return 1;
}

int	AvHBite2::GetShootAnimation() const
{
	return 4;
}

char* AvHBite2::GetViewModel() const
{
	return kLevel3ViewModel;
}

void AvHBite2::Precache()
{
	AvHBite::Precache();
	
	this->mEvent = PRECACHE_EVENT(1, kBite2EventName);
}

void AvHBite2::Spawn() 
{ 
    AvHBite::Spawn(); 
	
	this->Precache();
	
	this->m_iId = AVH_WEAPON_BITE2;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsBite2Gun);
	
	SET_MODEL(ENT(this->pev), kNullModel);
	
	FallInit();// get ready to fall down.
} 



