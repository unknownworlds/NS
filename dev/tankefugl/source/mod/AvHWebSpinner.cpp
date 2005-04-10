//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHWebSpinner.cpp $
// $Date: 2002/11/22 21:28:17 $
//
//-------------------------------------------------------------------------------
// $Log: AvHWebSpinner.cpp,v $
// Revision 1.14  2002/11/22 21:28:17  Flayra
// - mp_consistency changes
//
// Revision 1.13  2002/10/18 22:23:24  Flayra
// - Refactoring for checking max buildings in radius as well
//
// Revision 1.12  2002/07/24 19:09:18  Flayra
// - Linux issues
//
// Revision 1.11  2002/07/24 18:55:53  Flayra
// - Linux case sensitivity stuff
//
// Revision 1.10  2002/07/24 18:45:43  Flayra
// - Linux and scripting changes
//
// Revision 1.9  2002/07/08 17:23:03  Flayra
// - Don't allow builder to web a player by hitting him
//
// Revision 1.8  2002/06/25 17:50:07  Flayra
// - Some refactoring, new view model, removed old code, no longer costs resources to use, web projectile can ensnare marines without creating web
//
// Revision 1.7  2002/06/10 19:47:25  Flayra
// - New level 2 view model
//
// Revision 1.6  2002/06/03 16:39:10  Flayra
// - Added different deploy times (this should be refactored a bit more)
//
// Revision 1.5  2002/05/28 18:13:37  Flayra
// - Limit total webs, limit webs in sphere
//
// Revision 1.4  2002/05/23 02:32:39  Flayra
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

#include "mod/AvHConstants.h"

#ifdef AVH_SERVER
#include "mod/AvHGamerules.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHEntities.h"
#include "dlls/cbasedoor.h"
#include "dlls/plats.h"
#include "mod/AvHTitles.h"
#endif


#ifdef AVH_SERVER
LINK_ENTITY_TO_CLASS(kwWebProjectile, AvHWebProjectile);

bool AvHWebProjectile::CreateWeb()
{
	bool theSuccessfulStrand = false;

	AvHWebSpinner* theWebSpinner = NULL;
	if(this->GetWebSpinner(theWebSpinner))
	{
		vec3_t theNewPoint = this->pev->origin;
		
		vec3_t theLastPoint;
		if(theWebSpinner->GetLastPoint(theLastPoint))
		{
			// Check line between these two points.  If there's an obstruction then don't create strand
			TraceResult theObstructionTR;
			UTIL_TraceLine(theLastPoint, theNewPoint, ignore_monsters, NULL, &theObstructionTR);
			if(theObstructionTR.flFraction == 1.0f)
			{
				// ...and if distance between this point and last valid point is short enough
				float theDistance = (theNewPoint - theLastPoint).Length();
				if(theDistance < kMaxWebDistance)
				{
					// Make sure our team doesn't have too many webs already
					AvHTeam* theTeam = GetGameRules()->GetTeam((AvHTeamNumber)this->pev->team);
					ASSERT(theTeam);

					AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(this->pev->owner));
					ASSERT(thePlayer);

					int theNumStrands = theTeam->GetNumWebStrands();
					if(theNumStrands < BALANCE_VAR(kMaxTeamStrands))
					{
						// Finally, make sure there aren't too many strands in our immediate area, to prevent visibility problems.
						// Note that it doesn't care if they are enemy or friendly web strands, so this may be a problem for alien vs. alien
						int theNumWebsNearby = UTIL_CountEntitiesInSphere(theNewPoint, BALANCE_VAR(kBuildingVisibilityRadius), kesTeamWebStrand);
						if(theNumWebsNearby < BALANCE_VAR(kNumWebsAllowedInRadius))
						{
							// Add new web strand
							AvHWebStrand* theWebStrand = GetClassPtr((AvHWebStrand*)NULL);
							theWebStrand->Setup(theLastPoint, theNewPoint);
							theWebStrand->Spawn();
							//theWebStrand->pev->owner = this->pev->owner;
							theWebStrand->pev->team = this->pev->team;
							
							// Add decal at this point
							UTIL_DecalTrace(&theObstructionTR, DECAL_SPIT1 + RANDOM_LONG(0,1));
							
							// Increment team strands
							theTeam->SetNumWebStrands(theNumStrands + 1);
							
							theSuccessfulStrand = true;
						}
						else
						{
							// Send player message indicating they've reached the limit for this particular area
							thePlayer->SendMessage(kTooManyWebsNearby, true);
						}
					}
					else
					{
						// Send player message indicating they've reached the limit
						thePlayer->SendMessage(kTooManyWebs, true);
					}
				}
			}
		}

		theWebSpinner->SetLastPoint(theNewPoint);
	}

	return theSuccessfulStrand;
}

