//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: AvH event declarations 
//
// $Workfile: AvHEvents.cpp$
// $Date: 2002/10/24 21:23:37 $
//
//-------------------------------------------------------------------------------
// $Log: AvHEvents.cpp,v $
// Revision 1.47  2002/10/24 21:23:37  Flayra
// - Reworked jetpacks
// - Removed bullet casing from grenade launcher
//
// Revision 1.46  2002/10/16 20:52:34  Flayra
// - Removed weapon upgrade sounds
// - Fixed acid projectile hitting player
// - Play HMG firing at full volume
// - Added paralysis projectile
// - Fixed spike orientation
//
// Revision 1.45  2002/10/16 00:54:44  Flayra
// - Removed unneeded events
// - Commented out broken parasite projectile
// - Added general particle event
// - Added distress beacon event
//
// Revision 1.44  2002/09/25 20:43:53  Flayra
// - Effects update
//
// Revision 1.43  2002/09/23 22:13:38  Flayra
// - Fixed bug where damage upgrades were applying when they shouldn't (for aliens)
// - Updated spike effect
// - Removed jetpack effect until it can be done properly
// - Lots of sound and effects changes
//
// Revision 1.42  2002/09/09 19:50:29  Flayra
// - Reworking jetpack effects, still needs more work
//
// Revision 1.41  2002/08/31 18:01:01  Flayra
// - Work at VALVe
//
// Revision 1.40  2002/08/16 02:34:42  Flayra
// - Removed ASSERTs when creating temporary entities, it can fail in big firefights
//
// Revision 1.39  2002/08/09 00:56:38  Flayra
// - Added particle system back when phasing in items from commander mode
//
// Revision 1.38  2002/08/02 22:00:53  Flayra
// - Removed old HL events, add correct bullet types for new bullet effects
//
// Revision 1.37  2002/07/26 23:04:14  Flayra
// - Generate numerical feedback for damage events
//
// Revision 1.36  2002/07/23 17:02:23  Flayra
// - Turret velocity is constructed on client in same way as server, instead of being sent across as calculated (network precision issues)
//
// Revision 1.35  2002/07/10 14:40:23  Flayra
// - Added special spike hit effect
//
// Revision 1.34  2002/07/08 16:57:19  Flayra
// - Added "invalid action" event, reworking for random spread for bullet fire
//
// Revision 1.33  2002/07/01 22:41:40  Flayra
// - Removed outdated overwatch target and tension events
//
// Revision 1.32  2002/07/01 21:31:49  Flayra
// - Regular update
//
// Revision 1.31  2002/06/25 17:57:15  Flayra
// - Removed old events, added new events, fixed infinite loop crash in PlayMeleeHitEffects, added parasite projectile
//
// Revision 1.30  2002/06/03 16:44:27  Flayra
// - Moved offense chamber firing into event, fixed duplicate empty sound (now it's an event, not server-side), weapons play view model anim in weapon (not event), started to add grenade event for grenade gun
//
// Revision 1.29  2002/05/28 17:36:55  Flayra
// - Don't play machine gun or pistol sounds louder when upgraded, changed welder from looping sound to periodic sound
//
// Revision 1.28  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"

//#include "mod/AvHMarineWeapons.h"
//#include "mod/AvHAlienWeapons.h"
//#include "mod/AvHAlienAbilities.h"

#include "cl_dll/eventscripts.h"
#include "cl_dll/in_defs.h"
#include "cl_dll/ev_hldm.h"
#include "common/event_api.h"
#include "common/event_args.h"
#include "common/dlight.h"
#include "common/r_efx.h"
#include "mod/AvHMarineWeaponConstants.h"
#include "mod/AvHAlienWeaponConstants.h"
#include "mod/AvHParticleSystemManager.h"
#include "pm_shared/pm_defs.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHEvents.h"
#include "mod/AvHSelectionHelper.h"
#include "pm_shared/pm_defs.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHParticleConstants.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHAlienAbilityConstants.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "mod/AvHParticleTemplate.h"
#include "mod/AvHParticleTemplateClient.h"
#include "mod/AvHClientVariables.h"
#include "util/MathUtil.h"
#include "mod/AvHHulls.h"

//extern AvHKnife				gKnife;
//extern AvHMachineGun		gMachineGun;
//extern AvHPistol			gPistol;
//extern AvHSonicGun			gSonicGun;
//extern AvHHeavyMachineGun	gHeavyMachineGun;
//extern AvHGrenadeGun		gGrenadeGun;
//extern AvHWelder			gWelder;
//extern AvHMine				gMine;
//extern AvHSpitGun			gSpitGun;
//extern AvHClaws				gClaws;
//extern AvHSpore				gSpores;
//extern AvHBite				gBite;
//extern AvHBite2				gBite2;
//extern AvHSpikeGun			gSpikeGun;
//extern AvHSwipe				gSwipe;
//extern AvHWebSpinner		gWebSpinner;
//extern AvHPrimalScream		gPrimalScream;
//extern AvHParasiteGun		gParasite;
//extern AvHUmbraGun			gUmbra;
//extern AvHBlinkGun			gBlink;
//extern AvHDivineWind		gDivineWind;
//extern AvHParalysisGun		gParalysisGun;
//extern AvHBileBombGun		gBileBomb;
//extern AvHAcidRocketGun		gAcidRocket;
//extern AvHHealingSpray		gHealingSpray;
//extern AvHBabblerGun		gBabblerGun;

// Alien abilities
//extern AvHLeap				gLeap;
//extern AvHCharge			gCharge;


// The sound constants are stored here, even though it's used on client
#define SND_CHANGE_PITCH	(1<<7)		// duplicated in protocol.h change sound pitch
#define SND_CHANGE_VOL		(1<<6)		// duplicated in protocol.h change sound vol

void V_PunchAxis( int axis, float punch );
//extern double gClientTimeLastUpdate;
void DrawCircleOnGroundAtPoint(vec3_t inOrigin, int inNumSides, int inStartAngle, int inRadius, float inR, float inG, float inB, float inA, bool inUseRedInstead, float inInnerRadius);

extern void ComputeGrenadeVelocity(Vector& inForward, Vector& inVelocity, Vector& outVelocity);

class LightType
{
public:
	LightType(int inIndex, dlight_t* inLight) : mIndex(inIndex), mLight(inLight)
		{}

	int			mIndex;
	dlight_t*	mLight;
};

typedef vector<LightType>				DLightListType;

DLightListType							gJetpackLights;
const float								kArbitraryLargeLightTime = 2000.0f;
//SelectionListType						gSelectionList;
extern playermove_t*					pmove;

#include "pm_shared/pm_debug.h"
extern DebugPointListType				gTriDebugLocations;
extern DebugPointListType				gSquareDebugLocations;
extern DebugEntityListType				gCubeDebugEntities;

AvHSelectionHelper						gSelectionHelper;
extern AvHParticleTemplateListClient	gParticleTemplateList;

extern const Vector						g_vecZero;

//
// Macros to make these things easier, less error-prone and clearer
//
#define AVH_DECLARE_EVENT(s) \
extern "C" \
{\
void EV_##s(struct event_args_s *args); \
}

extern "C" Vector						gPredictedPlayerOrigin;

#define AVH_NAME_TO_EVENT(s) "events/"#s".sc"

