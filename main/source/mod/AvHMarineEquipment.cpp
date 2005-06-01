//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHMarineEquipment.cpp $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHMarineEquipment.cpp,v $
// Revision 1.50  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.49  2002/11/12 02:25:29  Flayra
// - HLTV updates
//
// Revision 1.48  2002/11/06 01:40:17  Flayra
// - Turrets now need an active turret factory to keep firing
//
// Revision 1.47  2002/11/03 04:50:43  Flayra
// - Hard-coded gameplay constants instead of putting in skill.cfg
// - Ammo and health now expire
//
// Revision 1.46  2002/10/28 20:35:46  Flayra
// - Allow HAs and jetpacks to only be picked up by marines
//
// Revision 1.45  2002/10/25 23:19:30  Flayra
// -  Fixed bug where people were being telefragged improperly
//
// Revision 1.44  2002/10/24 21:32:09  Flayra
// - All heavy armor to be given via console
// - Fix for AFKers on inf portals, also for REIN players when recycling portals
//
// Revision 1.43  2002/10/20 21:10:48  Flayra
// - Optimizations
//
// Revision 1.42  2002/10/19 22:33:44  Flayra
// - Various server optimizations
//
// Revision 1.41  2002/10/18 22:20:49  Flayra
// - Reduce llamability of placing phases near drops or hazards
//
// Revision 1.40  2002/10/16 20:54:30  Flayra
// - Added phase gate sound
// - Fixed ghostly command station view model problem after building it
//
// Revision 1.39  2002/10/16 01:00:14  Flayra
// - Allow any jetpack to be picked up (needed for cheat, but this is the way all weapons work too, may need to be changed for marine vs. marine, not sure)
//
// Revision 1.38  2002/10/03 18:57:20  Flayra
// - Picking up heavy armor holsters your weapon for view model switch
// - Added "armory's upgrading, ammo not available" message but removed it for some reason (I think it was acting strange, like playing way too often)
//
// Revision 1.37  2002/09/25 21:12:26  Flayra
// - Undid solidity change (causes Sys_Error)
//
// Revision 1.36  2002/09/25 20:48:47  Flayra
// - Phase gates no longer solid
//
// Revision 1.35  2002/09/23 22:21:21  Flayra
// - Added jetpack and heavy armor
// - Added "cc online" sound
// - Turret factories now upgrade to advanced turret factories for siege
// - Added automatic resupply at armory, but removed it
// - Observatories scan in 2D now, to match commander range overlay
//
// Revision 1.34  2002/09/09 19:59:31  Flayra
// - Fixed up phase gates (no longer teleport you unless you have two, and they work properly now)
// - Refactored reinforcements
// - Fixed bug where secondary command station couldn't be built
//
// Revision 1.33  2002/08/31 18:01:02  Flayra
// - Work at VALVe
//
// Revision 1.32  2002/08/16 02:39:14  Flayra
// - Fixed command station problems after game ends
//
// Revision 1.31  2002/08/09 01:06:02  Flayra
// - Removed ability for command station to be resurrected
// - Fixed up phase gate effects
//
// Revision 1.30  2002/08/02 21:55:55  Flayra
// - Allow phase teleport to fail nicely.  For some reason, players don't hear their own phase sound anymore, it seems to play at the point where they left instead of where they arrive
//
// Revision 1.29  2002/07/26 23:05:54  Flayra
// - Numerical event feedback
// - Started to add sparks when buildings were hit but didn't know the 3D point to play it at
//
// Revision 1.28  2002/07/24 18:45:42  Flayra
// - Linux and scripting changes
//
// Revision 1.27  2002/07/23 17:11:47  Flayra
// - Phase gates must be built and can be destroyed, observatories decloak aliens, hooks for fast reinforcements upgrade, nuke damage increased, commander banning
//
// Revision 1.26  2002/07/08 17:02:57  Flayra
// - Refactored reinforcements, updated entities for new artwork
//
// Revision 1.25  2002/07/01 21:29:59  Flayra
// - Removed phase particles, added implosion instead, don't select command station on log-in (messy for drawing building radii)
//
// Revision 1.24  2002/06/25 18:04:43  Flayra
// - Renamed some buildings, armory is now upgraded to advanced armory
//
// Revision 1.23  2002/06/10 19:58:17  Flayra
// - Scan update happens more often, in case aliens go cloaked during scan
//
// Revision 1.22  2002/06/03 16:50:35  Flayra
// - Renamed weapons factory and armory, added ammo resupplying
//
// Revision 1.21  2002/05/28 17:51:34  Flayra
// - Tried to make nuke sound play, extended shake duration to sound length, reinforcement refactoring, mark command stations as mapper placed, so they aren't deleted on level cleanup, support for point-entity buildable command stations
//
// Revision 1.20  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHPlayer.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHGamerules.h"
#include "dlls/client.h"
#include "common/vec_op.h"
#include "common/vector_util.h"
#include "mod/AvHSoundListManager.h"
#include "dlls/weapons.h"
#include "mod/AvHServerVariables.h"
#include "mod/AvHConstants.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHMovementUtil.h"
#include "dlls/explode.h"
#include "dlls/weapons.h"
#include "util/MathUtil.h"
#include "mod/AvHTitles.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHParticleConstants.h"
#include "mod/AvHMarineTurret.h"
#include "mod/AvHSiegeTurret.h"
#include "mod/AvHHulls.h"

//LINK_ENTITY_TO_CLASS(kwMine, AvHMine);
//LINK_ENTITY_TO_CLASS(kwDeployedTurret, AvHDeployedTurret);
//LINK_ENTITY_TO_CLASS(kwTurret, AvHTurret);
LINK_ENTITY_TO_CLASS(kwDeployedMine, AvHDeployedMine);
LINK_ENTITY_TO_CLASS(kwHealth, AvHHealth);
LINK_ENTITY_TO_CLASS(kwCatalyst, AvHCatalyst);
LINK_ENTITY_TO_CLASS(kwGenericAmmo, AvHGenericAmmo);
LINK_ENTITY_TO_CLASS(kwJetpack, AvHJetpack);
LINK_ENTITY_TO_CLASS(kwAmmoPack, AvHAmmoPack);
LINK_ENTITY_TO_CLASS(kwHeavyArmor, AvHHeavyArmor);

LINK_ENTITY_TO_CLASS(kwWelder, AvHWelder);
LINK_ENTITY_TO_CLASS(kwScan, AvHScan);
LINK_ENTITY_TO_CLASS(kwPhaseGate, AvHPhaseGate);
//LINK_ENTITY_TO_CLASS(kwSiegeTurret, AvHSiegeTurret);
LINK_ENTITY_TO_CLASS(kwNuke, AvHNuke);

LINK_ENTITY_TO_CLASS(kwInfantryPortal, AvHInfantryPortal);
LINK_ENTITY_TO_CLASS(kwTeamCommand, AvHCommandStation);
LINK_ENTITY_TO_CLASS(kwTurretFactory, AvHTurretFactory);
LINK_ENTITY_TO_CLASS(kwArmory, AvHArmory);
LINK_ENTITY_TO_CLASS(kwAdvancedArmory, AvHArmory);
//LINK_ENTITY_TO_CLASS(kwAdvancedArmory, AvHAdvancedArmory);
LINK_ENTITY_TO_CLASS(kwArmsLab, AvHArmsLab);
LINK_ENTITY_TO_CLASS(kwPrototypeLab, AvHPrototypeLab);
LINK_ENTITY_TO_CLASS(kwObservatory, AvHObservatory);

extern int						gTeleportEventID;
extern int						gSiegeHitEventID;
extern int						gSiegeViewHitEventID;
extern AvHSoundListManager		gSoundListManager;

void AvHDeployedMine::Precache(void)
{
	PRECACHE_UNMODIFIED_MODEL(kTripmineWModel);
	PRECACHE_UNMODIFIED_MODEL(kTripmineW2Model);
	PRECACHE_UNMODIFIED_SOUND(kTripmineDeploySound);
	PRECACHE_UNMODIFIED_SOUND(kTripmineActivateSound);
	PRECACHE_UNMODIFIED_SOUND(kTripmineChargeSound);
	PRECACHE_UNMODIFIED_SOUND(kTripmineStepOnSound);
}

void AvHDeployedMine::ActiveTouch(CBaseEntity* inOther)
{
	float kTimeBetweenBeeps = 3.0f;
	
	// Hack to circumvent the hack where owners can't collide.  If this isn't done, then mines will blowup when touching the world sometimes.
	//edict_t* theTempOwner = this->pev->owner;
	//this->pev->owner = this->m_pRealOwner;
	bool theEntityCanDoDamage = GetGameRules()->CanEntityDoDamageTo(this, inOther);
	//this->pev->owner = theTempOwner;
	// Check team here and only emit warning beep for friendlies
	if(theEntityCanDoDamage && (this->pev->team != inOther->pev->team))
	{
		GetGameRules()->MarkDramaticEvent(kMineExplodePriority, inOther, this);
		this->Detonate();
	}
	else
	{
		if(gpGlobals->time > this->mLastTimeTouched + kTimeBetweenBeeps)
		{
			// Only players trigger this, not buildings or other mines
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inOther);
			if(thePlayer)
			{
				// Play warning proximity beep
				EMIT_SOUND( ENT(pev), CHAN_BODY, kTripmineStepOnSound, 0.5, ATTN_NORM ); // shut off chargeup
				this->mLastTimeTouched = gpGlobals->time;
			}
		}
	}
}

