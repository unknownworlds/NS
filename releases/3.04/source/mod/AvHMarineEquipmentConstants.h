//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHMarineEquipmentConstants.h $
// $Date: 2002/11/05 06:17:25 $
//
//-------------------------------------------------------------------------------
// $Log: AvHMarineEquipmentConstants.h,v $
// Revision 1.42  2002/11/05 06:17:25  Flayra
// - Balance changes
//
// Revision 1.41  2002/11/03 04:50:51  Flayra
// - Hard-coded gameplay constants instead of putting in skill.cfg
//
// Revision 1.40  2002/10/25 21:48:21  Flayra
// - Fixe for wrong player model when holding mines
//
// Revision 1.39  2002/10/24 21:32:35  Flayra
// - Regular update (welders were taking down aliens and structures fairly regularly)
//
// Revision 1.38  2002/10/20 02:36:14  Flayra
// - Regular update
//
// Revision 1.37  2002/10/18 22:21:01  Flayra
// - Tweaked jetpacks to make them more useful
//
// Revision 1.36  2002/10/16 20:54:52  Flayra
// - Put back in special hive sound (needed because of cue)
//
// Revision 1.35  2002/10/16 01:01:22  Flayra
// - Removed unneeded sounds
//
// Revision 1.34  2002/10/03 18:57:38  Flayra
// - Added heavy view models
// - Tried to fix pickup item order by adding weapon sizes
//
// Revision 1.33  2002/09/23 22:21:49  Flayra
// - Added heavy armor, jetpack and advanced turret factory
//
// Revision 1.32  2002/09/09 19:59:47  Flayra
// - Regular update
//
// Revision 1.31  2002/08/31 18:01:02  Flayra
// - Work at VALVe
//
// Revision 1.30  2002/08/16 02:39:38  Flayra
// - Tweaked mine damage for new 20% less damage
// - Tweaked jetpack so it's not totally nuts
//
// Revision 1.29  2002/08/09 01:07:43  Flayra
// - Tweaked jetpack physics
// - Tweaked flight physics
//
// Revision 1.28  2002/08/02 21:54:57  Flayra
// - More flier air control
//
// Revision 1.27  2002/07/23 17:12:19  Flayra
// - Turret tweaks, regular update
//
// Revision 1.26  2002/07/01 22:41:40  Flayra
// - Removed outdated overwatch target and tension events
//
// Revision 1.25  2002/07/01 21:30:53  Flayra
// - Removed outdated commandpoints event, added siege turret shockwave, tweaked ranges and damage
//
// Revision 1.24  2002/06/25 18:05:34  Flayra
// - Renamed some buildings, armory is now upgraded to advanced armory, fixed mine ROF (why doesn't the compiler catch this?), updated player animations
//
// Revision 1.23  2002/06/03 16:51:03  Flayra
// - Renamed weapons factory and armory, added ammo resupplying
//
// Revision 1.22  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_MARINE_EQUIPMENT_CONSTANTS_H
#define AVH_MARINE_EQUIPMENT_CONSTANTS_H

// Jetpack
const float kJetpackForce					= 1250;
const float	kJetpackLateralScalar			= 12;
const float kJetpackEnergyGainRate			= .1f;
const float kJetpackMinimumEnergyToJetpack	= .1f;
const float kJetpackEnergyLossRate			= kJetpackEnergyGainRate*1.5f;

// Welder constants
#define			kWelderVModel				"models/v_welder.mdl"
#define			kWelderHVVModel				"models/v_welder_hv.mdl"
#define			kWelderWModel				"models/w_welder.mdl"
#define			kWelderPModel				"models/p_welder.mdl"

#define			kWeldingSound				"weapons/welderidle.wav"
#define			kWeldingHitSound			"weapons/welderhit.wav"
#define			kWeldingStopSound			"weapons/welderstop.wav"

#define			kwsWelder					"weapon_welder"
#define			kwWelder					weapon_welder
#define			kWelderEventName			"events/Welder.sc"
#define			kWelderStartEventName		"events/WelderStart.sc"
#define			kWelderEndEventName			"events/WelderEnd.sc"
#define			kWelderConstEventName		"events/WelderConst.sc"
const float		kWelderThinkTime =			3.0f;
const int		kWelderBarrelLength =		10;

#define			kOverwatchStartSound		"misc/overwatchstart.wav"
#define			kOverwatchEndSound			"misc/overwatchend.wav"

#define			kRegenerationSound			"misc/regeneration.wav"
#define			kStartCloakSound			"misc/startcloak.wav"
#define			kEndCloakSound				"misc/endcloak.wav"

