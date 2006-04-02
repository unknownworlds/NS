//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

// vgui_SpectatorPanel.cpp: implementation of the SpectatorPanel class.
//
//////////////////////////////////////////////////////////////////////

#include "hud.h"
#include "cl_util.h"
#include "common/const.h"
#include "common/entity_state.h"
#include "common/cl_entity.h"
#include "pm_shared/pm_shared.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_SpectatorPanel.h"
#include "vgui_scorepanel.h"
#include "mod/AvHOverviewControl.h"


#define BANNER_WIDTH	256
#define BANNER_HEIGHT	64


#define OPTIONS_BUTTON_X 96
#define CAMOPTIONS_BUTTON_X 200


class Spectator_CheckButtonHandler : public ICheckButton2Handler
{

public:

	Spectator_CheckButtonHandler(SpectatorPanel * panel)
	{
		m_pFather = panel;
	}

	virtual void StateChanged(CCheckButton2* pButton)
    {
        m_pFather->StateChanged(pButton);
    }

private:
    
    SpectatorPanel * m_pFather;

};

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

SpectatorPanel::SpectatorPanel(int x,int y,int wide,int tall) : Panel(x,y,wide,tall)
{
    m_overviewButton     = NULL;
    m_autoDirectorButton = NULL;
    m_firstPersonButton  = NULL;
    m_overviewControl    = NULL;
}

SpectatorPanel::~SpectatorPanel()
{

}

void SpectatorPanel::StateChanged(CCheckButton2* pButton)
{
    bool theOverviewMode = false;
    if(m_overviewButton)
    {
        theOverviewMode = m_overviewButton->IsChecked();
    }
    gHUD.m_Spectator.SetOverviewMode(theOverviewMode);

    int theMode = OBS_NONE;

    if (m_firstPersonButton->IsChecked())
    {
        theMode = OBS_IN_EYE;
    }
    else
    {
        theMode = OBS_CHASE_LOCKED;
    }
    
    gHUD.m_Spectator.SetMode(theMode);

    if (m_autoDirectorButton != NULL)
    {
        if (m_autoDirectorButton->IsChecked())
        {
            gEngfuncs.Cvar_SetValue("spec_autodirector", 1);
        }
        else
        {
            gEngfuncs.Cvar_SetValue("spec_autodirector", 0);
        }
    }

    Update(); // Update so that the components reflect the new state.

}

void SpectatorPanel::ActionSignal(int cmd)
{

	switch (cmd)
	{
		/*
        case SPECTATOR_PANEL_CMD_NONE :			break;
        */

		case SPECTATOR_PANEL_CMD_PLAYERS :		
            gViewPort->UpdatePlayerMenu();
            gViewPort->ShowCommandMenu(gViewPort->m_SpectatorPlayerMenu);
            break;

		case SPECTATOR_PANEL_CMD_NEXTPLAYER :	gHUD.m_Spectator.FindNextPlayer(true);
												break;

		case SPECTATOR_PANEL_CMD_PREVPLAYER :	gHUD.m_Spectator.FindNextPlayer(false);
												break;

    /*
		case SPECTATOR_PANEL_CMD_HIDEMENU	:	ShowMenu(false); 
												break;

		case SPECTATOR_PANEL_CMD_CAMERA :		gViewPort->ShowCommandMenu( gViewPort->m_SpectatorCameraMenu );
												break;
		
        */
		default : 	gEngfuncs.Con_DPrintf("Unknown SpectatorPanel ActionSingal %i.\n",cmd); break;
	}

}


