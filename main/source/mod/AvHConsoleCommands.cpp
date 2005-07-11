//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHConsoleCommands.cpp$
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHConsoleCommands.cpp,v $
// Revision 1.31  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.30  2002/11/15 04:46:53  Flayra
// - Utility command
//
// Revision 1.29  2002/11/03 04:49:56  Flayra
// - Team balance fixes
//
// Revision 1.28  2002/10/28 20:33:55  Flayra
// - Added confirmation to auth command
//
// Revision 1.27  2002/10/24 21:22:40  Flayra
// - Commented out adjust score cheat for release (even with cheats on, this is abusive)
//
// Revision 1.26  2002/10/20 16:45:19  Flayra
// - Linux update
//
// Revision 1.25  2002/10/20 16:35:46  Flayra
// - Added hack code to create a fake client to test profiling
//
// Revision 1.24  2002/10/18 22:17:12  Flayra
// - Added redeem cheat
//
// Revision 1.23  2002/10/16 00:51:33  Flayra
// - Added auth command and setauth cheat
// - Require cheats for some dev commands (oops)
//
// Revision 1.22  2002/10/03 18:41:35  Flayra
// - Allow setskin to work without chets, for testing
//
// Revision 1.21  2002/09/23 22:11:46  Flayra
// - Regular update
//
// Revision 1.20  2002/09/09 19:49:25  Flayra
// - Added deathmessage cheat
//
// Revision 1.19  2002/08/16 02:33:32  Flayra
// - Added endgame cheat to end game for either team (to test endgame music)
//
// Revision 1.18  2002/08/09 00:56:11  Flayra
// - Added "adjustscore" cheat for testing new scoreboard
//
// Revision 1.17  2002/08/02 22:01:27  Flayra
// - Added some new cheats, refactored cheat names, changes for new alert system
//
// Revision 1.16  2002/07/10 14:39:24  Flayra
// - Added "overflow" cheat for debugging
//
// Revision 1.15  2002/07/08 16:49:34  Flayra
// - Unhacked this to fix some random bug
//
// Revision 1.14  2002/06/03 16:41:19  Flayra
// - Added "invul" cheat
//
// Revision 1.13  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "util/nowarnings.h"
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/player.h"
#include "dlls/weapons.h"
#include "mod/AvHGamerules.h"
#include "mod/AvHConstants.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHEntities.h"
#include "mod/AvHParticleTemplateServer.h"
#include "mod/AvHPlayer.h"
#include "dlls/client.h"
#include "dlls/game.h"
#include "dlls/util.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHServerVariables.h"
#include "mod/AvHMessage.h"
#include "mod/AvHSoundListManager.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHTitles.h"
#include "mod/AvHParticleSystemManager.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHMovementUtil.h"
#include "mod/AvHSharedUtil.h"
#include "textrep/TRFactory.h"
#include "mod/AvHEntities.h"
#include "mod/AvHScriptManager.h"
#include "mod/AvHCommandConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include <stdio.h>
#include "game_shared/voice_gamemgr.h"
#include "mod/AvHNetworkMessages.h"
#include "mod/AvHNexusServer.h"

extern AvHParticleTemplateListServer	gParticleTemplateList;
extern CVoiceGameMgr					g_VoiceGameMgr;
extern int								gCommanderPointsAwardedEventID;
extern cvar_t							allow_spectators;
extern cvar_t							avh_tournamentmode;
extern int                              kNumEntsProcessedForPlayerOne;

#ifdef WIN32
// this is the LINK_ENTITY_TO_CLASS function that creates a player (bot)
HINSTANCE h_Library = NULL;
typedef void (*LINK_ENTITY_FUNC)(entvars_t *);

void player( entvars_t *pev )
{
	if(!h_Library)
	{
		string libName = string(getModDirectory()) + "\\dlls\\ns.dll";
		h_Library = LoadLibrary(libName.c_str());
	}
	   
	static LINK_ENTITY_FUNC otherClassName = NULL;
	if (otherClassName == NULL)
		otherClassName = (LINK_ENTITY_FUNC)GetProcAddress(h_Library, "player");
	if (otherClassName != NULL)
	{
		(*otherClassName)(pev);
	}
}
#endif

//------------------------------------------------------------------
// begin future MapUtils.h / MapUtils.cpp
//------------------------------------------------------------------
typedef std::pair<string,int> EntityInfoNodeType;
typedef std::map<string,int> EntityInfoMapType;
typedef std::vector<EntityInfoNodeType> EntityInfoVectorType;

bool EntityInfoSortFunction(const EntityInfoNodeType& left,const EntityInfoNodeType& right)
{
	if(left.second > right.second)
	{ return true; }
	if(left.first.compare(right.first) < 0)
	{ return true; }
	return false;
}

bool EntityInfoShouldCount(const string& inClassName)
{
	bool theShouldCount = true;
	if(inClassName.empty()) // unassigned player slot
	{
		theShouldCount = false;
	}
	else if(inClassName.compare(0,7,"weapon_") == 0)
	{
		theShouldCount = false;
	}
	else if(inClassName.compare("bodyque") == 0)
	{
		theShouldCount = false;
	}
	else if(inClassName.compare("player") == 0)
	{
		theShouldCount = false;
	}
	return theShouldCount;
}

EntityInfoVectorType EntityInfoGetVector(void)
{
	EntityInfoMapType theMap;
	const char* theClassName;
	string theClassNameString;

	for(int index = 0; index < gpGlobals->maxEntities; ++index)
	{
		edict_t* theEdict = INDEXENT(index);
		if(!FNullEnt(theEdict))
		{
			theClassName = STRING(theEdict->v.classname);
			if(theClassName)
			{
				++theMap[string(theClassName)]; 
			}
		}
	}

	EntityInfoVectorType theVector;

	EntityInfoMapType::iterator end = theMap.end();
	for(EntityInfoMapType::iterator current = theMap.begin(); current != end; ++current)
	{
		if(EntityInfoShouldCount(current->first))
		{
			theVector.push_back(EntityInfoNodeType(current->first,current->second));
		}
	}

	//sort it
	sort(theVector.begin(),theVector.end(),EntityInfoSortFunction);

	return theVector;
}

int EntityInfoGetCount()
{
	int theCount = 0;
	EntityInfoVectorType theVector = EntityInfoGetVector();
	EntityInfoVectorType::iterator end = theVector.end();
	for(EntityInfoVectorType::iterator current = theVector.begin(); current != end; ++current)
	{
		theCount += current->second;
	}
	return theCount;
}
//------------------------------------------------------------------
// end future MapUtils.h / MapUtils.cpp
//------------------------------------------------------------------