// Ripped from old grenade
void AvHDeployedMine::Explode(TraceResult* inTrace, int inBitsDamageType)
{
	float		flRndSound;// sound randomizer
	
	pev->model = iStringNull;//invisible
	pev->solid = SOLID_NOT;// intangible
	
	float theDamageModifier;
	int theUpgradeLevel = AvHPlayerUpgrade::GetWeaponUpgrade(this->pev->iuser3, this->pev->iuser4, &theDamageModifier);
	float theDamage = this->pev->dmg*theDamageModifier;

	pev->takedamage = DAMAGE_NO;

	// TODO: Look at upgrade and mark up damage

	// Pull out of the wall a bit
	if ( inTrace->flFraction != 1.0 )
	{
		pev->origin = inTrace->vecEndPos + (inTrace->vecPlaneNormal * (theDamage - 24) * 0.6);
	}

	int iContents = UTIL_PointContents ( pev->origin );
	
	MESSAGE_BEGIN( MSG_PAS, SVC_TEMPENTITY, pev->origin );
		WRITE_BYTE( TE_EXPLOSION );		// This makes a dynamic light and the explosion sprites/sound
		WRITE_COORD( pev->origin.x );	// Send to PAS because of the sound
		WRITE_COORD( pev->origin.y );
		WRITE_COORD( pev->origin.z );
		if (iContents != CONTENTS_WATER)
		{
			WRITE_SHORT( g_sModelIndexFireball );
		}
		else
		{
			WRITE_SHORT( g_sModelIndexWExplosion );
		}
		WRITE_BYTE( (theDamage - 50) * .60  ); // scale * 10
		WRITE_BYTE( 15  ); // framerate
		WRITE_BYTE( TE_EXPLFLAG_NONE );
	MESSAGE_END();

	CSoundEnt::InsertSound ( bits_SOUND_COMBAT, pev->origin, NORMAL_EXPLOSION_VOLUME, 3.0 );

	//RadiusDamage(this->pev, this->pevOwner, theDamage, CLASS_NONE, inBitsDamageType);
    int theRadius = BALANCE_VAR(kMineRadius);
	RadiusDamage(this->pev->origin, this->pev, this->mPlacer, theDamage, theRadius, CLASS_NONE, inBitsDamageType);

	// Play view shake here
	float theShakeAmplitude = 80;
	float theShakeFrequency = 100;
	float theShakeDuration = 1.0f;
	float theShakeRadius = 700;
	UTIL_ScreenShake(this->pev->origin, theShakeAmplitude, theShakeFrequency, theShakeDuration, theShakeRadius);

	if ( RANDOM_FLOAT( 0 , 1 ) < 0.5 )
	{
		UTIL_DecalTrace( inTrace, DECAL_SCORCH1 );
	}
	else
	{
		UTIL_DecalTrace( inTrace, DECAL_SCORCH2 );
	}

	flRndSound = RANDOM_FLOAT( 0 , 1 );

	switch ( RANDOM_LONG( 0, 2 ) )
	{
		case 0:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris1.wav", 0.55, ATTN_NORM);	break;
		case 1:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris2.wav", 0.55, ATTN_NORM);	break;
		case 2:	EMIT_SOUND(ENT(pev), CHAN_VOICE, "weapons/debris3.wav", 0.55, ATTN_NORM);	break;
	}

	pev->effects |= EF_NODRAW;
	SetThink( &CGrenade::Smoke );
	pev->velocity = g_vecZero;
	pev->nextthink = gpGlobals->time + 0.3;

	if (iContents != CONTENTS_WATER)
	{
		int sparkCount = RANDOM_LONG(0,3);
		for ( int i = 0; i < sparkCount; i++ )
			Create( "spark_shower", pev->origin, inTrace->vecPlaneNormal, NULL );
	}
}

void AvHDeployedMine::Smoke(void)
{
	if (UTIL_PointContents(this->pev->origin ) == CONTENTS_WATER)
	{
		UTIL_Bubbles(this->pev->origin - Vector( 64, 64, 64 ), this->pev->origin + Vector( 64, 64, 64 ), 100 );
	}
	else
	{
		float theDamageModifier;
		int theUpgradeLevel = AvHPlayerUpgrade::GetWeaponUpgrade(this->pev->iuser3, this->pev->iuser4, &theDamageModifier);
		int theDamage = this->pev->dmg*theDamageModifier;
		
		MESSAGE_BEGIN( MSG_PVS, SVC_TEMPENTITY, this->pev->origin);
			WRITE_BYTE( TE_SMOKE );
			WRITE_COORD( this->pev->origin.x );
			WRITE_COORD( this->pev->origin.y );
			WRITE_COORD( this->pev->origin.z );
			WRITE_SHORT( g_sModelIndexSmoke );
			WRITE_BYTE( (theDamage - 50) * 0.80 ); // scale * 10
			WRITE_BYTE( 12  ); // framerate
		MESSAGE_END();
	}
	UTIL_Remove( this );
}

void AvHDeployedMine::Killed(entvars_t* inAttacker, int inGib)
{
	this->Detonate();

	CBasePlayerItem::Killed(inAttacker, inGib);
}

int	AvHDeployedMine::TakeDamage(entvars_t *inInflictor, entvars_t *inAttacker, float inDamage, int inBitsDamageType)
{
	// Don't include this for potential overflow reasons
	//UTIL_Sparks(this->pev->origin);

	return CBasePlayerItem::TakeDamage(inInflictor, inAttacker, inDamage, inBitsDamageType);
}

void AvHDeployedMine::Detonate()
{
	// Stop charging up
	EMIT_SOUND(ENT(this->pev), CHAN_BODY, "common/null.wav", 0.5, ATTN_NORM ); 

	if(!this->mDetonated && this->mPoweredUp)
	{
		TraceResult tr;
		UTIL_TraceLine(this->pev->origin + this->mVecDir * 8, this->pev->origin - this->mVecDir * 64, dont_ignore_monsters, ENT(this->pev), &tr);
		
		this->Explode(&tr, NS_DMG_NORMAL);

		this->mDetonated = true;
	}
}

const float kTripmineActiveThinkTime = .8f;
const float kTripminePowerUpThinkTime = .2f;
const float kTripminePowerUpTime = 3.8f;
const float kTripmineFailTime = 20.0f;

void AvHDeployedMine::SetPlacer(entvars_t* inPlacer)
{
	this->mPlacer = inPlacer;
}

void AvHDeployedMine::PowerupThink()
{
	// Find an owner
	if(this->mOwner == NULL)
	{
		edict_t* theOldOwner = this->pev->owner;
		this->pev->owner = NULL;

		TraceResult tr;
		UTIL_TraceLine(this->pev->origin + this->mVecDir * 8, this->pev->origin - this->mVecDir * 32, dont_ignore_monsters, ENT(this->pev), &tr);

		if (tr.fStartSolid || (theOldOwner && tr.pHit == theOldOwner))
		{
			this->pev->owner = theOldOwner;
		}
		else if (tr.flFraction < 1.0)
		{
			this->pev->owner = tr.pHit;
			this->mOwner = CBaseEntity::Instance(this->pev->owner);
			this->mOwnerOrigin = this->mOwner->pev->origin;
			this->mOwnerAngles = this->mOwner->pev->angles;
		}
		else
		{
			STOP_SOUND(ENT(this->pev), CHAN_VOICE, kTripmineDeploySound);
			STOP_SOUND(ENT(this->pev), CHAN_BODY, kTripmineChargeSound);

			SetThink(&CBaseEntity::SUB_Remove);
			this->pev->nextthink = gpGlobals->time + 0.1;

			ALERT(at_console, "WARNING:Tripmine at %.0f, %.0f, %.0f removed\n", this->pev->origin.x, this->pev->origin.y, this->pev->origin.z);
			//KillBeam();
			return;
		}
	}
	else
	{
		this->DetonateIfOwnerInvalid();
	}

	if(!this->mPoweredUp)
	{
		if(gpGlobals->time > (this->mTimePlaced + kTripminePowerUpTime))
		{
			//if(this->pev->solid == SOLID_BBOX)
			//{
				// play enabled sound
				EMIT_SOUND_DYN( ENT(pev), CHAN_VOICE, kTripmineActivateSound, 0.5, ATTN_NORM, 1.0, 75 );
				
				SetTouch(&AvHDeployedMine::ActiveTouch);
				
				SetThink(&AvHDeployedMine::ActiveThink);
				this->pev->nextthink = gpGlobals->time + kTripmineActiveThinkTime;
					
				this->mPoweredUp = true;
			//}
		}
	}

	if(!this->mPoweredUp)
	{
		this->pev->nextthink = gpGlobals->time + kTripminePowerUpThinkTime;
	}
}

void AvHDeployedMine::DetonateIfOwnerInvalid()
{
	if((this->mOwner == NULL) || (this->mOwner->pev->origin != this->mOwnerOrigin) || (this->mOwner->pev->angles != this->mOwnerAngles))
	{
		this->Detonate();
	}
}

// Check to see if our position is no longer valid
void AvHDeployedMine::ActiveThink()
{
	this->DetonateIfOwnerInvalid();
	this->pev->nextthink = gpGlobals->time + kTripmineActiveThinkTime;
}

void AvHDeployedMine::Spawn(void)
{
	this->Precache();

	this->pev->movetype = MOVETYPE_FLY;
	this->pev->solid = SOLID_BBOX;
	this->pev->classname = MAKE_STRING(kwsDeployedMine);
	this->pev->iuser3 = AVH_USER3_MINE;
	
	SET_MODEL(ENT(pev), kTripmineWModel);

	UTIL_SetSize(this->pev, kMineMinSize, kMineMaxSize);
	UTIL_SetOrigin(this->pev, this->pev->origin );

	// Can't emit beep until active
	this->mTimePlaced = gpGlobals->time;
	this->mLastTimeTouched = this->mTimePlaced;
	
	SetThink(&AvHDeployedMine::PowerupThink);
	this->pev->nextthink = gpGlobals->time + kTripminePowerUpThinkTime;

	// give them hit points
	this->pev->takedamage = DAMAGE_YES;
	this->pev->health = BALANCE_VAR(kMineHealth);
	this->pev->dmg = BALANCE_VAR(kMineDamage);
	
	// play deploy sound
	EMIT_SOUND( ENT(this->pev), CHAN_VOICE, kTripmineDeploySound, .8f, ATTN_NORM );
	EMIT_SOUND( ENT(this->pev), CHAN_BODY, kTripmineChargeSound, .5f, ATTN_NORM ); // chargeup

	UTIL_MakeAimVectors(this->pev->angles);
	this->mVecDir = gpGlobals->v_forward;
	this->mVecEnd = this->pev->origin + this->mVecDir * 2048;

	this->mDetonated = false;
	this->mPoweredUp = false;
	this->mOwner = NULL;
	this->mPlacer = NULL;
}

AvHPlayerEquipment::AvHPlayerEquipment()
{
	this->mIsPersistent = false;
	this->mLifetime = -1;
}

void AvHPlayerEquipment::KeyValue(KeyValueData* pkvd)
{
	// Any entity placed by the mapper is persistent
	this->SetPersistent();
	
	if(FStrEq(pkvd->szKeyName, "teamchoice"))
	{
		//this->mTeam = (AvHTeamNumber)(atoi(pkvd->szValue));
		this->pev->team = (AvHTeamNumber)(atoi(pkvd->szValue));
		
		pkvd->fHandled = TRUE;
	}
	else if(FStrEq(pkvd->szKeyName, "angles"))
	{
		// TODO: Insert code here
		//pkvd->fHandled = TRUE;
		int a = 0;
	}
	else if(FStrEq(pkvd->szKeyName, "lifetime"))
	{
		this->mLifetime = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBasePlayerItem::KeyValue(pkvd);
	}
}

// 0 means never expire, -1 means no value was set so use default
int AvHPlayerEquipment::GetLifetime() const
{
	int theLifetime = this->mLifetime;

	if(theLifetime < 0)
	{
		theLifetime = AvHSUGetWeaponStayTime();
	}

	return theLifetime;
}

bool AvHPlayerEquipment::GetIsPersistent() const
{
	return this->mIsPersistent;
}

