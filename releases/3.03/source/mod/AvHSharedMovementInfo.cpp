//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Data structure for keeping track of movement that occurs over multiple ticks.
//
// $Workfile:     $
// $Date: 2002/07/24 18:45:43 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSharedMovementInfo.cpp,v $
// Revision 1.2  2002/07/24 18:45:43  Flayra
// - Linux and scripting changes
//
// Revision 1.1  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifdef AVH_CLIENT
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#endif

#ifdef AVH_SERVER
#include "extdll.h"
#include "util.h"
#include "common/vector_util.h"
#endif

#include "mod/AvHSharedMovementInfo.h"


AvHPlayerMovementInfo::AvHPlayerMovementInfo()
{
	this->mBlinkStartTime = 0.0f;
}


AvHSharedMovementInfo* AvHSharedMovementInfo::sMovementInfo = NULL;

AvHSharedMovementInfo* AvHSharedMovementInfo::Instance()
{
	if(!sMovementInfo)
	{
		sMovementInfo = new AvHSharedMovementInfo();
	}
	return sMovementInfo;
}


void AvHSharedMovementInfo::GetBlinkStartInfo(int inPlayerIndex, float* inOrigin, float* inDirection, float& inStartTime)
{
	AvHPlayerMovementInfo& theMovementInfo = this->mPlayerInfoList[inPlayerIndex];

	VectorCopy(theMovementInfo.mBlinkOrigin, inOrigin);
	VectorCopy(theMovementInfo.mBlinkDirection, inDirection);
	inStartTime = theMovementInfo.mBlinkStartTime;
}

void AvHSharedMovementInfo::SetBlinkStartInfo(int inPlayerIndex, const float* inOrigin, const float* inDirection, float inStartTime)
{
	AvHPlayerMovementInfo& theMovementInfo = this->mPlayerInfoList[inPlayerIndex];

	VectorCopy(inOrigin, theMovementInfo.mBlinkOrigin);
	VectorCopy(inDirection, theMovementInfo.mBlinkDirection);
	theMovementInfo.mBlinkStartTime = inStartTime;
}

