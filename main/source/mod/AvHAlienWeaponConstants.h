//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAlienWeaponConstants.h$
// $Date: 2002/11/05 06:17:25 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAlienWeaponConstants.h,v $
// Revision 1.29  2002/11/05 06:17:25  Flayra
// - Balance changes
//
// Revision 1.28  2002/10/28 20:33:11  Flayra
// - Increased claw range so the onos can hit ground targets without crouching
//
// Revision 1.27  2002/10/20 02:36:14  Flayra
// - Regular update
//
// Revision 1.26  2002/10/18 22:17:03  Flayra
// - Balance
//
// Revision 1.25  2002/10/16 20:51:06  Flayra
// - Fixed problem where projectile hit player
// - Added paralysis projectile
//
// Revision 1.24  2002/10/16 00:47:56  Flayra
// - Removed unneeded bite2 sounds
// - Removed blink fail
// - Lowered blink ROF in case that was causing the spam of particle systems
//
// Revision 1.23  2002/10/03 18:38:04  Flayra
// - Toned down primal scream (rate of fire, energy acquisition, +30% damage instead of %40%)
//
// Revision 1.22  2002/09/25 20:42:26  Flayra
// - Balance changes
//
// Revision 1.21  2002/09/23 22:09:07  Flayra
// - Regular update
//
// Revision 1.20  2002/09/09 19:48:15  Flayra
// - Spit damage adjusted
//
// Revision 1.19  2002/08/31 18:01:00  Flayra
// - Work at VALVe
//
// Revision 1.18  2002/08/16 02:31:23  Flayra
// - Big balance change: all weapons reduced by 20% damage
//
// Revision 1.17  2002/08/09 00:54:56  Flayra
// - Regular update
//
// Revision 1.16  2002/07/26 23:03:08  Flayra
// - New artwork
//
// Revision 1.15  2002/07/23 16:57:52  Flayra
// - Lots of tweaks (new artwork, balance/feel)
//
// Revision 1.14  2002/07/08 16:45:24  Flayra
// - Spit gun and bites were shooting through walls, balance changes
//
// Revision 1.13  2002/07/01 21:15:02  Flayra
// - Added new alien abilities (primal scream, babblers, bilebomb, umbra)
//
// Revision 1.12  2002/06/25 17:28:55  Flayra
// - Consolidated view models, correct animations for player models, removed ancient weapons
//
// Revision 1.11  2002/06/10 19:49:37  Flayra
// - Updated with new alien view model artwork (with running anims)
//
// Revision 1.10  2002/06/03 16:25:34  Flayra
// - Removed outdated v_leap.mdl (!)
//
// Revision 1.9  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVHALIENWEAPONCONSTANTS_H
#define AVHALIENWEAPONCONSTANTS_H

#include "mod/AvHBasePlayerWeaponConstants.h"

// Alien view models
#define			kLevel1ViewModel		"models/v_lvl1.mdl"
#define			kLevel2ViewModel		"models/v_lvl2.mdl"
#define			kLevel3ViewModel		"models/v_lvl3.mdl"
#define			kLevel4ViewModel		"models/v_lvl4.mdl"
#define			kLevel5ViewModel		"models/v_lvl5.mdl"

// Spit gun constants.
const int		kSpitGRange			= 400; // range not used
const float		kSpitLifetime		= 2.0f;
const float		kSpitGXPunch		= 1.4f;
const float		kSpitParentVelocityScalar = .1f;
const int		kSpitVelocity		= 1500;
#define			kSpitGunSprite		"sprites/bigspit.spr"
#define			kSpitHitSound1		"weapons/spithit1.wav"
#define			kSpitHitSound2		"weapons/spithit2.wav"
#define			kSpitGEventName		"events/SpitGun.sc"
#define			kSpitGAnimExt		"ability1"
#define			kSpitGFireSound1	"weapons/spit-1.wav"
#define			kSpitGFireSound2	"weapons/spit-2.wav"
const int		kSpitGBarrelLength	= 20;
#define			kSpitClassName		"spitgunspit"

// The spit size must be small so that the collision results on the server
// are close to the collision results for the temp entity on the client.
const float		kSpitSize			= 1;