bool AvHWebProjectile::GetWebSpinner(AvHWebSpinner*& outSpinner)
{
	bool theSuccess = false;

	CBaseEntity* theEntity = CBaseEntity::Instance(this->pev->owner);
	if(theEntity)
	{
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
		if(thePlayer)
		{
			AvHWebSpinner* theWebSpinner = dynamic_cast<AvHWebSpinner*>(thePlayer->m_pActiveItem);
			if(theWebSpinner)
			{
				outSpinner = theWebSpinner;
				theSuccess = true;
			}
		}
	}

	return theSuccess;
}

void AvHWebProjectile::Precache(void)
{
	CBaseEntity::Precache();
	
	PRECACHE_UNMODIFIED_MODEL(kWebProjectileSprite);
}


void AvHWebProjectile::Spawn()
{
	this->Precache();
	CBaseEntity::Spawn();
	
	this->pev->movetype = MOVETYPE_FLY;
	this->pev->classname = MAKE_STRING(kWebProjectileClassName);
	
	SET_MODEL(ENT(this->pev), kWebProjectileSprite);
	this->pev->solid = SOLID_BBOX;
	
	// Comment out effects line, uncomment next four, then comment out creation of temp entity in EV_SpitGun to see server side spit for testing
	if(!GetGameRules()->GetDrawInvisibleEntities())
	{
		this->pev->effects = EF_NODRAW;
	}
	else
	{
		this->pev->frame = 0;
		this->pev->scale = 0.5;
		this->pev->rendermode = kRenderTransAlpha;
		this->pev->renderamt = 255;
	}
	
	UTIL_SetSize(this->pev, Vector( 0, 0, 0), Vector(0, 0, 0));
	//UTIL_SetSize(this->pev, Vector( -16, -16, -16), Vector(16, 16, 16));
	//UTIL_SetSize(this->pev, Vector( -50, -50, -50), Vector(50, 50, 50));
	
	SetTouch(&AvHWebProjectile::WebTouch);
}


void AvHWebProjectile::WebTouch(CBaseEntity* inOther)
{
	// Never hit the player who fired it
	if(inOther != CBaseEntity::Instance(this->pev->owner))
	{
		// If we an enemy player, ensnare them
//		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inOther);
//		if(thePlayer && GetGameRules()->CanEntityDoDamageTo(this, inOther))
//		{
//			EMIT_SOUND(ENT(inOther->pev), CHAN_AUTO, kWebStrandBreakSound, 1.0, ATTN_IDLE);
//			thePlayer->SetEnsnareState(true);
//		}
//		else
//		{

		// Can't hit players directly
		if(!inOther->IsPlayer())
		{
			// If not in tourny mode, pass through friends
			if((inOther->pev->team == 0) || (GetGameRules()->CanEntityDoDamageTo(this, inOther)))
			{
				// Create new strand
				if(this->CreateWeb())
				{
					// Play sound when it hits
					//EMIT_SOUND(ENT(this->m_pPlayer->pev), CHAN_AUTO, kWebSpinSound2, 1.0f, ATTN_NORM);
				}
			}
		}

		// Kill it off
		SetThink(&AvHWebProjectile::SUB_Remove);
		this->pev->nextthink = gpGlobals->time + .1f;
		
		SetTouch(NULL);
	}
}
#endif



LINK_ENTITY_TO_CLASS(kwWebSpinner, AvHWebSpinner);

BOOL AvHWebSpinner::Deploy()
{
	#ifdef AVH_SERVER
	this->mPlacedValidWebPointSinceDeploy = false;
	#endif

	return AvHAlienWeapon::Deploy();
}

