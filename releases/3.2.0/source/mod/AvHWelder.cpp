//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHWelder.cpp $
// $Date: 2002/11/22 21:28:17 $
//
//-------------------------------------------------------------------------------
// $Log: AvHWelder.cpp,v $
// Revision 1.23  2002/11/22 21:28:17  Flayra
// - mp_consistency changes
//
// Revision 1.22  2002/11/12 02:29:48  Flayra
// - Change inflictor for better logging
//
// Revision 1.21  2002/11/06 01:39:26  Flayra
// - Damage refactoring (TakeDamage assumes caller has already adjusted for friendly fire, etc.)
//
// Revision 1.20  2002/10/03 18:47:40  Flayra
// - Added heavy view model
//
// Revision 1.19  2002/09/23 22:37:04  Flayra
// - Fixed bug (typo?!) where welder was removing jetpacks (and removing marine status!)
//
// Revision 1.18  2002/08/16 02:49:45  Flayra
// - Updates to allow welder to repair buildables
// - Removed ability to repair soldier armor until it's visible
//
// Revision 1.17  2002/07/26 23:03:08  Flayra
// - New artwork
//
// Revision 1.16  2002/06/25 17:50:31  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.15  2002/06/03 16:39:10  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.14  2002/05/28 17:44:26  Flayra
// - Tweak weapon deploy volume, as Valve's sounds weren't normalized, welder clears webs now, welder no longer makes ambient hum, slight refactoring of variable names away from Valve style
//
// Revision 1.13  2002/05/23 02:32:40  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHMarineWeapons.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHWeldable.h"
#include "mod/AvHSpecials.h"

#ifdef AVH_SERVER
#include "mod/AvHPlayerUpgrade.h"
#include "AvHServerUtil.h"
#include "mod/AvHPlayer.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHGamerules.h"

extern int	gWelderConstEventID;
#endif

int AvHWelder::GetDeployAnimation() const
{
	return 3;
}

float AvHWelder::GetDeployTime() const
{
	return .55f;
}

char* AvHWelder::GetHeavyViewModel() const
{
	return kWelderHVVModel;
}

char* AvHWelder::GetPlayerModel() const
{
	return kWelderPModel;
}

int	AvHWelder::GetShootAnimation() const
{
	return 2;
}

char* AvHWelder::GetViewModel() const
{
	return kWelderVModel;
}

char* AvHWelder::GetWorldModel() const
{
	return kWelderWModel;
}