#define AVH_HOOK_EVENT(s) \
    gEngfuncs.pfnHookEvent(AVH_NAME_TO_EVENT(s), EV_##s)

//#define AVH_DEFINE_EVENT(s, Class) \
//void EV_##s(struct event_args_s *args) \
//{\
//    static Class theWeapon(AVH_NAME_TO_EVENT(s));

//#define AVH_DEFINE_EVENT_END \
///* TODO: Add cleanup or other default behavior? */ \
//} 

//physent_t* GetEntity(int inPhysIndex);

//
// Declare events
//

//// Marine weapon events
AVH_DECLARE_EVENT(Knife)
AVH_DECLARE_EVENT(MachineGun)
AVH_DECLARE_EVENT(Pistol)
AVH_DECLARE_EVENT(SonicGun)
AVH_DECLARE_EVENT(HeavyMachineGun)
AVH_DECLARE_EVENT(GrenadeGun)
AVH_DECLARE_EVENT(Grenade)

// Alien weapon events
AVH_DECLARE_EVENT(SpitGun)
AVH_DECLARE_EVENT(OffenseChamber)
AVH_DECLARE_EVENT(Claws)
AVH_DECLARE_EVENT(Swipe)
//AVH_DECLARE_EVENT(EnsnareShoot)
//AVH_DECLARE_EVENT(EnsnareHit)

AVH_DECLARE_EVENT(SporeShoot)
AVH_DECLARE_EVENT(SporeCloud)
AVH_DECLARE_EVENT(UmbraGun)
AVH_DECLARE_EVENT(UmbraCloud)

AVH_DECLARE_EVENT(Bite)
AVH_DECLARE_EVENT(Bite2)
AVH_DECLARE_EVENT(SpikeGun)
//AVH_DECLARE_EVENT(LayEgg)
AVH_DECLARE_EVENT(BuildGun)
AVH_DECLARE_EVENT(HealingSpray)
AVH_DECLARE_EVENT(Metabolize)
AVH_DECLARE_EVENT(MetabolizeSuccess)
AVH_DECLARE_EVENT(SpinWeb)
//AVH_DECLARE_EVENT(Babbler)
AVH_DECLARE_EVENT(PrimalScream)
AVH_DECLARE_EVENT(Cocoon)

AVH_DECLARE_EVENT(Jetpack)
AVH_DECLARE_EVENT(Welder)
AVH_DECLARE_EVENT(WelderConst)
AVH_DECLARE_EVENT(WelderStart)
AVH_DECLARE_EVENT(WelderEnd)
//AVH_DECLARE_EVENT(OverwatchStart)
//AVH_DECLARE_EVENT(OverwatchTarget)
//AVH_DECLARE_EVENT(OverwatchTension)
//AVH_DECLARE_EVENT(OverwatchEnd)
AVH_DECLARE_EVENT(Regeneration)
AVH_DECLARE_EVENT(StartCloak);
AVH_DECLARE_EVENT(EndCloak);
//AVH_DECLARE_EVENT(WallJump)
AVH_DECLARE_EVENT(Flight)
AVH_DECLARE_EVENT(Select)
AVH_DECLARE_EVENT(Teleport)
AVH_DECLARE_EVENT(PhaseIn)
AVH_DECLARE_EVENT(SiegeHit)
AVH_DECLARE_EVENT(SiegeViewHit)
AVH_DECLARE_EVENT(StopScream)
AVH_DECLARE_EVENT(CommandPoints)
AVH_DECLARE_EVENT(AlienSightOn)
AVH_DECLARE_EVENT(AlienSightOff)
//AVH_DECLARE_EVENT(ParalysisGun)
//AVH_DECLARE_EVENT(ParalysisStart)
AVH_DECLARE_EVENT(ParasiteGun)
AVH_DECLARE_EVENT(BlinkSuccess)
AVH_DECLARE_EVENT(DivineWind)
AVH_DECLARE_EVENT(BileBomb)
AVH_DECLARE_EVENT(AcidRocket)
AVH_DECLARE_EVENT(Stomp);
AVH_DECLARE_EVENT(Devour);
AVH_DECLARE_EVENT(InvalidAction)
AVH_DECLARE_EVENT(Particle)
AVH_DECLARE_EVENT(DistressBeacon)
AVH_DECLARE_EVENT(LevelUp)

// Alien abilities
AVH_DECLARE_EVENT(Leap);
AVH_DECLARE_EVENT(Charge);
//AVH_DECLARE_EVENT(HiveHit)
AVH_DECLARE_EVENT(EmptySound)
AVH_DECLARE_EVENT(NumericalInfo)
AVH_DECLARE_EVENT(WeaponAnimation)
AVH_DECLARE_EVENT(Ability);

//extern "C"
//{
//void EV_FireGlock1( struct event_args_s *args  );
//void EV_FireGlock2( struct event_args_s *args  );
//}

//
// Hook 'em on Initialize
//
void Game_HookEvents( void )
{
//	gEngfuncs.pfnHookEvent( "events/glock1.sc",					EV_FireGlock1 );
//	gEngfuncs.pfnHookEvent( "events/glock2.sc",					EV_FireGlock2 );
	
    // Hook marine weapon events
	gEngfuncs.pfnHookEvent( kKNEventName, EV_Knife );
	gEngfuncs.pfnHookEvent( kMGEventName, EV_MachineGun );
	gEngfuncs.pfnHookEvent( kHGEventName, EV_Pistol );
	gEngfuncs.pfnHookEvent( kSGEventName, EV_SonicGun );
	gEngfuncs.pfnHookEvent( kHMGEventName, EV_HeavyMachineGun );
	gEngfuncs.pfnHookEvent( kGGEventName, EV_GrenadeGun );
    gEngfuncs.pfnHookEvent( kGREventName, EV_Grenade );

	// Alien weapon events
	gEngfuncs.pfnHookEvent( kSpitGEventName, EV_SpitGun );
	gEngfuncs.pfnHookEvent( kOffenseChamberEventName, EV_OffenseChamber);
	gEngfuncs.pfnHookEvent( kClawsEventName, EV_Claws );
	gEngfuncs.pfnHookEvent( kSwipeEventName, EV_Swipe );
	//gEngfuncs.pfnHookEvent( kEnsnareShootEventName, EV_EnsnareShoot);
	//gEngfuncs.pfnHookEvent( kEnsnareHitEventName, EV_EnsnareHit);
	
	gEngfuncs.pfnHookEvent( kSporeShootEventName, EV_SporeShoot);
	gEngfuncs.pfnHookEvent( kSporeCloudEventName, EV_SporeCloud);
	gEngfuncs.pfnHookEvent( kUmbraShootEventName, EV_UmbraGun);
	gEngfuncs.pfnHookEvent( kUmbraCloudEventName, EV_UmbraCloud);

	gEngfuncs.pfnHookEvent( kSpikeShootEventName, EV_SpikeGun);
	gEngfuncs.pfnHookEvent( kBiteEventName, EV_Bite);
	gEngfuncs.pfnHookEvent( kBite2EventName, EV_Bite2);
	//gEngfuncs.pfnHookEvent( kEggLayerShootEventName, EV_LayEgg);
	gEngfuncs.pfnHookEvent( kBuildingGunEventName, EV_BuildGun);
	gEngfuncs.pfnHookEvent( kHealingSprayEventName, EV_HealingSpray);
	gEngfuncs.pfnHookEvent( kMetabolizeEventName, EV_Metabolize);
	gEngfuncs.pfnHookEvent( kMetabolizeSuccessEventName, EV_MetabolizeSuccess);
	gEngfuncs.pfnHookEvent( kWebSpinnerShootEventName, EV_SpinWeb);
//	gEngfuncs.pfnHookEvent( kBabblerGunEventName, EV_Babbler);
	gEngfuncs.pfnHookEvent( kPrimalScreamShootEventName, EV_PrimalScream);
	gEngfuncs.pfnHookEvent( kStopPrimalScreamSoundEvent, EV_StopScream);
	
	gEngfuncs.pfnHookEvent( kJetpackEvent, EV_Jetpack );
	gEngfuncs.pfnHookEvent( kWelderEventName, EV_Welder );
	gEngfuncs.pfnHookEvent( kWelderStartEventName, EV_WelderStart );
	gEngfuncs.pfnHookEvent( kWelderEndEventName, EV_WelderEnd );
	gEngfuncs.pfnHookEvent( kWelderConstEventName, EV_WelderConst );
	//gEngfuncs.pfnHookEvent( kStartOverwatchEvent, EV_OverwatchStart );
	//gEngfuncs.pfnHookEvent( kEndOverwatchEvent, EV_OverwatchEnd );
	gEngfuncs.pfnHookEvent( kRegenerationEvent, EV_Regeneration );
	gEngfuncs.pfnHookEvent( kStartCloakEvent, EV_StartCloak );
	gEngfuncs.pfnHookEvent( kEndCloakEvent, EV_EndCloak );
	//gEngfuncs.pfnHookEvent( kWallJumpEvent, EV_WallJump );
	gEngfuncs.pfnHookEvent( kFlightEvent, EV_Flight );
	gEngfuncs.pfnHookEvent( kTeleportEvent, EV_Teleport );
	gEngfuncs.pfnHookEvent( kPhaseInEvent, EV_PhaseIn );
	gEngfuncs.pfnHookEvent( kSiegeHitEvent, EV_SiegeHit );
	gEngfuncs.pfnHookEvent( kSiegeViewHitEvent, EV_SiegeViewHit );
	gEngfuncs.pfnHookEvent( kCommanderPointsAwardedEvent, EV_CommandPoints );
	gEngfuncs.pfnHookEvent( kAlienSightOnEvent, EV_AlienSightOn);
	gEngfuncs.pfnHookEvent( kAlienSightOffEvent, EV_AlienSightOff);
//	gEngfuncs.pfnHookEvent( kParalysisShootEventName, EV_ParalysisGun);
//	gEngfuncs.pfnHookEvent( kParalysisStartEventName, EV_ParalysisStart);
	gEngfuncs.pfnHookEvent( kParasiteShootEventName, EV_ParasiteGun);
	gEngfuncs.pfnHookEvent( kBlinkEffectSuccessEventName, EV_BlinkSuccess);
	gEngfuncs.pfnHookEvent( kDivineWindShootEventName, EV_DivineWind);
	gEngfuncs.pfnHookEvent( kBileBombShootEventName, EV_BileBomb);
	gEngfuncs.pfnHookEvent( kAcidRocketShootEventName, EV_AcidRocket);
	gEngfuncs.pfnHookEvent( kStompShootEventName, EV_Stomp);
	gEngfuncs.pfnHookEvent( kDevourShootEventName, EV_Devour);
				
	gEngfuncs.pfnHookEvent( kLeapEventName, EV_Leap);
	gEngfuncs.pfnHookEvent( kChargeEventName, EV_Charge);
	gEngfuncs.pfnHookEvent( kAbilityEventName, EV_Ability);
	
	//gEngfuncs.pfnHookEvent( kHiveHitEvent, EV_HiveHit );
	gEngfuncs.pfnHookEvent( kEmptySoundEvent, EV_EmptySound );
	gEngfuncs.pfnHookEvent( kNumericalInfoEvent, EV_NumericalInfo );
	gEngfuncs.pfnHookEvent( kInvalidActionEvent, EV_InvalidAction);
	gEngfuncs.pfnHookEvent( kParticleEvent, EV_Particle);
	gEngfuncs.pfnHookEvent( kDistressBeaconEvent, EV_DistressBeacon);
	gEngfuncs.pfnHookEvent( kWeaponAnimationEvent, EV_WeaponAnimation);
	gEngfuncs.pfnHookEvent( kLevelUpEvent, EV_LevelUp);
}

#define LOUD_GUN_VOLUME			1000
#define NORMAL_GUN_VOLUME		600
#define QUIET_GUN_VOLUME		200

#define	BRIGHT_GUN_FLASH		512
#define NORMAL_GUN_FLASH		256
#define	DIM_GUN_FLASH			128

#define BIG_EXPLOSION_VOLUME	2048
#define NORMAL_EXPLOSION_VOLUME	1024
#define SMALL_EXPLOSION_VOLUME	512

#define	WEAPON_ACTIVITY_VOLUME	64

#define VECTOR_CONE_1DEGREES	Vector( 0.00873, 0.00873, 0.00873 )
#define VECTOR_CONE_2DEGREES	Vector( 0.01745, 0.01745, 0.01745 )
#define VECTOR_CONE_3DEGREES	Vector( 0.02618, 0.02618, 0.02618 )
#define VECTOR_CONE_4DEGREES	Vector( 0.03490, 0.03490, 0.03490 )
#define VECTOR_CONE_5DEGREES	Vector( 0.04362, 0.04362, 0.04362 )
#define VECTOR_CONE_6DEGREES	Vector( 0.05234, 0.05234, 0.05234 )
#define VECTOR_CONE_7DEGREES	Vector( 0.06105, 0.06105, 0.06105 )
#define VECTOR_CONE_8DEGREES	Vector( 0.06976, 0.06976, 0.06976 )
#define VECTOR_CONE_9DEGREES	Vector( 0.07846, 0.07846, 0.07846 )
#define VECTOR_CONE_10DEGREES	Vector( 0.08716, 0.08716, 0.08716 )
#define VECTOR_CONE_15DEGREES	Vector( 0.13053, 0.13053, 0.13053 )
#define VECTOR_CONE_20DEGREES	Vector( 0.17365, 0.17365, 0.17365 )

// <<< cgc >>>
// This is duplicated here, make sure it's up to date
//#define SND_CHANGE_PITCH	(1<<7)		// duplicated in protocol.h change sound pitch

int& GetUpgradeState(int inIndex)
{
	cl_entity_t* thePlayer = GetEntity(inIndex);
	return thePlayer->curstate.iuser4;
}

// From Counter-strike
#define MAX_DEAD_PLAYER_MODELS 64
TEMPENTITY* g_DeadPlayerModels[MAX_DEAD_PLAYER_MODELS];
#define MAX_BODY_TIME 5
#define FTENT_BODYTRACE			0x00100000
#define FTENT_BODYGRAVITY		0x00200000

void RemoveAllDecals()
{
	for ( int har = 0; har < (int)CVAR_GET_FLOAT( "r_decals" ); har++ )
		gEngfuncs.pEfxAPI->R_DecalRemoveAll ( har );

	//if ( g_pParticleMan )
	//	 g_pParticleMan->ResetParticles();

	if ( g_DeadPlayerModels == NULL )
		 return;

	// Is the dead player model list already clean?
	if ( g_DeadPlayerModels[0] == NULL )
		return;

	// Clear dead player model list
	for ( int i = 0; i < MAX_DEAD_PLAYER_MODELS; i++ )
	{
		if ( g_DeadPlayerModels[i] )
		{
			g_DeadPlayerModels[i]->die = 0;
			g_DeadPlayerModels[i] = 0;
		}
	}
}

// lowers body into ground
void RemoveBody(TEMPENTITY* te, float frametime, float current_time)
{
	if ( current_time >= te->entity.curstate.fuser2 + MAX_BODY_TIME + CVAR_GET_FLOAT( "cl_corpsestay" ) )
		 te->entity.origin[2] -= frametime * 5;
}

// Play a sound? Through up some smoke?
void HitBody( TEMPENTITY *ent, struct pmtrace_s *ptr )
{
	if ( ptr->plane.normal.z > 0 )
		ent->flags |= FTENT_BODYGRAVITY;
}

// From Counter-strike
void CreateCorpse ( Vector vOrigin, Vector vAngles, const char *pModel, float flAnimTime, int iSequence, int iBody )
{
	int framecount = 255;

	int iModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex ( pModel );
	TEMPENTITY *pBody = gEngfuncs.pEfxAPI->R_TempModel( vOrigin, Vector( 0, 0, 0 ), vAngles, 100, gEngfuncs.pEventAPI->EV_FindModelIndex ( pModel ), 0 );

	if ( pBody == NULL)
		return;

	pBody->flags |=  FTENT_COLLIDEWORLD | FTENT_CLIENTCUSTOM | FTENT_SPRANIMATE | FTENT_BODYTRACE | FTENT_PERSIST;
	pBody->frameMax = framecount;

	pBody->entity.curstate.animtime = flAnimTime;
	pBody->entity.curstate.framerate = 1;
	pBody->entity.curstate.sequence = iSequence;
	pBody->entity.curstate.frame = 0;
	pBody->entity.curstate.body = iBody;
	pBody->entity.curstate.renderamt = 255;
	pBody->entity.curstate.fuser1 = gHUD.m_flTime + 1;
	pBody->entity.curstate.fuser2 = gHUD.m_flTime + CVAR_GET_FLOAT ("cl_corpsestay");	// estimated end time of animation

	// let entity sink 5 seconds then kill it after cl_corpsestay is over
	pBody->die = gEngfuncs.GetClientTime() + pBody->entity.curstate.fuser2 + 5;
	pBody->callback = RemoveBody;
	pBody->hitcallback = HitBody;
	pBody->bounceFactor = 0;

	// Save body to list so we can remove after round ends
	for ( int i = 0; i < 64; i ++ )
	{
		if ( g_DeadPlayerModels[ i ] == NULL )
		{
			g_DeadPlayerModels[ i ] = pBody;
			break;
		}
	}
}

void PlayMeleeHitEffects(struct event_args_s* inArgs, int inRange, const string& inSoundName)
{
	int thePlayer = inArgs->entindex;

	vec3_t theForward, theRight, theUp;
	gEngfuncs.pfnAngleVectors(inArgs->angles, theForward, theRight, theUp);

	vec3_t theAimingDir;
	for(int i = 0; i < 3; i++)
	{
		theAimingDir[i] = theForward[i];// + theRight[i] + theUp[i];
	}
	VectorNormalize(theAimingDir);

	// Do a trace within certain range, ignoring ourselves
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );

	gEngfuncs.pEventAPI->EV_PushPMStates();
	
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(-1);

	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );

	Vector theStartPos;
	//VectorMA(inArgs->origin, 17, theAimingDir, theStartPos);
	VectorCopy(inArgs->origin, theStartPos);
	//gTriDebugLocations.push_back(DebugPoint(theStartPos[0], theStartPos[1], theStartPos[2]));
	//gTriDebugLocations.push_back(DebugPoint(theStartPos[0] + theAimingDir[0]*inRange, theStartPos[1]+ theAimingDir[1]*inRange, theStartPos[2] + theAimingDir[2]*inRange));
	
	Vector theEndPos;
	VectorMA(theStartPos, inRange, theAimingDir, theEndPos);
	//gTriDebugLocations.push_back(DebugPoint(theEndPos[0], theEndPos[1], theEndPos[2]));
	
	pmtrace_t tr;
	bool theDone = false;
	int theNumIterations = 0;

	do
	{
		gEngfuncs.pEventAPI->EV_PlayerTrace(theStartPos, theEndPos, PM_NORMAL, thePlayer, &tr);
	
		// Did we hit something marked as enemy?
		int theHit = gEngfuncs.pEventAPI->EV_IndexFromTrace(&tr);
		if(theHit == thePlayer)
		{
			// tr.endpos: -71.1894, 3274.23, -527.888
			// theStartPos: -71.20, 3274.02, -517.89
			// theAimingDir: -0.01169, -0.02157, .99969
			// Infinite loop: this next line doesn't changea anything
			VectorMA(tr.endpos, kHitOffsetAmount, theAimingDir, theStartPos);
		}
		else if(tr.fraction < 1.0f)
		{
			Vector theHitPos = tr.endpos;
			
			// Play sound
			gEngfuncs.pEventAPI->EV_PlaySound(thePlayer, theHitPos, CHAN_AUTO, inSoundName.c_str(), 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

			// Play generic damage effect
			//AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsMeleeDamage, theHitPos, &(tr.plane.normal));
			
			theDone = true;
		}
		else
		{
			theDone = true;
		}

		// Prevent infinite loop
		theNumIterations++;

		if(theNumIterations > 20)
		{
			theDone = true;
		}

	} while(!theDone);

	gEngfuncs.pEventAPI->EV_PopPMStates();
}

//
// Define them
//
void EV_Knife(struct event_args_s* inArgs)
{
	int idx = inArgs->entindex;
	
	// Play attack sound
	char* theSoundToPlay = "";
	
	int theRandomSound = gEngfuncs.pfnRandomLong(0, 1);
	switch(theRandomSound)
	{
	case 0:
		theSoundToPlay = kKNFireSound1;
		break;
	case 1:
		theSoundToPlay = kKNFireSound2;
		break;
	}
	gEngfuncs.pEventAPI->EV_PlaySound(idx, inArgs->origin, CHAN_WEAPON, theSoundToPlay, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

	// TODO: Now do a trace line and if we hit a wall, play the wall sound, if we hit a person, play a hit-flesh sound
	// TODO: Add sparks where it hit
	
	// General x-punch axis
	if (EV_IsLocal(idx))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
		/*
		float theHalfSpread = kKNXPunch/2.0f;
		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis(0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
			V_PunchAxis(1, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}
		*/
	}
}

//void EV_Grenade(struct event_args_s* inArgs)
//{
//	int idx = inArgs->entindex;
//	vec3_t up, right, forward;
//	gEngfuncs.pfnAngleVectors(inArgs->angles, forward, right, up);
//	
//	// Play attack sound
//	char* theSoundToPlay = kKNFireSound1;
//	gEngfuncs.pEventAPI->EV_PlaySound(idx, inArgs->origin, CHAN_WEAPON, theSoundToPlay, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
//
//	// Create a temporary entity that bounces and dies after a certain amount of time
//	int theModelIndex;
//	struct model_s* theModel = gEngfuncs.CL_LoadModel("models/w_grenade.mdl", &theModelIndex);
//	ASSERT(theModel);
//	TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->CL_TempEntAlloc(gPredictedPlayerOrigin, theModel);
//	ASSERT(theTempEntity);
//	if(theTempEntity)
//	{
//		vec3_t theStartPos, theEndPos, vecSrc;
//		EV_GetGunPosition(inArgs, vecSrc, inArgs->origin);
//		VectorMA(vecSrc, kGBarrelLength, forward, theStartPos);
//		
//		VectorCopy(theStartPos, theTempEntity->entity.origin);
//		VectorCopy(theStartPos, theTempEntity->entity.prevstate.origin);
//		VectorCopy(theStartPos, theTempEntity->entity.curstate.origin);
//		theTempEntity->die += kGrenDetonateTime;
//		//theTempEntity->hitcallback = SpitHit;
//		theTempEntity->flags |= (FTENT_COLLIDEALL | FTENT_PERSIST);
//
//		vec3_t theSourceVelocity;
//		VectorCopy(inArgs->velocity, theSourceVelocity);
//
//		vec3_t theVelocity;
//		ComputeGrenadeVelocity(forward, theSourceVelocity, theVelocity);
//
//		VectorCopy(theVelocity, theTempEntity->entity.baseline.origin);
//		VectorCopy(theVelocity, theTempEntity->entity.baseline.velocity);
//	}
//
//	// General x-punch axis
////	if (EV_IsLocal(idx))
////	{
////		gEngfuncs.pEventAPI->EV_WeaponAnimation(ANIM_FIRE1 + gEngfuncs.pfnRandomLong(0,2), 2);
////	}
//}

void EV_MachineGun(struct event_args_s* args)
{
    // What to do about this static member?
    static int tracerCount[ 32 ];
	
    // Figure out which weapon description to assocate this weapon with
    int theWeaponIndex = args->iparam1;
	
	// Play attack animation and add muzzle flash
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
	
	//AngleVectors( angles, forward, right, up );
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	// Get upgrade
	int theTracerFreq;
	int theUpgradeLevel = AvHPlayerUpgrade::GetWeaponUpgrade(AVH_USER3_MARINE_PLAYER, GetUpgradeState(idx), NULL, &theTracerFreq);

	// Vary flange effect more with higher upgrade
	int theUpperBound = theUpgradeLevel*10;
	int thePitch = 100 + (gEngfuncs.pfnRandomLong(0, theUpperBound) - theUpperBound);

	if ( EV_IsLocal( idx ) )
	{
		// Add muzzle flash to current weapon model
		if(theUpgradeLevel > 0)
		{
			EV_MuzzleFlash();
		}
		
		gEngfuncs.pEventAPI->EV_WeaponAnimation(args->iparam2, 2);
	}
	
	// General ejecting ammo if any
	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex(kMGEjectModel);
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	
	EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20, -12, 4 );
	
	// Only eject brass when upgraded
	//if(theUpgradeLevel > 0)
	//{
	//	VectorScale(ShellVelocity, theUpgradeLevel, ShellVelocity);
		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL); 
	//}
	
	// Play one of basic attack sounds
	float theVolume = args->fparam1;
	float theAttenuation = .8f + .3*theUpgradeLevel;
	
	char* theSoundToPlay = kMGFireSound1;

	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, theSoundToPlay, theVolume, theAttenuation, 0, thePitch);
	
	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	//EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, VECTOR_CONE_6DEGREES, kMGRange, BULLET_PLAYER_MP5, 2, &tracerCount[idx-1] );
	EV_HLDM_FireBulletsPlayer( idx, forward, right, up, 1, vecSrc, vecAiming, kMGRange, BULLET_PLAYER_MP5, theTracerFreq, &tracerCount[idx-1], kMGSpread, args->iparam1);
	
	// General x-punch axis
	if ( EV_IsLocal( idx ) )
	{
		
		// Multiply punch by upgrade level
		//float theHalfSpread = (kMGXPunch/4.0f)*(theUpgradeLevel+1);

		// Changed to ignore upgrade level
		float theHalfSpread = (kMGXPunch/4.0f);
		
		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis( 0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}
	}
}


void EV_Pistol(struct event_args_s* args)
{
    // What to do about this static member?
    static int tracerCount[ 32 ];
	
    // Figure out which weapon description to assocate this weapon with
    int theWeaponIndex = args->iparam1;
	
	// Play attack animation and add muzzle flash
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
	
	//AngleVectors( angles, forward, right, up );
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	
	// Get upgrade
	int theTracerFreq;
	int theUpgradeLevel = AvHPlayerUpgrade::GetWeaponUpgrade(AVH_USER3_MARINE_PLAYER, GetUpgradeState(idx), NULL, &theTracerFreq);
	
	// Vary flange effect more with higher upgrade
	int theUpperBound = theUpgradeLevel*10;
	int thePitch = 100 + (gEngfuncs.pfnRandomLong(0, theUpperBound) - theUpperBound);
	
	if ( EV_IsLocal( idx ) )
	{
		// Add muzzle flash to current weapon model
		if(theUpgradeLevel > 0)
		{
			EV_MuzzleFlash();
		}
		
		gEngfuncs.pEventAPI->EV_WeaponAnimation(args->iparam2, 2);
	}
	
	// General ejecting ammo if any
	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex(kHGEjectModel);
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	
	EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20, -12, 4 );
	
	// Only eject brass when upgraded
	//if(theUpgradeLevel > 0)
	//{
	//	VectorScale(ShellVelocity, theUpgradeLevel, ShellVelocity);
	EV_EjectBrass(ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL); 
	//}
	
	// Play one of basic attack sounds
	float theVolume = args->fparam1;
	float theAttenuation = .8f + .3*theUpgradeLevel;
	
	char* theSoundToPlay = kHGFireSound1;
	
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, theSoundToPlay, theVolume, theAttenuation, 0, thePitch);
	
	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );
	
	//EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, VECTOR_CONE_6DEGREES, kHGRange, BULLET_PLAYER_MP5, 2, &tracerCount[idx-1] );
	EV_HLDM_FireBulletsPlayer( idx, forward, right, up, 1, vecSrc, vecAiming, kHGRange, BULLET_PLAYER_357, theTracerFreq, &tracerCount[idx-1], kHGSpread, args->iparam1);
	
	// General x-punch axis
	if ( EV_IsLocal( idx ) )
	{
		// Multiply punch by upgrade level
		//float theHalfSpread = (kHGXPunch/3.0f)*(theUpgradeLevel+1);

		// Changed to ignore upgrade level
		float theHalfSpread = (kHGXPunch/3.0f);

		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis( 0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}
	}
}

