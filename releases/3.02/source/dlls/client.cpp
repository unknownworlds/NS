//	
//	Copyright (c) 1999, Valve LLC. All rights reserved.
//	
//	This product contains software technology licensed from Id 
//	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
//	All Rights Reserved.
//	
//	Use, distribution, and modification of this source code and/or resulting
//	object code is restricted to non-commercial enhancements to products from
//	Valve LLC.  All other use, distribution, or modification is prohibited
//	without written permission from Valve LLC.
//	
// Robin, 4-22-98: Moved set_suicide_frame() here from player.cpp to allow us to 
//				   have one without a hardcoded player.mdl in tf_client.cpp
//
//===== client.cpp ========================================================
//
//  client/server game specific stuff
//
// $Workfile: client.cpp $
// $Date: 2002/11/22 21:09:09 $
//
//-------------------------------------------------------------------------------
// $Log: client.cpp,v $
// Revision 1.69  2002/11/22 21:09:09  Flayra
// - Added "lastinv" command back in
// - mp_consistency changes
//
// Revision 1.68  2002/11/15 23:31:01  Flayra
// - Added "ready" verification for tourny mode
//
// Revision 1.67  2002/11/15 05:08:31  Flayra
// - Little tweak for refinery
//
// Revision 1.66  2002/11/15 04:48:40  Flayra
// - Oops
//
// Revision 1.65  2002/11/15 04:41:32  Flayra
// - Big performance improvements for AddToFullPack (whew)
//
// Revision 1.64  2002/11/12 02:17:48  Flayra
// - Renamed "avhplayer" to "player"
// - Tweaked enhanced sight to actually be useful at lower levels
//
// Revision 1.63  2002/11/03 04:53:20  Flayra
// - AddToFullPack optimizations and cleanup
//
// Revision 1.62  2002/10/25 21:50:01  Flayra
// - New attempted fix for overflows.  Propagate skin and playerclass in new way, so entities outside the ready room aren't propagated to all clients that go back to the ready room immediately on game end.
//
// Revision 1.61  2002/10/24 21:16:10  Flayra
// - Log Sys_Errors
// - Allow players to change their name before they first leave the ready room
//
// Revision 1.60  2002/10/20 17:25:04  Flayra
// - Redid performance improvement (agh)
//
// Revision 1.59  2002/10/20 16:34:09  Flayra
// - Returned code back to normal
//
// Revision 1.58  2002/10/19 22:33:48  Flayra
// - Various server optimizations
//
// Revision 1.57  2002/10/18 22:14:23  Flayra
// - Server optimizations (untested though, may need backing out)
//
// Revision 1.56  2002/10/16 00:38:50  Flayra
// - Queue up name change until end of game
// - Removed precaching of unneeded sounds
// - Optimized AvHDetermineVisibility (being called 1000s of times per tick)
//
// Revision 1.55  2002/10/03 18:27:39  Flayra
// - Moved order completion sounds into HUD sounds
//
// Revision 1.54  2002/09/25 20:37:53  Flayra
// - Precache more sayings
//
// Revision 1.53  2002/09/23 22:01:02  Flayra
// - Propagate skin
// - Don't treat unclaimed hives at world objects (even though team is 0) for visibility purposes
// - Added heavy model
// - Removed old mapper build preprocessor directives
//
// Revision 1.52  2002/08/31 18:01:17  Flayra
// - Work at VALVe
//
// Revision 1.51  2002/08/16 02:24:31  Flayra
// - Removed "give" cheat
//
// Revision 1.50  2002/08/09 00:15:46  Flayra
// - Removed behavior where "drop" would drop alien weapons in a disconcerting manner
//
// Revision 1.49  2002/07/25 16:50:15  flayra
// - Linux build changes
//
// Revision 1.48  2002/07/24 18:46:19  Flayra
// - Linux and scripting changes
//
// Revision 1.47  2002/07/23 16:46:38  Flayra
// - Added power-armor drawing, tweaked invul drawing
//
// Revision 1.46  2002/07/10 14:36:21  Flayra
// - .mp3 and particle fixes
//
// Revision 1.45  2002/07/08 16:38:57  Flayra
// - Draw invulnerable players differently
//
//===============================================================================
#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "saverestore.h"
#include "player.h"
#include "spectator.h"
#include "client.h"
#include "soundent.h"
#include "gamerules.h"
#include "game.h"
#include "customentity.h"
#include "weapons.h"
#include "weaponinfo.h"
#include "usercmd.h"
#include "netadr.h"
#include "util/nowarnings.h"
#include "mod/AvHPlayer.h"
#include "mod/AvHSoundListManager.h"
#include "game.h"
#include "mod/AvHParticleTemplateServer.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHSpecials.h"
#include "util/Zassert.h"
#include "dlls/cbasedoor.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHMovementUtil.h"
#include "mod/AvHSoundConstants.h"
#include "mod/AvHHulls.h"
#include "mod/AvHServerVariables.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHAlienEquipmentConstants.h"
#include "dlls/triggers.h"
#include "util/MathUtil.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHCloakable.h"
#include "mod/AvHAlienAbilityConstants.h"
#include "mod/UPPUtil.h"

#include "voice_gamemgr.h"
extern CVoiceGameMgr	g_VoiceGameMgr;

extern AvHSoundListManager				gSoundListManager;
//edict_t *EntSelectSpawnPoint( CBaseEntity *pPlayer );
extern int gmsgSetParticleTemplates;
//extern int gServerTick;
//extern int gUpdateEntitiesTick;


extern DLL_GLOBAL ULONG		g_ulModelIndexPlayer;
extern DLL_GLOBAL BOOL		g_fGameOver;
extern DLL_GLOBAL int		g_iSkillLevel;
extern DLL_GLOBAL ULONG		g_ulFrameCount;

extern void CopyToBodyQue(entvars_t* pev);
extern int giPrecacheGrunt;
extern int gmsgShowMenu;
extern int g_teamplay;

void LinkUserMessages( void );

/*
 * used by kill command and disconnect command
 * ROBIN: Moved here from player.cpp, to allow multiple player models
 */
void set_suicide_frame(entvars_t* pev)
{       
	if (!FStrEq(STRING(pev->model), "models/player.mdl"))
		return; // allready gibbed

//	pev->frame		= $deatha11;
	pev->solid		= SOLID_NOT;
	pev->movetype	= MOVETYPE_TOSS;
	pev->deadflag	= DEAD_DEAD;
	pev->nextthink	= -1;
}


/*
===========
ClientConnect

called when a player connects to a server
============
*/
BOOL ClientConnect( edict_t *pEntity, const char *pszName, const char *pszAddress, char szRejectReason[ 128 ]  )
{
	return g_pGameRules->ClientConnected( pEntity, pszName, pszAddress, szRejectReason );
	
	// a client connecting during an intermission can cause problems
	//	if (intermission_running)
	//		ExitIntermission ();
	
}


/*
===========
ClientDisconnect

  called when a player disconnects from a server
  
	GLOBALS ASSUMED SET:  g_fGameOver
	============
*/
void ClientDisconnect( edict_t *pEntity )
{
	if (g_fGameOver)
		return;
	
	char text[256];
	sprintf( text, "- %s has left the game\n", STRING(pEntity->v.netname) );

	CBaseEntity* theBaseEntity = CBaseEntity::Instance(ENT(pEntity));
	UTIL_SayTextAll(text, theBaseEntity);
	
	CSound *pSound;
	pSound = CSoundEnt::SoundPointerForIndex( CSoundEnt::ClientSoundIndex( pEntity ) );
	{
		// since this client isn't around to think anymore, reset their sound. 
		if ( pSound )
		{
			pSound->Reset();
		}
	}
	
	// since the edict doesn't get deleted, fix it so it doesn't interfere.
	pEntity->v.takedamage = DAMAGE_NO;// don't attract autoaim
	pEntity->v.solid = SOLID_NOT;// nonsolid
	UTIL_SetOrigin ( &pEntity->v, pEntity->v.origin );
	
	g_pGameRules->ClientDisconnected( pEntity );

	//voogru: If this isnt set, clients around this player will crash.
	pEntity->v.effects |= EF_NODRAW;

//	CBaseEntity* theEntity = CBaseEntity::Instance(ENT(pEntity));
//	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
//	if(thePlayer)
//	{
//		thePlayer->SetLeftServer();
//	}
	//UTIL_Remove(theEntity);
}


// called by ClientKill and DeadThink
void respawn(entvars_t* pev, BOOL fCopyCorpse)
{
	// AVH isn't dm, coop or single-player.
	//if (gpGlobals->coop || gpGlobals->deathmatch)
	//{
		if ( fCopyCorpse )
		{
			// make a copy of the dead body for appearances sake
			CopyToBodyQue(pev);
		}

		// respawn player
		GetClassPtr( (AvHPlayer *)pev)->Spawn( );
	//}
	//else
	//{       // restart the entire server
	//	SERVER_COMMAND("reload\n");
	//}
}

/*
============
ClientKill

Player entered the suicide command

GLOBALS ASSUMED SET:  g_ulModelIndexPlayer
============
*/
void ClientKill( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;

	CBasePlayer *pl = (CBasePlayer*) CBasePlayer::Instance( pev );

	//if(GetGameRules()->GetCheatsEnabled())
	//{
		if ( pl->m_fNextSuicideTime > gpGlobals->time )
			return;  // prevent suiciding too ofter

		pl->m_fNextSuicideTime = gpGlobals->time + 1;  // don't let them suicide for 5 seconds after suiciding
		
        bool theIsCombatModeForSuicide = GetGameRules()->GetIsCombatMode();
        
        #ifdef DEBUG
        if(GetGameRules()->GetIsCombatMode())
        {
            theIsCombatModeForSuicide = false;
        }
        #endif
        
        bool theCanSuicide = GetGameRules()->CanPlayerBeKilled(pl) && !theIsCombatModeForSuicide;

		if(theCanSuicide)
		{
			pl->Suicide();

			//	pev->modelindex = g_ulModelIndexPlayer;
			//	pev->frags -= 2;		// extra penalty
			//	respawn( pev );
		}
	//}
}

/*
===========
ClientPutInServer

called each time a player is spawned
============
*/
void ClientPutInServer( edict_t *pEntity )
{
	//CBasePlayer *pPlayer;
	AvHPlayer *pPlayer;

	entvars_t *pev = &pEntity->v;

	if(pev->flags & FL_PROXY)
	{
		ALERT(at_logged, "Player with FL_PROXY put in server.\n");
	}

	if(pev->flags & FL_PROXY)
	{
		pev->flags |= FL_PROXY;
	}

	pPlayer = GetClassPtr((AvHPlayer *)pev);
	pPlayer->SetCustomDecalFrames(-1); // Assume none;

	// Allocate a CBasePlayer for pev, and call spawn
	pPlayer->SetPlayMode(PLAYMODE_READYROOM);
	//pPlayer->Spawn();

	// Reset interpolation during first frame
	pPlayer->pev->effects |= EF_NOINTERP;
}

//// HOST_SAY
// String comes in as
// say blah blah blah
// or as
// blah blah blah
//
void Host_Say( edict_t *pEntity, int teamonly )
{
	AvHPlayer*	client;
	AvHPlayer*	theTalkingPlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(pEntity));
	int			j;
	char*		p;
	char		text[256];
	char		szTemp[256];
	const char*	cpSay = "say";
	const char*	cpSayTeam = "say_team";
	const char*	pcmd = CMD_ARGV(0);
	bool		theTalkerInReadyRoom = theTalkingPlayer->GetInReadyRoom();
	//bool		theTalkerIsObserver = (theTalkingPlayer->GetPlayMode() == PLAYMODE_OBSERVER) || (theTalkingPlayer->GetPlayMode() == PLAYMODE_AWAITINGREINFORCEMENT);

	// We can get a raw string now, without the "say " prepended
	if ( CMD_ARGC() == 0 )
		return;

	//Not yet.
	if ( theTalkingPlayer->m_flNextChatTime > gpGlobals->time )
		 return;

	if ( !stricmp( pcmd, cpSay) || !stricmp( pcmd, cpSayTeam ) )
	{
		if ( CMD_ARGC() >= 2 )
		{
			p = (char *)CMD_ARGS();

			if(GetGameRules()->GetIsTournamentMode())
			{
				if(!GetGameRules()->GetGameStarted())
				{
					if(!strcmp(CMD_ARGV(1), kReadyNotification))
					{
						// Team is ready
						AvHTeam* theTeam = GetGameRules()->GetTeam((AvHTeamNumber)(pEntity->v.team));
						if(theTeam && !theTeam->GetIsReady())
						{
							theTeam->SetIsReady();
						}
					}
					else if (!strcmp(CMD_ARGV(1), kNotReadyNotification))
					{
						// Team is no longer ready
						AvHTeam* theTeam = GetGameRules()->GetTeam((AvHTeamNumber)(pEntity->v.team));
						if(theTeam && theTeam->GetIsReady())
						{
							theTeam->SetIsReady(false);
						}
					}
				}
			}
		}
		else
		{
			// say with a blank message, nothing to do
			return;
		}
	}
	else  // Raw text, need to prepend argv[0]
	{
		if ( CMD_ARGC() >= 2 )
		{
			sprintf( szTemp, "%s %s", ( char * )pcmd, (char *)CMD_ARGS() );
		}
		else
		{
			// Just a one word command, use the first word...sigh
			sprintf( szTemp, "%s", ( char * )pcmd );
		}
		p = szTemp;
	}