void AvHWelder::FireProjectiles(void)
{
#ifdef AVH_SERVER
	
	Vector theWelderBarrel = this->GetWorldBarrelPoint();

	UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);
	Vector vecEnd = theWelderBarrel + gpGlobals->v_forward*BALANCE_VAR(kWelderRange);

	TraceResult tr;
	UTIL_TraceLine(theWelderBarrel, vecEnd, dont_ignore_monsters, dont_ignore_glass, ENT( m_pPlayer->pev ), &tr);
	float theROF = this->GetRateOfFire();
	bool theDidWeld = false;

	if(tr.flFraction != 1.0f)
	{
		// If it's a weldable, check if it's valid for construction
		CBaseEntity* theEntity = CBaseEntity::Instance(tr.pHit);
		AvHWeldable* theWeldable = dynamic_cast<AvHWeldable*>(theEntity);
		if(theWeldable)
		{
			if(theWeldable->GetCanBeWelded())
			{
				// Build it by the amount of our rate of fire
				theWeldable->AddBuildTime(theROF);

				AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
				ASSERT(thePlayer);
				thePlayer->TriggerProgressBar(theWeldable->entindex(), 1);
				theDidWeld = true;
			}
		}
		// else it's not a weldable, apply damage to it
		else
		{
			if(this->m_pPlayer->pev->team == theEntity->pev->team)
			{
				if( this->RepairTarget(theEntity, theROF) )
					theDidWeld = true;
			}
			else
			{
				// Apply damage to it and playback other event
				float theScalar = 1.0f;
				if(GetGameRules()->CanEntityDoDamageTo(this->m_pPlayer, theEntity, &theScalar))
				{
					float theDamageMultiplier;
					AvHPlayerUpgrade::GetWeaponUpgrade(this->m_pPlayer->pev->iuser3, this->m_pPlayer->pev->iuser4, &theDamageMultiplier);
					float theDamage = this->mDamage*theDamageMultiplier*theScalar;
					
					theEntity->TakeDamage(this->pev, this->m_pPlayer->pev, theDamage, DMG_BURN);
				}
			}
		}


		if(!theDidWeld)
		{
			if(this->GetIsWelding())
			{
				PLAYBACK_EVENT_FULL(0, this->m_pPlayer->edict(), gWelderConstEventID, 0, this->m_pPlayer->pev->origin, (float *)&g_vecZero, 0.0, 0.0, 1, 0, 0, 0 );
				this->SetIsWelding(false);
			}
		}
		else
		{
			if(!this->GetIsWelding())
			{
				PLAYBACK_EVENT_FULL(0, this->m_pPlayer->edict(), gWelderConstEventID, 0, this->m_pPlayer->pev->origin, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
				this->SetIsWelding(true);
			}
		}
	}
	else
	{
		if(this->GetIsWelding())
		{
			PLAYBACK_EVENT_FULL(0, this->m_pPlayer->edict(), gWelderConstEventID, 0, this->m_pPlayer->pev->origin, (float *)&g_vecZero, 0.0, 0.0, 1, 0, 0, 0 );
			this->SetIsWelding(false);
		}
	}

	// Scan area for webs, and clear them.  I can't make the webs solid, and it seems like the welder might do this, so why not?  Also
	// adds neat element of specialization where a guy with a welder might be needed to clear an area before an attack, kinda RPS
	const float kWebClearingRadius = 75;
	CBaseEntity* thePotentialWebStrand = NULL;
	while((thePotentialWebStrand = UTIL_FindEntityInSphere(thePotentialWebStrand, theWelderBarrel, kWebClearingRadius)) != NULL)
	{
		AvHWebStrand* theWebStrand = dynamic_cast<AvHWebStrand*>(thePotentialWebStrand);
		if(theWebStrand)
		{
			theWebStrand->Break();
		}
	}

	#endif
}

int	AvHWelder::GetBarrelLength() const
{
	// Note that this variable is used in EV_Welder
	return kWelderBarrelLength; 
}

float AvHWelder::GetRateOfFire() const
{
	return BALANCE_VAR(kWelderROF);
}

void AvHWelder::Holster( int skiplocal )
{
	AvHMarineWeapon::Holster(skiplocal);

#ifdef AVH_SERVER
	if(this->GetIsWelding()) 
	{
		PLAYBACK_EVENT_FULL(0, this->m_pPlayer->edict(), gWelderConstEventID, 0, this->m_pPlayer->pev->origin, (float *)&g_vecZero, 0.0, 0.0, 1, 0, 0, 0 );
		this->SetIsWelding(false);
	}
#endif

	PLAYBACK_EVENT_FULL(0, this->m_pPlayer->edict(), this->mEndEvent, 0, this->m_pPlayer->pev->origin, (float *)&g_vecZero, 0.0, 0.0, 0, 0, 0, 0 );
	this->PlaybackEvent(this->mEndEvent);
}

void AvHWelder::WeaponIdle(void)
{
#ifdef AVH_SERVER
	if(this->GetIsWelding() && this->mAttackButtonDownLastFrame) 
	{
		PLAYBACK_EVENT_FULL(0, this->m_pPlayer->edict(), gWelderConstEventID, 0, this->m_pPlayer->pev->origin, (float *)&g_vecZero, 0.0, 0.0, 1, 0, 0, 0 );
		this->SetIsWelding(false);
	}
#endif

	AvHMarineWeapon::WeaponIdle();
}