void EV_SonicGun(struct event_args_s* args)
{
    // What to do about this static member?
    static int tracerCount[ 32 ];
	
    // Figure out which weapon description to assocate this weapon with
    int theWeaponIndex = args->iparam1;
	
	// Play attack animation and add muzzle flash
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
	
	//AngleVectors( angles, forward, right, up );
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	
	// Get upgrade
	int theUpgradeLevel = AvHPlayerUpgrade::GetWeaponUpgrade(AVH_USER3_MARINE_PLAYER, GetUpgradeState(idx));
	
	if ( EV_IsLocal( idx ) )
	{
		// Add muzzle flash to current weapon model
		if(theUpgradeLevel > 0)
		{
			EV_MuzzleFlash();
		}

		gEngfuncs.pEventAPI->EV_WeaponAnimation(args->iparam2, 2);
	}
	
	// General ejecting ammo if any
	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex(kSGEjectModel);
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	
	EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20, -12, 4 );
	
	//if(theUpgradeLevel > 0)
	//{
	//	VectorScale(ShellVelocity, theUpgradeLevel, ShellVelocity);
		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL); 
	//}

	float theAttenuation = .8f + .2*theUpgradeLevel;
	//float theVolume = min(.85f + .05*theUpgradeLevel, 1.0f);
	float theVolume = args->fparam1;
	
	// TODO: Synch up with propagated random seed?  
	// Play one of basic attack sounds
	//if(gEngfuncs.pfnRandomLong(0, 1) == 0)
	char* theSoundToPlay = kSGFireSound1;
	
	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, theSoundToPlay, theVolume, theAttenuation, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );
	
	//if ( gEngfuncs.GetMaxClients() > 1 )
	//{
		vec3_t theBarrelOffset, theBarrelTip;
		VectorScale(forward, 30, theBarrelOffset);
		VectorAdd(vecSrc, theBarrelOffset, theBarrelTip);

//		if(theUpgradeLevel > 0)
//		{
//			AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsShotgun, theBarrelTip);
//		}
		//EV_HLDM_FireBullets( idx, forward, right, up, kSGBulletsPerShot, vecSrc, vecAiming, kSGRange, BULLET_PLAYER_BUCKSHOT, 0, &tracerCount[idx-1], VECTOR_CONE_20DEGREES.x, VECTOR_CONE_20DEGREES.y);
		EV_HLDM_FireBulletsPlayer( idx, forward, right, up, BALANCE_VAR(kSGBulletsPerShot), vecSrc, vecAiming, kSGRange, BULLET_PLAYER_BUCKSHOT, 0, &tracerCount[idx-1], kSGSpread, args->iparam1);
		//}
	
	// General x-punch axis
	if ( EV_IsLocal( idx ) )
	{
		//float theHalfSpread = (kSGXPunch/3.0f)*(theUpgradeLevel+1);

		// Changed to ignore upgrade level
		float theHalfSpread = (kSGXPunch/3.0f);


		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis( 0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}
	}
}

void EV_HeavyMachineGun(struct event_args_s* args)
{
    // What to do about this static member?
    static int tracerCount[ 32 ];
	
    // Figure out which weapon description to assocate this weapon with
    int theWeaponIndex = args->iparam1;
	
	// Play attack animation and add muzzle flash
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = args->entindex;
	VectorCopy( args->origin, origin );
	VectorCopy( args->angles, angles );
	VectorCopy( args->velocity, velocity );
	
	//AngleVectors( angles, forward, right, up );
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	int theTracerLevel;
	int theUpgradeLevel = AvHPlayerUpgrade::GetWeaponUpgrade(AVH_USER3_MARINE_PLAYER, GetUpgradeState(idx), NULL, &theTracerLevel);
	
	if ( EV_IsLocal( idx ) )
	{
		if(theUpgradeLevel > 0)
		{
			// Add muzzle flash to current weapon model
			EV_MuzzleFlash();
		}

		gEngfuncs.pEventAPI->EV_WeaponAnimation(args->iparam2, 2);
	}
	
	// General ejecting ammo if any
	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex(kHMGEjectModel);
	vec3_t ShellVelocity;
	vec3_t ShellOrigin;
	
	EV_GetDefaultShellInfo(args, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20, -12, 4 );
	
	//if(theUpgradeLevel > 0)
	//{
	//	VectorScale(ShellVelocity, theUpgradeLevel, ShellVelocity);
		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL); 
	//}
	
	// TODO: Synch up with propagated random seed?  
	// Play one of basic attack sounds
	int theHighIndex = theUpgradeLevel;
	//int theRandomLong = gEngfuncs.pfnRandomLong(0, theHighIndex);
	//float theVolume = min(.6 + .2*theUpgradeLevel, 1.0f);
	float theVolume = args->fparam1;
	float theAttenuation = .8f + .2*theUpgradeLevel;
	int thePitch = 100;
	if(theUpgradeLevel > 0)
	{
		int theVariance = theUpgradeLevel*6;
		thePitch += (gEngfuncs.pfnRandomLong( 0, theVariance))/(theVariance/2);
	}
	
	char* theSoundToPlay = kHMGFireSound1;
	
	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, theSoundToPlay, theVolume, theAttenuation, 0, thePitch);
	
	EV_GetGunPosition( args, vecSrc, origin );
	VectorCopy( forward, vecAiming );
	
	//EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, kHMGRange, BULLET_PLAYER_357, theTracerLevel, &tracerCount[idx-1], theSpreadX, theSpreadY);
	EV_HLDM_FireBulletsPlayer( idx, forward, right, up, 1, vecSrc, vecAiming, kHMGRange, BULLET_PLAYER_MP5, theTracerLevel, &tracerCount[idx-1], kHMGSpread, args->iparam1);
	
//	if(theUpgradeLevel > 1)
//	{
//		if(gEngfuncs.pfnRandomLong(0, 1) == 0)
//		{
//			vec3_t theBarrelOffset, theBarrelTip;
//			VectorScale(forward, 100, theBarrelOffset);
//			VectorAdd(vecSrc, theBarrelOffset, theBarrelTip);
//			
//			AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsHeavyMGSmoke, vecSrc);
//		}
//	}
	
	// General x-punch axis
	if ( EV_IsLocal( idx ) )
	{
		//float theHalfSpread = (kHMGXPunch/4.0f)*(theUpgradeLevel+1);
		
		// Changed to ignore upgrade level
		float theHalfSpread = (kHMGXPunch/4.0f);

		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis( 0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}
	}
}

