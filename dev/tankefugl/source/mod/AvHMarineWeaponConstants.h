//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHMarineWeaponConstants.h $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHMarineWeaponConstants.h,v $
// Revision 1.32  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.31  2002/11/06 01:39:35  Flayra
// - Regular update
//
// Revision 1.30  2002/11/05 06:17:26  Flayra
// - Balance changes
//
// Revision 1.29  2002/10/24 21:33:31  Flayra
// - Regular update
//
// Revision 1.28  2002/10/20 02:36:14  Flayra
// - Regular update
//
// Revision 1.27  2002/10/16 01:01:38  Flayra
// - Lowered shotty range
//
// Revision 1.26  2002/10/07 17:49:31  Flayra
// - Shotty balance
//
// Revision 1.25  2002/10/03 18:58:41  Flayra
// - Added heavy view models
// - Grenade clip is now 6 instead of 4
//
// Revision 1.24  2002/09/23 22:22:39  Flayra
// - Regular update
//
// Revision 1.23  2002/09/09 20:00:02  Flayra
// - Balance changes
//
// Revision 1.22  2002/08/31 18:01:02  Flayra
// - Work at VALVe
//
// Revision 1.20  2002/08/09 01:08:00  Flayra
// - Lowered shotgun ROF to match animations
// - Constants for shotgun sounds
//
// Revision 1.19  2002/08/02 21:53:56  Flayra
// - Balance
//
// Revision 1.18  2002/07/28 19:21:28  Flayra
// - Balance changes after/during RC4a
//
// Revision 1.17  2002/07/23 17:13:54  Flayra
// - Marine weapons given back their mojo
//
// Revision 1.16  2002/07/08 17:10:22  Flayra
// - Balance tweaks, bullet spread
//
// Revision 1.15  2002/07/01 22:41:29  Flayra
// - Moved grenade damage to a constant
//
// Revision 1.14  2002/07/01 21:37:50  Flayra
// - Balance changes, removed outdated grenade constants
//
// Revision 1.13  2002/06/25 18:07:13  Flayra
// - Updated player animations, balance changes
//
// Revision 1.12  2002/06/10 19:58:45  Flayra
// - MG clip size increased to 50, grenades toned down
//
// Revision 1.11  2002/06/03 16:51:32  Flayra
// - Regular update
//
// Revision 1.10  2002/05/28 17:53:33  Flayra
// - Removed extra knife sounds, tweaked HMG ROF for sound purposes, added marine deploy weapon volume so our sounds are normalized
//
// Revision 1.9  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
// Revision 1.10  2002/05/14 19:49:33  Charlie
// - Pistol damage tweaked
//
// Revision 1.9  2002/05/01 02:17:05  Charlie
// - Tweaked LMG
//
//===============================================================================
#ifndef AVHMARINEWEAPONCONSTANTS_H
#define AVHMARINEWEAPONCONSTANTS_H

#include "mod/AvHBasePlayerWeaponConstants.h"

// I would have much preferred they weren't all #defined outside of the class
// like this but the precaching issues that assume constant string addresses are getting to be too much.

// Knife constants.
const int		kKNRange			= 35;
const float		kKNXPunch			= .75f;
#define			kKNEventName		"events/Knife.sc"
#define			kKNVModel			"models/v_kn.mdl"
#define			kKNHVVModel			"models/v_kn_hv.mdl"
#define			kKNPModel			"models/p_kn.mdl"
#define			kKNWModel			"models/w_kn.mdl"
#define			kKNAnimExt			"knife"
#define			kKNFireSound1		"weapons/kn-1.wav"
#define			kKNFireSound2		"weapons/kn-2.wav"
#define			kKNHitSound1		"weapons/kn-hit-1.wav"
#define			kKNHitSound2		"weapons/kn-hit-2.wav"
#define			kKNHitWallSound		"weapons/kn-hit-wall.wav"
#define			kKNDeploySound		"weapons/kn-deploy.wav"
const int		kKNBarrelLength		= 15;

// Machine gun constants.
const int		kMGRange			= 8192;
const float		kMGXPunch			= 1.8f;
#define			kMGEjectModel		"models/pshell.mdl"
#define			kMGEventName		"events/MachineGun.sc"
#define			kMGVModel			"models/v_mg.mdl"
#define			kMGHVVModel			"models/v_mg_hv.mdl"
#define			kMGWModel			"models/w_mg.mdl"
#define			kMGPModel			"models/p_mg.mdl"
#define			kMGFireSound1		"weapons/mg-1.wav"
#define			kMGFireSound2		"weapons/mg-2.wav"
#define			kMGFireSound3		"weapons/mg-3.wav"
#define			kMGFireSound4		"weapons/mg-4.wav"
#define			kMGReloadSound		"weapons/g_reload.wav"
#define			kMGDeploySound		"weapons/mg-deploy.wav"
const int		kMGBarrelLength		= 25;
#define			kMGSpread			VECTOR_CONE_4DEGREES

