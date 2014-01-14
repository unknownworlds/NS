//=========== (C) Copyright 1999 Valve, L.L.C. All rights reserved. ===========
//
// The copyright to the contents herein is the property of Valve, L.L.C.
// The contents may be used and/or copied only with the written permission of
// Valve, L.L.C., or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: VGUI scoreboard
//
// $Workfile:     $
// $Date: 2002/10/24 21:13:15 $
//
//-----------------------------------------------------------------------------
// $Log: vgui_ScorePanel.cpp,v $
// Revision 1.15  2002/10/24 21:13:15  Flayra
// - Fixed gamma correction for auth icons
//
// Revision 1.14  2002/10/18 22:13:48  Flayra
// - Gamma-correction for auth icons
//
// Revision 1.13  2002/10/16 00:37:33  Flayra
// - Added support for authentication in scoreboard
//
// Revision 1.12  2002/09/09 19:42:55  Flayra
// - Fixed problem where scores were sometimes displayed for marines
//
// Revision 1.11  2002/08/31 18:02:11  Flayra
// - Work at VALVe
//
// Revision 1.10  2002/08/09 00:11:33  Flayra
// - Fixed scoreboard
//
// Revision 1.9  2002/07/08 16:15:13  Flayra
// - Refactored team color management
//
// Revision 1.8  2002/06/25 17:06:48  Flayra
// - Removed memory overwrite from hlcoder list
//
// Revision 1.7  2002/06/03 16:12:21  Flayra
// - Show player status for players on your team (LEV1, GEST, COMM, etc.)
//
// Revision 1.6  2002/04/16 19:30:21  Charlie
// - Fixed crash when holding tab right when joining server, added "REIN" status
//
// Revision 1.5  2002/03/27 21:17:18  Charlie
// - Scoreboard now shows alien scores, and doesn't show marine scores.  Also draws DEAD and COMM indicators, and scoring is handled properly for aliens (points for kills, buildings)
//
// Revision 1.4  2002/03/08 02:37:52  Charlie
// - Refactored crappy-ass score panel.  It's not perfect, but it's better.  Removed TFC code, added DEAD and COMM tags to scoreboard.
//
// Revision 1.3  2001/11/13 17:51:02  Charlie
// - Increased max teams, changed team colors (allow aliens vs. aliens and fronts vs. fronts), general scoreboard support
//
// Revision 1.2  2001/09/13 22:28:01  Charlie
// - Updated NS with new Half-life 1108 patch in preparation for voice support and spectator mode
//
// Revision 1.1.1.1.2.1  2001/09/13 14:42:30  Charlie
// - HL1108
//
//
// $NoKeywords: $
//=============================================================================


#include<VGUI_LineBorder.h>

#include "hud.h"
#include "cl_util.h"
#include "common/const.h"
#include "common/entity_state.h"
#include "common/cl_entity.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_ScorePanel.h"
#include "..\game_shared\vgui_helpers.h"
#include "..\game_shared\vgui_loadtga.h"
#include "mod/AvHConstants.h"
#include "mod/AvHTitles.h"
#include "mod/AvHBasePlayerWeaponConstants.h"
#include "vgui_SpectatorPanel.h"
#include "cl_dll/demo.h"
#include "mod/AvHServerVariables.h"
#include "util\STLUtil.h"
#include "ui/ScoreboardIcon.h"

#include "common/ITrackerUser.h"
extern ITrackerUser *g_pTrackerUser;

hud_player_info_t	 g_PlayerInfoList[MAX_PLAYERS+1];	   // player info from the engine
extra_player_info_t  g_PlayerExtraInfo[MAX_PLAYERS+1];   // additional player info sent directly to the client dll
team_info_t			 g_TeamInfo[MAX_TEAMS+1];
int					 g_IsSpectator[MAX_PLAYERS+1];

int HUD_IsGame( const char *game );
int EV_TFC_IsAllyTeam( int iTeam1, int iTeam2 );

// Scoreboard dimensions
#define SBOARD_TITLE_SIZE_Y			YRES(22)

#define X_BORDER					XRES(4)

void LoadData(void* inBuffer, const unsigned char* inData, int inSizeToCopy, int& inSizeVariable);
void SaveData(unsigned char* inBuffer, const void* inData, int inSizeToCopy, int& inSizeVariable);

int	ScorePanel_InitializeDemoPlayback(int inSize, unsigned char* inBuffer)
{
	int theBytesRead = 0;

	LoadData(&g_PlayerInfoList, inBuffer, (MAX_PLAYERS+1)*sizeof(hud_player_info_t), theBytesRead);
	LoadData(&g_PlayerExtraInfo, inBuffer, (MAX_PLAYERS+1)*sizeof(extra_player_info_t), theBytesRead);
	LoadData(&g_TeamInfo, inBuffer, (MAX_PLAYERS+1)*sizeof(team_info_t), theBytesRead);
	LoadData(&g_IsSpectator, inBuffer, (MAX_PLAYERS+1)*sizeof(int), theBytesRead);

	return theBytesRead;
}

void ScorePanel_InitializeDemoRecording()
{
	// Now save out team info
	int theTotalSize = (MAX_PLAYERS + 1)*(sizeof(hud_player_info_t) + sizeof(extra_player_info_t) + sizeof(team_info_t) + sizeof(int));
	unsigned char* theCharArray = new unsigned char[theTotalSize];
	if(theCharArray)
	{
		int theCounter = 0;
		SaveData(theCharArray, &g_PlayerInfoList, (MAX_PLAYERS+1)*sizeof(hud_player_info_t), theCounter);
		SaveData(theCharArray, &g_PlayerExtraInfo, (MAX_PLAYERS+1)*sizeof(extra_player_info_t), theCounter);
		SaveData(theCharArray, &g_TeamInfo, (MAX_PLAYERS+1)*sizeof(team_info_t), theCounter);
		SaveData(theCharArray, &g_IsSpectator, (MAX_PLAYERS+1)*sizeof(int), theCounter);
		
		Demo_WriteBuffer(TYPE_PLAYERINFO, theTotalSize, theCharArray);
	}
}

// Column sizes
class SBColumnInfo
{
public:
	char				*m_pTitle;		// If null, ignore, if starts with #, it's localized, otherwise use the string directly.
	int					m_Width;		// Based on 640 width. Scaled to fit other resolutions.
	Label::Alignment	m_Alignment;	
};

// grid size is marked out for 640x480 screen

SBColumnInfo g_ColumnInfo[NUM_COLUMNS] =
{
	{NULL,			24,			Label::a_center},		// tracker column
    {NULL,			24,			Label::a_center},		// status icons
	{NULL,			110,		Label::a_center},		// name
	{NULL,			56,			Label::a_center},		// class
	{NULL,			40,			Label::a_center},     // resources
	{NULL,			18,			Label::a_center},     // weld
	{NULL,			18,			Label::a_center},     // weld
	{"#SCORE",		35,			Label::a_center},     // score
    {"#KILLS",      35,         Label::a_center},     // kills
	{"#DEATHS",		35,			Label::a_center},     // deaths
	{"#LATENCY",	35,			Label::a_center},     // ping
	{"#VOICE",		40,			Label::a_center},     
	{NULL,			2,			Label::a_center},		// blank column to take up the slack
};


#define TEAM_NO				0
#define TEAM_YES			1
#define TEAM_SPECTATORS		2
#define TEAM_BLANK			3

//-----------------------------------------------------------------------------
// ScorePanel::HitTestPanel.
//-----------------------------------------------------------------------------