void GrenadeHit(struct tempent_s* ent, struct pmtrace_s* ptr)
{
	char* theSoundToPlay = kGRHitSound;

	gEngfuncs.pEventAPI->EV_PlaySound(ent->entity.index, ptr->endpos, CHAN_AUTO, theSoundToPlay, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	//ASSERT(ptr);
	//ASSERT(ent);
	//ent->die = gEngfuncs.GetClientTime();
}

void EV_GrenadeGun(struct event_args_s* inArgs)
{
    // What to do about this static member?
    static int tracerCount[ 32 ];
	
    // Figure out which weapon description to assocate this weapon with
    int theWeaponIndex = inArgs->iparam1;
	
	// Play attack animation and add muzzle flash
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = inArgs->entindex;
	VectorCopy( inArgs->origin, origin );
	VectorCopy( inArgs->angles, angles );
	VectorCopy( inArgs->velocity, velocity );
	
	//AngleVectors( angles, forward, right, up );
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	int theUpgradeLevel = AvHPlayerUpgrade::GetWeaponUpgrade(AVH_USER3_MARINE_PLAYER, GetUpgradeState(idx));
	
	if ( EV_IsLocal( idx ) )
	{
		// Add muzzle flash to current weapon model
		if(theUpgradeLevel > 0)
		{
			EV_MuzzleFlash();
		}
		
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
	
//	// General ejecting ammo if any
//	int shell = gEngfuncs.pEventAPI->EV_FindModelIndex(kGGEjectModel);
//	vec3_t ShellVelocity;
//	vec3_t ShellOrigin;
//	
//	EV_GetDefaultShellInfo(inArgs, origin, velocity, ShellVelocity, ShellOrigin, forward, right, up, 20, -12, 4 );
//	
//	//if(theUpgradeLevel > 0)
//	//{
//	//	VectorScale(ShellVelocity, theUpgradeLevel, ShellVelocity);
//		EV_EjectBrass(ShellOrigin, ShellVelocity, angles[ YAW ], shell, TE_BOUNCE_SHELL); 
//	//}
	
	char* theSoundToPlay = kGGFireSound1;
	
	gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, theSoundToPlay, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
	EV_GetGunPosition( inArgs, vecSrc, origin );
	VectorCopy( forward, vecAiming );
	
	// General x-punch axis
	if ( EV_IsLocal( idx ) )
	{
		//float theHalfSpread = (kGGXPunch/2.0f)*(theUpgradeLevel+1);
		
		// Changed to ignore upgrade level
		float theHalfSpread = (kGGXPunch/2.0f);

		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis( 0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}
	}

	// Create temp entity for grenade (assumes grenade is created at inArgs->origin, with velocity inArgs->angles)
//	ASSERT(inArgs);
//
//	Vector theStartPos;
//	VectorCopy(inArgs->origin, theStartPos);
//
//	// Fire grenade
//	int theModelIndex;
//	struct model_s* theModel = gEngfuncs.CL_LoadModel(kGGAmmoModel, &theModelIndex);
//	if(theModel)
//	{
//		TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->CL_TempEntAlloc(gPredictedPlayerOrigin, theModel);
//		if(theTempEntity)
//		{
//			VectorCopy(theStartPos, theTempEntity->entity.origin);
//			VectorCopy(theStartPos, theTempEntity->entity.prevstate.origin);
//			VectorCopy(theStartPos, theTempEntity->entity.curstate.origin);
//			theTempEntity->die += kSpitLifetime;
//			theTempEntity->hitcallback = GrenadeHit;
//			theTempEntity->flags |= (FTENT_COLLIDEALL | FTENT_PERSIST | FTENT_GRAVITY| FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP);
//			theTempEntity->entity.curstate.framerate = 30;
//			theTempEntity->frameMax = 4;//theModel->numframes;
//		
//			// Read origin as origin, and angles as velocity (from AvHAlienTurret::Shoot())
//			Vector theStartVelocity;
//			VectorCopy(inArgs->angles, theStartVelocity);
//			
//			// Temp entities interpret baseline origin as velocity.
//			VectorCopy(theStartVelocity, theTempEntity->entity.baseline.origin);
//			VectorCopy(theStartVelocity, theTempEntity->entity.baseline.velocity);
//		}
//	}
}

void EV_Grenade(struct event_args_s* inArgs)
{
    // What to do about this static member?
    static int tracerCount[ 32 ];
	
    // Figure out which weapon description to assocate this weapon with
    int theWeaponIndex = inArgs->iparam1;
	
	// Play attack animation and add muzzle flash
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = inArgs->entindex;
	VectorCopy( inArgs->origin, origin );
	VectorCopy( inArgs->angles, angles );
	VectorCopy( inArgs->velocity, velocity );
	
	//AngleVectors( angles, forward, right, up );
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);

	int theUpgradeLevel = AvHPlayerUpgrade::GetWeaponUpgrade(AVH_USER3_MARINE_PLAYER, GetUpgradeState(idx));
	
	if ( EV_IsLocal( idx ) )
	{

		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
	
	char* theSoundToPlay = NULL;

	switch(inArgs->iparam2)
	{
	case 4:
	case 7:
		theSoundToPlay = kGRFireSound1;
		break;
	case 3:
	case 6:
		theSoundToPlay = kGRPrimeSound;
		break;
	}

	if(theSoundToPlay)
		gEngfuncs.pEventAPI->EV_PlaySound( idx, origin, CHAN_WEAPON, theSoundToPlay, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

//	EV_GetGunPosition( inArgs, vecSrc, origin );
//	VectorCopy( forward, vecAiming );
//
//    // Create grenade temp entity
//    int theModelIndex;
//    struct model_s* theModel = gEngfuncs.CL_LoadModel(kGRWModel, &theModelIndex);
//    if(theModel)
//    {
//        TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->CL_TempEntAlloc(gPredictedPlayerOrigin, theModel);
//        if(theTempEntity)
//        {
//            vec3_t theStartPos, theEndPos;
//            EV_GetGunPosition(inArgs, vecSrc, inArgs->origin);
//            VectorMA(vecSrc, kGRBarrelLength, forward, theStartPos);
//            
//            VectorCopy(theStartPos, theTempEntity->entity.origin);
//            VectorCopy(theStartPos, theTempEntity->entity.prevstate.origin);
//            VectorCopy(theStartPos, theTempEntity->entity.curstate.origin);
//            theTempEntity->die += BALANCE_VAR(kGrenDetonateTime);
//            theTempEntity->hitcallback = GrenadeHit;
//            theTempEntity->flags |= (FTENT_COLLIDEALL | FTENT_GRAVITY | FTENT_ROTATE | FTENT_PERSIST);
//            theTempEntity->clientIndex = inArgs->entindex;	// Entity to ignore collisions with
//
//            theTempEntity->entity.baseline.angles[0] = rand()%360;
//
//            //theTempEntity->entity.curstate.framerate = 30;
//            //theTempEntity->frameMax = 4;//theModel->numframes;
//            
//            // Temp entities interpret baseline origin as velocity.
//            Vector theBaseVelocity;
//            VectorScale(inArgs->velocity, kGrenadeParentVelocityScalar, theBaseVelocity);
//            
//            Vector theStartVelocity;
//            VectorMA(theBaseVelocity, kGrenadeVelocity, forward, theStartVelocity);
//            
//            VectorCopy(theStartVelocity, theTempEntity->entity.baseline.origin);
//            VectorCopy(theStartVelocity, theTempEntity->entity.baseline.velocity);
//        }
//    }
}

char *EV_HLDM_DamageDecal( physent_t *pe );

void CreateDecal(struct pmtrace_s* inTrace)
{
	physent_t *pe;
	pe = gEngfuncs.pEventAPI->EV_GetPhysent( inTrace->ent );
	if(pe)
	{
		char* inDecalName = EV_HLDM_DamageDecal( pe );
		
		// Only decal brush models such as the world etc.
		if (  inDecalName && inDecalName[0] && pe && ( pe->solid == SOLID_BSP || pe->movetype == MOVETYPE_PUSHSTEP ) )
		{
			if ( CVAR_GET_FLOAT( "r_decals" ) )
			{
				gEngfuncs.pEfxAPI->R_DecalShoot( 
					gEngfuncs.pEfxAPI->Draw_DecalIndex( gEngfuncs.pEfxAPI->Draw_DecalIndexFromName( inDecalName) ), 
					gEngfuncs.pEventAPI->EV_IndexFromTrace( inTrace ), 0, inTrace->endpos, 0 );
			}
		}
	}
}

void SpitHit(struct tempent_s* ent, struct pmtrace_s* ptr)
{
	char* theSoundToPlay = kSpitHitSound1;
	if(gEngfuncs.pfnRandomLong(0, 1) == 0)
	{
		theSoundToPlay = kSpitHitSound2;
	}

	gEngfuncs.pEventAPI->EV_PlaySound(ent->entity.index, ptr->endpos, CHAN_AUTO, theSoundToPlay, .6f, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	if(ptr && ent)
	{
		ent->die = gEngfuncs.GetClientTime();
		
		AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsSpitHit, ptr->endpos, &(ptr->plane.normal));
		
		// Create splat here too
		CreateDecal(ptr);
	}
}

void EV_SpitGun(struct event_args_s* inArgs)
{
	ASSERT(inArgs);

    // Figure out which weapon description to assocate this weapon with
    int theWeaponIndex = inArgs->iparam1;
	
	// Play attack animation and add muzzle flash
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = inArgs->entindex;
	VectorCopy( inArgs->origin, origin );
	VectorCopy( inArgs->angles, angles );
	VectorCopy( inArgs->velocity, velocity );
	
	//AngleVectors( angles, forward, right, up );
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	
	// Play attack sound
	char* theSoundToPlay = kSpitGFireSound1;
	if(gEngfuncs.pfnRandomLong(0, 1) == 0)
	{
		theSoundToPlay = kSpitGFireSound2;
	}
	
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, theSoundToPlay, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
  
	// Fire spit glob
	int theModelIndex;
	struct model_s* theModel = gEngfuncs.CL_LoadModel(kSpitGunSprite, &theModelIndex);
	if(theModel)
	{
		TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->CL_TempEntAlloc(gPredictedPlayerOrigin, theModel);
		if(theTempEntity)
		{
			vec3_t theStartPos, theEndPos;
			EV_GetGunPosition(inArgs, vecSrc, inArgs->origin);
			VectorMA(vecSrc, kSpitGBarrelLength, forward, theStartPos);
			
			// Create tiny spittle where it leaves
			AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsSpitShoot, theStartPos);
			
			VectorCopy(theStartPos, theTempEntity->entity.origin);
			VectorCopy(theStartPos, theTempEntity->entity.prevstate.origin);
			VectorCopy(theStartPos, theTempEntity->entity.curstate.origin);
			theTempEntity->die += kSpitLifetime;
			theTempEntity->hitcallback = SpitHit;
			theTempEntity->flags |= (FTENT_COLLIDEALL | FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP/* | FTENT_PERSIST*/);
			theTempEntity->clientIndex = inArgs->entindex;	// Entity to ignore collisions with
			theTempEntity->entity.curstate.framerate = 30;
			theTempEntity->frameMax = 4;//theModel->numframes;
			
			// Temp entities interpret baseline origin as velocity.
			Vector theBaseVelocity;
			VectorScale(inArgs->velocity, kSpitParentVelocityScalar, theBaseVelocity);
			
			Vector theStartVelocity;
			VectorMA(theBaseVelocity, kSpitVelocity, forward, theStartVelocity);
			
			VectorCopy(theStartVelocity, theTempEntity->entity.baseline.origin);
			VectorCopy(theStartVelocity, theTempEntity->entity.baseline.velocity);
		}
	}
	
	// General x-punch axis
	if ( EV_IsLocal( idx ) )
	{
		float theHalfSpread = kSpitGXPunch/2.0f;
		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis( 0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}
		
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
}

void SpikeHit(struct tempent_s* ent, struct pmtrace_s* ptr)
{
	//EV_HLDM_DecalGunshot( &tr, iBulletType );
	
	//int theSprite = gEngfuncs.pEventAPI->EV_FindModelIndex(kSpikeGunHitSprite);
	//TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->R_TempSprite(ptr->endpos, vec3_origin, .6f, theSprite, kRenderTransAdd, kRenderFxNoDissipation, .5f, .4f, FTENT_COLLIDEALL | FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP | FTENT_PERSIST);
	//if(theTempEntity)
	//{
	//	theTempEntity->entity.curstate.framerate = 30;
	//}
	
	// Play spike ricochet sounds
	char* theRandomSound = NULL;
	
	switch (gEngfuncs.pfnRandomLong(0, 2))
	{
	case 0:
		theRandomSound = "weapons/a_ric1.wav";
		break;
	case 1:
		theRandomSound = "weapons/a_ric2.wav";
		break;
	case 2:
		theRandomSound = "weapons/a_ric3.wav";
		break;
	}
	
	const float kAlienRicochetVolume = .25f;
	ASSERT(theRandomSound);
	gEngfuncs.pEventAPI->EV_PlaySound( 0, ptr->endpos, CHAN_STATIC, theRandomSound, kAlienRicochetVolume, ATTN_NORM, 0, 96 + gEngfuncs.pfnRandomLong(0,0xf) );
	
	// Create spike hit particle system
	AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsSpikeHitEffect, ptr->endpos);

	// Kill off temp ent
	ent->die = -1;
}

void EV_OffenseChamber(struct event_args_s* inArgs)
{
    //static int theTracerCount[ 32 ];
	
	// Play attack animation and add muzzle flash
	int idx;
	vec3_t origin;
	vec3_t velocity;
	
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = inArgs->entindex;
	VectorCopy( inArgs->origin, origin );
	VectorCopy( inArgs->velocity, velocity );
	
	// Play one of basic attack sounds
	float theVolume = inArgs->fparam1;
	float theAttenuation = 1.2f;
	
	char* theSoundToPlay = kAlienTurretFire1;
	
	int theUpperBound = 30;
	int thePitch = 100 + (gEngfuncs.pfnRandomLong(0, theUpperBound) - theUpperBound/2);
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, theSoundToPlay, theVolume, theAttenuation, 0, thePitch);
	
	// Read origin as origin, and angles as direction (from AvHAlienTurret::Shoot())
	Vector theStartPos;
	VectorCopy(inArgs->origin, theStartPos);

	Vector theNetworkDirToEnemy;
	VectorCopy(inArgs->angles, theNetworkDirToEnemy);

	// Take into account network precision
	Vector theDirToEnemy;
	VectorScale(theNetworkDirToEnemy, 1/100.0f, theDirToEnemy);

	Vector theDirToEnemyAngles;
	VectorAngles(theDirToEnemy, theDirToEnemyAngles);

	Vector theInitialVelocity;
	VectorScale(theDirToEnemy, kOffenseChamberSpikeVelocity, theInitialVelocity);
	
	// Create spike projectile
	TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->R_TempModel(theStartPos, theDirToEnemy, theDirToEnemyAngles, 100, gEngfuncs.pEventAPI->EV_FindModelIndex(kSpikeProjectileModel), 0);
	if(theTempEntity)
	{
		//vec3_t theStartPos, theEndPos;
		//EV_GetGunPosition(inArgs, vecSrc, inArgs->origin);
		//VectorMA(vecSrc, kSpikeBarrelLength, forward, theStartPos);
		
		VectorCopy(theStartPos, theTempEntity->entity.origin);
		VectorCopy(theStartPos, theTempEntity->entity.prevstate.origin);
		VectorCopy(theStartPos, theTempEntity->entity.curstate.origin);
		theTempEntity->hitcallback = SpikeHit;
		theTempEntity->flags = (FTENT_COLLIDEALL | FTENT_PERSIST | FTENT_COLLIDEKILL);
		theTempEntity->die += kSpikeLifetime;
		theTempEntity->clientIndex = inArgs->iparam1;	// Entity to ignore collisions with

		//theTempEntity->entity.curstate.framerate = 30;
		//theTempEntity->frameMax = 4;//theModel->numframes;
		
		// Temp entities interpret baseline origin as velocity.
		VectorCopy(theInitialVelocity, theTempEntity->entity.baseline.origin);
		VectorCopy(theInitialVelocity, theTempEntity->entity.baseline.velocity);
		
		// Set orientation
		//VectorCopy(inArgs->angles, theTempEntity->entity.angles);
	}
	
//
//
//	ASSERT(inArgs);
//	
//	// Play attack animation and add muzzle flash
//	int idx = inArgs->entindex;
//	
//	Vector theStartPos;
//	VectorCopy(inArgs->origin, theStartPos);
//	
//	// Play attack sound
//	gEngfuncs.pEventAPI->EV_PlaySound(idx, theStartPos, CHAN_WEAPON, kAlienTurretFire1, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
//	
//	// Fire spit glob
//	int theModelIndex;
//	struct model_s* theModel = gEngfuncs.CL_LoadModel(kAlienTurretSprite, &theModelIndex);
//	if(theModel)
//	{
//		TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->CL_TempEntAlloc(gPredictedPlayerOrigin, theModel);
//		if(theTempEntity)
//		{
//			// Create tiny spittle where it leaves
//			AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsSpitShoot, theStartPos);
//			
//			VectorCopy(theStartPos, theTempEntity->entity.origin);
//			VectorCopy(theStartPos, theTempEntity->entity.prevstate.origin);
//			VectorCopy(theStartPos, theTempEntity->entity.curstate.origin);
//			theTempEntity->die += kSpitLifetime;
//			theTempEntity->hitcallback = SpitHit;
//			theTempEntity->flags |= (FTENT_COLLIDEALL | FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP/* | FTENT_PERSIST*/);
//			theTempEntity->entity.curstate.framerate = 30;
//			theTempEntity->frameMax = 4;//theModel->numframes;
//			
//			// Read origin as origin, and angles as direction (from AvHAlienTurret::Shoot())
//			Vector theDirToEnemy;
//			VectorCopy(inArgs->angles, theDirToEnemy);
//			
//			Vector theStartVelocity;
//			VectorScale(theDirToEnemy, kAlienTurretProjectileVelocity, theStartVelocity);
//			
//			// Temp entities interpret baseline origin as velocity.
//			VectorCopy(theStartVelocity, theTempEntity->entity.baseline.origin);
//			VectorCopy(theStartVelocity, theTempEntity->entity.baseline.velocity);
//		}
//	}
}

void EV_Claws(struct event_args_s* inArgs)
{
	int idx = inArgs->entindex;
	
	// Play attack sound
	char* theSoundToPlay = "";
	
	int theRandomSound = gEngfuncs.pfnRandomLong(0, 2);
	switch(theRandomSound)
	{
	case 0:
		theSoundToPlay = kClawsSound1;
		break;
	case 1:
		theSoundToPlay = kClawsSound2;
		break;
	case 2:
		theSoundToPlay = kClawsSound3;
		break;
	}

	const int kBasePitch = 94;
	const int kVariablePitchRange = 0xF;

	int theRandomPitch = gEngfuncs.pfnRandomLong( 0, kVariablePitchRange );
	int thePitch = kBasePitch + (inArgs->fparam1*kClawsAdrenPitchFactor) + theRandomPitch;
	gEngfuncs.pEventAPI->EV_PlaySound(idx, inArgs->origin, CHAN_WEAPON, theSoundToPlay, inArgs->fparam1, ATTN_NORM, 0, thePitch);
	
	// General x-punch axis
	if (EV_IsLocal(idx))
	{
	
		/*
		float theHalfSpread = (inArgs->fparam1*kClawsPunch)/2.0f;
		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis(0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
			V_PunchAxis(1, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}
		*/

		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}

	int theRandomLong = gEngfuncs.pfnRandomLong(0, 1);
	if(theRandomLong == 0)
	{
		PlayMeleeHitEffects(inArgs, kClawsRange, kClawsHitSound1);
	}
	else
	{
		PlayMeleeHitEffects(inArgs, kClawsRange, kClawsHitSound2);
	}
}


void EV_Swipe(struct event_args_s* inArgs)
{
	int idx = inArgs->entindex;
	
	// Play attack sound
	char* theSoundToPlay = "";
	
	int theRandomSound = gEngfuncs.pfnRandomLong(0, 3);
	switch(theRandomSound)
	{
	case 0:
		theSoundToPlay = kSwipeSound1;
		break;
	case 1:
		theSoundToPlay = kSwipeSound2;
		break;
	case 2:
		theSoundToPlay = kSwipeSound3;
		break;
	case 3:
		theSoundToPlay = kSwipeSound4;
		break;
	}
	gEngfuncs.pEventAPI->EV_PlaySound(idx, inArgs->origin, CHAN_WEAPON, theSoundToPlay, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
	// General x-punch axis
	if (EV_IsLocal(idx))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
		
		/*
		float theHalfSpread = kSwipePunch/2.0f;
		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis(0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
			V_PunchAxis(1, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}
		*/
	}

	if(gEngfuncs.pfnRandomLong(0, 1) == 0)
	{
		PlayMeleeHitEffects(inArgs, kSwipeRange, kSwipeHitSound1);
	}
	else
	{
		PlayMeleeHitEffects(inArgs, kSwipeRange, kSwipeHitSound2);
	}
}

//void EnsnareHit(struct tempent_s* ent, struct pmtrace_s* ptr)
//{
//	ent->die = gEngfuncs.GetClientTime();
//}
//
//void EV_EnsnareShoot(struct event_args_s* inArgs)
//{
//	int idx = inArgs->entindex;
//	gEngfuncs.pEventAPI->EV_PlaySound(idx, inArgs->origin, CHAN_WEAPON, kEnsnareFireSound, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
//	
//	vec3_t up, right, forward;
//	gEngfuncs.pfnAngleVectors(inArgs->angles, forward, right, up);
//	
//	// Fire spit glob
//	int theModelIndex;
//	struct model_s* theModel = gEngfuncs.CL_LoadModel(kEnsnareSprite, &theModelIndex);
//	ASSERT(theModel);
//	TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->CL_TempEntAlloc(gPredictedPlayerOrigin, theModel);
//	if(theTempEntity)
//	{
//		vec3_t vecSrc, vecAiming;
//		vec3_t theStartPos, theEndPos;
//		EV_GetGunPosition(inArgs, vecSrc, inArgs->origin);
//		VectorMA(vecSrc, kEnsnareBarrelLength, forward, theStartPos);
//		
//		// Create tiny ensnare bits where it leaves
//		AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsEnsnareShoot, theStartPos);
//		
//		VectorCopy(theStartPos, theTempEntity->entity.origin);
//		VectorCopy(theStartPos, theTempEntity->entity.prevstate.origin);
//		VectorCopy(theStartPos, theTempEntity->entity.curstate.origin);
//		theTempEntity->die += 10.0f;
//		theTempEntity->hitcallback = EnsnareHit;
//		theTempEntity->flags |= (FTENT_COLLIDEALL | FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP | FTENT_PERSIST);
//		theTempEntity->entity.curstate.framerate = 30;
//		theTempEntity->frameMax = 4;//theModel->numframes;
//		
//		// Temp entities interpret baseline origin as velocity.
//		Vector theBaseVelocity;
//		VectorScale(inArgs->velocity, kEnsnareParentVelocityScalar, theBaseVelocity);
//		
//		Vector theStartVelocity;
//		VectorMA(theBaseVelocity, kEnsnareVelocity, forward, theStartVelocity);
//		
//		VectorCopy(theStartVelocity, theTempEntity->entity.baseline.origin);
//		VectorCopy(theStartVelocity, theTempEntity->entity.baseline.velocity);
//	}
//}
//
//void EV_EnsnareHit(struct event_args_s* inArgs)
//{
//	int idx = inArgs->entindex;
//	gEngfuncs.pEventAPI->EV_PlaySound(idx, inArgs->origin, CHAN_WEAPON, kEnsnareHitSound, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
//
//	AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsEnsnareHit, inArgs->origin);
//}

void ShootCloud(struct event_args_s* inArgs, const char* inSpriteName)
{
	int idx = inArgs->entindex;
	gEngfuncs.pEventAPI->EV_PlaySound(idx, inArgs->origin, CHAN_WEAPON, kSporeFireSound, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
	if (EV_IsLocal(idx))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
	
	// Create temp cloud projectile
	int theModelIndex;
	struct model_s* theModel = gEngfuncs.CL_LoadModel(inSpriteName, &theModelIndex);
	if(theModel)
	{
		TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->CL_TempEntAlloc(gPredictedPlayerOrigin, theModel);
		if(theTempEntity)
		{
			vec3_t angles;
			VectorCopy( inArgs->angles, angles );
			
			//vec3_t velocity
			//VectorCopy( inArgs->velocity, velocity );
			
			vec3_t forward, right, up;
			gEngfuncs.pfnAngleVectors(angles, forward, right, up);
			
			vec3_t theStartPos, theEndPos, vecSrc;
			EV_GetGunPosition(inArgs, vecSrc, inArgs->origin);
			VectorMA(vecSrc, kSporeBarrelLength, forward, theStartPos);
			
			VectorCopy(theStartPos, theTempEntity->entity.origin);
			VectorCopy(theStartPos, theTempEntity->entity.prevstate.origin);
			VectorCopy(theStartPos, theTempEntity->entity.curstate.origin);
			//theTempEntity->die += kSpitLifetime;
			//theTempEntity->hitcallback = SpitHit;
			theTempEntity->flags |= (FTENT_COLLIDEALL | FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP | FTENT_COLLIDEKILL);
			theTempEntity->entity.curstate.framerate = 20;
			theTempEntity->frameMax = 17;//theModel->numframes;
			
			// Temp entities interpret baseline origin as velocity.
			Vector theStartVelocity(0, 0, 0);
			VectorMA(theStartVelocity, kShootCloudVelocity, forward, theStartVelocity);
			
			VectorCopy(theStartVelocity, theTempEntity->entity.baseline.origin);
			VectorCopy(theStartVelocity, theTempEntity->entity.baseline.velocity);
		}
	}
}

void EV_SporeShoot(struct event_args_s* inArgs)
{
	AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsSporeShoot, inArgs->origin);

	ShootCloud(inArgs, kClientSporeSprite);
}

void EV_SporeCloud(struct event_args_s* inArgs)
{
	int idx = inArgs->entindex;
	gEngfuncs.pEventAPI->EV_PlaySound(idx, inArgs->origin, CHAN_AUTO, kSporeCloudSound, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

	AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsSporeCloud, inArgs->origin);
}

void EV_UmbraGun(struct event_args_s* inArgs)
{
	ShootCloud(inArgs, kClientUmbraSprite);
	
//	if (EV_IsLocal(inArgs->entindex))
//	{
//		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
//	}
}

void EV_UmbraCloud(struct event_args_s* inArgs)
{
	int thePitch = gEngfuncs.pfnRandomLong(75, 150);
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_AUTO, kUmbraFireSound, inArgs->fparam1, ATTN_IDLE, 0, thePitch);
	
	AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsUmbraCloud, inArgs->origin);
}