void SpectatorPanel::Initialize()
{
	int x,y,wide,tall;
	
	getBounds(x,y,wide,tall);

	CSchemeManager * pSchemes = gViewPort->GetSchemeManager();


    int colorR = 128 / gHUD.GetGammaSlope();
    int colorG = 128 / gHUD.GetGammaSlope();
    int colorB = 128 / gHUD.GetGammaSlope();

    int armedColorR = 255 / gHUD.GetGammaSlope();
    int armedColorG = 255 / gHUD.GetGammaSlope();
    int armedColorB = 255 / gHUD.GetGammaSlope();


	SchemeHandle_t hSmallScheme = pSchemes->getSchemeHandle( /*"Team Info Text"*/ "PieMenuScheme" );
	Font* font = pSchemes->getFont(hSmallScheme);

	//m_TopBorder = new CTransparentPanel(64, 0, 0, ScreenWidth, YRES(PANEL_HEIGHT));
	m_TopBorder = new CTransparentPanel(1, 0, ScreenHeight() - YRES(32), ScreenWidth(), YRES(SPECTATOR_PANEL_HEIGHT));
    m_TopBorder->setParent(this);

	m_BottomBorder = new CTransparentPanel(1, 0, ScreenHeight() - YRES(32), ScreenWidth(), YRES(SPECTATOR_PANEL_HEIGHT));
	m_BottomBorder->setParent(this);

	setPaintBackgroundEnabled(false);

	m_ExtraInfo = new Label( "Extra Info", 0, 0, wide, YRES(SPECTATOR_PANEL_HEIGHT) );
	m_ExtraInfo->setParent(m_TopBorder);
	m_ExtraInfo->setFont( font );

	m_ExtraInfo->setPaintBackgroundEnabled(false);
	m_ExtraInfo->setFgColor( 143, 143, 54, 0 );
	m_ExtraInfo->setContentAlignment( vgui::Label::a_west );

	

	m_TimerImage = new CImageLabel( "timer", 0, 0, 14, 14 );
	m_TimerImage->setParent(m_TopBorder);

	m_TopBanner = new CImageLabel( "banner", 0, 0, XRES(BANNER_WIDTH), YRES(BANNER_HEIGHT) );
	m_TopBanner->setParent(this);

	m_CurrentTime = new Label( "00:00", 0, 0, wide, YRES(SPECTATOR_PANEL_HEIGHT) );
	m_CurrentTime->setParent(m_TopBorder);
	m_CurrentTime->setFont( pSchemes->getFont(hSmallScheme) );
	m_CurrentTime->setPaintBackgroundEnabled(false);
	m_CurrentTime->setFgColor( 143, 143, 54, 0 );
	m_CurrentTime->setContentAlignment( vgui::Label::a_west );

	m_Separator = new Panel( 0, 0, XRES( 64 ), YRES( 96 ));
	m_Separator->setParent( m_TopBorder );
	m_Separator->setFgColor( 59, 58, 34, 48 );
	m_Separator->setBgColor( 59, 58, 34, 48 );
	
	for ( int j= 0; j < TEAM_NUMBER; j++ )
	{
		m_TeamScores[j] = new Label( "   ", 0, 0, wide, YRES(SPECTATOR_PANEL_HEIGHT) );
		m_TeamScores[j]->setParent( m_TopBorder );
		m_TeamScores[j]->setFont( pSchemes->getFont(hSmallScheme) );
		m_TeamScores[j]->setPaintBackgroundEnabled(false);
		m_TeamScores[j]->setFgColor( 143, 143, 54, 0 );
		m_TeamScores[j]->setContentAlignment( vgui::Label::a_west );
		m_TeamScores[j]->setVisible ( false );
	}
	
    
	m_PrevPlayerButton= new ColorButton("<", XRES(390 - 20 - 4), YRES(6), XRES(20), YRES(20), false, false );
	m_PrevPlayerButton->setParent( m_BottomBorder );
	m_PrevPlayerButton->setContentAlignment( vgui::Label::a_center );
	m_PrevPlayerButton->setBoundKey( (char)255 );	// special no bound to avoid leading spaces in name 
	m_PrevPlayerButton->addActionSignal( new CSpectatorHandler_Command(this,SPECTATOR_PANEL_CMD_PREVPLAYER) );
	m_PrevPlayerButton->setUnArmedBorderColor ( colorR, colorG, colorB, 0 );
	m_PrevPlayerButton->setArmedBorderColor ( armedColorR, armedColorR, armedColorR, 0);
	m_PrevPlayerButton->setUnArmedColor ( colorR, colorG, colorB, 0 );
	m_PrevPlayerButton->setArmedColor ( armedColorR, armedColorR, armedColorR, 0);

	m_NextPlayerButton= new ColorButton(">", XRES(390 + 200 + 4), YRES(6), XRES(20), YRES(20), false, false );
	m_NextPlayerButton->setParent( m_BottomBorder );
	m_NextPlayerButton->setContentAlignment( vgui::Label::a_center );
	m_NextPlayerButton->setBoundKey( (char)255 );	// special no bound to avoid leading spaces in name 
	m_NextPlayerButton->addActionSignal( new CSpectatorHandler_Command(this,SPECTATOR_PANEL_CMD_NEXTPLAYER) );
	m_NextPlayerButton->setUnArmedBorderColor ( colorR, colorG, colorB, 0 );
	m_NextPlayerButton->setArmedBorderColor ( armedColorR, armedColorR, armedColorR, 0);
	m_NextPlayerButton->setUnArmedColor ( colorR, colorG, colorB, 0);
	m_NextPlayerButton->setArmedColor ( armedColorR, armedColorR, armedColorR, 0);
	
	// Initialize the bottom title.

    m_BottomMainLabel = new ColorButton( "Spectator Bottom", XRES(390), YRES(6), XRES(200), YRES(20), false, false  );
	m_BottomMainLabel->setFont(font);
    m_BottomMainLabel->setParent(m_BottomBorder);
    m_BottomMainLabel->setContentAlignment( vgui::Label::a_center );
    m_BottomMainLabel->addActionSignal( new CSpectatorHandler_Command(this,SPECTATOR_PANEL_CMD_PLAYERS) );

	m_BottomMainLabel->setUnArmedBorderColor ( colorR, colorG, colorB, 0);
	m_BottomMainLabel->setArmedBorderColor ( armedColorR, armedColorR, armedColorR, 0);
	m_BottomMainLabel->setUnArmedColor ( colorR, colorG, colorB, 0);
    m_BottomMainLabel->setArmedColor ( armedColorR, armedColorR, armedColorR, 0);



    m_menuVisible = true;

	m_insetVisible = false;

    m_ExtraInfo->setVisible( false );
	m_Separator->setVisible( false );
	m_TimerImage->setVisible( false );

    m_TopBorder->setVisible(true);
    m_BottomBorder->setVisible( true );

    m_overviewButton = new CCheckButton2();
    m_overviewButton->setFont(font);
    m_overviewButton->setParent( m_BottomBorder );
    m_overviewButton->SetText("Overview");
    m_overviewButton->setPos(XRES(10), YRES(6));
    m_overviewButton->setSize(XRES(100), YRES(20));
    m_overviewButton->SetImages("gfx/vgui/640_checkset.tga", "gfx/vgui/640_checkunset.tga");
    m_overviewButton->SetHandler(new Spectator_CheckButtonHandler(this));
    m_overviewButton->SetTextColor(colorR, colorG, colorB, 0);

	m_firstPersonButton = new CCheckButton2();
    m_firstPersonButton->setFont(font);
	m_firstPersonButton->setParent( m_BottomBorder );
    m_firstPersonButton->SetText("First person");
    m_firstPersonButton->setPos(XRES(10 + 100), YRES(6));
    m_firstPersonButton->setSize(XRES(100), YRES(20));
    m_firstPersonButton->SetImages("gfx/vgui/640_checkset.tga", "gfx/vgui/640_checkunset.tga");
    m_firstPersonButton->SetHandler(new Spectator_CheckButtonHandler(this));
    m_firstPersonButton->SetTextColor(colorR, colorG, colorB, 0);

    m_autoDirectorButton = new CCheckButton2();
    m_autoDirectorButton->setFont(font);
    m_autoDirectorButton->setParent( m_BottomBorder );
    m_autoDirectorButton->SetText("Auto-director");
    m_autoDirectorButton->setPos(XRES(10 + 200), YRES(6));
    m_autoDirectorButton->setSize(XRES(100), YRES(20));
    m_autoDirectorButton->SetImages("gfx/vgui/640_checkset.tga", "gfx/vgui/640_checkunset.tga");
    m_autoDirectorButton->SetHandler(new Spectator_CheckButtonHandler(this));
    m_autoDirectorButton->SetTextColor(colorR, colorG, colorB, 0);


/*
    m_OverviewData.insetWindowX = 4;	// upper left corner
	m_OverviewData.insetWindowY = 4 + SPECTATOR_PANEL_HEIGHT;
	m_OverviewData.insetWindowHeight = 180;
	m_OverviewData.insetWindowWidth = 240;


    theDrawInfo.mX = XRES(m_OverviewData.insetWindowX + m_OverviewData.insetWindowWidth + 4);
    theDrawInfo.mY = YRES(SPECTATOR_PANEL_HEIGHT + 4);
    theDrawInfo.mWidth  = ScreenWidth() - theDrawInfo.mX - XRES(4);
    theDrawInfo.mHeight = ScreenHeight() - YRES(SPECTATOR_PANEL_HEIGHT + 4) - theDrawInfo.mY;
*/

    int theX      = XRES(4 + 240 + 4);
    int theY      = YRES(SPECTATOR_PANEL_HEIGHT + 4);
    int theWidth  = ScreenWidth() - theX - XRES(4);
    int theHeight = ScreenHeight() - YRES(SPECTATOR_PANEL_HEIGHT + 4) - theY;

    m_overviewControl = new AvHOverviewControl;
    m_overviewControl->setPos(theX, theY);
    m_overviewControl->setSize(theWidth, theHeight);
    m_overviewControl->setParent(this);

}

