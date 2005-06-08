//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAlienAbilityConstants.h$
// $Date: 2002/11/22 22:01:00 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAlienAbilityConstants.h,v $
// Revision 1.8  2002/11/22 22:01:00  Flayra
// - Case-sensitive problem that is now an issue for linux with mp_consistency
//
// Revision 1.7  2002/10/16 00:45:44  Flayra
// - Toned down leap and charge damage (is this frame-rate dependent?)
//
// Revision 1.6  2002/08/16 02:31:01  Flayra
// - Big balance change: all weapons reduced by 20% damage
//
// Revision 1.5  2002/06/25 17:26:29  Flayra
// - Regular update for leap and charge
//
// Revision 1.4  2002/06/03 16:20:35  Flayra
// - Removed outdated v_leap.mdl (!)
//
// Revision 1.3  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_ALIEN_ABILITY_CONSTANTS_H
#define AVH_ALIEN_ABILITY_CONSTANTS_H

// Leap constants.
const int		kLeapRange				= 600;
const float		kLeapPunch				= 2.5;
#define			kLeapSound				"weapons/leap1.wav"
#define			kLeapHitSound1			"weapons/leaphit1.wav"
#define			kLeapKillSound			"weapons/leapkill.wav"
#define			kLeapEventName			"events/Leap.sc"
#define			kLeapPModel				"models/null.mdl"
const float		kLeapROF				= 1.5f;
const float		kLeapDuration			= 1.0f;

// Charge constants.
const float		kChargePunch			= 2.5;
#define			kChargeSound			"weapons/charge1.wav"
#define			kChargeKillSound		"weapons/chargekill.wav"
#define			kChargeEventName		"events/Charge.sc"
const float		kChargeROF				= 5.0f;

#define			kAlienSightOnSound		"misc/aliensighton.wav"
#define			kAlienSightOffSound		"misc/aliensightoff.wav"

const int		kAlienCloakRenderMode			= kRenderTransTexture;
const int		kAlienCloakAmount				= 25;
// puzl: 1061   full cloaking
const int		kAlienStructureCloakAmount		= 0;
// :puzl

const int		kAlienSelfCloakingBaseOpacity	= 130;

const int		kAlienCloakViewModelRenderMode	= kRenderTransAdd;
const int		kAlienCloakViewModelAmount		= 35;
const int		kAlienCloakViewModelLevelAmount	= 10;

#endif