// Swipe constants.
const int		kSwipeRange			= 60;
const float		kSwipePunch			= 4.0;
#define			kSwipeSound1		"weapons/swipe1.wav"
#define			kSwipeSound2		"weapons/swipe2.wav"
#define			kSwipeSound3		"weapons/swipe3.wav"
#define			kSwipeSound4		"weapons/swipe4.wav"
#define			kSwipeHitSound1		"weapons/swipehit1.wav"
#define			kSwipeHitSound2		"weapons/swipehit2.wav"
#define			kSwipeKillSound		"weapons/swipekill.wav"

#define			kSwipeEventName		"events/Swipe.sc"
#define			kSwipeAnimExt		"ability1"
#define			kSwipeFireSound1	"weapons/swipe-1.wav"
#define			kSwipeFireSound2	"weapons/swipe-2.wav"
const int		kSwipeBarrelLength	= 30;

// Claws constants.
const int		kClawsRange			= 90;
const float		kClawsPunch			= 10.0;
#define			kClawsSound1		"weapons/claws1.wav"
#define			kClawsSound2		"weapons/claws2.wav"
#define			kClawsSound3		"weapons/claws3.wav"
#define			kClawsHitSound1		"weapons/clawshit1.wav"
#define			kClawsHitSound2		"weapons/clawshit2.wav"
#define			kClawsKillSound		"weapons/clawskill.wav"
#define			kClawsEventName		"events/Claws.sc"
#define			kClawsAnimExt		"ability1"
#define			kClawsFireSound1	"weapons/claws-1.wav"
#define			kClawsFireSound2	"weapons/claws-2.wav"
const int		kClawsBarrelLength	= 30;
const int		kClawsAdrenPitchFactor	= 50;

// Spores constants.
const int		kSporeRange			= 60;
const float		kSporePunch			= 2.0;

const float		kSporeParentVelocityScalar = .1f;
const int		kShootCloudVelocity	= 1100;
#define			kSporeSprite		"sprites/spore.spr"
#define			kClientSporeSprite	"sprites/spore2.spr"

#define			kSporeFireSound			"weapons/sporefire.wav"
#define			kSporeCloudSound		"weapons/sporecloud.wav"
#define			kSporeShootEventName	"events/ShootSpores.sc"
#define			kSporeCloudEventName	"events/SporeCloud.sc"
#define			kSporeAnimExt			"ability2"
const int		kSporeBarrelLength		= 30;
const int		kSporePointCost			= 2;


// Spike gun constants.
const int		kSpikeRange				= 8012;
const float		kSpikePunch				= 4.0;

#define			kSpikeSpread			VECTOR_CONE_3DEGREES
#define			kSpikeFireSound			"weapons/spikefire.wav"
#define			kSpikeProjectileModel	"models/spike.mdl"
#define			kSpikeShootEventName	"events/SpikeGun.sc"
#define			kSpikeAnimExt			"ability2"
const int		kSpikeBarrelLength		= 30;
const int		kSpikeVelocity			= 3500;
#define			kSpikeGunHitSprite		"sprites/spikehit.spr"


// Paralysis gun constants.
const int		kParalysisRange			= 8012;
const float		kParalysisPunch			= 2.0;

#define			kParalysisFireSound			"weapons/paralysisfire.wav"
#define			kParalysisHitSound			"weapons/paralysishit.wav"
#define			kParalysisShootEventName	"events/ParalysisGun.sc"
#define			kParalysisStartEventName	"events/ParalysisStart.sc"
#define			kParalysisProjectileModel	"models/paralysis.mdl"
#define			kParalysisAnimExt			"ability3"
const float		kParalysisROF				= 1.0f;
const int		kParalysisDamage			= 0;
const int		kParalysisBarrelLength		= 200;
const int		kParalysisVelocity			= 3500;

const int		kParalysisMaxClip			= 10;
const int		kParalysisStartClip			= 10;

// Bite constants.
const float		kBitePunch				= 2.5;
#define			kBiteSound				"weapons/bite.wav"
#define			kBiteHitSound1			"weapons/bitehit1.wav"
#define			kBiteHitSound2			"weapons/bitehit2.wav"
#define			kBiteKillSound			"weapons/bitekill.wav"
#define			kBiteEventName			"events/Bite.sc"
#define			kBiteAnimExt			"ability1"
#define			kBiteFireSound1			"weapons/bite-1.wav"
#define			kBiteFireSound2			"weapons/bite-2.wav"
const int		kBiteBarrelLength		= 20;

