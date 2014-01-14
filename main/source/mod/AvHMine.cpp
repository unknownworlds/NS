//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: This is the weapon that marines use to deploy mines (not the mines themselves)
//
// $Workfile: AvHMine.cpp $
// $Date: 2002/10/25 21:48:21 $
//
//-------------------------------------------------------------------------------
// $Log: AvHMine.cpp,v $
// Revision 1.10  2002/10/25 21:48:21  Flayra
// - Fixe for wrong player model when holding mines
//
// Revision 1.9  2002/10/16 20:53:09  Flayra
// - Removed weapon upgrade sounds
//
// Revision 1.8  2002/10/16 01:01:58  Flayra
// - Fixed mines being resupplied from armory
//
// Revision 1.7  2002/10/03 18:46:17  Flayra
// - Added heavy view model
//
// Revision 1.6  2002/07/24 19:09:17  Flayra
// - Linux issues
//
// Revision 1.5  2002/07/24 18:55:52  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.4  2002/07/24 18:45:42  Flayra
// - Linux and scripting changes
//
// Revision 1.3  2002/06/25 17:47:14  Flayra
// - Fixed mine, refactored for new disabled/enabled state
//
// Revision 1.2  2002/06/03 16:37:31  Flayra
// - Constants and tweaks to make weapon anims and times correct with new artwork, added different deploy times (this should be refactored a bit more)
//
// Revision 1.1  2002/05/23 02:33:42  Flayra
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
#include "mod/AvHMarineEquipmentConstants.h"

#ifdef AVH_SERVER
#include "mod/AvHGamerules.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHServerUtil.h"
#endif

LINK_ENTITY_TO_CLASS(kwMine, AvHMine);

void AvHMine::DeductCostForShot(void)
{
	AvHBasePlayerWeapon::DeductCostForShot();

	//this->m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType]--;

	//if(this->m_pPlayer->m_rgAmmo[m_iPrimaryAmmoType] <= 0)
	if(!this->m_iClip)
	{
		if ( this->m_pPlayer )
			this->m_pPlayer->EffectivePlayerClassChanged();
		// no more mines! 
		RetireWeapon();
	}

}

bool AvHMine::GetCanBeResupplied() const
{
    return false;
}

int	AvHMine::GetDeployAnimation() const
{
	return 2;
}

char* AvHMine::GetHeavyViewModel() const
{
	return kTripmineHVVModel;
}

char* AvHMine::GetPlayerModel() const
{
	return kTripminePModel;
}

char* AvHMine::GetWorldModel() const
{
	return kTripmineWModel;
}

char* AvHMine::GetViewModel() const
{
	return kTripmineVModel;
}

int	AvHMine::GetShootAnimation() const
{
	// Return deploy animation for now, this should play fire animation, then a little later, play the deploy animation
	return 2;
}

void AvHMine::Holster(int skiplocal)
{
	if(!this->m_iClip)
	{
		// Out of mines
		SetThink(&AvHMine::DestroyItem);
		this->pev->nextthink = gpGlobals->time + 0.1;
	}

	AvHMarineWeapon::Holster(skiplocal);
}

void AvHMine::Init()
{
	this->mRange = kMineRange;
	this->mDamage = 0; // What to do here?  Is it taking damage from CGrenade?
}

bool AvHMine::ProcessValidAttack(void)
{
	bool theSuccess = AvHMarineWeapon::ProcessValidAttack();

    // This test is not necessary since the new collision code makes it so
    // that interpenetrating objects are not a problem.

    /*
    if(theSuccess)
	{
		#ifdef AVH_SERVER
		theSuccess = false;
		
		Vector theDropLocation;
		Vector theDropAngles;
		if(this->GetDropLocation(theDropLocation, &theDropAngles))
		{
			Vector theMineMinSize = Vector (kMineMinSize);
			Vector theMineMaxSize = Vector (kMineMaxSize);

			// TODO: Rotate extents by theDropAngles, to test bounding box extents as the mine would be placed

			if(AvHSHUGetIsAreaFree(theDropLocation, theMineMinSize, theMineMaxSize))
			{
				theSuccess = true;
			}
		}
		#endif
	}
    */
#ifdef AVH_SERVER

    if(theSuccess)
    {

    	Vector theMineOrigin;
    	Vector theMineAngles;
        
        theSuccess = this->GetDropLocation(theMineOrigin, &theMineAngles);

    }

#endif

	return theSuccess;
}