// remove quotes if present
	if (*p == '"')
	{
		p++;
		p[strlen(p)-1] = 0;
	}

// make sure the text has content
	for ( char *pc = p; pc != NULL && *pc != 0; pc++ )
	{
		if ( isprint( *pc ) && !isspace( *pc ) )
		{
			pc = NULL;	// we've found an alphanumeric character,  so text is valid
			break;
		}
	}
	if ( pc != NULL )
		return;  // no character found, so say nothing

// turn on color set 2  (color on,  no sound)
	if ( teamonly )
		sprintf( text, "%c(TEAM) %s: ", 2, STRING( pEntity->v.netname ) );
	else
		sprintf( text, "%c%s: ", 2, STRING( pEntity->v.netname ) );

	j = sizeof(text) - 2 - strlen(text);  // -2 for /n and null terminator
	if ( (int)strlen(p) > j )
		p[j] = 0;

	strcat( text, p );
	strcat( text, "\n" );

	theTalkingPlayer->m_flNextChatTime = gpGlobals->time + CHAT_INTERVAL;
	// loop through all players
	// Start with the first player.
	// This may return the world in single player if the client types something between levels or during spawn
	// so check it, or it will infinite loop

	client = NULL;
	while ( ((client = (AvHPlayer*)UTIL_FindEntityByClassname( client, "player" )) != NULL) && (!FNullEnt(client->edict())) ) 
	{
		if ( !client->pev )
			continue;
		
		if ( client->edict() == pEntity )
			continue;

		if ( !(client->IsNetClient()) )	// Not a client ? (should never be true)
			continue;

	    // can the receiver hear the sender? or has he muted him?
	    if ( g_VoiceGameMgr.PlayerHasBlockedPlayer( client, theTalkingPlayer ) )
		    continue;
    
		// Don't differentiate between team and non-team when not playing
		bool theTalkingPlayerIsPlaying = ((theTalkingPlayer->GetPlayMode() == PLAYMODE_PLAYING) || (theTalkingPlayer->GetPlayMode() == PLAYMODE_AWAITINGREINFORCEMENT) || (theTalkingPlayer->GetPlayMode() == PLAYMODE_REINFORCING));
		bool theClientIsPlaying = ((client->GetPlayMode() == PLAYMODE_PLAYING) || (client->GetPlayMode() == PLAYMODE_AWAITINGREINFORCEMENT) || (client->GetPlayMode() == PLAYMODE_REINFORCING));
		bool theTalkerIsObserver = theTalkingPlayer->IsObserver();
		bool theClientIsObserver = client->IsObserver();

        bool theClientInReadyRoom = client->GetInReadyRoom();

        if (theClientInReadyRoom != theTalkerInReadyRoom)
        {
            continue;
        }

        if (!theClientIsObserver || theClientIsPlaying) // Non-playing Observers hear everything.
        {

		    if ( theTalkingPlayerIsPlaying && teamonly && g_pGameRules->PlayerRelationship(client, CBaseEntity::Instance(pEntity)) != GR_TEAMMATE )
			    continue;

		    // chat can never go between play area and non-play area
		    if(theTalkingPlayerIsPlaying != theClientIsPlaying)
			    continue;

		    // chat of any kind doesn't go from ready room to play area in tournament mode
		    if(theTalkerInReadyRoom && GetGameRules()->GetIsTournamentMode() && theClientIsPlaying)
			    continue;
        
        }

		UTIL_SayText(text, client, ENTINDEX(pEntity));

	}

	// print to the sending client
	UTIL_SayText(text, CBaseEntity::Instance(ENT(pEntity)));

	// echo to server console
	g_engfuncs.pfnServerPrint( text );

	char * temp;
	if ( teamonly )
		temp = "say_team";
	else
		temp = "say";
	
	// team match?
	if ( g_teamplay )
	{
		UTIL_LogPrintf( "\"%s<%i><%s><%s>\" %s \"%s\"\n", 
			STRING( pEntity->v.netname ), 
			GETPLAYERUSERID( pEntity ),
#ifdef USE_UPP
			UPPUtil_GetNetworkID(pEntity).c_str(),
#else
			AvHSUGetPlayerAuthIDString( pEntity ).c_str(),
#endif
			//g_engfuncs.pfnInfoKeyValue( g_engfuncs.pfnGetInfoKeyBuffer( pEntity ), "model" ),
			AvHSUGetTeamName(pEntity->v.team),
			temp,
			p );
	}
	else
	{
		UTIL_LogPrintf( "\"%s<%i><%s><%i>\" %s \"%s\"\n", 
			STRING( pEntity->v.netname ), 
			GETPLAYERUSERID( pEntity ),
#ifdef USE_UPP
			UPPUtil_GetNetworkID(pEntity).c_str(),
#else
			AvHSUGetPlayerAuthIDString( pEntity ).c_str(),
#endif
			GETPLAYERUSERID( pEntity ),
			temp,
			p );
	}
}


/*
===========
ClientCommand
called each time a player uses a "cmd" command
============
*/

// Use CMD_ARGV,  CMD_ARGV, and CMD_ARGC to get pointers the character string command.
void ClientCommand( edict_t *pEntity )
{
	const char *pcmd = CMD_ARGV(0);
	const char *pstr;

	// Is the client spawned yet?
	if ( !pEntity->pvPrivateData )
		return;

	entvars_t *pev = &pEntity->v;

	AvHPlayer* thePlayer = GetClassPtr((AvHPlayer *)pev);
	bool thePlayerCanAct = thePlayer->GetIsAbleToAct();

	if ( FStrEq(pcmd, "say" ) )
	{
		Host_Say( pEntity, 0 );
	}
	else if ( FStrEq(pcmd, "say_team" ) )
	{
		Host_Say( pEntity, 1 );
	}
	else if ( FStrEq(pcmd, "fullupdate" ) )
	{
		GetClassPtr((CBasePlayer *)pev)->ForceClientDllUpdate(); 
	}
	else if ( FStrEq(pcmd, "give" ) )
	{
		if(GetGameRules()->GetCheatsEnabled())
		{
			int iszItem = ALLOC_STRING( CMD_ARGV(1) );	// Make a copy of the classname
			GetClassPtr((CBasePlayer *)pev)->GiveNamedItem( STRING(iszItem) );
		}
	}

	else if ( FStrEq(pcmd, "drop" ))
	{
        if (thePlayerCanAct)
        {
		    // player is dropping an item. 
		    GetClassPtr((AvHPlayer *)pev)->DropItem((char *)CMD_ARGV(1));
        }
	}
	else if ( FStrEq(pcmd, "fov" ) )
	{
		if (GetGameRules()->GetCheatsEnabled() && (CMD_ARGC() > 1))
		{
			GetClassPtr((CBasePlayer *)pev)->m_iFOV = atoi( CMD_ARGV(1) );
		}
		else
		{
			CLIENT_PRINTF( pEntity, print_console, UTIL_VarArgs( "\"fov\" is \"%d\"\n", (int)GetClassPtr((CBasePlayer *)pev)->m_iFOV ) );
		}
	}
	else if ( FStrEq(pcmd, "use" ))
	{
        if (thePlayerCanAct)
        {
		    GetClassPtr((CBasePlayer *)pev)->SelectItem((char *)CMD_ARGV(1));
        }
	}
	else if (((pstr = strstr(pcmd, "weapon_")) != NULL)  && (pstr == pcmd))
	{
        if (thePlayerCanAct)
        {
		    GetClassPtr((CBasePlayer *)pev)->SelectItem(pcmd);
        }
	}
//	else if (FStrEq(pcmd, "lastinv" ))
//	{
//      if (thePlayerCanAct)
//      {
//		    GetClassPtr((CBasePlayer *)pev)->SelectLastItem();
//      }
//	}
//	else if ( FStrEq( pcmd, "spectate" ) && (pev->flags & FL_PROXY) )	// added for proxy support
//	{
//		CBasePlayer * pPlayer = GetClassPtr((CBasePlayer *)pev);
//
//		edict_t *pentSpawnSpot = g_pGameRules->GetPlayerSpawnSpot( pPlayer );
//		pPlayer->StartObserver( pev->origin, VARS(pentSpawnSpot)->angles);
//	}
	else if ( g_pGameRules->ClientCommand( GetClassPtr((CBasePlayer *)pev), pcmd ) )
	{
		// MenuSelect returns true only if the command is properly handled,  so don't print a warning
	}
	else if ( FStrEq( pcmd, "specmode" )  )	// new spectator mode
	{
		CBasePlayer * pPlayer = GetClassPtr((CBasePlayer *)pev);
		if ( pPlayer->IsObserver() )
		{
            int theMode = atoi(CMD_ARGV(1));
			pPlayer->Observer_SetMode(theMode);
			pPlayer->SetDefaultSpectatingSettings(pPlayer->pev->iuser1, pPlayer->pev->iuser2);
		}
	}
    else if (FStrEq( pcmd, "follow")) // follow a specific player
    {
		CBasePlayer * pPlayer = GetClassPtr((CBasePlayer *)pev);
		if ( pPlayer->IsObserver() )
		{
			pPlayer->Observer_SpectatePlayer(atoi( CMD_ARGV(1) ));
			pPlayer->SetDefaultSpectatingSettings(pPlayer->pev->iuser1, pPlayer->pev->iuser2);
		}
    }
	else if ( FStrEq( pcmd, "follownext" )  )	// follow next player
	{
		CBasePlayer * pPlayer = GetClassPtr((CBasePlayer *)pev);
		if ( pPlayer->IsObserver() )
		{
			pPlayer->Observer_FindNextPlayer(atoi( CMD_ARGV(1) ));
			pPlayer->SetDefaultSpectatingSettings(pPlayer->pev->iuser1, pPlayer->pev->iuser2);
		}
	}
	else
	{
		// tell the user they entered an unknown command
		char command[128];

		// check the length of the command (prevents crash)
		// max total length is 192 ...and we're adding a string below ("Unknown command: %s\n")
		strncpy( command, pcmd, 127 );
		command[127] = '\0';

		// tell the user they entered an unknown command
		ClientPrint( &pEntity->v, HUD_PRINTCONSOLE, UTIL_VarArgs( "Unknown command: %s\n", command ) );
	}
    //else if(!AvHClientCommand(pEntity))
    //{
	//	// tell the user they entered an unknown command
	//	ClientPrint( &pEntity->v, HUD_PRINTCONSOLE, UTIL_VarArgs( "Unknown command: %s\n", pcmd ) );
	//}
}

//bool AvHClientCommand( edict_t *pEntity )
//{
//}