void ParticleCallback( struct particle_s* particle, float frametime )
{
	int i;

	for ( i = 0; i < 3; i++ )
	{
		particle->org[ i ] += particle->vel[ i ] * frametime;
	}
}


void EV_Jetpack(struct event_args_s* inArgs)
{
 	AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsJetpackEffect, inArgs->origin);

	// Update jetpack sound every once in awhile
	if(gEngfuncs.pfnRandomLong(0, 3) == 0)
	{
		gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_BODY, kJetpackSound, .5f, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	}

	// Use this as index instead?
	//args->entindex;

//	// Create a dlight for jetpack
//	int theIndexToUse = gJetpackLights.size();
//	dlight_t*	dl = gEngfuncs.pEfxAPI->CL_AllocDlight(theIndexToUse);
//	VectorCopy (args->origin, dl->origin);
//	dl->radius = 180;
//	dl->color.r = 180;
//	dl->color.g = 180;
//	dl->color.b = 250;
//
//	// don't die for forseeable future
//	dl->die = gEngfuncs.GetClientTime() + kArbitraryLargeLightTime;
//	//dl->die += kArbitraryLargeLightTime;
//
//	gJetpackLights.push_back(LightType(args->entindex, dl));
//
//	gEngfuncs.pEventAPI->EV_PlaySound( args->entindex, args->origin, CHAN_BODY, kJetpackSound, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
	// Create temporary smoke trail!
	//void		( *R_Sprite_Trail )				( int type, float * start, float * end, int modelIndex, int count, float life, float size, float amplitude, int renderamt, float speed );
	//int theSmokeModelIndex = gEngfuncs.pEventAPI->EV_FindModelIndex( "sprites/smoke.spr" );
	//gEngfuncs.pEfxAPI->R_PlayerSprites(args->entindex, theSmokeModelIndex, 20, 100);
	
	//gEngfuncs.pEfxAPI->R_Sprite_Trail( TE_SPRITETRAIL, tr.endpos, fwd, theSmokeModelIndex, (int)(n * flDamage * 0.3), 0.1, gEngfuncs.pfnRandomFloat( 10, 20 ) / 100.0, 100, 255, 200 );
	
}

// Passing no param resets all sounds
void EndJetpackEffects(int inIndex)
{
	for(DLightListType::iterator theIter = gJetpackLights.begin(); theIter != gJetpackLights.end(); theIter++)
	{
		if((theIter->mIndex == inIndex) || (inIndex == -1))
		{
			// Mark it for deletion by telling it to die a long time ago
			if(theIter->mLight)
			{
				theIter->mLight->die = 0;
			}
			gJetpackLights.erase(theIter);

			vec3_t theOrigin;
			gEngfuncs.pEventAPI->EV_PlaySound(inIndex, theOrigin, CHAN_BODY, "common/null.wav", 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
		}
	}
}

void EV_EndJetpack(struct event_args_s* args)
{
	// find dlight in list
	//int theIndex = args->entindex;
	//EndJetpackEffects(theIndex);
}

void DrawCircleOnGroundAtPoint(vec3_t inOrigin, int inNumSides, int inStartAngle, int inRadius, float inR, float inG, float inB, float inA, bool inUseRedInstead, float inInnerRadius)
{
	static HSPRITE theGreenSprite = 0;
	if(!theGreenSprite)
		theGreenSprite = Safe_SPR_Load("sprites/green.spr");
	
	static HSPRITE theRedSprite = 0;
	if(!theRedSprite)
		theRedSprite = Safe_SPR_Load("sprites/red.spr");
	
	HSPRITE theSprite = theGreenSprite;
	if(inUseRedInstead)
		theSprite = theRedSprite;
	
	if(gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer(theSprite), 0))
	{
		gEngfuncs.pTriAPI->CullFace( TRI_NONE );
		
		gEngfuncs.pTriAPI->RenderMode( kRenderGlow  );
		//gEngfuncs.pTriAPI->RenderMode( kRenderTransTexture );
		gEngfuncs.pTriAPI->Color4f(inR, inG, inB, inA);
		
		gEngfuncs.pTriAPI->Begin( TRI_TRIANGLES );
		float theAngleIncrement = (2*M_PI)/inNumSides;
		float theStartAngle = (2*M_PI)*(inStartAngle/360.0f);
		
		for(int i = 0; i < inNumSides; i++)
		{
			for(int j = 0; j < 3; j++)
			{
				float thePoint[3];
				float theCurrentPointAngle = theStartAngle + i*theAngleIncrement;
				thePoint[0] = inOrigin[0] + inRadius*cos(theCurrentPointAngle);
				thePoint[1] = inOrigin[1] + inRadius*sin(theCurrentPointAngle);
				thePoint[2] = inOrigin[2];
				
				float theNextPoint[3];
				float theNextPointAngle = theStartAngle + (i+1)*theAngleIncrement;
				theNextPoint[0] = inOrigin[0] + inRadius*cos(theNextPointAngle);
				theNextPoint[1] = inOrigin[1] + inRadius*sin(theNextPointAngle);
				theNextPoint[2] = inOrigin[2];
				
				//gEngfuncs.pTriAPI->Color4f(inR, inG, inB, 1.0f);
				//gEngfuncs.pTriAPI->Brightness( 1 );
				gEngfuncs.pTriAPI->TexCoord2f(0, 0);
				gEngfuncs.pTriAPI->Vertex3f( inOrigin[0], inOrigin[1], inOrigin[2] );
				
				//gEngfuncs.pTriAPI->Color4f(inR, inG, inB, inA);
				//gEngfuncs.pTriAPI->Brightness( 1 );
				gEngfuncs.pTriAPI->TexCoord2f(0, 1);
				gEngfuncs.pTriAPI->Vertex3f( theNextPoint[0], theNextPoint[1], theNextPoint[2] );
				
				//gEngfuncs.pTriAPI->Color4f(inR, inG, inB, inA);
				//gEngfuncs.pTriAPI->Brightness( 1 );
				gEngfuncs.pTriAPI->TexCoord2f(1, 1);
				gEngfuncs.pTriAPI->Vertex3f( thePoint[0], thePoint[1], thePoint[2] );
				
			}
		}
		gEngfuncs.pTriAPI->End();
		gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
	}
}

//void DrawOrderDirectionIndicator(const AvHOrder& inOrder)
//{
//	static HSPRITE theSprite = 0;
//	if(!theSprite)
//		theSprite = Safe_SPR_Load("sprites/320questionmark.spr");
//	
//	int theCurrentFrame = 0;
//	
//	// 
//	vec3_t theScreenPos;
//	vec3_t theWorldPos;
//	inOrder.GetLocation(theWorldPos);
	
//	gEngfuncs.pTriAPI->WorldToScreen((float*)theWorldPos, (float*)theScreenPos);
//
//	int theBaseX = theScreenPos.x*ScreenWidth;
//	int theBaseY = theScreenPos.y*ScreenHeight;
//
//	int theSpriteWidth = SPR_Width(theSprite, theCurrentFrame);
//	int theSpriteHeight = SPR_Height(theSprite, theCurrentFrame);
//	
//	int theX = min(max(theSpriteWidth, theBaseX), ScreenWidth - theSpriteWidth);
//	int theY = min(max(theSpriteHeight, theBaseY), ScreenHeight - theSpriteHeight);
//	
//	SPR_Set(theSprite, 255, 255, 255);
//	SPR_DrawHoles((int)0, theX, theY, NULL);

//	gHUD.SetOrderPos(theWorldPos);
//}

//void DrawOrdersForPlayers(EntityListType& inPlayerList)
//{
//	OrderListType theOrders = gHUD.GetOrderList();
//
//	gHUD.ClearOrderPos();
//	
//	// Run through the order list type
//	for(OrderListType::iterator theIter = theOrders.begin(); theIter != theOrders.end(); theIter++)
//	{
//		// For each one, if the order is for a player in the inPlayerList, draw it
//		vec3_t theOrderLocation;
//		theIter->GetLocation(theOrderLocation);
//
//		if(theIter->GetOrderTargetType() == ORDERTARGETTYPE_TARGET)
//		{
//			int theTargetIndex = theIter->GetTargetIndex();
//			cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(theTargetIndex);
//			if(theEntity)
//			{
//				VectorCopy(theEntity->origin, theOrderLocation);
//			}
//		}
//
//		// Draw dotted line or something from relevant player to dest?
//		bool theDrawWaypoint = false;
//		EntityListType thePlayerList = theIter->GetReceivers();
//		for(EntityListType::iterator thePlayerIter = thePlayerList.begin(); thePlayerIter != thePlayerList.end(); thePlayerIter++)
//		{
//			cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(*thePlayerIter);
//			if(theEntity)
//			{
//				EntityListType::iterator theSearchIter = std::find(inPlayerList.begin(), inPlayerList.end(), *thePlayerIter);
//				if(theSearchIter != inPlayerList.end())
//				{
//					//gEngfuncs.pEfxAPI->R_ParticleLine((float*)theEntity->origin, (float*)theOrderLocation, 0, 255, 0, .05f);
//					theDrawWaypoint = true;
//				}
//			}
//		}
//		if(theDrawWaypoint)
//		{
//			float r, g, b, a;
//			theIter->GetOrderColor(r, g, b, a);
//
//			// This doesn't really use the color yet
//			DrawCircleOnGroundAtPoint(theOrderLocation, 11, 0, 20, r, g, b, a, false, 0.0f);
//
//			// Draw HUD indicator at this position
//			DrawOrderDirectionIndicator(*theIter);
//		}
//	}
//}

//void DrawBlips(const pVector& inView)
//{
//	// Get enemy blips from HUD and draw 'em
//	AvHVisibleBlipList& theEnemyBlipList = gHUD.GetEnemyBlipList();
//	theEnemyBlipList.Draw(inView);
//
//	// Get friendly blips from HUD and draw 'em
//	AvHVisibleBlipList& theFriendlyBlipList = gHUD.GetFriendlyBlipList();
//	theFriendlyBlipList.Draw(inView);
//}

void DrawDebugEffects()
{
	for(DebugPointListType::iterator theIter = gTriDebugLocations.begin(); theIter != gTriDebugLocations.end(); theIter++)
	{
		vec3_t thePosition;
		thePosition.x = theIter->x;
		thePosition.y = theIter->y;
		thePosition.z = theIter->z;
		int theRadius = theIter->mSize;
		DrawCircleOnGroundAtPoint(thePosition, 3, 0, theRadius, 1, 1, 0, .5f, false, 0.0f);
	}
	for(theIter = gSquareDebugLocations.begin(); theIter != gSquareDebugLocations.end(); theIter++)
	{
		vec3_t thePosition;
		thePosition.x = theIter->x;
		thePosition.y = theIter->y;
		thePosition.z = theIter->z;
		int theRadius = theIter->mSize;
		DrawCircleOnGroundAtPoint(thePosition, 4, 0, theRadius, 1, 1, 0, .5f, false, 0.0f);
	}
	gSquareDebugLocations.clear();
	
	// Run through particle systems, drawing a box around each one
	
	//	for(DebugEntityListType::iterator theCubeIter = gCubeDebugEntities.begin(); theCubeIter != gCubeDebugEntities.end(); theCubeIter++)
	//	{
	//		cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(*theCubeIter);
	//		if(theEntity)
	//		{
	//			Vector theStartPos = theEntity->curstate.origin + theEntity->curstate.mins;
	//			Vector theEndPos = theEntity->curstate.origin + theEntity->curstate.maxs;
	//			//gEngfuncs.pEfxAPI->R_ParticleBox( (float *)&(theStartPos), (float *)&(theEndPos), 5.0, 0, 255, .5f );
	//			float theMiddleX = (theStartPos.x + theEndPos.x)/2.0f;
	//			float theMiddleY = (theStartPos.y + theEndPos.y)/2.0f;
	//			Vector thePoint;
	//			thePoint.x = theMiddleX;
	//			thePoint.y = theMiddleY;
	//			thePoint.z = theStartPos.z;
	//			DrawCircleOnGroundAtPoint(thePoint, 4, 45, 64, .8f, .8f, .8f, .5f, false, 0.0f);
	//
	//			thePoint.z = theEndPos.z;
	//			DrawCircleOnGroundAtPoint(thePoint, 4, 45, 64, .8f, .8f, .8f, .5f, false, 0.0f);
	//		}
	//	}
}

void DrawMarineLight(const pVector& inView, vec3_t& inStartPos, vec3_t& inEndPos)
{
	gEngfuncs.pEfxAPI->R_RocketTrail(inStartPos, inEndPos, 1);

//	// Draw triangle strip that faces camera
//	const float kBeamAlpha = .7f;
//	const float kBeamWidth = 10;
//
//	pVector up(0, 0, 1);
//	pVector right = inView ^ up;
//	right.normalize();
//	pVector nup = right ^ inView;
//	//pVector nup = inView ^ right;
//
//	// The particles should face you unless constrained (this is for rain and the like)
//	if(this->mConstrainPitch)
//		nup = up;
//
//	//right *= size_scale/2.0f;
//	//nup *= size_scale/2.0f;
//
//	// Quad draws v0, v1, v2, v3
//	//pVector V0 = -(right + nup);
//
//	// Upper left
//	pVector V0 = -(right + nup);
//	//V0 = -(right - nup);
//	V0 = nup - right;
//
//	// Lower left
//	pVector V1 = -(right + nup);
//
//	// Lower right
//	pVector V2 = right + nup;
//	V2 = right - nup;
//
//	// Upper right
//	pVector V3 = right - nup;
//	V3 = right + nup;
//
//	// Tri draws v0, v4, v3
//	//pVector V4 = nup;
//	pVector V4 = -nup;
//
//	// Load red sprite
//	//if(gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer(this->mSprite), theTextureOffsetToUse))
//	//{
//		gEngfuncs.pTriAPI->Begin( TRI_TRIANGLES );
//
//		gEngfuncs.pTriAPI->Color4f(1.0f, 0.0f, 0.0f, kBeamAlpha);
//		
//			//gEngfuncs.pTriAPI->TexCoord2f(0,0);
//			pVector ver = p + sV0;
//			gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
//			
//			//gEngfuncs.pTriAPI->TexCoord2f(.5,1);
//			ver = p + sV4;
//			gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
//			
//			//gEngfuncs.pTriAPI->TexCoord2f(1,0);
//			ver = p + sV3;
//			gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
//
//		gEngfuncs.pTriAPI->End();
//	//}
}

void DrawMarineLights(const pVector& inView)
{
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, false );
	
	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();
	
	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers(-1);
	
	physent_t* theEntity = NULL;
	int theNumEnts = pmove->numphysent;
	for (int i = 0; i < theNumEnts; i++)
	{
		theEntity = gEngfuncs.pEventAPI->EV_GetPhysent(i);
		
		// If player is a marine
		if(theEntity->iuser3 == AVH_USER3_MARINE_PLAYER)
		{
			// Get middle of head
			//theEntity->model->
			//model->attachment = model->origin;
			
			// Get eyepiece attachment
			cl_entity_t* theClientEntity = gEngfuncs.GetEntityByIndex(theEntity->info);
			if(theClientEntity)
			{
				Vector theHeadPos = theClientEntity->attachment[2];

				// Get look direction
				Vector theForward, theRight, theUp;

				gEngfuncs.pfnAngleVectors(theClientEntity->angles, theForward, theRight, theUp);
				Vector theAiming;
				for(int i = 0; i < 3; i++)
				{
					theAiming[i] = theForward[i] + theRight[i] + theUp[i];
				}
				
				// Build this line
				Vector theStartPos = theHeadPos;
				Vector theEndPos;
				VectorMA(theStartPos, 8000, theAiming, theEndPos);
				
				// Perform trace, ignore glass (!), ignore ourself
				gEngfuncs.pEventAPI->EV_SetTraceHull(2);
				//int theFoundEntity = theEntity->info;
				int theFoundEntity = -1;
				pmtrace_t tr;
				
				bool theDone = false;
				do
				{
					// NOTE: ** When implementing this again, put in check for infinite loop condition! **

					// Run a traceline along this line until it hits
					gEngfuncs.pEventAPI->EV_PlayerTrace(theStartPos, theEndPos, PM_GLASS_IGNORE, theFoundEntity, &tr);
				
					//if((tr.fraction >= (1.0f - kFloatTolerance)) || (tr.fraction == 0.0f)/*|| (theNumIterations > 100)*//* || (tr.fraction < kFloatTolerance)*/)
					//{
					//	theDone = true;
					//}
					//else
					//{
					//	// Offset a bit so we don't hit again
					//	VectorMA(tr.endpos, kHitOffsetAmount, theForward, theStartPos);
					//}

					if(tr.startsolid)
					{
						VectorMA(tr.endpos, kHitOffsetAmount, theForward, theStartPos);
					}
					else
					{
						theDone = true;
					}

				} while(!theDone);
					
				// Draw red additive light along this line
				DrawMarineLight(inView, theStartPos, tr.endpos);
			}
		}
	}

	gEngfuncs.pEventAPI->EV_PopPMStates();
}