void AvHPlayerEquipment::SetPersistent()
{
	this->mIsPersistent = true;
}

void AvHHealth::Precache(void)
{
	PRECACHE_UNMODIFIED_MODEL(kHealthModel);
	PRECACHE_UNMODIFIED_SOUND(kHealthPickupSound);
}

void AvHHealth::Spawn(void)
{
	this->Precache();
	
	SET_MODEL(ENT(pev), kHealthModel);
	this->pev->movetype = MOVETYPE_TOSS;
	this->pev->solid = SOLID_TRIGGER;
	this->pev->framerate = 1.0;
	
	UTIL_SetSize(pev, kHealthMinSize, kHealthMaxSize); 
	UTIL_SetOrigin( pev, pev->origin );
	
	SetTouch(&AvHHealth::Touch);

	// Expire after a time.
	int theLifetime = this->GetLifetime();
	if(theLifetime > 0)
	{
		SetThink(&AvHHealth::SUB_Remove);
		this->pev->nextthink = gpGlobals->time + theLifetime;
	}
	
	this->pev->iuser3 = AVH_USER3_MARINEITEM;
}

BOOL AvHHealth::GiveHealth(CBaseEntity* inOther, float points)
{
	BOOL theSuccess = FALSE;

// puzl: 1017
// Amount of health to give is now a paramater to allow us to vary the resupply amount for the armoury

//	float thePointsPerHealth = BALANCE_VAR(kPointsPerHealth)
	
	
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inOther);
	if(thePlayer && thePlayer->GetIsRelevant() && thePlayer->GetIsMarine())
	{
		float thePlayerMaxHealth = AvHPlayerUpgrade::GetMaxHealth(thePlayer->pev->iuser4, thePlayer->GetUser3(), thePlayer->GetExperienceLevel());
		if(thePlayer->pev->health < thePlayerMaxHealth)
		{
			float thePointsGiven = min(points, (thePlayerMaxHealth - thePlayer->pev->health));
			
			thePlayer->pev->health += thePointsGiven;
			
			if(CVAR_GET_FLOAT(kvDrawDamage))
			{
				thePlayer->PlaybackNumericalEvent(kNumericalInfoHealthEvent, thePointsGiven);
			}
			
			// Remove parasite if player has one
			//int& theUser4 = thePlayer->pev->iuser4;
			//SetUpgradeMask(&theUser4, MASK_PARASITED, false);
			
			EMIT_SOUND(ENT(inOther->pev), CHAN_ITEM, kHealthPickupSound, 1, ATTN_NORM);

			theSuccess = TRUE;
		}
	}

	return theSuccess;
}

void AvHHealth::Touch(CBaseEntity* inOther)
{
	// puzl: 1017 medpack health amount
	if(AvHHealth::GiveHealth(inOther, BALANCE_VAR(kPointsPerHealth)))
	{
		UTIL_Remove(this);
	}
}


void AvHCatalyst::Precache(void)
{
    PRECACHE_UNMODIFIED_MODEL(kCatalystModel);
    PRECACHE_UNMODIFIED_SOUND(kCatalystPickupSound);
}

void AvHCatalyst::Spawn(void)
{
    this->Precache();
    
    SET_MODEL(ENT(pev), kCatalystModel);
    this->pev->movetype = MOVETYPE_TOSS;
    this->pev->solid = SOLID_TRIGGER;
    
    UTIL_SetSize(pev, kCatalystMinSize, kCatalystMaxSize); 
    UTIL_SetOrigin( pev, pev->origin );
    
    SetTouch(&AvHCatalyst::Touch);
    
    // Expire after a time.
    int theLifetime = this->GetLifetime();
    if(theLifetime > 0)
    {
        SetThink(&AvHCatalyst::SUB_Remove);
        this->pev->nextthink = gpGlobals->time + theLifetime;
    }
    
    this->pev->iuser3 = AVH_USER3_MARINEITEM;
}

BOOL AvHCatalyst::GiveCatalyst(CBaseEntity* inOther)
{
    BOOL theSuccess = FALSE;
    
    float theCatalystDuration = BALANCE_VAR(kCatalystDuration);
    
    AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inOther);
    if(thePlayer && thePlayer->GetIsRelevant() && thePlayer->GetIsMarine() && !thePlayer->GetIsCatalysted())
    {
        //// The player takes damage too
        //float theDamagePercent = BALANCE_VAR(kCatalystDamagePercent);
        //float theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(thePlayer->pev->iuser4, (AvHUser3)thePlayer->pev->iuser3);
        //float theDamage = theDamagePercent*theMaxHealth;
        //
        //// Never kill the player
        //theDamage = min(theDamage, thePlayer->pev->health - 1);
        //thePlayer->TakeDamage(thePlayer->pev, thePlayer->pev, theDamage, DMG_GENERIC | DMG_IGNOREARMOR);

        EMIT_SOUND(ENT(inOther->pev), CHAN_ITEM, kCatalystPickupSound, 1, ATTN_NORM);

        thePlayer->SetIsCatalysted(true, theCatalystDuration);

        theSuccess = TRUE;
    }
    
    return theSuccess;
}

void AvHCatalyst::Touch(CBaseEntity* inOther)
{
    if(AvHCatalyst::GiveCatalyst(inOther))
    {
        UTIL_Remove(this);
    }
}

void AvHHeavyArmor::Precache(void)
{
	PRECACHE_UNMODIFIED_MODEL(kHeavyModel);
	PRECACHE_UNMODIFIED_SOUND(kHeavyPickupSound);
}

void AvHHeavyArmor::Spawn(void)
{
	this->Precache();
	
	SET_MODEL(ENT(pev), kHeavyModel);
	this->pev->movetype = MOVETYPE_TOSS;
	this->pev->solid = SOLID_TRIGGER;
	UTIL_SetSize(pev, kHeavyMinSize, kHeavyMaxSize); 
	UTIL_SetOrigin( pev, pev->origin );
	
	SetTouch(&AvHHeavyArmor::Touch);
	
	this->pev->iuser3 = AVH_USER3_HEAVY;
}

void AvHHeavyArmor::Touch(CBaseEntity* inOther)
{
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inOther);
	if(thePlayer && thePlayer->GetIsRelevant())
	{
		if(thePlayer->GetIsMarine())
		{
			// Check to make sure they don't have heavy armor or jetpack already
			if((!thePlayer->GetHasJetpack() || GetGameRules()->GetIsCombatMode()) && !thePlayer->GetHasHeavyArmor())//voogru: ignore in combat mode since were trying to touch it.
			{
				// Needed because view model changes
				if(thePlayer->HolsterWeaponToUse())
				{
					// Give player heavy armor
					SetUpgradeMask(&thePlayer->pev->iuser4, MASK_UPGRADE_7, false);
					SetUpgradeMask(&thePlayer->pev->iuser4, MASK_UPGRADE_13);

					// Mark player with heavy armor
					thePlayer->EffectivePlayerClassChanged();

					// Set new armor value
					thePlayer->pev->armorvalue = AvHPlayerUpgrade::GetMaxArmorLevel(thePlayer->pev->iuser4, (AvHUser3)thePlayer->pev->iuser3);

					EMIT_SOUND(ENT(inOther->pev), CHAN_ITEM, kHeavyPickupSound, 1, ATTN_NORM);
					
					UTIL_Remove(this);
				}
			}
		}
	}
}

void AvHJetpack::Precache(void)
{
	PRECACHE_UNMODIFIED_MODEL(kJetpackModel);
	PRECACHE_UNMODIFIED_SOUND(kJetpackPickupSound);
}

void AvHJetpack::Spawn(void)
{
	this->Precache();
	
	SET_MODEL(ENT(pev), kJetpackModel);
	this->pev->movetype = MOVETYPE_TOSS;
	this->pev->solid = SOLID_TRIGGER;
	UTIL_SetSize(pev, kJetpackMinSize, kJetpackMaxSize); 
	UTIL_SetOrigin( pev, pev->origin );
	
	SetTouch(&AvHJetpack::Touch);
	
	this->pev->iuser3 = AVH_USER3_JETPACK;
}

void AvHJetpack::Touch(CBaseEntity* inOther)
{
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inOther);
	if(thePlayer && thePlayer->GetIsRelevant())
	{
		if(thePlayer->GetIsMarine())
		{
			// Check to make sure they don't have heavy armor or jetpack already
			if((!thePlayer->GetHasHeavyArmor() || GetGameRules()->GetIsCombatMode()) && !thePlayer->GetHasJetpack())//voogru: ignore in combat mode since were trying to touch it.
			{
                if(thePlayer->HolsterWeaponToUse())
                {
				    // Give player jetpack
					SetUpgradeMask(&thePlayer->pev->iuser4, MASK_UPGRADE_13, false);
				    SetUpgradeMask(&thePlayer->pev->iuser4, MASK_UPGRADE_7);
				    
					// Mark player with jetpack
					thePlayer->EffectivePlayerClassChanged();

					EMIT_SOUND(ENT(inOther->pev), CHAN_ITEM, kJetpackPickupSound, 1, ATTN_NORM);

					// Set new armor value
					thePlayer->pev->armorvalue = AvHPlayerUpgrade::GetMaxArmorLevel(thePlayer->pev->iuser4, (AvHUser3)thePlayer->pev->iuser3);

				    // Set full energy to start
				    thePlayer->pev->fuser3 = 1.0f*kNormalizationNetworkFactor;


				    UTIL_Remove(this);
                }
			}
		}
	}
}


void AvHAmmoPack :: Precache( void )
{
	PRECACHE_UNMODIFIED_MODEL(kAmmoPackModel);
	PRECACHE_UNMODIFIED_SOUND(kAmmoPackPickupSound);
}

void AvHAmmoPack :: Spawn( void )
{
	this->Precache();

	SET_MODEL(ENT(pev), kAmmoPackModel);

	this->pev->movetype = MOVETYPE_TOSS;
	this->pev->solid = SOLID_TRIGGER;
	this->m_flNoTouch = gpGlobals->time + 0.25;

	UTIL_SetSize(pev, kAmmoPackMinSize, kAmmoPackMaxSize); 
	UTIL_SetOrigin( pev, pev->origin );

	SetTouch(&AvHAmmoPack::Touch);
}

void AvHAmmoPack :: Touch( CBaseEntity *inOther)
{
	if(this->m_flNoTouch > gpGlobals->time)
		return;

	//Dont touch non-players
	if(!inOther->IsPlayer())
		return;

	AvHPlayer *thePlayer = dynamic_cast<AvHPlayer*>(inOther);

	//if they dont have a weapon that uses this ammo, dont pick up the ammo pack.
	if ( !(thePlayer->pev->weapons & (1<<m_iWeaponID)) )
		return;

	if( thePlayer->GiveAmmo(this->m_iAmmoAmt, this->m_szAmmoType, this->m_iMaxAmmo) != -1)
	{
		EMIT_SOUND(ENT(thePlayer->pev), CHAN_ITEM, kAmmoPackPickupSound, 1, ATTN_NORM);
		thePlayer->PlaybackNumericalEvent(kNumericalInfoAmmoEvent, 0);
		UTIL_Remove(this);
	}
}