/*
========================
ClientUserInfoChanged

called after the player changes
userinfo - gives dll a chance to modify it before
it gets sent into the rest of the engine.
========================
*/
void ClientUserInfoChanged( edict_t *pEntity, char *infobuffer )
{
	// Is the client spawned yet?
	if ( !pEntity->pvPrivateData )
		return;

	const char* theCurrentNetName = STRING(pEntity->v.netname);
	const char* theNameKeyValue = g_engfuncs.pfnInfoKeyValue(infobuffer, "name");
	
	// msg everyone if someone changes their name,  and it isn't the first time (changing no name to current name)
	if ( pEntity->v.netname && STRING(pEntity->v.netname)[0] != 0 && !FStrEq(theCurrentNetName, theNameKeyValue) )
	{
		AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(pEntity));
		
		// Don't change player info after match has started unless player hasn't left ready room
		if(!GetGameRules()->GetArePlayersAllowedToJoinImmediately() && thePlayer && theNameKeyValue && thePlayer->GetHasLeftReadyRoom())
		{
			thePlayer->SetDesiredNetName(string(theNameKeyValue));
			
			char text[256];
			sprintf( text, "* Name will be changed to %s next game.\n", theNameKeyValue);
			UTIL_SayText(text, CBaseEntity::Instance(ENT(pEntity)));
			
			// Restore name key value changed by engine
			char theName[256];
			strcpy(theName, theCurrentNetName);
			g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pEntity), infobuffer, "name", theName);
		}
		else
		{
			char sName[256];
			char *pName = g_engfuncs.pfnInfoKeyValue( infobuffer, "name" );
			strncpy( sName, pName, sizeof(sName) - 1 );
			sName[ sizeof(sName) - 1 ] = '\0';
			
			// First parse the name and remove any %'s
			for ( char *pApersand = sName; pApersand != NULL && *pApersand != 0; pApersand++ )
			{
				// Replace it with a space
				if ( *pApersand == '%' )
					*pApersand = ' ';
			}
			
			// Set the name
			g_engfuncs.pfnSetClientKeyValue( ENTINDEX(pEntity), infobuffer, "name", sName );
			//thePlayer->SetDesiredNetName(MAKE_STRING(sName));
			
			char text[256];
			sprintf( text, "* %s changed name to %s\n", STRING(pEntity->v.netname), g_engfuncs.pfnInfoKeyValue( infobuffer, "name" ) );

			UTIL_SayTextAll(text, CBaseEntity::Instance(ENT(pEntity)));
			
			// team match?
			if ( g_teamplay )
			{
				UTIL_LogPrintf( "\"%s<%i><%s><%s>\" changed name to \"%s\"\n", 
					STRING( pEntity->v.netname ), 
					GETPLAYERUSERID( pEntity ), 
#ifdef USE_UPP
					UPPUtil_GetNetworkID(pEntity).c_str(),
#else
					AvHSUGetPlayerAuthIDString( pEntity ).c_str(),
#endif
					g_engfuncs.pfnInfoKeyValue( infobuffer, "model" ), 
					g_engfuncs.pfnInfoKeyValue( infobuffer, "name" ) );
			}
			else
			{
				UTIL_LogPrintf( "\"%s<%i><%s><%i>\" changed name to \"%s\"\n", 
					STRING( pEntity->v.netname ), 
					GETPLAYERUSERID( pEntity ), 
#ifdef USE_UPP
					UPPUtil_GetNetworkID(pEntity).c_str(),
#else
					AvHSUGetPlayerAuthIDString( pEntity ).c_str(),
#endif
					GETPLAYERUSERID( pEntity ), 
					g_engfuncs.pfnInfoKeyValue( infobuffer, "name" ) );
			}
		}
	}

	g_pGameRules->ClientUserInfoChanged( GetClassPtr((CBasePlayer *)&pEntity->v), infobuffer );
}

static int g_serveractive = 0;

void ServerDeactivate( void )
{
	// It's possible that the engine will call this function more times than is necessary
	//  Therefore, only run it one time for each call to ServerActivate 
	if ( g_serveractive != 1 )
	{
		return;
	}

	g_serveractive = 0;

	// Peform any shutdown operations here...
	//
}

void ServerActivate( edict_t *pEdictList, int edictCount, int clientMax )
{
	int				i;
	CBaseEntity		*pClass;

	// Every call to ServerActivate should be matched by a call to ServerDeactivate
	g_serveractive = 1;

	// Clients have not been initialized yet
	for ( i = 0; i < edictCount; i++ )
	{
		if ( pEdictList[i].free )
			continue;
		
		// Clients aren't necessarily initialized until ClientPutInServer()
		if ( i < clientMax || !pEdictList[i].pvPrivateData )
			continue;

		pClass = CBaseEntity::Instance( &pEdictList[i] );
		// Activate this entity if it's got a class & isn't dormant
		if ( pClass && !(pClass->pev->flags & FL_DORMANT) )
		{
			pClass->Activate();
		}
		else
		{
			ALERT( at_console, "Can't instance %s\n", STRING(pEdictList[i].v.classname) );
		}
	}

	// Link user messages here to make sure first client can get them...
	LinkUserMessages();
}


/*
================
PlayerPreThink

Called every frame before physics are run
================
*/
void PlayerPreThink( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->PreThink( );
}

/*
================
PlayerPostThink

Called every frame after physics are run
================
*/
void PlayerPostThink( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->PostThink( );
}



void ParmsNewLevel( void )
{
}


void ParmsChangeLevel( void )
{
	// retrieve the pointer to the save data
	SAVERESTOREDATA *pSaveData = (SAVERESTOREDATA *)gpGlobals->pSaveData;

	if ( pSaveData )
		pSaveData->connectionCount = BuildChangeList( pSaveData->levelList, MAX_LEVEL_CONNECTIONS );
}

void ShowMenu(entvars_s *pev, int ValidSlots, int DisplayTime, BOOL ShowLater, char Menu[500])
{
	MESSAGE_BEGIN(MSG_ONE, gmsgShowMenu, NULL, pev);	// Begin our message to the client dll
		WRITE_SHORT(ValidSlots);	// Which are the valid slots
		WRITE_CHAR(DisplayTime);	// For how long? (0 = until selection)
		WRITE_BYTE(ShowLater);	// Show later? (TRUE or FALSE)
		WRITE_STRING(Menu);	// And our menu
	MESSAGE_END();	// End the message
}

//
// GLOBALS ASSUMED SET:  g_ulFrameCount
//
void StartFrame( void )
{
	if ( g_pGameRules )
		g_pGameRules->Think();

	if ( g_fGameOver )
		return;

// New SDK changes
//	gpGlobals->teamplay = CVAR_GET_FLOAT("teamplay");
//	gpGlobals->deathmatch = CVAR_GET_FLOAT("deathmatch");
//	g_iSkillLevel = CVAR_GET_FLOAT("skill");
	gpGlobals->teamplay = teamplay.value;

	g_ulFrameCount++;
}

//void PrecacheFootsteps(const char* inBaseName, int inNumber, bool inPrecacheAlienAlso = true)
//{
//	string theBaseString = kFootstepDirectory + kConcreteBaseName;
//
//	for(int i = 1; i <= inNumber; i++)
//	{
//		char theFilename[128];
//		sprintf(theFilename, "%s%d.wav", theBaseString.c_str(), i);
//
//	}
//	//PRECACHE_SOUND("player/pl_step1.wav");		// walk on concrete
//	
//}

