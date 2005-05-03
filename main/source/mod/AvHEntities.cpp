//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHEntities.cpp$
// $Date: 2002/11/22 21:26:25 $
//
//-------------------------------------------------------------------------------
// $Log: AvHEntities.cpp,v $
// Revision 1.44  2002/11/22 21:26:25  Flayra
// - mp_consistency changes
// - Fixes to allow NULL owner
//
// Revision 1.43  2002/11/12 02:23:22  Flayra
// - Removed ancient egg
//
// Revision 1.42  2002/11/03 04:47:56  Flayra
// - Moved constants into .dll out of .cfg
//
// Revision 1.41  2002/10/20 21:10:14  Flayra
// - Optimizations
//
// Revision 1.40  2002/10/18 22:19:01  Flayra
// - Fixed bug where resources were still getting destroyed
//
// Revision 1.39  2002/10/16 00:53:16  Flayra
// - Play marine tower harvesting sound louder so it can be heard
//
// Revision 1.38  2002/10/03 18:42:31  Flayra
// - func_resources now are destroyed if a resource tower is destroyed
//
// Revision 1.37  2002/09/23 22:12:31  Flayra
// - Resource towers give 3 points to team in general
//
// Revision 1.36  2002/08/16 02:34:23  Flayra
// - Webs no longer affect friendlies in tourny mode
//
// Revision 1.35  2002/07/24 18:45:41  Flayra
// - Linux and scripting changes
//
// Revision 1.34  2002/07/23 17:01:33  Flayra
// - Updated resource model, removed old junk
//
// Revision 1.33  2002/07/10 14:40:04  Flayra
// - Fixed bug where .mp3s weren't being processed client-side
//
// Revision 1.32  2002/07/08 16:55:35  Flayra
// - Added max ensnare, can't remember why I'm tagging team starts, changed resources functions to floats (for proper handicapping)
//
// Revision 1.31  2002/07/01 21:26:41  Flayra
// - Toned down resource tower sounds (moved to CHAN_BODY to cut down on multiples playing)
//
// Revision 1.30  2002/06/25 17:54:20  Flayra
// - New info_location entity, make resource tower sounds very quiet
//
// Revision 1.29  2002/06/10 19:52:31  Flayra
// - Marked non-visible entities as nodraw, fixes visible entities problems!
//
// Revision 1.28  2002/05/28 17:37:01  Flayra
// - Track number of web strands to enforce limit, reworking of webs in general, removed duplicate code for AvHResourceTower
//
// Revision 1.27  2002/05/23 02:33:41  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHEntities.h"
#include "dlls/player.h"
#include "dlls/gamerules.h"
#include "dlls/explode.h"
#include "dlls/soundent.h"
#include "dlls/weapons.h"
#include "dlls/cpushable.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHServerVariables.h"
#include "mod/AvHPlayer.h"
#include "mod/AvHTitles.h"
#include "mod/AvHSoundListManager.h"
#include "mod/AvHParticleTemplateServer.h"
#include "mod/AvHParticleConstants.h"
#include "mod/AvHParticleSystemEntity.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHHulls.h"
#include "engine/studio.h"
#include "mod/AvHBaseBuildable.h"
#include "mod/AvHScriptManager.h"
#include "dlls/animation.h"
#include "util/MathUtil.h"

extern DLL_GLOBAL CGameRules*	g_pGameRules;
BOOL IsSpawnPointValid( CBaseEntity *pPlayer, CBaseEntity *pSpot );

LINK_ENTITY_TO_CLASS( keSpectate, AvHSpectateEntity );
LINK_ENTITY_TO_CLASS( keJoinTeam, AvHJoinTeamEntity );
//LINK_ENTITY_TO_CLASS( keLeaveGame, AvHLeaveGameEntity );
LINK_ENTITY_TO_CLASS( keTeamStart, AvHTeamStartEntity );
LINK_ENTITY_TO_CLASS( keAutoAssign, AvHAutoAssignEntity );
LINK_ENTITY_TO_CLASS( keMapInfo, AvHMapInfo );
LINK_ENTITY_TO_CLASS( keGameplay, AvHGameplay );

LINK_ENTITY_TO_CLASS( keSeethrough, AvHSeeThrough);
LINK_ENTITY_TO_CLASS( keSeethroughDoor, AvHSeeThroughDoor);

//LINK_ENTITY_TO_CLASS( keWaypoint, AvHWaypoint);
LINK_ENTITY_TO_CLASS( keNoBuild, AvHNoBuild);
LINK_ENTITY_TO_CLASS( keFuncResource, AvHFuncResource);

LINK_ENTITY_TO_CLASS( keGamma, AvHGamma );

LINK_ENTITY_TO_CLASS( keMP3Audio, AvHMP3Audio);

LINK_ENTITY_TO_CLASS( keTriggerPresence, TriggerPresence);
LINK_ENTITY_TO_CLASS( keTriggerRandom, AvHTriggerRandom );
LINK_ENTITY_TO_CLASS( keTriggerScript, AvHTriggerScript);

//LINK_ENTITY_TO_CLASS( keTeamEgg, AvHEgg );
LINK_ENTITY_TO_CLASS( keTeamWebStrand, AvHWebStrand );

LINK_ENTITY_TO_CLASS( keFog, AvHFog);

LINK_ENTITY_TO_CLASS(kwResourceTower, AvHResourceTower);

LINK_ENTITY_TO_CLASS(keInfoLocation, AvHInfoLocation);

// Dummy initializer
StringList AvHMP3Audio::sSoundList;

extern AvHSoundListManager				gSoundListManager;
extern AvHParticleTemplateListServer	gParticleTemplateList;
const float kFallThinkInterval = .1f;

AvHBaseEntity::AvHBaseEntity()
{
	//this->mTeam = TEAM_IND;
}

AvHTeamNumber AvHBaseEntity::GetTeamNumber() const
{
	//return this->mTeam;
	return (AvHTeamNumber)this->pev->team;
}

void AvHBaseEntity::FallThink(void)
{
	this->pev->nextthink = gpGlobals->time + kFallThinkInterval;
	
	if(this->pev->flags & FL_ONGROUND)
	{
		// clatter if we have an owner (i.e., dropped by someone)
		// don't clatter if the gun is waiting to respawn (if it's waiting, it is invisible!)
		//if ( !FNullEnt( this->pev->owner ) )
		//{
		//	int pitch = 95 + RANDOM_LONG(0,29);
		//	EMIT_SOUND_DYN(ENT(this->pev), CHAN_VOICE, "items/weapondrop1.wav", 1, ATTN_NORM, 0, pitch);	
		//}
		
		// lie flat
		this->pev->angles.x = 0;
		this->pev->angles.z = 0;
		
		UTIL_SetOrigin( pev, pev->origin );// link into world.
	}
}

void AvHBaseEntity::KeyValue( KeyValueData* pkvd )
{
	if(FStrEq(pkvd->szKeyName, "teamchoice"))
	{
		//this->mTeam = (AvHTeamNumber)(atoi(pkvd->szValue));
		this->pev->team = (AvHTeamNumber)(atoi(pkvd->szValue));
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseEntity::KeyValue(pkvd);
	}
}

void AvHBaseEntity::NotifyUpgrade(AvHUpgradeMask inUpgradeMask)
{
}

AvHClientCommandEntity::AvHClientCommandEntity()
{
}

void AvHClientCommandEntity::ClientCommandTouch( CBaseEntity *pOther )
{
	// If incoming entity is a player
	CBasePlayer* theBasePlayer = dynamic_cast<CBasePlayer*>(pOther);
	if(theBasePlayer)
	{
		// execute client command for that player
		g_pGameRules->ClientCommand(theBasePlayer, this->GetClientCommand());
	}
}

void AvHClientCommandEntity::Spawn(void)
{
	pev->movetype = MOVETYPE_NONE;
	pev->solid = SOLID_TRIGGER;
	pev->iuser3 = AVH_USER3_CLIENT_COMMAND;

	SET_MODEL( ENT(pev), STRING(pev->model) );
	this->pev->effects = EF_NODRAW;

	SetTouch(&AvHClientCommandEntity::ClientCommandTouch);
}

const char*	AvHJoinTeamEntity::GetClientCommand()
{ 
	const char* theCommand = "";

	AvHTeamNumber theTeamNumber = this->GetTeamNumber();
	switch(theTeamNumber)
	{
	case TEAM_ONE:
		theCommand = kcJoinTeamOne;
		break;
	case TEAM_TWO:
		theCommand = kcJoinTeamTwo;
		break;
	case TEAM_THREE:
		theCommand = kcJoinTeamThree;
		break;
	case TEAM_FOUR:
		theCommand = kcJoinTeamFour;
		break;
	}

	return theCommand; 
}

void AvHTeamStartEntity::KeyValue( KeyValueData* pkvd )
{
	AvHBaseEntity::KeyValue(pkvd);
}