BOOL AvHGenericAmmo::GiveAmmo(CBaseEntity* inOther)
{
	// Give ammo to the player.  It will be added as generic ammo, added as one clip to 
	// the player's current weapon.
	if (inOther->GiveAmmo( 0, kwsGenericAmmo, 0 ) != -1)
	{
		EMIT_SOUND(ENT(inOther->pev), CHAN_ITEM, kAmmoPickupSound, 1, ATTN_NORM);
		
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inOther);
		if(thePlayer)
		{
			// Just say "ammo received" instead of number of bullets, too confusing
			thePlayer->PlaybackNumericalEvent(kNumericalInfoAmmoEvent, 0);
		}
		
		return TRUE;
	}

	return FALSE;
}

BOOL AvHGenericAmmo::AddAmmo( CBaseEntity *pOther ) 
{ 
	if(this->mDropped)
	{
		return GiveAmmo(pOther);
	}

	return FALSE;
}

void AvHGenericAmmo::Dropped(void)
{
	this->mDropped = true;
	SetThink(NULL);

	// Expire after a time
	SetThink(&AvHGenericAmmo::SUB_Remove);
	this->pev->nextthink = gpGlobals->time + AvHSUGetWeaponStayTime();
}

void AvHGenericAmmo::Precache( void )
{
	PRECACHE_UNMODIFIED_MODEL(kAmmoModel);
	PRECACHE_UNMODIFIED_SOUND(kAmmoPickupSound);
}

void AvHGenericAmmo::Spawn( void )
{ 
	Precache( );
	SET_MODEL(ENT(pev), kAmmoModel);
	UTIL_SetSize(pev, kAmmoMinSize, kAmmoMaxSize); 
	
	CBasePlayerAmmo::Spawn( );
	this->mDropped = false;
	SetThink(&AvHGenericAmmo::Dropped);
	this->pev->nextthink = gpGlobals->time + .15f;

	this->pev->iuser3 = AVH_USER3_MARINEITEM;
}


const float kScanThinkTime = .5f;

AvHScan::AvHScan()
{
}

void AvHScan::Precache(void)
{
	CBaseAnimating::Precache();
	
	PRECACHE_UNMODIFIED_MODEL(kScanModel);
	PRECACHE_UNMODIFIED_SOUND(kScanSound);
}

void AvHScan::Spawn(void)
{
//	this->Precache();
//	CBaseAnimating::Spawn();
//	
//	SET_MODEL(ENT(this->pev), kScanModel);
//	
//	this->pev->movetype = MOVETYPE_NONE;
//	this->pev->solid = SOLID_NOT;
//	
//    this->pev->classname = MAKE_STRING(kwsScan);
//	
//	this->pev->takedamage = DAMAGE_NO;
//	
//	// Start animating
//	this->pev->sequence	= 0;
//	this->pev->frame = 0;
//	ResetSequenceInfo();
	
	this->pev->effects |= (/*EF_BRIGHTFIELD |*/ EF_DIMLIGHT);
	
	this->mTimeCreated = gpGlobals->time;

	SetThink(&AvHScan::ScanThink);
	this->pev->nextthink = gpGlobals->time + GetGameRules()->GetFirstScanThinkTime();
	
	EMIT_SOUND(this->edict(), CHAN_AUTO, kScanSound, 1.0f, ATTN_NORM);

	AvHSUPlayParticleEvent(kpsScanEffect, this->edict(), this->pev->origin);
}

void AvHScan::ScanThink()
{
	// Remove cloaking from nearby enemies
	FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		if(theEntity->GetIsRelevant() && (theEntity->pev->team != this->pev->team))
		{
			// Check that entity is in range of scan
			float theDistance = VectorDistance(theEntity->pev->origin, this->pev->origin);
			if(theDistance < BALANCE_VAR(kScanRadius))
			{
				// Remove cloaking, if player has it
				theEntity->TriggerUncloak();
			}
		}
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName)

	// Look in sphere for cloakables
	CBaseEntity* theSphereEntity = NULL;
	while ((theSphereEntity = UTIL_FindEntityInSphere(theSphereEntity, this->pev->origin, BALANCE_VAR(kScanRadius))) != NULL)
	{
		if(!AvHSUGetIsExternalClassName(STRING(theSphereEntity->pev->classname)))
		{
			// TODO: Check team here?
			AvHCloakable* theCloakable = dynamic_cast<AvHCloakable*>(theSphereEntity);
			if(theCloakable)
			{
				theCloakable->Uncloak();
			}
		}
	}

	float theScanTime = GetGameRules()->GetBuildTimeForMessageID(BUILD_SCAN);
	if(gpGlobals->time < (this->mTimeCreated + theScanTime))
	{
		this->pev->nextthink = gpGlobals->time + kScanThinkTime;
	}
	else
	{
		UTIL_Remove(this);
	}
}



AvHPhaseGate::AvHPhaseGate() : AvHMarineBaseBuildable(TECH_PHASE_GATE, BUILD_PHASEGATE, kwsPhaseGate, AVH_USER3_PHASEGATE)
{
	this->mEnabled = false;
	this->mTimeOfLastDeparture=0.0f;
	this->mHasWarmedUp=false;
}

int	AvHPhaseGate::GetSequenceForBoundingBox() const
{
	return 1;
}

void AvHPhaseGate::Killed(entvars_t* inAttacker, int inGib)
{
	this->SetEnabled(false);
	
	AvHMarineBaseBuildable::Killed(inAttacker, inGib);

	GetGameRules()->MarkDramaticEvent(kPGDeathPriority, this->entindex(), inAttacker);
}

void AvHPhaseGate::Precache(void)
{
	CBaseAnimating::Precache();
	
	PRECACHE_UNMODIFIED_MODEL(kPhaseGateModel);
	PRECACHE_UNMODIFIED_SOUND(kPhaseGateSound);
	PRECACHE_UNMODIFIED_SOUND(kPhaseGateTransportSound);
}

void AvHPhaseGate::SetHasBeenBuilt()
{
	AvHBuildable::SetHasBeenBuilt();

	// Include a "warm-up" time so movement chambers don't teleport the player immediately
	this->mTimeOfLastDeparture=gpGlobals->time;
	SetThink(&AvHPhaseGate::IdleThink);

	this->pev->nextthink = gpGlobals->time + kBuildingUseWarmupTime;
}

int	AvHPhaseGate::GetIdleAnimation() const
{
	return 0;
}

void AvHPhaseGate::IdleThink()
{
	bool theIsEnabled = false;
	this->mHasWarmedUp=true;
	// Check if there are any other phase gates on our team and set our enabled state accordingly
	bool theDone = false;
	bool potentialDeadlock=false;
	int theNumPhaseGates=0;
	if(this->GetIsBuilt() && !this->GetIsRecycling())
	{
		edict_t* thePhaseGateEdict = ENT(this->pev);
		
		// Keep looping until we come back to ourself or we find another gate on our team to phase to
		while(!theDone)
		{
			thePhaseGateEdict = FIND_ENTITY_BY_CLASSNAME(thePhaseGateEdict, kwsPhaseGate);
			AvHPhaseGate* thePhaseGate = dynamic_cast<AvHPhaseGate*>(CBaseEntity::Instance(thePhaseGateEdict));
			
			// This assert fails in normal operation, seemingly harmlessly
			//ASSERT(thePhaseGate);
			
			if(thePhaseGateEdict == ENT(this->pev))
			{
				theDone = true;
			}
			else if((thePhaseGateEdict->v.team == this->pev->team) && thePhaseGate && thePhaseGate->GetIsBuilt() && !thePhaseGate->GetIsRecycling() && thePhaseGate->HasWarmedUp() )
			{
				theIsEnabled = true;
				theDone = true;
			}
			theNumPhaseGates++;
		}
	}

	AvHBaseBuildable::AnimateThink();

	this->SetEnabled(theIsEnabled);

	this->pev->nextthink = gpGlobals->time + kPhaseGateIdleThink;
}

void AvHPhaseGate::ResetEntity()
{
	this->SetEnabled(false);

	AvHMarineBaseBuildable::ResetEntity();
}

void AvHPhaseGate::SetEnabled(bool inEnabledState)
{
	if(inEnabledState != this->mEnabled)
	{
		if(inEnabledState)
		{
			// Start animating
			this->pev->sequence	= 0;
			this->pev->frame = 0;
			ResetSequenceInfo();
			
			//this->pev->effects |= EF_BRIGHTLIGHT;
			this->pev->effects |= EF_DIMLIGHT;
		
			UTIL_EmitAmbientSound( ENT(this->pev), this->pev->origin, kPhaseGateSound, 1.0f, ATTN_NORM, 0, 100);

			SetUse(&AvHPhaseGate::TeleportUse);
		
			this->mEnabled = true;
		}
		else
		{
			// Stop animating
			this->pev->sequence	= 1;
			this->pev->frame = 0;
			ResetSequenceInfo();
		
			this->pev->effects &= ~EF_DIMLIGHT;
		
			UTIL_EmitAmbientSound(ENT(this->pev), this->pev->origin, kPhaseGateSound, 1.0f, .5, SND_STOP, 100);

			SetUse(NULL);
		
			this->mEnabled = false;
		}
	}
}

bool AvHPhaseGate::GetAreTeammatesBlocking(AvHPlayer* inPlayer, const Vector& inOrigin) const
{
	
    // This is based off the logic of AvHSUKillPlayersTouchingPlayer so that
    // the results are consistent.
	
    bool theResult = false;

    FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
	if(theEntity->GetIsRelevant() && theEntity->pev->team == inPlayer->pev->team)
	{
		float theDistanceToPlayer = VectorDistance(inOrigin, theEntity->pev->origin);
		if(theDistanceToPlayer < 30)
		{
            if (gpGlobals->time - theEntity->GetTimeOfLastTeleport() < kPhaseGateAmnestyTime)
            {
                theResult = true;
            }
		}
	}
	END_FOR_ALL_ENTITIES(kAvHPlayerClassName)

    return theResult;

}