void ClientPrecache( void )
{
	// Precache sound lists for all player types
	gSoundListManager.Clear();

	#ifndef AVH_MAPPER_BUILD
	for(int i = ((int)AVH_USER3_NONE + 1); i < (int)AVH_USER3_ALIEN_EMBRYO; i++)
	{
		char theSoundListName[256];
		bool theIsAlien = (i > 3);
		AvHUser3 theUser3 = (AvHUser3)(i);

		bool theLoadSounds = false;

		// No squad leader and commander sounds
		switch(theUser3)
		{
		case AVH_USER3_MARINE_PLAYER:
		case AVH_USER3_ALIEN_PLAYER1:
		case AVH_USER3_ALIEN_PLAYER2:
		case AVH_USER3_ALIEN_PLAYER3:
		case AVH_USER3_ALIEN_PLAYER4:
		case AVH_USER3_ALIEN_PLAYER5:
			theLoadSounds = true;
		}

		if(theLoadSounds)
		{
			// Die sounds
			sprintf(theSoundListName, kPlayerLevelDieSoundList, i);
			gSoundListManager.PrecacheSoundList(theSoundListName);
			
			// Spawn sounds
			sprintf(theSoundListName, kPlayerLevelSpawnSoundList, i);
			gSoundListManager.PrecacheSoundList(theSoundListName);
			
			// Pain sounds
			sprintf(theSoundListName, kPlayerLevelPainSoundList, i);
			gSoundListManager.PrecacheSoundList(theSoundListName);
			
			// Wound sounds
			sprintf(theSoundListName, kPlayerLevelWoundSoundList, i);
			gSoundListManager.PrecacheSoundList(theSoundListName);
			
			// Attack sounds (aliens only)
			if(theIsAlien)
			{
				sprintf(theSoundListName, kPlayerLevelAttackSoundList, i);
				gSoundListManager.PrecacheSoundList(theSoundListName);
				
				// Idle sounds
				sprintf(theSoundListName, kPlayerLevelIdleSoundList, i);
				gSoundListManager.PrecacheSoundList(theSoundListName);
				
				// Movement sounds
				sprintf(theSoundListName, kPlayerLevelMoveSoundList, i);
				gSoundListManager.PrecacheSoundList(theSoundListName);
			}
		}
	}

	// Precache misc. alien sounds
	//PRECACHE_SOUND(kAdrenalineSound);
	PRECACHE_UNMODIFIED_SOUND(kGestationSound);
	PRECACHE_UNMODIFIED_SOUND(kEggDestroyedSound);
	PRECACHE_UNMODIFIED_SOUND(kEggIdleSound);
	PRECACHE_UNMODIFIED_SOUND(kPrimalScreamResponseSound);
	
	// Preacache sayings sound list
	for(i = 1; i <= 9; i++)
	{
		char theSoundListName[256];

		sprintf(theSoundListName, kSoldierSayingList, i);
		gSoundListManager.PrecacheSoundList(theSoundListName);

		sprintf(theSoundListName, kCommanderSayingList, i);
		gSoundListManager.PrecacheSoundList(theSoundListName);
		
		sprintf(theSoundListName, kAlienSayingList, i);
		gSoundListManager.PrecacheSoundList(theSoundListName);
	}

	// Precache order sounds
	gSoundListManager.PrecacheSoundList(kSoldierOrderRequestList);
	gSoundListManager.PrecacheSoundList(kSoldierOrderAckList);

	// Precache other sound lists
	gSoundListManager.PrecacheSoundList(kResourceTowerSoundList);
	gSoundListManager.PrecacheSoundList(kAlienResourceTowerSoundList);
	gSoundListManager.PrecacheSoundList(kHiveWoundSoundList);
	gSoundListManager.PrecacheSoundList(kMarineConstructionSoundList);
	gSoundListManager.PrecacheSoundList(kElectricSparkSoundList);
	gSoundListManager.PrecacheSoundList(kAlienConstructionSoundList);

	// Other equipment and effect sounds
	PRECACHE_UNMODIFIED_SOUND(kMarineBuildingDeploy);
	PRECACHE_UNMODIFIED_SOUND(kJetpackSound);
	PRECACHE_UNMODIFIED_SOUND(kWeldingSound);
	PRECACHE_UNMODIFIED_SOUND(kWeldingHitSound);
	//PRECACHE_UNMODIFIED_SOUND(kOverwatchStartSound);
	//PRECACHE_UNMODIFIED_SOUND(kOverwatchEndSound);
	PRECACHE_UNMODIFIED_SOUND(kRegenerationSound);
	PRECACHE_UNMODIFIED_SOUND(kStartCloakSound);
	PRECACHE_UNMODIFIED_SOUND(kEndCloakSound);
	//PRECACHE_UNMODIFIED_SOUND(kWallJumpSound);
	PRECACHE_UNMODIFIED_SOUND(kWingFlapSound1);
	PRECACHE_UNMODIFIED_SOUND(kWingFlapSound2);
	PRECACHE_UNMODIFIED_SOUND(kWingFlapSound3);
	PRECACHE_UNMODIFIED_SOUND(kSiegeHitSound1);
	PRECACHE_UNMODIFIED_SOUND(kSiegeHitSound2);
	#endif
	
	// setup precaches always needed
	PRECACHE_UNMODIFIED_SOUND("player/sprayer.wav");			// spray paint sound for PreAlpha
	
	// PRECACHE_SOUND("player/pl_jumpland2.wav");		// UNDONE: play 2x step sound

	// Alien step sounds
	PRECACHE_UNMODIFIED_SOUND("player/pl_step1-a.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_step2-a.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_step3-a.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_step4-a.wav");

	PRECACHE_UNMODIFIED_SOUND("player/pl_step1-a1.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_step2-a1.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_step3-a1.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_step4-a1.wav");

	PRECACHE_UNMODIFIED_SOUND("player/pl_step1-a5.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_step2-a5.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_step3-a5.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_step4-a5.wav");

	PRECACHE_UNMODIFIED_SOUND("player/pl_step1-h.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_step2-h.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_step3-h.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_step4-h.wav");

	// Marine step sounds
	PRECACHE_UNMODIFIED_SOUND("player/pl_fallpain3-1.wav");		
	PRECACHE_UNMODIFIED_SOUND("player/pl_fallpain3-2.wav");		
	PRECACHE_UNMODIFIED_SOUND("player/pl_fallpain3-3.wav");		
	PRECACHE_UNMODIFIED_SOUND("player/pl_fallpain3-4.wav");		
	PRECACHE_UNMODIFIED_SOUND("player/pl_fallpain3-5.wav");		
	PRECACHE_UNMODIFIED_SOUND("player/pl_fallpain3-6.wav");		
	PRECACHE_UNMODIFIED_SOUND("player/pl_fallpain3-7.wav");		
	PRECACHE_UNMODIFIED_SOUND("player/pl_fallpain3-8.wav");		
	PRECACHE_UNMODIFIED_SOUND(kDigestingSound);

	PRECACHE_UNMODIFIED_SOUND("player/pl_step1.wav");		// walk on concrete
	PRECACHE_UNMODIFIED_SOUND("player/pl_step2.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_step3.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_step4.wav");

	PRECACHE_SOUND("common/npc_step1.wav");		// NPC walk on concrete
	PRECACHE_SOUND("common/npc_step2.wav");
	PRECACHE_SOUND("common/npc_step3.wav");
	PRECACHE_SOUND("common/npc_step4.wav");

	PRECACHE_UNMODIFIED_SOUND("player/pl_metal1.wav");		// walk on metal
	PRECACHE_UNMODIFIED_SOUND("player/pl_metal2.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_metal3.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_metal4.wav");

	PRECACHE_UNMODIFIED_SOUND("player/pl_dirt1.wav");		// walk on dirt
	PRECACHE_UNMODIFIED_SOUND("player/pl_dirt2.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_dirt3.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_dirt4.wav");

	PRECACHE_UNMODIFIED_SOUND("player/pl_duct1.wav");		// walk in duct
	PRECACHE_UNMODIFIED_SOUND("player/pl_duct2.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_duct3.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_duct4.wav");

	PRECACHE_UNMODIFIED_SOUND("player/pl_grate1.wav");		// walk on grate
	PRECACHE_UNMODIFIED_SOUND("player/pl_grate2.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_grate3.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_grate4.wav");

	PRECACHE_UNMODIFIED_SOUND("player/pl_slosh1.wav");		// walk in shallow water
	PRECACHE_UNMODIFIED_SOUND("player/pl_slosh2.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_slosh3.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_slosh4.wav");

	PRECACHE_UNMODIFIED_SOUND("player/pl_tile1.wav");		// walk on tile
	PRECACHE_UNMODIFIED_SOUND("player/pl_tile2.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_tile3.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_tile4.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_tile5.wav");

	PRECACHE_UNMODIFIED_SOUND("player/pl_swim1.wav");		// breathe bubbles
	PRECACHE_UNMODIFIED_SOUND("player/pl_swim2.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_swim3.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_swim4.wav");

	PRECACHE_UNMODIFIED_SOUND("player/pl_ladder1.wav");	// climb ladder rung
	PRECACHE_UNMODIFIED_SOUND("player/pl_ladder2.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_ladder3.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_ladder4.wav");

	PRECACHE_UNMODIFIED_SOUND("player/pl_wade1.wav");		// wade in water
	PRECACHE_UNMODIFIED_SOUND("player/pl_wade2.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_wade3.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_wade4.wav");

	PRECACHE_SOUND("debris/wood1.wav");			// hit wood texture
	PRECACHE_SOUND("debris/wood2.wav");
	PRECACHE_SOUND("debris/wood3.wav");

	PRECACHE_UNMODIFIED_SOUND("plats/train_use1.wav");		// use a train

	PRECACHE_UNMODIFIED_SOUND("buttons/spark5.wav");		// hit computer texture
	PRECACHE_UNMODIFIED_SOUND("buttons/spark6.wav");
	PRECACHE_SOUND("debris/glass1.wav");
	PRECACHE_SOUND("debris/glass2.wav");
	PRECACHE_SOUND("debris/glass3.wav");

	PRECACHE_UNMODIFIED_SOUND( SOUND_FLASHLIGHT_ON );
	PRECACHE_UNMODIFIED_SOUND( SOUND_FLASHLIGHT_OFF );

// player gib sounds
	PRECACHE_SOUND("common/bodysplat.wav");		               

// player pain sounds
	PRECACHE_UNMODIFIED_SOUND("player/pl_pain2.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_pain4.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_pain5.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_pain6.wav");
	PRECACHE_UNMODIFIED_SOUND("player/pl_pain7.wav");

	PRECACHE_UNMODIFIED_MODEL("models/player.mdl");
	PRECACHE_UNMODIFIED_MODEL(kReadyRoomModel);

	PRECACHE_UNMODIFIED_MODEL(kMarineSoldierModel);
	PRECACHE_UNMODIFIED_MODEL(kHeavySoldierModel);
	PRECACHE_UNMODIFIED_MODEL(kAlienLevelOneModel);

	PRECACHE_UNMODIFIED_MODEL(kAlienLevelTwoModel);
	PRECACHE_UNMODIFIED_MODEL(kAlienLevelThreeModel);
	PRECACHE_UNMODIFIED_MODEL(kAlienLevelFourModel);
	PRECACHE_UNMODIFIED_MODEL(kAlienLevelFiveModel);

    PRECACHE_UNMODIFIED_MODEL(kMarineCommanderModel);
    PRECACHE_UNMODIFIED_MODEL(kAlienGestateModel);


	// hud sounds, for marines and aliens (change AvHSharedUtil::AvHSHUGetCommonSoundName if these sound names change)
	PRECACHE_UNMODIFIED_SOUND("common/wpn_hudoff.wav");
	PRECACHE_UNMODIFIED_SOUND("common/wpn_hudon.wav");
	PRECACHE_UNMODIFIED_SOUND("common/wpn_moveselect.wav");
	PRECACHE_UNMODIFIED_SOUND("common/wpn_select.wav");
	PRECACHE_UNMODIFIED_SOUND("common/wpn_denyselect.wav");

	PRECACHE_UNMODIFIED_SOUND("common/wpn_hudoff-a.wav");
	PRECACHE_UNMODIFIED_SOUND("common/wpn_hudon-a.wav");
	PRECACHE_UNMODIFIED_SOUND("common/wpn_moveselect-a.wav");

	PRECACHE_UNMODIFIED_SOUND("common/wpn_select-a.wav");
	PRECACHE_UNMODIFIED_SOUND("common/wpn_denyselect-a.wav");
	
	// geiger sounds
	PRECACHE_UNMODIFIED_SOUND("player/geiger6.wav");
	PRECACHE_UNMODIFIED_SOUND("player/geiger5.wav");
	PRECACHE_UNMODIFIED_SOUND("player/geiger4.wav");
	PRECACHE_UNMODIFIED_SOUND("player/geiger3.wav");
	PRECACHE_UNMODIFIED_SOUND("player/geiger2.wav");
	PRECACHE_UNMODIFIED_SOUND("player/geiger1.wav");

	PRECACHE_UNMODIFIED_MODEL("sprites/muzzleflash1.spr");
	PRECACHE_UNMODIFIED_MODEL("sprites/muzzleflash2.spr"); 
	PRECACHE_UNMODIFIED_MODEL("sprites/muzzleflash3.spr"); 
	PRECACHE_UNMODIFIED_MODEL("sprites/digesting.spr"); 
	PRECACHE_UNMODIFIED_MODEL("sprites/membrane.spr"); 
	PRECACHE_UNMODIFIED_MODEL("sprites/hera_fog.spr"); 
	PRECACHE_UNMODIFIED_MODEL("sprites/spore.spr"); 
	PRECACHE_UNMODIFIED_MODEL("sprites/spore2.spr"); 
	PRECACHE_UNMODIFIED_MODEL("sprites/umbra.spr"); 
	PRECACHE_UNMODIFIED_MODEL("sprites/umbra2.spr"); 
	PRECACHE_UNMODIFIED_MODEL("sprites/webstrand.spr"); 
	
	if (giPrecacheGrunt)
		UTIL_PrecacheOther("monster_human_grunt");

}

/*
================
Sys_Error

  Engine is going to shut down, allows setting a breakpoint in game .dll to catch that occasion
  ================
  */
  void Sys_Error( const char *error_string )
  {
#ifdef DEBUG	  
#ifdef WIN32
		// Default case, do nothing.  MOD AUTHORS:  Add code ( e.g., _asm { int 3 }; here to cause a breakpoint for debugging your game .dlls
		_asm { int 3 };
#endif
#else
		char theDebugString[2056];
		sprintf(theDebugString, "Sys_Error: %s\n", error_string);
		ALERT(at_error, theDebugString); 
#endif

}

/*
===============
GetGameDescription

Returns the descriptive name of this .dll.  E.g., Half-Life, or Team Fortress 2
===============
*/
const char *GetGameDescription()
{
	if ( g_pGameRules ) // this function may be called before the world has spawned, and the game rules initialized
		return g_pGameRules->GetGameDescription();
	else
		return "Half-Life";
}

/*
================
PlayerCustomization

A new player customization has been registered on the server
UNDONE:  This only sets the # of frames of the spray can logo
animation right now.
================
*/
void PlayerCustomization( edict_t *pEntity, customization_t *pCust )
{
	entvars_t *pev = &pEntity->v;
	CBasePlayer *pPlayer = (CBasePlayer *)GET_PRIVATE(pEntity);

	if (!pPlayer)
	{
		ALERT(at_console, "PlayerCustomization:  Couldn't get player!\n");
		return;
	}

	if (!pCust)
	{
		ALERT(at_console, "PlayerCustomization:  NULL customization!\n");
		return;
	}

	switch (pCust->resource.type)
	{
	case t_decal:
		pPlayer->SetCustomDecalFrames(pCust->nUserData2); // Second int is max # of frames.
		break;
	case t_sound:
	case t_skin:
	case t_model:
		// Ignore for now.
		break;
	default:
		ALERT(at_console, "PlayerCustomization:  Unknown customization type!\n");
		break;
	}
}

/*
================
SpectatorConnect

A spectator has joined the game
================
*/
void SpectatorConnect( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorConnect( );
}

/*
================
SpectatorConnect

A spectator has left the game
================
*/
void SpectatorDisconnect( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorDisconnect( );
}

/*
================
SpectatorConnect

A spectator has sent a usercmd
================
*/
void SpectatorThink( edict_t *pEntity )
{
	entvars_t *pev = &pEntity->v;
	CBaseSpectator *pPlayer = (CBaseSpectator *)GET_PRIVATE(pEntity);

	if (pPlayer)
		pPlayer->SpectatorThink( );
}

////////////////////////////////////////////////////////
// PAS and PVS routines for client messaging
//

/*
================
SetupVisibility

A client can have a separate "view entity" indicating that his/her view should depend on the origin of that
view entity.  If that's the case, then pViewEntity will be non-NULL and will be used.  Otherwise, the current
entity's origin is used.  Either is offset by the view_ofs to get the eye position.

From the eye position, we set up the PAS and PVS to use for filtering network messages to the client.  At this point, we could
 override the actual PAS or PVS values, or use a different origin.

NOTE:  Do not cache the values of pas and pvs, as they depend on reusable memory in the engine, they are only good for this one frame
================
*/
void SetupVisibility( edict_t *pViewEntity, edict_t *pClient, unsigned char **pvs, unsigned char **pas )
{
	Vector org;
	edict_t *pView = pClient;
	bool theUseSpecialPASOrigin = false;
	Vector theSpecialPASOrigin;

	// Find the client's PVS
	if ( pViewEntity )
	{
		pView = pViewEntity;
	}

	// Proxy sees and hears all
	if ( (pClient->v.flags & FL_PROXY) || GetGameRules()->GetIsCheatEnabled(kcViewAll))
	{
		//ALERT(at_logged, "Setting PVS and PAS to NULL for FL_PROXY\n");
		*pvs = NULL;	// the spectator proxy sees
		*pas = NULL;	// and hears everything
		return;
	}

	// Tracking Spectators use the visibility of their target
	CBasePlayer *pPlayer = (CBasePlayer *)CBaseEntity::Instance( pClient );
	if ( (pPlayer->pev->iuser2 != 0) && (pPlayer->m_hObserverTarget != NULL) &&  (pPlayer->m_afPhysicsFlags & PFLAG_OBSERVER) && pPlayer->IsAlive())
	{
		pView = pPlayer->m_hObserverTarget->edict();
		UTIL_SetOrigin( pPlayer->pev, pPlayer->m_hObserverTarget->pev->origin );
	}

	// Tracking Spectators use the visibility of their target
//	AvHPlayer *thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(pClient));

	// cgc - something could be wrong here, crashing from Robin's spectator code...did I do something wrong?
//	if(thePlayer)
//	{
//		if ( (thePlayer->pev->iuser2 != 0) && (thePlayer->m_hObserverTarget != NULL) )
//		{
//			pView = thePlayer->m_hObserverTarget->edict();
//		}
//
//		if(thePlayer->GetRole() == ROLE_COMMANDER)
//		{
//			thePlayer->GetSpecialPASOrigin(theSpecialPASOrigin);
//			theUseSpecialPASOrigin = true;
//		}
//	}

	org = pView->v.origin + pView->v.view_ofs;
	if ( pView->v.flags & FL_DUCKING )
	{
		org = org + ( VEC_HULL_MIN - VEC_DUCK_HULL_MIN );
	}

	*pvs = ENGINE_SET_PVS ( (float *)&org );

	if(theUseSpecialPASOrigin)
	{
		*pas = ENGINE_SET_PAS ( (float *)&theSpecialPASOrigin );
	}
	else
	{
		*pas = ENGINE_SET_PAS ( (float *)&org );
	}
}