//void DrawRangeIndicator()
//{
//	Vector theCenter;
//	bool theIsValid;
//	float theRadius;
//	
//	if(gHUD.GetRangeDrawingInfo(theCenter, theRadius, theIsValid))
//	{
//		const float theCircleWidth = 20.0f;
//		float theInnerRadius = theRadius - theCircleWidth;
//
//		static theAngle = 0;
//		DrawCircleOnGroundAtPoint(theCenter, theRadius/200, theAngle++, theRadius, 1.0f, 1.0f, 1.0f, 1.0f, !theIsValid, theInnerRadius);
//	}
//}

//void UpdateJetpackLights()
//{
//	for(DLightListType::iterator theIter = gJetpackLights.begin(); theIter != gJetpackLights.end(); theIter++)
//	{
//		// Treat locally differently
//		int theIndex = theIter->mIndex;
//		vec3_t theOrigin;
//
//		if(theIndex == 0)
//		{
//			VectorCopy(gPredictedPlayerOrigin, theOrigin);
//		}
//		else
//		{
//			cl_entity_t* thePlayer = gEngfuncs.GetEntityByIndex(theIndex);
//			if(thePlayer)
//			{
//				// Get known position of entity
//				VectorCopy(thePlayer->origin, theOrigin);
//			}
//			else
//			{
//				ASSERT(false);
//			}
//		}
//		
//		VectorCopy(theOrigin, theIter->mLight->origin);
//		
//		// Make sure it still won't die
//		theIter->mLight->die = gEngfuncs.GetClientTime() + kArbitraryLargeLightTime;
//		
//		// Update jetpack sound every once in awhile
//		if(gEngfuncs.pfnRandomLong(0, 70) == 0)
//		{
//			gEngfuncs.pEventAPI->EV_PlaySound( theIndex, theOrigin, CHAN_BODY, kJetpackSound, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
//		}
//	}
//}

void EV_WelderGeneralEffects(struct event_args_s* inArgs, const Vector& thePos)
{
	int theIndex = inArgs->entindex;
	cl_entity_t* thePlayer = GetEntity(theIndex);
	
	// Don't play flashing effects if player is photosensitive
	if(CVAR_GET_FLOAT(kvDynamicLights))
	{
		// Make flashing lights
		int theLightIndex = 20; /* TODO: What do to about this index? */
		dlight_t* theLight = gEngfuncs.pEfxAPI->CL_AllocDlight(theLightIndex);
		VectorCopy (thePlayer->origin, theLight->origin);
		
		int theUpgradeLevel = AvHPlayerUpgrade::GetWeaponUpgrade(AVH_USER3_MARINE_PLAYER, GetUpgradeState(theIndex));
		
		theLight->radius = 180 + theUpgradeLevel*15;
		theLight->color.r = 220;
		theLight->color.g = 220;
		theLight->color.b = 255;
		
		// Have it die before it fires again, so it flickers on and off
		theLight->die = gEngfuncs.GetClientTime() + BALANCE_VAR(kWelderROF)/2.2f;
	}
	
	// TODO: Apply burn mark
	//gEngfuncs.pEfxAPI->R_DecalShoot( gEngfuncs.pEfxAPI->Draw_DecalIndex( gEngfuncs.pEfxAPI->Draw_DecalIndexFromName( "{shot1" ) ), 
	//								gEngfuncs.pEventAPI->EV_IndexFromTrace(&theTraceResult), 
	//								0, 
	//								theTraceResult.endpos, 
	//								0 );
	
	// emit heavy smoke
	if(gEngfuncs.pfnRandomLong(0, 1) == 0)
	{
		AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsWelderSmoke, thePos);
	}
}

void EV_WelderHitEffects(struct event_args_s* inArgs, const Vector& thePos)
{
	int theIndex = inArgs->entindex;
	cl_entity_t* thePlayer = GetEntity(theIndex);
	
	// Fire sparks
	//Vector theVector(inPev->origin);
	//float theRandomFloat = RANDOM_FLOAT(0.5F, 1.0f);
	//float theHeight = theRandomFloat*(inPev->absmax.z - inPev->absmin.z);
	
	//gEngfuncs.pEfxAPI->R_SparkShower(theTraceResult.endpos);
	//gEngfuncs.pEfxAPI->R_SparkShower(theTraceResult.endpos);
	
	// Fire heavy smoke
	//if(gEngfuncs.pfnRandomLong(0, 1) == 0)
	//{
	//	AvHParticleSystemManager::Instance()->CreateParticleSystem(5, theTraceResult.endpos);
	//}
	
	// Generate still plasma
	if(gEngfuncs.pfnRandomLong(0, 1) == 0)
	{
		AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsWelderBluePlasma, thePos);
	}
	
	// Generate plasma shower
	if(gEngfuncs.pfnRandomLong(0, 8) == 0)
	{
		AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsWelderBluePlasmaDrops, thePos);
	}
	
	// Generate spark shower
	float theFloatVec[3];
	thePos.CopyToArray(theFloatVec);

	int theUpgradeLevel = AvHPlayerUpgrade::GetWeaponUpgrade(AVH_USER3_MARINE_PLAYER, GetUpgradeState(theIndex));
	for(int i = 0; i < theUpgradeLevel+1; i++)
	{
		gEngfuncs.pEfxAPI->R_SparkShower(theFloatVec);
	}
}

void EV_Welder(struct event_args_s* inArgs)
{
	// This event means the welder is on, but we don't know if we're hitting anything or are constructing
	int theIndex = inArgs->entindex;
	cl_entity_t* thePlayer = GetEntity(theIndex);

	// Hot blue flame
	
	// See if we're hitting something and playback smoke and sparks if so
	// Trace to see where the sparks should emanate from
	vec3_t vecSrc;
	vec3_t origin;
	vec3_t forward, right, up;
	
	gEngfuncs.pfnAngleVectors(inArgs->angles, forward, right, up);
	
	// Calculate theStartPos, taking into account the welder barrel length
	vec3_t theStartPos, theEndPos;
	EV_GetGunPosition(inArgs, vecSrc, inArgs->origin);
	VectorMA(vecSrc, kWelderBarrelLength, forward, theStartPos);

	// Calculate theEndPos, welder range along facing
	theEndPos = theStartPos + forward * BALANCE_VAR(kWelderRange);

	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
	
	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();
	
	// Now add in all of the players.
	//int theNumPlayers = gEngfuncs.GetMaxClients();
	//gEngfuncs.pEventAPI->EV_SetSolidPlayers ( -1 );

	gEngfuncs.pEventAPI->EV_SetTraceHull( 2 );

	// General x-punch axis
	if (EV_IsLocal(theIndex))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}

	// Don't hit ourselves, but how?  Setting this to theIndex isn't right, we need the other kind of index, but how to get it?
	// For now this function will cause the welder to hit the player using the welder when aiming down, sigh
	int theEntityToIngore = -1;

	struct pmtrace_s theTraceResult;
	gEngfuncs.pEventAPI->EV_PlayerTrace(theStartPos, theEndPos, PM_NORMAL, theEntityToIngore, &theTraceResult);
	if(theTraceResult.fraction != 1.0f)
	{
		// Adjust the trace so it's offset a bit towards us so particles aren't clipped away
		//Vector theResult = theTraceResult.endpos;//theStartPos + forward*(theTraceResult.fraction - 0.001);
		Vector theResult = theStartPos + forward*((theTraceResult.fraction - 0.1f)*BALANCE_VAR(kWelderRange));
		
		EV_WelderGeneralEffects(inArgs, theResult);
		EV_WelderHitEffects(inArgs, theResult);
		
		// Update status bar if possible
		physent_t* thePhysEnt = gEngfuncs.pEventAPI->EV_GetPhysent( theTraceResult.ent );
		//if(!thePhysEnt || !thePhysEnt->player)
		if(thePhysEnt /*&& !thePhysEnt->player*/)
		{
			bool theIsBuilding;
			bool theIsResearching;
			float thePercentage;
			AvHSHUGetBuildResearchState(thePhysEnt->iuser3, thePhysEnt->iuser4, thePhysEnt->fuser1, theIsBuilding, theIsResearching, thePercentage);

//			if(thePhysEnt->iuser3 == AVH_USER3_WELD)
//			{
//				if((thePercentage < 1.0f) && (thePercentage != -1))
//				{
//					gHUD.SetProgressStatus(thePercentage);
//				}
//				else
//				{
//					gHUD.HideProgressStatus();
//				}
//			}
		}
	}
	// Fire light smoke from muzzle
	else //if(gEngfuncs.pfnRandomLong(0, 1) == 0)
	{
		// TODO: emit this from the actual end of the barrel
		//AvHParticleSystemManager::Instance()->CreateParticleSystem(5, theStartPos + forward*10);
		
		EV_WelderGeneralEffects(inArgs, theStartPos + (forward*(.3f*BALANCE_VAR(kWelderRange))));
		
		// Stop playing hit sound
		//gEngfuncs.pEventAPI->EV_StopSound( theIndex, CHAN_WEAPON, kWeldingHitSound);
	}
	
	gEngfuncs.pEventAPI->EV_PopPMStates();
}

//void ClientPlayRandomConstructionSound(struct event_args_s* inArgs)
//{
//	int theRandomInteger = gEngfuncs.pfnRandomLong(1, 5);
//	char theSoundName[128];
//	sprintf(theSoundName, kMarineConstructionSounds, theRandomInteger);
//	
//	int thePitch = gEngfuncs.pfnRandomLong(100, 125);
//	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_BODY, theSoundName, 1, ATTN_NORM, SND_CHANGE_PITCH | SND_CHANGE_VOL, thePitch);
//}

// This event means the welder is constructing something, it's fired by the server
void EV_WelderConst(struct event_args_s* inArgs)
{
	int theIndex = inArgs->entindex;
	cl_entity_t* thePlayer = GetEntity(theIndex);

	switch(inArgs->iparam1)
	{
	case 0://Start playing loop sound.
		gEngfuncs.pEventAPI->EV_PlaySound( inArgs->entindex, thePlayer->origin, CHAN_BODY, kWeldingHitSound, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
		break;
	case 1://Stop playing loop sound.
		gEngfuncs.pEventAPI->EV_StopSound( inArgs->entindex, CHAN_BODY, kWeldingHitSound);
		gEngfuncs.pEventAPI->EV_PlaySound( inArgs->entindex, thePlayer->origin, CHAN_AUTO, kWeldingStopSound, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
		break;
	}
}

void EV_WelderStart(struct event_args_s* inArgs)
{
	int theIndex = inArgs->entindex;
	cl_entity_t* thePlayer = GetEntity(theIndex);
	
	gEngfuncs.pEventAPI->EV_PlaySound( inArgs->entindex, thePlayer->origin, CHAN_WEAPON, kWeldingSound, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
}

void EV_WelderEnd(struct event_args_s* inArgs)
{
	gEngfuncs.pEventAPI->EV_StopSound( inArgs->entindex, CHAN_WEAPON, kWeldingSound);
	
	// Hide the status, it will be shown below if possible
	gHUD.HideProgressStatus();
}

//void EV_OverwatchStart(struct event_args_s* inArgs)
//{
//	cl_entity_t* thePlayer = GetEntity(inArgs->entindex);
//	gEngfuncs.pEventAPI->EV_PlaySound( inArgs->entindex, thePlayer->origin, CHAN_VOICE, kOverwatchStartSound, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
//}
//
//void EV_OverwatchEnd(struct event_args_s* inArgs)
//{
//	cl_entity_t* thePlayer = GetEntity(inArgs->entindex);
//	gEngfuncs.pEventAPI->EV_PlaySound( inArgs->entindex, thePlayer->origin, CHAN_VOICE, kOverwatchEndSound, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
//}

void EV_Regeneration(struct event_args_s* inArgs)
{
	cl_entity_t* thePlayer = GetEntity(inArgs->entindex);
	ASSERT(thePlayer);
	
	float theVolume = inArgs->fparam1;
	
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, thePlayer->origin, CHAN_AUTO, kRegenerationSound, theVolume, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
}

void EV_StartCloak(struct event_args_s* inArgs)
{
	cl_entity_t* thePlayer = GetEntity(inArgs->entindex);
	ASSERT(thePlayer);

	float theVolume = inArgs->fparam1;

	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, thePlayer->origin, CHAN_AUTO, kStartCloakSound, theVolume, .4, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
}

void EV_EndCloak(struct event_args_s* inArgs)
{
	cl_entity_t* thePlayer = GetEntity(inArgs->entindex);
	ASSERT(thePlayer);

	float theVolume = inArgs->fparam1;

	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, thePlayer->origin, CHAN_AUTO, kEndCloakSound, theVolume, .4, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
}

//void EV_WallJump(struct event_args_s* inArgs)
//{
//	cl_entity_t* thePlayer = GetEntity(inArgs->entindex);
//	gEngfuncs.pEventAPI->EV_PlaySound( inArgs->entindex, thePlayer->origin, CHAN_VOICE, kWallJumpSound, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
//}

void EV_Flight(struct event_args_s* inArgs)
{
	cl_entity_t* thePlayer = GetEntity(inArgs->entindex);

	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, thePlayer->origin, CHAN_AUTO, kEndCloakSound, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
 	//gEngfuncs.pEventAPI->EV_PlaySound( inArgs->entindex, thePlayer->origin, CHAN_AUTO, kStartCloakSound/*kWingFlapSound*/, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
}

void TeleportCallback( struct tempent_s *ent, float frametime, float currenttime)
{
}

void EV_Teleport(struct event_args_s* inArgs)
{
	// Create a temp entity of this player and fade him out!
	cl_entity_t* thePlayer = GetEntity(inArgs->entindex);

//	// Kill earlier effect so the sound plays right?
//	int idx = inArgs->entindex;
//	gEngfuncs.pEventAPI->EV_KillEvents(idx, kTeleportEvent);
		
//	TEMPENTITY* theTempEnt = gEngfuncs.pEfxAPI->CL_TentEntAllocCustom( (float*)inArgs->origin, thePlayer->model, 0, TeleportCallback);
//	if(theTempEnt)
//	{
//		VectorCopy(inArgs->origin, theTempEnt->entity.origin);
//		VectorCopy(inArgs->angles, theTempEnt->entity.angles);
//
//		theTempEnt->flags |= FTENT_FADEOUT;
//		theTempEnt->entity.baseline.renderamt = 255;
//		theTempEnt->entity.curstate.renderfx = kRenderFxStrobeFaster;
//		theTempEnt->fadeSpeed = 1.3f;
//		theTempEnt->entity.curstate.movetype = MOVETYPE_NONE;
//
//		theTempEnt->entity.curstate.modelindex = thePlayer->curstate.modelindex;
//		theTempEnt->entity.curstate.frame = thePlayer->curstate.frame;
//		theTempEnt->entity.curstate.colormap = thePlayer->curstate.colormap;
//		theTempEnt->entity.curstate.skin = thePlayer->curstate.skin;
//		theTempEnt->entity.curstate.framerate = thePlayer->curstate.framerate;
//		theTempEnt->entity.curstate.body = thePlayer->curstate.body;
//		theTempEnt->entity.curstate.weaponmodel = thePlayer->curstate.weaponmodel;
//		theTempEnt->entity.curstate.sequence = thePlayer->curstate.sequence;
//		theTempEnt->entity.curstate.gaitsequence = thePlayer->curstate.gaitsequence;
//		theTempEnt->entity.curstate.usehull = thePlayer->curstate.usehull;
//		theTempEnt->entity.curstate.playerclass = thePlayer->curstate.playerclass;
//		theTempEnt->entity.curstate.animtime = thePlayer->curstate.animtime;
//		
//		memcpy(&theTempEnt->entity.curstate.controller[0], &thePlayer->curstate.controller[0], 4 * sizeof( byte ));
//		memcpy(&theTempEnt->entity.curstate.blending[0], &thePlayer->curstate.blending[0], 2 * sizeof( byte ));

		if(CVAR_GET_FLOAT(kvDynamicLights))
		{
			dlight_t* theDLight = gEngfuncs.pEfxAPI->CL_AllocDlight(1);
			VectorCopy(inArgs->origin, theDLight->origin);
			theDLight->radius = 200;
			theDLight->color.r = 180;
			theDLight->color.g = 180;
			theDLight->color.b = 250;
			
			// don't die for forseeable future
			theDLight->die = gEngfuncs.GetClientTime() + .25f;
		}

		// Draw cool particle system
		AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsTeleport, inArgs->origin);
		//	}
}

void EV_PhaseIn(struct event_args_s* inArgs)
{
	// Create a temp entity of this player and fade him out!
	cl_entity_t* thePlayer = GetEntity(inArgs->entindex);
	
	int thePitch = gEngfuncs.pfnRandomLong(75, 150);
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_AUTO, kPhaseInSound, 1, ATTN_NORM, 0, thePitch);
	
	if(CVAR_GET_FLOAT(kvDynamicLights))
	{
		dlight_t* theDLight = gEngfuncs.pEfxAPI->CL_AllocDlight(1);
		VectorCopy(inArgs->origin, theDLight->origin);
		theDLight->radius = 200;
		theDLight->color.r = 180;
		theDLight->color.g = 180;
		theDLight->color.b = 250;
		
		// don't die for forseeable future
		theDLight->die = gEngfuncs.GetClientTime() + 1.0f;
	}
	
	// Draw cool particle system
	AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsPhaseIn, inArgs->origin);
}