void AvHTeamStartEntity::Spawn(void)
{
	AvHBaseEntity::Spawn();

	this->pev->effects |= EF_NODRAW;

	// Mark with special iuser3
	AvHUser3 theUser3 = AVH_USER3_SPAWN_READYROOM;

	AvHTeamNumber theTeamNumber = this->GetTeamNumber();
	if( theTeamNumber == GetGameRules()->GetTeamA()->GetTeamNumber() )
	{
		theUser3 = AVH_USER3_SPAWN_TEAMA;
	}
	else if( theTeamNumber == GetGameRules()->GetTeamB()->GetTeamNumber() )
	{
		theUser3 = AVH_USER3_SPAWN_TEAMB;
	}
	else
	{
		ASSERT(false);
	}

	this->pev->iuser3 = theUser3;
}



//void AvHBuildableAnimating::BuildUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
//{
//	// Check teams to see if right person is using it
//	if(this->pev->team == pActivator->pev->team)
//	{
//		if(!this->GetIsBuilt())
//		{
//			if(AvHSUPlayerCanBuild(pActivator->pev))
//			{
//				this->mTimeLastUsed = gpGlobals->time;
//				
//				if(RANDOM_LONG(0, 4) == 0)
//				{
//					// Set health, taking into account upgrade
//					this->SetNormalizedBuildPercentage(this->GetNormalizedBuildPercentage() + 1.0f);
//					this->pev->health = this->GetMinHitPoints() + (this->GetNormalizedBuildPercentage())*(this->GetMaxHitPoints() - this->GetMinHitPoints());
//					//this->pev->armorvalue = this->mMaxArmor;
//					this->pev->rendermode = kRenderTransTexture;
//					this->pev->renderamt = (this->pev->health/this->GetMaxHitPoints())*255;
//				}
//				
//				//this->PlayBuildSound();
//				AvHSUPlayRandomConstructionEffect(this);
//				
//				if(this->GetIsBuilt())
//				{
//					this->SetConstructionComplete();
//				}
//				
//				AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(pActivator);
//				if(thePlayer && !this->GetIsBuilt())
//				{
//					thePlayer->TriggerProgressBar(this->entindex(), 1);
//					//this->pev->fuser1 = this->GetNormalizedBuildPercentage()*kNormalizationNetworkFactor;
//				}
//			}
//		}
//	}
//}
//
//int AvHBuildableAnimating::GetMinHitPoints() const
//{
//	return .5f*this->GetMaxHitPoints();
//}
//
//int	AvHBuildableAnimating::GetMaxHitPoints() const
//{
//	return 200;
//}
//
//bool AvHBuildableAnimating::GetIsBuilt() const
//{
//	return (this->GetNormalizedBuildPercentage() >= 1.0f);
//}
//
//void AvHBuildableAnimating::SetIsBuilt()
//{
//	this->SetNormalizedBuildPercentage(1.0f);
//}
//
//float AvHBuildableAnimating::GetNormalizedBuildPercentage() const
//{
//	return this->pev->fuser1/kNormalizationNetworkFactor;
//}
//
//void AvHBuildableAnimating::SetNormalizedBuildPercentage(float inPercentage)
//{
//	this->pev->fuser1 = inPercentage*kNormalizationNetworkFactor;
//}
//
//
//void AvHBuildableAnimating::Spawn(void)
//{
//	CBaseAnimating::Spawn();
//
//	SetUse(BuildUse);
//
//	this->pev->iuser3 = AVH_USER3_BUILDABLE;
//	this->pev->fuser1 = 0.0f;
//	this->mTimeLastUsed = -1;
//}


//// Build site entities
//AvHResource::AvHResource(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inHealth, AvHSelectableUser4 inUser4) : mThinkInterval(.1f)
//{
//	this->mBuildRange = 0.0f;
//	this->mResourceRating = 0;
//	this->mHasResource = false;
//	this->mStartAlreadyBuilt = false;
//	this->mValidTeams = 0;
//	this->mBuildSoundPlaying = false;
//	this->mTimeLastContributed = -1;
//}
//
//int	AvHResource::BloodColor( void )
//{ 
//	return DONT_BLEED; 
//}
//
//float AvHResource::GetBuildRange(void) const
//{
//	return this->mBuildRange;
//}
//
//bool AvHResource::GetIsActive(void) const
//{
//	return this->GetIsBuilt() && (this->pev->health > 0);
//}
//
//float AvHResource::GetTimeLastContributed()
//{
//	return this->mTimeLastContributed;
//}
//
//void AvHResource::SetTimeLastContributed(float inTime)
//{
//	this->mTimeLastContributed = inTime;
//}
//
//void AvHResource::Killed( entvars_t *pevAttacker, int iGib )
//{
//	// lots of smoke
//	MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
//	WRITE_BYTE( TE_SMOKE );
//	WRITE_COORD( RANDOM_FLOAT( pev->absmin.x, pev->absmax.x ) );
//	WRITE_COORD( RANDOM_FLOAT( pev->absmin.y, pev->absmax.y ) );
//	WRITE_COORD( pev->origin.z );
//	WRITE_SHORT( g_sModelIndexSmoke );
//	WRITE_BYTE( 25 ); // scale * 10
//	WRITE_BYTE( 10 ); // framerate
//	MESSAGE_END();
//	
//	if (pev->dmgtime + RANDOM_FLOAT( 0, 5 ) > gpGlobals->time)
//	{
//		Vector vecSrc = Vector( (float)RANDOM_FLOAT( pev->absmin.x, pev->absmax.x ), (float)RANDOM_FLOAT( pev->absmin.y, pev->absmax.y ), (float)0 );
//		vecSrc = vecSrc + Vector( (float)0, (float)0, (float)RANDOM_FLOAT( pev->origin.z, pev->absmax.z ) );
//		
//		UTIL_Sparks( vecSrc );
//	}
//
//	CBaseAnimating::Killed(pevAttacker, iGib);
//}
//
//
////void AvHResource::LoopSound(void) const
////{
////	//UTIL_EmitAmbientSound( ENT(this->pev), pev->origin, kResourceHum, 100.0f, ATTN_NORM, SND_SPAWNING, RANDOM_LONG( 50, 100 ));
////	UTIL_EmitAmbientSound( ENT(this->pev), pev->origin, kResourceHum, 1, 1.25, SND_SPAWNING, 100);
////}
//
//int	AvHResource::ObjectCaps( void ) 
//{ 
//	return FCAP_CONTINUOUS_USE;
//}
//
////void AvHResource::PlayBuildSound(void)
////{
////	if(!this->mBuildSoundPlaying)
////	{
////		UTIL_EmitAmbientSound( ENT(this->pev), this->pev->origin, kResourceBuildSound, 100.0f, ATTN_NORM, 0, RANDOM_LONG( 50, 100 ));
////		this->mBuildSoundPlaying = true;
////	}
////}
////
////void AvHResource::StopBuildSound(void)
////{
////	if(this->mBuildSoundPlaying)
////	{
////		UTIL_EmitAmbientSound(ENT(this->pev), this->pev->origin, kResourceBuildSound, 0, 0, SND_STOP, 0);
////		this->mBuildSoundPlaying = false;
////	}
////}
//
//void AvHResource::Precache( void )
//{
//	CBaseAnimating::Precache();
//
//	//PRECACHE_SOUND(kResourceHum);
//	//PRECACHE_SOUND(kResourceBuildSound);
//
//	if(pev->model)
//	{
//		PRECACHE_MODEL( (char *)STRING(pev->model) );
//	}
//}
//
//void AvHResource::ResourceThink(void)
//{
////	if((gpGlobals->time - this->mTimeLastUsed > .2f) && this->mBuildSoundPlaying)
////	{
////		this->StopBuildSound();
////	}
//	pev->nextthink = gpGlobals->time + this->mThinkInterval;
//}
//
//void AvHResource::ResourceTouch( CBaseEntity *pOther )
//{
//}
//
////void AvHResource::ResourceUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
////{
////}
//
//void AvHResource::SetConstructionComplete()
//{
//	//SetUse(NULL);
//	this->SetIsBuilt();
//	//this->LoopSound();
//	this->pev->health = this->GetMaxHitPoints();
//	this->pev->rendermode = kRenderNormal;
//	this->pev->renderamt = 0;
//	this->pev->takedamage = 1;
//	pev->solid = SOLID_SLIDEBOX;
//
//	this->mTimeLastContributed = gpGlobals->time;
//}
//
//void AvHResource::Spawn(void)
//{
//	this->Precache();
//
//    pev->classname	= MAKE_STRING(kAvHResourceClassName);
//	
//	pev->movetype	= MOVETYPE_FLY;
//
//	SET_MODEL( ENT(pev), STRING(pev->model) );
//
//	if(this->mStartAlreadyBuilt)
//	{
//		this->SetIsBuilt();
//		this->pev->health = this->GetMaxHitPoints();
//		this->pev->rendermode = kRenderNormal;
//		SetUse(NULL);
//		//this->LoopSound();
//		this->pev->solid = SOLID_SLIDEBOX;
//	}
//	else
//	{
//		this->pev->fuser1 = 0;
//		this->pev->rendermode = kRenderTransTexture;
//		this->pev->solid = SOLID_NOT;
//		this->pev->health = this->GetMinHitPoints();
//		//SetTouch(ResourceTouch);
//		//SetUse(ResourceUse);
//	}
//
//	SetThink(ResourceThink);
//	pev->nextthink = gpGlobals->time + this->mThinkInterval;
//
//	this->mTimeLastContributed = gpGlobals->time;
//}
//
//int	AvHResource::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
//{
//	int theReturnCode = 0;
//
//	if (pev->takedamage )
//	{
//		// Resources have armor too, get protection from upgrades
//		float theNewDamage = AvHSUCalculateDamageLessArmor(this->pev, flDamage, bitsDamageType, GetGameRules()->IsMultiplayer());
//		theReturnCode = CBaseAnimating::TakeDamage(pevInflictor, pevAttacker, theNewDamage, bitsDamageType);
//
////		pev->health -= theNewDamage;
////		if (pev->health <= 0)
////		{
////			pev->health = 0;
////			pev->takedamage = DAMAGE_NO;
////			pev->dmgtime = gpGlobals->time;
////			
////			ClearBits (pev->flags, FL_MONSTER); // why are they set in the first place???
////			
////			SetUse(NULL);
////
////			// SetThink?
////
////			//SUB_UseTargets( this, USE_ON, 0 ); // wake up others
////		}
//	}
//
//	return theReturnCode;
//}