void AvHPhaseGate::KillBuildablesTouchingPlayer(AvHPlayer* inPlayer, entvars_t* inInflictor)
{

    FOR_ALL_BASEENTITIES();
	AvHBaseBuildable* theBuildable = dynamic_cast<AvHBaseBuildable*>(theBaseEntity);
	if(theBuildable)
	{
        if (theBuildable->pev->iuser3 != AVH_USER3_HIVE && theBuildable->pev->iuser3 != AVH_USER3_PHASEGATE)
        {
	        float theDistanceToPlayer = VectorDistance(inPlayer->pev->origin, theBuildable->pev->origin);
		    if(theDistanceToPlayer < 50)
		    {
			    theBuildable->TakeDamage(inInflictor, theBuildable->pev, 10000, DMG_GENERIC);
		    }
        }
    }
    END_FOR_ALL_BASEENTITIES();

}
bool AvHPhaseGate::HasWarmedUp() const
{
	return this->mHasWarmedUp;
}
bool AvHPhaseGate::GetIsEnabled() const
{
	return this->GetIsBuilt() && this->mEnabled && !this->GetIsRecycling();
}
void AvHPhaseGate::SetTimeOfLastDeparture(float timeOfLastDeparture) 
{
	mTimeOfLastDeparture=timeOfLastDeparture;
}
bool AvHPhaseGate::IsReadyToUse()
{
	bool theReturn=false;
	if ( (gpGlobals->time - mTimeOfLastDeparture) > BALANCE_VAR(kPhaseGateDepartureInterval) ) 
	{
		theReturn=true;
	}
	return theReturn;
}

void AvHPhaseGate::TeleportUse(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(pActivator);
	CBasePlayerItem* thePlayerItem = dynamic_cast<CBasePlayerItem*>(pActivator);
	
	// Only players on the team of the person that created the phase gate can travel through,
	bool theTeleportAllowed = (thePlayer != NULL) && (thePlayer->pev->team == this->pev->team) && thePlayer->GetIsAbleToAct();
	//bool theTeleportAllowed = thePlayerItem || (thePlayer && ((pActivator->pev->team == this->pev->team) && (this->pev->team != 0)));
	if(theTeleportAllowed && this->GetIsEnabled())
	{
		float theLastTeleportTime = thePlayer->GetTimeOfLastTeleport();
		theTeleportAllowed = (theLastTeleportTime == -1) || ((gpGlobals->time - theLastTeleportTime) >= BALANCE_VAR(kPhaseGateDelay));
		if(theTeleportAllowed)
		{
			if(!GetGameRules()->GetIsTesting())
			{
				// Teleport to "next" gate.  If there isn't more then one gate, respawn the player
				vec3_t theOrigin;
				
				bool theDone = false;
				bool theSuccess = false;
				edict_t* thePhaseGateEdict = ENT(this->pev);
				AvHPhaseGate *theTargetGate=0;
				// Keep looping until we come back to ourself or we find another gate on our team to phase to
				while(!theDone)
				{
					thePhaseGateEdict = FIND_ENTITY_BY_CLASSNAME(thePhaseGateEdict, kwsPhaseGate);
					AvHPhaseGate* thePhaseGate = dynamic_cast<AvHPhaseGate*>(CBaseEntity::Instance(thePhaseGateEdict));

					// This assert fails in normal operation, seemingly harmlessly
					//ASSERT(thePhaseGate);
					
					if(thePhaseGateEdict == ENT(this->pev))
					{
						theDone = true;
					}
					else if((thePhaseGateEdict->v.team == this->pev->team) && thePhaseGate && thePhaseGate->GetIsEnabled() && this->IsReadyToUse() )
					{
						// Players come through on top of phase gate, plus a little above
						theOrigin = thePhaseGateEdict->v.origin;
					
						theOrigin.z +=kRespawnFudgeFactorHeight;

						// Add in proper hull size so players don't get stuck
						Vector thePlayerMinSize, thePlayerMaxSize;						
						thePlayer->GetSize(thePlayerMinSize, thePlayerMaxSize);
						theOrigin.z += -thePlayerMinSize.z;

						//Elven Thief - Changed from AvHUGetHull(false to true to allow crouching marines access to phase gates.
						// check out bug 487
                        if(AvHSUGetIsEnoughRoomForHull(theOrigin, AvHMUGetHull(true, thePlayer->pev->iuser3), thePlayer->edict(), true, true))
						{
							theSuccess = true;
							theDone = true;
						}
                        
                        // Now check to see if there are any teammates blocking who just phased in.
                        
                        if (GetAreTeammatesBlocking(thePlayer, theOrigin))
                        {
                            theSuccess = false;
                            theDone = true;
                        }

						if ( theDone == true )
						{
							theTargetGate=thePhaseGate;
						}


					}
				}
				
				if(theSuccess)
				{
					// Mark the player as just having teleported so he doesn't teleport immediately again
					thePlayer->SetPosition(theOrigin);

					thePlayer->pev->velocity = g_vecZero;
					thePlayer->SetTimeOfLastTeleport(gpGlobals->time);
					
					int theFlags = 0;//thePlayer ? FEV_NOTHOST : 0;
					this->SetTimeOfLastDeparture(gpGlobals->time);
					AvHSUPlayPhaseInEffect(theFlags, this, thePlayer);

					AvHSUKillPlayersTouchingPlayer(thePlayer, this->pev);
                    KillBuildablesTouchingPlayer(thePlayer, this->pev);
					
					Vector theFadeColor;
					theFadeColor.x = 235;
					theFadeColor.y = 255;
					theFadeColor.z = 255;
					UTIL_ScreenFade(thePlayer, theFadeColor, .9f, 0.0f, 255, FFADE_IN);

    			}
			}
		}
	}
}

void AvHPhaseGate::UpdateOnRecycle(void)
{
	this->SetEnabled(false);
}

void AvHPhaseGate::UpdateOnRemove(void)
{
	// Make sure sound gets turned off when round ends without it being killed
	if(this->pev)
	{
		UTIL_EmitAmbientSound(ENT(this->pev), this->pev->origin, kPhaseGateSound, 1.0f, .5, SND_STOP, 100);
	}
}


//AvHSiegeTurret::AvHSiegeTurret()
//{
//	float theStartTime = RANDOM_FLOAT(0, avh_siegerof.value);
//	this->mTimeLastFired = gpGlobals->time - theStartTime;
//	this->mTimeToConstruct = GetGameRules()->GetBuildTimeForMessageID(BUILD_SIEGE);
//}
//
//CBaseEntity* AvHSiegeTurret::BestVisibleEnemy(void)
//{
//	CBaseEntity *theCurrentEnemy = NULL;
//	CBaseEntity *theBestEnemy = NULL;
//	float theDistanceToBestEnemy = -1;
//	
//	while((theCurrentEnemy = UTIL_FindEntityInSphere(theCurrentEnemy, this->pev->origin, avh_siegemaxrange.value)) != NULL)
//	{
//		// If entity is a valid target and within our valid range
//		if(this->GetIsValidTarget(theCurrentEnemy))
//		{
//			// Is it closer than our current target?
//			float theDistanceToCurrentEnemy = AvHSUEyeToBodyDistance(this->pev, theCurrentEnemy);
//			if((theDistanceToBestEnemy == -1) || (theDistanceToCurrentEnemy < theDistanceToBestEnemy))
//			{
//				theBestEnemy = theCurrentEnemy;
//				theDistanceToBestEnemy = theDistanceToCurrentEnemy;
//			}
//		}
//	}
//	return theBestEnemy;
//}
//
//bool AvHSiegeTurret::GetIsValidTarget(CBaseEntity* inEntity) const
//{
//	bool theTargetIsValid = false;
//	
//	if(AvHDeployedTurret::GetIsValidTarget(inEntity))
//	{
//		float theDistanceToCurrentEnemy = AvHSUEyeToBodyDistance(this->pev, inEntity);
//		if(theDistanceToCurrentEnemy >= this->GetMinimumRange())
//		{
//			// We have to see it as well
//			Vector vecMid = this->pev->origin + this->pev->view_ofs;
//			Vector vecMidEnemy = inEntity->BodyTarget(vecMid);
//			if(FBoxVisible(this->pev, inEntity->pev, vecMidEnemy))
//			{
//				theTargetIsValid = true;
//			}
//		}
//	}
//	return theTargetIsValid;
//}
//
//char* AvHSiegeTurret::GetActiveSound() const
//{
//	return kSiegeActive;
//}
//
//char* AvHSiegeTurret::GetAlertSound() const
//{
//	return kSiegeAlert;
//}
//
//char* AvHSiegeTurret::GetDeploySound() const
//{
//	return kSiegeDeploy;
//}
//
//char* AvHSiegeTurret::GetPingSound() const
//{
//	return kSiegePing;
//}
//
//int AvHSiegeTurret::GetPointValueOfTarget(void) const
//{
//	return 3;
//}
//
//int	AvHSiegeTurret::GetMinimumRange() const
//{
//	return avh_siegeminrange.value;
//}
//
//bool AvHSiegeTurret::NeedsLineOfSight() const
//{
//	return true;
//}
//
//void AvHSiegeTurret::Precache(void)
//{
//	AvHDeployedTurret::Precache();
//	
//	PRECACHE_MODEL(kSiegeTurretModel);
//	PRECACHE_SOUND(kSiegeTurretFire1);
//}
//
//
//void AvHSiegeTurret::Shoot(Vector &vecSrc, Vector &vecDirToEnemy)
//{
//	// Only fire once every few seconds...this is hacky but there's no way to override think functions so it must be done
//	// I wish it was easier to change the update rate but it's not so...
//	if((gpGlobals->time - this->mTimeLastFired) > avh_siegerof.value)
//	{
//		// Find enemy player in range, ignore walls and everything else
//		if(this->m_hEnemy)
//		{
//			if(this->GetIsValidTarget(this->m_hEnemy))
//			{
//				// Apply damage, taking upgrade into account
//				float theDamageMultiplier;
//				AvHPlayerUpgrade::GetWeaponUpgrade(this->pev->iuser4, &theDamageMultiplier);
//				float theDamage = theDamageMultiplier*avh_siegedamage.value;
//				
//				if(!GetGameRules()->GetIsTesting())
//				{
//					::RadiusDamage(this->m_hEnemy->pev->origin, this->pev, this->pev, theDamage, avh_siegesplashradius.value, CLASS_NONE, DMG_BLAST);
//				}
//				
//				// Play fire sound
//				EMIT_SOUND_DYN(ENT(this->pev), CHAN_AUTO, kSiegeTurretFire1, 1.0, ATTN_NORM, 0, PITCH_NORM);
//				
//				this->pev->effects &= ~EF_MUZZLEFLASH;
//				
//				// Send normal effect to all
//				PLAYBACK_EVENT_FULL(0, this->m_hEnemy->edict(), gSiegeHitEventID, 0, this->m_hEnemy->pev->origin, this->m_hEnemy->pev->angles, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
//				
//				if(this->m_hEnemy->IsPlayer())
//				{
//					// Send personal view shake to recipient only (check for splash here, pass param to lessen effect for others?)
//					// TODO: Use upgrade level to parameterize screen shake and fade?
//					PLAYBACK_EVENT_FULL(FEV_HOSTONLY, this->m_hEnemy->edict(), gSiegeViewHitEventID, 0, this->m_hEnemy->pev->origin, this->m_hEnemy->pev->angles, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
//					
//					Vector theFadeColor;
//					theFadeColor.x = 255;
//					theFadeColor.y = 100;
//					theFadeColor.z = 100;
//					UTIL_ScreenFade(this->m_hEnemy, theFadeColor, .3f, 0.0f, 255, FFADE_OUT);
//				}
//			}
//			else
//			{
//				this->m_hEnemy = NULL;
//			}
//		}
//
//		this->mTimeLastFired = gpGlobals->time;
//	}
//}
//
//void AvHSiegeTurret::Spawn()
//{
//	this->Precache();
//	AvHDeployedTurret::Spawn();
//	
//	SET_MODEL(ENT(this->pev), kSiegeTurretModel);
//	UTIL_SetSize(pev, kSiegeMinSize, kSiegeMaxSize); 
//	
//    this->pev->classname = MAKE_STRING(kwsSiegeTurret);
//	
//	this->pev->takedamage = 1;
//	this->pev->health = (int)(avh_siegehealth.value);
//	this->pev->armorvalue = (int)(avh_siegehealth.value);
//}



