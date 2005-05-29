//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHParasiteGun.cpp $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHParasiteGun.cpp,v $
// Revision 1.15  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.14  2002/11/12 02:25:51  Flayra
// - Parasite no longer does damage to teammates improperly
//
// Revision 1.13  2002/11/06 01:38:37  Flayra
// - Added ability for buildings to be enabled and disabled, for turrets to be shut down
// - Damage refactoring (TakeDamage assumes caller has already adjusted for friendly fire, etc.)
//
// Revision 1.12  2002/09/09 20:00:32  Flayra
// - Parasite works on buildings now
// - Parasite no longer changes score
//
// Revision 1.11  2002/08/31 18:01:02  Flayra
// - Work at VALVe
//
// Revision 1.10  2002/08/16 02:40:36  Flayra
// - Damage types
// - Parasite can now affect organic enemy targets (babblers, alien buildings)
//
// Revision 1.9  2002/07/24 19:09:17  Flayra
// - Linux issues
//
// Revision 1.8  2002/07/24 18:55:52  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.7  2002/07/24 18:45:42  Flayra
// - Linux and scripting changes
//
// Revision 1.6  2002/07/01 21:19:29  Flayra
// - Removed hard-coded parasite damage
//
// Revision 1.5  2002/06/25 17:50:59  Flayra
// - Reworking for correct player animations and new enable/disable state, new view model artwork, alien weapon refactoring
//
// Revision 1.4  2002/06/10 19:49:06  Flayra
// - Updated with new alien view model artwork (with running anims)
//
// Revision 1.3  2002/06/03 16:27:06  Flayra
// - Animation constants and changes with new artwork
//
// Revision 1.2  2002/05/28 17:58:08  Flayra
// - Parasite works properly in tournament mode now
//
// Revision 1.1  2002/05/23 02:33:20  Flayra
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
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHPlayerUpgrade.h"

#ifdef AVH_SERVER
#include "mod/AvHGamerules.h"
#endif

LINK_ENTITY_TO_CLASS(kwParasiteGun, AvHParasiteGun);
extern int gParasiteStartEventID;

void AvHParasiteGun::Init()
{
	this->mRange = kParasiteRange;
	this->mDamage = BALANCE_IVAR(kParasiteDamage);
}

int	AvHParasiteGun::GetBarrelLength() const
{
	return kParasiteBarrelLength;
}

float AvHParasiteGun::GetRateOfFire() const
{
    return BALANCE_FVAR(kParasiteROF);
}

int AvHParasiteGun::GetDamageType() const
{
	return NS_DMG_ORGANIC;
	//return NS_DMG_NORMAL;
}

int	AvHParasiteGun::GetDeployAnimation() const
{
	return 6;
}

bool AvHParasiteGun::GetFiresUnderwater() const
{
	return true;
}

bool AvHParasiteGun::GetIsDroppable() const
{
	return false;
}

int AvHParasiteGun::GetShootAnimation() const
{
	return -1;
}

void AvHParasiteGun::FireProjectiles(void)
{
	#ifdef AVH_SERVER
	
	UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);
	
	Vector vecAiming = gpGlobals->v_forward;
	Vector vecSrc = this->m_pPlayer->GetGunPosition( ) + vecAiming;
	Vector vecEnd = vecSrc + vecAiming*kParasiteRange;
	
	// Ignore damage upgrades
	//int theTracerFreq;
	//float theDamageMultiplier;
	//AvHPlayerUpgrade::GetWeaponUpgrade(this->m_pPlayer->pev->iuser4, &theDamageMultiplier, &theTracerFreq);
	
	// Perform trace to hit victim
	TraceResult tr;
	UTIL_TraceLine(vecSrc, vecEnd, dont_ignore_monsters, dont_ignore_glass, this->m_pPlayer->edict(), &tr);
	CBaseEntity* theEntityHit = CBaseEntity::Instance(tr.pHit);
	if(theEntityHit)
	{
		float theScalar = 1.0f;

		bool theCanDoDamageTo = GetGameRules()->CanEntityDoDamageTo(this->m_pPlayer, theEntityHit, &theScalar);
		
		theScalar *= AvHPlayerUpgrade::GetAlienRangedDamageUpgrade(this->m_pPlayer->pev->iuser4);

		if(theCanDoDamageTo || (theEntityHit->pev->team == TEAM_IND))
		{
            AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntityHit);
            if(!thePlayer || thePlayer->GetCanBeAffectedByEnemies())
            {
                bool thePlayEffect = false;

                AvHBaseBuildable* theBuildable = dynamic_cast<AvHBaseBuildable*>(theEntityHit);
			    if((thePlayer || theBuildable) && !GetHasUpgrade(theEntityHit->pev->iuser4, MASK_PARASITED))
			    {
				    // Increase score for guy who parasited
				    //this->m_pPlayer->AddPoints(1, TRUE);

				    // joev: bug 0000841 - Don't allow alien players/structures to actually be parasited.
					// They can take the damage, just not have the "upgrade".
					if (theEntityHit->pev->team != this->m_pPlayer->pev->team) {
						SetUpgradeMask(&theEntityHit->pev->iuser4, MASK_PARASITED);
					}
					// :joev
                    thePlayEffect = true;
			    }

                if(theCanDoDamageTo)
			    {
                    float theDamage = this->mDamage*theScalar;
				    if(theEntityHit->TakeDamage(this->pev, this->m_pPlayer->pev, theDamage, this->GetDamageType()) > 0)
                    {
                        thePlayEffect = true;
                    }
			    }

                if(thePlayEffect)
                {
                    // Play parasite-hit sound at player
    			    EMIT_SOUND(theEntityHit->edict(), CHAN_AUTO, kParasiteHitSound, 1.0f, ATTN_NORM);
                }
            }
		}
	}

	#endif	
}

char* AvHParasiteGun::GetViewModel() const
{
	return kLevel1ViewModel;
}

void AvHParasiteGun::Precache()
{
	AvHAlienWeapon::Precache();

	PRECACHE_UNMODIFIED_SOUND(kParasiteFireSound);
	PRECACHE_UNMODIFIED_SOUND(kParasiteHitSound);
	PRECACHE_UNMODIFIED_MODEL(kParasiteProjectileModel);

	this->mEvent = PRECACHE_EVENT(1, kParasiteShootEventName);
}

void AvHParasiteGun::Spawn() 
{ 
    AvHAlienWeapon::Spawn();

	Precache();

	this->m_iId = AVH_WEAPON_PARASITE;

    // Set our class name
	this->pev->classname = MAKE_STRING(kwsParasiteGun);

	SET_MODEL(ENT(this->pev), kNullModel);

	FallInit();// get ready to fall down.
} 

bool AvHParasiteGun::UsesAmmo(void) const
{
	return false;
}