//#define			kWallJumpSound				"player/walljump.wav"
#define			kWingFlapSound1				"player/wingflap1.wav"
#define			kWingFlapSound2				"player/wingflap2.wav"
#define			kWingFlapSound3				"player/wingflap3.wav"
const float		kWingFlapLateralScalar		= 215;
const float		kWingFlapZVelocityScalar	= 1.0f;
const float     kWingThrustForwardScalar    = .85f;
const float     kWingThrustBackwardScalar   = .25f;

const int		kMineBarrellLength			= 30;
const int		kMineRange					= 128;
const float		kMineROF					= 1.0f;
#define			kTripmineVModel				"models/v_mine.mdl"
#define			kTripmineHVVModel			"models/v_mine_hv.mdl"
#define			kTripmineWModel				"models/w_mine.mdl"
#define			kTripmineW2Model			"models/w_mine2.mdl"
#define			kTripminePModel				"models/p_mine.mdl"
#define			kTripmineDeploySound		"weapons/mine_deploy.wav"
#define			kTripmineActivateSound		"weapons/mine_activate.wav"
#define			kTripmineChargeSound		"weapons/mine_charge.wav"
#define			kTripmineStepOnSound		"weapons/mine_step.wav"

#define			kMineMinSize				Vector( -8, -8, 0)
#define			kMineMaxSize				Vector(8, 8, 2)

#define			kDeployedTurretModel		"models/b_sentry.mdl"

#define			kTurretFire1				"turret/turret-1.wav"
#define			kTurretFire2				"turret/turret-2.wav"
#define			kTurretFire3				"turret/turret-3.wav"
#define			kTurretFire4				"turret/turret-4.wav"
#define			kTurretPing					"turret/turret_ping.wav"
#define			kTurretDeploy				"turret/turret_deploy.wav"

// Note, these must be concentric and only vary with the number
// If these change, change kMarineConstructionSounds below also
#define			kTurretBuild1				"misc/build1.wav"
#define			kTurretBuild2				"misc/build2.wav"
#define			kTurretBuild3				"misc/build3.wav"
#define			kTurretBuild4				"misc/build4.wav"
#define			kTurretBuild5				"misc/build5.wav"

// Construction sounds
#define			kMarineConstructionSoundList	"misc/build"
#define			kAlienConstructionSoundList		"misc/a-build"
#define			kMarineBuildingDeploy		"misc/b_marine_deploy.wav"
#define			kMarineBuildingKilled		"misc/b_marine_killed.wav"

// Electrical sounds
#define			kElectricSparkSoundList		"misc/elecspark"

// Resource stuff
#define			kResourceTowerDeploySound	"misc/res_deploy.wav"
#define			kFuncResourceMinSize		Vector(-16.0, -16.0, 0.0)
#define			kFuncResourceMaxSize		Vector(16.0, 16.0, .1)

// Health
#define			kHealthModel				"models/w_health.mdl"
#define			kHealthPickupSound			"items/health.wav"
#define			kHealthMinSize				Vector(-16, -16, 0)
#define			kHealthMaxSize				Vector(16, 16, 4)

// Catalyst
#define			kCatalystModel				"models/w_catalyst.mdl"
#define			kCatalystPickupSound		"items/catalyst.wav"
#define			kCatalystMinSize			Vector(-16, -16, 0)
#define			kCatalystMaxSize			Vector(16, 16, 4)

// Heavy armor
#define			kHeavyModel					"models/w_heavy.mdl"
#define			kHeavyPickupSound			"items/pickup_heavy.wav"
#define			kHeavyMinSize				Vector(-16, -16, 0)
#define			kHeavyMaxSize				Vector(16, 16, 28)

// Jetpack
#define			kJetpackModel				"models/w_jetpack.mdl"
#define			kJetpackPickupSound			"items/pickup_jetpack.wav"
#define			kJetpackMinSize				Vector(-16, -16, 0)
#define			kJetpackMaxSize				Vector(16, 16, 28)

// Ammo
#define			kAmmoModel					"models/w_ammo.mdl"
#define			kAmmoPickupSound			"items/9mmclip1.wav"
#define			kAmmoMinSize				Vector(-16, -16, 0)
#define			kAmmoMaxSize				Vector(16, 16, 28)

// Ammo
#define			kAmmoPackModel  				"models/w_ammopack.mdl"
#define			kAmmoPackPickupSound			"items/9mmclip1.wav"
#define			kAmmoPackMinSize				Vector(-16, -16, 0)
#define			kAmmoPackMaxSize				Vector(16, 16, 24)

