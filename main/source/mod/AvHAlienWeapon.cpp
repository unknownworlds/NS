//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAlienWeapon.cpp $
// $Date: 2002/10/03 18:37:32 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAlienWeapon.cpp,v $
// Revision 1.9  2002/10/03 18:37:32  Flayra
// - Moved alien energy to fuser3
//
// Revision 1.8  2002/09/23 22:08:59  Flayra
// - Updates to allow marine weapons stick around
//
// Revision 1.7  2002/08/31 18:01:00  Flayra
// - Work at VALVe
//
// Revision 1.6  2002/08/16 02:32:09  Flayra
// - Added damage types
// - Swapped umbra and bile bomb
//
// Revision 1.5  2002/07/08 16:43:26  Flayra
// - Web change?
//
// Revision 1.4  2002/06/25 17:29:56  Flayra
// - Better default behavior
//
// Revision 1.3  2002/06/03 16:25:10  Flayra
// - Switch alien weapons quickly, renamed CheckValidAttack()
//
// Revision 1.2  2002/05/28 17:37:14  Flayra
// - Reduced times where alien weapons are dropped and picked up again (but didn't eliminate, they are still being instantiated somewhere)
//
// Revision 1.1  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHAlienWeapon.h"
#include "mod/AvHMovementUtil.h"
#include "mod/AvHSpecials.h"

#ifdef AVH_CLIENT
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
extern int g_runfuncs;
#include "cl_dll/com_weapons.h"

#include "pm_defs.h"
#include "pm_shared.h"
#include "pm_movevars.h"
extern playermove_t *pmove;
#endif

#include "mod/AvHSharedUtil.h"

AvHAlienWeapon::AvHAlienWeapon()
{
	#ifdef AVH_SERVER
	this->SetGroundLifetime(0);
	#endif
}

bool AvHAlienWeapon::ProcessValidAttack(void)
{
	bool theCanAttack = false;
	
	if(AvHBasePlayerWeapon::ProcessValidAttack())
	{
		if(this->IsUseable())
		{
			theCanAttack = true;
		}
	}
	
	return theCanAttack;
}

void AvHAlienWeapon::DeductCostForShot(void)
{
	if(this->GetTakesEnergy())
	{
		AvHBasePlayerWeapon::DeductCostForShot();

		float theEnergyCost = this->GetEnergyForAttack();
		float& theFuser = this->GetEnergyLevel();
		AvHMUDeductAlienEnergy(theFuser, theEnergyCost);
	}
}

// Cannot ever drop alien weapons
void AvHAlienWeapon::Drop(void) 
{
	int a = 0;
}

bool AvHAlienWeapon::GetAllowedForUser3(AvHUser3 inUser3)
{
	bool theAllowed = false;
	
	// Alien weapons for aliens.  Don't take into account exact roles until needed (and until weapons have stabilized)
	switch(inUser3)
	{
	case AVH_USER3_ALIEN_PLAYER1:
	case AVH_USER3_ALIEN_PLAYER2:
	case AVH_USER3_ALIEN_PLAYER3:
	case AVH_USER3_ALIEN_PLAYER4:
	case AVH_USER3_ALIEN_PLAYER5:
		theAllowed = true;
		break;
	}
	
	return theAllowed;
}

int	AvHAlienWeapon::GetDamageType() const
{
	// Assume melee attack, piercing
	//return NS_DMG_PIERCING;
	return NS_DMG_NORMAL;
}

float AvHAlienWeapon::GetDeployTime() const
{
	// Aliens are Quake-style/arcadey, make weapon switch fast
	return .1f;
}

float AvHAlienWeapon::GetEnergyForAttack() const
{
	float theEnergy = 0.0f;

	AvHMUGetEnergyCost((AvHWeaponID)(this->m_iId), theEnergy);

	return theEnergy;
}

bool AvHAlienWeapon::GetFiresUnderwater() const
{
	return true;
}

// Never live on ground
int	AvHAlienWeapon::GetGroundLifetime() const
{
	return 0;
}

