//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHEvents.h$
// $Date: 2002/10/24 21:23:44 $
//
//-------------------------------------------------------------------------------
// $Log: AvHEvents.h,v $
// Revision 1.11  2002/10/24 21:23:44  Flayra
// - Reworked jetpacks
//
// Revision 1.10  2002/09/09 19:50:35  Flayra
// - Reworking jetpack effects, still needs more work
//
// Revision 1.9  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVHEVENTS_H
#define AVHEVENTS_H

#include "mod/AvHConstants.h"
#include "particles/p_vector.h"

void DrawBlips(const pVector& inView);
void DrawDebugEffects();
void DrawMarineLights(const pVector& inView);
void DrawOrdersForPlayers(EntityListType& inPlayerList);
void DrawRangeIndicator();
//void EndJetpackEffects(int inIndex = -1);
//void UpdateJetpackLights();

#endif