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
// $Date: 2002/05/23 02:33:20 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSharedMovementInfo.h,v $
// Revision 1.1  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_SHAREDMOVEMENTINFO_H
#define AVH_SHAREDMOVEMENTINFO_H

#include "types.h"

class AvHPlayerMovementInfo
{
public:
				AvHPlayerMovementInfo();
				
	vec3_t		mBlinkOrigin;
	vec3_t		mBlinkDirection;
	float		mBlinkStartTime;
}	;

class AvHSharedMovementInfo
{
public:
	static	AvHSharedMovementInfo*	Instance();

	void	GetBlinkStartInfo(int inPlayerIndex, float* inOrigin, float* inDirection, float& inStartTime);
	void	SetBlinkStartInfo(int inPlayerIndex, const float* inOrigin, const float* inDirection, float inStartTime);
	
private:
	static AvHSharedMovementInfo*				sMovementInfo;

	typedef map<int, AvHPlayerMovementInfo>		PlayerMovementInfoList;
	PlayerMovementInfoList						mPlayerInfoList;

};

#endif