AvHMarineBaseBuildable::AvHMarineBaseBuildable(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inUser4) : AvHBaseBuildable(inTechID, inMessageID, inClassName, inUser4)
{
	this->SetEnergy(0.0f);
}

char* AvHMarineBaseBuildable::GetDeploySound() const
{
	return kMarineBuildingDeploy;
}

bool AvHMarineBaseBuildable::GetIsTechnologyAvailable(AvHMessageID inMessageID) const
{
	bool theIsAvailable = AvHBaseBuildable::GetIsTechnologyAvailable(inMessageID);
	
	// Disable scanning if not enough energy
	if(theIsAvailable && AvHSHUGetDoesTechCostEnergy(inMessageID))
	{
		int theEnergyCost = GetGameRules()->GetCostForMessageID(inMessageID);
		if(this->mEnergy < theEnergyCost)
		{
			theIsAvailable = false;
		}
	}
	
	return theIsAvailable;
}

char* AvHMarineBaseBuildable::GetKilledSound() const
{
	return kMarineBuildingKilled;
}

int	AvHMarineBaseBuildable::GetPointValue() const
{
	return BALANCE_VAR(kScoringMarineBuildableValue);
}

int AvHMarineBaseBuildable::GetTakeDamageAnimation() const
{
    return -1;
}

void AvHMarineBaseBuildable::ResetEntity()
{
	AvHBaseBuildable::ResetEntity();
}

void AvHMarineBaseBuildable::SetEnergy(float inEnergy)
{
	this->mEnergy = max(min(inEnergy, kMarineStructureMaxEnergy), 0.0f);
	
	float theNormValue = this->mEnergy/kMarineStructureMaxEnergy;

	if(this->pev && this->GetIsBuilt())
	{
		AvHSHUSetEnergyState(this->pev->iuser3, this->pev->fuser1, theNormValue);
	}
}

int AvHMarineBaseBuildable::TakeDamage(entvars_t* inInflictor, entvars_t* inAttacker, float inDamage, int inBitsDamageType)
{
	//UTIL_Sparks()

	return AvHBaseBuildable::TakeDamage(inInflictor, inAttacker, inDamage, inBitsDamageType);
}

void AvHMarineBaseBuildable::TechnologyBuilt(AvHMessageID inMessageID)
{
	// Pay energy cost
	if(AvHSHUGetDoesTechCostEnergy(inMessageID))
	{
		int theEnergyCost = GetGameRules()->GetCostForMessageID(inMessageID);
		float theNewEnergy = max(this->mEnergy - theEnergyCost, 0.0f);
		this->SetEnergy(theNewEnergy);

		// Play animation?
		this->PlayAnimationAtIndex(this->GetActiveAnimation(), true);
	}
}

// Marine buildings
const float kNukeThinkInterval = 1.5f;

AvHNuke::AvHNuke() : AvHMarineBaseBuildable(TECH_NULL, BUILD_NUKE, kwsNuke, AVH_USER3_NUKE)
{
}

void AvHNuke::Precache()
{
	AvHMarineBaseBuildable::Precache();
	PRECACHE_UNMODIFIED_SOUND(kNukeActive);
	PRECACHE_UNMODIFIED_SOUND(kNukeExplode);
}

void AvHNuke::ActiveThink()
{
	float theThinkInterval = kNukeThinkInterval;

	// If not active
	if(!this->mActive)
	{
		// Set construction complete
		this->SetConstructionComplete();

		// Play sounds

		// Set active
		this->mActive = true;

		// Mark as a monster
		SetBits(this->pev->flags, FL_MONSTER);

		this->mTimeActivated = gpGlobals->time;
	}
	
	// Emit higher and higher frequency sound
	float theTimeToDetonate = GetGameRules()->GetBuildTimeForMessageID(this->GetMessageID());
	
	float thePercentDone = (gpGlobals->time - this->mTimeActivated)/theTimeToDetonate;
	thePercentDone = min(max(0.0f, thePercentDone), 1.0f);
	
	int thePitch = 100 + thePercentDone*3;
	EMIT_SOUND_DYN(ENT(pev), CHAN_BODY, kNukeActive, 1.0, ATTN_NORM, 0, thePitch);
	
	// If active
	if(this->mActive)
	{
		// If enough time has passed
		ASSERT(this->mTimeActivated != -1);
		
		if(gpGlobals->time > (this->mTimeActivated + theTimeToDetonate))
		{
			// Detonation visuals (any bigger magnitude then this and the explosion doesn't show up)
			int theMagnitude = 300;
			ExplosionCreate(this->pev->origin, this->pev->angles, this->edict(), theMagnitude, FALSE, this->pev->team);
			
			// Do damage (theDamage at epicenter, falls off from there, doesn't hurt people in water, only hurts people that can be seen by blast)
			float theDamage = kNukeDamage;
			float theRadius = kNukeRange;
			::RadiusDamage(this->pev->origin, this->pev, this->pev, theDamage, theRadius, CLASS_NONE, DMG_BLAST);
			
			// Play view shake here
			float theShakeAmplitude = 100;
			float theShakeFrequency = 150;
			float theShakeDuration = 10.0f;
			float theShakeRadius = 2000;
			UTIL_ScreenShake(this->pev->origin, theShakeAmplitude, theShakeFrequency, theShakeDuration, theShakeRadius);
			
			// Add white out effect for players in radius
			FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
				if(theEntity->GetIsRelevant() && !theEntity->GetIsInTopDownMode())
				{
					// If near the epicenter, or if it's visible, we're blinded
					bool theNearEpicenter = false;
					float theDistance = VectorDistance(theEntity->pev->origin, this->pev->origin);
					if(theDistance < theRadius/2)
					{
						theNearEpicenter = true;
					}

					bool theExplosionVisible = false;
					if(theEntity->GetIsEntityInSight(this))
					{
						theExplosionVisible = true;
					}

					if(theNearEpicenter || theExplosionVisible)
					{
						Vector theFadeColor;
						theFadeColor.x = 255;
						theFadeColor.y = 255;
						theFadeColor.z = 255;
						UTIL_ScreenFade(theEntity, theFadeColor, 1.5f, .5f, 255, FFADE_IN);
					}
				}
			END_FOR_ALL_ENTITIES(kAvHPlayerClassName)

			//this->pev->flags |= EF_NODRAW;
			SET_MODEL(ENT(pev), kNullModel);
			this->pev->flags |= EF_BRIGHTFIELD;
			this->pev->flags |= EF_BRIGHTLIGHT;
				
			// Remove entity
			SetThink(&AvHNuke::DeathThink);
			theThinkInterval = theShakeDuration;

			// Play special nuke sound (not playing for some reason)
			EMIT_SOUND(this->edict(), CHAN_BODY, kNukeExplode, 1.0f, ATTN_IDLE);
		}
	}
	
	this->pev->nextthink = gpGlobals->time + theThinkInterval;
}

void AvHNuke::DeathThink()
{
	UTIL_Remove(this);
}

void AvHNuke::Spawn()
{
	AvHBaseBuildable::Spawn();
	
	// Set ActiveThink
	this->mActive = false;
	this->mTimeActivated = -1;

	SetThink(&AvHNuke::ActiveThink);
	this->pev->nextthink = gpGlobals->time + kNukeThinkInterval;
}

char* AvHNuke::GetDeploySound() const
{
	return kNukeDeploy;
}

char* AvHNuke::GetKilledSound() const
{
	return kNukeKilled;
}


const float kInfantryPortalThinkTime = 1.0f;
#define kInfantryPortalLightEffect EF_LIGHT

AvHInfantryPortal::AvHInfantryPortal() : AvHMarineBaseBuildable(TECH_INFANTRYPORTAL, BUILD_INFANTRYPORTAL, kwsInfantryPortal, AVH_USER3_INFANTRYPORTAL)
{
}

void AvHInfantryPortal::Killed(entvars_t* inAttacker, int inGib)
{
	AvHBaseBuildable::Killed(inAttacker, inGib);

	GetGameRules()->MarkDramaticEvent(kIPDeathPriority, this->entindex(), inAttacker);
}

float AvHInfantryPortal::GetReinforceTime() const
{
	float theReinforceTime = BALANCE_VAR(kMarineRespawnTime);

	if(GetGameRules()->GetCheatsEnabled())
	{
		theReinforceTime = 2;
	}

	theReinforceTime = max(0.0f, theReinforceTime);

	return theReinforceTime;
}

void AvHInfantryPortal::PortalThink()
{
	this->UpdateReinforcements();

	AvHBaseBuildable::AnimateThink();

	this->pev->nextthink = gpGlobals->time + kInfantryPortalThinkTime; 
}


void AvHInfantryPortal::ResetEntity()
{
	AvHMarineBaseBuildable::ResetEntity();
	AvHReinforceable::ResetEntity();
}

void AvHInfantryPortal::SetHasBeenBuilt()
{
	AvHBuildable::SetHasBeenBuilt();

	SetThink(&AvHInfantryPortal::PortalThink);
	this->pev->nextthink = gpGlobals->time + kInfantryPortalThinkTime; 
	this->pev->effects |= kInfantryPortalLightEffect;
}

void AvHInfantryPortal::Precache()
{
	AvHMarineBaseBuildable::Precache();

	PRECACHE_UNMODIFIED_SOUND(kTransportSound);
}


void AvHInfantryPortal::CueRespawnEffect(AvHPlayer* inPlayer)
{
	// Playback teleporting event
	PLAYBACK_EVENT_FULL(0, inPlayer->edict(), gTeleportEventID, 0, inPlayer->pev->origin, inPlayer->pev->angles, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );

	EMIT_SOUND(ENT(this->pev), CHAN_AUTO, kTransportSound, .8, ATTN_NORM);
}