void ScorePanel::HitTestPanel::internalMousePressed(MouseCode code)
{
	for(int i=0;i<_inputSignalDar.getCount();i++)
	{
		_inputSignalDar[i]->mousePressed(code,this);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

vgui::Color BuildColor( int R, int G, int B, float gamma )
{
	ASSERT( gamma != 0 );
	return vgui::Color( R/gamma, G/gamma, B/gamma, 0 );
}

//-----------------------------------------------------------------------------
// Purpose: Create the ScoreBoard panel
//-----------------------------------------------------------------------------
ScorePanel::ScorePanel(int x,int y,int wide,int tall) : Panel(x,y,wide,tall)
{
	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle("Scoreboard Title Text");
	SchemeHandle_t hSmallScheme = pSchemes->getSchemeHandle("Scoreboard Small Text");
	SchemeHandle_t hTinyScheme = pSchemes->getSchemeHandle("Scoreboard Tiny Text");
	Font *tfont = pSchemes->getFont(hTitleScheme);
	Font *smallfont = pSchemes->getFont(hSmallScheme);
	Font *tinyfont = pSchemes->getFont(hTinyScheme);
	
	setBgColor(0, 0, 0, 96);
	m_pCurrentHighlightLabel = NULL;
	m_iHighlightRow = -1;
	// : 0001073
	m_pTrackerIcon = NULL;
	m_pDevIcon = NULL;
	m_pPTIcon = NULL;
	m_pGuideIcon = NULL;
	m_pServerOpIcon = NULL;
	m_pContribIcon = NULL;
	m_pCheatingDeathIcon = NULL;
	m_pVeteranIcon = NULL;
	
	m_pHMG = NULL;
	m_pMine = NULL;
	m_pWeld = NULL;
	m_pGL = NULL;
	m_pSG = NULL;


	m_pTrackerIcon = vgui_LoadTGANoInvertAlpha("gfx/vgui/640_scoreboardtracker.tga");
	m_pDevIcon = vgui_LoadTGANoInvertAlpha("gfx/vgui/640_scoreboarddev.tga");
	m_pPTIcon = vgui_LoadTGANoInvertAlpha("gfx/vgui/640_scoreboardpt.tga");
	m_pGuideIcon = vgui_LoadTGANoInvertAlpha("gfx/vgui/640_scoreboardguide.tga");
	m_pServerOpIcon = vgui_LoadTGANoInvertAlpha("gfx/vgui/640_scoreboardserverop.tga");
	m_pContribIcon = vgui_LoadTGANoInvertAlpha("gfx/vgui/640_scoreboardcontrib.tga");
	m_pCheatingDeathIcon = vgui_LoadTGANoInvertAlpha("gfx/vgui/640_scoreboardcd.tga");
	m_pVeteranIcon = vgui_LoadTGANoInvertAlpha("gfx/vgui/640_scoreboardveteran.tga");

	

	m_pHMG = vgui_LoadTGANoInvertAlpha("gfx/vgui/640_scoreboardhmg.tga");
	m_pMine = vgui_LoadTGANoInvertAlpha("gfx/vgui/640_scoreboardmine.tga");
	m_pWeld = vgui_LoadTGANoInvertAlpha("gfx/vgui/640_scoreboardwelder.tga");
	m_pGL = vgui_LoadTGANoInvertAlpha("gfx/vgui/640_scoreboardgl.tga");
	m_pSG = vgui_LoadTGANoInvertAlpha("gfx/vgui/640_scoreboardsg.tga");

	m_iIconFrame = 0;
	m_iLastFrameIncrementTime = gHUD.GetTimeOfLastUpdate();
	
	// Initialize the top title.
	m_TitleLabel.setFont(tfont);
	m_TitleLabel.setText("");
	m_TitleLabel.setBgColor( 0, 0, 0, 255 );
	m_TitleLabel.setFgColor( Scheme::sc_primary1 );
	m_TitleLabel.setContentAlignment( vgui::Label::a_center );

	LineBorder *border = new LineBorder(Color(60, 60, 60, 128));
	setBorder(border);
	setPaintBorderEnabled(true);

	int xpos = g_ColumnInfo[0].m_Width + 3;
	if (ScreenWidth() >= 640)
	{
		// only expand column size for res greater than 640
		xpos = XRES(xpos);
	}
	m_TitleLabel.setBounds(xpos, 4, wide, SBOARD_TITLE_SIZE_Y);
	m_TitleLabel.setContentFitted(false);
	m_TitleLabel.setParent(this);

	// Setup the header (labels like "name", "class", etc..).
	m_HeaderGrid.SetDimensions(NUM_COLUMNS, 1);
	m_HeaderGrid.SetSpacing(0, 0);
	
	for(int i=0; i < NUM_COLUMNS; i++)
	{
		if (g_ColumnInfo[i].m_pTitle && g_ColumnInfo[i].m_pTitle[0] == '#')
			m_HeaderLabels[i].setText(CHudTextMessage::BufferedLocaliseTextString(g_ColumnInfo[i].m_pTitle));
		else if(g_ColumnInfo[i].m_pTitle)
			m_HeaderLabels[i].setText(g_ColumnInfo[i].m_pTitle);

		int xwide = g_ColumnInfo[i].m_Width;
		if (ScreenWidth() >= 640)
		{
			xwide = XRES(xwide);
		}
		else if (ScreenWidth() == 400)
		{
			// hack to make 400x300 resolution scoreboard fit
			if (i == 1)
			{
				// reduces size of player name cell
				xwide -= 28;
			}
			else if (i == 0)
			{
				// tracker icon cell
				xwide -= 8;
			}
		}
		
		m_HeaderGrid.SetColumnWidth(i, xwide);
		m_HeaderGrid.SetEntry(i, 0, &m_HeaderLabels[i]);

		m_HeaderLabels[i].setBgColor(0,0,0,255);
		m_HeaderLabels[i].setBgColor(0,0,0,255);
		
        int theColorIndex = 0;
        Color gammaAdjustedTeamColor = BuildColor(kTeamColors[theColorIndex][0], kTeamColors[theColorIndex][1], kTeamColors[theColorIndex][2], gHUD.GetGammaSlope());
        int theR, theG, theB, theA;
        gammaAdjustedTeamColor.getColor(theR, theG, theB, theA);
        m_HeaderLabels[i].setFgColor(theR, theG, theB, theA);

		m_HeaderLabels[i].setFont(smallfont);
		m_HeaderLabels[i].setContentAlignment(g_ColumnInfo[i].m_Alignment);

		int yres = 12;
		if (ScreenHeight() >= 480)
		{
			yres = YRES(yres);
		}
		m_HeaderLabels[i].setSize(50, yres);
	}

	// Set the width of the last column to be the remaining space.
	int ex, ey, ew, eh;
	m_HeaderGrid.GetEntryBox(NUM_COLUMNS - 2, 0, ex, ey, ew, eh);
	m_HeaderGrid.SetColumnWidth(NUM_COLUMNS - 1, (wide - X_BORDER) - (ex + ew));

	m_HeaderGrid.AutoSetRowHeights();
	m_HeaderGrid.setBounds(X_BORDER, SBOARD_TITLE_SIZE_Y, wide - X_BORDER*2, m_HeaderGrid.GetRowHeight(0));
	m_HeaderGrid.setParent(this);
	m_HeaderGrid.setBgColor(0,0,0,255);


	// Now setup the listbox with the actual player data in it.
	int headerX, headerY, headerWidth, headerHeight;
	m_HeaderGrid.getBounds(headerX, headerY, headerWidth, headerHeight);
	m_PlayerList.setBounds(headerX, headerY+headerHeight, headerWidth, tall - headerY - headerHeight - 6);
	m_PlayerList.setBgColor(0,0,0,255);
	m_PlayerList.setParent(this);

	for(int row=0; row < NUM_ROWS; row++)
	{
		CGrid *pGridRow = &m_PlayerGrids[row];

		pGridRow->SetDimensions(NUM_COLUMNS, 1);
		
		for(int col=0; col < NUM_COLUMNS; col++)
		{
			m_PlayerEntries[col][row].setContentFitted(false);
			m_PlayerEntries[col][row].setRow(row);
			m_PlayerEntries[col][row].addInputSignal(this);
			pGridRow->SetEntry(col, 0, &m_PlayerEntries[col][row]);
		}

		pGridRow->setBgColor(0,0,0,255);
//		pGridRow->SetSpacing(2, 0);f
		pGridRow->SetSpacing(0, 0);
		pGridRow->CopyColumnWidths(&m_HeaderGrid);
		pGridRow->AutoSetRowHeights();
		pGridRow->setSize(PanelWidth(pGridRow), pGridRow->CalcDrawHeight());
		pGridRow->RepositionContents();

		m_PlayerList.AddItem(pGridRow);
	}


	// Add the hit test panel. It is invisible and traps mouse clicks so we can go into squelch mode.
	m_HitTestPanel.setBgColor(0,0,0,255);
	m_HitTestPanel.setParent(this);
	m_HitTestPanel.setBounds(0, 0, wide, tall);
	m_HitTestPanel.addInputSignal(this);

	m_pCloseButton = new CommandButton( "x", wide-XRES(12 + 4), YRES(2), XRES( 12 ) , YRES( 12 ) );
	m_pCloseButton->setParent( this );
	m_pCloseButton->addActionSignal( new CMenuHandler_StringCommandWatch( "-showscores", true ) );
	m_pCloseButton->setBgColor(0,0,0,255);
	m_pCloseButton->setFgColor( 255, 255, 255, 0 );
	m_pCloseButton->setFont(tfont);
	m_pCloseButton->setBoundKey( (char)255 );
	m_pCloseButton->setContentAlignment(Label::a_center);
	Initialize();
}


//-----------------------------------------------------------------------------
// Purpose: Called each time a new level is started.
//-----------------------------------------------------------------------------
void ScorePanel::Initialize( void )
{
	// Clear out scoreboard data
	m_iLastKilledBy = 0;
	m_fLastKillTime = 0;
	m_iPlayerNum = 0;
	m_iNumTeams = 0;
	// : 0001073
//	for( int counter = 0; counter < MAX_PLAYERS+1; counter++ )
//	{
//		delete g_PlayerExtraInfo[counter].icon;
//	}

	memset( g_PlayerExtraInfo, 0, sizeof g_PlayerExtraInfo );
	memset( g_TeamInfo, 0, sizeof g_TeamInfo );
}

bool HACK_GetPlayerUniqueID( int iPlayer, char playerID[16] )
{
	return !!gEngfuncs.GetPlayerUniqueID( iPlayer, playerID );
}
//-----------------------------------------------------------------------------
// Purpose: Recalculate the internal scoreboard data
//-----------------------------------------------------------------------------
void ScorePanel::Update()
{
	// Set the title
	char title[128];

	char theServerName[MAX_SERVERNAME_LENGTH+1];
	if (gViewPort->m_szServerName)
	{
		memset(theServerName, 0, MAX_SERVERNAME_LENGTH+1);
		int iServerNameLength = max((int)strlen(gViewPort->m_szServerName),MAX_SERVERNAME_LENGTH);
		strncat(theServerName, gViewPort->m_szServerName, iServerNameLength);
	}
	theServerName[MAX_SERVERNAME_LENGTH]=0;
	char theMapName[MAX_MAPNAME_LENGTH+1];
	sprintf(theMapName, "%s", gHUD.GetMapName().c_str());

	int theTimeElapsed = gHUD.GetGameTime();
	char elapsedString[64];
	if ( theTimeElapsed > 0 ) {
		int theMinutesElapsed = theTimeElapsed/60;
		int theSecondsElapsed = theTimeElapsed%60;
		sprintf(elapsedString, "Game time: %d:%02d", theMinutesElapsed, theSecondsElapsed);
	}
	else {
		sprintf(elapsedString, "");
	}

	sprintf(title, "%32s    Map: %s        %s", theServerName, theMapName, elapsedString);

	m_TitleLabel.setText(title);

    int theColorIndex = 0;
    
    // Set gamma-correct title color
    Color gammaAdjustedTeamColor = BuildColor(kTeamColors[theColorIndex][0], kTeamColors[theColorIndex][1], kTeamColors[theColorIndex][2], gHUD.GetGammaSlope());
    
    int theR, theG, theB, theA;
    gammaAdjustedTeamColor.getColor(theR, theG, theB, theA);

    m_TitleLabel.setFgColor(theR, theG, theB, theA);

	m_iRows = 0;
	gViewPort->GetAllPlayersInfo();

	// Clear out sorts
	int i = 0;
	for (i = 0; i < NUM_ROWS; i++)
	{
		m_iSortedRows[i] = 0;
		m_iIsATeam[i] = TEAM_IND;
	}

	// Fix for memory overrun bug
	for (i = 0; i < MAX_PLAYERS; i++)
	{
		m_bHasBeenSorted[i] = false;
	}

	SortTeams();

	// set scrollbar range
	m_PlayerList.SetScrollRange(m_iRows);

	FillGrid();
	if ( gViewPort->m_pSpectatorPanel->m_menuVisible )
	{
		 m_pCloseButton->setVisible ( true );
	}
	else 
	{
		 m_pCloseButton->setVisible ( false );
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sort all the teams
//-----------------------------------------------------------------------------
void ScorePanel::SortTeams()
{
	// clear out team scores
	float theCurrentTime = gHUD.GetTimeOfLastUpdate();

	for ( int i = 1; i <= m_iNumTeams; i++ )
	{
		if ( !g_TeamInfo[i].scores_overriden )
			g_TeamInfo[i].score =0;
		g_TeamInfo[i].frags = g_TeamInfo[i].deaths = g_TeamInfo[i].ping = g_TeamInfo[i].packetloss = 0;
	}

	// recalc the team scores, then draw them
	for ( i = 1; i <= MAX_PLAYERS; i++ )
	{
		if ( g_PlayerInfoList[i].name == NULL )
			continue; // empty player slot, skip

		if ( g_PlayerExtraInfo[i].teamname[0] == 0 )
			continue; // skip over players who are not in a team

		// find what team this player is in
		for ( int j = 1; j <= m_iNumTeams; j++ )
		{
			if ( !stricmp( g_PlayerExtraInfo[i].teamname, g_TeamInfo[j].name ) )
				break;
		}
		if ( j > m_iNumTeams )  // player is not in a team, skip to the next guy
			continue;

		if ( !g_TeamInfo[j].scores_overriden )
		{
            g_TeamInfo[j].score += g_PlayerExtraInfo[i].score;
		}

		g_TeamInfo[j].deaths += g_PlayerExtraInfo[i].deaths;
		g_TeamInfo[j].frags += g_PlayerExtraInfo[i].frags;
		g_TeamInfo[j].ping += g_PlayerInfoList[i].ping;
		g_TeamInfo[j].packetloss += g_PlayerInfoList[i].packetloss;

		if ( g_PlayerInfoList[i].thisplayer )
			g_TeamInfo[j].ownteam = TRUE;
		else
			g_TeamInfo[j].ownteam = FALSE;

		// Set the team's number (used for team colors)
		g_TeamInfo[j].teamnumber = g_PlayerExtraInfo[i].teamnumber;
	}

	// find team ping/packetloss averages
	for ( i = 1; i <= m_iNumTeams; i++ )
	{
		g_TeamInfo[i].already_drawn = FALSE;

		if ( g_TeamInfo[i].players > 0 )
		{
			g_TeamInfo[i].ping /= g_TeamInfo[i].players;  // use the average ping of all the players in the team as the teams ping
			g_TeamInfo[i].packetloss /= g_TeamInfo[i].players;  // use the average ping of all the players in the team as the teams ping
		}
	}
	vector<string> teams;
	if ( gHUD.GetHUDTeam() == TEAM_TWO || gHUD.GetHUDTeam() == TEAM_FOUR ) {
		SortActivePlayers(kAlien1Team);
		SortActivePlayers(kMarine1Team);
		SortActivePlayers(kAlien2Team);
		SortActivePlayers(kMarine2Team);
	}
	else {
		SortActivePlayers(kMarine1Team);
		SortActivePlayers(kAlien1Team);
		SortActivePlayers(kMarine2Team);
		SortActivePlayers(kAlien2Team);
	}

	SortActivePlayers(kSpectatorTeam);
	SortActivePlayers(kUndefinedTeam);
}

void ScorePanel::SortActivePlayers(char* inTeam, bool inSortByEntityIndex)
{
	for(int i = 1; i <= m_iNumTeams; i++)
	{
		if(!strcmp(g_TeamInfo[i].name, inTeam))
		{
			int best_team = i;
			
			// Put this team in the sorted list
			m_iSortedRows[ m_iRows ] = best_team;
			m_iIsATeam[ m_iRows ] = TEAM_YES;
			g_TeamInfo[best_team].already_drawn = TRUE;  // set the already_drawn to be TRUE, so this team won't get sorted again
			m_iRows++;
			
			// Now sort all the players on this team
			SortPlayers(0, g_TeamInfo[best_team].name, inSortByEntityIndex);
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Sort a list of players
//-----------------------------------------------------------------------------
void ScorePanel::SortPlayers( int iTeam, char *team, bool inSortByEntityIndex)
{
	bool bCreatedTeam = false;

	// draw the players, in order,  and restricted to team if set
	while ( 1 )
	{
		// Find the top ranking player
		int theBestTotalScore = -99999;
        int theBestDeaths = 0;
		int theBestPlayer = 0;

		for ( int i = 1; i <= MAX_PLAYERS; i++ )
		{
			if ( m_bHasBeenSorted[i] == false && g_PlayerInfoList[i].name )
			{
				cl_entity_t *ent = gEngfuncs.GetEntityByIndex( i );

				if ( ent && !(team && stricmp(g_PlayerExtraInfo[i].teamname, team)) )  
				{
					extra_player_info_t *pl_info = &g_PlayerExtraInfo[i];

					// Sort by player index to mask marine status
					if(inSortByEntityIndex)
					{
						if((theBestPlayer == 0) || (i < theBestPlayer))
						{
							theBestPlayer = i;
						}
					}
					else
                    {
                        // overall rank = score + kills (with least deaths breaking ties)
                        int thePlayerScore = pl_info->score;
                        int thePlayerDeaths = pl_info->deaths;
                        if((thePlayerScore > theBestTotalScore) || ((thePlayerScore == theBestTotalScore) && (pl_info->deaths < theBestDeaths)))
                        {
                            theBestPlayer = i;
                            theBestTotalScore = thePlayerScore;
                            theBestDeaths = thePlayerDeaths;
                        }
                    }
				}
			}
		}

		if ( !theBestPlayer )
			break;

		// If we haven't created the Team yet, do it first
		if (!bCreatedTeam && iTeam)
		{
			m_iIsATeam[ m_iRows ] = iTeam;
			m_iRows++;

			bCreatedTeam = true;
		}

		// Put this player in the sorted list
		m_iSortedRows[ m_iRows ] = theBestPlayer;
		m_bHasBeenSorted[ theBestPlayer ] = true;
		m_iRows++;
	}

	if (team)
	{
		m_iIsATeam[m_iRows++] = TEAM_BLANK;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Recalculate the existing teams in the match
//-----------------------------------------------------------------------------
void ScorePanel::RebuildTeams()
{
	// clear out player counts from teams
	for ( int i = 1; i <= m_iNumTeams; i++ )
	{
		g_TeamInfo[i].players = 0;
	}

	// rebuild the team list
	gViewPort->GetAllPlayersInfo();
	m_iNumTeams = 0;
	for ( i = 1; i <= MAX_PLAYERS; i++ )
	{
		if ( g_PlayerInfoList[i].name == NULL )
			continue;

		if ( g_PlayerExtraInfo[i].teamname[0] == 0 )
			continue; // skip over players who are not in a team

		// is this player in an existing team?
		for ( int j = 1; j <= m_iNumTeams; j++ )
		{
			if ( g_TeamInfo[j].name[0] == '\0' )
				break;

			if ( !stricmp( g_PlayerExtraInfo[i].teamname, g_TeamInfo[j].name ) )
				break;
		}

		if ( j > m_iNumTeams )
		{ // they aren't in a listed team, so make a new one
			// search through for an empty team slot
			for ( int j = 1; j <= m_iNumTeams; j++ )
			{
				if ( g_TeamInfo[j].name[0] == '\0' )
					break;
			}
			m_iNumTeams = max( j, m_iNumTeams );

			strncpy( g_TeamInfo[j].name, g_PlayerExtraInfo[i].teamname, MAX_TEAM_NAME );
			g_TeamInfo[j].players = 0;
		}

		g_TeamInfo[j].players++;
	}

	// clear out any empty teams
	for ( i = 1; i <= m_iNumTeams; i++ )
	{
		if ( g_TeamInfo[i].players < 1 )
			memset( &g_TeamInfo[i], 0, sizeof(team_info_t) );
	}

	// Update the scoreboard
	Update();
}

//-----------------------------------------------------------------------------

int ScorePanel::GetIconFrame(void)
{
	const static int kIconFrameDuration = 0.25;

	int current_time = gHUD.GetTimeOfLastUpdate();
	if( (m_iLastFrameIncrementTime - current_time) > kIconFrameDuration )
	{
		m_iLastFrameIncrementTime = current_time;
		m_iIconFrame++;
		if( m_iIconFrame >= 1000 )
		{
			m_iIconFrame = 0;
		}
	}
	return m_iIconFrame;
}

//-----------------------------------------------------------------------------

void ScorePanel::FillGrid()
{
	bool isNsMode=( strnicmp(gHUD.GetMapName().c_str(), "ns_", 3) == 0 );

	CSchemeManager *pSchemes = gViewPort->GetSchemeManager();
	SchemeHandle_t hScheme = pSchemes->getSchemeHandle("Scoreboard Text");
	SchemeHandle_t hTitleScheme = pSchemes->getSchemeHandle("Scoreboard Title Text");
	SchemeHandle_t hSmallScheme = pSchemes->getSchemeHandle("Scoreboard Small Text");
	SchemeHandle_t hTinyScheme = pSchemes->getSchemeHandle("Scoreboard Tiny Text");

	Font *sfont = pSchemes->getFont(hScheme);
	Font *tfont = pSchemes->getFont(hTitleScheme);
	Font *smallfont = pSchemes->getFont(hSmallScheme);
	Font *tinyfont = pSchemes->getFont(hTinyScheme);

	// update highlight position
	int x, y;
	getApp()->getCursorPos(x, y);
	cursorMoved(x, y, this);
	
	// remove highlight row if we're not in squelch mode
	if (!GetClientVoiceMgr()->IsInSquelchMode())
	{
		m_iHighlightRow = -1;
	}
	
	bool bNextRowIsGap = false;
	m_HeaderLabels[COLUMN_EXTRA].setText("");
	m_HeaderLabels[COLUMN_MINE].setText("");
	m_HeaderLabels[COLUMN_WELD].setText("");
	if ( isNsMode ) {
		if ( gHUD.GetHUDTeam() == TEAM_ONE || gHUD.GetHUDTeam() == TEAM_THREE ) {
			m_HeaderLabels[COLUMN_EXTRA].setText(CHudTextMessage::BufferedLocaliseTextString("#COLWEAP"));
		}
		else if ( gHUD.GetHUDTeam() == TEAM_TWO || gHUD.GetHUDTeam() == TEAM_FOUR ) { 
			m_HeaderLabels[COLUMN_EXTRA].setText(CHudTextMessage::BufferedLocaliseTextString("#COLRES"));
		}
	}
	else {
		if ( gHUD.GetHUDTeam() != TEAM_IND && gHUD.GetHUDTeam() != TEAM_SPECT )
			m_HeaderLabels[COLUMN_EXTRA].setText(CHudTextMessage::BufferedLocaliseTextString("#COLLEVEL"));
	}
	
	for(int row=0; row < NUM_ROWS; row++)
	{
		CGrid *pGridRow = &m_PlayerGrids[row];
		pGridRow->SetRowUnderline(0, false, 0, 0, 0, 0, 0);
		
		if(row >= m_iRows)
		{
			for(int col=0; col < NUM_COLUMNS; col++)
				m_PlayerEntries[col][row].setVisible(false);
			
			continue;
		}
		
		bool bRowIsGap = false;
		if (bNextRowIsGap)
		{
			bNextRowIsGap = false;
			bRowIsGap = true;
		}
		
		// Get the player's data
		int theSortedRow = m_iSortedRows[row];
		hud_player_info_t* pl_info = &g_PlayerInfoList[theSortedRow];
		extra_player_info_t* theExtraPlayerInfo = &g_PlayerExtraInfo[theSortedRow];
		int thePlayerClass = theExtraPlayerInfo->playerclass;
		short theTeamNumber = theExtraPlayerInfo->teamnumber;
		string theCustomIcon = (string)theExtraPlayerInfo->customicon;
// : 0001073
		short thePlayerAuthentication = theExtraPlayerInfo->auth;
		bool thePlayerIsDead = false;
		switch( thePlayerClass )
		{
		case PLAYERCLASS_DEAD_MARINE:
		case PLAYERCLASS_DEAD_ALIEN:
		case PLAYERCLASS_REINFORCING:
			thePlayerIsDead = true;
			break;
		}

		// Code to test DEBUG
#if 0
		#ifdef DEBUG
		extern int gGlobalDebugAuth;
		thePlayerAuthentication = 1;
		thePlayerAuthentication <<= gGlobalDebugAuth;
		#endif
#endif

		team_info_t* team_info = &g_TeamInfo[m_iSortedRows[row]];
		int theColorIndex = theTeamNumber % iNumberOfTeamColors;

        int theLocalPlayerTeam = 0;
        if(gEngfuncs.GetLocalPlayer())
        {
            theLocalPlayerTeam = gEngfuncs.GetLocalPlayer()->curstate.team;
        }
		
		for(int col=0; col < NUM_COLUMNS; col++)
		{
			CLabelHeader *pLabel = &m_PlayerEntries[col][row];

			pLabel->setVisible(true);
			pLabel->setText2("");
			pLabel->setImage(NULL);
			pLabel->setFont(sfont);
			pLabel->setTextOffset(0, 0);
			
			int rowheight = 13;
			if (ScreenHeight() > 480)
			{
				rowheight = YRES(rowheight);
			}
			else
			{
				// more tweaking, make sure icons fit at low res
				rowheight = 15;
			}
			pLabel->setSize(pLabel->getWide(), rowheight);
			pLabel->setBgColor(0, 0, 0, 255);
			
			char sz[128];

			Color gammaAdjustedTeamColor = BuildColor(kTeamColors[theColorIndex][0], kTeamColors[theColorIndex][1], kTeamColors[theColorIndex][2], gHUD.GetGammaSlope());
            pLabel->setFgColor(gammaAdjustedTeamColor[0], gammaAdjustedTeamColor[1], gammaAdjustedTeamColor[2], 0);

			if (m_iIsATeam[row] == TEAM_BLANK)
			{
				pLabel->setText(" ");
				continue;
			}
			else if ( m_iIsATeam[row] == TEAM_YES )
			{
				theColorIndex = team_info->teamnumber % iNumberOfTeamColors;

				// team color text for team names
				
				

				// different height for team header rows
				rowheight = 20;
				if (ScreenHeight() >= 480)
				{
					rowheight = YRES(rowheight);
				}
				pLabel->setSize(pLabel->getWide(), rowheight);
				pLabel->setFont(tfont);

				pGridRow->SetRowUnderline(	0,
											true,
											YRES(3),
											gammaAdjustedTeamColor[0],
											gammaAdjustedTeamColor[1],
											gammaAdjustedTeamColor[2],
											0 );
			}
			else if ( m_iIsATeam[row] == TEAM_SPECTATORS )
			{
				// grey text for spectators
				pLabel->setFgColor(100, 100, 100, 0);

				// different height for team header rows
				rowheight = 20;
				if (ScreenHeight() >= 480)
				{
					rowheight = YRES(rowheight);
				}
				pLabel->setSize(pLabel->getWide(), rowheight);
				pLabel->setFont(tfont);

				pGridRow->SetRowUnderline(0, true, YRES(3), 100, 100, 100, 0);
			}
			else
			{
				if(thePlayerIsDead)
				{
					pLabel->setFgColor(255, 0, 0, 0);
				}
				else
				{
					// team color text for player names
					pLabel->setFgColor(gammaAdjustedTeamColor[0], gammaAdjustedTeamColor[1], gammaAdjustedTeamColor[2], 0);
				}
				
				// Set background color
				if ( pl_info && pl_info->thisplayer ) // if it is their name, draw it a different color
				{
					// Highlight this player
					pLabel->setFgColor(Scheme::sc_white);
					pLabel->setBgColor(gammaAdjustedTeamColor[0], gammaAdjustedTeamColor[1], gammaAdjustedTeamColor[2], 196 );
				}
				else if ( theSortedRow == m_iLastKilledBy && m_fLastKillTime && m_fLastKillTime > gHUD.m_flTime )
				{
					// Killer's name
					pLabel->setBgColor( 255,0,0, 255 - ((float)15 * (float)(m_fLastKillTime - gHUD.m_flTime)) );
				}
			}				

			// Align 
            switch(col)
            {
            case COLUMN_NAME:
            case COLUMN_CLASS:
                pLabel->setContentAlignment( vgui::Label::a_west );
                break;

            case COLUMN_TRACKER:
            case COLUMN_RANK_ICON:
            case COLUMN_VOICE:
                pLabel->setContentAlignment( vgui::Label::a_center );
                break;

            case COLUMN_SCORE:
            case COLUMN_KILLS:
			case COLUMN_EXTRA:
			case COLUMN_MINE:
			case COLUMN_WELD:
			case COLUMN_DEATHS:
            case COLUMN_LATENCY:
            default:
                pLabel->setContentAlignment( vgui::Label::a_center );
                break;
            }

			// Fill out with the correct data
			strcpy(sz, "");
			if ( m_iIsATeam[row] )
			{
				char sz2[128];

				switch (col)
				{
				case COLUMN_NAME:
					if ( m_iIsATeam[row] == TEAM_SPECTATORS )
					{
						sprintf( sz2, CHudTextMessage::BufferedLocaliseTextString( "#Spectators" ) );
					}
					else
					{
						if(team_info)
						{
							sprintf( sz2, gViewPort->GetTeamName(team_info->teamnumber) );
						}
					}

					strcpy(sz, sz2);

					// Append the number of players
					if ( m_iIsATeam[row] == TEAM_YES && team_info)
					{
						if (team_info->players == 1)
						{
							sprintf(sz2, "(%d %s)", team_info->players, CHudTextMessage::BufferedLocaliseTextString( "#Player" ) );
						}
						else
						{
							sprintf(sz2, "(%d %s)", team_info->players, CHudTextMessage::BufferedLocaliseTextString( "#Player_plural" ) );
						}

						pLabel->setText2(sz2);
						pLabel->setFont2(smallfont);
					}
					break;
				case COLUMN_VOICE:
					break;
				case COLUMN_CLASS:
					break;
                case COLUMN_SCORE:
                    if ((m_iIsATeam[row] == TEAM_YES) && team_info && (theLocalPlayerTeam == team_info->teamnumber || (gHUD.GetPlayMode() == PLAYMODE_OBSERVER)))
                        sprintf(sz, "%d",  team_info->score);
                    break;
				case COLUMN_MINE:
					break;
				case COLUMN_WELD:
					break;
				case COLUMN_EXTRA:
					break;
				case COLUMN_KILLS:
					if ((m_iIsATeam[row] == TEAM_YES) && team_info) 
						sprintf(sz, "%d",  team_info->frags );
					break;
				case COLUMN_DEATHS:
					if ((m_iIsATeam[row] == TEAM_YES) && team_info)
						sprintf(sz, "%d",  team_info->deaths );
					break;
				case COLUMN_LATENCY:
					if ((m_iIsATeam[row] == TEAM_YES) && team_info)
						sprintf(sz, "%d", team_info->ping );
					break;
				default:
					break;
				}
			}
			else
			{
				// Are these stats for an enemy?  Score and other stats shouldn't be drawn for enemies.
				bool theIsForEnemy = false;

				int theLocalPlayerTeam = 0;
				if(gEngfuncs.GetLocalPlayer())
				{
					theLocalPlayerTeam = gEngfuncs.GetLocalPlayer()->curstate.team;
				}

				if((theLocalPlayerTeam != 0) && (theExtraPlayerInfo->teamnumber != theLocalPlayerTeam))
				{
					theIsForEnemy = true;
				}
				
				switch (col)
				{
				case COLUMN_NAME:
					
					if (g_pTrackerUser)
					{
						int playerSlot = m_iSortedRows[row];
						int trackerID = gEngfuncs.GetTrackerIDForPlayer(playerSlot);
						const char *trackerName = g_pTrackerUser->GetUserName(trackerID);
						if (trackerName && *trackerName)
						{
							sprintf(sz, "   (%s)", trackerName);
							pLabel->setText2(sz);
						}
					}
					
					if(pl_info)
					{
						sprintf(sz, "%s  ", pl_info->name);
					}
					break;
				case COLUMN_VOICE:
					sz[0] = 0;
					// in HLTV mode allow spectator to turn on/off commentator voice
					if (pl_info && (!pl_info->thisplayer || gEngfuncs.IsSpectateOnly()))
					{
						GetClientVoiceMgr()->UpdateSpeakerImage(pLabel, theSortedRow);
					}
					break;
				case COLUMN_CLASS:
					// No class for other team's members (unless allied or spectator, and make sure player is on our team)
					strcpy(sz, "");

					if(team_info && ((theLocalPlayerTeam == theTeamNumber) || (gHUD.GetPlayMode() == PLAYMODE_OBSERVER)))
					{
						switch(thePlayerClass)
						{
						case (int)(PLAYERCLASS_DEAD_MARINE):
						case (int)(PLAYERCLASS_DEAD_ALIEN):
							sprintf(sz, "%s", CHudTextMessage::BufferedLocaliseTextString(kClassDead));
							break;
						case (int)(PLAYERCLASS_REINFORCING):
							sprintf(sz, "%s", CHudTextMessage::BufferedLocaliseTextString(kClassReinforcing));
							break;
						case (int)(PLAYERCLASS_REINFORCINGCOMPLETE):
							sprintf(sz, "%s", CHudTextMessage::BufferedLocaliseTextString(kClassReinforcingComplete));
							break;
						case (int)(PLAYERCLASS_ALIVE_JETPACK_MARINE):
							sprintf(sz, "%s", CHudTextMessage::BufferedLocaliseTextString(kClassJetpackMarine));
							break;
						case (int)(PLAYERCLASS_ALIVE_HEAVY_MARINE):
							sprintf(sz, "%s", CHudTextMessage::BufferedLocaliseTextString(kClassHeavyMarine));
							break;
						case (int)(PLAYERCLASS_COMMANDER):
							sprintf(sz, "%s", CHudTextMessage::BufferedLocaliseTextString(kClassCommander));
							break;
						case (int)(PLAYERCLASS_ALIVE_LEVEL1):
							sprintf(sz, "%s", CHudTextMessage::BufferedLocaliseTextString(kClassLevel1));
							break;
						case (int)(PLAYERCLASS_ALIVE_LEVEL2):
							sprintf(sz, "%s", CHudTextMessage::BufferedLocaliseTextString(kClassLevel2));
							break;
						case (int)(PLAYERCLASS_ALIVE_LEVEL3):
							sprintf(sz, "%s", CHudTextMessage::BufferedLocaliseTextString(kClassLevel3));
							break;
						case (int)(PLAYERCLASS_ALIVE_LEVEL4):
							sprintf(sz, "%s", CHudTextMessage::BufferedLocaliseTextString(kClassLevel4));
							break;
						case (int)(PLAYERCLASS_ALIVE_LEVEL5):
							sprintf(sz, "%s", CHudTextMessage::BufferedLocaliseTextString(kClassLevel5));
							break;
						case (int)(PLAYERCLASS_ALIVE_DIGESTING):
							sprintf(sz, "%s", CHudTextMessage::BufferedLocaliseTextString(kClassDigesting));
							break;
						case (int)(PLAYERCLASS_ALIVE_GESTATING):
							sprintf(sz, "%s", CHudTextMessage::BufferedLocaliseTextString(kClassGestating));
							break;
						default:
							break;
						}
					}
					break;
						
				case COLUMN_RANK_ICON:
// : 0001073
#ifdef USE_OLDAUTH
					// Check if we have authority.  Right now these override the tracker icons.  Listed in increasing order of "importance".
					if(thePlayerAuthentication & PLAYERAUTH_CHEATINGDEATH)
					{
						// Red
						pLabel->setImage(m_pCheatingDeathIcon);
						pLabel->setFgColorAsImageColor(false);
						m_pCheatingDeathIcon->setColor(BuildColor(255, 69, 9, gHUD.GetGammaSlope()));
					}
					if(thePlayerAuthentication & PLAYERAUTH_VETERAN)
					{
						// Yellow
						pLabel->setImage(m_pVeteranIcon);
						pLabel->setFgColorAsImageColor(false);
						m_pVeteranIcon->setColor(BuildColor(248, 252, 0, gHUD.GetGammaSlope()));
					}
					if(thePlayerAuthentication & PLAYERAUTH_BETASERVEROP)
					{
						// Whitish
						pLabel->setImage(m_pServerOpIcon);
						pLabel->setFgColorAsImageColor(false);
						m_pServerOpIcon->setColor(BuildColor(220, 220, 220, gHUD.GetGammaSlope()));
					}
					if(thePlayerAuthentication & PLAYERAUTH_CONTRIBUTOR)
					{
						// Light blue
						pLabel->setImage(m_pContribIcon);
						pLabel->setFgColorAsImageColor(false);
						m_pContribIcon->setColor(BuildColor(117, 214, 241, gHUD.GetGammaSlope()));
					}
					if(thePlayerAuthentication & PLAYERAUTH_GUIDE)
					{
						// Magenta
						pLabel->setImage(m_pGuideIcon);
						pLabel->setFgColorAsImageColor(false);
						m_pGuideIcon->setColor(BuildColor(208, 16, 190, gHUD.GetGammaSlope()));
					}
					if(thePlayerAuthentication & PLAYERAUTH_PLAYTESTER)
					{
						// Orange
						pLabel->setImage(m_pPTIcon);
						pLabel->setFgColorAsImageColor(false);
						m_pPTIcon->setColor(BuildColor(255, 167, 54, gHUD.GetGammaSlope()));
					}
					if(thePlayerAuthentication & PLAYERAUTH_DEVELOPER)
					{
						// TSA blue
						pLabel->setImage(m_pDevIcon);
						pLabel->setFgColorAsImageColor(false);
						m_pDevIcon->setColor(BuildColor(100, 215, 255, gHUD.GetGammaSlope()));
					}

					if(thePlayerAuthentication & PLAYERAUTH_SERVEROP)
					{
						// Bright green
						pLabel->setImage(m_pServerOpIcon);
						pLabel->setFgColorAsImageColor(false);
						m_pServerOpIcon->setColor(BuildColor(0, 255, 0, gHUD.GetGammaSlope()));
					}

					// Allow custom icons to override other general icons
					if(thePlayerAuthentication & PLAYERAUTH_CUSTOM)
					{
						if(theCustomIcon != "")
						{
							string theIconName = theCustomIcon.substr(0, strlen(theCustomIcon.c_str()) - 3);
							string theFullCustomIconString = string("gfx/vgui/640_") + theIconName + string(".tga");

							vgui::BitmapTGA *pIcon = GetIconPointer(theCustomIcon);

							//Icon hasnt been loaded, load it now and add it to list of icons.
							if(pIcon == NULL)
							{
								pIcon = vgui_LoadTGANoInvertAlpha(theFullCustomIconString.c_str());

								if(pIcon)
									m_CustomIconList.push_back( make_pair(pIcon, theCustomIcon) );
							}
							
							if(pIcon)
							{
								pLabel->setImage(pIcon);
								pLabel->setFgColorAsImageColor(false);

								// Parse color (last 3 bytes are the RGB values 1-9)
								string theColor = theCustomIcon.substr( strlen(theCustomIcon.c_str())-3, 3);
								int theRed = (MakeIntFromString(theColor.substr(0, 1))/9.0f)*255;
								int theGreen = (MakeIntFromString(theColor.substr(1, 1))/9.0f)*255;
								int theBlue = (MakeIntFromString(theColor.substr(2, 1))/9.0f)*255;

								pIcon->setColor(BuildColor(theRed, theGreen, theBlue, gHUD.GetGammaSlope()));
							}
						}
					}
					
					if(g_pTrackerUser)
					{
						int playerSlot = theSortedRow;
						int trackerID = gEngfuncs.GetTrackerIDForPlayer(playerSlot);

						if (g_pTrackerUser->IsFriend(trackerID) && trackerID != g_pTrackerUser->GetTrackerID())
						{
							pLabel->setImage(m_pTrackerIcon);
							pLabel->setFgColorAsImageColor(false);
							m_pTrackerIcon->setColor(Color(255, 255, 255, 0));
						}
					}
#else
					if( theExtraPlayerInfo->icon )
					{
						vgui::Bitmap* image = theExtraPlayerInfo->icon->getImage( this->GetIconFrame() );
						if( image ) { pLabel->setImage( image ); }
					}
#endif
					break;
                case COLUMN_SCORE:
                    if(!theIsForEnemy && theLocalPlayerTeam != TEAM_IND || (gHUD.GetPlayMode() == PLAYMODE_OBSERVER))
                    {
                        const float kDeltaDisplayTime = 3.0f;
                        float theTimeSinceChange = gHUD.GetTimeOfLastUpdate() - theExtraPlayerInfo->timeOfLastScoreChange;
                        if((theExtraPlayerInfo->score > theExtraPlayerInfo->lastScore) && (theTimeSinceChange > 0) && (theTimeSinceChange < kDeltaDisplayTime) && (theExtraPlayerInfo->teamnumber != 0))
                        {
                            // draw score with change
                            int theDelta = (theExtraPlayerInfo->score - theExtraPlayerInfo->lastScore);
                            sprintf(sz, "(+%d) %d",  theDelta, theExtraPlayerInfo->score);
                        }
                        else
                        {
                            sprintf(sz, "%d", theExtraPlayerInfo->score);
                        }
                        
                    }
                    break;
				case COLUMN_WELD:
					if ((theLocalPlayerTeam == theTeamNumber) || (gHUD.GetPlayMode() == PLAYMODE_OBSERVER))
                    {
						if ( isNsMode ) {
							if ( theExtraPlayerInfo->teamnumber == TEAM_ONE || theExtraPlayerInfo->teamnumber == TEAM_THREE )  {
								if ( theExtraPlayerInfo->extra & WEAPON_WELDER ) {
									pLabel->setFgColorAsImageColor(false);
									pLabel->setImage(m_pWeld);
									m_pWeld->setColor(BuildColor(0, 149, 221, gHUD.GetGammaSlope()));
								}
							}
						}
					}
					break;

				case COLUMN_MINE:
					if ((theLocalPlayerTeam == theTeamNumber) || (gHUD.GetPlayMode() == PLAYMODE_OBSERVER))
                    {
						if ( isNsMode ) {
							if ( theExtraPlayerInfo->teamnumber == TEAM_ONE || theExtraPlayerInfo->teamnumber == TEAM_THREE )  {
								if ( theExtraPlayerInfo->extra & WEAPON_MINE ) {
									pLabel->setFgColorAsImageColor(false);
									pLabel->setImage(m_pMine);
									m_pMine->setColor(BuildColor(0, 149, 221, gHUD.GetGammaSlope()));
								}
							}
						}
					}
					break;

				case COLUMN_EXTRA:
					if ((theLocalPlayerTeam == theTeamNumber) || (gHUD.GetPlayMode() == PLAYMODE_OBSERVER))
                    {
						if ( isNsMode ) {
							if ( theExtraPlayerInfo->teamnumber == TEAM_ONE || theExtraPlayerInfo->teamnumber == TEAM_THREE )  {
								int r=0,	g=149,	b=221;
								if ( theExtraPlayerInfo->extra & WEAPON_HMG ) {
									pLabel->setFgColorAsImageColor(false);
									pLabel->setImage(m_pHMG);
									m_pHMG->setColor(BuildColor(r, g, b, gHUD.GetGammaSlope()));
								}
								if ( theExtraPlayerInfo->extra & WEAPON_SG ) {
									pLabel->setFgColorAsImageColor(false);
									pLabel->setImage(m_pSG);
									m_pSG->setColor(BuildColor(r, g, b, gHUD.GetGammaSlope()));
								}
								if ( theExtraPlayerInfo->extra & WEAPON_GL ) {
									pLabel->setFgColorAsImageColor(false);
									pLabel->setImage(m_pGL);
									m_pGL->setColor(BuildColor(r, g, b, gHUD.GetGammaSlope()));
								}
							}
							else if ( theExtraPlayerInfo->teamnumber == TEAM_TWO || theExtraPlayerInfo->teamnumber == TEAM_FOUR ) {
								sprintf(sz, "%d", theExtraPlayerInfo->extra);
							}
						}
						else if ( theExtraPlayerInfo->teamnumber == TEAM_ONE || theExtraPlayerInfo->teamnumber == TEAM_TWO ||
								  theExtraPlayerInfo->teamnumber == TEAM_THREE || theExtraPlayerInfo->teamnumber == TEAM_FOUR ) {
							sprintf(sz, "%d", theExtraPlayerInfo->extra);
						}
					}
                    break;
				case COLUMN_KILLS:
                    sprintf(sz, "%d", theExtraPlayerInfo->frags);
                    break;

				case COLUMN_DEATHS:
					sprintf(sz, "%d", theExtraPlayerInfo->deaths);
					break;
				case COLUMN_LATENCY:
					if(pl_info)
					{
						sprintf(sz, "%d", pl_info->ping );
					}
					break;
				default:
					break;
				}
			}

            pLabel->setText(sz);
		}
	}

	for(row=0; row < NUM_ROWS; row++)
	{
		CGrid *pGridRow = &m_PlayerGrids[row];

		pGridRow->AutoSetRowHeights();
		pGridRow->setSize(PanelWidth(pGridRow), pGridRow->CalcDrawHeight());
		pGridRow->RepositionContents();
	}

	// hack, for the thing to resize
	m_PlayerList.getSize(x, y);
	m_PlayerList.setSize(x, y);
}


//-----------------------------------------------------------------------------
// Purpose: Setup highlights for player names in scoreboard
//-----------------------------------------------------------------------------
void ScorePanel::DeathMsg( int killer, int victim )
{
	// if we were the one killed,  or the world killed us, set the scoreboard to indicate suicide
	if ( victim == m_iPlayerNum || killer == 0 )
	{
		m_iLastKilledBy = killer ? killer : m_iPlayerNum;
		m_fLastKillTime = gHUD.m_flTime + 10;	// display who we were killed by for 10 seconds

		if ( killer == m_iPlayerNum )
			m_iLastKilledBy = m_iPlayerNum;
	}
}


void ScorePanel::Open( void )
{
	RebuildTeams();
	setVisible(true);
	m_HitTestPanel.setVisible(true);
}

bool ScorePanel::SetSquelchMode(bool inMode)
{
	bool theSuccess = false;

	if(inMode && !GetClientVoiceMgr()->IsInSquelchMode())
	{
		GetClientVoiceMgr()->StartSquelchMode();
		m_HitTestPanel.setVisible(false);
		theSuccess = true;
	}
	else if(!inMode && GetClientVoiceMgr()->IsInSquelchMode())
	{
		GetClientVoiceMgr()->StopSquelchMode();
		theSuccess = true;
	}

	return theSuccess;
}

void ScorePanel::mousePressed(MouseCode code, Panel* panel)
{
	if(gHUD.m_iIntermission)
		return;

	if (!GetClientVoiceMgr()->IsInSquelchMode())
	{
		//GetClientVoiceMgr()->StartSquelchMode();
		//m_HitTestPanel.setVisible(false);
		this->SetSquelchMode(true);
	}
	else if (m_iHighlightRow >= 0)
	{
		// mouse has been pressed, toggle mute state
		int iPlayer = m_iSortedRows[m_iHighlightRow];
		if (iPlayer > 0)
		{
			// print text message
			hud_player_info_t *pl_info = &g_PlayerInfoList[iPlayer];

			if (pl_info && pl_info->name && pl_info->name[0])
			{
				char string[256];
				if (GetClientVoiceMgr()->IsPlayerBlocked(iPlayer))
				{
					char string1[1024];

					// remove mute
					GetClientVoiceMgr()->SetPlayerBlockedState(iPlayer, false);

					sprintf( string1, CHudTextMessage::BufferedLocaliseTextString( "#Unmuted" ), pl_info->name );
					sprintf( string, "%c** %s\n", HUD_PRINTTALK, string1 );

					gHUD.m_TextMessage.MsgFunc_TextMsg(NULL, (int)strlen(string)+1, string );
				}
				else
				{
					char string1[1024];
					char string2[1024];

					// mute the player
					GetClientVoiceMgr()->SetPlayerBlockedState(iPlayer, true);

					sprintf( string1, CHudTextMessage::BufferedLocaliseTextString( "#Muted" ), pl_info->name );
					sprintf( string2, CHudTextMessage::BufferedLocaliseTextString( "#No_longer_hear_that_player" ) );
					sprintf( string, "%c** %s %s\n", HUD_PRINTTALK, string1, string2 );

					gHUD.m_TextMessage.MsgFunc_TextMsg(NULL, (int)strlen(string)+1, string );
				}
			}
		}
	}
}

void ScorePanel::cursorMoved(int x, int y, Panel *panel)
{
	if (GetClientVoiceMgr()->IsInSquelchMode())
	{
		// look for which cell the mouse is currently over
		for (int i = 0; i < NUM_ROWS; i++)
		{
			int row, col;
			if (m_PlayerGrids[i].getCellAtPoint(x, y, row, col))
			{
				MouseOverCell(i, col);
				return;
			}
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: Handles mouse movement over a cell
// Input  : row - 
//			col - 
//-----------------------------------------------------------------------------
void ScorePanel::MouseOverCell(int row, int col)
{
	CLabelHeader *label = &m_PlayerEntries[col][row];

	// clear the previously highlighted label
	if (m_pCurrentHighlightLabel != label)
	{
		m_pCurrentHighlightLabel = NULL;
		m_iHighlightRow = -1;
	}
	if (!label)
		return;

	// don't act on teams
	if (m_iIsATeam[row] != TEAM_IND)
		return;

	// don't act on disconnected players or ourselves
	hud_player_info_t *pl_info = &g_PlayerInfoList[ m_iSortedRows[row] ];
	if (!pl_info->name || !pl_info->name[0])
		return;

	if (pl_info->thisplayer && !gEngfuncs.IsSpectateOnly() )
		return;

	// only act on audible players
	//if (!GetClientVoiceMgr()->IsPlayerAudible(m_iSortedRows[row]))
	//	return;

	// setup the new highlight
	m_pCurrentHighlightLabel = label;
	m_iHighlightRow = row;
}

//-----------------------------------------------------------------------------
// Purpose: Label paint functions - take into account current highligh status
//-----------------------------------------------------------------------------
void CLabelHeader::paintBackground()
{
	Color oldBg;
	getBgColor(oldBg);

	if (gViewPort->GetScoreBoard()->m_iHighlightRow == _row)
	{
		setBgColor(134, 91, 19, 0);
	}

	Panel::paintBackground();

	setBgColor(oldBg);
}
		

//-----------------------------------------------------------------------------
// Purpose: Label paint functions - take into account current highligh status
//-----------------------------------------------------------------------------
void CLabelHeader::paint()
{
	Color oldFg;
	getFgColor(oldFg);

	if (gViewPort->GetScoreBoard()->m_iHighlightRow == _row)
	{
		setFgColor(255, 255, 255, 0);
	}

	// draw text
	int x, y, iwide, itall;
	getTextSize(iwide, itall);
	calcAlignment(iwide, itall, x, y);
	_dualImage->setPos(x, y);

	int x1, y1;
	_dualImage->GetImage(1)->getPos(x1, y1);
	_dualImage->GetImage(1)->setPos(_gap, y1);

	_dualImage->doPaint(this);

	// get size of the panel and the image
	if (_image)
	{
		Color imgColor;
		getFgColor( imgColor );
		if( _useFgColorAsImageColor )
		{
			_image->setColor( imgColor );
		}
		_image->getSize(iwide, itall);
		calcAlignment(iwide, itall, x, y);
		_image->setPos(x, y);
		_image->doPaint(this);
	}

	setFgColor(oldFg[0], oldFg[1], oldFg[2], oldFg[3]);
}


void CLabelHeader::calcAlignment(int iwide, int itall, int &x, int &y)
{
	// calculate alignment ourselves, since vgui is so broken
	int wide, tall;
	getSize(wide, tall);

	x = 0, y = 0;

	// align left/right
	switch (_contentAlignment)
	{
		// left
		case Label::a_northwest:
		case Label::a_west:
		case Label::a_southwest:
		{
			x = 0;
			break;
		}
		
		// center
		case Label::a_north:
		case Label::a_center:
		case Label::a_south:
		{
			x = (wide - iwide) / 2;
			break;
		}
		
		// right
		case Label::a_northeast:
		case Label::a_east:
		case Label::a_southeast:
		{
			x = wide - iwide;
			break;
		}
	}

	// top/down
	switch (_contentAlignment)
	{
		// top
		case Label::a_northwest:
		case Label::a_north:
		case Label::a_northeast:
		{
			y = 0;
			break;
		}
		
		// center
		case Label::a_west:
		case Label::a_center:
		case Label::a_east:
		{
			y = (tall - itall) / 2;
			break;
		}
		
		// south
		case Label::a_southwest:
		case Label::a_south:
		case Label::a_southeast:
		{
			y = tall - itall;
			break;
		}
	}

// don't clip to Y
//	if (y < 0)
//	{
//		y = 0;
//	}
	if (x < 0)
	{
		x = 0;
	}

	x += _offset[0];
	y += _offset[1];
}