void EV_SiegeHit(struct event_args_s* inArgs)
{
	char* theSoundToPlay = kSiegeHitSound1;
	if(gEngfuncs.pfnRandomLong(0, 1) == 1)
	{
		theSoundToPlay = kSiegeHitSound2;
	}
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_BODY, theSoundToPlay, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
	gEngfuncs.pEfxAPI->R_BlobExplosion(inArgs->origin);
}

void EV_SiegeViewHit(struct event_args_s* inArgs)
{
	const float kPunchRange = 5.0f;
	
	V_PunchAxis(0, gEngfuncs.pfnRandomFloat(-kPunchRange, kPunchRange));
	V_PunchAxis(1, gEngfuncs.pfnRandomFloat(-kPunchRange, kPunchRange));
	V_PunchAxis(2, gEngfuncs.pfnRandomFloat(-kPunchRange, kPunchRange));
}	

void EV_CommandPoints(struct event_args_s* inArgs)
{
	if(CVAR_GET_FLOAT(kvDynamicLights))
	{
		dlight_t* theDLight = gEngfuncs.pEfxAPI->CL_AllocDlight(1);
		VectorCopy(inArgs->origin, theDLight->origin);
		theDLight->radius = 60;
		theDLight->color.r = 100;
		theDLight->color.g = 250;
		theDLight->color.b = 100;
		
		// don't die for forseeable future
		theDLight->die = gEngfuncs.GetClientTime() + 1.0f;
	}
}

void EV_AlienSightOn(struct event_args_s* inArgs)
{
	int thePitch = gEngfuncs.pfnRandomLong(75, 150);
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_AUTO, kAlienSightOnSound, inArgs->fparam1, ATTN_IDLE, 0, thePitch);
}

void EV_AlienSightOff(struct event_args_s* inArgs)
{
	int thePitch = gEngfuncs.pfnRandomLong(75, 150);
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_AUTO, kAlienSightOffSound, inArgs->fparam1, ATTN_IDLE, 0, thePitch);
}

void EV_ParasiteGun(struct event_args_s* inArgs)
{
	// Sharp falloff, so it's only heard at very close and by the firer
	int thePitch = gEngfuncs.pfnRandomLong(75, 150);
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_AUTO, kParasiteFireSound, .4f*inArgs->fparam1, .2, 0, thePitch);

	if (EV_IsLocal(inArgs->entindex))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
}

void EV_BlinkSuccess(struct event_args_s* inArgs)
{
	// Create particle at origin, but also at angles (interpret as dest)
	//AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsBlinkEffect, inArgs->origin);
	//AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsBlinkEffect, inArgs->angles);
	
	int thePitch = gEngfuncs.pfnRandomLong(75, 150);
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_AUTO, kBlinkSuccessSound, inArgs->fparam1, ATTN_IDLE, 0, thePitch);
	//gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->angles, CHAN_WEAPON, kBlinkSuccessSound, inArgs->fparam1, ATTN_IDLE, 0, thePitch);
}

void EV_DivineWind(struct event_args_s* inArgs)
{
	cl_entity_t* thePlayer = GetEntity(inArgs->entindex);

	float theSilenceVolumeFactor = AvHPlayerUpgrade::GetSilenceVolumeLevel((AvHUser3)thePlayer->curstate.iuser3, thePlayer->curstate.iuser4);
	int thePitch = gEngfuncs.pfnRandomLong(75, 150);

	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_AUTO, kDivineWindFireSound, theSilenceVolumeFactor, ATTN_IDLE, 0, thePitch);

	if (EV_IsLocal(inArgs->entindex))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
}

void BileBombHit(struct tempent_s* ent, struct pmtrace_s* ptr)
{
	gEngfuncs.pEventAPI->EV_PlaySound(ent->entity.index, ptr->endpos, CHAN_AUTO, kBileBombHitSound, 1, ATTN_IDLE, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

	AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsBilebomb, ptr->endpos, &(ptr->plane.normal));
}

void EV_BileBomb(struct event_args_s* inArgs)
{
	ASSERT(inArgs);

    // Figure out which weapon description to assocate this weapon with
    int theWeaponIndex = inArgs->iparam1;
	
	// Play attack animation and add muzzle flash
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = inArgs->entindex;
	VectorCopy( inArgs->origin, origin );
	VectorCopy( inArgs->angles, angles );
	VectorCopy( inArgs->velocity, velocity );
	
	//AngleVectors( angles, forward, right, up );
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	
	int thePitch = gEngfuncs.pfnRandomLong(75, 150);
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_AUTO, kBileBombFireSound, inArgs->fparam1, ATTN_IDLE, 0, thePitch);

	vec3_t theBombAngles;
	VectorAngles(forward, theBombAngles);

	theBombAngles.y -= 90;

	// Fire bomb
	TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->R_TempModel(inArgs->origin, forward, theBombAngles, 100, gEngfuncs.pEventAPI->EV_FindModelIndex(kBileBombProjectileModel), 0);
	if(theTempEntity)
	{
		theTempEntity->hitcallback = BileBombHit;
		theTempEntity->flags = (FTENT_GRAVITY | FTENT_COLLIDEALL | FTENT_COLLIDEKILL | FTENT_PERSIST);
		theTempEntity->die += 5;
		theTempEntity->entity.curstate.framerate = 30;
		//theTempEntity->frameMax = 4;//theModel->numframes;
		theTempEntity->clientIndex = inArgs->entindex;	// Entity to ignore collisions with
		
		Vector theAiming = forward;// + up;
		VectorNormalize(theAiming);
		
		Vector theStartVelocity;
		VectorMA(Vector(0, 0, 0), kBileBombVelocity, theAiming, theStartVelocity);
		
		VectorCopy(theStartVelocity, theTempEntity->entity.baseline.origin);
		VectorCopy(theStartVelocity, theTempEntity->entity.baseline.velocity);
	}

	// General x-punch axis
	if ( EV_IsLocal( idx ) )
	{
		float theHalfSpread = kBileBombPunch/2.0f;
		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis( 0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}

		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}

}

void AcidRocketHit(struct tempent_s* ent, struct pmtrace_s* ptr)
{
	char* theSoundToPlay = kAcidRocketHitSound;
	gEngfuncs.pEventAPI->EV_PlaySound(ent->entity.index, ptr->endpos, CHAN_AUTO, theSoundToPlay, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

	AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsAcidHitEffect, ptr->endpos, &(ptr->plane.normal));
}

void EV_AcidRocket(struct event_args_s* inArgs)
{
	ASSERT(inArgs);
	
    // Figure out which weapon description to assocate this weapon with
    int theWeaponIndex = inArgs->iparam1;
	
	// Play attack animation and add muzzle flash
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = inArgs->entindex;
	VectorCopy( inArgs->origin, origin );
	VectorCopy( inArgs->angles, angles );
	VectorCopy( inArgs->velocity, velocity );
	
	//AngleVectors( angles, forward, right, up );
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	
	int thePitch = gEngfuncs.pfnRandomLong(75, 150);
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_AUTO, kAcidRocketFireSound, inArgs->fparam1, ATTN_IDLE, 0, thePitch);

	vec3_t theRocketAngles;
	VectorAngles(forward, theRocketAngles);

	vec3_t theGunPosition;
	EV_GetGunPosition(inArgs, theGunPosition, inArgs->origin);

	Vector theRocketOrigin;
	VectorMA(theGunPosition, kAcidRocketBarrelLength, forward, theRocketOrigin);
	
	// Projectile is rotated a bit
	TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->R_TempModel(theRocketOrigin, forward, theRocketAngles, 100, gEngfuncs.pEventAPI->EV_FindModelIndex(kAcidRocketProjectileModel), 0);
	if(theTempEntity)
	{
		theTempEntity->hitcallback = AcidRocketHit;
		theTempEntity->flags = (FTENT_COLLIDEALL | FTENT_COLLIDEKILL | FTENT_PERSIST);
		theTempEntity->entity.curstate.framerate = 30;
		theTempEntity->frameMax = 4;//theModel->numframes;
		
		// Temp entities interpret baseline origin as velocity.
		Vector theBaseVelocity;
		VectorScale(inArgs->velocity, kAcidRocketParentVelocityScalar, theBaseVelocity);
		
		Vector theStartVelocity;
		VectorMA(theBaseVelocity, kAcidRocketVelocity, forward, theStartVelocity);
		
		VectorCopy(theStartVelocity, theTempEntity->entity.baseline.origin);
		VectorCopy(theStartVelocity, theTempEntity->entity.baseline.velocity);
	}
	
	// General x-punch axis
	if ( EV_IsLocal( idx ) )
	{
		float theHalfSpread = kAcidRocketPunch/2.0f;
		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis( 0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}

		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
}

void UpdateStomp(TEMPENTITY* inEntity, float frametime, float inCurrentTime)
{
	float theTimeScalar = (inCurrentTime - inEntity->entity.curstate.fuser1)/(inEntity->entity.curstate.fuser2/inEntity->entity.curstate.fuser1);
	theTimeScalar = max(min(theTimeScalar, 1.0f), 0.0f);

	// Fade it out according to lifetime
	int theAlpha = (1.0f - theTimeScalar)*kStompModelRenderAmount;
	inEntity->entity.curstate.renderamt = theAlpha;

	// Don't create at end of trail
	if(theTimeScalar < .7f)
	{
		// Every once in a awhile, create a smoke puff
		if(gEngfuncs.pfnRandomLong(0, 1) == 0)
		{
			AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsStompSmoke, inEntity->entity.origin);
		}
	}
}

void EV_Stomp(struct event_args_s* inArgs)
{
	ASSERT(inArgs);

	Vector theStartPos;
	VectorCopy(inArgs->origin, theStartPos);

	// Not sure why this is necessary, AvHStomp::GetStompOrigin() should take care of it
	if (EV_IsLocal(inArgs->entindex))
	{
		cl_entity_s* theLocalPlayer = gEngfuncs.GetLocalPlayer();
		if(theLocalPlayer)
		{
			float theZOffset = theLocalPlayer->curstate.mins.z;
			theStartPos.z += theZOffset;
		}
	}

	// Initial velocity store in angles, to make sure event and server entity match up
	Vector theStartVelocity;
	VectorCopy(inArgs->angles, theStartVelocity);

	char* theSoundToPlay = kStompFireSound;
	
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, theStartPos, CHAN_WEAPON, theSoundToPlay, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
	if (EV_IsLocal(inArgs->entindex))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}

	vec3_t up, right, forward, vecSrc;//, origin;

//	VectorCopy( inArgs->origin, origin );
	gEngfuncs.pfnAngleVectors(inArgs->angles, forward, right, up);

	// Create stomp projectile that flies forward, going through objects and stunning anyone it touches

	vec3_t theStompAngles;
	VectorAngles(forward, theStompAngles);

	// Always draw level
	theStompAngles.x = 0;

	// Projectile artwork is oriented down the wrong axis
	theStompAngles.y -= 180;
	
	TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->R_TempModel(theStartPos, forward, theStompAngles, 100, gEngfuncs.pEventAPI->EV_FindModelIndex(kStompProjectileModel), 0);
	if(theTempEntity)
	{
//		vec3_t theStartPos, theEndPos;
//		EV_GetGunPosition(inArgs, vecSrc, inArgs->origin);
//		VectorMA(vecSrc, kStompBarrelLength, forward, theStartPos);
		
		VectorCopy(theStartPos, theTempEntity->entity.origin);
		VectorCopy(theStartPos, theTempEntity->entity.prevstate.origin);
		VectorCopy(theStartPos, theTempEntity->entity.curstate.origin);
		//theTempEntity->hitcallback = StompHit;
		theTempEntity->flags = (FTENT_PERSIST | FTENT_CLIENTCUSTOM/*| FTENT_SMOKETRAIL | FTENT_FADEOUT*/);
		theTempEntity->callback = UpdateStomp;

		// Set time created and time to expire so we can blend it in UpdateStomp
		theTempEntity->entity.curstate.fuser1 = gHUD.m_flTime;
		theTempEntity->entity.curstate.fuser2 = gHUD.m_flTime + kStompProjectileLifetime;	// estimated end time of animation

		theTempEntity->die = gEngfuncs.GetClientTime() + kStompProjectileLifetime;
		theTempEntity->bounceFactor = 0;

		theTempEntity->entity.curstate.rendermode = kRenderTransAdd;
		theTempEntity->entity.curstate.renderamt = kStompModelRenderAmount;

		//theTempEntity->entity.curstate.framerate = 30;
		//theTempEntity->frameMax = 4;//theModel->numframes;
		
		// Temp entities interpret baseline origin as velocity.

		// Zero out vertical component as it's a ground stomp
		forward[2] = 0.0f;

		// Send shockwave in direction player is looking, but always make it go kStompProjectilVelocity
		VectorNormalize(forward);

		Vector theStartVelocity;
		VectorScale(forward, kStompProjectileVelocity, theStartVelocity);
		
		VectorCopy(theStartVelocity, theTempEntity->entity.baseline.origin);
		VectorCopy(theStartVelocity, theTempEntity->entity.baseline.velocity);
	
		// Set orientation
		//VectorCopy(inArgs->angles, theTempEntity->entity.angles);
	}

	// Create stomp effect at Onos
	AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsStompEffect, theStartPos);
}

void EV_Devour(struct event_args_s* inArgs)
{
	ASSERT(inArgs);

	char* theSoundToPlay = kDevourFireSound;
	
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_WEAPON, theSoundToPlay, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
	if (EV_IsLocal(inArgs->entindex))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
}

void EV_Leap(struct event_args_s* inArgs)
{
	char* theSoundToPlay = kLeapSound;
	

	if (EV_IsSpec(inArgs->entindex))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
		gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_WEAPON, theSoundToPlay, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	}
	if (!EV_IsLocal(inArgs->entindex)) {
		gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_WEAPON, theSoundToPlay, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	}
}

void EV_Charge(struct event_args_s* inArgs)
{
	char* theSoundToPlay = kChargeSound;
	
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_WEAPON, theSoundToPlay, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

	if (EV_IsLocal(inArgs->entindex))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
}

//We're sending a leap event.
void EV_Ability(struct event_args_s* inArgs)
{
	if (EV_IsLocal(inArgs->entindex))
		gHUD.SetAlienAbility((AvHMessageID)inArgs->iparam1);
}



//physent_t* GetEntity(int inPhysIndex)
//{
//	return gEngfuncs.pEventAPI->EV_GetPhysent( inPhysIndex );
//}


void EV_Select(struct event_args_s* args)
{
	gSelectionHelper.ProcessPendingSelections();
}


