/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// teamplay_gamerules.cpp
//
#include	"extdll.h"
#include	"util.h"
#include	"cbase.h"
#include	"player.h"
#include	"weapons.h"
#include	"gamerules.h"
#include	"teamplay_gamerules.h"
#include	"game.h"
#include	"mod/AvHConstants.h"
#include	"mod/AvHServerVariables.h"
#include	"mod/AvHServerUtil.h"
#include	"mod/AvHNetworkMessages.h"

static char team_names[MAX_TEAMS][MAX_TEAMNAME_LENGTH];
static int team_scores[MAX_TEAMS];
static int num_teams = 0;

extern DLL_GLOBAL BOOL		g_fGameOver;

std::string GetLogStringForPlayer( edict_t *pEntity ); //defined in client.cpp

CHalfLifeTeamplay :: CHalfLifeTeamplay()
{
	m_DisableDeathMessages = FALSE;
	m_DisableDeathPenalty = FALSE;

	memset( team_names, 0, sizeof(team_names) );
	memset( team_scores, 0, sizeof(team_scores) );
	num_teams = 0;

	// Copy over the team from the server config
	m_szTeamList[0] = 0;

	// Cache this because the team code doesn't want to deal with changing this in the middle of a game
	//strncpy( m_szTeamList, teamlist.string, TEAMPLAY_TEAMLISTLENGTH );
	strcpy(this->m_szTeamList, kTeamString);

	edict_t *pWorld = INDEXENT(0);
	if ( pWorld && pWorld->v.team )
	{
		if ( teamoverride.value )
		{
			const char *pTeamList = STRING(pWorld->v.team);
			if ( pTeamList && strlen(pTeamList) )
			{
				strncpy( m_szTeamList, pTeamList, TEAMPLAY_TEAMLISTLENGTH );
			}
		}
	}
	// Has the server set teams
	if ( strlen( m_szTeamList ) )
		m_teamLimit = TRUE;
	else
		m_teamLimit = FALSE;

	RecountTeams();
}

extern cvar_t timeleft, fragsleft;

#include "game_shared/voice_gamemgr.h"
extern CVoiceGameMgr	g_VoiceGameMgr;

void CHalfLifeTeamplay :: Think ( void )
{
	///// Check game rules /////
	static int last_frags;
	static int last_time;

	int frags_remaining = 0;
	int time_remaining = 0;

	g_VoiceGameMgr.Update(gpGlobals->frametime);

	if ( g_fGameOver )   // someone else quit the game already
	{
		CHalfLifeMultiplay::Think();
		return;
	}

	float flTimeLimit = CVAR_GET_FLOAT("mp_timelimit") * 60;
	
	time_remaining = (int)(flTimeLimit ? ( flTimeLimit - gpGlobals->time ) : 0);

	// Don't map switch in tourny mode, it signals the end of the match instead
	bool theIsTournyMode = (CVAR_GET_FLOAT(kvTournamentMode) > 0);
	if ( flTimeLimit != 0 && (gpGlobals->time >= flTimeLimit) && !theIsTournyMode)
	{
		GoToIntermission();
		return;
	}

	float flFragLimit = fraglimit.value;
	if ( flFragLimit )
	{
		int bestfrags = 9999;
		int remain;

		// check if any team is over the frag limit
		for ( int i = 0; i < num_teams; i++ )
		{
			if ( team_scores[i] >= flFragLimit )
			{
				GoToIntermission();
				return;
			}

			remain = flFragLimit - team_scores[i];
			if ( remain < bestfrags )
			{
				bestfrags = remain;
			}
		}
		frags_remaining = bestfrags;
	}

	// Updates when frags change
	if ( frags_remaining != last_frags )
	{
		g_engfuncs.pfnCvar_DirectSet( &fragsleft, UTIL_VarArgs( "%i", frags_remaining ) );
	}

	// Updates once per second
	if ( timeleft.value != last_time )
	{
		g_engfuncs.pfnCvar_DirectSet( &timeleft, UTIL_VarArgs( "%i", time_remaining ) );
	}

	last_frags = frags_remaining;
	last_time  = time_remaining;
}