void SpectatorPanel::ShowMenu(bool isVisible)
{

    if ( !isVisible )
	{
		gViewPort->HideCommandMenu();
	}

	m_menuVisible = isVisible;

	gViewPort->UpdateCursorState();

}


void SpectatorPanel::EnableInsetView(bool isEnabled)
{


	int x = gHUD.m_Spectator.m_OverviewData.insetWindowX;
	int y = gHUD.m_Spectator.m_OverviewData.insetWindowY;
	int wide = gHUD.m_Spectator.m_OverviewData.insetWindowWidth;
	int tall = gHUD.m_Spectator.m_OverviewData.insetWindowHeight;
	int offset = x + wide + 2;
	
	if ( isEnabled )
	{
		// short black bar to see full inset
		m_TopBorder->setBounds(	XRES(offset), 0, XRES(640 - offset ), YRES(SPECTATOR_PANEL_HEIGHT) );

		if ( gEngfuncs.IsSpectateOnly() )
		{
			m_TopBanner->setVisible( true );
			m_TopBanner->setPos( XRES(offset), 0 );
		}
		else
			m_TopBanner->setVisible( false );
		
	}
	else
	{	
		// full black bar, no inset border
		// show banner only in real HLTV mode
		if ( gEngfuncs.IsSpectateOnly() )
		{
			m_TopBanner->setVisible( true );
			m_TopBanner->setPos( 0,0 );
		}
		else
			m_TopBanner->setVisible( false );

		m_TopBorder->setBounds( 0, 0, ScreenWidth(), YRES(SPECTATOR_PANEL_HEIGHT) );

	}

	m_insetVisible = isEnabled;

	Update();

}


