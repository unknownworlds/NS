//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHServerVariables.h $
// $Date: 2002/11/22 21:26:57 $
//
//-------------------------------------------------------------------------------
// $Log: AvHServerVariables.h,v $
// Revision 1.24  2002/11/22 21:26:57  Flayra
// - Added mp_version for All-Seeing Eye
//
// Revision 1.23  2002/11/12 18:52:16  Flayra
// - Renamed ns_ variables to mp_
//
// Revision 1.22  2002/11/12 18:45:02  Flayra
// - Added mp_logdetail
//
// Revision 1.21  2002/11/03 04:52:10  Flayra
// - Removed server variables, hard-coded them
//
// Revision 1.20  2002/10/24 21:42:19  Flayra
// - Regular update
//
// Revision 1.19  2002/10/18 22:22:29  Flayra
// - Added easter egg server variable
//
// Revision 1.18  2002/10/16 01:06:52  Flayra
// - Removed demoing variable, added serverops variable
//
// Revision 1.17  2002/10/03 19:07:27  Flayra
// - Regular update
//
// Revision 1.16  2002/09/23 22:30:16  Flayra
// - Added mp_weaponstaytime
//
// Revision 1.15  2002/09/09 20:06:07  Flayra
// - Voting variables
//
// Revision 1.14  2002/07/26 23:08:05  Flayra
// - Added mp_drawdamage variable
//
// Revision 1.13  2002/07/24 18:45:43  Flayra
// - Linux and scripting changes
//
// Revision 1.12  2002/07/23 17:24:54  Flayra
// - Regular update
//
// Revision 1.11  2002/07/08 17:16:58  Flayra
// - Regular update
//
// Revision 1.10  2002/07/01 21:45:28  Flayra
// - Removed turret range from skill.cfg (for visible building placement ranges)
//
// Revision 1.9  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_SERVERVARIABLES_H
#define AVH_SERVERVARIABLES_H

#include "common/cvardef.h"

extern cvar_t	avh_tournamentmode;

// Variables
#define kvTournamentMode			"mp_tournamentmode"
#define kvTrainingMode				"mp_trainingmode"
#define kvDrawDamage				"mp_drawdamage"
#define kvDeathMatchMode			"mp_deathmatchmode"
#define kvCountDownTime				"mp_countdowntime"
#define kvDrawInvisible				"mp_drawinvisibleentities"
#define kvAssert					"mp_assert"
#define kvNetworkMeterRate			"mp_networkmeterrate"
#define kvNetworkDebug				"mp_networkdebug"
#define kvLateJoinTime				"mp_latejointime"
#define kvLogDetail					"mp_logdetail"
#define kvServerScripts				"mp_serverscripts"
#define kvTeamSizeHandicapping		"mp_teamsizehandicapping"
#define kvTeam1DamagePercent        "mp_team1damagepercent"
#define kvTeam2DamagePercent        "mp_team2damagepercent"
#define kvTeam3DamagePercent        "mp_team3damagepercent"
#define kvTeam4DamagePercent        "mp_team4damagepercent"
#define kvSpawnInvulnerableTime		"mp_spawninvulnerabletime"
#define kvVoteCastTime				"mp_votecasttime"
#define kvVoteDownTime				"mp_votedowntime"
#define kvMinVotesNeeded			"mp_minvotesneeded"
#define kvServerOps					"mp_serverops"
#define kvLimitTeams				"mp_limitteams"
#define kvVotePercentNeeded			"mp_votepercentneeded"
#define kvAutoConcede				"mp_autoconcede"
#define kvCombatTime				"mp_combattime"
#define kvBulletCam					"mp_bulletcam"
#define kvTesting					"mp_testing"
#define kvKillDelay					"mp_killdelay"
#define kvPerformance				"mp_performance"
#define kvIronMan					"mp_ironman"
#define kvIronManTime				"mp_ironmantime"

#define kvEasterEggChance			"mp_eastereggchance"
#define kvUplink					"mp_uplink"
#define kvMapVoteRatio              "mp_mapvoteratio"

#define kvBlockScripts              "mp_blockscripts"

#endif