void ReportPlayer(CBasePlayer* inPlayer, const char* inCommand)
{
	#ifdef AVH_PLAYTEST_BUILD
		// Tell the server that this player executed x message
		char* theMessage = UTIL_VarArgs("%s initiated command \"%s\"\n", STRING(inPlayer->pev->netname), inCommand);
		UTIL_ClientPrintAll(HUD_PRINTNOTIFY, theMessage);
		UTIL_LogPrintf(theMessage);
	#endif
}

BOOL AvHGamerules::ClientCommand( CBasePlayer *pPlayer, const char *pcmd )
{
//adding Nexus TunnelToClient functionality up here...
	if( strcmp( pcmd, "NexusData" ) == 0 )
	{
		const char* arg1 = CMD_ARGV(1);
		return AvHNexus::recv(pPlayer->pev,arg1,strlen(arg1));
	}
//non-Nexus signal handler down here...

	AvHPlayer*	theAvHPlayer = dynamic_cast<AvHPlayer*>(pPlayer);
	AvHTeam*	theTeam = NULL;
	bool		theSuccess = false;
	bool		theIsDeveloper = false;
	bool		theIsGuide = false;
	bool		theIsServerOp = false;
	bool		theIsPlaytester = false;
	bool		theIsPlayerHelper = false;
	bool		theIsPlaytest = false;
	bool		theReportPlayer = false;
	bool		theIsDedicatedServer = false;
	bool		theIsDebug = false;

	#ifdef DEBUG
	theIsDebug = true;
	#endif

	if(theAvHPlayer)
	{
		theTeam = theAvHPlayer->GetTeamPointer();
		theIsDeveloper = theAvHPlayer->GetIsMember(PLAYERAUTH_DEVELOPER);
		theIsGuide = theAvHPlayer->GetIsMember(PLAYERAUTH_GUIDE);
		theIsPlaytester = theAvHPlayer->GetIsMember(PLAYERAUTH_PLAYTESTER);
		theIsPlayerHelper = theIsDeveloper || theIsGuide || theIsPlaytester;

		#ifdef AVH_PLAYTEST_BUILD
		theIsPlaytest = theIsPlaytester || theIsDeveloper;
		#endif
	}
	else
	{
		theIsDedicatedServer = true;
	}

	if( !theAvHPlayer || theAvHPlayer->GetIsMember(PLAYERAUTH_SERVEROP) )
	{
		theIsServerOp = true;
	}

	if(g_VoiceGameMgr.ClientCommand(pPlayer, pcmd))
	{
		theSuccess = true;
	}
	else if ( FStrEq( pcmd, kcJoinTeamOne ) )
	{
		if(theAvHPlayer)
		{
			this->AttemptToJoinTeam(theAvHPlayer, TEAM_ONE, true);
			theSuccess = true;
		}
	}
	else if ( FStrEq( pcmd, kcJoinTeamTwo ) )
	{
		if(theAvHPlayer)
		{
			this->AttemptToJoinTeam(theAvHPlayer, TEAM_TWO, true);
			theSuccess = true;
		}
	}
	else if ( FStrEq( pcmd, kcJoinTeamThree ) )
	{
		if(theAvHPlayer)
		{
			this->AttemptToJoinTeam(theAvHPlayer, TEAM_THREE, true);
			theSuccess = true;
		}
	}
	else if ( FStrEq( pcmd, kcJoinTeamFour ) )
	{
		if(theAvHPlayer)
		{
			this->AttemptToJoinTeam(theAvHPlayer, TEAM_FOUR, true);
			theSuccess = true;
		}
	}
	else if ( FStrEq( pcmd, kcAutoAssign ) )
	{
		if(theAvHPlayer)
		{
			this->AutoAssignPlayer(theAvHPlayer);
			theSuccess = true;
		}
	}
	else if ( FStrEq( pcmd, kcReadyRoom ) )
	{
		if(theAvHPlayer)
		{
			if(!(theAvHPlayer->pev->flags & FL_FAKECLIENT))
			{
				if(!theAvHPlayer->GetIsBeingDigested())
				{
					// puzl: 984
					// Add a throttle on the readyroom key
					const static int kReadyRoomThrottleTimeout=2.0f;
					if ( (theAvHPlayer->GetTimeLastF4() == -1.0f) || 
						 (gpGlobals->time > theAvHPlayer->GetTimeLastF4() + kReadyRoomThrottleTimeout) )  
					{
						theAvHPlayer->SendMessage(kReadyRoomThrottleMessage);
						theAvHPlayer->SetTimeLastF4(gpGlobals->time);
					}
					else if ( gpGlobals->time < theAvHPlayer->GetTimeLastF4() + kReadyRoomThrottleTimeout )
					{
						theAvHPlayer->SetPlayMode(PLAYMODE_READYROOM, true);
					}
				}
				theSuccess = true;
			}
		}
		theSuccess = true;
	}
	else if (FStrEq(pcmd, kcStartGame))
	{
		if(this->GetCheatsEnabled())
		{
			this->SetGameStarted(true);
		}
		theSuccess = true;
	}
	else if(FStrEq(pcmd, kcSwitch))
	{
		if(this->GetCheatsEnabled() || theIsPlaytest)
		{
			AvHTeamNumber thePlayerTeam = theAvHPlayer->GetTeam();
			if(thePlayerTeam != TEAM_IND)
			{
				ReportPlayer(theAvHPlayer, kcSwitch);
				AvHTeamNumber teamA = GetGameRules()->GetTeamA()->GetTeamNumber();
				AvHTeamNumber teamB = GetGameRules()->GetTeamB()->GetTeamNumber();

				// Get other team
				AvHTeamNumber theOtherTeamNumber = (thePlayerTeam == teamA) ? teamB : teamA;
				
				// Remember current position
				Vector theCurrentPosition = theAvHPlayer->pev->origin;
				Vector theCurrentAngles = theAvHPlayer->pev->angles;

				// Switch teams
				theAvHPlayer->SetPlayMode(PLAYMODE_READYROOM, true);
				this->AttemptToJoinTeam(theAvHPlayer, theOtherTeamNumber, false);

				// tankefugl: 0001010 - Boost the player 32 units up to avoid sticking in the ground
				theCurrentPosition[2] += 32.0f;

				// Set our position again
				theAvHPlayer->pev->origin = theCurrentPosition;
				theAvHPlayer->pev->angles = theCurrentAngles;
				theAvHPlayer->pev->fixangle = 1;
			}

			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, "givexp"))
	{
		if(this->GetCheatsEnabled())
		{
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theAvHPlayer->GetSpectatingEntity());
			if(!thePlayer)
			{
				thePlayer = theAvHPlayer;
			}

			if(thePlayer && thePlayer->GetIsRelevant())
			{
				int theTargetLevel = 1;

				int theNumArgs = CMD_ARGC();
				if(theNumArgs == 2)
				{
					sscanf(CMD_ARGV(1), "%d", &theTargetLevel);
				}

				GetGameRules()->AwardExperience(thePlayer, theTargetLevel);

				theSuccess = true;
			}
		}
	}
	else if(FStrEq(pcmd, kcRestartRound) || FStrEq(pcmd, kcRestart))
	{
		if(!theAvHPlayer || theIsServerOp || theIsPlaytest || theIsDedicatedServer || theIsDebug || this->GetCheatsEnabled())
		{
            if(theAvHPlayer)
            {
                ReportPlayer(theAvHPlayer, pcmd);
            }

			// Reset game world
			this->ResetGame(true);
		}
		theSuccess = true;
	}
	#ifdef AVH_PLAYTEST_BUILD
	else if(FStrEq(pcmd, kcTournyMode))
	{
		if(theIsPlaytest)
		{
			char theCommand[128];
			sprintf(theCommand, "%s %d\n", kvTournamentMode, !((int)avh_tournamentmode.value));
			SERVER_COMMAND(theCommand);
			
			theSuccess = true;
		}
	}
	#endif
