//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Constants for the particle system, also named particles from ns.ps
//
// $Workfile: AvHParticleConstants.h $
// $Date: 2002/10/24 21:34:09 $
//
//-------------------------------------------------------------------------------
// $Log: AvHParticleConstants.h,v $
// Revision 1.12  2002/10/24 21:34:09  Flayra
// - Regular update
//
// Revision 1.11  2002/10/16 01:02:25  Flayra
// - New effects!
//
// Revision 1.10  2002/09/25 20:49:12  Flayra
// - New bacterial spray effect
//
// Revision 1.9  2002/09/23 22:24:04  Flayra
// - Regular update
//
// Revision 1.8  2002/06/25 18:09:00  Flayra
// - Added umbra effect
//
// Revision 1.7  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_PARTICLE_CONSTANTS_H
#define AVH_PARTICLE_CONSTANTS_H

#define	kpscSystemName				"pSystemName"
#define kpscGenSource				"pGenSource"
#define kpscGenShape				"pGenShape"
#define kpscSprite					"pSprite"
#define kpscSpriteNumFrames			"pSpriteNumFrames"
#define kpscGenShapeParams			"pGenShapeParams"
#define kpscNumParticles			"pNumParticles"
#define kpscGenRate					"pGenRate"
#define kpscSize					"pSize"
#define kpscSystemLifetime			"pSystemLifetime"
#define kpscParticleLifetime		"pLifetime"
#define kpscVelocityShape			"pVelShape"
#define kpscVelocityParams			"pVelParams"
#define kpscScale					"pScale"
#define kpscRendermode				"pRenderMode"
#define kpscMaxAlpha				"pMaxAlpha"
#define kpscSystemToGen				"pPSToGen"
#define kpscSpawnFlags				"pSpawnFlags"

// Not in .fgd, only in .ps.  Move into .fgd?
//#define kpscBaseColor				"pBaseColor"
#define kpscAnimationSpeed			"pAnimationSpeed"

// Pre-defined particle systems
#define kpsShotgun					"ShotgunSmoke"
#define kpsWelderSmoke				"WelderLightSmoke"
#define kpsWelderBluePlasma			"WelderBluePlasma"
#define kpsWelderBluePlasmaDrops	"WelderBluePlasmaDrops"
#define kpsPhaseIn					"PhaseInEffect"
#define kpsTeleport					"TeleportEffect"
#define kpsSpitShoot				"SpitShoot"
#define kpsSpitHit					"SpitHit"
#define kpsSporeShoot				"SporeShoot"
#define kpsSporeCloud				"SporeCloud"
#define kpsUmbraCloud				"UmbraCloud"
#define kpsMeleeDamage				"MeleeDamage"
#define kpsPhaseGateIdle			"PhaseGateIdle"
#define kpsResourceEmission			"ResourceEmission"
#define kpsSpikeHitEffect			"SpikeHit"
#define kpsAcidHitEffect			"AcidHit"
#define kpsBacteriaSpray			"BacteriaSpray"
#define kpsXenocide					"Xenocide"
#define kpsBilebomb					"BileBomb"
#define kpsCommandHack				"CommandHack"
#define kpsScanEffect				"ScanEffect"
#define kpsJetpackEffect			"JetpackEffect"
#define kpsSmokePuffs				"SmokePuffs"
#define kpsChamberDeath				"ChamberDeath"
#define kpsHiveDeath				"HiveDeath"
#define kpsStompEffect				"StompEffect"
#define kpsPheromoneEffect			"PheromoneEffect"
#define kpsBuildableLightDamage		"BuildableLightDamage"
#define kpsBuildableHeavyDamage		"BuildableHeavyDamage"
#define kpsStompSmoke				"StompSmoke"

#endif