#include "entity_state.h"

//CachedEntity	gCachedEntities[kMaxPlayers][kMaxEntities];
//
//void ResetCachedEntities()
//{
//	for(int i = 0; i < kMaxPlayers; i++)
//	{
//		for(int j = 0; j < kMaxEntities; j++)
//		{
//			gCachedEntities[i][j].ResetCachedEntity();
//		}
//	}
//}



// Array sizes ( memory is 5624 * 32 = 179968 bytes )
#define MAX_CLIENTS  ( 32 )
#define MAX_ENTITIES ( 900 + MAX_CLIENTS * 15 )

// Re-check entities only this often once they are found to be in the PVS
// NOTE:  We could have a separate timeout for players if we wanted to tweak the coherency time for them ( you'd have
//  to check the entitynum against 1 to gpGlobals->maxclients ), etc.
//#define PVS_COHERENCY_TIME 1.0f

extern float kPVSCoherencyTime;

typedef struct
{
	bool	mLastVisibility;
	float	m_fTimeEnteredPVS;
} ENTITYPVSSTATUS;

typedef struct
{
	ENTITYPVSSTATUS m_Status[ MAX_ENTITIES ];
	
	// Remember player leaf data so we can invalidate pvs history when leaf changes
	int   headnode;   // -1 to use normal leaf check
	int   num_leafs;
	short  leafnums[ MAX_ENT_LEAFS ];
	
} PLAYERPVSSTATUS;

static PLAYERPVSSTATUS g_PVSStatus[ MAX_CLIENTS ];

void ResetPlayerPVS( edict_t *client, int clientnum )
{
	ASSERT( clientnum >=0 && clientnum < MAX_CLIENTS );
	
	PLAYERPVSSTATUS *pvs = &g_PVSStatus[ clientnum ];
	
	memset( pvs, 0, sizeof( PLAYERPVSSTATUS ) );
	
	// Copy leaf data in right away
	pvs->headnode = client->headnode;
	pvs->num_leafs = client->num_leafs;
	memcpy( pvs->leafnums, client->leafnums, sizeof( pvs->leafnums ) );
}

bool TimeToResetPVS( edict_t *client, int clientnum )
{
	ASSERT( clientnum >=0 && clientnum < MAX_CLIENTS );
	
	PLAYERPVSSTATUS *pvs = &g_PVSStatus[ clientnum ];
	
	if ( (pvs->headnode != client->headnode) || (pvs->num_leafs != client->num_leafs))
		return true;
	
	if ( client->num_leafs > 0 )
	{
		for ( int i = 0; i < client->num_leafs; i++ )
		{
			if ( client->leafnums[ i ] != pvs->leafnums[ i ] )
			{
				return true;
			}
		}
	}
	
	// Still the same
	return false;
}

void MarkEntityInPVS( int clientnum, int entitynum, float time, bool inpvs )
{
	ASSERT( clientnum >=0 && clientnum < MAX_CLIENTS );
	ASSERT( entitynum >= 0 && entitynum < MAX_ENTITIES );
	
	PLAYERPVSSTATUS *pvs = &g_PVSStatus[ clientnum ];
	ENTITYPVSSTATUS *es  = &pvs->m_Status[ entitynum ];
	
	es->m_fTimeEnteredPVS = time;
	es->mLastVisibility = inpvs;
}

bool GetTimeToRecompute( int clientnum, int entitynum, float currenttime, bool& outCachedVisibility)
{
	ASSERT( clientnum >=0 && clientnum < MAX_CLIENTS );
	ASSERT( entitynum >= 0 && entitynum < MAX_ENTITIES );
	
	PLAYERPVSSTATUS *pvs = &g_PVSStatus[ clientnum ];
	ENTITYPVSSTATUS *es = &pvs->m_Status[ entitynum ];

	bool theTimeToRecompute = false;

	int theUseCaching = BALANCE_IVAR(kDebugServerCaching);
	if(theUseCaching)
	{
		// Always recompute players?
		if((entitynum > 0) && (entitynum < MAX_CLIENTS))
		{
			theTimeToRecompute = true;
		}
		// If we haven't computed for awhile, or our PVS has been reset
		else if((currenttime > (es->m_fTimeEnteredPVS + kPVSCoherencyTime)) || (es->m_fTimeEnteredPVS == 0.0f))
		{
			theTimeToRecompute = true;
		}
		else
		{
			outCachedVisibility = es->mLastVisibility; 
		}
	}
	else
	{
		theTimeToRecompute = true;
	}

	return theTimeToRecompute;
}

bool CheckEntityRecentlyInPVS( int clientnum, int entitynum, float currenttime )
{
	ASSERT( clientnum >=0 && clientnum < MAX_CLIENTS );
	ASSERT( entitynum >= 0 && entitynum < MAX_ENTITIES );
	
	PLAYERPVSSTATUS *pvs = &g_PVSStatus[ clientnum ];
	ENTITYPVSSTATUS *es = &pvs->m_Status[ entitynum ];
 
	int theUseCaching = BALANCE_IVAR(kDebugServerCaching);
	if(!theUseCaching)
	{
		return false;
	}
	
	// Wasn't in pvs before, sigh...
	if ( es->m_fTimeEnteredPVS == 0.0f )
	{
		return false;
	}
	
	// If we've been saying yes for kPVSCoherencyTime, say no now instead so that we'll do a full check
	if ( es->m_fTimeEnteredPVS + kPVSCoherencyTime < currenttime )
	{
		return false;
	}
	
	// Keep assuming it's in the pvs for a bit of time (trivial inclusion)
	return true;
}


int kNumReturn0 = 0;
int kNumReturn1 = 0;
int kNumCached = 0;
int kNumComputed = 0;
int kMaxE = 0;
int kNumEntsProcessedForPlayerOne = 0;
int kMaxEProcessedForPlayerOne = 0;
extern int kServerFrameRate;

// ENGINE_CHECK_VISIBILITY, from Yahn, with his comments:
// 
// Here's the engine code.  You'll note a slow path if the ent leaf cache is missed, which causes a recomputation via the much more expensive 
// CM_HeadnodeVisible call.  That does change the cache data on the entity, so that's likely your problem with blinking if you are doing some 
// kind of memcpy operation on the old data.
//#ifdef 0
//int DLL_CALLBACK SV_CheckVisibility( const struct edict_s *entity, unsigned char *pset )
//{
// int i;
// qboolean visible;
// edict_t *e;
// 
// if ( !pset )
//	 return 1;
// 
// if ( entity->headnode >= 0 )   //
// {
//	 // we use num_leafs as a cache/counter, it will circle around, but so what
//	 //  if we don't find the leaf we want here, we'll end up doing the full test anyway
//	 int leaf;
//	 i = 0; 
//	 
//	 do
//	 {
//		 leaf = entity->leafnums[i];
//		 // Cache is all -1 to start...
//		 if ( leaf == -1 )
//			 break;
//		 
//		 if ( pset[ leaf >> 3 ] & (1 << (leaf & 7 ) ) )
//		 {
//			 return 1;
//		 }
//		 
//		 i++;
//		 if ( i >= MAX_ENT_LEAFS )
//			 break;
//		 
//	 } while ( 1 );
//	 
//	 // Didn't find it in "cache"
//	 
//	 visible = CM_HeadnodeVisible( sv.worldmodel->nodes + entity->headnode, pset, &leaf );
//	 if ( !visible )
//	 {
//		 return 0;
//	 }
//	 
//	 // Cache leaf that was visible
//	 //
//	 e = ( edict_t * )entity;
//	 
//	 e->leafnums[ entity->num_leafs ] = (short)leaf;
//	 e->num_leafs = ( entity->num_leafs + 1 ) % MAX_ENT_LEAFS;
//	 
//	 return 2;
// }
// else
// {
//	 for (i=0 ; i < entity->num_leafs ; i++)
//	 {
//		 if ( pset[entity->leafnums[i] >> 3] & (1 << (entity->leafnums[i]&7) ))
//			 break;
//	 }
//	 
//	 visible = ( i < entity->num_leafs );
//	 if ( !visible )
//	 {
//		 return 0;
//	 }
// }
// 
// return 1;
//}
//#endif

// defaults for clientinfo messages
#define	DEFAULT_VIEWHEIGHT	28

/*
===================
CreateBaseline

Creates baselines used for network encoding, especially for player data since players are not spawned until connect time.
===================
*/
void CreateBaseline( int player, int eindex, struct entity_state_s *baseline, struct edict_s *entity, int playermodelindex, vec3_t player_mins, vec3_t player_maxs )
{
	baseline->origin		= entity->v.origin;
	baseline->angles		= entity->v.angles;
	baseline->frame			= entity->v.frame;
	baseline->skin			= (short)entity->v.skin;

	// render information
	baseline->rendermode	= (byte)entity->v.rendermode;
	baseline->renderamt		= (byte)entity->v.renderamt;
	baseline->rendercolor.r	= (byte)entity->v.rendercolor.x;
	baseline->rendercolor.g	= (byte)entity->v.rendercolor.y;
	baseline->rendercolor.b	= (byte)entity->v.rendercolor.z;
	baseline->renderfx		= (byte)entity->v.renderfx;

	if ( player )
	{
		baseline->mins			= player_mins;
		baseline->maxs			= player_maxs;

		baseline->colormap		= eindex;
		baseline->modelindex	= playermodelindex;
		baseline->friction		= 1.0;
		baseline->movetype		= MOVETYPE_WALK;

		baseline->scale			= entity->v.scale;
		baseline->solid			= SOLID_SLIDEBOX;
		baseline->framerate		= 1.0;
		baseline->gravity		= 1.0;

	}
	else
	{
		baseline->mins			= entity->v.mins;
		baseline->maxs			= entity->v.maxs;

		baseline->colormap		= 0;
		baseline->modelindex	= entity->v.modelindex;//SV_ModelIndex(pr_strings + entity->v.model);
		baseline->movetype		= entity->v.movetype;

		baseline->scale			= entity->v.scale;
		baseline->solid			= entity->v.solid;
		baseline->framerate		= entity->v.framerate;
		baseline->gravity		= entity->v.gravity;
	}
}

typedef struct
{
	char name[32];
	int	 field;
} entity_field_alias_t;

#define FIELD_ORIGIN0			0
#define FIELD_ORIGIN1			1
#define FIELD_ORIGIN2			2
#define FIELD_ANGLES0			3
#define FIELD_ANGLES1			4
#define FIELD_ANGLES2			5

static entity_field_alias_t entity_field_alias[]=
{
	{ "origin[0]",			0 },
	{ "origin[1]",			0 },
	{ "origin[2]",			0 },
	{ "angles[0]",			0 },
	{ "angles[1]",			0 },
	{ "angles[2]",			0 },
};

void Entity_FieldInit( struct delta_s *pFields )
{
	entity_field_alias[ FIELD_ORIGIN0 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ORIGIN0 ].name );
	entity_field_alias[ FIELD_ORIGIN1 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ORIGIN1 ].name );
	entity_field_alias[ FIELD_ORIGIN2 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ORIGIN2 ].name );
	entity_field_alias[ FIELD_ANGLES0 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ANGLES0 ].name );
	entity_field_alias[ FIELD_ANGLES1 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ANGLES1 ].name );
	entity_field_alias[ FIELD_ANGLES2 ].field		= DELTA_FINDFIELD( pFields, entity_field_alias[ FIELD_ANGLES2 ].name );
}