// puzl: 0001073
	#ifdef USE_OLDAUTH
	else if(FStrEq(pcmd, "forceuplink"))
	{
		if(theIsDeveloper || theIsDedicatedServer)
		{
			this->InitializeAuthentication();
			UTIL_SayText("Initialized authentication.", theAvHPlayer);
		}
		theSuccess = true;
	}
	# endif
	else if(FStrEq(pcmd, kcSetBalanceVar))
	{
		if( theAvHPlayer && theAvHPlayer->GetIsAuthorized(AUTH_ACTION_ADJUST_BALANCE,0) )
		{
			BalanceValueContainer* container = BalanceValueContainerFactory::get(BalanceValueContainerFactory::getDefaultFilename());
			int theNumArgs = CMD_ARGC();
			if( theNumArgs == 3 )
			{
				string name(CMD_ARGV(1));
				string value(CMD_ARGV(2));
				if( value.at(0) == '"' )
				{
					container->insert(name,value.substr(1,value.length()-1));
				}
				else if( value.at(value.length()-1) == 'f' || value.find('.') != string::npos )
				{
					float fvalue = (float)atof(value.c_str());
					container->insert(name,fvalue);
				}
				else
				{
					int ivalue = atoi(value.c_str());
					container->insert(name,ivalue);
				}
			}
		}
		
		theSuccess = true;
	}
	else if(FStrEq(pcmd, kcNSChangeLevel))
	{
		if(theIsServerOp || theIsPlaytest)
		{
			char theLevelName[1024];
			if(sscanf(CMD_ARGV(1), "%s", theLevelName) == 1)
			{
				ReportPlayer(theAvHPlayer, pcmd);
				CHANGE_LEVEL(theLevelName, NULL );
			}
			else
			{
				UTIL_SayText("Usage: changelevel <mapname>", theAvHPlayer);
			}
		}
		else
		{
			UTIL_SayText("You're not authorized to changelevel.", theAvHPlayer);
		}

		theSuccess = true;
	}
	else if (FStrEq(pcmd, kcTestEvent))
	{
		if(theAvHPlayer && this->GetCheatsEnabled())
		{
			vec3_t theOrigin = theAvHPlayer->pev->origin;
			theOrigin.z -= 500;
			int theFlags = 0;//FEV_GLOBAL;// | FEV_HOSTONLY;
			PLAYBACK_EVENT_FULL(theFlags, theAvHPlayer->edict(), gCommanderPointsAwardedEventID, 0, theOrigin, (float *)&g_vecZero, 0.0, 0.0, /*theWeaponIndex*/ 0, 0, 0, 0 );
			theSuccess = true;
		}
	}
	else if (FStrEq(pcmd, kcGetNumPlayers))
	{
		if(theAvHPlayer && this->GetCheatsEnabled())
		{
			int theNumPlayers = GetGameRules()->GetNumberOfPlayers();
			char theMessage[128];
			sprintf(theMessage, "Num players: %d\n", theNumPlayers);
			UTIL_SayText(theMessage, theAvHPlayer);
			theSuccess = true;
		}
	}
	else if (FStrEq(pcmd, kcAddCat))
	{
		if(theAvHPlayer && (this->GetCheatsEnabled() || theIsPlaytest))
		{
			AvHTeam* theTeam = theAvHPlayer->GetTeamPointer();
			if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN))
			{
				ReportPlayer(theAvHPlayer, pcmd);

				int theCategory = 0;
				sscanf(CMD_ARGV(1), "%d", &theCategory);
				AvHAlienUpgradeCategory theUpgradeCategory = AvHAlienUpgradeCategory(theCategory);
				
				theTeam->AddTeamUpgrade(theUpgradeCategory);
				
				theSuccess = true;
			}
		}
	}
	else if (FStrEq(pcmd, kcSendMessage))
	{
		if(theAvHPlayer)
		{
			if(this->GetCheatsEnabled())
			{
				if(CMD_ARGC() >= 3)
				{
					char theTooltipText[512];
					sscanf(CMD_ARGV(1), "%s", theTooltipText);
					
					int theMode = 0;
					sscanf(CMD_ARGV(2), "%d", &theMode);

					if(theMode == 0)
					{
						UTIL_ShowMessage(theTooltipText, theAvHPlayer);
					}
					else if(theMode == 1)
					{
						UTIL_Error(theAvHPlayer, theTooltipText);
					}
					else
					{
						bool theIsToolTip = (theMode == 2);

						theAvHPlayer->SendMessage(theTooltipText, (theIsToolTip)?TOOLTIP:NORMAL);
					}
					
					theSuccess = true;
				}
			}
		}
	}
	else if (FStrEq(pcmd, kcTooltip))
	{
		if(theAvHPlayer && theIsPlayerHelper)
		{
			char thePlayerName[512];
			sscanf(CMD_ARGV(1), "%s", thePlayerName);

			// Read first word as target player
			AvHPlayer* theTargetPlayer = NULL;

			FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
				const char* thePlayerCStrName = STRING(theEntity->pev->netname);
				if(!strcmp(thePlayerCStrName, thePlayerName))
				{
					theTargetPlayer = theEntity;
				}
			END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
			
			if(theTargetPlayer)
			{
				string theToolTip;
				theToolTip = string(STRING(theAvHPlayer->pev->netname));
				theToolTip += ":";

				int theNumArgs = CMD_ARGC();
				
				for(int i=0; i < theNumArgs - 2; i++)
				{
					theToolTip += string(" ");
					char theTooltipText[512];
					sscanf(CMD_ARGV(i+2), "%s ", theTooltipText);
					theToolTip += string(theTooltipText);

					// For some reason, some punctuation comes through as separate arguments
					//if(strcmp(theTooltipText, ","))
					//{
					//	theToolTip += string(" ");
					//}
				}
				
				theTargetPlayer->SendMessage(theToolTip.c_str(), TOOLTIP);
				theSuccess = true;
			}
			else
			{
				char theErrorMessage[1024];
				sprintf(theErrorMessage, "Player \"%s\" not found.  Usage: %s CaseSensitivePlayerName Message goes here.\n", thePlayerName, kcTooltip);
				UTIL_SayText(theErrorMessage, theAvHPlayer);
			}
		}
	}
	else if (FStrEq(pcmd, kcRemoveCat))
	{
		if(theAvHPlayer)
		{
			if(this->GetCheatsEnabled() || theIsPlaytest)
			{
				AvHTeam* theTeam = theAvHPlayer->GetTeamPointer();
				if(theTeam && (theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN))
				{
					ReportPlayer(theAvHPlayer, pcmd);

					int theCategory = 0;
					sscanf(CMD_ARGV(1), "%d", &theCategory);
					AvHAlienUpgradeCategory theUpgradeCategory = AvHAlienUpgradeCategory(theCategory);
					
					theTeam->RemoveAlienUpgradeCategory(theUpgradeCategory);
					
					theSuccess = true;
				}
			}
		}
	}
	else if (FStrEq(pcmd, kcEndGame))
	{
		// Put this back in when doing larger scale playtesting
		if(this->GetCheatsEnabled() || theIsServerOp || theIsPlaytest)
		{
			ReportPlayer(theAvHPlayer, pcmd);

			string theCheat = kcEndGame1;
			
			int theTeam = 0;
			sscanf(CMD_ARGV(1), "%d", &theTeam);
			
			if(theTeam == 2)
			{
				theCheat = kcEndGame2;
			}
			
			this->SetCheatEnabled(theCheat);
			
			//this->mVictoryTeam = theTeamNumber;
			//this->mVictoryTime = gpGlobals->time;
			////this->mVictoryDraw = true;
		}
		theSuccess = true;
	}
	else if (FStrEq(pcmd, kcStartCommandMode))
	{
		if(this->GetCheatsEnabled() && theAvHPlayer)
		{
			if( theAvHPlayer->GetTeamPointer(true)->GetTeamType() != AVH_CLASS_TYPE_MARINE )
			{
				for( int counter = TEAM_ACTIVE_BEGIN; counter < TEAM_ACTIVE_END; ++counter )
				{
					AvHTeam* team = GetGameRules()->GetTeam((AvHTeamNumber)counter);
					if( team && team->GetTeamType() == AVH_CLASS_TYPE_MARINE )
					{ 
						this->AttemptToJoinTeam( theAvHPlayer, (AvHTeamNumber)counter );
					}
				}
			}
			
			// Find position of command station
			FOR_ALL_ENTITIES(kwsTeamCommand, AvHCommandStation*)
				if( theEntity->GetTeamNumber() == theAvHPlayer->pev->team )
				{
					Vector theCommandStationOrigin;
					theCommandStationOrigin.x = (theEntity->pev->absmax.x + theEntity->pev->absmin.x)/2.0f;
					theCommandStationOrigin.y = (theEntity->pev->absmax.y + theEntity->pev->absmin.y)/2.0f;
					theCommandStationOrigin.z = (theEntity->pev->absmax.z + theEntity->pev->absmin.z)/2.0f;
			
					// Circle the station, trying points around it to see if there's room
					for(int i = 0; i < 8; i++)
					{
						const int kDistance = 100;
						Vector theOffset;
						float theAngle = (i/(float)360)*2*M_PI;
						theOffset.x = cos(theAngle)*kDistance;
						theOffset.y = sin(theAngle)*kDistance;
						theOffset.z = 20;
						
						Vector thePosition = theCommandStationOrigin + theOffset;
						
						if(AvHSUGetIsEnoughRoomForHull(thePosition, AvHMUGetHull(false, theAvHPlayer->pev->iuser3), NULL))
						{
							// Teleport to this place
							theAvHPlayer->pev->origin = thePosition;
							theSuccess = true;
							break;
						}
					}
				}
			END_FOR_ALL_ENTITIES(kwsTeamCommand)
		}
	}
	else if (FStrEq(pcmd, kcHurt))
	{
		if(this->GetCheatsEnabled())
		{
			theAvHPlayer->TakeDamage(theAvHPlayer->pev, theAvHPlayer->pev, 100, DMG_GENERIC);
			theSuccess = true;
		}
	}
	else if (FStrEq(pcmd, kcSetCullDistance))
	{
		if(this->GetCheatsEnabled())
		{
			float theCullDistance = 0.0f;
			if(sscanf(CMD_ARGV(1), "%f", &theCullDistance) == 1)
			{
				this->mMapExtents.SetTopDownCullDistance(theCullDistance);
				theSuccess = true;
			}
		}
	}
	else if (FStrEq(pcmd, kcSetGamma))
	{
		if(this->GetCheatsEnabled())
		{
			sscanf(CMD_ARGV(1), "%f", &this->mMapGamma);
		}
		theSuccess = true;
	}
	else if (FStrEq(pcmd, kcStopCommandMode))
	{
		// Fixes problem where players can bind "stopcommandermode" and execute in ready room or maybe as spectator to get weapon
		if(theAvHPlayer && theAvHPlayer->GetIsInTopDownMode())
		{
			theAvHPlayer->SetUser3(AVH_USER3_MARINE_PLAYER);
			
			// Cheesy way to make sure player class change is sent to everyone
			theAvHPlayer->EffectivePlayerClassChanged();
			
			theSuccess = true;
		}
		else
		{
			int a = 0;
		}
	}
	else if(FStrEq(pcmd, kcBigDig))
	{
		if(this->GetCheatsEnabled())
		{
			bool theNewState = !GetGameRules()->GetIsCheatEnabled(kcBigDig);
			GetGameRules()->SetCheatEnabled(kcBigDig, theNewState);
			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, kcLowCost))
	{
		if(this->GetCheatsEnabled())
		{
			bool theNewState = !GetGameRules()->GetIsCheatEnabled(kcLowCost);
			GetGameRules()->SetCheatEnabled(kcLowCost, theNewState);
			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, kcHighDamage))
	{
		if(this->GetCheatsEnabled())
		{
			GetGameRules()->SetCheatEnabled(kcHighDamage);
			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, kcHighTech))
	{
		if(this->GetCheatsEnabled())
		{
			GetGameRules()->SetCheatEnabled(kcHighTech);
			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, kcSlowResearch))
	{
		if(this->GetCheatsEnabled())
		{
			GetGameRules()->SetCheatEnabled(kcSlowResearch);
			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, kcDetectAll))
	{
		if(this->GetCheatsEnabled())
		{
			GetGameRules()->SetCheatEnabled(kcDetectAll);
			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, kcNumInfos))
	{
		if(this->GetCheatsEnabled())
		{
			int theNumInfos = this->mInfoLocations.size();

			char theNumInfosString[128];
			sprintf(theNumInfosString, "Num infos: %d\n", theNumInfos);
			UTIL_SayText(theNumInfosString, theAvHPlayer);
			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, kcNumSpawns))
	{
		if(this->GetCheatsEnabled())
		{
			int theNumSpawns = this->mSpawnList.size();
			
			char theNumSpawnsString[128];
			sprintf(theNumSpawnsString, "Num spawns: %d\n", theNumSpawns);
			UTIL_SayText(theNumSpawnsString, theAvHPlayer);
			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, kcEntityInfo) || FStrEq(pcmd, kcMapUtilEntityInfo))
	{
		if(this->GetCheatsEnabled())
		{
			EntityInfoVectorType theEntityInfo = EntityInfoGetVector();

			ALERT(at_console, "------------------------------------------------------\n");
			ALERT(at_logged, "------------------------------------------------------\nEntityInfo Report:\n------------------------------------------------------\n");
			
			char theEntityMessage[1024];
			EntityInfoVectorType::iterator end = theEntityInfo.end();
			for(EntityInfoVectorType::iterator current = theEntityInfo.begin(); current != end; ++current)
			{
				sprintf(theEntityMessage, "\t%3d \"%s\" entities\n", current->second, current->first.c_str());
				ALERT(at_console, theEntityMessage);
				ALERT(at_logged, theEntityMessage);
			}

			ALERT(at_console, "------------------------------------------------------\n");
			ALERT(at_logged, "------------------------------------------------------\n");

			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, kcInvul))
	{
		if(this->GetCheatsEnabled())
		{
			if(theAvHPlayer->pev->takedamage == DAMAGE_AIM)
			{
				theAvHPlayer->pev->takedamage = DAMAGE_NO;
				UTIL_SayText("Invul ON", theAvHPlayer);
			}
			else
			{
				theAvHPlayer->pev->takedamage = DAMAGE_AIM;
				UTIL_SayText("Invul OFF", theAvHPlayer);
			}
			theSuccess = true;
		}
	}
	else if (FStrEq(pcmd, kcEditPS))
	{
		if(this->GetCheatsEnabled() && theAvHPlayer)
		{
			const char* theName = CMD_ARGV(1);
			uint32 theParticleIndex;
			if(gParticleTemplateList.GetTemplateIndexWithName(theName, theParticleIndex))
			{
				theAvHPlayer->SendMessage(kEditingParticleSystem, TOOLTIP);
				NetMsg_EditPS( theAvHPlayer->pev, theParticleIndex );
			}
			else
			{
				theAvHPlayer->SendMessage(kNoParticleSystem, TOOLTIP);
			}
			theSuccess = true;
		}
	}
	else if (FStrEq(pcmd, kcListPS))
	{
		if(this->GetCheatsEnabled() && theAvHPlayer)
		{
			int theNumTemplates = gParticleTemplateList.GetNumberTemplates();
			
			for(int i = 0; i < theNumTemplates; i++)
			{
				string theTemplateName("<system not found>");
				const AvHParticleTemplate* theTemplate = gParticleTemplateList.GetTemplateAtIndex(i);
				if(theTemplate)
				{ theTemplateName = theTemplate->GetName(); }

				theTemplateName += "\n";
				NetMsg_ListPS( theAvHPlayer->pev, theTemplateName );
			}
			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, kcRedeem))
	{
		if(this->GetCheatsEnabled())
		{
			theAvHPlayer->Redeem();
			theSuccess = true;
		}
	}
	else if (FStrEq(pcmd, kcBuildMiniMap))
	{
		if(this->GetCheatsEnabled() && theAvHPlayer)
		{
			const char* theCStrLevelName = STRING(gpGlobals->mapname);
			if(theCStrLevelName && !FStrEq(theCStrLevelName, ""))
			{
				GetGameRules()->BuildMiniMap(theAvHPlayer);
				theSuccess = true;
			}
		}
	}
    else if(FStrEq(pcmd, "votemap"))
    {
        if(theAvHPlayer)
        {
            // If map number passed
            int theMapNumber = 0;
            if(sscanf(CMD_ARGV(1), "%d", &theMapNumber) == 1)
            {
                // Add vote to this map.  Server might change maps immediately.
                GetGameRules()->VoteMap(theAvHPlayer->entindex(), theMapNumber);
            }
            else
            {
                // Print the list of maps and votes to the player
                StringList theMapVoteList;
                GetGameRules()->GetMapVoteStrings(theMapVoteList);

                for(StringList::iterator theIter = theMapVoteList.begin(); theIter != theMapVoteList.end(); theIter++)
                {
                    ClientPrint(theAvHPlayer->pev, HUD_PRINTNOTIFY, theIter->c_str());
                }
            }
            
            theSuccess = true;
        }
    }
	else if(FStrEq(pcmd, "weapondebug"))
	{
		//                     ClientPrint(theAvHPlayer->pev, HUD_PRINTNOTIFY, theIter->c_str());
        if(theAvHPlayer)
        {
			ClientPrint(theAvHPlayer->pev, HUD_PRINTNOTIFY, "weapondebug\n");
			FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
				theEntity->PrintWeaponListToClient(theAvHPlayer);
			END_FOR_ALL_ENTITIES(kAvHPlayerClassName)
			theSuccess = true;
		}
	}