float& AvHAlienWeapon::GetEnergyLevel()
{
#ifdef AVH_CLIENT
	float& theFuser = pmove->fuser3;
#endif
	
#ifdef AVH_SERVER
	float& theFuser = this->m_pPlayer->pev->fuser3;
#endif
	
	return theFuser;
}

bool AvHAlienWeapon::GetIsDroppable() const
{
	return false;
}

bool AvHAlienWeapon::GetIsGunPositionValid() const
{
    // Check to make sure our gun barrel isn't coming out on the other side of a wall.
    
    vec3_t theBarrelOrigin;
    vec3_t theBarrelEnd;
    
#ifdef AVH_CLIENT
    
    extern vec3_t v_view_ofs; // In view.cpp
    extern vec3_t v_cl_angles;
    
    cl_entity_s* thePlayer = gHUD.GetVisiblePlayer();
    
    VectorAdd(thePlayer->curstate.origin, v_view_ofs, theBarrelOrigin);
    
    vec3_t theBarrelDirection;
    AngleVectors(v_angles, theBarrelDirection, NULL, NULL);
    
    VectorMA(theBarrelOrigin, GetBarrelLength(), theBarrelDirection, theBarrelEnd);
    
#endif
    
#ifdef AVH_SERVER
    
    VectorCopy(m_pPlayer->GetGunPosition(), theBarrelOrigin);
    VectorCopy(GetWorldBarrelPoint(), theBarrelEnd);
    
#endif
    
    return AvHTraceLineAgainstWorld(theBarrelOrigin, theBarrelEnd) == 1;
}

float AvHAlienWeapon::ComputeAttackInterval() const
{
	float theROF = this->GetRateOfFire();
	
	int theUser4 = this->m_pPlayer->pev->iuser4;
	
	#ifdef AVH_CLIENT
	cl_entity_t* theLocalPlayer = gEngfuncs.GetLocalPlayer();
	if(theLocalPlayer)
	{
		theUser4 = theLocalPlayer->curstate.iuser4;
	}
	#endif

	// Speed attack if in range of primal scream
	if(GetHasUpgrade(theUser4, MASK_BUFFED))
	{
		float thePrimalScreamROFFactor = 1.0f + BALANCE_FVAR(kPrimalScreamROFFactor);
		theROF /= thePrimalScreamROFFactor;
	}

	// Check for focus
    if(AvHSHUGetIsWeaponFocusable(AvHWeaponID(this->m_iId)))
    {
	    int theFocusLevel = AvHGetAlienUpgradeLevel(theUser4, MASK_UPGRADE_8);
	    if(theFocusLevel > 0)
	    {
		    // Slow down ROF by x% for each level
		    const float theFocusROFPercentSlowdownPerLevel = BALANCE_FVAR(kFocusROFPercentSlowdownPerLevel);
		    float theSlowDownPercentage = 1.0f + theFocusLevel*theFocusROFPercentSlowdownPerLevel;
		    
		    theROF *= theSlowDownPercentage;
	    }
    }

	return theROF;
}

void AvHAlienWeapon::Precache(void)
{
	AvHBasePlayerWeapon::Precache();
}

void AvHAlienWeapon::Spawn()
{
	AvHBasePlayerWeapon::Spawn();
	
	this->pev->iuser3 = AVH_USER3_NONE;
	
	#ifdef AVH_SERVER
	this->SetGroundLifetime(0);
	#endif
}


BOOL AvHAlienWeapon::IsUseable(void)
{
	BOOL theIsUseable = FALSE;

	// Make sure we have enough energy for this attack
	float theEnergyCost = this->GetEnergyForAttack();
	float& theFuser = this->GetEnergyLevel();
	
	if(AvHMUHasEnoughAlienEnergy(theFuser, theEnergyCost) && this->GetEnabledState())
	{
		theIsUseable = TRUE;
	}

	return theIsUseable;
}

bool AvHAlienWeapon::UsesAmmo(void) const
{
	return false;
}

BOOL AvHAlienWeapon::UseDecrement(void)
{
	return true;
}