// Pistol constants.
const int		kHGRange			= 8192;
const float		kHGXPunch			= 1.8f;
#define			kHGEjectModel		"models/shell.mdl"
#define			kHGEventName		"events/Pistol.sc"
#define			kHGVModel			"models/v_hg.mdl"
#define			kHGHVVModel			"models/v_hg_hv.mdl"
#define			kHGWModel			"models/w_hg.mdl"
#define			kHGPModel			"models/p_hg.mdl"
#define			kHGFireSound1		"weapons/hg-1.wav"
#define			kHGFireSound2		"weapons/hg-2.wav"
#define			kHGFireSound3		"weapons/hg-3.wav"
#define			kHGFireSound4		"weapons/hg-4.wav"
#define			kHGReloadSound		"weapons/g_reload.wav"
#define			kHGDeploySound		"weapons/hg-deploy.wav"
const int		kHGBarrelLength		= 10;
#define			kHGSpread			VECTOR_CONE_1DEGREES

// Sonic/Shot gun constants.
const int		kSGRange			= 700;
const float		kSGXPunch			= .8f;
#define			kSGEjectModel		"models/shotshell.mdl"
#define			kSGEventName		"events/SonicGun.sc"
#define			kSGVModel			"models/v_sg.mdl"
#define			kSGHVVModel			"models/v_sg_hv.mdl"
#define			kSGWModel			"models/w_sg.mdl"
#define			kSGPModel			"models/p_sg.mdl"
#define			kSGFireSound1		"weapons/sg-1.wav"
#define			kSGFireSound2		"weapons/sg-2.wav"
#define			kSGFireSound3		"weapons/sg-3.wav"
#define			kSGFireSound4		"weapons/sg-4.wav"
#define			kSGReloadSound		"weapons/g_reload.wav"
#define			kSGCockSound		"weapons/sg-cock.wav"
#define			kSGDeploySound		"weapons/sg-deploy.wav"
const int		kSGBarrelLength		= 25;
#define			kSGSpread			VECTOR_CONE_20DEGREES

// Heavy machine gun
const int		kHMGRange			= 6000;
const float		kHMGXPunch			= 2;
#define			kHMGEjectModel		"models/pshell.mdl"
#define			kHMGEventName		"events/HeavyMachineGun.sc"
#define			kHMGVModel			"models/v_hmg.mdl"
#define			kHMGHVVModel		"models/v_hmg_hv.mdl"
#define			kHMGWModel			"models/w_hmg.mdl"
#define			kHMGPModel			"models/p_hmg.mdl"
#define			kHMGFireSound1		"weapons/hmg-1.wav"
#define			kHMGFireSound2		"weapons/hmg-2.wav"
#define			kHMGFireSound3		"weapons/hmg-3.wav"
#define			kHMGFireSound4		"weapons/hmg-4.wav"
#define			kHMGReloadSound		"weapons/g_reload.wav"
#define			kHMGDeploySound		"weapons/hmg-deploy.wav"
const int		kHMGBarrelLength	= 25;
#define			kHMGSpread			VECTOR_CONE_8DEGREES

// Grenade gun
const int		kGGRange			= 2000;
const float		kGGXPunch			= .6f;
#define			kGGAmmoModel		"models/w_grenade.mdl"
#define			kGGEjectModel		"models/grenade.mdl"
#define			kGGEventName		"events/GrenadeGun.sc"
#define			kGGVModel			"models/v_gg.mdl"
#define			kGGHVVModel			"models/v_gg_hv.mdl"
#define			kGGWModel			"models/w_gg.mdl"
#define			kGGPModel			"models/p_gg.mdl"
#define			kGrenadeBounceSound1	"weapons/grenade_hit1.wav"
#define			kGrenadeBounceSound2	"weapons/grenade_hit2.wav"
#define			kGrenadeBounceSound3	"weapons/grenade_hit3.wav"
#define			kGGFireSound1		"weapons/gg-1.wav"
#define			kGGFireSound2		"weapons/gg-2.wav"
#define			kGGFireSound3		"weapons/gg-3.wav"
#define			kGGFireSound4		"weapons/gg-4.wav"
#define			kGGReloadSound		"weapons/g_reload.wav"
#define			kGGDeploySound		"weapons/gg-deploy.wav"
const int		kGGBarrelLength		= 25;

// Grenade constants.
const int		kGRRange			= 35;
const float		kGRXPunch			= .75f;
#define			kGREventName		"events/Grenade.sc"
#define			kGRVModel			"models/v_gr.mdl"
#define			kGRHVVModel			"models/v_gr_hv.mdl"
#define			kGRPModel			"models/p_gr.mdl"
#define			kGRWModel			"models/w_gr.mdl"
#define			kGRAnimExt			"grenade"
#define			kGRFireSound1		"weapons/grenade_throw.wav"
#define			kGRDeploySound		"weapons/grenade_draw.wav"
#define			kGRExplodeSound     "weapons/grenade_explode.wav"
#define			kGRHitSound         "weapons/grenade_hit.wav"
#define			kGRPrimeSound		"weapons/grenade_prime.wav"
const int		kGRBarrelLength		= 15;

// Misc.
const float		kDeployMarineWeaponVolume	= .3f;

#endif