//=========================================================
// ClientCommand
// the user has typed a command which is unrecognized by everything else;
// this check to see if the gamerules knows anything about the command
//=========================================================
BOOL CHalfLifeTeamplay :: ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
	if(g_VoiceGameMgr.ClientCommand(pPlayer, pcmd))
		return TRUE;

	if ( FStrEq( pcmd, "menuselect" ) )
	{
		if ( CMD_ARGC() < 2 )
			return TRUE;

		int slot = atoi( CMD_ARGV(1) );

		// select the item from the current menu

		return TRUE;
	}

	return FALSE;
}

const char *CHalfLifeTeamplay::SetDefaultPlayerTeam( CBasePlayer *pPlayer )
{
	// copy out the team name from the model
	char *mdls = g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model" );
	pPlayer->SetTeamID(mdls);

	RecountTeams();

	// update the current player of the team he is joining
	const char* theTeamName = pPlayer->TeamID();

	if ( theTeamName == '\0' || !IsValidTeam( theTeamName ) || defaultteam.value )
	{
		const char *pTeamName = NULL;
		
		if ( defaultteam.value )
		{
			pTeamName = team_names[0];
		}
		else
		{
			pTeamName = TeamWithFewestPlayers();
		}
		pPlayer->SetTeamID(pTeamName);
	}

	return pPlayer->TeamID();
}


//=========================================================
// InitHUD
//=========================================================
void CHalfLifeTeamplay::InitHUD( CBasePlayer *pPlayer )
{
	SetDefaultPlayerTeam( pPlayer );
	CHalfLifeMultiplay::InitHUD( pPlayer );

	StringList team_name_list;
	for( int counter = 0; counter < num_teams; counter++ )
	{ team_name_list.push_back( string("#") + string(team_names[counter]) ); }

	NetMsg_TeamNames( pPlayer->pev, team_name_list );

	RecountTeams();

	ChangePlayerTeam( pPlayer, pPlayer->TeamID(), FALSE, FALSE );
	int clientIndex = pPlayer->entindex();
	RecountTeams();

	pPlayer->NeedsTeamUpdate();
}


void CHalfLifeTeamplay::ChangePlayerTeam( CBasePlayer *pPlayer, const char *pTeamName, BOOL bKill, BOOL bGib )
{
	int damageFlags = DMG_GENERIC;
	int clientIndex = pPlayer->entindex();

	if ( !bGib )
	{
		damageFlags |= DMG_NEVERGIB;
	}
	else
	{
		damageFlags |= DMG_ALWAYSGIB;
	}

	if ( bKill )
	{
		// kill the player,  remove a death,  and let them start on the new team
		m_DisableDeathMessages = TRUE;
		m_DisableDeathPenalty = TRUE;

		entvars_t *pevWorld = VARS( INDEXENT(0) );
		pPlayer->TakeDamage( pevWorld, pevWorld, 900, damageFlags );

		m_DisableDeathMessages = FALSE;
		m_DisableDeathPenalty = FALSE;
	}

	pPlayer->SetTeamID(pTeamName);

	g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", pPlayer->TeamID());

	// notify everyone's HUD of the team change
	pPlayer->SendTeamUpdate();

	pPlayer->EffectivePlayerClassChanged();
}