void EV_Bite(struct event_args_s* inArgs)
{
	int idx = inArgs->entindex;
	
	// Play attack sound
	char* theSoundToPlay = "";
	
	int theRandomSound = gEngfuncs.pfnRandomLong(0, 1);
	switch(theRandomSound)
	{
	case 0:
	case 1:
		theSoundToPlay = kBiteSound;
		break;
	//case 1:
	//	theSoundToPlay = kBiteSound2;
	//	break;
	}


	const int kBasePitch = 80;
	const int kVariablePitchRange = 0x0;

	const int kBiteAdrenPitchFactor = 30;
	
	int theRandomPitch = gEngfuncs.pfnRandomLong( 0, kVariablePitchRange );
	int thePitch = kBasePitch + (inArgs->fparam1*kBiteAdrenPitchFactor) + theRandomPitch;
	gEngfuncs.pEventAPI->EV_PlaySound(idx, inArgs->origin, CHAN_WEAPON, theSoundToPlay, inArgs->fparam1, ATTN_NORM, 0, thePitch);

	// General x-punch axis
	if (EV_IsLocal(idx))
	{
		float theHalfSpread = kBitePunch/2.0f;
		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis(0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
			V_PunchAxis(1, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}

		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}

	if(gEngfuncs.pfnRandomLong(0, 1) == 0)
	{
		PlayMeleeHitEffects(inArgs, BALANCE_VAR(kBiteRange), kBiteHitSound1);
	}
	else
	{
		PlayMeleeHitEffects(inArgs, BALANCE_VAR(kBiteRange), kBiteHitSound2);
	}
}


void EV_Bite2(struct event_args_s* inArgs)
{
	int idx = inArgs->entindex;
	
	// Play attack sound
	char* theSoundToPlay = "";
	
	int theRandomSound = gEngfuncs.pfnRandomLong(0, 1);
	switch(theRandomSound)
	{
	case 0:
	case 1:
		theSoundToPlay = kBite2Sound;
		break;
	//case 1:
	//	theSoundToPlay = kBiteSound2;
	//	break;
	}


	const int kBasePitch = 80;
	const int kVariablePitchRange = 0x0;

	const int kBiteAdrenPitchFactor = 30;
	
	int theRandomPitch = gEngfuncs.pfnRandomLong( 0, kVariablePitchRange );
	int thePitch = kBasePitch + (inArgs->fparam1*kBiteAdrenPitchFactor) + theRandomPitch;
	gEngfuncs.pEventAPI->EV_PlaySound(idx, inArgs->origin, CHAN_WEAPON, theSoundToPlay, inArgs->fparam1, ATTN_NORM, 0, thePitch);

	// General x-punch axis
	if (EV_IsLocal(idx))
	{
//		gEngfuncs.pEventAPI->EV_WeaponAnimation(ANIM_FIRE1 + gEngfuncs.pfnRandomLong(0,2), 2);
		
		float theHalfSpread = kBitePunch/2.0f;
		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis(0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
			V_PunchAxis(1, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}

		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}

	if(gEngfuncs.pfnRandomLong(0, 1) == 0)
	{
		PlayMeleeHitEffects(inArgs, BALANCE_VAR(kBiteRange), kBiteHitSound1);
	}
	else
	{
		PlayMeleeHitEffects(inArgs, BALANCE_VAR(kBiteRange), kBiteHitSound2);
	}
}


void EV_BuildGun(struct event_args_s* inArgs)
{
	int idx = inArgs->entindex;
	
	// Play attack sound
	char* theSoundToPlay = "";
	
	int theRandomSound = gEngfuncs.pfnRandomLong(0, 1);
	switch(theRandomSound)
	{
	case 0:
		theSoundToPlay = kBuildingGunSound1;
		break;
	case 1:
		theSoundToPlay = kBuildingGunSound2;
		break;
	}
	gEngfuncs.pEventAPI->EV_PlaySound(idx, inArgs->origin, CHAN_WEAPON, theSoundToPlay, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
	// General x-punch axis
	if (EV_IsLocal(idx))
	{
//		gEngfuncs.pEventAPI->EV_WeaponAnimation(ANIM_FIRE1 + gEngfuncs.pfnRandomLong(0,2), 2);
		
		float theHalfSpread = kBuildingGunPunch/2.0f;
		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis(0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
			V_PunchAxis(1, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}
	}
}

void EV_HealingSpray(struct event_args_s* inArgs)
{
	ASSERT(inArgs);
	
	// Play attack animation and add muzzle flash
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = inArgs->entindex;
	VectorCopy( inArgs->origin, origin );
	VectorCopy( inArgs->angles, angles );
	VectorCopy( inArgs->velocity, velocity );
	
	//AngleVectors( angles, forward, right, up );
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	
	// Play attack sound
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, kHealingSpraySound, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

	// Load model
	//int theModelIndex;
	//struct model_s* theModel = gEngfuncs.CL_LoadModel(kWebProjectileSprite, &theModelIndex);
	//if(theModel)
	//{
		// Create some spray effect
		//gEngfuncs.pEfxAPI->R_Sprite_Spray(inArgs->origin, forward, theModelIndex, 15, 100, 2);
		//vec3_t theEndPoint;
		//VectorMA(inArgs->origin, kHealingSprayRange, forward, theEndPoint);
		//gEngfuncs.pEfxAPI->R_BeamLightning(inArgs->origin, theEndPoint, theModelIndex, .5f, 10, 1.0f, 1.0f, 1.0f);
	//}


	vec3_t theStartPos;
	EV_GetGunPosition(inArgs, vecSrc, inArgs->origin);
	VectorMA(vecSrc, kHealingSprayBarrelLength, forward, theStartPos);
	
	AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsBacteriaSpray, theStartPos);

	if (EV_IsLocal(inArgs->entindex))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
}

void EV_Metabolize(struct event_args_s* inArgs)
{
	char* theSoundToPlay = "";
	
	int theRandomSound = gEngfuncs.pfnRandomLong(0, 2);
	switch(theRandomSound)
	{
	case 0:
		theSoundToPlay = kMetabolizeFireSound1;
		break;
	case 1:
		theSoundToPlay = kMetabolizeFireSound2;
		break;
	case 2:
		theSoundToPlay = kMetabolizeFireSound3;
		break;
	}
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_WEAPON, theSoundToPlay, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
//	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_WEAPON, kMetabolizeFireSound, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

	if (EV_IsLocal(inArgs->entindex))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
}

void EV_MetabolizeSuccess(struct event_args_s* inArgs)
{
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_WEAPON, kMetabolizeSuccessSound, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
	if (EV_IsLocal(inArgs->entindex))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
}

void WebHit(struct tempent_s* ent, struct pmtrace_s* ptr)
{
	gEngfuncs.pEventAPI->EV_PlaySound(ent->entity.index, ptr->endpos, CHAN_AUTO, kWebStrandHitSound, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	//gEngfuncs.pEventAPI->EV_PlaySound(ent->entity.index, ptr->endpos, CHAN_AUTO, kWebSpinSound1, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	if(ptr && ent)
	{
		ent->die = gEngfuncs.GetClientTime();
		
		//AvHParticleSystemManager::Instance()->CreateParticleSystem(kpsSpitHit, ptr->endpos, &(ptr->plane.normal));
		
		// Create splat here too
		CreateDecal(ptr);
	}
}


void EV_SpinWeb(struct event_args_s* inArgs)
{
	ASSERT(inArgs);
	
	// Play attack animation and add muzzle flash
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = inArgs->entindex;
	VectorCopy( inArgs->origin, origin );
	VectorCopy( inArgs->angles, angles );
	VectorCopy( inArgs->velocity, velocity );
	
	//AngleVectors( angles, forward, right, up );
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	
	if( EV_IsLocal( idx ) )
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
	
	// Play attack sound
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, kWebSpinSound1, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
	// Fire spit glob
	int theModelIndex;
	struct model_s* theModel = gEngfuncs.CL_LoadModel(kWebProjectileSprite, &theModelIndex);
	if(theModel)
	{
		TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->CL_TempEntAlloc(gPredictedPlayerOrigin, theModel);
		if(theTempEntity)
		{
			vec3_t theStartPos, theEndPos;
			EV_GetGunPosition(inArgs, vecSrc, inArgs->origin);
			VectorMA(vecSrc, kSpitGBarrelLength, forward, theStartPos);
			
			VectorCopy(theStartPos, theTempEntity->entity.origin);
			VectorCopy(theStartPos, theTempEntity->entity.prevstate.origin);
			VectorCopy(theStartPos, theTempEntity->entity.curstate.origin);
			//theTempEntity->die += kSpitLifetime;
			theTempEntity->hitcallback = WebHit;
			theTempEntity->flags |= (FTENT_COLLIDEALL | FTENT_SPRANIMATE | FTENT_SPRANIMATELOOP/* | FTENT_PERSIST*/);
			theTempEntity->clientIndex = inArgs->entindex;	// Entity to ignore collisions with
			theTempEntity->entity.curstate.framerate = 30;
			theTempEntity->frameMax = 4;//theModel->numframes;
			
			// Temp entities interpret baseline origin as velocity.
			Vector theBaseVelocity;
			VectorScale(inArgs->velocity, kWebProjectileParentVelocityScalar, theBaseVelocity);
			
			Vector theStartVelocity;
			VectorMA(theBaseVelocity, kWebProjectileVelocity, forward, theStartVelocity);
			
			VectorCopy(theStartVelocity, theTempEntity->entity.baseline.origin);
			VectorCopy(theStartVelocity, theTempEntity->entity.baseline.velocity);
		}
	}
	
	// General x-punch axis
	if ( EV_IsLocal( idx ) )
	{
		float theHalfSpread = kWebGXPunch/2.0f;
		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis( 0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}
	}
}

//void EV_Babbler(struct event_args_s* inArgs)
//{
//	ASSERT(inArgs);
//	
//	// Play attack animation and add muzzle flash
//	int idx = inArgs->entindex;
//	
//	// Play attack sound
//	gEngfuncs.pEventAPI->EV_PlaySound(idx, inArgs->origin, CHAN_WEAPON, kBabblerGunSound, inArgs->fparam1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
//	
//	if ( EV_IsLocal( idx ) )
//	{
//		float theHalfSpread = kBabblerXPunch/2.0f;
//		if(theHalfSpread > 0.0f)
//		{
//			V_PunchAxis( 0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
//		}
//
//		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
//	}
//}



void EV_PrimalScream(struct event_args_s* inArgs)
{
	int idx = inArgs->entindex;
	
	// Play attack sound
	
	// You can hear this waaay far off
	float theFalloffFactor = .5f;
	gEngfuncs.pEventAPI->EV_PlaySound(idx, inArgs->origin, CHAN_VOICE, kPrimalScreamSound, 1.0f, theFalloffFactor, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));

	if (EV_IsLocal(inArgs->entindex))
	{
		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
}

void EV_StopScream(struct event_args_s* inArgs)
{
	int idx = inArgs->entindex;
	
	gEngfuncs.pEventAPI->EV_StopSound(idx, CHAN_VOICE, kPrimalScreamSound);
}


void EV_SpikeGun(struct event_args_s* inArgs)
{
    //static int theTracerCount[ 32 ];

	// Play attack animation and add muzzle flash
	int idx;
	vec3_t origin;
	vec3_t angles;
	vec3_t velocity;
	
	vec3_t vecSrc, vecAiming;
	vec3_t up, right, forward;
	
	idx = inArgs->entindex;
	VectorCopy( inArgs->origin, origin );
	VectorCopy( inArgs->angles, angles );
	VectorCopy( inArgs->velocity, velocity );
	
	//AngleVectors( angles, forward, right, up );
	gEngfuncs.pfnAngleVectors(angles, forward, right, up);
	
//	if ( EV_IsLocal( idx ) )
//	{
//		gEngfuncs.pEventAPI->EV_WeaponAnimation(5 + + gEngfuncs.pfnRandomLong(0,2), 2);
//	}
	
	// Play one of basic attack sounds
	float theVolume = inArgs->fparam1;
	float theAttenuation = 1.2f;
	
	char* theSoundToPlay = kSpikeFireSound;
	
	int theUpperBound = 30;
	int thePitch = 100 + (gEngfuncs.pfnRandomLong(0, theUpperBound) - theUpperBound/2);
	gEngfuncs.pEventAPI->EV_PlaySound(idx, origin, CHAN_WEAPON, theSoundToPlay, theVolume, theAttenuation, 0, thePitch);
	
	EV_GetGunPosition( inArgs, vecSrc, origin );
	VectorCopy( forward, vecAiming );

	// Create ricochet and spike hit decals
	EV_HLDM_FireBulletsPlayer( idx, forward, right, up, 1, vecSrc, vecAiming, kSpikeRange, BULLET_MONSTER_9MM, 0, NULL, kSpikeSpread, inArgs->iparam1);

//	// Create two spike projectiles, flying forward until they hit something
//	//for(int i = 0; i < 2; i++)
//	//{
//		vec3_t theEntStartPos = gPredictedPlayerOrigin;
//	
//		//TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->CL_TempEntAlloc(theEntStartPos, theModel);
//		vec3_t theSpikeAngles;
//		VectorAngles(forward, theSpikeAngles);
//		
//		TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->R_TempModel(gPredictedPlayerOrigin, forward, theSpikeAngles, 100, gEngfuncs.pEventAPI->EV_FindModelIndex(kSpikeProjectileModel), 0);
//		if(theTempEntity)
//		{
//			vec3_t theStartPos, theEndPos;
//			EV_GetGunPosition(inArgs, vecSrc, inArgs->origin);
//			VectorMA(vecSrc, kSpikeBarrelLength, forward, theStartPos);
//			
//			VectorCopy(theStartPos, theTempEntity->entity.origin);
//			VectorCopy(theStartPos, theTempEntity->entity.prevstate.origin);
//			VectorCopy(theStartPos, theTempEntity->entity.curstate.origin);
//			theTempEntity->hitcallback = SpikeHit;
//			theTempEntity->flags |= (FTENT_COLLIDEALL | FTENT_COLLIDEKILL/* | FTENT_PERSIST*/);
//			//theTempEntity->entity.curstate.framerate = 30;
//			//theTempEntity->frameMax = 4;//theModel->numframes;
//			
//			// Temp entities interpret baseline origin as velocity.
//			Vector theStartVelocity;
//			VectorScale(forward, kSpikeVelocity, theStartVelocity);
//			
//			VectorCopy(theStartVelocity, theTempEntity->entity.baseline.origin);
//			VectorCopy(theStartVelocity, theTempEntity->entity.baseline.velocity);
//
//			// Set orientation
//			//VectorCopy(inArgs->angles, theTempEntity->entity.angles);
//		}
//	//}
	
	////EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, kMGRange, BULLET_PLAYER_MP5, theTracerFreq, &tracerCount[idx-1] );
	//EV_HLDM_FireBullets( idx, forward, right, up, 1, vecSrc, vecAiming, kSpikeRange, BULLET_MONSTER_9MM, 0, NULL /* &theTracerCount[0]*/, 0.0f, 0.0f);
	
	// General x-punch axis
	if ( EV_IsLocal( idx ) )
	{
		// Multiply punch by upgrade level
		float theHalfSpread = (kSpikePunch/2.0f);
		if(theHalfSpread > 0.0f)
		{
			V_PunchAxis( 0, gEngfuncs.pfnRandomFloat( -theHalfSpread, theHalfSpread ) );
		}

		gEngfuncs.pEventAPI->EV_WeaponAnimation(inArgs->iparam2, 2);
	}
}

//void EV_HiveHit(struct event_args_s* inArgs)
//{
//	// when a hive comes under attack, let all the minions know it
//
//	// iparam1 is the hurt sound to play
//
//
//
//	const float kPunchRange = 3.0f;
//	
//	V_PunchAxis(0, gEngfuncs.pfnRandomFloat(-kPunchRange, kPunchRange));
//	V_PunchAxis(1, gEngfuncs.pfnRandomFloat(-kPunchRange, kPunchRange));
//	V_PunchAxis(2, gEngfuncs.pfnRandomFloat(-kPunchRange, kPunchRange));
//}	

void EV_EmptySound(struct event_args_s* inArgs)
{
	int thePitch = 100;
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_WEAPON, kEmptySound, .8f, ATTN_NORM, 0, thePitch);
}

void EV_NumericalInfo(struct event_args_s* inArgs)
{
	// Can't send easily send events to one team only, so only draw them for the right team
	cl_entity_s* theLocalPlayer = gEngfuncs.GetLocalPlayer();
	if(theLocalPlayer)
	{
		int theTeamNumber = inArgs->iparam2;
		if((theLocalPlayer->curstate.team == theTeamNumber) || (theTeamNumber == 0))
		{
			float theNumber = inArgs->fparam1;
			int theEventType = inArgs->iparam1;
			gHUD.AddNumericalInfoMessage(inArgs->origin, theNumber, theEventType);
		}
	}
}

void EV_InvalidAction(struct event_args_s* inArgs)
{
	int thePitch = 100;
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_WEAPON, kInvalidSound, .8f, ATTN_NORM, 0, thePitch);
}

void EV_Particle(struct event_args_s* inArgs)
{
	// Read template
	uint32 theTemplateIndex = inArgs->iparam1;
	
	// Lookup template
	const AvHParticleTemplate* theTemplate = gParticleTemplateList.GetTemplateAtIndex(theTemplateIndex);
	if(theTemplate)
	{
		string theTemplateName = theTemplate->GetName();
		AvHParticleSystemManager::Instance()->CreateParticleSystem(theTemplateName, inArgs->origin);
	}
}

void EV_DistressBeacon(struct event_args_s* inArgs)
{
	// Play distress beacon sound
	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_BODY, kDistressBeaconSound, 1, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
	// Read time before distress beacon will end
	int theDuration = inArgs->iparam1;

	// Create blinking light that will die in that time
	if(CVAR_GET_FLOAT(kvDynamicLights))
	{
		dlight_t* theLight = gEngfuncs.pEfxAPI->CL_AllocDlight(21);
		VectorCopy(inArgs->origin, theLight->origin);
		theLight->radius = 250;
		theLight->color.r = 250;
		theLight->color.g = 180;
		theLight->color.b = 180;
		
		// don't die for forseeable future
		theLight->die = gEngfuncs.GetClientTime() + theDuration;
	}
}

void EV_WeaponAnimation(struct event_args_s* inArgs)
{
	// General x-punch axis
	if(EV_IsLocal(inArgs->entindex))
	{
		int theAnimation = max(inArgs->iparam2, 0);
		gEngfuncs.pEventAPI->EV_WeaponAnimation(theAnimation, 2);
	}
}


void EV_LevelUp(struct event_args_s* inArgs)
{
	// General x-punch axis
	if(EV_IsLocal(inArgs->entindex))
	{
	}
	

	cl_entity_t* thePlayer = GetEntity(inArgs->entindex);
	// Play sound
	int theIsMarine = inArgs->iparam1;
	const char* theSound = theIsMarine ? kLevelUpMarineSound : kLevelUpAlienSound;

	float theSilenceVolumeFactor = theIsMarine ? 1.0 : AvHPlayerUpgrade::GetSilenceVolumeLevel((AvHUser3)thePlayer->curstate.iuser3, thePlayer->curstate.iuser4);

	gEngfuncs.pEventAPI->EV_PlaySound(inArgs->entindex, inArgs->origin, CHAN_VOICE, theSound, theSilenceVolumeFactor, ATTN_NORM, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
	
	// Play nice particle effect?
}