int	AvHWebSpinner::GetBarrelLength() const
{
	return kWebSpinnerBarrelLength;
}

float AvHWebSpinner::GetRateOfFire() const
{
	return BALANCE_VAR(kWebSpinnerROF);
}

int AvHWebSpinner::GetDeployAnimation() const
{
	int theDeployAnimation = 5;
	
	AvHWeaponID thePreviousID = this->GetPreviousWeaponID();
	
	switch(thePreviousID)
	{
	case AVH_WEAPON_WEBSPINNER:
		theDeployAnimation = -1;
		break;

	// Spit and bile bomb look the same
	case AVH_WEAPON_SPIT:
	case AVH_WEAPON_BILEBOMB:
		theDeployAnimation = 5;
		break;
	}
	
	return theDeployAnimation;
}

float AvHWebSpinner::GetDeployTime() const
{
	return 1.0f;
}

int	AvHWebSpinner::GetIdleAnimation() const
{
	return 2;
}

int	AvHWebSpinner::GetShootAnimation() const
{
	return 4;
}

bool AvHWebSpinner::GetFiresUnderwater() const
{
	return true;
}

bool AvHWebSpinner::GetIsDroppable() const
{
	return false;
}
	
void AvHWebSpinner::Init()
{
	this->mRange = kWebSpinnerRange;

	#ifdef AVH_SERVER
	this->mPlacedValidWebPointSinceDeploy = false;
	#endif
}

char* AvHWebSpinner::GetViewModel() const
{
	return kLevel2ViewModel;
}


void AvHWebSpinner::Precache(void)
{
	AvHAlienWeapon::Precache();
	
	PRECACHE_UNMODIFIED_SOUND(kWebSpinSound1);
	PRECACHE_UNMODIFIED_SOUND(kWebSpinSound2);
	
	this->mEvent = PRECACHE_EVENT(1, kWebSpinnerShootEventName);
}

void AvHWebSpinner::Spawn()
{
    AvHAlienWeapon::Spawn(); 
	
	Precache();
	
	this->m_iId = AVH_WEAPON_WEBSPINNER;
	
    // Set our class name
	this->pev->classname = MAKE_STRING(kwsWebSpinner);
	
	SET_MODEL(ENT(this->pev), kNullModel);
	
	FallInit();// get ready to fall down.
	
}

bool AvHWebSpinner::UsesAmmo(void) const
{
	return false;
}