//=========================================================
// ClientUserInfoChanged
//=========================================================
void CHalfLifeTeamplay::ClientUserInfoChanged( CBasePlayer *pPlayer, char *infobuffer )
{
	char text[1024];

	// prevent skin/color/model changes
	char *mdls = g_engfuncs.pfnInfoKeyValue( infobuffer, "model" );

	if ( !stricmp( mdls, pPlayer->TeamID() ) )
		return;

	if ( defaultteam.value )
	{
		int clientIndex = pPlayer->entindex();

		g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->TeamID() );
		g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "team", pPlayer->TeamID() );
		sprintf( text, "* Not allowed to change teams in this game!\n" );
		UTIL_SayText( text, pPlayer );
		return;
	}

	if ( defaultteam.value || !IsValidTeam( mdls ) )
	{
		int clientIndex = pPlayer->entindex();

		g_engfuncs.pfnSetClientKeyValue( clientIndex, g_engfuncs.pfnGetInfoKeyBuffer( pPlayer->edict() ), "model", pPlayer->TeamID() );
		sprintf( text, "* Can't change team to \'%s\'\n", mdls );
		UTIL_SayText( text, pPlayer );
		sprintf( text, "* Server limits teams to \'%s\'\n", m_szTeamList );
		UTIL_SayText( text, pPlayer );
		return;
	}
	// notify everyone of the team change
	sprintf( text, "* %s has changed to team \'%s\'\n", STRING(pPlayer->pev->netname), mdls );
	UTIL_SayTextAll( text, pPlayer );

	UTIL_LogPrintf( "%s joined team \"%s\"\n", GetLogStringForPlayer( pPlayer->edict() ).c_str(), mdls );

	ChangePlayerTeam( pPlayer, mdls, TRUE, TRUE );
	// recound stuff
	RecountTeams();
}

//=========================================================
// Deathnotice. 
//=========================================================
void CHalfLifeTeamplay::DeathNotice( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pevInflictor )
{
	if ( m_DisableDeathMessages )
		return;
	
	if ( pVictim && pKiller && pKiller->flags & FL_CLIENT )
	{
		CBasePlayer *pk = (CBasePlayer*) CBaseEntity::Instance( pKiller );

		if ( pk )
		{
			if ( (pk != pVictim) && (PlayerRelationship( pVictim, pk ) == GR_TEAMMATE) )
			{
			  string tmpstr("teammate");
				NetMsg_DeathMsg( ENTINDEX(ENT(pKiller)), ENTINDEX(pVictim->edict()), tmpstr);
				return;
			}
		}
	}

	CHalfLifeMultiplay::DeathNotice( pVictim, pKiller, pevInflictor );
}

//=========================================================
//=========================================================
void CHalfLifeTeamplay :: PlayerKilled( CBasePlayer *pVictim, entvars_t *pKiller, entvars_t *pInflictor )
{
	if ( !m_DisableDeathPenalty )
	{
		CHalfLifeMultiplay::PlayerKilled( pVictim, pKiller, pInflictor );
		RecountTeams();
	}
}


//=========================================================
// IsTeamplay
//=========================================================
BOOL CHalfLifeTeamplay::IsTeamplay( void )
{
	return TRUE;
}

BOOL CHalfLifeTeamplay::FPlayerCanTakeDamage( CBasePlayer *pPlayer, CBaseEntity *pAttacker )
{
	if ( pAttacker && PlayerRelationship( pPlayer, pAttacker ) == GR_TEAMMATE )
	{
		// my teammate hit me.
		if ( (friendlyfire.value == 0) && (pAttacker != pPlayer) )
		{
			// friendly fire is off, and this hit came from someone other than myself,  then don't get hurt
			return FALSE;
		}
	}

	return CHalfLifeMultiplay::FPlayerCanTakeDamage( pPlayer, pAttacker );
}

//=========================================================
//=========================================================
int CHalfLifeTeamplay::PlayerRelationship( CBaseEntity *pPlayer, CBaseEntity *pTarget )
{
	// half life multiplay has a simple concept of Player Relationships.
	// you are either on another player's team, or you are not.
	if ( !pPlayer || !pTarget || !pTarget->IsPlayer() )
		return GR_NOTTEAMMATE;

	if ( (*GetTeamID(pPlayer) != '\0') && (*GetTeamID(pTarget) != '\0') && !stricmp( GetTeamID(pPlayer), GetTeamID(pTarget) ) )
	{
		return GR_TEAMMATE;
	}

	return GR_NOTTEAMMATE;
}

//=========================================================
//=========================================================
BOOL CHalfLifeTeamplay::ShouldAutoAim( CBasePlayer *pPlayer, edict_t *target )
{
	// always autoaim, unless target is a teammate
	CBaseEntity *pTgt = CBaseEntity::Instance( target );
	if ( pTgt && pTgt->IsPlayer() )
	{
		if ( PlayerRelationship( pPlayer, pTgt ) == GR_TEAMMATE )
			return FALSE; // don't autoaim at teammates
	}

	return CHalfLifeMultiplay::ShouldAutoAim( pPlayer, target );
}

