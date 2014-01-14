//
//  Copyright (c) 1999, Valve LLC. All rights reserved.
//  
//  This product contains software technology licensed from Id 
//  Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
//  All Rights Reserved.
//
//  Use, distribution, and modification of this source code and/or resulting
//  object code is restricted to non-commercial enhancements to products from
//  Valve LLC.  All other use, distribution, or modification is prohibited
//  without written permission from Valve LLC.
//
//
// $Workfile: game.cpp $
// $Date: 2002/11/22 21:11:38 $
//
//-------------------------------------------------------------------------------
// $Log: game.cpp,v $
// Revision 1.37  2002/11/22 21:11:38  Flayra
// - Changed default timelimit to 60
// - Added mp_version for All-Seeing Eye
//
// Revision 1.36  2002/11/15 04:30:08  Flayra
// - Changed mp_authicons default to 1
//
// Revision 1.35  2002/11/12 18:52:32  Flayra
// - Added mp_logdetail
//
// Revision 1.34  2002/11/03 04:53:59  Flayra
// - Removed variables, hard-coded instead
//
// Revision 1.33  2002/10/24 21:17:02  Flayra
// - Added authicons variable
//
// Revision 1.32  2002/10/18 22:15:24  Flayra
// - Added easter eggs
//
// Revision 1.31  2002/10/16 00:39:09  Flayra
// - Removed demoing variable, added serverops variable
//
// Revision 1.30  2002/10/04 18:02:52  Flayra
// - Regular update
//
// Revision 1.29  2002/10/03 18:26:19  Flayra
// - Made voting percentage and min votes server variables (mp_votepercentneeded and mp_minvotesneeded)
//
// Revision 1.28  2002/09/23 22:01:23  Flayra
// - Regular update
//
// Revision 1.27  2002/09/09 19:44:16  Flayra
// - Added marine respawn time back in
// - Added vote time variable
//
// Revision 1.26  2002/08/09 00:15:54  Flayra
// - Regular update
//
// Revision 1.25  2002/07/26 23:00:53  Flayra
// - Added mp_drawdamage variable
//
// Revision 1.24  2002/07/24 18:46:19  Flayra
// - Linux and scripting changes
//
// Revision 1.23  2002/07/23 16:46:57  Flayra
// - Regular update
//
// Revision 1.22  2002/07/08 16:27:57  Flayra
// - Regular update, added document header
//
//===============================================================================
#include "extdll.h"
#include "engine/eiface.h"
#include "util.h"
#include "game.h"
#include "mod/AvHServerVariables.h"
#include "mod/AvHServerUtil.h"

cvar_t	displaysoundlist = {"displaysoundlist","0"};

// multiplayer server rules
cvar_t	fragsleft	= {"mp_fragsleft","0", FCVAR_SERVER | FCVAR_UNLOGGED };	  // Don't spam console/log files/users with this changing
cvar_t	timeleft	= {"mp_timeleft","0" , FCVAR_SERVER | FCVAR_UNLOGGED };	  // "      "
cvar_t  allow_spectators = { "mp_allowspectators", "1.0", FCVAR_SERVER };		// 0 prevents players from being spectators

// multiplayer server rules
cvar_t	teamplay	= {"mp_teamplay","0", FCVAR_SERVER };
cvar_t	fraglimit	= {"mp_fraglimit","0", FCVAR_SERVER };
cvar_t	timelimit	= {"mp_timelimit","60", FCVAR_SERVER };
cvar_t	friendlyfire= {"mp_friendlyfire","0", FCVAR_SERVER };
cvar_t	falldamage	= {"mp_falldamage","1", FCVAR_SERVER };
cvar_t	weaponstay	= {"mp_weaponstay",".5", FCVAR_SERVER };
cvar_t	forcerespawn= {"mp_forcerespawn","1", FCVAR_SERVER };
cvar_t	flashlight	= {"mp_flashlight","0", FCVAR_SERVER };
cvar_t	aimcrosshair= {"mp_autocrosshair","0", FCVAR_SERVER };
cvar_t	decalfrequency = {"decalfrequency","30", FCVAR_SERVER };
cvar_t	teamlist = {"mp_teamlist","marine1;alien2", FCVAR_SERVER };
cvar_t	teamoverride = {"mp_teamoverride","1" };
cvar_t	defaultteam = {"mp_defaultteam","0" };
cvar_t	allowmonsters={"mp_allowmonsters","0", FCVAR_SERVER };
cvar_t  mp_chattime = {"mp_chattime","10", FCVAR_SERVER };