bool AvHInfantryPortal::GetCanReinforce() const
{
	return this->GetIsBuilt() && !GetGameRules()->GetIsCombatMode();
}

bool AvHInfantryPortal::GetSpawnLocationForPlayer(CBaseEntity* inPlayer, Vector& outLocation) const
{
	// Set player position to on top of gate
	vec3_t thePosition = AvHSHUGetRealLocation(this->pev->origin, this->pev->mins, this->pev->maxs);

	Vector theIPMinSize, theIPMaxSize;
	AvHSHUGetSizeForTech(BUILD_INFANTRYPORTAL, theIPMinSize, theIPMaxSize);
	thePosition.z += theIPMaxSize.z;

	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inPlayer);
	ASSERT(thePlayer);

	// Respawn player on top of portal
	Vector thePlayerMinSize, thePlayerMaxSize;						
	thePlayer->GetSize(thePlayerMinSize, thePlayerMaxSize);

	thePosition.z += (-thePlayerMinSize.z + kRespawnFudgeFactorHeight);
	
	VectorCopy(thePosition, outLocation);
	
	return true;
}

AvHTeamNumber AvHInfantryPortal::GetReinforceTeamNumber() const
{
	return this->GetTeamNumber();
}

void AvHInfantryPortal::ResetReinforcingPlayer(bool inSuccess)
{
	// If we're respawning, we telefrag.  Make sure to telefrag after the player's new location has been set
	bool theTelefrag = false;
	AvHPlayer* thePlayer = this->GetReinforcingPlayer();
	if(inSuccess && thePlayer)
	{
		theTelefrag = true;
	}

	AvHReinforceable::ResetReinforcingPlayer(inSuccess);

	if(theTelefrag)
	{
		AvHSUKillPlayersTouchingPlayer(thePlayer, this->pev);
	}
}

void AvHInfantryPortal::UpdateOnRecycle(void)
{
	this->ResetReinforcingPlayer(false);
	this->pev->effects &= ~kInfantryPortalLightEffect;
}

void AvHInfantryPortal::UpdateOnRemove(void)
{
	this->ResetReinforcingPlayer(false);
	SetThink(NULL);
}

int AvHInfantryPortal::GetIdleAnimation() const
{
	// AvHBaseBuildable randomly shows Idle2, which we don't have in this model.
	return 2;
}	

int AvHInfantryPortal::GetIdle1Animation() const
{
	// AvHBaseBuildable randomly shows Idle2, which we don't have in this model.
	return 2;
}	

int AvHInfantryPortal::GetIdle2Animation() const
{
	// AvHBaseBuildable randomly shows Idle2, which we don't have in this model.
	return 2;
}	

// tankefugl: 
int AvHInfantryPortal::GetDeployAnimation() const
{
	return 0;
}

int AvHInfantryPortal::GetSpawnAnimation() const
{
	return 1;
}
// :tankefugl

const int kCommandStationExitAnimation = 12;

AvHCommandStation::AvHCommandStation() : AvHMarineBaseBuildable(TECH_COMMAND_CENTER, BUILD_COMMANDSTATION, kwsTeamCommand, AVH_USER3_COMMANDER_STATION)
{
	this->mCommanderAtThisStation = -1;
	this->mTimeToPlayOnlineSound = -1;
}

int	AvHCommandStation::GetIdleAnimation() const
{
	int theAnimation = 2;

	// If we're in use
	if(this->mCommanderAtThisStation != -1 || GetGameRules()->GetIsCombatMode())
	{
		theAnimation = 3;
	}

	return theAnimation;
}

int AvHCommandStation::GetPointValue() const
{
	return BALANCE_VAR(kScoringCCValue);
}

void AvHCommandStation::Killed( entvars_t *pevAttacker, int iGib )
{
    this->SetInactive();

    AvHMarineBaseBuildable::Killed(pevAttacker, iGib);
}

int	AvHCommandStation::ObjectCaps( void ) 
{ 
	
    // Recycled command stations cannot be used.

    int theObjectCaps = 0;
    
    if ( !(pev->effects & EF_NODRAW) )
    {
        theObjectCaps |= FCAP_CONTINUOUS_USE;
    }

    return theObjectCaps;

}

void AvHCommandStation::CommandTouch(CBaseEntity* pOther)
{
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(pOther);
	if(thePlayer)
	{
		AvHTeamNumber theStationTeamNumber = this->GetTeamNumber();
		if(thePlayer->pev->team == theStationTeamNumber)
		{
			// Check to see if we already have a commander and don't make the person a commander if so
			if(GetGameRules()->GetNumCommandersOnTeam(theStationTeamNumber) == 0)
			{
				thePlayer->SendMessage(kHelpTextCSAttractMode, TOOLTIP);
			}
		}
	}
}

void AvHCommandStation::CommandUse( CBaseEntity* pActivator, CBaseEntity* pCaller, USE_TYPE useType, float value )
{
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(pActivator);

	// Mapper-placed CCs can be killed but they don't go away
	if(thePlayer && !(thePlayer->pev->flags & FL_FAKECLIENT) && !this->GetHasBeenKilled() && thePlayer->GetIsAbleToAct())
	{
		AvHTeam* theTeam = thePlayer->GetTeamPointer();
		if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_MARINE))
		{
			AvHTeamNumber theStationTeamNumber = this->GetTeamNumber();
			if(thePlayer->pev->team == theStationTeamNumber)
			{
				// Check to see if we already have a commander and don't make the person a commander if so
				if(GetGameRules()->GetNumCommandersOnTeam(theStationTeamNumber) == 0)
				{
					string theErrorMessage;
					if(thePlayer->GetCanCommand(theErrorMessage))
					{
						if(this->pev->health > 0 && !this->GetIsRecycling())
						{
							thePlayer->SetUser3(AVH_USER3_COMMANDER_PLAYER);
							//thePlayer->SetSelection(this->entindex());
						
							EMIT_SOUND(ENT(this->pev), CHAN_AUTO, kCommandStationStartSound, .8, kCommandStationAttenuation);
						
							SetThink(&AvHCommandStation::CommanderUsingThink);
							this->pev->nextthink = gpGlobals->time + kCommandStationThinkInterval;
							
							const char* theTarget = (theStationTeamNumber == TEAM_ONE) ? kTargetCommandStationUseTeamOne : kTargetCommandStationUseTeamTwo;
							FireTargets(theTarget, NULL, NULL, USE_TOGGLE, 0.0f);
						
							this->mCommanderAtThisStation = thePlayer->entindex();

							this->PlayAnimationAtIndex(5, true);
							
							this->mTimeToPlayOnlineSound = this->GetTimeAnimationDone();

							GetGameRules()->MarkDramaticEvent(kCCNewCommanderPriority, thePlayer, this);
						}
						else
						{
							thePlayer->SendMessage(kCommandStationDestroyed, TOOLTIP);
						}
					}
					else
					{
						thePlayer->SendMessage(theErrorMessage.c_str());
					}
				}
				else
				{
					// The player somehow touches the command station while still a commander
					if(thePlayer->GetUser3() != AVH_USER3_COMMANDER_PLAYER)
					{
						thePlayer->SendMessage(kCommandStationInUse, TOOLTIP);
					}
				}
			}
			else
			{
				thePlayer->SendMessage(kCommandStationForOtherTeam, TOOLTIP);
			}
		}
	}
}

void AvHCommandStation::Precache(void)
{
	AvHMarineBaseBuildable::Precache();

	PRECACHE_UNMODIFIED_MODEL(kCommandStationModel);
	PRECACHE_UNMODIFIED_SOUND(kCommandStationStartSound);
	PRECACHE_UNMODIFIED_SOUND(kCommandStationEndSound);
}

void AvHCommandStation::ActivateThink(void)
{
	// Don't allow use of the Command station in Combat mode
	if(!GetGameRules()->GetIsCombatMode())
	{
		SetTouch(&AvHCommandStation::CommandTouch);
		SetUse(&AvHCommandStation::CommandUse);
	}
	
	SetThink(&AvHBaseBuildable::AnimateThink);
	this->pev->nextthink = gpGlobals->time + .1f;
}

void AvHCommandStation::SetHasBeenBuilt()
{
	AvHBuildable::SetHasBeenBuilt();

	SetThink(&AvHCommandStation::ActivateThink);
	this->pev->nextthink = gpGlobals->time + 2.0f;
}

void AvHCommandStation::EjectCommander()
{
	// If this command station had a commander, log him out!
	if(this->mCommanderAtThisStation != -1)
	{
		// if the command station is killed, kick out any commander
		FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
			if(theEntity->pev->team == this->pev->team)
			{
				if(theEntity->GetUser3() == AVH_USER3_COMMANDER_PLAYER)
				{
					theEntity->StopTopDownMode();
					theEntity->SetUser3(AVH_USER3_MARINE_PLAYER);
					
					this->PlayAnimationAtIndex(kCommandStationExitAnimation, true);

					GetGameRules()->MarkDramaticEvent(kCCEjectPriority, theEntity, this);

					break;
				}
			}
			END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
	}
	
	this->mCommanderAtThisStation = -1;
}

char* AvHCommandStation::GetKilledSound() const
{
	return kCommandStationDeathSound;
}


void AvHCommandStation::SetInactive()
{
	AvHBaseBuildable::SetInactive();

	this->EjectCommander();
}

void AvHCommandStation::Materialize()
{
	AvHMarineBaseBuildable::Materialize();

	this->mCommanderAtThisStation = -1;
	
	//this->ResetEntity();
}

void AvHCommandStation::Spawn(void)
{
	AvHMarineBaseBuildable::Spawn();

	this->Materialize();
}

int	AvHCommandStation::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	int theReturnCode = 0;

	if(this->pev->health > 0)
	{
		theReturnCode = AvHBaseBuildable::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);

		if(this->pev->health <= 0)
		{
			AvHTeamNumber theStationTeamNumber = (AvHTeamNumber)this->pev->team;
			const char* theTarget = (theStationTeamNumber == TEAM_ONE) ? kTargetCommandStationDestroyedTeamOne : kTargetCommandStationDestroyedTeamTwo;
			FireTargets(theTarget, NULL, NULL, USE_TOGGLE, 0.0f);

			GetGameRules()->MarkDramaticEvent(kCCDeathPriority, this->entindex(), false, pevAttacker);
		}
	}
	
	return theReturnCode;
}