//=========================================================
//=========================================================
int CHalfLifeTeamplay::IPointsForKill( CBasePlayer *pAttacker, CBasePlayer *pKilled )
{
	if ( !pKilled )
		return 0;

	if ( !pAttacker )
		return 1;

	if ( pAttacker != pKilled && PlayerRelationship( pAttacker, pKilled ) == GR_TEAMMATE )
		return -1;

	return 1;
}

//=========================================================
//=========================================================
const char *CHalfLifeTeamplay::GetTeamID( CBaseEntity *pEntity )
{
	if ( pEntity == NULL || pEntity->pev == NULL )
		return "";

	// return their team name
	return pEntity->TeamID();
}


int CHalfLifeTeamplay::GetTeamIndex( const char *pTeamName )
{
	if ( pTeamName && *pTeamName != 0 )
	{
		// try to find existing team
		for ( int tm = 0; tm < num_teams; tm++ )
		{
			if ( !stricmp( team_names[tm], pTeamName ) )
				return tm;
		}
	}
	
	return -1;	// No match
}


const char *CHalfLifeTeamplay::GetIndexedTeamName( int teamIndex )
{
	if ( teamIndex < 0 || teamIndex >= num_teams )
		return "";

	return team_names[ teamIndex ];
}


BOOL CHalfLifeTeamplay::IsValidTeam( const char *pTeamName ) 
{
	if ( !m_teamLimit )	// Any team is valid if the teamlist isn't set
		return TRUE;

	return ( GetTeamIndex( pTeamName ) != -1 ) ? TRUE : FALSE;
}

const char *CHalfLifeTeamplay::TeamWithFewestPlayers( void )
{
	int i;
	int minPlayers = MAX_TEAMS;
	int teamCount[ MAX_TEAMS ];
	char *pTeamName = NULL;

	memset( teamCount, 0, MAX_TEAMS * sizeof(int) );
	
	// loop through all clients, count number of players on each team
	for ( i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );

		if ( plr )
		{
			int team = GetTeamIndex( plr->TeamID() );
			if ( team >= 0 )
				teamCount[team] ++;
		}
	}

	// Find team with least players
	for ( i = 0; i < num_teams; i++ )
	{
		if ( teamCount[i] < minPlayers )
		{
			minPlayers = teamCount[i];
			pTeamName = team_names[i];
		}
	}

	return pTeamName;
}


//=========================================================
//=========================================================
void CHalfLifeTeamplay::RecountTeams( void )
{
	char	*pName;
	char	teamlist[TEAMPLAY_TEAMLISTLENGTH];

	// loop through all teams, recounting everything
	num_teams = 0;

	// Copy all of the teams from the teamlist
	// make a copy because strtok is destructive
	strcpy( teamlist, m_szTeamList );
	pName = teamlist;
	pName = strtok( pName, ";" );
	while ( pName != NULL && *pName )
	{
		if ( GetTeamIndex( pName ) < 0 )
		{
			strcpy( team_names[num_teams], pName );
			num_teams++;
		}
		pName = strtok( NULL, ";" );
	}

	if ( num_teams < 2 )
	{
		num_teams = 0;
		m_teamLimit = FALSE;
	}

	// Sanity check
	memset( team_scores, 0, sizeof(team_scores) );

	// loop through all clients
	for ( int i = 1; i <= gpGlobals->maxClients; i++ )
	{
		CBaseEntity *plr = UTIL_PlayerByIndex( i );

		if ( plr )
		{
			const char *pTeamName = plr->TeamID();
			// try add to existing team
			int tm = GetTeamIndex( pTeamName );
			
			if ( tm < 0 ) // no team match found
			{ 
				if ( !m_teamLimit )
				{
					// add to new team
					tm = num_teams;
					num_teams++;
					team_scores[tm] = 0;
					strncpy( team_names[tm], pTeamName, MAX_TEAMNAME_LENGTH );
				}
			}

			if ( tm >= 0 )
			{
				team_scores[tm] += plr->pev->frags;
			}
		}
	}
}