AvHSeeThrough::AvHSeeThrough()
{
	this->mCommanderAlpha = 0;
	this->mPlayerAlpha = 255;
}

void AvHSeeThrough::KeyValue( KeyValueData* pkvd )
{
	if(FStrEq(pkvd->szKeyName, "commanderAlpha") || FStrEq(pkvd->szKeyName, "seeThroughAlpha"))
	{
		int theAlpha = atoi(pkvd->szValue);
		this->mCommanderAlpha = min(max(0, theAlpha), 255);
		pkvd->fHandled = TRUE;
	}
	if(FStrEq(pkvd->szKeyName, "playerAlpha"))
	{
		int theAlpha = atoi(pkvd->szValue);
		this->mPlayerAlpha = min(max(0, theAlpha), 255);
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseEntity::KeyValue(pkvd);
	}
}

void AvHSeeThrough::Spawn()
{
	this->Precache();
	CBaseEntity::Spawn();

	this->pev->solid = SOLID_BSP;
	this->pev->movetype = MOVETYPE_PUSH;
	SET_MODEL( ENT(pev), STRING(pev->model) );
	
    this->pev->classname = MAKE_STRING(kesSeethrough);
	
	this->pev->iuser3 = AVH_USER3_ALPHA;
	this->pev->fuser1 = this->mCommanderAlpha;
	this->pev->fuser2 = this->mPlayerAlpha;
}



AvHSeeThroughDoor::AvHSeeThroughDoor()
{
	this->mCommanderAlpha = 0;
	this->mPlayerAlpha = 255;
}

void AvHSeeThroughDoor::KeyValue( KeyValueData* pkvd )
{
	// For backwards compatibility with old AvHSeeThrough
	if(FStrEq(pkvd->szKeyName, "commanderAlpha") || FStrEq(pkvd->szKeyName, "seeThroughAlpha"))
	{
		int theAlpha = atoi(pkvd->szValue);
		this->mCommanderAlpha = min(max(0, theAlpha), 255);
		pkvd->fHandled = TRUE;
	}
	if(FStrEq(pkvd->szKeyName, "playerAlpha"))
	{
		int theAlpha = atoi(pkvd->szValue);
		this->mPlayerAlpha = min(max(0, theAlpha), 255);
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseDoor::KeyValue(pkvd);
	}
}


void AvHSeeThroughDoor::Spawn()
{
	this->Precache();
	CBaseDoor::Spawn();
	
	this->pev->solid = SOLID_BSP;
	this->pev->movetype = MOVETYPE_PUSH;
	SET_MODEL( ENT(pev), STRING(pev->model) );
	
    this->pev->classname = MAKE_STRING(kesSeethroughDoor);

	this->pev->iuser3 = AVH_USER3_ALPHA;
	this->pev->fuser1 = this->mCommanderAlpha;
	this->pev->fuser2 = this->mPlayerAlpha;
}



AvHNoBuild::AvHNoBuild()
{
}

void AvHNoBuild::KeyValue(KeyValueData* pkvd)
{
	AvHBaseEntity::KeyValue(pkvd);
}

void AvHNoBuild::Spawn()
{
	this->Precache();
	AvHBaseEntity::Spawn();
	
	this->pev->solid = SOLID_BSP;
	this->pev->movetype = MOVETYPE_PUSH;

	//this->pev->solid = SOLID_TRIGGER;
	//this->pev->movetype = MOVETYPE_NONE;

	SET_MODEL( ENT(pev), STRING(pev->model) );
	
    this->pev->classname = MAKE_STRING(kesNoBuild);
	this->pev->iuser3 = AVH_USER3_NOBUILD;
	
	this->pev->rendermode = kRenderTransAdd;
	this->pev->renderamt = 0;
	this->pev->effects = EF_NODRAW;
}




AvHMP3Audio::AvHMP3Audio()
{
	this->mUseState = false;
	this->mSoundVolume = 255;
	this->mLooping = false;
}

int AvHMP3Audio::GetFadeDistance() const
{
	return this->mFadeDistance;
}

void AvHMP3Audio::KeyValue( KeyValueData* pkvd )
{
	if(FStrEq(pkvd->szKeyName, "soundname"))
	{
		this->mSoundName = pkvd->szValue;
		pkvd->fHandled = TRUE;
	}
	else if(FStrEq(pkvd->szKeyName, "soundvolume"))
	{
		this->mSoundVolume = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if(FStrEq(pkvd->szKeyName, "fadedistance"))
	{
		this->mFadeDistance = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		AvHBaseEntity::KeyValue(pkvd);
	}
}

void AvHMP3Audio::Precache()
{
	CBaseEntity::Precache();
	PRECACHE_UNMODIFIED_MODEL(kNullModel);
}

void AvHMP3Audio::Spawn()
{
	Precache();
	
	AvHBaseEntity::Spawn();
	
	// Set spawn flags
	if(this->pev->spawnflags & 1)
	{
		// Don't fade sound with position, treat as HUD sound
		this->mFadeDistance = 0;
	}
	if(this->pev->spawnflags & 2)
	{
		this->mLooping = true;
	}
	if(this->pev->spawnflags & 4)
	{
		this->pev->iuser3 = AVH_USER3_AUDIO_ON; 
	}
	else
	{
		this->pev->iuser3 = AVH_USER3_AUDIO_OFF;
	}
	
	// Set model
    pev->classname = MAKE_STRING(kesMP3Audio);
	
	this->pev->solid = SOLID_NOT;
	this->pev->movetype = MOVETYPE_NONE;

	//this->pev->effects |= EF_NODRAW;

	//SET_MODEL(ENT(pev), STRING(pev->model));
	SET_MODEL(ENT(this->pev), kNullModel);
	UTIL_SetOrigin(pev, pev->origin);
	
	// Set use so it can be toggled on and off like a switch, not used by the player 
	SetUse(&AvHMP3Audio::SpecialSoundUse);

	// Add sound name to global registry (relative path without mod directory).  
	// This is pushing back duplicates.  Do we care?  If SoundList changes to a hash, this could break.
	int theListSize = sSoundList.size();
	sSoundList.push_back(this->mSoundName);
	theListSize = sSoundList.size();
	
	// The sound index is the position in this list (assume we're adding to end)
	int theSoundIndexOffset = sSoundList.size() - 1;
	
	// Set up variables
	ASSERT(theSoundIndexOffset >= 0);
	ASSERT(theSoundIndexOffset < 256);
	int theValue = (theSoundIndexOffset << 8);

	int theEntIndex = this->entindex();
	theValue |= (theEntIndex << 16);
	ASSERT(theEntIndex == (theValue >> 16));
	this->pev->fuser1 = theValue;
	//memcpy(&this->pev->fuser1, &theValue, sizeof(float));
	int theConvertedFuser1 = (int)(this->pev->fuser1);
	//int theConvertedFuser1 = 0;
	//memcpy(&theConvertedFuser1, &this->pev->fuser1, sizeof(float));
	ASSERT(theConvertedFuser1 == theValue);
	this->pev->fuser2 = gpGlobals->time;

	// Mash all these little ints into iuser4 (fade distance is max of 16-bits)
	theValue = this->mFadeDistance;
	theValue |= (this->mSoundVolume << 16);
	theValue |= (this->pev->spawnflags << 24);
	this->pev->iuser4 = theValue;
}

void AvHMP3Audio::ClearSoundNameList()
{
	sSoundList.clear();
}

StringList AvHMP3Audio::GetSoundNameList()
{
	return sSoundList;
}


void AvHMP3Audio::SpecialSoundUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	bool theOldUseState = this->mUseState;

	// Can't toggle it once it's been welded
	switch(useType)
	{
	case USE_OFF:
		this->mUseState = false;
		break;
		
	case USE_ON:
		this->mUseState = true;
		break;
		
	case USE_SET:
		// Handle this?
		break;
		
	case USE_TOGGLE:
		this->mUseState = !this->mUseState;
		break;
	}

	// Just turned on or off?
	if(theOldUseState != this->mUseState)
	{
		// Update time of action
		this->pev->fuser2 = gpGlobals->time;
		
		// Update type of action
		if(this->mUseState)
		{
			this->pev->iuser3 = AVH_USER3_AUDIO_ON;
		}
		else
		{
			this->pev->iuser3 = AVH_USER3_AUDIO_OFF;
		}
	}
}