#ifdef DEBUG
    else if(FStrEq(pcmd, "catalyst"))
    {
        if(this->GetCheatsEnabled())
        {
            if(AvHCatalyst::GiveCatalyst(theAvHPlayer))
            {
                theSuccess = true;
            }
        }
    }
	else if(FStrEq(pcmd, "showmenu"))
	{
		short theSlots = 0x1f;
		char theDisplayTime = -1;
		bool theNeedMore = false;
		char* theString = NULL;
		
		char theMenuText[1024];
		if(sscanf(CMD_ARGV(1), "%s", theMenuText) == 1)
		{
			NetMsg_ShowMenu( theAvHPlayer->pev, theSlots, theDisplayTime, theNeedMore ? 1 : 0, string(theMenuText) );
		}
		
		theSuccess = true;
	}
    else if(FStrEq(pcmd, "calcxp"))
    {
        if(theAvHPlayer)
        {
            char theString[512];
			sprintf(theString, "Experience for levels:\n");
            
            for(int i=1; i <= 10; i++)
            {
                float theExperienceForLevel = AvHPlayerUpgrade::GetExperienceForLevel(i);
                char theExpString[128];
                sprintf(theExpString, "\t%d/%d\n", i, (int)theExperienceForLevel);
                strcat(theString, theExpString);
            }

            ALERT(at_logged, theString);
            theSuccess = true;
        }
    }
    else if(FStrEq(pcmd, "calcspawn"))
    {
        if(theAvHPlayer)
        {
            char theString[512];
            sprintf(theString, "Spawn times (marines):\n");

            char theResult[128];
            sprintf(theResult, "1v1 -> low: %f  high: %f\n", AvHSUCalcCombatSpawnTime(AVH_CLASS_TYPE_MARINE, 1, 0, 1), AvHSUCalcCombatSpawnTime(AVH_CLASS_TYPE_MARINE, 1, 9, 1));
            strcat(theString, theResult);

            sprintf(theResult, "16v16 -> low: %f  high: %f\n", AvHSUCalcCombatSpawnTime(AVH_CLASS_TYPE_MARINE, 16, 0, 1), AvHSUCalcCombatSpawnTime(AVH_CLASS_TYPE_MARINE, 16, 9, 1));
            strcat(theString, theResult);

            ALERT(at_logged, theString);
            theSuccess = true;
        }
    }
	else if(FStrEq(pcmd, "testscores"))
	{
		PlayerListType thePlayerList;
		
		FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
			if(UTIL_IsValidEntity(theEntity->edict()))
			{
				thePlayerList.push_back(theEntity);
			}
		END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
			
		// Find random player
		if(thePlayerList.size() > 0)
		{
			AvHPlayer* thePlayer = thePlayerList[RANDOM_LONG(0, thePlayerList.size() - 1)];
			ASSERT(thePlayer);

			// Changes scores
			int theChangeType = RANDOM_LONG(0, 2);
			int theSign = RANDOM_LONG(0, 1) ? 1 : -1;
			int theAmount = RANDOM_LONG(0, 5);

			switch(theChangeType)
            {
            case 0:
                thePlayer->SetScore(thePlayer->GetScore() + theSign*theAmount);
                break;
            case 1:
                thePlayer->pev->frags += theSign*theAmount;
                break;
            case 2:
                thePlayer->m_iDeaths += theSign*theAmount;
                break;
			}

			thePlayer->EffectivePlayerClassChanged();

			theSuccess = true;
		}
	}
