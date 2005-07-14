//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHHealingSpray.cpp $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHHealingSpray.cpp,v $
// Revision 1.11  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.10  2002/11/12 02:25:14  Flayra
// - Healing spray no longer heals unbuilt structures
//
// Revision 1.9  2002/10/03 18:48:16  Flayra
// - Healing spray only damages players
//
// Revision 1.8  2002/09/23 22:16:13  Flayra
// - Healing spray no longer goes through walls
//
// Revision 1.7  2002/09/09 19:51:44  Flayra
// - Healing spray heals players more generically, playing sound when effective
//
// Revision 1.6  2002/08/31 18:01:01  Flayra
// - Work at VALVe
//
// Revision 1.5  2002/08/16 02:35:35  Flayra
// - New damage types
// - Healing spray can affect enemy non-player organics (buildings, babblers)
//
// Revision 1.4  2002/07/24 19:09:16  Flayra
// - Linux issues
//
// Revision 1.3  2002/07/24 18:45:41  Flayra
// - Linux and scripting changes
//
// Revision 1.2  2002/07/23 17:04:06  Flayra
// - Spray only hurts players (prevents builders from easily taking out mines)
//
// Revision 1.1  2002/06/25 17:24:29  Flayra
// - Level 2 healing ability
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

#ifdef AVH_SERVER
#include "mod/AvHBaseBuildable.h"
#include "mod/AvHGamerules.h"
#endif

#include "common/hldm.h"
#include "common/event_api.h"
#include "common/event_args.h"
#include "common/vector_util.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHConstants.h"
#include "mod/AvHSharedUtil.h"

LINK_ENTITY_TO_CLASS(kwHealingSpray, AvHHealingSpray);

void AvHHealingSpray::Init()
{
	this->mRange = kHealingSprayRange;
	this->mDamage = BALANCE_VAR(kHealingSprayDamage);
}

int	AvHHealingSpray::GetBarrelLength() const
{
	return kHealingSprayBarrelLength;
}

float AvHHealingSpray::GetRateOfFire() const
{
	return BALANCE_VAR(kHealingSprayROF);
}

int	AvHHealingSpray::GetDamageType() const
{
//	return NS_DMG_ORGANIC;
	return NS_DMG_NORMAL;
}

int	AvHHealingSpray::GetDeployAnimation() const
{
	int theDeployAnimation = -1;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
		// Spit and bile bomb look the same
	case AVH_WEAPON_SPIT:
	case AVH_WEAPON_BILEBOMB:
		theDeployAnimation = 5;
		break;
	}
	
	return theDeployAnimation;
}

int	AvHHealingSpray::GetIdleAnimation() const
{
	return 2;
}

int	AvHHealingSpray::GetShootAnimation() const
{
	return 4;
}

char* AvHHealingSpray::GetViewModel() const
{
	return kLevel2ViewModel;
}

void AvHHealingSpray::Precache()
{
	AvHAlienWeapon::Precache();

	PRECACHE_UNMODIFIED_SOUND(kHealingSpraySound);
	
	this->mEvent = PRECACHE_EVENT(1, kHealingSprayEventName);
}


void AvHHealingSpray::Spawn() 
{ 
    AvHAlienWeapon::Spawn(); 
	
	this->Precache();
	
	this->m_iId = AVH_WEAPON_HEALINGSPRAY;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsHealingSpray);
	
	SET_MODEL(ENT(this->pev), kNullModel);
	
	FallInit();// get ready to fall down.
} 


void AvHHealingSpray::FireProjectiles(void)
{
	#ifdef AVH_SERVER
	
	// Look for entities in cone
	CBaseEntity* theCurrentEntity = NULL;
	vec3_t theOriginatingPosition = this->m_pPlayer->GetGunPosition();

	while((theCurrentEntity = UTIL_FindEntityInSphere(theCurrentEntity, theOriginatingPosition, kHealingSprayRange)) != NULL)
	{
		// Can't affect self
		if(theCurrentEntity != this->m_pPlayer)
		{
			// If entity is in view cone, and within range
			if(this->m_pPlayer->FInViewCone(&theCurrentEntity->pev->origin))
			{
				// UTIL_FindEntityInSphere doesn't seem to take height into account.  Make sure the entity is within range.
                float theMaxEntitySize = max(Length(theCurrentEntity->pev->mins), Length(theCurrentEntity->pev->maxs));

                vec3_t theVectorDiff;
                VectorSubtract(theCurrentEntity->pev->origin, theOriginatingPosition, theVectorDiff);
				float theDiff = Length(theVectorDiff) - theMaxEntitySize;

                if(theDiff < kHealingSprayRange/2)
				{
					// Make sure entity is in line of fire
					TraceResult tr;
					UTIL_TraceLine(theOriginatingPosition, theCurrentEntity->Center(), dont_ignore_monsters, dont_ignore_glass, ENT(pev)/*pentIgnore*/, &tr);

					CBaseEntity* theBlockedByEntity = CBaseEntity::Instance(tr.pHit);

					if((tr.flFraction == 1.0) || (theBlockedByEntity == theCurrentEntity))
					{
						// Heal friendly player or building in range 
                        float theFocusAmount = 1.0f;
                        if(AvHSHUGetIsWeaponFocusable(AvHWeaponID(this->m_iId)))
                        {
                            theFocusAmount = AvHPlayerUpgrade::GetFocusDamageUpgrade(this->m_pPlayer->pev->iuser4);
                        }
                        
						float theDamage = this->mDamage*theFocusAmount;
						AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theCurrentEntity);
						
						if(theCurrentEntity->pev->team == this->m_pPlayer->pev->team)
						{
							// Players and buildables heal armor too
							AvHBaseBuildable* theBuildable = dynamic_cast<AvHBaseBuildable*>(theCurrentEntity);
							const int theBuildableHealingSprayScalar = BALANCE_VAR(kHealingSprayBuildableScalar);
							if(thePlayer)
							{
								// Players heal by base amount, plus percentage of health
								float thePercentage = BALANCE_VAR(kHealingSprayPlayerPercent)/100.0f;
								theDamage += thePercentage*theCurrentEntity->pev->max_health;
								thePlayer->Heal(theDamage, true);
							}
							else if(theBuildable)
							{
								// Structures heal base amount times scalar
                                float theAmount = theDamage*(float)theBuildableHealingSprayScalar;
								if(theBuildable->Regenerate(theAmount, true))
                                {
                                    // Award experience for healing the hive.  Might award a little more if barely wounded, but that seems OK.
                                    if(GetGameRules()->GetIsCombatMode() && (theBuildable->pev->iuser3 == AVH_USER3_HIVE))
                                    {
                                        AvHPlayer* theHealsprayingPlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
                                        if(theHealsprayingPlayer && (theHealsprayingPlayer->pev->team == theBuildable->pev->team))
                                        {
                                            float theCombatHealExperienceScalar = BALANCE_VAR(kCombatHealExperienceScalar);
                                            theHealsprayingPlayer->AwardExperienceForObjective(theAmount*theCombatHealExperienceScalar, theBuildable->GetMessageID());
                                        }
                                    }
                                }
							}
							else
							{
								theCurrentEntity->TakeHealth(theDamage, this->GetDamageType());
							}
						}
						else if(thePlayer)
						{
							thePlayer->TakeDamage(this->pev, this->m_pPlayer->pev, theDamage, this->GetDamageType());
						}
					}
				}
			}
		}
	}
	
	#endif	
}