AvHMapInfo::AvHMapInfo()
{
	this->mMapExtents.ResetMapExtents();
}

const AvHMapExtents& AvHMapInfo::GetMapExtents() const
{
	return this->mMapExtents;
}

void AvHMapInfo::KeyValue( KeyValueData* pkvd )
{
	if (FStrEq(pkvd->szKeyName, "viewheight"))
	{
		this->mMapExtents.SetMaxViewHeight(atoi(pkvd->szValue));
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "minviewheight"))
	{
		this->mMapExtents.SetMinViewHeight(atoi(pkvd->szValue));
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "minx"))
	{
		this->mMapExtents.SetMinMapX(atoi(pkvd->szValue));
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "miny"))
	{
		this->mMapExtents.SetMinMapY(atoi(pkvd->szValue));
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "maxx"))
	{
		this->mMapExtents.SetMaxMapX(atoi(pkvd->szValue));
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "maxy"))
	{
		this->mMapExtents.SetMaxMapY(atoi(pkvd->szValue));
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "culldistance"))
	{
		this->mMapExtents.SetTopDownCullDistance(atoi(pkvd->szValue));
		pkvd->fHandled = TRUE;
	}
	else
	{
		AvHBaseEntity::KeyValue(pkvd);
	}
}

void AvHMapInfo::Spawn()
{
	AvHBaseEntity::Spawn();

    this->pev->classname = MAKE_STRING(kwsMapInfoClassName);
	this->pev->solid = SOLID_NOT;
	this->pev->movetype = MOVETYPE_NONE;
	this->pev->effects = EF_NODRAW;

	// Did mapper check "don't draw background" flag?
	if(pev->spawnflags & 1)
	{
		this->mMapExtents.SetDrawMapBG(false);
	}
}

AvHGameplay::AvHGameplay()
{
	this->mTeamAType = AVH_CLASS_TYPE_UNDEFINED;
	this->mTeamBType = AVH_CLASS_TYPE_UNDEFINED;
	
	this->mInitialHives = 1;
}

AvHClassType AvHGameplay::GetTeamAType() const
{
	return this->mTeamAType;
}

AvHClassType AvHGameplay::GetTeamBType() const
{
	return this->mTeamBType;
}

int	AvHGameplay::GetInitialHives() const
{
	return this->mInitialHives;
}

int	AvHGameplay::GetInitialAlienPoints() const
{
	return BALANCE_VAR(kNumInitialAlienPoints);
}

int	AvHGameplay::GetInitialMarinePoints() const
{
	return BALANCE_VAR(kNumInitialMarinePoints);
}

int	AvHGameplay::GetAlienRespawnCost() const
{
	return 0;
}

int	AvHGameplay::GetMarineRespawnCost() const
{
	return 0;
}

// Not used currently
int	AvHGameplay::GetAlienRespawnTime() const
{
	float theTimeToRespawn = BALANCE_VAR(kAlienRespawnTime);

	if(GetGameRules()->GetIsTesting() || GetGameRules()->GetCheatsEnabled())
	{
		theTimeToRespawn = 2.0f;
	}

	return theTimeToRespawn;
}

int	AvHGameplay::GetTowerInjectionTime() const
{
	return BALANCE_VAR(kFuncResourceInjectionTime);
}

int	AvHGameplay::GetTowerInjectionAmount() const
{
	return BALANCE_VAR(kFuncResourceInjectionAmount);
}


void AvHGameplay::KeyValue( KeyValueData* pkvd )
{
	if (FStrEq(pkvd->szKeyName, "teamone"))
	{
		this->mTeamAType = (AvHClassType)(atoi(pkvd->szValue));
		pkvd->fHandled = TRUE;
	}
	if (FStrEq(pkvd->szKeyName, "teamtwo"))
	{
		this->mTeamBType = (AvHClassType)(atoi(pkvd->szValue));
		pkvd->fHandled = TRUE;
	}
	else
	{
		AvHBaseEntity::KeyValue(pkvd);
	}
}

void AvHGameplay::Reset()
{
	// This should have all the correct defaults (same as ns.fgd)
	this->mTeamAType = AVH_CLASS_TYPE_MARINE;
	this->mTeamBType = AVH_CLASS_TYPE_ALIEN;
	
	this->mInitialHives = 1;
}

// Should we even do this?
void AvHGameplay::Spawn()
{
	AvHBaseEntity::Spawn();
	
    this->pev->classname = MAKE_STRING(kwsGameplayClassName);
	this->pev->solid = SOLID_NOT;
	this->pev->movetype = MOVETYPE_NONE;
	this->pev->effects = EF_NODRAW;
}



AvHGamma::AvHGamma()
{
	this->mGammaScalar = 1.0f;
}
	
