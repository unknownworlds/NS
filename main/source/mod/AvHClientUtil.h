//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHClientUtil.h $
// $Date: 2002/07/23 16:59:41 $
//
//-------------------------------------------------------------------------------
// $Log: AvHClientUtil.h,v $
// Revision 1.3  2002/07/23 16:59:41  Flayra
// - AvHCUWorldToScreen now returns true if the position is in front of player
//
// Revision 1.2  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_CLIENTUTIL_H
#define AVH_CLIENTUTIL_H

#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#include "cl_dll/ammo.h"
#include "cl_dll/chudmisc.h"
#include "util/nowarnings.h"
#include "common/cl_entity.h"
#include "mod/AvHConstants.h"
#include "mod/AvHMessage.h"
#include "mod/AvHSpecials.h"

int AvHCUGetIconHeightForPlayer(AvHUser3 theUser3);
void AvHCUGetViewAngles(cl_entity_t* inEntity, float* outViewAngles);
void AvHCUGetViewOrigin(cl_entity_t* inEntity, float* outViewOrigin);
bool AvHCUWorldToScreen(float* inWorldCoords, float* outScreenCoords);
bool AvHCUGetIsEntityInPVSAndVisible(int inEntityIndex);
void AvHCUTrimExtraneousLocationText(string& ioTranslatedString);

#endif