/*
==================
Entity_Encode

Callback for sending entity_state_t info over network. 
FIXME:  Move to script
==================
*/
void Entity_Encode( struct delta_s *pFields, const unsigned char *from, const unsigned char *to )
{
	entity_state_t *f, *t;
	int localplayer = 0;
	static int initialized = 0;

	if ( !initialized )
	{
		Entity_FieldInit( pFields );
		initialized = 1;
	}

	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	// Never send origin to local player, it's sent with more resolution in clientdata_t structure
	localplayer =  ( t->number - 1 ) == ENGINE_CURRENT_PLAYER();
	if ( localplayer )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}

	if ( ( t->impacttime != 0 ) && ( t->starttime != 0 ) )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );

		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ANGLES0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ANGLES1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ANGLES2 ].field );
	}

	if ( ( t->movetype == MOVETYPE_FOLLOW ) &&
		 ( t->aiment != 0 ) )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}
	else if ( t->aiment != f->aiment )
	{
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}
}

static entity_field_alias_t player_field_alias[]=
{
	{ "origin[0]",			0 },
	{ "origin[1]",			0 },
	{ "origin[2]",			0 },
};

void Player_FieldInit( struct delta_s *pFields )
{
	player_field_alias[ FIELD_ORIGIN0 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ORIGIN0 ].name );
	player_field_alias[ FIELD_ORIGIN1 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ORIGIN1 ].name );
	player_field_alias[ FIELD_ORIGIN2 ].field		= DELTA_FINDFIELD( pFields, player_field_alias[ FIELD_ORIGIN2 ].name );
}

/*
==================
Player_Encode

Callback for sending entity_state_t for players info over network. 
==================
*/
void Player_Encode( struct delta_s *pFields, const unsigned char *from, const unsigned char *to )
{
	entity_state_t *f, *t;
	int localplayer = 0;
	static int initialized = 0;

	if ( !initialized )
	{
		Player_FieldInit( pFields );
		initialized = 1;
	}

	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	// Never send origin to local player, it's sent with more resolution in clientdata_t structure
	localplayer =  ( t->number - 1 ) == ENGINE_CURRENT_PLAYER();
	if ( localplayer )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}

	if ( ( t->movetype == MOVETYPE_FOLLOW ) &&
		 ( t->aiment != 0 ) )
	{
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}
	else if ( t->aiment != f->aiment )
	{
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN0 ].field );
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN1 ].field );
		DELTA_SETBYINDEX( pFields, entity_field_alias[ FIELD_ORIGIN2 ].field );
	}
}

#define CUSTOMFIELD_ORIGIN0			0
#define CUSTOMFIELD_ORIGIN1			1
#define CUSTOMFIELD_ORIGIN2			2
#define CUSTOMFIELD_ANGLES0			3
#define CUSTOMFIELD_ANGLES1			4
#define CUSTOMFIELD_ANGLES2			5
#define CUSTOMFIELD_SKIN			6
#define CUSTOMFIELD_SEQUENCE		7
#define CUSTOMFIELD_ANIMTIME		8

entity_field_alias_t custom_entity_field_alias[]=
{
	{ "origin[0]",			0 },
	{ "origin[1]",			0 },
	{ "origin[2]",			0 },
	{ "angles[0]",			0 },
	{ "angles[1]",			0 },
	{ "angles[2]",			0 },
	{ "skin",				0 },
	{ "sequence",			0 },
	{ "animtime",			0 },
};

void Custom_Entity_FieldInit( struct delta_s *pFields )
{
	custom_entity_field_alias[ CUSTOMFIELD_ORIGIN0 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN0 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ORIGIN1 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN1 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ORIGIN2 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN2 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANGLES0 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES0 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANGLES1 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES1 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANGLES2 ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES2 ].name );
	custom_entity_field_alias[ CUSTOMFIELD_SKIN ].field	= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_SKIN ].name );
	custom_entity_field_alias[ CUSTOMFIELD_SEQUENCE ].field= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_SEQUENCE ].name );
	custom_entity_field_alias[ CUSTOMFIELD_ANIMTIME ].field= DELTA_FINDFIELD( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANIMTIME ].name );
}

/*
==================
Custom_Encode

Callback for sending entity_state_t info ( for custom entities ) over network. 
FIXME:  Move to script
==================
*/
void Custom_Encode( struct delta_s *pFields, const unsigned char *from, const unsigned char *to )
{
	entity_state_t *f, *t;
	int beamType;
	static int initialized = 0;

	if ( !initialized )
	{
		Custom_Entity_FieldInit( pFields );
		initialized = 1;
	}

	f = (entity_state_t *)from;
	t = (entity_state_t *)to;

	beamType = t->rendermode & 0x0f;
		
	if ( beamType != BEAM_POINTS && beamType != BEAM_ENTPOINT )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN0 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN1 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ORIGIN2 ].field );
	}

	if ( beamType != BEAM_POINTS )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES0 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES1 ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANGLES2 ].field );
	}

	if ( beamType != BEAM_ENTS && beamType != BEAM_ENTPOINT )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_SKIN ].field );
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_SEQUENCE ].field );
	}

	// animtime is compared by rounding first
	// see if we really shouldn't actually send it
	if ( (int)f->animtime == (int)t->animtime )
	{
		DELTA_UNSETBYINDEX( pFields, custom_entity_field_alias[ CUSTOMFIELD_ANIMTIME ].field );
	}
}

/*
=================
RegisterEncoders

Allows game .dll to override network encoding of certain types of entities and tweak values, etc.
=================
*/
void RegisterEncoders( void )
{
	DELTA_ADDENCODER( "Entity_Encode", Entity_Encode );
	DELTA_ADDENCODER( "Custom_Encode", Custom_Encode );
	DELTA_ADDENCODER( "Player_Encode", Player_Encode );
}

int GetWeaponData( struct edict_s *player, struct weapon_data_s *info )
{
#if defined( CLIENT_WEAPONS )
	int i;
	weapon_data_t *item;
	entvars_t *pev = &player->v;
	CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev );
	AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(pl);
	CBasePlayerWeapon *gun;
	
	ItemInfo II;

	memset( info, 0, 32 * sizeof( weapon_data_t ) );

	if ( !pl )
		return 1;

	// go through all of the weapons and make a list of the ones to pack
	for ( i = 0 ; i < MAX_ITEM_TYPES ; i++ )
	{
		if ( pl->m_rgpPlayerItems[ i ] )
		{
			// there's a weapon here. Should I pack it?
			CBasePlayerItem *pPlayerItem = pl->m_rgpPlayerItems[ i ];

			while ( pPlayerItem )
			{
				gun = (CBasePlayerWeapon *)pPlayerItem->GetWeaponPtr();
				if ( gun && gun->UseDecrement() )
				{
					// Get The ID.
					memset( &II, 0, sizeof( II ) );
					gun->GetItemInfo( &II );

					ASSERT((II.iId >= 0) && (II.iId < 32));

					if ( II.iId >= 0 && II.iId < 32 )
					{
						item = &info[ II.iId ];
					 	
						item->m_iId						= II.iId;
						item->m_iClip					= gun->m_iClip;

						item->m_flTimeWeaponIdle		= max( gun->m_flTimeWeaponIdle, -0.001 );
						item->m_flNextPrimaryAttack		= max( gun->m_flNextPrimaryAttack, -0.001 );
						item->m_flNextSecondaryAttack	= max( gun->m_flNextSecondaryAttack, -0.001 );
						item->m_fInReload				= gun->m_fInReload;

						//thePlayer->SetDebugCSP(item);

						//item->m_fInSpecialReload		= gun->m_fInSpecialReload;
						//item->fuser1					= max( gun->pev->fuser1, -0.001 );
						item->fuser2					= gun->m_flStartThrow;
						item->fuser3					= gun->m_flReleaseThrow;
						//item->iuser1					= gun->m_chargeReady;
						//item->iuser2					= gun->m_fInAttack;

						// Pass along enabled state in iuser3 (for when hives and ensnare enable and disable weapons)
						item->iuser3					= gun->pev->iuser3;
						
						//item->m_flPumpTime				= max( gun->m_flPumpTime, -0.001 );
					}
				}
				pPlayerItem = pPlayerItem->m_pNext;
			}
		}
	}
#else
	memset( info, 0, 32 * sizeof( weapon_data_t ) );
#endif
	return 1;
}

/*
=================
UpdateClientData

Data sent to current client only
engine sets cd to 0 before calling.
=================
*/
void UpdateClientData ( const struct edict_s *ent, int sendweapons, struct clientdata_s *cd )
{
	cd->flags			= ent->v.flags;
	cd->health			= ent->v.health;

	cd->viewmodel		= MODEL_INDEX( STRING( ent->v.viewmodel ) );

	cd->waterlevel		= ent->v.waterlevel;
	cd->watertype		= ent->v.watertype;
	cd->weapons			= ent->v.weapons;

	// Vectors
	cd->origin			= ent->v.origin;
	cd->velocity		= ent->v.velocity;
	cd->view_ofs		= ent->v.view_ofs;
	cd->punchangle		= ent->v.punchangle;

	cd->bInDuck			= ent->v.bInDuck;
	cd->flTimeStepSound = ent->v.flTimeStepSound;
	cd->flDuckTime		= ent->v.flDuckTime;
	cd->flSwimTime		= ent->v.flSwimTime;
	cd->waterjumptime	= ent->v.teleport_time;

	strcpy( cd->physinfo, ENGINE_GETPHYSINFO( ent ) );

	cd->maxspeed		= ent->v.maxspeed;
	cd->fov				= ent->v.fov;
	cd->weaponanim		= ent->v.weaponanim;

	cd->pushmsec		= ent->v.pushmsec;

	// Spectator
	cd->iuser1			= ent->v.iuser1;
	cd->iuser2			= ent->v.iuser2;

	// AvH specials
	cd->iuser3			= ent->v.iuser3;
	cd->iuser4			= ent->v.iuser4;
	cd->fuser1			= ent->v.fuser1;
	cd->fuser2			= ent->v.fuser2;
	cd->fuser3			= ent->v.fuser3;

	// Added by mmcguire for oriented skulk player models.
	cd->vuser1          = ent->v.vuser1;

	// Added for extra player info for first-person spectating
	cd->vuser4          = ent->v.vuser4;

#if defined( CLIENT_WEAPONS )
	if ( sendweapons )
	{
		entvars_t *pev = (entvars_t *)&ent->v;
		CBasePlayer *pl = ( CBasePlayer *) CBasePlayer::Instance( pev );

		if ( pl )
		{
			cd->m_flNextAttack	= pl->m_flNextAttack;
			//cd->fuser2			= pl->m_flNextAmmoBurn;
			//cd->fuser3			= pl->m_flAmmoStartCharge;
			//cd->vuser1.x		= pl->ammo_9mm;
			//cd->vuser1.y		= pl->ammo_357;
			//cd->vuser1.z		= pl->ammo_argrens;
			//cd->ammo_nails		= pl->ammo_bolts;
			//cd->ammo_shells		= pl->ammo_buckshot;
			//cd->ammo_rockets	= pl->ammo_rockets;
			//cd->ammo_cells		= pl->ammo_uranium;
			//cd->vuser2.x		= pl->ammo_hornets;

			if ( pl->m_pActiveItem )
			{
				CBasePlayerWeapon *gun;
				gun = (CBasePlayerWeapon *)pl->m_pActiveItem->GetWeaponPtr();
				if ( gun && gun->UseDecrement() )
				{
					ItemInfo II;
					memset( &II, 0, sizeof( II ) );
					gun->GetItemInfo( &II );

					cd->m_iId = II.iId;

//					cd->vuser3.z	= gun->m_iSecondaryAmmoType;
//					cd->vuser4.x	= gun->m_iPrimaryAmmoType;
//					cd->vuser4.y	= pl->m_rgAmmo[gun->m_iPrimaryAmmoType];
//					cd->vuser4.z	= pl->m_rgAmmo[gun->m_iSecondaryAmmoType];
//					
//					if ( pl->m_pActiveItem->m_iId == WEAPON_RPG )
//					{
//						cd->vuser2.y = ( ( CRpg * )pl->m_pActiveItem)->m_fSpotActive;
//						cd->vuser2.z = ( ( CRpg * )pl->m_pActiveItem)->m_cActiveRockets;
//					}
				}
			}
		}
	}
#endif
}