void SpectatorPanel::Update()
{
    
    // Update the check boxes.
    m_overviewButton->SetChecked(gHUD.m_Spectator.IsInOverviewMode());
    m_overviewButton->setVisible(gHUD.GetIsNSMode());
    
    m_firstPersonButton->SetChecked(g_iUser1 == OBS_IN_EYE);

    m_autoDirectorButton->SetChecked(CVAR_GET_FLOAT("spec_autodirector") != 0);
    m_autoDirectorButton->setVisible(gEngfuncs.IsSpectateOnly());

    m_overviewControl->setVisible(gHUD.m_Spectator.IsInOverviewMode());

	int iTextWidth, iTextHeight;
	int iTimeHeight, iTimeWidth;
	int offset,j;

    // TODO Max: Figure out what this is for.

	if ( m_insetVisible )
		offset = gHUD.m_Spectator.m_OverviewData.insetWindowX + gHUD.m_Spectator.m_OverviewData.insetWindowWidth + 2;
	else
		offset = 0;

	//bool visible = gHUD.m_Spectator.m_drawstatus->value != 0;
    bool visible = true;
	
	m_ExtraInfo->setVisible( visible );
	m_TimerImage->setVisible( visible );
	m_CurrentTime->setVisible( visible );
	m_Separator->setVisible( visible );

	for ( j= 0; j < TEAM_NUMBER; j++ )
		m_TeamScores[j]->setVisible( visible );

	if ( !visible )
		return;
		
	m_ExtraInfo->getTextSize( iTextWidth, iTextHeight );
	m_CurrentTime->getTextSize( iTimeWidth, iTimeHeight );

	iTimeWidth += XRES ( 14 ); // +timer icon
	iTimeWidth += ( 4-(iTimeWidth%4) );

	if ( iTimeWidth > iTextWidth )
		iTextWidth = iTimeWidth;

	int xPos = ScreenWidth() - ( iTextWidth + XRES ( 4 + offset ) );

	m_ExtraInfo->setBounds( xPos, YRES( 1 ), iTextWidth, iTextHeight );

	m_TimerImage->setBounds( xPos, YRES( 2 ) + iTextHeight , XRES(14), YRES(14) );
	
	m_CurrentTime->setBounds( xPos + XRES ( 14 + 1 ), YRES( 2 ) + iTextHeight , iTimeWidth, iTimeHeight );

	m_Separator->setPos( ScreenWidth() - ( iTextWidth + XRES ( 4+2+4+offset ) ) , YRES( 1 ) );
	m_Separator->setSize( XRES( 4 ), YRES( SPECTATOR_PANEL_HEIGHT - 2 ) );

	for ( j= 0; j < TEAM_NUMBER; j++ )
	{
		int iwidth, iheight;
			
		m_TeamScores[j]->getTextSize( iwidth, iheight );
		m_TeamScores[j]->setBounds( ScreenWidth() - ( iTextWidth + XRES ( 4+2+4+2+offset ) + iwidth ), YRES( 1 ) + ( iheight * j ), iwidth, iheight );
	}

}