void AvHCommandStation::CommanderUsingThink(void)
{
	AvHTeamNumber theStationTeamNumber = this->GetTeamNumber();
	
	// Check to see if we already have a commander and don't make the person a commander if so
	if(GetGameRules()->GetNumCommandersOnTeam(theStationTeamNumber) == 0)
	{
		this->mCommanderAtThisStation = -1;
		this->mTimeToPlayOnlineSound = -1;
		
		EMIT_SOUND(ENT(this->pev), CHAN_AUTO, kCommandStationEndSound, .8, kCommandStationAttenuation);
		this->PlayAnimationAtIndex(kCommandStationExitAnimation, true);

		//SetThink(NULL);
		SetThink(&AvHBaseBuildable::AnimateThink);
		this->pev->nextthink = gpGlobals->time + .1f;
	}
	else
	{
		if(this->mTimeToPlayOnlineSound > 0.0f)
		{
			if(gpGlobals->time > this->mTimeToPlayOnlineSound)
			{
				ASSERT(this->mCommanderAtThisStation > 0);
				
				// If enough time has passed and we haven't played online sound, play it
				AvHPlayer* theCommander = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(this->mCommanderAtThisStation)));
				if(theCommander)
				{
					theCommander->PlayHUDSound(HUD_SOUND_MARINE_CCONLINE);
				}
				
				this->mTimeToPlayOnlineSound = -1;
			}
		}
	
		AvHBaseBuildable::AnimateThink();
		
		this->pev->nextthink = gpGlobals->time + kCommandStationThinkInterval;
	}
}

bool AvHCommandStation::GetIsTechnologyAvailable(AvHMessageID inMessageID) const
{
    bool theTechIsAvailable = AvHMarineBaseBuildable::GetIsTechnologyAvailable(inMessageID);

    // Only allow CC to be recycled if it's unoccupied
    if(inMessageID == BUILD_RECYCLE)
    {
        theTechIsAvailable = (this->mCommanderAtThisStation == -1);
    }

    return theTechIsAvailable;
}

AvHTurretFactory::AvHTurretFactory() : AvHMarineBaseBuildable(TECH_TURRET_FACTORY, BUILD_TURRET_FACTORY, kwsTurretFactory, AVH_USER3_TURRET_FACTORY)
{
}

void AvHTurretFactory::CheckTurretEnabledState() const
{
	// Check to see if turrets should come online
	FOR_ALL_ENTITIES(kwsDeployedTurret, AvHMarineTurret*)
	if(theEntity->pev->team == this->pev->team)
	{
		theEntity->CheckEnabledState();
	}
	END_FOR_ALL_ENTITIES(kwsDeployedTurret);
		
	FOR_ALL_ENTITIES(kwsSiegeTurret, AvHSiegeTurret*)
	if(theEntity->pev->team == this->pev->team)
	{
		theEntity->CheckEnabledState();
	}
	END_FOR_ALL_ENTITIES(kwsSiegeTurret)
}

void AvHTurretFactory::TriggerAddTech() const
{
	AvHBuildable::TriggerAddTech();

	this->CheckTurretEnabledState();
}

void AvHTurretFactory::TriggerRemoveTech() const
{
	AvHBuildable::TriggerRemoveTech();

	this->CheckTurretEnabledState();
}

int	AvHTurretFactory::GetIdle1Animation() const
{
	int theAnimation = 3;

	// Different animation when electrified
	if(GetHasUpgrade(this->pev->iuser4, MASK_UPGRADE_11))
	{
		theAnimation = 12;
	}

	return theAnimation;
}

int	AvHTurretFactory::GetIdle2Animation() const
{
	return this->GetIdle1Animation();
}

int AvHTurretFactory::GetResearchAnimation() const
{
	return 4;
}

void AvHTurretFactory::SetHasBeenBuilt()
{
	AvHBuildable::SetHasBeenBuilt();

	// Animate
	SetThink(&AvHBaseBuildable::AnimateThink);
	this->pev->nextthink = gpGlobals->time + .1f;
}

bool AvHTurretFactory::GetSupportsTechID(AvHTechID	inTechID) const
{
	bool theSuccess = AvHBuildable::GetSupportsTechID(inTechID);
	
	if(this->GetTechID() == TECH_ADVANCED_TURRET_FACTORY)
	{
		if(!theSuccess)
		{
			// Adv. turret factory also counts as a turret factory
			theSuccess = (inTechID == TECH_TURRET_FACTORY);
		}
	}
	
	return theSuccess;
}

void AvHTurretFactory::Upgrade()
{
	// Set iuser3
	this->pev->iuser3 = AVH_USER3_ADVANCED_TURRET_FACTORY;

	// Set classname
	this->pev->classname = MAKE_STRING(kwsAdvancedTurretFactory);

	this->mMessageID = TURRET_FACTORY_UPGRADE;
	this->SetTechID(TECH_ADVANCED_TURRET_FACTORY);

	this->SetSelectID(this->pev->iuser3);

	this->SetConstructionComplete(true);

	this->HealthChanged();
}


AvHArmory::AvHArmory() : AvHMarineBaseBuildable(TECH_ARMORY, BUILD_ARMORY, kwsArmory, AVH_USER3_ARMORY)
{
}

int	AvHArmory::GetSequenceForBoundingBox() const
{
	return 2;
}

bool AvHArmory::GetSupportsTechID(AvHTechID	inTechID) const
{
	bool theSuccess = AvHBuildable::GetSupportsTechID(inTechID);

	if(this->GetTechID() == TECH_ADVANCED_ARMORY)
	{
		// Adv. armory also counts as an armory
		if(!theSuccess)
		{
			theSuccess = (inTechID == TECH_ARMORY);
		}
	}

	return theSuccess;
}

int	AvHArmory::GetIdle1Animation() const
{
	int theAnim = 2;
	if(this->pev->iuser3 == AVH_USER3_ADVANCED_ARMORY)
	{
		theAnim = 3;
	}
	return theAnim;
}

int	AvHArmory::GetIdle2Animation() const
{
	return this->GetIdle1Animation();
}

int	AvHArmory::GetActiveAnimation() const
{
	int theAnim = 5;
	if(this->pev->iuser3 == AVH_USER3_ADVANCED_ARMORY)
	{
		theAnim = 12;
	}
	return theAnim;
}

int	AvHArmory::GetResearchAnimation() const
{
	return 5;
}

void AvHArmory::Precache()
{
	AvHMarineBaseBuildable::Precache();
	
	PRECACHE_UNMODIFIED_SOUND(kArmoryResupplySound);
}

void AvHArmory::ResupplyUse(CBaseEntity* inActivator, CBaseEntity* inCaller, USE_TYPE inUseType, float inValue)
{
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inCaller);
	
	if(thePlayer && (thePlayer->pev->team == this->pev->team) && this->GetIsBuilt() && !this->GetIsRecycling() && thePlayer->GetIsAbleToAct())
	{
        if(thePlayer->GetCanBeResupplied())
        {
			// puzl: 1017
//            // Give health back occasionally
//            bool theGiveHealthIfNeeded = (RANDOM_LONG(0, 3) == 0);
//            
			// resupply gives 10 health each use
			thePlayer->Resupply(true);		

            // Always play "getting ammo" sound when ammo or health are needed, to indicate to player when to stop pressing +use
            EMIT_SOUND(thePlayer->edict(), CHAN_WEAPON, kArmoryResupplySound, .3f, ATTN_NORM);
        }
	}
}

void AvHArmory::SetHasBeenBuilt()
{
	AvHBuildable::SetHasBeenBuilt();

	SetUse(&AvHArmory::ResupplyUse);
}

void AvHArmory::Upgrade()
{
	// Set iuser3
	this->pev->iuser3 = AVH_USER3_ADVANCED_ARMORY;

	// Set classname
	this->pev->classname = MAKE_STRING(kwsAdvancedArmory);

	this->mMessageID = ARMORY_UPGRADE;
	this->SetTechID(TECH_ADVANCED_ARMORY);

	this->SetSelectID(this->pev->iuser3);

	this->SetConstructionComplete(true);

	this->HealthChanged();
}


AvHArmsLab::AvHArmsLab() : AvHMarineBaseBuildable(TECH_ARMSLAB, BUILD_ARMSLAB, kwsArmsLab, AVH_USER3_ARMSLAB)
{
}

int	AvHArmsLab::GetResearchAnimation() const
{
	return 5;
}

int	AvHArmsLab::GetSequenceForBoundingBox() const
{
	return 1;
}

AvHPrototypeLab::AvHPrototypeLab() : AvHMarineBaseBuildable(TECH_PROTOTYPE_LAB, BUILD_PROTOTYPE_LAB, kwsPrototypeLab, AVH_USER3_PROTOTYPE_LAB)
{
}


const float kObservatoryThinkTime = 1.0f;
const float kObservatoryStartEnergy = 40;

AvHObservatory::AvHObservatory() : AvHMarineBaseBuildable(TECH_OBSERVATORY, BUILD_OBSERVATORY, kwsObservatory, AVH_USER3_OBSERVATORY)
{
}

int	AvHObservatory::GetSequenceForBoundingBox() const
{
	return 1;
}

void AvHObservatory::ObservatoryThink()
{
	// Remove cloaking from nearby enemies
	if ( !this->GetIsRecycling() ) {
		FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
		if(theEntity->GetIsRelevant() && (theEntity->pev->team != this->pev->team))
		{
			// Check that entity is in range of scan
			float theDistance = VectorDistance2D(theEntity->pev->origin, this->pev->origin);
			if(theDistance < BALANCE_VAR(kObservatoryXYDetectionRadius))
			{
				// Remove cloaking, if player has it
				theEntity->TriggerUncloak();
			}
		}
		END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
	}

	AvHBaseBuildable::AnimateThink();

	// Update scanning energy
	float theRate = kMarineStructureEnergyRate;
	if(GetGameRules()->GetCheatsEnabled() && !GetGameRules()->GetIsCheatEnabled(kcSlowResearch))
	{
		theRate *= 6;
	}

	this->SetEnergy(this->mEnergy + (kObservatoryThinkTime*theRate));

	this->pev->nextthink = gpGlobals->time + kObservatoryThinkTime;
}

void AvHObservatory::Materialize()
{
	AvHMarineBaseBuildable::Materialize();
}

void AvHObservatory::ResetEntity()
{
	AvHMarineBaseBuildable::ResetEntity();

	this->SetEnergy(0);
}

void AvHObservatory::SetHasBeenBuilt()
{
	AvHBuildable::SetHasBeenBuilt();

	SetThink(&AvHObservatory::ObservatoryThink);
	this->pev->nextthink = gpGlobals->time + kObservatoryThinkTime;

	this->SetEnergy(kObservatoryStartEnergy);
}

void AvHObservatory::Spawn()
{
	AvHMarineBaseBuildable::Spawn();
}

int	AvHObservatory::GetActiveAnimation() const
{
	return 2;
}

int	AvHObservatory::GetIdle1Animation() const
{
	return 3;
}

int	AvHObservatory::GetIdle2Animation() const
{
	return 3;
}

int	AvHObservatory::GetResearchAnimation() const
{
	return 4;
}