// Bite2 constants.
const int		kBite2Range				= 60;
const float		kBite2Punch				= 2.5;
#define			kBite2Sound				"weapons/bite2.wav"
#define			kBite2EventName			"events/Bite2.sc"
#define			kBite2AnimExt			"ability1"
#define			kBite2FireSound1		"weapons/bite2-1.wav"
#define			kBite2FireSound2		"weapons/bite2-2.wav"
const int		kBite2BarrelLength		= 20;

// Healing spray constants.
const int		kHealingSprayRange			= 300;
#define			kHealingSpraySound			"weapons/alien_spray.wav"
#define			kHealingSprayEventName		"events/HealingSpray.sc"
#define			kHealingSprayAnimExt		"ability2"
const int		kHealingSprayBarrelLength	= 30;

// Metabolize constants
#define			kMetabolizeEventName		"events/Metabolize.sc"
#define			kMetabolizeSuccessEventName	"events/MetabolizeSuccess.sc"
//#define			kMetabolizeFireSound		"weapons/metabolize_fire.wav"
#define			kMetabolizeFireSound1		"weapons/metabolize1.wav"
#define			kMetabolizeFireSound2		"weapons/metabolize2.wav"
#define			kMetabolizeFireSound3		"weapons/metabolize3.wav"
#define			kMetabolizeSuccessSound		"weapons/metabolize_success.wav"

// Web spinning constants.
const int		kWebSpinnerRange			= 500;
#define			kWebSpinnerShootEventName	"events/SpinWeb.sc"
#define			kWebProjectileSprite		"sprites/webprojectile.spr"
#define			kWebSpinnerAnimExt			"ability4"
#define			kWebSpinSound1				"weapons/webspin1.wav"
#define			kWebSpinSound2				"weapons/webspin2.wav"
const int		kWebSpinnerBarrelLength		= 20;
#define			kWebProjectileClassName		"webgunproj"
const float		kWebProjectileParentVelocityScalar		= .05f;
const int		kWebProjectileVelocity					= 1000;
const float		kWebGXPunch					= 2.0;

// Babbler gun constants.
const int		kBabblerGunRange			= 400;
#define			kBabblerGunSound			"player/role3_spawn1.wav"
#define			kBabblerGunEventName		"events/BabblerGun.sc"
#define			kBabblerGunAnimExt			"ability4"
//#define			kBabblerModel				"models/w_babbler.mdl"
#define			kBabblerModel				"models/player/alien1/alien1.mdl"
const float		kBabblerGunROF				= 1.0f;
const int		kBabblerGunBarrelLength		= 50;
const int		kBabblerBiteDamage			= 20;
const int		kBabblerExplodeDamage		= 40;
const float		kBabblerXPunch				= 8.0;
#define			kBabblerHunt1Sound			"player/role3_idle1.wav"
#define			kBabblerHunt2Sound			"player/role3_move1.wav"
#define			kBabblerHunt3Sound			"player/role3_move1.wav"
#define			kBabblerBiteSound			"weapons/bite.wav"
#define			kBabblerDieSound			"player/role3_die1.wav"
#define			kBabblerBlastSound			"weapons/divinewindexplode.wav"

// Primal scream constants.
#define			kPrimalScreamShootEventName	"events/PrimalScream.sc"
#define			kStopPrimalScreamSoundEvent	"events/StopScream.sc"
#define			kPrimalScreamAnimExt		"ability4"
#define			kPrimalScreamSound			"weapons/primalscream.wav"
const int		kPrimalScreamBarrelLength	= 100;

// Building gun constants.
const int		kBuildingGunRange			= 50;
const float		kBuildingGunPunch			= 2.5;
#define			kBuildingGunEventName		"events/Build.sc"
#define			kBuildingGunAnimExt			"ability6"
#define			kBuildingGunSound1			"weapons/build1.wav"
#define			kBuildingGunSound2			"weapons/build2.wav"
const float		kBuildingGunROF				= 1.0f;
const int		kBuildingGunDamage			= 50;
const int		kBuildingGunBarrelLength	= 100;
const int		kBuildingGunPointCost		= 3;

// Parasite gun constants.
const int		kParasiteRange			= 2048;
const float		kParasitePunch			= .5;

