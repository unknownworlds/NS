//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHCocoon.cpp $
// $Date: 2002/07/24 19:09:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHCocoon.cpp,v $
// Revision 1.6  2002/07/24 19:09:16  Flayra
// - Linux issues
//
// Revision 1.5  2002/07/24 18:55:51  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.4  2002/07/24 18:45:41  Flayra
// - Linux and scripting changes
//
// Revision 1.3  2002/06/03 16:39:09  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.2  2002/05/23 02:34:00  Flayra
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
#include "mod/AvHGamerules.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHConstants.h"

#ifdef AVH_SERVER
#include "mod/AvHEntities.h"
#include "mod/AvHGamerules.h"
#endif

LINK_ENTITY_TO_CLASS(kwCocoon, AvHCocoon);

BOOL AvHCocoon::Deploy()
{
	return DefaultDeploy(kCocoonVModel, kCocoonPModel, this->GetDeployAnimation(), kCocoonAnimExt);
}

int	AvHCocoon::GetBarrelLength() const
{
	return kCocoonBarrelLength;
}

bool AvHCocoon::GetFiresUnderwater() const
{
	return true;
}

bool AvHCocoon::GetIsDroppable() const
{
	return false;
}
	
void AvHCocoon::Init()
{
	this->mRange = kCocoonRange;
	this->mROF = kCocoonROF;
}

void AvHCocoon::Precache(void)
{
	AvHAlienWeapon::Precache();
	
	PRECACHE_MODEL(kCocoonVModel);
	PRECACHE_MODEL(kCocoonPModel);
	PRECACHE_MODEL(kAlienGunWModel);

	PRECACHE_SOUND(kCocoonSound1);
	PRECACHE_SOUND(kCocoonSound2);
	
	this->mEvent = PRECACHE_EVENT(1, kCocoonShootEventName);
}

void AvHCocoon::Spawn()
{
    AvHAlienWeapon::Spawn(); 
	
	Precache();
	
	this->m_iId = -1;//AVH_WEAPON_COCOON;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsCocoon);
	
	SET_MODEL(ENT(this->pev), kAlienGunWModel);
	
	FallInit();// get ready to fall down.
	
}

bool AvHCocoon::UsesAmmo(void) const
{
	return false;
}

void AvHCocoon::FireProjectiles(void)
{
#ifdef AVH_SERVER
	
	// Make sure we have enough points to shoot this thing
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
	ASSERT(thePlayer);
	
	vec3_t theNewPoint = this->pev->origin;
	TraceResult	tr;
	
	// Do a traceline to see who to cocoon
	UTIL_MakeVectors(this->pev->angles);

	Vector theForwardDir = (gpGlobals->v_forward + gpGlobals->v_right + gpGlobals->v_up);
	UTIL_TraceLine(this->pev->origin + this->pev->view_ofs, this->pev->origin + this->pev->view_ofs + theForwardDir*128, dont_ignore_monsters, ENT(this->m_pPlayer->pev), &tr);
	if(tr.flFraction != 1.0)
	{
		CBaseEntity* theEntity = CBaseEntity::Instance(tr.pHit);
		if(theEntity)
		{
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
			if(thePlayer /*&& (thePlayer->pev->team != this->pev->team)*/ && (thePlayer->GetRole() != ROLE_GESTATING) && (thePlayer->GetRole() != ROLE_COCOONED))
			{
				GetGameRules()->MarkDramaticEvent(kCocoonPriority, thePlayer, this->m_pPlayer);

				// Play successful cocoon sound!
				EMIT_SOUND_DYN(ENT(pev), CHAN_WEAPON, kCocoonSound2, 1.0, ATTN_NORM, 0, 100 + RANDOM_LONG(-3,3) );
				
				// Yes!
				thePlayer->StartCocooning();
			}
		}
	}
				
	#endif	
}