#endif
	else if(FStrEq(pcmd, kcOrderSelf))
	{
		if(this->GetCheatsEnabled())
		{
			GetGameRules()->SetCheatEnabled(kcOrderSelf);
			theSuccess = true;
		}
	}
#ifdef DEBUG
	else if(FStrEq(pcmd, kcAdjustScore))
	{
		if(theAvHPlayer)
		{
			AvHTeam* theTeam = theAvHPlayer->GetTeamPointer();
			if(theTeam)
			{
				int theAmount = 0;
				sscanf(CMD_ARGV(1), "%d", &theAmount);
				
				theAvHPlayer->AddPoints(theAmount, TRUE);
				theAvHPlayer->EffectivePlayerClassChanged();
				
				theSuccess = true;
			}
		}
	}
	else if(FStrEq(pcmd, kcAttackCS))
	{
		if(this->GetCheatsEnabled())
		{
			FOR_ALL_ENTITIES(kwsTeamCommand, AvHCommandStation*)
				if(theEntity->pev->team == theAvHPlayer->pev->team)
				{
					GetGameRules()->TriggerAlert((AvHTeamNumber)theAvHPlayer->pev->team, ALERT_UNDER_ATTACK, theEntity->entindex());
					theSuccess = true;
					break;
				}
				END_FOR_ALL_ENTITIES(kwsTeamCommand);
		}
	}
	else if (FStrEq(pcmd, kcPlayHUDSound))
	{
		if(theAvHPlayer && GetGameRules()->GetCheatsEnabled())
		{
			AvHHUDSound theHUDSound = HUD_SOUND_INVALID;
			if(sscanf(CMD_ARGV(1), "%d", &theHUDSound) == 1)
			{
				theSuccess = theAvHPlayer->PlayHUDSound(theHUDSound);
			}
		}
	}
	else if(FStrEq(pcmd, kcHighSpeed))
	{
		if(this->GetCheatsEnabled())
		{
			// Toggle hispeed cheat
			bool theHiSpeedEnabled = GetGameRules()->GetIsCheatEnabled(kcHighSpeed);
			GetGameRules()->SetCheatEnabled(kcHighSpeed, !theHiSpeedEnabled);
			theSuccess = true;
		}
	}
	else if (FStrEq(pcmd, kcCrash))
	{
		if(this->GetCheatsEnabled())
		{
			char* theCrashString = NULL;
			*theCrashString = 1;
		}
	}
	else if (FStrEq(pcmd, kcAssert))
	{
		if(this->GetCheatsEnabled())
		{
			ASSERT(false);
			theSuccess = true;
		}
	}
	else if (FStrEq(pcmd, kcRun))
	{
		if(this->GetCheatsEnabled() /*|| theIsDeveloper*/)
		{
			const char* theScriptName = CMD_ARGV(1);
			if(theScriptName)
			{
				AvHScriptManager::Instance()->RunScript(theScriptName);
				theSuccess = true;
			}
		}
	}
	else if (FStrEq(pcmd, kcClientRun))
	{
		if(this->GetCheatsEnabled() /*|| theIsDeveloper*/)
		{
			const char* theScriptName = CMD_ARGV(1);
			if(theAvHPlayer && theScriptName)
			{
				theAvHPlayer->RunClientScript(theScriptName);
				theSuccess = true;
			}
		}
	}
	else if(FStrEq(pcmd, kcParasite))
	{
		if(this->GetCheatsEnabled())
		{
			SetUpgradeMask(&theAvHPlayer->pev->iuser4, MASK_PARASITED);
		}
	}
	else if(FStrEq( pcmd, kcStun) )
	{
		if(this->GetCheatsEnabled() && theAvHPlayer)
		{
			theAvHPlayer->SetIsStunned(true, 4.0f);
			theSuccess = true;
		}
	}
	else if (FStrEq(pcmd, kcWeb))
	{
		if(this->GetCheatsEnabled())
		{
			theAvHPlayer->SetEnsnareState(true);
			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, kcDigest))
	{
		if(this->GetCheatsEnabled())
		{
			int theDevourerIndex = 0;
			int theDevoureeIndex = 0;
			if((sscanf(CMD_ARGV(1), "%d", &theDevourerIndex) == 1) && (sscanf(CMD_ARGV(2), "%d", &theDevoureeIndex) == 1))
			{
				AvHPlayer* theDevourerPlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theDevourerIndex)));
				AvHPlayer* theDevoureePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theDevoureeIndex)));
				if(theDevourerPlayer && theDevoureePlayer)
				{
					if(!theDevoureePlayer->GetIsBeingDigested())
					{
						theDevourerPlayer->StartDigestion(theDevoureePlayer->entindex());
					}
					else
					{
						theDevourerPlayer->StopDigestion(false);
					}
				}
			}
			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, kcBoxes))
	{
		if(this->GetCheatsEnabled())
		{
			FOR_ALL_BASEENTITIES()
				AvHBaseBuildable* theBuildable = dynamic_cast<AvHBaseBuildable*>(theBaseEntity);
			if(theBuildable)
			{
				vec3_t theMinPosition = theBuildable->pev->origin + theBuildable->pev->mins;
				vec3_t theMaxPosition = theBuildable->pev->origin + theBuildable->pev->maxs;
				
				MESSAGE_BEGIN( MSG_BROADCAST, SVC_TEMPENTITY );
				
				WRITE_BYTE(TE_BOX);
				
				WRITE_COORD(theMinPosition.x);
				WRITE_COORD(theMinPosition.y);
				WRITE_COORD(theMinPosition.x);
				
				WRITE_COORD(theMaxPosition.x);
				WRITE_COORD(theMaxPosition.y);
				WRITE_COORD(theMaxPosition.z);
				
				WRITE_SHORT(100);
				
				WRITE_BYTE(0);
				WRITE_BYTE(255);
				WRITE_BYTE(0);
				
				MESSAGE_END();
			}
			
			END_FOR_ALL_BASEENTITIES();
			
			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, kcOverflow))
	{
		if(this->GetCheatsEnabled())
		{
			// Force an overflow
			for(int i = 0; i < 100; i++)
			{
				theAvHPlayer->ForceClientDllUpdate();
			}
			theSuccess = true;
		}
	}
	else if ( FStrEq( pcmd, kcViewAll) )
	{
		// Allow even with cheats off right now, put this back in for first beta
		if(this->GetCheatsEnabled() && theAvHPlayer)
		{
			GetGameRules()->SetCheatEnabled(kcViewAll);
			theSuccess = true;
		}
	}
	else if(FStrEq(pcmd, kcDeathMessage))
	{
		if(this->GetCheatsEnabled())
		{
			const char* theWeaponName = CMD_ARGV(1);
			
			NetMsg_DeathMsg( theAvHPlayer->entindex(), theAvHPlayer->entindex(), string(theWeaponName) );
		}
	}
	else if(FStrEq(pcmd, kcSetSkin))
	{
		if(this->GetCheatsEnabled())
		{
			int theSkin = 0;
			if(sscanf(CMD_ARGV(1), "%d", &theSkin) == 1)
			{
				theAvHPlayer->SetSkin(theSkin);
				
				theSuccess = true;
			}
		}
	}
	else if(FStrEq(pcmd, kcElectric))
	{
		if(this->GetCheatsEnabled())
		{
			bool theNewValue = !GetGameRules()->GetIsCheatEnabled(kcElectric);
			GetGameRules()->SetCheatEnabled(kcElectric, theNewValue);
		}
	}
	else if(FStrEq(pcmd, kcRoomType))
	{
		if(this->GetCheatsEnabled())
		{
			int theRoomType = 0;
			if(sscanf(CMD_ARGV(1), "%d", &theRoomType) == 1)
			{
				MESSAGE_BEGIN( MSG_ONE, SVC_ROOMTYPE, NULL, theAvHPlayer->pev);
				WRITE_SHORT( (short)theRoomType );
				MESSAGE_END();
				
				char theString[128];
				sprintf(theString, "Set room type to %d.\n", theRoomType);
				UTIL_SayText(theString, theAvHPlayer);
				
				theSuccess = true;
			}
		}
	}
	#ifdef WIN32
	else if(FStrEq(pcmd, "createfake"))
	{
		if(this->GetCheatsEnabled())
		{
			char theFakeClientName[256];
			sprintf(theFakeClientName, "Bot%d", RANDOM_LONG(0, 2000));
			edict_t* BotEnt = (*g_engfuncs.pfnCreateFakeClient)(theFakeClientName);
			
			// create the player entity by calling MOD's player function
			// (from LINK_ENTITY_TO_CLASS for player object)
			player( VARS(BotEnt) );
			
			char ptr[128];  // allocate space for message from ClientConnect
			ClientConnect( BotEnt, theFakeClientName, "127.0.0.1", ptr );
			
			// Pieter van Dijk - use instead of DispatchSpawn() - Hip Hip Hurray!
			ClientPutInServer( BotEnt );
			
			BotEnt->v.flags |= FL_FAKECLIENT;
		}
	}
	#endif
