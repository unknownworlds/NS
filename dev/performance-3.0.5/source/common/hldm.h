//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: hldm.h $
// $Date: 2002/08/02 21:42:34 $
//
//-------------------------------------------------------------------------------
// $Log: hldm.h,v $
// Revision 1.4  2002/08/02 21:42:34  Flayra
// - Allow ability to control how often a ricochet sound plays
//
// Revision 1.3  2002/07/08 16:17:46  Flayra
// - Reworked bullet firing to add random spread (bug #236)
//
//===============================================================================
#ifndef HLDM_H
#define HLDM_H

#include "common/pmtrace.h"

void EV_HLDM_GunshotDecalTrace( pmtrace_t *pTrace, char *decalName, int inChanceOfSound = 1);
void EV_HLDM_DecalGunshot( pmtrace_t *pTrace, int iBulletType, int inChanceOfSound = 1);
int EV_HLDM_CheckTracer( int idx, float *vecSrc, float *end, float *forward, float *right, int iBulletType, int iTracerFreq, int *tracerCount );
void EV_HLDM_FireBullets( int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, float flSpreadX, float flSpreadY );
void EV_HLDM_FireBulletsPlayer( int idx, float *forward, float *right, float *up, int cShots, float *vecSrc, float *vecDirShooting, float flDistance, int iBulletType, int iTracerFreq, int *tracerCount, Vector& inSpread, int inRandomSeed);

#endif