void AvHGamma::KeyValue(KeyValueData* pkvd)
{
	if(FStrEq(pkvd->szKeyName, "desiredgamma"))
	{
		this->mGammaScalar = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else
	{
		AvHBaseEntity::KeyValue(pkvd);
	}
}
	
float AvHGamma::GetGamma() const
{
	return this->mGammaScalar;
}


void AvHGamma::Spawn()
{
	AvHBaseEntity::Spawn();
    this->pev->classname = MAKE_STRING(kwsGammaClassName);
	this->pev->effects = EF_NODRAW;
}





//AvHEgg::AvHEgg()
//{
//}
//
//void AvHEgg::EggThink(void)
//{
//	// If we're not touching any 
//	//FallThink();
//	this->pev->nextthink = gpGlobals->time + kFallThinkInterval;
//	
//	if(this->pev->flags & FL_ONGROUND)
//	{
//		// lie flat
//		this->pev->angles.x = 0;
//		this->pev->angles.z = 0;
//
//		this->pev->solid = SOLID_BBOX;
//		this->pev->movetype = MOVETYPE_TOSS;
//
//		// Now it's big enough to block
//		this->pev->takedamage = DAMAGE_YES;
//
//		Vector theFinalPos = this->pev->origin;
//		//theFinalPos.z += 50;
//
//		UTIL_SetSize(this->pev, Vector( -32, -32, -50), Vector(32, 32, 64) );
//		UTIL_SetOrigin(this->pev, theFinalPos);// link into world.
//		SetThink (NULL);
//
//		// Start animating
//		this->pev->sequence	= 0;
//		this->pev->frame = 0;
//		//ResetSequenceInfo();
//		//AvHSUSetCollisionBoxFromSequence(this->pev);
//	}
//}
//
//void AvHEgg::Hatch()
//{
//}
//
//void AvHEgg::Killed( entvars_t *pevAttacker, int iGib )
//{
//	AvHSUExplodeEntity(this, matFlesh);
//
//	EMIT_SOUND(ENT(this->pev), CHAN_AUTO, kEggDestroyedSound, 1.0, ATTN_IDLE);	
//
//	//AvHBaseEntity::Killed(pevAttacker, iGib);
//	//CBaseAnimating::Killed(pevAttacker, iGib);
//	CBaseEntity::Killed(pevAttacker, iGib);
//}
//
//void AvHEgg::Precache(void)
//{
//	PRECACHE_SOUND(kEggDestroyedSound);
//	PRECACHE_MODEL(kEggModel);
//}
//
//void AvHEgg::Spawn()
//{
//	this->Precache( );
//
//	this->pev->solid = SOLID_BBOX;
//	this->pev->movetype = MOVETYPE_TOSS;
//	//this->pev->flags &= ~FL_ONGROUND;
//	//this->pev->velocity = Vector(0, 0, 8);
//	
//	SET_MODEL( ENT(this->pev), kEggModel);
//	UTIL_SetOrigin(pev, pev->origin);               // set size and link into world
//
//	DROP_TO_FLOOR(ENT(pev));
//
//	this->pev->classname = MAKE_STRING(kwsEggClassName);
//	this->pev->health = AvHPlayerUpgrade::GetMaxHealth(0, ROLE_GESTATING);
//	this->pev->max_health = pev->health;
//	this->pev->takedamage = DAMAGE_YES;
//	//this->pev->view_ofs = Vector(0,0,22);
//
//	this->pev->sequence = 0;
//	this->pev->frame = 0;
//	ResetSequenceInfo();
//	AvHSUSetCollisionBoxFromSequence(this->pev);
//}
//
//void AvHEgg::SpawnPlayer()
//{
//	// Bring origin up a bit, so when the player spawns he won't be stuck in the ground
//	this->pev->origin.z += 50;
//
//	AvHSUExplodeEntity(this, matFlesh);
//
//	SetThink(SUB_Remove);
//
//	this->pev->nextthink = gpGlobals->time + .3f;
//}
//
//int	AvHEgg::TakeDamage( entvars_t* pevInflictor, entvars_t* pevAttacker, float flDamage, int bitsDamageType )
//{
//	//return CBaseAnimating::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
//	return CBaseEntity::TakeDamage(pevInflictor, pevAttacker, flDamage, bitsDamageType);
//}


const float kTriggerPresenceThinkInterval = .05f;

TriggerPresence::TriggerPresence()
{
	this->mEnabled = true;
	this->mPresence = false;
	this->mPlayersDontActivate = false;
	this->mMonstersDontActivate = false;
	this->mPushablesDontActivate = false;
	this->mTeamAOnly = false;
	this->mTeamBOnly = false;
	
	this->mTimeOfLastTouch = -1;
	this->mTimeBeforeLeave = .5f;
	this->mMomentaryOpenTime = 1.0f;
	this->mMomentaryCloseTime = 1.0f;
}

void TriggerPresence::KeyValue(KeyValueData* pkvd)
{
	pkvd->fHandled = FALSE;
	
	if(FStrEq(pkvd->szKeyName, "master"))
	{
		this->mMaster = pkvd->szValue;
		pkvd->fHandled = TRUE;
	} 
	else if(FStrEq(pkvd->szKeyName, "targetenter"))
	{
		this->mTargetEnter = pkvd->szValue;
		pkvd->fHandled = TRUE;
	} 
	else if(FStrEq(pkvd->szKeyName, "targetleave"))
	{
		this->mTargetLeave = pkvd->szValue;
		pkvd->fHandled = TRUE;
	} 
	else if(FStrEq(pkvd->szKeyName, "momentarytarget"))
	{
		this->mMomentaryTarget = pkvd->szValue;
		pkvd->fHandled = TRUE;
	} 
	else if(FStrEq(pkvd->szKeyName, "timebeforeleave"))
	{
		this->mTimeBeforeLeave = fmax(atof(pkvd->szValue), 0.0f);
		pkvd->fHandled = TRUE;
	} 
	else if(FStrEq(pkvd->szKeyName, "momentaryopentime"))
	{
		this->mMomentaryOpenTime = fmax(atof(pkvd->szValue), 0.01f);
		pkvd->fHandled = TRUE;
	} 
	else if(FStrEq(pkvd->szKeyName, "momentaryclosetime"))
	{
		this->mMomentaryCloseTime = fmax(atof(pkvd->szValue), 0.01f);
		pkvd->fHandled = TRUE;
	} 
	else if(FStrEq(pkvd->szKeyName, "spawnflags"))
	{
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseEntity::KeyValue(pkvd);
	}
}

void TriggerPresence::Precache(void)
{
}

//void TriggerPresence::ResetEntity(void)
//{
	// Not sure if this should be here or not...think
	//this->mTimeOfLastTouch = -1;
	//this->SetPresence(false);
	//this->mEnabled = false;
//}

void TriggerPresence::SetPresence(bool inPresence)
{
	// If we current have presence and inPresence is false
	if(this->mPresence && !inPresence)
	{
		// fire mTargetLeave
		FireTargets(this->mTargetLeave.c_str(), this, this, USE_TOGGLE, 0.0f);
	}
	// else if we don't have presence and inPresence is true
	else if(!this->mPresence && inPresence)
	{
		// fire mTargetEnter
		if(this->mTargetEnter != "")
		{
			FireTargets(this->mTargetEnter.c_str(), this, this, USE_TOGGLE, 0.0f);
		}
	}
	
	// Set mPresence to inPresence
	this->mPresence = inPresence;
}

void TriggerPresence::Spawn(void)
{
	this->Precache();
	
	CBaseEntity::Spawn();
	
	// Spawn code
	this->SetTouch(&TriggerPresence::TriggerTouch);

	this->pev->movetype = MOVETYPE_NONE;
	this->pev->solid = SOLID_TRIGGER;
	
	SET_MODEL(ENT(pev), STRING(pev->model));
	this->pev->effects = EF_NODRAW;
	
	// Set spawn flags
	if(this->pev->spawnflags & 1)
	{
		this->mPlayersDontActivate = true;
	}
	if(this->pev->spawnflags & 2)
	{
		this->mMonstersDontActivate = true;
	}
	if(this->pev->spawnflags & 4)
	{
		this->mPushablesDontActivate = true;
	}
	if(this->pev->spawnflags & 8)
	{
		this->mTeamAOnly = true;
	}
	if(this->pev->spawnflags & 16)
	{
		this->mTeamBOnly = true;
	}
	
	// Don't start enabled if a master was specified
	if(this->mMaster != "")
	{
		this->mEnabled = false;
	}

	SetThink(&TriggerPresence::TriggerThink);
	this->pev->nextthink = gpGlobals->time + kTriggerPresenceThinkInterval;
}

void TriggerPresence::TriggerThink()
{
	if(this->mEnabled)
	{
		// If we have presence and haven't received a touch for a certain amount of time
		if(this->mPresence)
		{
			if(gpGlobals->time > this->mTimeOfLastTouch + this->mTimeBeforeLeave)
			{
				this->SetPresence(false);
			}
		}

		// Keep firing momentary target if presence detected
		if(this->mMomentaryTarget != "")
		{
			// Update the value if we're opening or closing
			float theDifference = this->mPresence ? kTriggerPresenceThinkInterval/this->mMomentaryOpenTime : -kTriggerPresenceThinkInterval/this->mMomentaryCloseTime;
			this->mMomentaryValue += theDifference;
			this->mMomentaryValue = min(max(this->mMomentaryValue, 0.0f), 1.0f);
				
			//float theAmount = RANDOM_FLOAT(0, 1);
			FireTargets(this->mMomentaryTarget.c_str(), this, this, USE_SET, this->mMomentaryValue);
		}
	}
	
	this->pev->nextthink = gpGlobals->time + kTriggerPresenceThinkInterval;
}

void TriggerPresence::TriggerTouch(CBaseEntity *pOther)
{
	if(this->mEnabled)
	{
		// If players activate and is player
		CBasePlayer* thePlayer = dynamic_cast<CBasePlayer*>(pOther);
		CBaseMonster* theMonster = dynamic_cast<CBaseMonster*>(pOther);
		CPushable* thePushable = dynamic_cast<CPushable*>(pOther);
		
		bool theTriggerFires = true;
		
		if(thePlayer && this->mPlayersDontActivate)
		{
			theTriggerFires = false;
		}
		if(theMonster && this->mMonstersDontActivate)
		{
			theTriggerFires = false;
		}
		if(thePushable && this->mPushablesDontActivate)
		{
			theTriggerFires = false;
		}
		if(this->mTeamAOnly && (pOther->pev->team != GetGameRules()->GetTeamA()->GetTeamNumber()))
		{
			theTriggerFires = false;
		}
		if(this->mTeamBOnly && (pOther->pev->team != GetGameRules()->GetTeamB()->GetTeamNumber()))
		{
			theTriggerFires = false;
		}
		
		//if(thePlayer)
		//{
		//	ClientPrint(thePlayer->pev, HUD_PRINTCONSOLE, "Player touch\n");
		//}

		if(theTriggerFires)
		{
			this->SetPresence(true);
			this->mTimeOfLastTouch = gpGlobals->time;
		}
	}
}

void TriggerPresence::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE inUseType, float inValue)
{
	switch(inUseType)
	{
	case USE_OFF:
		this->mEnabled = false;
		break;
	case USE_ON:
		this->mEnabled = true;
		break;
	case USE_SET:
		this->mEnabled = (bool)inValue;
		break;
	case USE_TOGGLE:
		this->mEnabled = !this->mEnabled;
		break;
	}
}


AvHTriggerRandom::AvHTriggerRandom()
{
	this->mMinFireTime = 0;
	this->mMaxFireTime = 1;
	this->mWaitBeforeReset = 3;

	this->mStartOn = false;
	this->mToggle = false;
	this->mRemoveOnFire = false;

	this->mFiredAtLeastOnce = false;
	this->mToggleableAndOn = false;
	this->mTimeOfLastActivation = -1;
	this->mTimeOfLastTrigger = -1;
}


void AvHTriggerRandom::KeyValue( KeyValueData* pkvd )
{
	pkvd->fHandled = FALSE;

	if (FStrEq(pkvd->szKeyName, "minfiretime"))
	{
		this->mMinFireTime = atof( pkvd->szValue );
		pkvd->fHandled = TRUE;
	} 
	else if (FStrEq(pkvd->szKeyName, "maxfiretime"))
	{
		this->mMaxFireTime = atof(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "wait"))
	{
		this->mWaitBeforeReset = atoi(pkvd->szValue);
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "spawnflags"))
	{
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "balancedtarget"))
	{
		this->mBalancedTarget = pkvd->szValue;
		pkvd->fHandled = TRUE;
	}
	else
	{
		// Else it's a target name, add it as a target
		this->mTargetList.push_back(pkvd->szKeyName);
		pkvd->fHandled = TRUE;
	}
}