// AvH server variables
cvar_t	avh_drawdamage				= {kvDrawDamage, "0", FCVAR_SERVER };
cvar_t	avh_tournamentmode			= {kvTournamentMode,"0", FCVAR_SERVER };
cvar_t	avh_deathmatchmode			= {kvDeathMatchMode, "0", FCVAR_SERVER };
cvar_t	avh_countdowntime			= {kvCountDownTime, ".2", FCVAR_SERVER };
cvar_t	avh_latejointime			= {kvLateJoinTime, "1", FCVAR_SERVER};
cvar_t	avh_logdetail				= {kvLogDetail, "0", FCVAR_SERVER};
//cvar_t	avh_teamsizehandicapping	= {kvTeamSizeHandicapping, "0", FCVAR_SERVER};
cvar_t	avh_team1damagepercent      = {kvTeam1DamagePercent, "100", FCVAR_SERVER};
cvar_t	avh_team2damagepercent      = {kvTeam2DamagePercent, "100", FCVAR_SERVER};
cvar_t	avh_team3damagepercent      = {kvTeam3DamagePercent, "100", FCVAR_SERVER};
cvar_t	avh_team4damagepercent      = {kvTeam4DamagePercent, "100", FCVAR_SERVER};
cvar_t	avh_structurelimit			= {kvStructureLimit, "300", FCVAR_SERVER};
cvar_t	avh_votecasttime			= {kvVoteCastTime, "2", FCVAR_SERVER};
cvar_t	avh_votedowntime			= {kvVoteDownTime, "180", FCVAR_SERVER};
cvar_t	avh_minvotesneeded			= {kvMinVotesNeeded, "2", FCVAR_SERVER};
cvar_t	avh_serverops				= {kvServerOps, "", FCVAR_PROTECTED};
cvar_t  avh_limitteams				= {kvLimitTeams, "2", FCVAR_PROTECTED};
cvar_t	avh_votepercentneeded		= {kvVotePercentNeeded, ".3", FCVAR_SERVER};
cvar_t	avh_autoconcede				= {kvAutoConcede, "4", FCVAR_SERVER};
cvar_t	avh_combattime				= {kvCombatTime, "10", FCVAR_SERVER};
cvar_t  avh_mapvoteratio            = {kvMapVoteRatio, ".6", FCVAR_SERVER};
cvar_t  avh_blockscripts            = {kvBlockScripts, "1", FCVAR_SERVER};
#ifdef DEBUG
	cvar_t  avh_testing            = {kvTesting, "0", FCVAR_SERVER};
#endif
cvar_t *avh_cheats=0;
cvar_t *showtriggers=0;
cvar_t *violence_ablood=0;
cvar_t *violence_agibs=0;
cvar_t *violence_hblood=0;
cvar_t *violence_hgibs=0;

// TODO: Remove
cvar_t	avh_ironman					= {kvIronMan, "0", FCVAR_SERVER};
cvar_t	avh_ironmantime				= {kvIronManTime, "2", FCVAR_SERVER};

#ifdef DEBUG
cvar_t	avh_spawninvulnerabletime	= {kvSpawnInvulnerableTime, "0", FCVAR_SERVER};
cvar_t	avh_trainingmode			= {kvTrainingMode,"0", FCVAR_SERVER };
cvar_t	avh_assert					= {kvAssert, "1", FCVAR_SERVER };
cvar_t	avh_bulletcam				= {kvBulletCam, "0", FCVAR_SERVER };
cvar_t	avh_drawinvisible			= {kvDrawInvisible, "0", FCVAR_SERVER };
cvar_t	avh_serverscripts			= {kvServerScripts, "0", FCVAR_SERVER};
#endif

#ifdef USE_NETWORK_METERING
cvar_t	avh_networkdebug			= {kvNetworkDebug, "0", FCVAR_SERVER };
cvar_t	avh_networkmeterrate		= {kvNetworkMeterRate, "3000", FCVAR_SERVER };
#endif

#ifdef PROFILE_BUILD
cvar_t	avh_performance				= {kvPerformance,"8191", FCVAR_SERVER};
#endif

// Other constants
cvar_t	avh_eastereggchance			= {kvEasterEggChance, "5", FCVAR_SERVER};
cvar_t	avh_uplink					= {kvUplink, "0", FCVAR_SERVER};
cvar_t	avh_killdelay				= {kvKillDelay, "0", FCVAR_SERVER};