void AvHWebSpinner::FireProjectiles(void)
{
#ifdef AVH_SERVER
	
	// Make sure we have enough points to shoot this thing
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(this->m_pPlayer);
	ASSERT(thePlayer);

//	int theNumResources = thePlayer->GetResources();
//	if(theNumResources >= kWebPointCost)
//	{
//		thePlayer->SetResources(theNumResources - kWebPointCost);

		// Spawn WebProjectile
		AvHWebProjectile* theWebProjectile = GetClassPtr((AvHWebProjectile*)NULL );
		theWebProjectile->Spawn();
		
		UTIL_MakeVectors(thePlayer->pev->v_angle);
		
		Vector vecAiming = gpGlobals->v_forward;
		Vector vecSrc = thePlayer->GetGunPosition( ) + vecAiming;
		
		UTIL_SetOrigin(theWebProjectile->pev, vecSrc);
		
		// This needs to be the same as in EV_WebProjectileGun
		Vector theBaseVelocity;
		VectorScale(this->pev->velocity, kWebProjectileParentVelocityScalar, theBaseVelocity);
		
		Vector theStartVelocity;
		VectorMA(theBaseVelocity, kWebProjectileVelocity, vecAiming, theStartVelocity);
		
		VectorCopy(theStartVelocity, theWebProjectile->pev->velocity);
		
		// Set owner
		theWebProjectile->pev->owner = ENT(thePlayer->pev);
		
		// Set WebProjectile's team :)
		theWebProjectile->pev->team = thePlayer->pev->team;
		
	//	// Do a traceline to see where they want to place new web point
	//	UTIL_MakeVectors(this->m_pPlayer->pev->v_angle);
	//
	//	Vector theForwardDir = gpGlobals->v_forward;
	//	VectorNormalize(theForwardDir);
	//
	//	TraceResult	theNewStrandTR;
	//	UTIL_TraceLine(this->pev->origin + this->pev->view_ofs, this->pev->origin + this->pev->view_ofs + theForwardDir*128, ignore_monsters, ENT(this->m_pPlayer->pev), &theNewStrandTR);
	//	if (theNewStrandTR.flFraction != 1.0)
	//	{
	//		//CBaseEntity* theWorld = dynamic_cast<CWorld*>(CBaseEntity::Instance(tr.pHit));
	//		//if(theWorld)
	//		//{
	//
	//		bool theSuccessfulStrand = false;
	//
	//		// Only allow webs on the world, not doors, entities, players
	//		CBaseDoor* theDoor = dynamic_cast<CBaseDoor*>(CBaseEntity::Instance(theNewStrandTR.pHit));
	//		CBasePlatTrain* theTrain = dynamic_cast<CBasePlatTrain*>(CBaseEntity::Instance(theNewStrandTR.pHit));
	//		if((!theDoor) && (!theTrain))
	//		{
	//			vec3_t theNewPoint = theNewStrandTR.vecEndPos;
	//			vec3_t theNewNormal = theNewStrandTR.vecPlaneNormal;
	//
	//			// If we've placed another point
	//			if(this->mPlacedValidWebPointSinceDeploy)
	//			{
	//				// Check line between these two points.  If there's an obstruction (that's not us?) then remember new point but don't create strand
	//				TraceResult theObstructionTR;
	//				UTIL_TraceLine(this->mLastValidWebPoint, theNewPoint, ignore_monsters, ENT(this->m_pPlayer->pev), &theObstructionTR);
	//				if(theObstructionTR.flFraction == 1.0f)
	//				{
	//					// Check to make sure the normals are different, can't place on the same surface for fairness
	//					if(!(theNewNormal == this->mLastValidWebNormal))
	//					{
	//						// ...and if distance between this point and last valid point is short enough
	//						float theDistance = (theNewPoint - this->mLastValidWebPoint).Length();
	//						if(theDistance < kMaxWebDistance)
	//						{
	//							// Add new web strand
	//							AvHWebStrand* theWebStrand = GetClassPtr((AvHWebStrand*)NULL);
	//							theWebStrand->Setup(this->mLastValidWebPoint, theNewPoint);
	//							theWebStrand->Spawn();
	//							theWebStrand->pev->owner = ENT(this->m_pPlayer->pev);
	//							theWebStrand->pev->team = this->m_pPlayer->pev->team;
	//
	//							theSuccessfulStrand = true;
	//						}
	//					}
	//				}
	//			}
	//			else
	//			{
	//				theSuccessfulStrand = true;
	//			}
	//
	//			if(theSuccessfulStrand)
	//			{
	//				// Play web confirmation sound
	//				EMIT_SOUND(ENT(this->m_pPlayer->pev), CHAN_AUTO, kWebSpinSound2, 1.0f, ATTN_NORM);
	//				
	//				// Add decal at this point
	//				UTIL_DecalTrace(&theNewStrandTR, DECAL_SPIT1 + RANDOM_LONG(0,1));
	//			}
	//
	//			// Set new valid last point to current point
	//			this->mLastValidWebPoint = theNewPoint;
	//			this->mLastValidWebNormal = theNewNormal;
	//			this->mPlacedValidWebPointSinceDeploy = true;
	//		}
	//	}
//	}
//	else
//	{
//		thePlayer->PlayHUDSound(HUD_SOUND_ALIEN_MORE);
//	}
				
	#endif	
}

#ifdef AVH_SERVER
bool AvHWebSpinner::GetLastPoint(vec3_t& outLastPoint) const
{
	bool theSuccess = false;

	if(this->mPlacedValidWebPointSinceDeploy)
	{
		VectorCopy(this->mLastValidWebPoint, outLastPoint);
		theSuccess = true;
	}

	return theSuccess;
}

void AvHWebSpinner::SetLastPoint(vec3_t& inLastPoint)
{
	VectorCopy(inLastPoint, this->mLastValidWebPoint);
	this->mPlacedValidWebPointSinceDeploy = true;
}
#endif

