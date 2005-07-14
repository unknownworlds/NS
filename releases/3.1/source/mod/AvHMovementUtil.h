//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHMovementUtil.h $
// $Date: 2002/10/03 18:59:04 $
//
//-------------------------------------------------------------------------------
// $Log: AvHMovementUtil.h,v $
// Revision 1.4  2002/10/03 18:59:04  Flayra
// - Refactored energy
//
// Revision 1.3  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef MOVEMENT_UTIL_H
#define MOVEMENT_UTIL_H

#include "mod/AvHBasePlayerWeaponConstants.h"
#include "mod/AvHSpecials.h"

int AvHMUGetHull(bool inIsDucking, int inUserVar);
int AvHMUGetOriginOffsetForUser3(AvHUser3 inUser3);
int AvHMUGetOriginOffsetForMessageID(AvHMessageID inMessageID);
bool AvHMUGetCanDuck(int inUser3);

bool AvHMUDeductAlienEnergy(float& ioFuser, float inNormAmount);
bool AvHMUGiveAlienEnergy(float& ioFuser, float inNormAmount);

bool AvHMUGetEnergyCost(AvHWeaponID inWeaponID, float& outEnergyCost);
float AvHMUGetWalkSpeedFactor(AvHUser3 inUser3);
bool AvHMUHasEnoughAlienEnergy(float& ioFuser, float inNormAmount);
void AvHMUUpdateAlienEnergy(float inTimePassed, int inUser3, int inUser4, float& ioFuser);
void AvHMUUpdateJetpackEnergy(bool inIsJetpacking, float theTimePassed, float& ioJetpackEnergy);

#endif