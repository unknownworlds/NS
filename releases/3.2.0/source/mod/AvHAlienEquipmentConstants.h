//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAlienEquipmentConstants.h$
// $Date: 2002/11/12 02:21:19 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAlienEquipmentConstants.h,v $
// Revision 1.19  2002/11/12 02:21:19  Flayra
// - Removed ancient egg class name
//
// Revision 1.18  2002/10/16 00:47:16  Flayra
// - Moved and removed some building sounds, added one for chamber death
//
// Revision 1.17  2002/09/23 22:07:30  Flayra
// - Updated to reflect 2D nature of sensory chamber detection
//
// Revision 1.16  2002/09/09 19:48:05  Flayra
// - Regular update
// - Sensory chambers detect enemies in range
//
// Revision 1.15  2002/08/31 18:01:00  Flayra
// - Work at VALVe
//
// Revision 1.14  2002/08/16 02:31:40  Flayra
// - Big balance change: all weapons reduced by 20% damage
//
// Revision 1.13  2002/08/02 21:57:53  Flayra
// - Increased alien turret speed
//
// Revision 1.12  2002/07/28 19:21:28  Flayra
// - Balance changes after/during RC4a
//
// Revision 1.11  2002/07/23 16:57:05  Flayra
// - Alien turret refactoring and fixing (the view offset in spawn() was causing them to always miss crouched players)
//
// Revision 1.10  2002/07/01 21:13:25  Flayra
// - Regular update
//
// Revision 1.9  2002/06/25 17:30:01  Flayra
// - Regular update
//
// Revision 1.8  2002/06/03 16:23:34  Flayra
// - Moved chamber firing into an event, added turret constants
//
// Revision 1.7  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef ALIEN_EQUIPMENT_CONSTANTS_H
#define ALIEN_EQUIPMENT_CONSTANTS_H

#define			kPrimalScreamResponseSound	"player/primalscreaming.wav"

#define			kEggDestroyedSound		"misc/egg_die.wav"
#define			kEggIdleSound			"misc/egg_idle.wav"
#define			kEggModel				"models/egg.mdl"
const int		kEggHitPoints =			200;

const int		kWebHitPoints =			20;
const int		kMaxWebDistance =		600;
const float		kWebThinkInterval =		.1f;
#define			kWebStrandSprite		"sprites/webstrand.spr"
const int		kWebStrandWidth =		60;
const int		kWebStrandLifetime =	50;
#define			kWebStrandBreakSound	"misc/web_break.wav"
#define			kWebStrandHardenSound	"misc/web_harden.wav"
#define			kWebStrandFormSound		"misc/web_form.wav"
#define			kWebStrandHitSound		"misc/web_hit.wav"

#define			kAlienResourceTowerModel			"models/ba_resource.mdl"

// Hive stuff
#define			kHiveModel					"models/hive.mdl"
#define			kHiveSpawnSound				"misc/hive_spawn.wav"
#define			kHiveAmbientSound			"misc/hive_ambient.wav"
#define			kHiveDeathSound				"misc/hive_death.wav"

// Movement chamber energize
#define			kAlienEnergySound			"misc/energy.wav"

// Doesn't jitter but a bit too tall
//#define			kHiveMinSize				Vector(-80, -80, -135)
//#define			kHiveMaxSize				Vector(80, 80, 50)
#define			kHiveMinSize				Vector(-80, -80, -145)
#define			kHiveMaxSize				Vector(80, 80, 50)

#define			kAlienResourceTowerSoundList		"misc/a_resource_idle"
const float		kAverageAlienUseSoundLength			= 1.1f;

#define			kwsAlienResourceTower				"alienresourcetower"
#define			kwAlienResourceTower				alienresourcetower

#define			kwsOffenseChamber					"offensechamber"
#define			kwOffenseChamber					offensechamber

#define			kwsDefenseChamber					"defensechamber"
#define			kwDefenseChamber					defensechamber

#define			kwsSensoryChamber					"sensorychamber"
#define			kwSensoryChamber					sensorychamber

#define			kwsMovementChamber					"movementchamber"
#define			kwMovementChamber					movementchamber

#define			kChamberDeploySound					"misc/alien_chamber_deploy.wav"
#define			kChamberDieSound					"misc/alien_chamber_die.wav"

#define			kSpikeProjectileClassName			"spikeprojectile"
#define			kSpikeLifetime						10

#define			kOffenseChamberModel				"models/ba_offense.mdl"
#define			kDefenseChamberModel				"models/ba_defense.mdl"
#define			kSensoryChamberModel				"models/ba_sensory.mdl"
#define			kMovementChamberModel				"models/ba_movement.mdl"

#define			kOffenseChamberEventName			"events/OffenseChamber.sc"
#define			kOffenseChamberSpikeVelocity		1500

#define			kAlienTurretFire1					"turret/aturret-1.wav"
#define			kAlienTurretDeploy					"misc/alien_chamber_deploy.wav"
#define			kAlienTurretProjectileVelocity		3000
#define			kAlienTurretSprite					"sprites/bigspit.spr"
const float		kAlienBuildingThinkInterval			= .5f;
const float		kAutoBuildScalar					= .2f;

#endif