void AvHTriggerRandom::ResetEntity()
{
	this->mFiredAtLeastOnce = false;
	this->mTimeOfLastActivation = -1;
	this->mTimeOfLastTrigger = -1;
	this->mToggleableAndOn = false;
	
	// Fire first if necessary
	if(this->mStartOn)
	{
		this->Use(NULL, NULL, USE_TOGGLE, 0.0f);
	}
}

void AvHTriggerRandom::SetNextTrigger()
{
	float theRandomTime = RANDOM_FLOAT(this->mMinFireTime, this->mMaxFireTime);
	this->pev->nextthink = gpGlobals->time + theRandomTime;
	this->mTimeOfLastTrigger = this->pev->nextthink;
}


void AvHTriggerRandom::Spawn(void)
{
	this->Precache();

    this->pev->classname = MAKE_STRING(kesTriggerRandom);
	this->pev->effects = EF_NODRAW;
	
	// Start on
	if(pev->spawnflags & 1)
	{
		this->mStartOn = true;
	}
	// Toggle
	if(pev->spawnflags & 2)
	{
		this->mToggle = true;
	}
	// Remove on fire
	if(pev->spawnflags & 4)
	{
		this->mRemoveOnFire = true;
	}

	if(this->mTargetList.size() == 0)
	{
		ALERT(at_warning, "No targets found in trigger_random (%s)", STRING(this->pev->targetname));
	}
}

void AvHTriggerRandom::TriggerTargetThink(void)
{
	// Pick random target in list
	int theNumTargetsToChooseFrom = this->mTargetList.size();
	if(theNumTargetsToChooseFrom > 0)
	{
		// Fire targets with this entity name
		string theTargetName;

		if(GetGameRules()->GetIsTournamentMode() && (this->mBalancedTarget != ""))
		{
			theTargetName = this->mBalancedTarget;
		}
		else
		{
			int theRandomIndex = RANDOM_LONG(0, theNumTargetsToChooseFrom - 1);
			theTargetName = this->mTargetList[theRandomIndex];
		}

		FireTargets(theTargetName.c_str(), NULL, NULL, USE_TOGGLE, 0.0f);

		if(this->mToggleableAndOn)
		{
			this->SetNextTrigger();
		}
		else
		{
			// Set think to NULL
			SetThink(NULL);
		}
	}
}

void AvHTriggerRandom::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value)
{
	// Don't activate if it's a one shot trigger
	if(!this->mRemoveOnFire || (!this->mFiredAtLeastOnce && (this->mTimeOfLastActivation == -1)))
	{
		// If haven't been triggered yet, or is toggleable
		if(!this->mFiredAtLeastOnce || this->mToggle)
		{
			// Don't trigger faster than wait time
			float theCurrentTime = gpGlobals->time;
			if((this->mTimeOfLastActivation == -1) || theCurrentTime >= (this->mTimeOfLastActivation + this->mWaitBeforeReset))
			{
				// Pick random time between min and max for think
				SetThink(&AvHTriggerRandom::TriggerTargetThink);
				this->SetNextTrigger();
				this->mTimeOfLastActivation = theCurrentTime;
				
				if(this->mToggle)
				{
					this->mToggleableAndOn = !this->mToggleableAndOn;
				}
			}
		}
	}
}



AvHTriggerScript::AvHTriggerScript()
{
	this->mStartOn = false;
	this->mTriggered = false;
}

void AvHTriggerScript::KeyValue(KeyValueData* pkvd)
{
	if(FStrEq(pkvd->szKeyName, "scriptname"))
	{
		const char* theCStrLevelName = STRING(gpGlobals->mapname);
		if(theCStrLevelName && !FStrEq(theCStrLevelName, ""))
		{
			string theLevelName = theCStrLevelName;
			this->mScriptName = AvHSHUBuildExecutableScriptName(string(pkvd->szValue), theLevelName);
		
			pkvd->fHandled = TRUE;
		}
	}
	else
	{
		CBaseEntity::KeyValue(pkvd);
	}
	
}

void AvHTriggerScript::ResetEntity()
{
	if(this->mStartOn)
	{
		this->Trigger();
	}
}

void AvHTriggerScript::Spawn()
{
	this->Precache();
	
	this->pev->classname = MAKE_STRING(kesTriggerScript);
	
	// Start on
	if(this->pev->spawnflags & 1)
	{
		this->mStartOn = true;
	}
}

void AvHTriggerScript::Trigger()
{
	AvHScriptManager::Instance()->RunScript(this->mScriptName);
}


void AvHTriggerScript::Use(CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE inUseType, float inValue)
{
	bool theNewState = false;

	switch(inUseType)
	{
	case USE_OFF:
		theNewState = false;
		break;
	case USE_ON:
		theNewState = true;
		break;
	case USE_SET:
		theNewState = (bool)inValue;
		break;
	case USE_TOGGLE:
		theNewState = !this->mTriggered;
		break;
	}

	// Trigger if necessary
	if((this->mTriggered != theNewState) && theNewState)
	{
		this->Trigger();
	}

	// Update triggered state
	this->mTriggered = theNewState;
}


AvHWebStrand::AvHWebStrand()
{
}

void AvHWebStrand::Break()
{
	EMIT_SOUND(ENT(this->pev), CHAN_AUTO, kWebStrandBreakSound, 1.0, ATTN_IDLE);
	UTIL_Remove(this);

	// Decrement number of strands
	AvHTeam* theTeam = GetGameRules()->GetTeam((AvHTeamNumber)this->pev->team);
	ASSERT(theTeam);
	theTeam->SetNumWebStrands(theTeam->GetNumWebStrands() - 1);
}

void AvHWebStrand::Killed(entvars_t *pevAttacker, int iGib)
{
	this->Break();

	CBeam::Killed(pevAttacker, iGib);
}

void AvHWebStrand::Precache(void)
{
	// Precache web strand sprite
	PRECACHE_UNMODIFIED_MODEL(kWebStrandSprite);
	PRECACHE_UNMODIFIED_SOUND(kWebStrandBreakSound);
}

void AvHWebStrand::Setup(const Vector& inPointOne, const Vector& inPointTwo)
{
	// Create a new entity with CBeam private data
	this->BeamInit(kWebStrandSprite, kWebStrandWidth);

	this->PointsInit(inPointOne, inPointTwo);
	this->SetColor( 255, 255, 255 );
	this->SetScrollRate( 0 );
	//this->SetBrightness( 64 );
	this->SetBrightness( 8 );

	this->pev->classname = MAKE_STRING(kesTeamWebStrand);
}

void AvHWebStrand::Spawn(void)
{
	this->Precache();

	CBeam::Spawn();

	// Spawn code
	this->SetTouch(&AvHWebStrand::StrandTouch);
	this->pev->solid = SOLID_TRIGGER;
	//this->pev->solid = SOLID_BBOX;
	this->pev->health = kWebHitPoints;
	this->pev->takedamage = DAMAGE_YES;

	//SetBits(this->pev->flags, FL_MONSTER);
	
	this->RelinkBeam();

	//SetThink(StrandExpire);
	//this->pev->nextthink = gpGlobals->time + kWebStrandLifetime;
}

void AvHWebStrand::StrandExpire()
{
	this->Break();
}


void AvHWebStrand::StrandTouch( CBaseEntity *pOther )
{
	// Webs can never break on friendlies
	//if(GetGameRules()->CanEntityDoDamageTo(this, pOther))
	if(pOther->pev->team != this->pev->team)
	{
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(pOther);
		if(thePlayer && thePlayer->GetCanBeAffectedByEnemies())
		{
			// Break web and ensnare player if possible (players can't be too webbed)
			if(thePlayer->SetEnsnareState(true))
			{
				this->Break();
			}
		}
	}
}

int	AvHWebStrand::TakeDamage( entvars_t *pevInflictor, entvars_t *pevAttacker, float flDamage, int bitsDamageType )
{
	float theDamage = 0;
	
	//if(bitsDamageType == DMG_SLASH)
	//{
		theDamage = flDamage;
	//}

	if(!pevAttacker)
	{
		pevAttacker = pevInflictor;
	}
	
	if(!pevInflictor)
	{
		pevInflictor = pevAttacker;
	}
	
	return CBeam::TakeDamage(pevInflictor, pevAttacker, theDamage, bitsDamageType);
}



const float kResourceThinkInterval = 1.0f;

AvHFuncResource::AvHFuncResource()
{
	this->mParticleSystemIndex = -1;
	this->mOccupied = false;
	this->mLastTimeDrawnUpon = -1;
}

void AvHFuncResource::DeleteParticleSystem()
{
	int theParticleSystemIndex = this->GetParticleSystemIndex();
	if(theParticleSystemIndex != -1)
	{
		AvHParticleSystemEntity* theParticleSystemEntity = NULL;
		if(AvHSUGetEntityFromIndex(theParticleSystemIndex, theParticleSystemEntity))
		{
			ASSERT(theParticleSystemEntity);
			
			UTIL_Remove(theParticleSystemEntity);
			this->mParticleSystemIndex = -1;
		}
	}
}

//int	AvHFuncResource::ObjectCaps(void) 
//{ 
//	return FCAP_CONTINUOUS_USE;
//}

