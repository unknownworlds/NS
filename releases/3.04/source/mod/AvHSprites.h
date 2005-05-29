//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHSprites.h $
// $Date: 2002/09/23 22:33:34 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSprites.h,v $
// Revision 1.17  2002/09/23 22:33:34  Flayra
// - Removed siege turret min range sprite
// - Added alien building sprites
//
// Revision 1.16  2002/09/09 20:07:18  Flayra
// - Added hiveinfo sprite
//
// Revision 1.15  2002/08/16 02:48:42  Flayra
// - Regular update
//
// Revision 1.14  2002/08/02 21:46:45  Flayra
// - Added constants for sprites
//
// Revision 1.13  2002/07/23 17:29:03  Flayra
// - Big update to hive sight and sprites
//
// Revision 1.12  2002/07/08 17:18:50  Flayra
// - Added marine upgrades sprite to be drawn in the future
//
// Revision 1.11  2002/07/01 21:24:37  Flayra
// - Removed outdated sprites, updated visible blips
//
// Revision 1.10  2002/06/25 18:19:39  Flayra
// - New alien upgrade system
//
// Revision 1.9  2002/06/03 17:00:43  Flayra
// - Help sprites moved into one animated sprite
//
// Revision 1.8  2002/05/23 02:32:57  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_SPRITES_H
#define AVH_SPRITES_H

#define		kJetpackSprite			"jetpack"

#define		kAlienEnergySprite		"a-energy"
#define		kAlienResourceSprite	"a-resources"

#define		kCombatExperienceSprite	"experience"

//#define		kAdrenSprite		"adren"
//#define		kEggSprite			"egg"
//#define		kHiveSprite			"hive"
//#define		kLifeformSprite		"alien"
#define		kAlienUpgradeSprite			"alienupgrades"
#define		kAlienUpgradeCategory		"alienupgradecategories"

#define		kOrdersSprite				"order"

#define		kAlienCursorSprite			"sprites/aliencursor.spr"

#define		kTopDownBGSprite			"sprites/topdownbg.spr"
#define		kTopDownTopHUDSprite		"sprites/topdowntop.spr"
#define		kTopDownBottomHUDSprite		"sprites/topdownbottom.spr"
#define		kMarineTopHUDSprite			"sprites/mainhud.spr"
#define		kLogoutSprite				"sprites/logout.spr"
#define		kHiveInfoSprite				"sprites/hiveinfo.spr"
#define		kHiveHealthSprite			"sprites/hivehealth.spr"
#define		kCursorsSprite				"sprites/cursors.spr"
#define		kCommandButtonSprite		"sprites/commandbutton.spr"
#define		kCommandStatusSprite		"sprites/commandstatus.spr"
#define		kCommandStatusSprite		"sprites/commandstatus.spr"
#define		kSelectAllSprite			"sprites/selectall.spr"

#define		kMarineOrderSprite			"sprites/hudorder.spr"
#define		kMarineUpgradesSprite		"sprites/upgrades.spr"

#define		kAlienBuildSprite			"sprites/ba-build.spr"
#define		kMarineBuildSprite			"sprites/b-build.spr"

#define		kAlienHealthSprite			"sprites/ba-health.spr"
#define		kMarineHealthSprite			"sprites/b-health.spr"

#define		kBuildCircleSprite			"sprites/buildcircle.spr"
//#define		kSiegeTurretSprite			"sprites/siegeturret.spr"

//#define		kOverwatchAimSprite			"sprites/overwatch-aim.spr"

#define		kMembraneSprite				"sprites/membrane.spr"
#define		kDigestingSprite			"sprites/digesting.spr"

#define		kHelpIconPrefix				"sprites/helpicons"

#define		kOverwatchCenterSprite		"sprites/overwatch-center.spr"
#define		kReticleSprite				"sprites/reticle.spr"
#define		kMarinePlayersSprite		"sprites/iplayerm.spr"
#define		kAlienPlayersSprite			"sprites/iplayera.spr"
#define		kStructuresSprite			"sprites/itech.spr"
#define		kSmallOrderSprite			"sprites/320orders.spr"

#define     kAlertSprite				"sprites/alert.spr"
#define		kCommBlipSprite				"sprites/comm-blip.spr"

#define		kWhiteSprite			    "sprites/white.spr"

#endif