#define			kParasiteFireSound		"weapons/parasitefire.wav"
#define			kParasiteHitSound		"weapons/parasitehit.wav"
#define			kParasiteShootEventName	"events/ParasiteGun.sc"
#define			kParasiteProjectileModel	"models/parasite.mdl"
#define			kParasiteAnimExt		"ability2"
const int		kParasiteBarrelLength	= 10;
const int		kParasiteVelocity		= 2500;

// Umbra gun constants.
const int		kUmbraRange				= 8012;
const float		kUmbraPunch				= 2.0;
const int		kUmbraVelocity			= 1100;
#define			kUmbraSprite			"sprites/umbra.spr"
#define			kClientUmbraSprite		"sprites/umbra2.spr"

#define			kUmbraFireSound			"weapons/umbrafire.wav"
#define			kUmbraBlockedSound		"weapons/umbrablocked.wav"
#define			kUmbraCloudEventName	"events/UmbraCloud.sc"
#define			kUmbraShootEventName	"events/UmbraGun.sc"
#define			kUmbraAnimExt			"ability3"
const int		kUmbraBarrelLength		= 30;

// Blink gun constants.
const float		kBlinkPunch				= 2.0;

#define			kBlinkSuccessSound		"weapons/blinksuccess.wav"
#define			kBlinkEffectSuccessEventName	"events/BlinkSuccess.sc"
#define			kBlinkAnimExt			"ability2"

// DivineWind gun constants.
const int		kDivineWindRange				= 8012;
const float		kDivineWindPunch				= 2.0;

#define			kDivineWindFireSound		"weapons/divinewindfire.wav"
#define			kDivineWindExplodeSound		"weapons/divinewindexplode.wav"
#define			kDivineWindShootEventName	"events/DivineWind.sc"
#define			kDivineWindAnimExt			"ability4"

// Bile bomb
const int		kBileBombRange				= 9214;
const float		kBileBombPunch				= 8.0;

#define			kBileBombFireSound			"weapons/bilebombfire.wav"
#define			kBileBombHitSound			"weapons/bilebombhit.wav"
#define			kBileBombShootEventName		"events/BileBomb.sc"
#define			kBileBombVModel				"models/v_lvl3.mdl"
#define			kBileBombAnimExt			"ability3"
#define			kBileBombProjectileModel	"models/bilebomb.mdl"

const int		kBileBombBarrelLength		= 20;
const float		kBileBombFrictionConstant	= .8f;
const int		kBileBombVelocity			= 650;

// The bile bomb size must be small so that the collision results on the server
// are close to the collision results for the temp entity on the client.
const float		kBileBombSize				= 1;

// Acid rocket
const int		kAcidRocketRange					= 8012;
const float		kAcidRocketPunch					= 2.0;
const float		kAcidRocketParentVelocityScalar		= .1f;
const int		kAcidRocketVelocity			= 2000;
#define			kAcidRocketFireSound		"weapons/acidrocketfire.wav"
#define			kAcidRocketHitSound			"weapons/acidrockethit.wav"
#define			kAcidRocketShootEventName	"events/AcidRocket.sc"
#define			kAcidRocketVModel			"models/v_lvl4.mdl"
#define			kAcidRocketProjectileModel	"models/acidrocket.mdl"
#define			kAcidRocketAnimExt			"ability4"

const int		kAcidRocketBarrelLength		= 40;

// Devour gun constants.
const float		kDevourPunch				= 2.0;

#define			kDevourFireSound			"weapons/devour.wav"
#define			kDevourSwallowSound			"weapons/devourswallow.wav"
#define			kDevourCompleteSound		"weapons/devourcomplete.wav"
#define			kDevourCancelSound			"weapons/devourcancel.wav"
#define			kDevourShootEventName		"events/Devour.sc"
#define			kDevourAnimExt				"ability3"


// Stomp gun constants.
#define			kStompFireSound				"weapons/stomp.wav"
#define			kStompShootEventName		"events/Stomp.sc"
//#define			kStompProjectileModel		"sprites/shockwave.spr"
#define			kStompProjectileModel		"models/stomp.mdl"
#define			kwsStompProjectile			"stompprojectile"
const int		kStompBarrelLength			= 30;
const float		kStompProjectileLifetime	= 1.2f;
const int		kStompModelRenderAmount		= 180;
const int		kStompProjectileVelocity	= 600;
#define			kStompAnimExt				"ability2"


#endif