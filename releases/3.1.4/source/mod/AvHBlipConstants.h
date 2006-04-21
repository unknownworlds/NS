//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHBlipConstants.h $
// $Date: $
//
//-------------------------------------------------------------------------------
// $Log: $
//===============================================================================
#ifndef AVH_BLIPCONSTANTS_H
#define	AVH_BLIPCONSTANTS_H

// Make sure to change BlipStatus_* in titles.txt when changing these
#define		kNumBlipTypes						9
#define		kBlipSprite							"blip"
const int	kEnemyBlipStatus					= 0;
const int	kVulnerableEnemyBlipStatus			= 1;
const int	kFriendlyBlipStatus					= 2;
const int	kVulnerableFriendlyBlipStatus		= 3;
const int	kMarineEnemyBlipStatus				= 4;
const int	kGorgeBlipStatus					= 5;
const int	kHiveBlipStatus						= 6;
const int	kEnemyStructureBlipsStatus			= 7;
const int	kUnbuiltHiveBlipsStatus				= 8;

const int	kBaseBlipInfoOffset					= 32;

#endif