// Phase gate!
#define			kwPhaseGate					phasegate
#define			kwsPhaseGate				"phasegate"
#define			kPhaseGateModel				"models/b_phasegate.mdl"
#define			kPhaseGateSound				"misc/phasegate.wav"
#define			kPhaseGateTransportSound	"misc/transport2.wav"
#define			kPhaseInSound				"misc/phasein.wav"
const			float						kPhaseGateAmnestyTime = 2.0f;


// Scan!
#define			kwScan						scan
#define			kwsScan						"scan"
#define			kScanModel					"models/b_scan.mdl"
#define			kScanSound					"misc/scan.wav"

// Siege turret
#define			kwsSiegeTurret				"siegeturret"
#define			kwSiegeTurret				siegeturret
const int		kSiegeSplashRadius			= 400;

// Note: update siege turret selected sprite if changing this
const float		kSiegeTurretMinRangeScalar	= .1f;

#define			kSiegeTurretModel			"models/b_siege.mdl"
#define			kSiegeTurretShockWave		"sprites/shockwave.spr"
#define			kSiegeTurretFire1			"turret/st_fire1.wav"
#define			kSiegeHitSound1				"turret/siegehit1.wav"
#define			kSiegeHitSound2				"turret/siegehit2.wav"
#define			kSiegePing					"turret/siege_ping.wav"
#define			kSiegeDeploy				"turret/siege_deploy.wav"

// Nuke
#define kwsNuke								"nuke"
#define kwNuke								nuke
#define	kNukeDeploy							"misc/b_nuke_deploy.wav"
#define	kNukeKilled							"misc/b_nuke_killed.wav"
#define	kNukeActive							"misc/b_nuke_active.wav"
#define	kNukeExplode						"misc/b_nuke_explode.wav"
#define kNukeModel							"models/b_nuke.mdl"
const float kNukeDamage =					4000;
const float kNukeRange =					1000;

// Special building class names
#define	kwsInfantryPortal			"team_infportal"
#define	kwInfantryPortal			team_infportal

#define kwTeamCommand				team_command
#define kwsTeamCommand				"team_command"

#define kwsTurretFactory			"team_turretfactory"
#define kwTurretFactory				team_turretfactory

#define kwsAdvancedTurretFactory	"team_advturretfactory"
#define kwAdvancedTurretFactory		team_advturretfactory

#define kwsArmory					"team_armory"
#define kwArmory					team_armory

#define kwsAdvancedArmory			"team_advarmory"
#define kwAdvancedArmory			team_advarmory

#define kwsArmsLab					"team_armslab"
#define kwArmsLab					team_armslab

#define kwsPrototypeLab				"team_prototypelab"
#define kwPrototypeLab				team_prototypelab

#define kwsObservatory				"team_observatory"
#define kwObservatory				team_observatory

#define kwsChemlab					"team_chemlab"
#define kwChemlab					team_chemlab

#define kwsMedlab					"team_medlab"
#define kwMedlab					team_medlab

#define	kwsNukePlant				"team_nukeplant"
#define	kwNukePlant					team_nukeplant

// Resource tower								
#define			kwsResourceTower				"resourcetower"
#define			kwResourceTower					resourcetower
#define			kResourceTowerModel				"models/b_resourcetower.mdl"
#define			kResourceTowerSoundList			"misc/resource_idle"
#define			kResourceTowerMinSoundInterval	1.5f
const int		kResourceTowerSightRange	=	500;

#define			kMarineItemMinSize				Vector(-16, -16, 0)
#define			kMarineItemMaxSize				Vector(16, 16, 42) 

// Command station constants
const float		kCommandStationAttenuation		= 2.0f;
const float		kCommandStationThinkInterval	= .1f;
#define			kCommandStationModel			"models/b_commandstation.mdl"
#define			kCommandStationStartSound		"misc/commstat-start.wav"
#define			kCommandStationEndSound			"misc/commstat-end.wav"
#define			kCommandStationDeathSound		"misc/commstat-die.wav"

#define			kInfantryPortalModel			"models/b_infportal.mdl"
#define			kTransportSound					"misc/transport.wav"

#define			kTurretFactoryModel				"models/b_turretfactory.mdl"
#define			kArmoryModel					"models/b_armory.mdl"
#define			kAdvancedWeaponFactoryModel		"models/b_advweaponfactory.mdl"
#define			kArmsLabModel					"models/b_armslab.mdl"
#define			kPrototypeLabModel				"models/b_prototypelab.mdl"
#define			kObservatoryModel				"models/b_observatory.mdl"
//#define			kNukePlantModel					"models/b_nukeplant.mdl"

// Misc build constants
#define			kArmoryResupplySound			"misc/resupply.wav"

#endif