/*
=================
CmdStart

We're about to run this usercmd for the specified player.  We can set up groupinfo and masking here, etc.
This is the time to examine the usercmd for anything extra.  This call happens even if think does not.
=================
*/
void CmdStart( const edict_t *player, const struct usercmd_s *cmd, unsigned int random_seed )
{
	entvars_t *pev = (entvars_t *)&player->v;
	AvHPlayer *pl = ( AvHPlayer *) CBasePlayer::Instance( pev );

	if( !pl )
		return;

	pl->SetCurrentCommand(cmd);

	pl->SetSizeForUser3();

	if ( pl->pev->groupinfo != 0 )
	{
		UTIL_SetGroupTrace( pl->pev->groupinfo, GROUP_OP_AND );
	}

	pl->random_seed = random_seed;
}

/*
=================
CmdEnd

Each cmdstart is exactly matched with a cmd end, clean up any group trace flags, etc. here
=================
*/
void CmdEnd ( const edict_t *player )
{
	entvars_t *pev = (entvars_t *)&player->v;
	AvHPlayer *pl = ( AvHPlayer *) CBasePlayer::Instance( pev );
	
	if( !pl )
		return;

	pl->SetSizeForUser3();
	
	if ( pl->pev->groupinfo != 0 )
	{
		UTIL_UnsetGroupTrace();
	}
}

/*
================================
ConnectionlessPacket

 Return 1 if the packet is valid.  Set response_buffer_size if you want to send a response packet.  Incoming, it holds the max
  size of the response_buffer, so you must zero it out if you choose not to respond.
================================
*/
int	ConnectionlessPacket( const struct netadr_s *net_from, const char *args, char *response_buffer, int *response_buffer_size )
{
	// Parse stuff from args
	int max_buffer_size = *response_buffer_size;

	// Zero it out since we aren't going to respond.
	// If we wanted to response, we'd write data into response_buffer
	*response_buffer_size = 0;

	// Since we don't listen for anything here, just respond that it's a bogus message
	// If we didn't reject the message, we'd return 1 for success instead.
	return 0;
}

/*
================================
GetHullBounds

  Engine calls this to enumerate player collision hulls, for prediction.  Return 0 if the hullnumber doesn't exist.
================================
*/
int GetHullBounds( int hullnumber, float *mins, float *maxs )
{
	int iret = 0;

	switch ( hullnumber )
	{
	case 0:
		HULL0_MIN.CopyToArray(mins);
		HULL0_MAX.CopyToArray(maxs);
		iret = 1;
		break;

	case 1:
		HULL1_MIN.CopyToArray(mins);
		HULL1_MAX.CopyToArray(maxs);
		iret = 1;
		break;

	case 2:
		HULL2_MIN.CopyToArray(mins);
		HULL2_MAX.CopyToArray(maxs);
		iret = 1;
		break;

	case 3:
		HULL3_MIN.CopyToArray(mins);
		HULL3_MAX.CopyToArray(maxs);
		iret = 1;
		break;
	}

	return iret;
}

/*
================================
CreateInstancedBaselines

Create pseudo-baselines for items that aren't placed in the map at spawn time, but which are likely
to be created during play ( e.g., grenades, ammo packs, projectiles, corpses, etc. )
================================
*/
void CreateInstancedBaselines ( void )
{
	int iret = 0;
	entity_state_t state;

	memset( &state, 0, sizeof( state ) );

	// Create any additional baselines here for things like grendates, etc.
	// iret = ENGINE_INSTANCE_BASELINE( pc->pev->classname, &state );

	// Destroy objects.
	//UTIL_Remove( pc );
}

/*
================================
InconsistentFile

One of the ENGINE_FORCE_UNMODIFIED files failed the consistency check for the specified player
 Return 0 to allow the client to continue, 1 to force immediate disconnection ( with an optional disconnect message of up to 256 characters )
================================
*/
int	InconsistentFile( const edict_t *player, const char *filename, char *disconnect_message )
{
	// Server doesn't care?
	if ( CVAR_GET_FLOAT( "mp_consistency" ) != 1 )
		return 0;

	// Default behavior is to kick the player
	sprintf( disconnect_message, "Server is enforcing file consistency for %s\n", filename );

	// Kick now with specified disconnect message.
	return 1;
}

/*
================================
AllowLagCompensation

 The game .dll should return 1 if lag compensation should be allowed ( could also just set
  the sv_unlag cvar.
 Most games right now should return 0, until client-side weapon prediction code is written
  and tested for them ( note you can predict weapons, but not do lag compensation, too, 
  if you want.
================================
*/
int AllowLagCompensation( void )
{
	return 1;
}

inline bool AvHDetermineVisibility(struct entity_state_s *state, int e, edict_t *ent, edict_t *host, unsigned char *pSet, bool& outIsParticleSystemEntity, bool& outIsParticleSystemEntityVisible)
{
	bool theIsVisible = false;
	bool theIsParticleSystemEntity = (ent->v.classname == MAKE_STRING(kesParticlesCustom));
	outIsParticleSystemEntity = theIsParticleSystemEntity;
	outIsParticleSystemEntityVisible = false;
	
	// Always propagate ourself
	if(ent != host)
	{
		//if(GET_RUN_CODE(256))

		bool theEntityIsAWeldable = ent->v.iuser3 == AVH_USER3_WELD;//(ent->v.classname == MAKE_STRING(kesWeldable));
		bool theIsNoBuild = ent->v.iuser3 == AVH_USER3_NOBUILD;//(ent->v.classname == MAKE_STRING(kesNoBuild));
		
		// Don't send if EF_NODRAW objects that aren't the host (with some exceptions)
		// This is a little convoluted because of performance (this function gets called many thousands of time per server tick)

		// Elven - edited out the !(ent->v.effects & EF_NODRAW) because it will always evaluate to true.
		// Reasoning:  if (v.effects & EF_NODRAW) and (ent!=host) were ever true, there would have been a return call in 
		// AddToFullPack before this function was called. 
		// Therefore, (ent->v.effects & EF_NODRAW) will always be false and !false will always be true.
		// puzl - undid this change as it was causing problems in comm mode. Structures, players etc. were not visible to the comm.  
		if(!(ent->v.effects & EF_NODRAW) ||  theEntityIsAWeldable || theIsNoBuild || (ent->v.classname == MAKE_STRING(kesMP3Audio)))
		{
			// This is duplicated from the above line for possible efficiency
			bool theIsMp3Entity = (ent->v.classname == MAKE_STRING(kesMP3Audio));

			// Ignore ents without valid / visible models
			// ...but don't cull out particle systems ever.
			// This should use an approximate bounding radius and cull it with the PVS normally but
			// I can't figure out how to make it work.  This would only cause more net traffic if
			// the particle system entity was out of sight and moving around due to map triggers, not
			// through normal operation
			if(ent->v.modelindex || STRING(ent->v.model) || theIsParticleSystemEntity || theIsMp3Entity)
			{
				if(theIsMp3Entity)
				{
					CBaseEntity* theEntity = CBaseEntity::Instance(ent);
					AvHMP3Audio* theMP3Audio = dynamic_cast<AvHMP3Audio*>(theEntity);
					if(theMP3Audio)
					{
						float theDistanceToEntity = VectorDistance(host->v.origin, ent->v.origin);
						int theFadeDistance = theMP3Audio->GetFadeDistance();
						if((theFadeDistance <= 0) || (theDistanceToEntity <= theFadeDistance))
						{
							theIsVisible = true;
						}
					}
				}
				
				// If we're in top down
				bool thePlayerInTopDown = (host->v.iuser4 & MASK_TOPDOWN);
				if(thePlayerInTopDown)
				{
					// Possible visible entities are world entities, sighted enemy entities, or entities on our team
					bool theEntityIsWorldEntity = (ent->v.team == 0) && (ent->v.iuser3 != AVH_USER3_HIVE);
					bool theIsSighted = (ent->v.iuser4 & MASK_VIS_SIGHTED);
					bool theOnSameTeam = (host->v.team == ent->v.team);
					
					if(theEntityIsWorldEntity || theIsSighted || theOnSameTeam)
					{
						CBaseEntity* theEntity = CBaseEntity::Instance(ent);
						bool theIsDoor = (dynamic_cast<CBaseDoor*>(theEntity) != NULL);
						
						// If entity is in front of the player and within visibility range
						vec3_t theEntityOrigin = ent->v.origin;
						float theMagnitude = theEntityOrigin.x + theEntityOrigin.y + theEntityOrigin.z;
						if((theMagnitude < 5.0f) || theIsDoor)
						{
							theEntityOrigin.x = (ent->v.absmin.x + ent->v.absmax.x)/2.0f;
							theEntityOrigin.y = (ent->v.absmin.y + ent->v.absmax.y)/2.0f;
							theEntityOrigin.z = (ent->v.absmin.z + ent->v.absmax.z)/2.0f;
						}
						bool theIsRelevantForTopDownPlayer = AvHSUGetIsRelevantForTopDownPlayer(host->v.origin, theEntityOrigin);
						if(!theIsRelevantForTopDownPlayer)
						{
							AvHPlayer* theReceivingPlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(host));

							// If the entity is selected, it's relevant
							if(theEntity && theReceivingPlayer && theReceivingPlayer->GetIsSelected(theEntity->entindex()))
							{
								theIsRelevantForTopDownPlayer = true;
							}
						}
						if(theIsRelevantForTopDownPlayer)
						{
							theIsVisible = true;
							if(theIsParticleSystemEntity)
							{
								outIsParticleSystemEntityVisible = true;
							}
						}
					}
				}
//				else
//				{
//					// Check visibility with the engine
//					if(ENGINE_CHECK_VISIBILITY((const struct edict_s *)ent, pSet))
//					{
//						theIsVisible = true;
//				
//						// If it's a particle system entity, save visibility for it
//						if(theIsParticleSystemEntity)
//						{
//							outIsParticleSystemEntityVisible = true;
//						}
//					}
//					else if(theIsParticleSystemEntity)
//					{
//						outIsParticleSystemEntityVisible = false;
//					}
//				}
			}
		}
	}
	else
	{
		theIsVisible = true;
	}

	return theIsVisible;
}