void AvHWelder::Init()
{
	this->mRange = BALANCE_VAR(kWelderRange);
	this->mDamage = BALANCE_VAR(kWelderDamage);
	this->SetIsWelding(false);
}

BOOL AvHWelder::IsUseable(void)
{
	// No ammo, always useable
	return TRUE;
}

void AvHWelder::Precache()
{
	AvHMarineWeapon::Precache();
	
	PRECACHE_UNMODIFIED_SOUND(kWeldingSound);
	PRECACHE_UNMODIFIED_SOUND(kWeldingHitSound);
	PRECACHE_UNMODIFIED_SOUND(kWeldingStopSound);
	
	this->mEvent = PRECACHE_EVENT(1, kWelderEventName);
	PRECACHE_EVENT(1, kWelderConstEventName);
	
	this->mStartEvent = PRECACHE_EVENT(1, kWelderStartEventName);
	this->mEndEvent = PRECACHE_EVENT(1, kWelderEndEventName);
}

#ifdef AVH_SERVER

bool AvHWelder::RepairTarget(CBaseEntity* inEntity, float inROF)
{
    int theAmountToRepair = inROF*BALANCE_VAR(kWelderRepairRate);
	
	bool theReturn = false;

	if(inEntity)
	{
		AvHPlayer* theHitPlayer = dynamic_cast<AvHPlayer*>(inEntity);

		if(theHitPlayer)
		{
			// Repair armor if possible
			int theCurrentArmor = theHitPlayer->pev->armorvalue;
			int theMaxArmor = AvHPlayerUpgrade::GetMaxArmorLevel(theHitPlayer->pev->iuser4, theHitPlayer->GetUser3());
			
			if(theCurrentArmor < theMaxArmor)
			{
				int theNewArmor = theCurrentArmor + (theAmountToRepair * BALANCE_VAR(kWelderPlayerModifier));
				theHitPlayer->pev->armorvalue = min(theMaxArmor, theNewArmor);
				theReturn = true;
			}
            
            // Burn off webs
            if(theHitPlayer->GetIsEnsnared())
            {
                theHitPlayer->SetEnsnareState(false);
				theReturn = true;
            }
		}
		else //if(GetHasUpgrade(inEntity->pev->iuser4, MASK_BUILDABLE))
		{
			// Max health in armorvalue
			AvHBaseBuildable* theBuildable = dynamic_cast<AvHBaseBuildable*>(inEntity);
			if(theBuildable)
			{
				if(theBuildable->Regenerate((theAmountToRepair * BALANCE_VAR(kWelderBuildingModifier)), false))
                {
                    // Award experience for welding the CC.  Might award a little more if barely wounded, but that seems OK.
                    if(GetGameRules()->GetIsCombatMode() && (theBuildable->pev->iuser3 == AVH_USER3_COMMANDER_STATION))
                    {
                        AvHPlayer* theWeldingPlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
                        if(theWeldingPlayer && (theWeldingPlayer->pev->team == theBuildable->pev->team))
                        {
                            float theCombatHealExperienceScalar = BALANCE_VAR(kCombatHealExperienceScalar);
                            theWeldingPlayer->AwardExperienceForObjective(theAmountToRepair*theCombatHealExperienceScalar, theBuildable->GetMessageID());
                        }
                    }
					theReturn = true;
                }
			}
		}
	}
	return theReturn;
}
#endif

void AvHWelder::Spawn() 
{ 
    AvHMarineWeapon::Spawn(); 

	Precache();

	this->m_iId = AVH_WEAPON_WELDER;
	//this->m_iDefaultAmmo = kWelderMaxClip;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsWelder);

	SET_MODEL(ENT(this->pev), kWelderWModel);

	FallInit();// get ready to fall down.
} 

// When welder is deployed, it makes the welding sound
void AvHWelder::WelderThink()
{
}

bool AvHWelder::UsesAmmo(void) const
{
	return false;
}