void AvHFuncResource::DrawUse(CBaseEntity* inActivator, CBaseEntity* inCaller, USE_TYPE useType, float value)
{
	// If nozzle is unoccupied
	if(!this->mOccupied)
	{
		// Check if gorge is using nozzle
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inActivator);
		if(thePlayer && thePlayer->GetIsRelevant() && (thePlayer->GetUser3() == AVH_USER3_ALIEN_PLAYER2))
		{
			// Check interval.  It slows down as the gorge becomes full.  Automatically allows multiple gorges feeding off the same node to balance out (guy with less will hog node, then they should ping back and forth).
			float thePlayerResources = thePlayer->GetResources();
			float theDrawMinInterval = BALANCE_VAR(kDrawMinInterval);
			float theDrawMaxInterval = BALANCE_VAR(kDrawMaxInterval);
			float theDrawInterval = theDrawMinInterval + (theDrawMaxInterval - theDrawMinInterval)*(thePlayerResources/kMaxAlienResources);
			if((this->mLastTimeDrawnUpon == -1) || (gpGlobals->time > (this->mLastTimeDrawnUpon + theDrawInterval)))
			{
				// Give gorge some resources
				float theDrawAmount = BALANCE_VAR(kDrawAmount);
				thePlayer->SetResources(thePlayer->GetResources() + theDrawAmount);

				// Play animation
				thePlayer->PlayerConstructUse();

				// Play sound
				AvHSUPlayRandomConstructionEffect(thePlayer, this);
			
				// Remember this
				this->mLastTimeDrawnUpon = gpGlobals->time;
			}
		}
	}
}

void AvHFuncResource::TurnOffParticleSystem()
{
	int theParticleSystemIndex = this->GetParticleSystemIndex();
	if(theParticleSystemIndex != -1)
	{
		AvHParticleSystemEntity* theParticleSystemEntity = NULL;
		if(AvHSUGetEntityFromIndex(theParticleSystemIndex, theParticleSystemEntity))
		{
			ASSERT(theParticleSystemEntity);
			theParticleSystemEntity->SetUseState(USE_OFF);
				
			//UTIL_Remove(theParticleSystemEntity);
			//this->mParticleSystemIndex = -1;
		}
	}
}

void AvHFuncResource::TurnOnParticleSystem()
{
	int theParticleSystemIndex = this->GetParticleSystemIndex();
	if(theParticleSystemIndex == -1)
	{
		uint32 theTemplateIndex;
		if(gParticleTemplateList.GetTemplateIndexWithName(kpsResourceEmission, theTemplateIndex))
		{
			edict_t	*pent;
			AvHParticleSystemEntity* thePSEntity;
			
			pent = CREATE_NAMED_ENTITY(MAKE_STRING(kesParticles));
			if ( FNullEnt( pent ) )
			{
				ALERT ( at_console, "NULL Ent in Create!\n" );
				ASSERT(false);
			}
			
			thePSEntity = dynamic_cast<AvHParticleSystemEntity*>(CBaseEntity::Instance(pent));
			ASSERT(thePSEntity);
			thePSEntity->SetTemplateIndex(theTemplateIndex);
			
			Vector thePosition;
			thePosition.x = (this->pev->absmax.x + this->pev->absmin.x)/2.0f;
			thePosition.y = (this->pev->absmax.y + this->pev->absmin.y)/2.0f;
			thePosition.z = (this->pev->absmax.z + this->pev->absmin.z)/2.0f;
			
			thePSEntity->pev->origin = thePosition;
			thePSEntity->pev->angles = this->pev->angles;
			
			DispatchSpawn(thePSEntity->edict());
			
			this->mParticleSystemIndex = thePSEntity->entindex();
		}
	}

	if(theParticleSystemIndex != -1)
	{
		AvHParticleSystemEntity* theParticleSystemEntity = NULL;
		if(AvHSUGetEntityFromIndex(theParticleSystemIndex, theParticleSystemEntity))
		{
			ASSERT(theParticleSystemEntity);
			theParticleSystemEntity->SetUseState(USE_ON);
			
			//UTIL_Remove(theParticleSystemEntity);
			//this->mParticleSystemIndex = -1;
		}
	}
}

void AvHFuncResource::FuncResourceThink()
{
	// If we don't have a resource tower, and we haven't created a particle system
	if(!this->mOccupied)
	{
		this->TurnOnParticleSystem();
	}

	//SetUse(&AvHFuncResource::DrawUse);

	this->pev->nextthink = gpGlobals->time + kResourceThinkInterval;
}

bool AvHFuncResource::GetIsActive() const
{
	return this->mActive;
}


bool AvHFuncResource::GetIsOccupied() const
{
	return this->mOccupied;
}

int	AvHFuncResource::GetParticleSystemIndex() const
{
	return this->mParticleSystemIndex;
}

void AvHFuncResource::KeyValue( KeyValueData* pkvd )
{
	if (FStrEq(pkvd->szKeyName, "targetOnBuild"))
	{
		this->mTargetOnBuild = pkvd->szValue;
		pkvd->fHandled = TRUE;
	}
	else if (FStrEq(pkvd->szKeyName, "targetOnDestroy"))
	{
		this->mTargetOnDestroy = pkvd->szValue;
		pkvd->fHandled = TRUE;
	}
	else
	{
		CBaseEntity::KeyValue(pkvd);
	}
}

void AvHFuncResource::Precache()
{
	CBaseAnimating::Precache();
	PRECACHE_UNMODIFIED_MODEL((char*)STRING(this->pev->model));
}

void AvHFuncResource::ResetEntity()
{
	// Set user3 in case it was removed when resource was destroyed
	this->pev->iuser3 = AVH_USER3_FUNC_RESOURCE;

	// Reset our properties back to original unharvested settings
	this->mOccupied = false;
	this->DeleteParticleSystem();

	this->mLastTimeDrawnUpon = -1;
}

void AvHFuncResource::Spawn()
{
	this->Precache();

	this->pev->solid = SOLID_BBOX;
	this->pev->movetype = MOVETYPE_TOSS;
	
	SET_MODEL(ENT(this->pev), STRING(this->pev->model));
	// set size and link into world
//	UTIL_SetOrigin(pev, pev->origin);               
	
//	DROP_TO_FLOOR(ENT(pev));

	//this->pev->solid = SOLID_TRIGGER;
	
    this->pev->classname = MAKE_STRING(kesFuncResource);

	// Set health greater then 0 so it seems "alive" (won't be needed when buildings can be selected, allows research to continue)
	this->pev->health = 1;
	this->pev->max_health = pev->health;
	this->pev->takedamage = DAMAGE_NO;
	
	// For some reason, drop before setting size
	DROP_TO_FLOOR(ENT(pev));

	UTIL_SetOrigin(this->pev, this->pev->origin);
	UTIL_SetSize(this->pev, kFuncResourceMinSize, kFuncResourceMaxSize);
	
	this->pev->iuser3 = AVH_USER3_FUNC_RESOURCE;

	SetUpgradeMask(&this->pev->iuser4, MASK_SELECTABLE);

	SetThink(&AvHFuncResource::FuncResourceThink);
	this->pev->nextthink = gpGlobals->time + kResourceThinkInterval;
}

void AvHFuncResource::TriggerOccupy()
{
	this->mOccupied = true;
}

void AvHFuncResource::TriggerBuild()
{
	this->mActive = true;
	if(this->mTargetOnBuild != "")
	{
		FireTargets(this->mTargetOnBuild.c_str(), NULL, NULL, USE_TOGGLE, 0.0f);
	}

	this->TurnOffParticleSystem();
}

void AvHFuncResource::TriggerDestroy()
{
	this->mActive = false;
	this->mOccupied = false;

	if(this->mTargetOnDestroy != "")
	{
		FireTargets(this->mTargetOnDestroy.c_str(), NULL, NULL, USE_TOGGLE, 0.0f);
	}
}


AvHFog::AvHFog()
{
	this->mFogColor[0] = this->mFogColor[1] = this->mFogColor[2] = 255;
	this->mFogStart = 0;
	this->mFogEnd = 1000;
	this->mFogExpireTime = 1.0f;
}