/*
AddToFullPack
 
Return 1 if the entity state has been filled in for the ent and the entity will be propagated to the client, 0 otherwise
 
state is the server maintained copy of the state info that is transmitted to the client
a MOD could alter values copied into state to send the "host" a different look for a particular entity update, etc.
e and ent are the entity that is being added to the update, if 1 is returned
host is the player's edict of the player whom we are sending the update to
player is 1 if the ent/e is a player and 0 otherwise
pSet is either the PAS or PVS that we previous set up.  We can use it to ask the engine to filter the entity against the PAS or PVS.
we could also use the pas/ pvs that we set in SetupVisibility, if we wanted to.  Caching the value is valid in that case, but still only for the current frame
*/
int AddToFullPack( struct entity_state_s *state, int e, edict_t *ent, edict_t *host, int hostflags, int player, unsigned char *pSet )
{ 
	int     i;

    if(e > kMaxEProcessedForPlayerOne)
    {
        kNumEntsProcessedForPlayerOne++;
        kMaxEProcessedForPlayerOne = e;
    }

	if((ent != host) && !(GET_RUN_CODE(512)))
	{
		kNumReturn0++;
		return 0;
	}

	// This is a common case
	// Ignore ents without valid / visible models
	if ( !ent->v.modelindex || !STRING( ent->v.model ) )
	{
		kNumReturn0++;
		return 0;
	}

	// don't send if flagged for NODRAW and it's not the host getting the message
	if ( ( ent->v.effects & EF_NODRAW ) &&
	  ( ent != host ) )
	{
		kNumReturn0++;
		return 0;
	}
	
	// Don't send spectators to other players
	if ( ( ent->v.flags & FL_SPECTATOR ) && ( ent != host ) )
	{
		kNumReturn0++;
		return 0;
	}

	AvHUser3 theUser3 = (AvHUser3)ent->v.iuser3;
	bool theCanBeTargetted = ent->v.targetname != 0;
	
	//if(!strcmp(theEntNameString, "func_illusionary"))
	if(theUser3 == AVH_USER3_FUNC_ILLUSIONARY)
	{
		// Check invisibility flags
		if(host->v.iuser4 & MASK_TOPDOWN)
		{
			if(FBitSet(ent->v.spawnflags, 1))
			{
				if(pSet != NULL)
				{
					kNumReturn0++;
					return 0;
				}
			}
		}
		// If we're not commander, and "invisible for player" is set
		else
		{
			if(FBitSet(ent->v.spawnflags, 2))
			{
				if(pSet != NULL)
				{
					kNumReturn0++;
					return 0;
				}
			}
		}
	}


	// Some entities can be determined to be visible without the engine check
	bool theIsParticleSystemEntity = false;
	bool theIsParticleSystemEntityVisible = false;
	bool theIsVisible = AvHDetermineVisibility(state, e, ent, host, pSet, theIsParticleSystemEntity, theIsParticleSystemEntityVisible);
	if(!theIsVisible)
	{
		// Check to see if the player has left their previous leaf.  If so, reset player's PVS info.
		int hostnum = ENTINDEX( host ) - 1;
		if ( TimeToResetPVS( host, hostnum ) )
		{
			ResetPlayerPVS( host, hostnum );
		}
		
		// Ignore if not the host and not touching a PVS/PAS leaf
		// If pSet is NULL, then the test will always succeed and the entity will be added to the update
		if ( ent != host )
		{
			if(GetTimeToRecompute( hostnum, e, gpGlobals->time, theIsVisible))
			{
				// Do time consuming check 
				theIsVisible = ENGINE_CHECK_VISIBILITY( (const struct edict_s *)ent, pSet );

				MarkEntityInPVS( hostnum, e, gpGlobals->time, theIsVisible);

				kNumComputed++;
			}

			if(!theIsVisible)
			{
				kNumReturn0++;
				return 0;
			}
		}
		
		// Don't send entity to local client if the client says it's predicting the entity itself.
		if ( ent->v.flags & FL_SKIPLOCALHOST )
		{
			if ( ( hostflags & 1 ) && ( ent->v.owner == host ) )
			{
				kNumReturn0++;
				return 0;
			}
		}
		
		if ( host->v.groupinfo )
		{
			UTIL_SetGroupTrace( host->v.groupinfo, GROUP_OP_AND );
		
			// Should always be set, of course
			if ( ent->v.groupinfo )
			{
				if ( g_groupop == GROUP_OP_AND )
				{
					if ( !(ent->v.groupinfo & host->v.groupinfo ) )
					{
						kNumReturn0++;
						return 0;
					}
				}
				else if ( g_groupop == GROUP_OP_NAND )
				{
					if ( ent->v.groupinfo & host->v.groupinfo )
					{
						kNumReturn0++;
						return 0;
					}
				}
			}
	
			UTIL_UnsetGroupTrace();
		}
	}

	memset( state, 0, sizeof( *state ) );
	
	// Assign index so we can track this entity from frame to frame and
	//  delta from it.
	state->number   = e;
	state->entityType = ENTITY_NORMAL;
	
	// Flag custom entities.
	if ( ent->v.flags & FL_CUSTOMENTITY )
	{
		state->entityType = ENTITY_BEAM;
	}
	
	// 
	// Copy state data
	//
	
	// Round animtime to nearest millisecond
	state->animtime   = (int)(1000.0 * ent->v.animtime ) / 1000.0;
	
	//memcpy( state->origin, ent->v.origin, 3 * sizeof( float ) );
	state->origin = ent->v.origin;
	
	//memcpy( state->angles, ent->v.angles, 3 * sizeof( float ) );
	state->angles = ent->v.angles;
	
	//memcpy( state->mins, ent->v.mins, 3 * sizeof( float ) );
	state->mins = ent->v.mins;
	
	//memcpy( state->maxs, ent->v.maxs, 3 * sizeof( float ) );
	state->maxs = ent->v.maxs;
	
	//memcpy( state->startpos, ent->v.startpos, 3 * sizeof( float ) );
	state->startpos = ent->v.startpos;
	
	//memcpy( state->endpos, ent->v.endpos, 3 * sizeof( float ) );
	state->endpos = ent->v.endpos;
	
	state->impacttime = ent->v.impacttime;
	state->starttime = ent->v.starttime;
	
	state->modelindex = ent->v.modelindex;
	 
	state->frame      = ent->v.frame;
	
	state->skin       = ent->v.skin;
	state->effects    = ent->v.effects;
	
	// This non-player entity is being moved by the game .dll and not the physics simulation system
	//  make sure that we interpolate it's position on the client if it moves
	if ( !player &&
		ent->v.animtime &&
		ent->v.velocity[ 0 ] == 0 && 
		ent->v.velocity[ 1 ] == 0 && 
		ent->v.velocity[ 2 ] == 0 )
	{
		state->eflags |= EFLAG_SLERP;
	}
	
	state->scale   = ent->v.scale;
	state->solid   = ent->v.solid;
	state->colormap   = ent->v.colormap;
	state->movetype   = ent->v.movetype;
	state->sequence   = ent->v.sequence;
	state->framerate  = ent->v.framerate;
	state->body       = ent->v.body;
	
	for (i = 0; i < 4; i++)
	{
		state->controller[i] = ent->v.controller[i];
	}
	
	for (i = 0; i < 2; i++)
	{
		state->blending[i]   = ent->v.blending[i];
	}
	
	state->rendermode    = ent->v.rendermode;
	state->renderamt     = ent->v.renderamt; 
	state->renderfx      = ent->v.renderfx;
	state->rendercolor.r = ent->v.rendercolor[0];
	state->rendercolor.g = ent->v.rendercolor[1];
	state->rendercolor.b = ent->v.rendercolor[2];


    // If classname indicates an entity that fades depending on viewing player role
	const char* theEntNameString = STRING(ent->v.classname);

    if(!player && AvHSSUGetIsClassNameFadeable(theEntNameString))
	{
		CBaseEntity* theSeethrough = CBaseEntity::Instance(ent);
		ASSERT(theSeethrough);
	
		// Default to player is full visibility
		state->rendermode = kRenderNormal;
	
		int theAlphaValue = theSeethrough->pev->fuser2;
		if(host->v.iuser4 & MASK_TOPDOWN)
		{
			 theAlphaValue = theSeethrough->pev->fuser1;
		}

		if(theAlphaValue != 255)
		{
			//state->rendermode = kRenderTransAdd;
			state->rendermode = kRenderTransTexture;
			state->renderamt = theAlphaValue;
			if(state->renderamt == 0)
			{
				state->effects |= EF_NODRAW;
			}
            
            if(host->v.iuser4 & MASK_TOPDOWN)
            {
                state->solid = SOLID_NOT;
            }

		}

	}
	
	// Inactive hives should be drawn for players on their team
	if((ent->v.iuser3 == AVH_USER3_HIVE) && (!GetHasUpgrade(ent->v.iuser4, MASK_BUILDABLE)))
	{
		// Assumes that aliens of both teams can build on the same hive location
		AvHPlayer* theReceivingPlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(host));
		if(theReceivingPlayer && theReceivingPlayer->GetIsAlien())
		{
			state->renderamt = 128;
		}
	}

	// Handle cloakables here
	if(!AvHSUGetIsExternalClassName(theEntNameString))
	{
		AvHCloakable* theCloakable = dynamic_cast<AvHCloakable*>(CBaseEntity::Instance(ent));
		if(theCloakable)
		{
			// Now updated in gamerules
			theCloakable->Update();

			float theOpacityScalar = theCloakable->GetOpacity();
			if(theOpacityScalar < 1.0f)
			{
				int theBaseOpacity = kAlienStructureCloakAmount;
				int theOpacityRange = 255 - kAlienStructureCloakAmount;

				// Allow spectators to see cloaked entities as if they are the player they are following, or as if they are on the same team as the alien otherwise
				bool theCanSeeCloaked = (host->v.team == ent->v.team);
				if(!theCanSeeCloaked)
				{
					int theHostIUserOne = host->v.iuser1;
					if((theHostIUserOne == OBS_CHASE_LOCKED) || (theHostIUserOne == OBS_CHASE_FREE) || (theHostIUserOne == OBS_IN_EYE) )
					{
						// View entities as player we're tracking
						int theHostIUserTwo = host->v.iuser2;
						CBaseEntity* theSpectatingEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theHostIUserTwo));
						if(theSpectatingEntity)
						{
							int theSpectatingEntityTeamNumber = theSpectatingEntity->pev->team;
							if((theSpectatingEntityTeamNumber == ent->v.team) || (host->v.team == TEAM_IND))
							{
								theCanSeeCloaked = true;
							}
						}
					}
				}

				if(theCanSeeCloaked)
				{
					theBaseOpacity = kAlienSelfCloakingBaseOpacity;
					theOpacityRange = 255 - theBaseOpacity;
				}

				int theOpacity = theBaseOpacity + theOpacityScalar*theOpacityRange;
				theOpacity = max(min(255, theOpacity), 0);

				state->rendermode = kRenderTransTexture;
				state->renderamt = theOpacity;
			}
		}
	}
	
	// Spectator
	state->iuser1 = ent->v.iuser1;
	state->iuser2 = ent->v.iuser2;
	
	// AvH specials
	state->iuser3 = ent->v.iuser3;
	state->iuser4 = ent->v.iuser4;

	// Slightly different processing for particle system entities
//	if(theIsParticleSystemEntity)
//	{
//		// propagate weapon model as custom data)
//		state->weaponmodel = ent->v.weaponmodel;
//	}
	
	state->fuser1 = ent->v.fuser1;
	state->fuser2 = ent->v.fuser2;
	state->fuser3 = ent->v.fuser3;
	state->vuser4 = ent->v.vuser4;





	
	state->aiment = 0;
	if ( ent->v.aiment )
	{
	 state->aiment = ENTINDEX( ent->v.aiment );
	}
	
	state->owner = 0;
	if ( ent->v.owner )
	{
	 int owner = ENTINDEX( ent->v.owner );
	 
	 // Only care if owned by a player
	 if ( owner >= 1 && owner <= gpGlobals->maxClients )
	 {
	  state->owner = owner; 
	 }
	}
	
	// HACK:  Somewhat...
	// Class is overridden for non-players to signify a breakable glass object ( sort of a class? )
	if ( !player )
	{
		state->playerclass  = ent->v.playerclass;
	}

	// Propagate team for all entities (mainly needed for client-side lasso selection)
	state->team			= ent->v.team;
	
	// Check special vision mode
	AvHPlayer* theReceivingPlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(host));
	if(theReceivingPlayer && (theReceivingPlayer->GetIsAlienSightActive()))
	{
		bool marineGlow = false;
		if (player) 
		{
			// Uncomment below to enable range for the alien flashlight
			// vec3_t theDistanceVec;
			// VectorSubtract(theReceivingPlayer->pev->origin, ent->v.origin, theDistanceVec);

			// int theDistance = theDistanceVec[0] * theDistanceVec[0] + theDistanceVec[1] * theDistanceVec[1] + theDistanceVec[2] * theDistanceVec[2];
			// int theRange = BALANCE_IVAR(kAlienFlashlightRange);
			// marineGlow = (theDistance < ( theRange * theRange));
			marineGlow = true;
		}

		if(( marineGlow || (ent->v.team == theReceivingPlayer->pev->team)) && (ent != theReceivingPlayer->edict()) && (ent->v.team != 0))
		{
			state->rendermode = kRenderTransAdd;
			state->renderamt = 150;
		}
	}
	
	// Special stuff for players only
	if(player)
	{
		state->basevelocity = ent->v.basevelocity;
		
		state->weaponmodel  = MODEL_INDEX( STRING( ent->v.weaponmodel ) );
		state->gaitsequence = ent->v.gaitsequence;
		state->spectator = ent->v.flags & FL_SPECTATOR;
		state->friction     = ent->v.friction;
		
		state->gravity      = ent->v.gravity;
		
		// New SDK doesn't propagate team for a reason?
		//state->playerclass  = ent->v.playerclass;
		//state->team			= ent->v.team;
		
		bool theIsDucking = ent->v.flags & FL_DUCKING;
		state->usehull = AvHMUGetHull(theIsDucking, ent->v.iuser3);
		
		// Propagate "energy level"
		state->fuser3 = ent->v.fuser3;
		
		// For skulk oriented player model
		state->vuser1 = ent->v.vuser1;
		
		// For weapons
		state->vuser4 = ent->v.vuser4;
		
		//state->skin = theTargetPlayer->GetSkin();
		state->skin = ent->v.skin;
		state->playerclass = ent->v.playerclass;
		//		state->armorvalue = ent->v.armorvalue;

		AvHPlayer* thePlayerEntity = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(ent));
		if(thePlayerEntity && thePlayerEntity->GetIsTemporarilyInvulnerable())
		{
			int theTeamIndex = ent->v.team;
			ASSERT(theTeamIndex >= 0);
			ASSERT(theTeamIndex < iNumberOfTeamColors);

			state->renderfx = kRenderFxGlowShell;
			state->rendercolor.r = kTeamColors[theTeamIndex][0];
			state->rendercolor.g = kTeamColors[theTeamIndex][1];
			state->rendercolor.b = kTeamColors[theTeamIndex][2];
			state->renderamt = kInvulShellRenderAmount;
		}
	}
	
	state->health		= ent->v.health;

	kNumReturn1++;

	return 1;
}
 
