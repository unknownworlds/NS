
#ifndef SCOREPANEL_H
#define SCOREPANEL_H

#include<VGUI_Panel.h>
#include<VGUI_TablePanel.h>
#include<VGUI_HeaderPanel.h>
#include<VGUI_TextGrid.h>
#include<VGUI_Label.h>
#include<VGUI_TextImage.h>
#include "..\game_shared\vgui_listbox.h"

#include <ctype.h>

#define MAX_SCORES					10
#define MAX_SCOREBOARD_TEAMS		5

// Scoreboard cells
#define COLUMN_TRACKER		0
#define COLUMN_RANK_ICON	1
#define COLUMN_NAME			2
#define COLUMN_CLASS		3
#define COLUMN_SCORE        4
#define COLUMN_KILLS        5
#define COLUMN_DEATHS		6
#define COLUMN_LATENCY		7
#define COLUMN_VOICE		8
#define COLUMN_BLANK		9
#define NUM_COLUMNS			10
#define NUM_ROWS			(MAX_PLAYERS + (MAX_SCOREBOARD_TEAMS * 2))

using namespace vgui;

#include "cl_dll/CLabelHeader.h"

class ScoreTablePanel;

#include "..\game_shared\vgui_grid.h"
#include "..\game_shared\vgui_defaultinputsignal.h"

//-----------------------------------------------------------------------------
// Purpose: Scoreboard back panel
//-----------------------------------------------------------------------------
class ScorePanel : public Panel, public vgui::CDefaultInputSignal
{
private:
	// Default panel implementation doesn't forward mouse messages when there is no cursor and we need them.
	class HitTestPanel : public Panel
	{
	public:
		virtual void	internalMousePressed(MouseCode code);
	};


private:

	Label			m_TitleLabel;
	
	// Here is how these controls are arranged hierarchically.
	// m_HeaderGrid
	//     m_HeaderLabels

	// m_PlayerGridScroll
	//     m_PlayerGrid
	//         m_PlayerEntries 

	CGrid			m_HeaderGrid;
	CLabelHeader	m_HeaderLabels[NUM_COLUMNS];			// Labels above the 
	CLabelHeader	*m_pCurrentHighlightLabel;
	int				m_iHighlightRow;
	
	vgui::CListBox	m_PlayerList;
	CGrid			m_PlayerGrids[NUM_ROWS];				// The grid with player and team info. 
	CLabelHeader	m_PlayerEntries[NUM_COLUMNS][NUM_ROWS];	// Labels for the grid entries.

	ScorePanel::HitTestPanel	m_HitTestPanel;
	CommandButton				*m_pCloseButton;
	CLabelHeader*	GetPlayerEntry(int x, int y)	{return &m_PlayerEntries[x][y];}

	vgui::BitmapTGA *m_pTrackerIcon;

	vgui::BitmapTGA *m_pDevIcon;
	vgui::BitmapTGA *m_pPTIcon;
	vgui::BitmapTGA *m_pGuideIcon;
	vgui::BitmapTGA *m_pServerOpIcon;
	vgui::BitmapTGA *m_pContribIcon;
	vgui::BitmapTGA *m_pCheatingDeathIcon;
	vgui::BitmapTGA *m_pVeteranIcon;
	vector< pair <vgui::BitmapTGA *, string> > m_CustomIconList;

public:
	
	int				m_iNumTeams;
	int				m_iPlayerNum;
	int				m_iShowscoresHeld;

	int				m_iRows;
	int				m_iSortedRows[NUM_ROWS];
	int				m_iIsATeam[NUM_ROWS];
	bool			m_bHasBeenSorted[MAX_PLAYERS];
	int				m_iLastKilledBy;
	int				m_fLastKillTime;
public:

	ScorePanel(int x,int y,int wide,int tall);

	void Update( void );

	void SortTeams( void );
	void SortActivePlayers(char* inTeam, bool inSortByEntityIndex = false);
	void SortPlayers( int iTeam, char *team, bool inSortByEntityIndex = false);
	void RebuildTeams( void );
	bool SetSquelchMode(bool inMode);

	void FillGrid();

	void DeathMsg( int killer, int victim );

	void Initialize( void );

	void Open( void );

	void MouseOverCell(int row, int col);

// InputSignal overrides.
public:

	virtual void mousePressed(MouseCode code, Panel* panel);
	virtual void cursorMoved(int x, int y, Panel *panel);

	vgui::BitmapTGA *GetIconPointer(string inIconName)
	{
		vgui::BitmapTGA *pIcon = NULL;

		for (int i = 0; i < m_CustomIconList.size(); i++) 
		{
			if(inIconName == m_CustomIconList[i].second)
			{
				if(m_CustomIconList[i].first)
				{
					pIcon = m_CustomIconList[i].first;
					break;
				}
			}
		}
		return pIcon;
	}

	void DeleteCustomIcons( void )
	{
		for (int i = 0; i < m_CustomIconList.size(); i++) 
		{
			if(m_CustomIconList[i].first)
				delete m_CustomIconList[i].first;

			m_CustomIconList[i].first = NULL;
			m_CustomIconList[i].second = "";
		}

		m_CustomIconList.clear();
	};

	friend CLabelHeader;
};

#endif