#ifdef AVH_SERVER
bool AvHMine::GetDropLocation(Vector& outLocation, Vector* outAngles) const
{
	bool theSuccess = false;

	UTIL_MakeVectors( m_pPlayer->pev->v_angle + m_pPlayer->pev->punchangle );
	Vector vecSrc	 = m_pPlayer->GetGunPosition( );
	Vector vecAiming = gpGlobals->v_forward;
	
	TraceResult tr;
	
	UTIL_TraceLine( vecSrc, vecSrc + vecAiming*this->mRange, dont_ignore_monsters, ENT( m_pPlayer->pev ), &tr );

	if (tr.flFraction < 1.0)
	{
		CBaseEntity* theEntity = CBaseEntity::Instance( tr.pHit );
		
		// : 981
		// Mines can't be planted on players or buildings
        if (!dynamic_cast<AvHDeployedMine*>(theEntity) && !dynamic_cast<AvHPlayer *>(theEntity) && !dynamic_cast<AvHBaseBuildable *>(theEntity))
        {
    
            int kOffset = 8;
            Vector thePotentialOrigin = tr.vecEndPos + tr.vecPlaneNormal * kOffset;

		    BaseEntityListType theEntityList;
		    theEntityList.push_back(theEntity);

		    // Make sure there isn't an entity nearby that this would block
		    theEntity = NULL;
		    const int kMineSearchRadius = 15;
		    while((theEntity = UTIL_FindEntityInSphere(theEntity, thePotentialOrigin, kMineSearchRadius)) != NULL)
		    {
			    theEntityList.push_back(theEntity);
		    }
		    
		    // For the mine placement to be valid, the entity it hit, and all the entities nearby must be valid and non-blocking
		    theSuccess = true;
		    for(BaseEntityListType::iterator theIter = theEntityList.begin(); theIter != theEntityList.end(); theIter++)
		    {
				// : 225 make sure there are no mines within kMineSearchRadius of each other ( 15 units )
			    CBaseEntity* theCurrentEntity = *theIter;
			    if(!theCurrentEntity || (theCurrentEntity->pev->flags & FL_CONVEYOR) || AvHSUGetIsExternalClassName(STRING(theCurrentEntity->pev->classname)) || dynamic_cast<CBaseDoor*>(theCurrentEntity) || dynamic_cast<CRotDoor*>(theCurrentEntity)
					|| dynamic_cast<AvHDeployedMine*>(theCurrentEntity) )
			    {
				    theSuccess = false;
				    break;
			    }
		    }

		    if(theSuccess)
		    {
			    VectorCopy(thePotentialOrigin, outLocation);
			    if(outAngles)
			    {
				    VectorCopy(UTIL_VecToAngles( tr.vecPlaneNormal ), *outAngles)
			    }
		    }

        }

	}

	return theSuccess;
}
#endif

void AvHMine::FireProjectiles(void)
{
#ifdef AVH_SERVER
	Vector theMineOrigin;
	Vector theMineAngles;
	if(this->GetDropLocation(theMineOrigin, &theMineAngles))
	{
		GetGameRules()->MarkDramaticEvent(kMinePlacePriority, this->m_pPlayer);

		AvHDeployedMine* theMine = dynamic_cast<AvHDeployedMine*>(CBaseEntity::Create( kwsDeployedMine, theMineOrigin, theMineAngles, m_pPlayer->edict() ));
		ASSERT(theMine);

		// Set the team so it doesn't blow us up, remember the owner so proper credit can be given
		theMine->pev->team = m_pPlayer->pev->team;
		//theMine->pev->owner = m_pPlayer->edict();
		theMine->SetPlacer(this->m_pPlayer->pev);

		// Set it as a marine item so it gets damage upgrades
        // Set any team-wide upgrades
        AvHTeam* theTeam = GetGameRules()->GetTeam(AvHTeamNumber(m_pPlayer->pev->team));
        ASSERT(theTeam);
        theMine->pev->iuser4 |= theTeam->GetTeamWideUpgrades();
	}
#endif
}

int	AvHMine::GetBarrelLength() const
{
	return kMineBarrellLength;
}

float AvHMine::GetRateOfFire() const
{
    return kMineROF;
}

bool AvHMine::GetFiresUnderwater() const
{
	return true;
}

BOOL AvHMine::PlayEmptySound()
{
	// None
	return 0;
}

void AvHMine::Precache()
{
	AvHMarineWeapon::Precache();

	UTIL_PrecacheOther(kwsDeployedMine);

	this->mEvent = PRECACHE_EVENT(1, kWeaponAnimationEvent);
}

bool AvHMine::Resupply()
{
 	return false;
}

void AvHMine::Spawn() 
{ 
	this->Precache();
	
    AvHMarineWeapon::Spawn(); 

	this->m_iId = AVH_WEAPON_MINE;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsMine);

	SET_MODEL(ENT(pev), kTripmineW2Model);

	FallInit();// get ready to fall down.

    int theNumMines = BALANCE_VAR(kMineMaxAmmo);
    
    #ifdef AVH_SERVER
    if(GetGameRules()->GetIsCombatMode())
    {
        theNumMines = BALANCE_VAR(kMineMaxAmmoCombat);
    }
    #endif

	this->m_iDefaultAmmo = theNumMines;
} 


bool AvHMine::UsesAmmo(void) const
{
	return true;
}

BOOL AvHMine::UseDecrement(void)
{
	return true;
}