void AvHFog::KeyValue(KeyValueData* pkvd)
{
	if(FStrEq(pkvd->szKeyName, "fogcolor"))
	{
		if(sscanf(pkvd->szValue, "%d %d %d", this->mFogColor + 0, this->mFogColor + 1, this->mFogColor + 2) == 3)
		{
			pkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(pkvd->szKeyName, "fogstart"))
	{
		if(sscanf(pkvd->szValue, "%f", &this->mFogStart) == 1)
		{
			pkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(pkvd->szKeyName, "fogend"))
	{
		if(sscanf(pkvd->szValue, "%f", &this->mFogEnd) == 1)
		{
			pkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(pkvd->szKeyName, "fogexpire"))
	{
		if(sscanf(pkvd->szValue, "%f", &this->mFogExpireTime) == 1)
		{
			pkvd->fHandled = TRUE;
		}
	}
	else
	{
		CBaseEntity::KeyValue(pkvd);
	}
}

void AvHFog::FogTouch(CBaseEntity* inEntity)
{
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(inEntity);
	if(thePlayer)
	{
		thePlayer->TriggerFog(this->entindex(), this->mFogExpireTime);
	}
}

void AvHFog::GetFogColor(int& outRed, int& outGreen, int& outBlue) const
{
	outRed = this->mFogColor[0];
	outGreen = this->mFogColor[1];
	outBlue = this->mFogColor[2];
}

float AvHFog::GetFogEnd() const
{
	return this->mFogEnd;
}

float AvHFog::GetFogStart() const
{
	return this->mFogStart;
}

void AvHFog::Spawn()
{
	this->Precache();

	// Don't show model but still allow triggering of it
	CBaseEntity::Spawn();
	
	this->pev->movetype = MOVETYPE_NONE;
	this->pev->solid = SOLID_TRIGGER;
	
	SET_MODEL(ENT(pev), STRING(pev->model));
	this->pev->effects = EF_NODRAW;

	// Set touch
	SetTouch(&AvHFog::FogTouch);
}



AvHResourceTower::AvHResourceTower() : AvHBaseBuildable(TECH_NULL, BUILD_RESOURCES, kwsResourceTower, AVH_USER3_RESTOWER)
{
	this->mResourceEntityIndex = -1;
	this->mTechLevel = 1;
	this->mTimeLastContributed = -1;
	this->mTimeOfLastSound = -1;
	this->mActivateTime = 0;
}

AvHResourceTower::AvHResourceTower(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inUser4) : AvHBaseBuildable(inTechID, inMessageID, inClassName, inUser4)
{
	this->mResourceEntityIndex = -1;
	this->mTechLevel = 1;
}

int	AvHResourceTower::GetSequenceForBoundingBox() const
{
	return 1;
}

// Called at end of deploy animation
void AvHResourceTower::ActivateThink()
{
	// Set regular think, but don't start for a little bit
	SetThink(&AvHResourceTower::ResourceTowerThink);
	this->pev->nextthink = gpGlobals->time + 4.0f;
	this->mTimeOfLastSound = -1;
	this->mScannedForFuncResource = false;
}

float AvHResourceTower::GetTimeLastContributed()
{
	return this->mTimeLastContributed;
}

char* AvHResourceTower::GetActiveSoundList() const
{
	return kResourceTowerSoundList;
}

int	AvHResourceTower::GetPointValue() const
{
	return BALANCE_VAR(kScoringResourceTowerValue);
}

void AvHResourceTower::SetTimeLastContributed(float inTime)
{
	this->mTimeLastContributed = inTime;
}

int AvHResourceTower::GetIdleAnimation() const
{
	int theIdleAnimation = -1;

	if(this->GetIsBuilt())
	{
		theIdleAnimation = this->GetActiveAnimation();
	}

	return theIdleAnimation;
}

void AvHResourceTower::ResourceTowerThink(void)
{
	if(this->GetIsBuilt())
	{
		// Check if
		if((this->mTimeOfLastSound == -1) || ((gpGlobals->time - this->mTimeOfLastSound) > kResourceTowerMinSoundInterval))
		{
			if(RANDOM_LONG(0, 3) == 1)
			{
				char* theSoundList = this->GetActiveSoundList();
				if(theSoundList)
				{
					gSoundListManager.PlaySoundInList(theSoundList, this, CHAN_BODY, .2f);
					this->mTimeOfLastSound = gpGlobals->time;
				}
			}
		}

		AvHFuncResource* theFuncResource= this->GetHostResource();
		if(theFuncResource)
		{
			theFuncResource->TriggerBuild();
		}

		AvHBaseBuildable::AnimateThink();
	}
	
	this->pev->nextthink = gpGlobals->time + .05f;
}

AvHFuncResource* AvHResourceTower::GetHostResource() const
{
	AvHFuncResource* theFuncResource = NULL;

	if(this->mResourceEntityIndex > 0)
	{
		// Look up func_resource
		edict_t* theEdict = g_engfuncs.pfnPEntityOfEntIndex(this->mResourceEntityIndex);
		if(!theEdict->free)
		{
			CBaseEntity* theEntity = CBaseEntity::Instance(theEdict);
			if(theEntity)
			{
				theFuncResource = dynamic_cast<AvHFuncResource*>(theEntity);
			}
		}
	}

	return theFuncResource;
}

bool AvHResourceTower::GetIsActive() const
{
	bool theIsActive = false;

	if(this->GetIsBuilt() && this->pev && !GetHasUpgrade(this->pev->iuser4, MASK_RECYCLING))
	{
		theIsActive = true;
	}

	return theIsActive;
}


int	AvHResourceTower::GetMaxHitPoints() const
{
	return 200;
}

int	AvHResourceTower::GetResourceEntityIndex() const
{
	return this->mResourceEntityIndex;
}

int	AvHResourceTower::GetTechLevel() const
{
	return this->mTechLevel;
}

void AvHResourceTower::Killed(entvars_t* pevAttacker, int iGib)
{
	AvHBaseBuildable::Killed(pevAttacker, iGib);

	// Look up AvHFuncResource and tell it to reset
	AvHFuncResource* theHostResource = this->GetHostResource();
	if(theHostResource)
	{
		theHostResource->TriggerDestroy();
	}

	// Play death sequence
	this->PlayAnimationAtIndex(this->GetKilledAnimation());
}

void AvHResourceTower::Precache(void)
{
	AvHBaseBuildable::Precache();

	//PRECACHE_SOUND(this->GetHarvestSound());
	PRECACHE_UNMODIFIED_SOUND(this->GetDeploySound());
	PRECACHE_UNMODIFIED_MODEL(this->GetModelName());
}

void AvHResourceTower::SetActivateTime(int inTime)
{
	this->mActivateTime = inTime;
}

void AvHResourceTower::SetHasBeenBuilt()
{
	AvHBuildable::SetHasBeenBuilt();
	
	// Set time for end of animation
	SetThink(&AvHResourceTower::ActivateThink);
	//this->pev->nextthink = gpGlobals->time + 8.0f;

	this->mTimeLastContributed = this->pev->nextthink = (gpGlobals->time + this->mActivateTime);
}

void AvHResourceTower::Spawn()
{
	AvHBaseBuildable::Spawn();

	this->mTimeLastContributed = gpGlobals->time;

	this->mActivateTime = BALANCE_VAR(kResourceTowerActivateTime);

	//Claim ourselves a func_resource immediately and set it occupied
	if(!this->mScannedForFuncResource && (this->mResourceEntityIndex <= 0))
	{
		float theNearestDistance = 10000000;
		int theFuncResourceID = -1;
		
		// Find nearest unoccupied func_resource and set the building's resource id
		FOR_ALL_ENTITIES(kesFuncResource, AvHFuncResource*)
			if(!theEntity->GetIsOccupied())
			{
				float theDistance = VectorDistance(theEntity->pev->origin, this->pev->origin);
				if(theDistance < theNearestDistance)
				{
					theNearestDistance = theDistance;
					theFuncResourceID = theEntity->entindex();
				}
			}
		END_FOR_ALL_ENTITIES(kesFuncResource)
			
		if(theFuncResourceID != -1)
		{
			this->mResourceEntityIndex = theFuncResourceID;

			AvHFuncResource* theFuncResource= this->GetHostResource();
			if(theFuncResource)
			{
				theFuncResource->TriggerOccupy();
			}
		}

		this->mScannedForFuncResource = true;
	}
}

void AvHResourceTower::Upgrade()
{
	AvHFuncResource* theFuncResource = this->GetHostResource();
	
	// Increment our tech level
	this->mTechLevel++;
}

char* AvHResourceTower::GetClassName() const
{
	return kwsResourceTower;
}

//char* AvHResourceTower::GetHarvestSound() const
//{
//	return kResourceTowerHarvestSound;
//}

char* AvHResourceTower::GetDeploySound() const
{
	return kResourceTowerDeploySound;
}

char* AvHResourceTower::GetModelName() const
{
	return kResourceTowerModel;
}


void AvHInfoLocation::KeyValue(KeyValueData* pkvd)
{
	// Look for name
	if(FStrEq(pkvd->szKeyName, "locationname"))
	{
		if(pkvd->szValue != "")
		{
			this->mLocationName = string(pkvd->szValue);
			pkvd->fHandled = TRUE;
		}
	}
	else
	{
		CBaseEntity::KeyValue(pkvd);
	}
}

void AvHInfoLocation::Spawn()
{
	// If we have a valid name
	if(this->mLocationName != "")
	{
		// Set model, size, hook into world
		SET_MODEL(ENT(this->pev), STRING(this->pev->model));
		this->pev->movetype = MOVETYPE_NONE;
		this->pev->solid = SOLID_NOT;
		UTIL_SetOrigin(this->pev, this->pev->origin);

		// Set as nodraw
		this->pev->effects |= EF_NODRAW;

		// Compute min and max
		this->mMaxExtent = this->pev->maxs;
		this->mMinExtent = this->pev->mins;
	}
	else
	{
		// Mark for removal
		UTIL_Remove(this);
	}
}

void AvHInfoLocation::UpdateOnRemove(void)
{
	int a = 0;
}

