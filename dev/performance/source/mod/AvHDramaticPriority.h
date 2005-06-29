//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHDramaticPriority.h $
// $Date: 2002/11/12 02:23:10 $
//
//-------------------------------------------------------------------------------
// $Log: AvHDramaticPriority.h,v $
// Revision 1.4  2002/11/12 02:23:10  Flayra
// - New priorities for HLTV
//
// Revision 1.3  2002/05/23 02:33:41  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_DRAMATIC_PRIORITY_H
#define AVH_DRAMATIC_PRIORITY_H

// Defined in SDK docs
const int kMinDramaticPriority = 1;
const int kMaxDramaticPriority = 15;

const int kGestateDeathPriority = 6;
const int kGorgeDeathPriority = 6;
const int kLerkDeathPriority = 8;
const int kFadeDeathPriority = 10;
const int kOnosDeathPriority = 13;
const int kHeavyDeathPriority = 9;

const int kCocoonPriority = 14;

const int kReinforcementsPriority = 6;
const int kHiveSpawnPriority = 6;
const int kEggSpawnPriority = 6;
const int kJoinTeamPriority = 15;
const int kCCDeathPriority = 12;
const int kIPDeathPriority = 10;
const int kPGDeathPriority = 8;
const int kCCEjectPriority = 5;
const int kCCNewCommanderPriority = 7;

const int kMineExplodePriority = 8;
const int kMinePlacePriority = 4;

const int kEvolveLevelOnePriority = 3;
const int kEvolveLevelTwoPriority = 4;
const int kEvolveLevelThreePriority = 5;
const int kEvolveLevelFourPriority = 6;
const int kEvolveLevelFivePriority = 7;

const int kEvolveUpgradePriority = 2;

#endif