#endif
    else if( FStrEq( pcmd, kcGiveUpgrade) )
    {
        // Allow even with cheats off right now, put this back in for first beta
        if(this->GetCheatsEnabled())
        {
            ReportPlayer(theAvHPlayer, pcmd);
            
            int theUpgrade = 0;
            sscanf(CMD_ARGV(1), "%d", &theUpgrade);
            AvHMessageID theNewUpgrade = AvHMessageID(theUpgrade);
            
            AvHTeam* theVisibleTeam = theAvHPlayer->GetTeamPointer(true);
            if(theVisibleTeam)
            {
                theVisibleTeam->GetTechNodes().SetResearchDone(theNewUpgrade);
                this->ProcessTeamUpgrade(theNewUpgrade, theAvHPlayer->GetTeam(true), 0, true);
            }	
            theSuccess = true;
        }
    }
    else if( FStrEq( pcmd, kcRemoveUpgrade) )
    {
        // Allow even with cheats off right now, put this back in for first beta
        if(this->GetCheatsEnabled())
        {
            ReportPlayer(theAvHPlayer, pcmd);
            
            int theUpgrade = 0;
            sscanf(CMD_ARGV(1), "%d", &theUpgrade);
            AvHMessageID theNewUpgrade = AvHMessageID(theUpgrade);
            
            AvHTeam* theVisibleTeam = theAvHPlayer->GetTeamPointer(true);
            if(theVisibleTeam)
            {
                theVisibleTeam->GetTechNodes().SetResearchDone(theNewUpgrade, false);
                this->ProcessTeamUpgrade(theNewUpgrade, theAvHPlayer->GetTeam(true), 0, false);
            }
            
            theSuccess = true;
        }
    }
	else if ( FStrEq( pcmd, kcSpectate ) )
	{
		if(theAvHPlayer)
		{
			if( !theAvHPlayer->GetIsAuthorized(AUTH_ACTION_JOIN_TEAM, TEAM_SPECT) )
			{
				AvHNexus::handleUnauthorizedJoinTeamAttempt(theAvHPlayer->edict(),TEAM_SPECT);
			}
// puzl: 0001073
#ifdef USE_OLDAUTH			 
			else if(allow_spectators.value && GetGameRules()->PerformHardAuthorization(theAvHPlayer))
#else
			else if(allow_spectators.value)
#endif
			{
				if(theAvHPlayer->GetPlayMode() == PLAYMODE_READYROOM)
				{
					theAvHPlayer->SetPlayMode(PLAYMODE_OBSERVER);
					//this->PutPlayerIntoSpectateMode(theAvHPlayer);
				}
			}
			else
			{
				theAvHPlayer->SendMessage(kSpectatorsNotAllowed, TOOLTIP);
			}
			theSuccess = true;
		}
	}
	else if ( FStrEq( pcmd, kcGivePoints) )
	{
		// Allow even with cheats off right now, put this back in for first beta
		if(this->GetCheatsEnabled() && theAvHPlayer)
		{
			int theAmount = 20;

			theAvHPlayer->SetResources(theAvHPlayer->GetResources() + theAmount, true);

			if(theTeam)
			{
				theTeam->AddResourcesGathered(theAmount);
			}

			theSuccess = true;
		}
	}
	else if(FStrEq( pcmd, kcKillCS) )
	{
		if(this->GetCheatsEnabled())
		{
			if(theTeam)
			{
				theTeam->KillCS();
				theSuccess = true;
			}
		}
	}
	else if(FStrEq( pcmd, kcKillHive) )
	{
		if(this->GetCheatsEnabled() || theIsPlaytest)
		{
			if(theTeam)
			{
				ReportPlayer(theAvHPlayer, pcmd);
				theTeam->KillHive();
				theSuccess = true;
			}
		}
	}
	else if(FStrEq( pcmd, kcSpawnHive) )
	{
		if(this->GetCheatsEnabled() || theIsPlaytest)
		{
			AvHTeam* theHiveTeam = theAvHPlayer->GetTeamPointer(true);
			if( theHiveTeam->GetTeamType() != AVH_CLASS_TYPE_ALIEN )
			{
				for( int counter = TEAM_ACTIVE_BEGIN; counter < TEAM_ACTIVE_END; ++counter )
				{
					theHiveTeam = GetGameRules()->GetTeam((AvHTeamNumber)counter);
					if( theHiveTeam && theHiveTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN )
					{ break; }
				}
			}
			
			if(theHiveTeam)
			{
				ReportPlayer(theAvHPlayer, pcmd);
				theHiveTeam->SpawnHive();
				theSuccess = true;
			}
		}
	}
	else if(FStrEq( pcmd, kcAlert) )
	{
		if(this->GetCheatsEnabled() && theAvHPlayer)
		{
			AvHTeam* theTeam = theAvHPlayer->GetTeamPointer(true);
			if(theTeam)
			{
				if(theTeam->GetTeamType() == AVH_CLASS_TYPE_ALIEN)
				{
					// Look for random hive, set it under attack
					AvHHive* theHive = AvHSUGetRandomActiveHive(theTeam->GetTeamNumber());
					if(theHive)
					{
                        AvHAlertType theRandomAlert = (RANDOM_LONG(0, 1) == 0) ? ALERT_HIVE_DYING : ALERT_UNDER_ATTACK;
						GetGameRules()->TriggerAlert(theTeam->GetTeamNumber(), theRandomAlert, theHive->entindex());
					}
				}
				else
				{
					if(this->GetIsCombatMode())
					{
						// Find Command station, trigger alert (to test for soldiers on ground)
						int theCCEntityIndex = 0;
						FOR_ALL_ENTITIES(kwsTeamCommand, AvHCommandStation*)
							theCCEntityIndex = theEntity->entindex();
							break;
						END_FOR_ALL_ENTITIES(kwsTeamCommand);

						if(theCCEntityIndex > 0)
						{
							this->TriggerAlert(theTeam->GetTeamNumber(), ALERT_UNDER_ATTACK, theCCEntityIndex);
						}
					}
					else
					{
						int theRandomEntity = RANDOM_LONG(1, 500);
						
						AvHAlertType theRandomAlert = AvHAlertType(RANDOM_LONG(0, ALERT_MAX_ALERTS-1));
						GetGameRules()->TriggerAlert(theTeam->GetTeamNumber(), theRandomAlert, theRandomEntity);
					}
				}
				
				theSuccess = true;
			}
		}
	}
	else if(FStrEq( pcmd, kcKillAll) )
	{
		if(this->GetCheatsEnabled())
		{
			FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*)
				theEntity->TakeDamage(theAvHPlayer->pev, theAvHPlayer->pev, 2000, DMG_GENERIC | DMG_ALWAYSGIB);
			END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
			
			theSuccess = true;
		}
	}