// Engine Cvars
cvar_t 	*g_psv_gravity = NULL;
cvar_t	*g_psv_aim = NULL;
cvar_t	*g_footsteps = NULL;

// END Cvars for Skill Level settings

// Register your console variables here
// This gets called one time when the game is initialied
void GameDLLInit( void )
{
	// Register cvars here:

	g_psv_gravity = CVAR_GET_POINTER( "sv_gravity" );
	g_psv_aim = CVAR_GET_POINTER( "sv_aim" );
	g_footsteps = CVAR_GET_POINTER( "mp_footsteps" );

	CVAR_REGISTER (&displaysoundlist);

	CVAR_REGISTER (&teamplay);
	CVAR_REGISTER (&fraglimit);
	CVAR_REGISTER (&timelimit);

	CVAR_REGISTER (&fragsleft);
	CVAR_REGISTER (&timeleft);
	CVAR_REGISTER (&allow_spectators);

	CVAR_REGISTER (&friendlyfire);
	CVAR_REGISTER (&falldamage);
	CVAR_REGISTER (&weaponstay);
	CVAR_REGISTER (&forcerespawn);
	CVAR_REGISTER (&flashlight);
	CVAR_REGISTER (&aimcrosshair);
	CVAR_REGISTER (&decalfrequency);
	CVAR_REGISTER (&teamlist);
	CVAR_REGISTER (&teamoverride);
	CVAR_REGISTER (&defaultteam);
	CVAR_REGISTER (&allowmonsters);
	CVAR_REGISTER (&mp_chattime);
	
	// Register AvH variables
	CVAR_REGISTER (&avh_drawdamage);
	CVAR_REGISTER (&avh_tournamentmode);
	CVAR_REGISTER (&avh_deathmatchmode);
	CVAR_REGISTER (&avh_countdowntime);

	avh_cheats=CVAR_GET_POINTER("sv_cheats");
	showtriggers=CVAR_GET_POINTER("showtriggers");
	violence_ablood=CVAR_GET_POINTER("violence_ablood");
	violence_agibs=CVAR_GET_POINTER("violence_agibs");
	violence_hblood=CVAR_GET_POINTER("violence_hblood");
	violence_hgibs=CVAR_GET_POINTER("violence_hgibs");


	CVAR_REGISTER (&avh_latejointime);
	CVAR_REGISTER (&avh_logdetail);
	//CVAR_REGISTER (&avh_teamsizehandicapping);
	CVAR_REGISTER (&avh_team1damagepercent);
	CVAR_REGISTER (&avh_team2damagepercent);
	CVAR_REGISTER (&avh_team3damagepercent);
	CVAR_REGISTER (&avh_team4damagepercent);
	CVAR_REGISTER (&avh_structurelimit);
	CVAR_REGISTER (&avh_votecasttime);
	CVAR_REGISTER (&avh_votedowntime);
	CVAR_REGISTER (&avh_minvotesneeded);
	CVAR_REGISTER (&avh_serverops);
	CVAR_REGISTER (&avh_limitteams);
	CVAR_REGISTER (&avh_votepercentneeded);
	CVAR_REGISTER (&avh_autoconcede);
	CVAR_REGISTER (&avh_combattime);
    CVAR_REGISTER (&avh_mapvoteratio);
    CVAR_REGISTER (&avh_blockscripts);

    // TODO: Remove
    CVAR_REGISTER (&avh_ironman);
	CVAR_REGISTER (&avh_ironmantime);

#ifdef DEBUG
    CVAR_REGISTER (&avh_spawninvulnerabletime);
    CVAR_REGISTER (&avh_trainingmode);
    CVAR_REGISTER (&avh_assert);
    CVAR_REGISTER (&avh_bulletcam);
	CVAR_REGISTER (&avh_testing);
    CVAR_REGISTER (&avh_serverscripts);
#endif

#ifdef USE_NETWORK_METERING
	CVAR_REGISTER (&avh_networkdebug);
    CVAR_REGISTER (&avh_drawinvisible);
#endif

#ifdef PROFILE_BUILD
	CVAR_REGISTER (&avh_performance);
#endif

	CVAR_REGISTER (&avh_eastereggchance);
	CVAR_REGISTER (&avh_uplink);
	CVAR_REGISTER (&avh_killdelay);

}