// puzl: 0001073
#ifdef USE_OLDAUTH
    #ifndef AVH_SECURE_PRERELEASE_BUILD
	else if(FStrEq(pcmd, kcAuth))
	{
		// Toggle auth mask
		bool theNewState = !theAvHPlayer->GetAllowAuth();
		theAvHPlayer->SetAllowAuth(theNewState);

		if(theNewState)
		{
			UTIL_SayText("Authentication ON\n", theAvHPlayer);
		}
		else
		{
			UTIL_SayText("Authentication OFF\n", theAvHPlayer);
		}
		
		theSuccess = true;
	}
    #endif
#endif
	else if(FStrEq(pcmd, kcNumEnts))
	{
        bool theEnableNumEnts = GetGameRules()->GetCheatsEnabled();

        if(theEnableNumEnts)
        {
			if(theAvHPlayer)
			{
				ReportPlayer(theAvHPlayer, pcmd);
			}

			int theEntityCount = this->GetNumEntities();

		    char theNumEntsString[1024];
		    sprintf(theNumEntsString, "Total entity count: %d\n", theEntityCount);
            ALERT(at_console, theNumEntsString);
            ALERT(at_logged, theNumEntsString);
		}

        theSuccess = true;
	}
	else if(FStrEq(pcmd, kcMapUtilNumEnts))
	{
        if(GetGameRules()->GetCheatsEnabled())
        {
			int theEntityCount = EntityInfoGetCount();

		    char theNumEntsString[1024];
		    sprintf(theNumEntsString, "Total map related entity count: %d\n", theEntityCount);
            ALERT(at_console, theNumEntsString);
            ALERT(at_logged, theNumEntsString);
        }

        theSuccess = true;
    }

    return theSuccess;
}