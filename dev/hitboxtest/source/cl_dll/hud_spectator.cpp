//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#include "hud.h"
#include "cl_util.h"
#include "cl_entity.h"
#include "triangleapi.h"
#include "vgui_TeamFortressViewport.h"
#include "vgui_SpectatorPanel.h"
#include "hltv.h"

#include "pm_shared.h"
#include "pm_defs.h"
#include "pmtrace.h"
#include "parsemsg.h"
#include "entity_types.h"

// these are included for the math functions
#include "com_model.h"
#include "demo_api.h"
#include "event_api.h"
#include "studio_util.h"
#include "screenfade.h"
#include "util/STLUtil.h"
#include "mod/AvHTitles.h"
#include "mod/AvHSprites.h"

#pragma warning(disable: 4244)

extern int		iJumpSpectator;
extern float	vJumpOrigin[3];
extern float	vJumpAngles[3]; 

extern void V_GetInEyePos(int entity, float * origin, float * angles );
extern void V_ResetChaseCam();
extern void V_GetChasePos(int target, float * cl_angles, float * origin, float * angles);
extern void VectorAngles( const float *forward, float *angles );
extern "C" void NormalizeAngles( float *angles );
extern float * GetClientColor( int clientIndex );

extern vec3_t v_origin;		// last view origin
extern vec3_t v_angles;		// last view angle
extern vec3_t v_cl_angles;	// last client/mouse angle
extern vec3_t v_sim_org;	// last sim origin

void SpectatorMode(void)
{

	if ( gEngfuncs.Cmd_Argc() <= 1 )
	{
		gEngfuncs.Con_Printf( "usage:  spec_mode <Mode>\n" );
		return;
	}

	// SetModes() will decide if command is executed on server or local
	if ( gEngfuncs.Cmd_Argc() == 2 )
		gHUD.m_Spectator.SetMode( atoi( gEngfuncs.Cmd_Argv(1) ));
	
	//else if ( gEngfuncs.Cmd_Argc() == 3 )
	//	gHUD.m_Spectator.SetMode( atoi( gEngfuncs.Cmd_Argv(1) ), atoi( gEngfuncs.Cmd_Argv(2) )  );	
}

void SpectatorSpray(void)
{
	vec3_t forward;
	char string[128];

	if ( !gEngfuncs.IsSpectateOnly() )
		return;

	AngleVectors(v_angles,forward,NULL,NULL);
	VectorScale(forward, 128, forward);
	VectorAdd(forward, v_origin, forward);
	pmtrace_t * trace = gEngfuncs.PM_TraceLine( v_origin, forward, PM_TRACELINE_PHYSENTSONLY, 2, -1 );
	if ( trace->fraction != 1.0 )
	{
		sprintf(string, "drc_spray %.2f %.2f %.2f %i", 
			trace->endpos[0], trace->endpos[1], trace->endpos[2], trace->ent );
		gEngfuncs.pfnServerCmd(string);
	}

}
void SpectatorHelp(void)
{
	if ( gViewPort )
	{
		gViewPort->ShowVGUIMenu( MENU_SPECHELP );
	}
	else
	{
  		char *text = CHudTextMessage::BufferedLocaliseTextString( "#Spec_Help_Text" );
			
		if ( text )
		{
			while ( *text )
			{
				if ( *text != 13 )
					gEngfuncs.Con_Printf( "%c", *text );
				text++;
			}
		}
	}
}

void SpectatorMenu( void )
{
	if ( gEngfuncs.Cmd_Argc() <= 1 )
	{
		gEngfuncs.Con_Printf( "usage:  spec_menu <0|1>\n" );
		return;
	}
	
	gViewPort->m_pSpectatorPanel->ShowMenu( atoi( gEngfuncs.Cmd_Argv(1))!=0  );
}

void ToggleScores( void )
{
	if ( gViewPort )
	{
		if (gViewPort->IsScoreBoardVisible() )
		{
			gViewPort->HideScoreBoard();
		}
		else
		{
			gViewPort->ShowScoreBoard();
		}
	}
}

//-----------------------------------------------------------------------------
// Purpose: 
//-----------------------------------------------------------------------------
int CHudSpectator::Init()
{
	gHUD.AddHudElem(this);

	m_iFlags |= HUD_ACTIVE;
	m_flNextObserverInput = 0.0f;
	m_zoomDelta	= 0.0f;
	m_moveDelta = 0.0f;
	iJumpSpectator	= 0;

	memset( &m_OverviewData, 0, sizeof(m_OverviewData));
	memset( &m_OverviewEntities, 0, sizeof(m_OverviewEntities));
	m_lastPrimaryObject = m_lastSecondaryObject = 0;

	gEngfuncs.pfnAddCommand ("spec_mode", SpectatorMode );
	gEngfuncs.pfnAddCommand ("spec_decal", SpectatorSpray );
	gEngfuncs.pfnAddCommand ("spec_help", SpectatorHelp );
	gEngfuncs.pfnAddCommand ("spec_menu", SpectatorMenu );
	gEngfuncs.pfnAddCommand ("togglescores", ToggleScores );

	m_drawnames		= gEngfuncs.pfnRegisterVariable("spec_drawnames","1",0);
	m_drawcone		= gEngfuncs.pfnRegisterVariable("spec_drawcone","1",0);
	m_drawstatus	= gEngfuncs.pfnRegisterVariable("spec_drawstatus","1",0);
	m_autoDirector	= gEngfuncs.pfnRegisterVariable("spec_autodirector","1",0);

    // Removed by mmcguire.
    m_overviewMode  = false;
    //m_overview	    = gEngfuncs.pfnRegisterVariable("spec_overview","1",0);
    //m_pip			= gEngfuncs.pfnRegisterVariable("spec_pip","1",0);
	
	if ( !m_drawnames || !m_drawcone || !m_drawstatus || !m_autoDirector /*|| !m_pip*/)
	{
		gEngfuncs.Con_Printf("ERROR! Couldn't register all spectator variables.\n");
		return 0;
	}

	return 1;
}


//-----------------------------------------------------------------------------
// UTIL_StringToVector originally from ..\dlls\util.cpp, slightly changed
//-----------------------------------------------------------------------------

void UTIL_StringToVector( float * pVector, const char *pString )
{
	char *pstr, *pfront, tempString[128];
	int	j;

	strcpy( tempString, pString );
	pstr = pfront = tempString;
	
	for ( j = 0; j < 3; j++ )		
	{
		pVector[j] = atof( pfront );
		
		while ( *pstr && *pstr != ' ' )
			pstr++;
		if (!*pstr)
			break;
		pstr++;
		pfront = pstr;
	}

	if (j < 2)
	{
		for (j = j+1;j < 3; j++)
			pVector[j] = 0;
	}
}

int UTIL_FindEntityInMap(char * name, float * origin, float * angle)
{
	int				n,found = 0;
	char			keyname[256];
	char			token[1024];

	cl_entity_t *	pEnt = gEngfuncs.GetEntityByIndex( 0 );	// get world model

	if ( !pEnt ) return 0;

	if ( !pEnt->model )	return 0;

	char * data = pEnt->model->entities;

	while (data)
	{
		data = gEngfuncs.COM_ParseFile(data, token);
		
		if ( (token[0] == '}') ||  (token[0]==0) )
			break;

		if (!data)
		{
			gEngfuncs.Con_DPrintf("UTIL_FindEntityInMap: EOF without closing brace\n");
			return 0;
		}

		if (token[0] != '{')
		{
			gEngfuncs.Con_DPrintf("UTIL_FindEntityInMap: expected {\n");
			return 0;
		}

		// we parse the first { now parse entities properties
		
		while ( 1 )
		{	
			// parse key
			data = gEngfuncs.COM_ParseFile(data, token);
			if (token[0] == '}')
				break; // finish parsing this entity

			if (!data)
			{	
				gEngfuncs.Con_DPrintf("UTIL_FindEntityInMap: EOF without closing brace\n");
				return 0;
			};
			
			strcpy (keyname, token);

			// another hack to fix keynames with trailing spaces
			n = strlen(keyname);
			while (n && keyname[n-1] == ' ')
			{
				keyname[n-1] = 0;
				n--;
			}
			
			// parse value	
			data = gEngfuncs.COM_ParseFile(data, token);
			if (!data)
			{	
				gEngfuncs.Con_DPrintf("UTIL_FindEntityInMap: EOF without closing brace\n");
				return 0;
			};
	
			if (token[0] == '}')
			{
				gEngfuncs.Con_DPrintf("UTIL_FindEntityInMap: closing brace without data");
				return 0;
			}

			if (!strcmp(keyname,"classname"))
			{
				if (!strcmp(token, name ))
				{
					found = 1;	// thats our entity
				}
			};

			if( !strcmp( keyname, "angle" ) )
			{
				float y = atof( token );
				
				if (y >= 0)
				{
					angle[0] = 0.0f;
					angle[1] = y;
				}
				else if ((int)y == -1)
				{
					angle[0] = -90.0f;
					angle[1] =   0.0f;;
				}
				else
				{
					angle[0] = 90.0f;
					angle[1] =  0.0f;
				}

				angle[2] =  0.0f;
			}

			if( !strcmp( keyname, "angles" ) )
			{
				UTIL_StringToVector(angle, token);
			}
			
			if (!strcmp(keyname,"origin"))
			{
				UTIL_StringToVector(origin, token);

			};
				
		} // while (1)

		if (found)
			return 1;

	}

	return 0;	// we search all entities, but didn't found the correct

}

//-----------------------------------------------------------------------------
// SetSpectatorStartPosition(): 
// Get valid map position and 'beam' spectator to this position
//-----------------------------------------------------------------------------

void CHudSpectator::SetSpectatorStartPosition()
{
	// search for info_player start
	if ( UTIL_FindEntityInMap( "trigger_camera",  m_cameraOrigin, m_cameraAngles ) )
		iJumpSpectator = 1;

	else if ( UTIL_FindEntityInMap( "info_player_start",  m_cameraOrigin, m_cameraAngles ) )
		iJumpSpectator = 1;

	else if ( UTIL_FindEntityInMap( "info_player_deathmatch",  m_cameraOrigin, m_cameraAngles ) )
		iJumpSpectator = 1;

	else if ( UTIL_FindEntityInMap( "info_player_coop",  m_cameraOrigin, m_cameraAngles ) )
		iJumpSpectator = 1;
	else
	{
		// jump to 0,0,0 if no better position was found
		VectorCopy(vec3_origin, m_cameraOrigin);
		VectorCopy(vec3_origin, m_cameraAngles);
	}
	
	VectorCopy(m_cameraOrigin, vJumpOrigin);
	VectorCopy(m_cameraAngles, vJumpAngles);

	iJumpSpectator = 1;	// jump anyway
}

//-----------------------------------------------------------------------------
// Purpose: Loads new icons
//-----------------------------------------------------------------------------
int CHudSpectator::VidInit()
{
	m_hsprPlayerMarine	= Safe_SPR_Load("sprites/iplayerm.spr");
	m_hsprPlayerAlien	= Safe_SPR_Load("sprites/iplayera.spr");
	m_hsprPlayerDead	= Safe_SPR_Load("sprites/iplayerdead.spr");
	m_hsprUnkownMap		= Safe_SPR_Load("sprites/tile.spr");
	//m_hsprBeam			= Safe_SPR_Load("sprites/laserbeam.spr");
	//m_hsprCamera		= Safe_SPR_Load("sprites/camera.spr");
	m_hCrosshair		= Safe_SPR_Load("sprites/crosshairs.spr");
    m_hsprWhite         = Safe_SPR_Load(kWhiteSprite);
	
	return 1;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Input  : flTime - 
//			intermission - 
//-----------------------------------------------------------------------------
int CHudSpectator::Draw(float flTime)
{

	// draw only in spectator mode
	if ( !g_iUser1  )
		return 0;

    // Removed by mmcguire.
    /*
	// if user pressed zoom, aplly changes
	if ( (m_zoomDelta != 0.0f) && (	g_iUser1 == OBS_MAP_FREE ) )
	{
		m_mapZoom += m_zoomDelta;

		if ( m_mapZoom > 3.0f ) 
			m_mapZoom = 3.0f;

		if ( m_mapZoom < 0.5f ) 
			m_mapZoom = 0.5f;
	}

	// if user moves in map mode, change map origin
	if ( (m_moveDelta != 0.0f) && (g_iUser1 != OBS_ROAMING) )
	{
		vec3_t	right;
		AngleVectors(v_angles, NULL, right, NULL);
		VectorNormalize(right);
		VectorScale(right, m_moveDelta, right );

		VectorAdd( m_mapOrigin, right, m_mapOrigin )

	}
    */
	    
    //DrawOverviewMap();

/*
	if (!IsInOverviewMode())
    {
		return 0;
    }

    if (m_hsprWhite != NULL)
    {

        float bgColor[] = { 0.1, 0.1, 0.1, 1 };
        float borderColor[] = { 1, 1, 1, 1 };

        gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
        gEngfuncs.pTriAPI->CullFace(TRI_NONE);

        gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)(gEngfuncs.GetSpritePointer(m_hsprWhite)), 0);

        float gammaScale = 1.0f / gHUD.GetGammaSlope();
        
        // Draw the border on the overview map and the inset view.

        gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
        gEngfuncs.pTriAPI->CullFace(TRI_NONE);

        gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)(gEngfuncs.GetSpritePointer(m_hsprWhite)), 0);
        gEngfuncs.pTriAPI->Color4f(gammaScale * borderColor[0], gammaScale * borderColor[1], gammaScale * borderColor[2], borderColor[3]);

        gEngfuncs.pTriAPI->Begin(TRI_LINES);
    
        int insetX1 = XRES(m_OverviewData.insetWindowX);
        int insetY1 = YRES(m_OverviewData.insetWindowY);
        int insetX2 = insetX1 + XRES(m_OverviewData.insetWindowWidth);
        int insetY2 = insetY1 + YRES(m_OverviewData.insetWindowHeight);

        gEngfuncs.pTriAPI->Vertex3f(insetX1, insetY1, 1);
        gEngfuncs.pTriAPI->Vertex3f(insetX2, insetY1, 1);

        gEngfuncs.pTriAPI->Vertex3f(insetX2, insetY1, 1);
        gEngfuncs.pTriAPI->Vertex3f(insetX2, insetY2, 1);
        
        gEngfuncs.pTriAPI->Vertex3f(insetX2, insetY2, 1);
        gEngfuncs.pTriAPI->Vertex3f(insetX1, insetY2, 1);
    
        gEngfuncs.pTriAPI->Vertex3f(insetX1, insetY2, 1);
        gEngfuncs.pTriAPI->Vertex3f(insetX1, insetY1, 1);

        gEngfuncs.pTriAPI->End();
    
    }*/



    /*
	// Only draw the icon names only if map mode is in Main Mode
	if ( g_iUser1 < OBS_MAP_FREE  ) 
		return 1;
	
	if ( !m_drawnames->value )
		return 1;
	
	// make sure we have player info
	gViewPort->GetAllPlayersInfo();
    */

	return 1;

}

bool CHudSpectator::IsInOverviewMode() const
{
    return g_iUser1 && m_overviewMode && gHUD.GetIsNSMode();
}

void CHudSpectator::SetOverviewMode(bool overviewMode)
{
    m_overviewMode = overviewMode;
}

void CHudSpectator::DrawOverviewMap()
{

	// draw only in spectator mode
	if (!IsInOverviewMode())
    {
		return;
    }

    AvHOverviewMap& theOverviewMap = gHUD.GetOverviewMap();

    AvHOverviewMap::DrawInfo theDrawInfo;

    theDrawInfo.mX = XRES(m_OverviewData.insetWindowX + m_OverviewData.insetWindowWidth + 4);
    theDrawInfo.mY = YRES(SPECTATOR_PANEL_HEIGHT + 4);
    theDrawInfo.mWidth  = ScreenWidth() - theDrawInfo.mX - XRES(4);
    theDrawInfo.mHeight = ScreenHeight() - YRES(SPECTATOR_PANEL_HEIGHT + 4) - theDrawInfo.mY;
    
    AvHMapExtents theMapExtents;
    theOverviewMap.GetMapExtents(theMapExtents);

    theDrawInfo.mFullScreen = true;

	float worldWidth  = theMapExtents.GetMaxMapX() - theMapExtents.GetMinMapX();
	float worldHeight = theMapExtents.GetMaxMapY() - theMapExtents.GetMinMapY();

	float xScale;
	float yScale;

	float aspect1 = worldWidth / worldHeight;
    float aspect2 = ((float)theDrawInfo.mWidth) / theDrawInfo.mHeight; 
    
    if (aspect1 > aspect2)
	{
		xScale = 1;
		yScale = 1 / aspect2;
	}
	else
	{
		xScale = aspect2;
		yScale = 1;
    }

	float centerX = (theMapExtents.GetMinMapX() + theMapExtents.GetMaxMapX()) / 2;
	float centerY = (theMapExtents.GetMinMapY() + theMapExtents.GetMaxMapY()) / 2;
    
    theDrawInfo.mViewWorldMinX = centerX - worldWidth  * xScale * 0.5;
    theDrawInfo.mViewWorldMinY = centerY - worldHeight * yScale * 0.5;
    theDrawInfo.mViewWorldMaxX = centerX + worldWidth  * xScale * 0.5;
    theDrawInfo.mViewWorldMaxY = centerY + worldHeight * yScale * 0.5;

    if (m_hsprWhite != NULL)
    {

        float bgColor[] = { 0.1, 0.1, 0.1, 1 };
        float borderColor[] = { 1, 1, 1, 1 };

        gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
        gEngfuncs.pTriAPI->CullFace(TRI_NONE);

        gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)(gEngfuncs.GetSpritePointer(m_hsprWhite)), 0);

        float gammaScale = 1.0f / gHUD.GetGammaSlope();
        
        // Draw the background.
        
	    gEngfuncs.pTriAPI->Color4f(gammaScale * bgColor[0], gammaScale * bgColor[1], gammaScale * bgColor[2], bgColor[3]);

        gEngfuncs.pTriAPI->Begin(TRI_QUADS);
    
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX, theDrawInfo.mY, 1);
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX, theDrawInfo.mY + theDrawInfo.mHeight, 1);
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX + theDrawInfo.mWidth, theDrawInfo.mY + theDrawInfo.mHeight, 1);
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX + theDrawInfo.mWidth, theDrawInfo.mY, 1);
    
        gEngfuncs.pTriAPI->End();
   
        // Draw the overview map.

        theOverviewMap.Draw(theDrawInfo);

        // Draw the border on the overview map and the inset view.

        gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
        gEngfuncs.pTriAPI->CullFace(TRI_NONE);

        gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)(gEngfuncs.GetSpritePointer(m_hsprWhite)), 0);
        gEngfuncs.pTriAPI->Color4f(gammaScale * borderColor[0], gammaScale * borderColor[1], gammaScale * borderColor[2], borderColor[3]);

        gEngfuncs.pTriAPI->Begin(TRI_LINES);
    
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX, theDrawInfo.mY, 1);
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX, theDrawInfo.mY + theDrawInfo.mHeight, 1);

        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX, theDrawInfo.mY + theDrawInfo.mHeight, 1);
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX + theDrawInfo.mWidth, theDrawInfo.mY + theDrawInfo.mHeight, 1);
        
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX + theDrawInfo.mWidth, theDrawInfo.mY + theDrawInfo.mHeight, 1);
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX + theDrawInfo.mWidth, theDrawInfo.mY, 1);
    
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX + theDrawInfo.mWidth, theDrawInfo.mY, 1);
        gEngfuncs.pTriAPI->Vertex3f(theDrawInfo.mX, theDrawInfo.mY, 1);

        int insetX1 = XRES(m_OverviewData.insetWindowX);
        int insetY1 = YRES(m_OverviewData.insetWindowY);
        int insetX2 = insetX1 + XRES(m_OverviewData.insetWindowWidth);
        int insetY2 = insetY1 + YRES(m_OverviewData.insetWindowHeight);

        gEngfuncs.pTriAPI->Vertex3f(insetX1, insetY1, 1);
        gEngfuncs.pTriAPI->Vertex3f(insetX2, insetY1, 1);

        gEngfuncs.pTriAPI->Vertex3f(insetX2, insetY1, 1);
        gEngfuncs.pTriAPI->Vertex3f(insetX2, insetY2, 1);
        
        gEngfuncs.pTriAPI->Vertex3f(insetX2, insetY2, 1);
        gEngfuncs.pTriAPI->Vertex3f(insetX1, insetY2, 1);
    
        gEngfuncs.pTriAPI->Vertex3f(insetX1, insetY2, 1);
        gEngfuncs.pTriAPI->Vertex3f(insetX1, insetY1, 1);

        gEngfuncs.pTriAPI->End();
    
    }


}


void CHudSpectator::DirectorMessage( int iSize, void *pbuf )
{
	float	value;
	char *	string;

	BEGIN_READ( pbuf, iSize );

	int cmd = READ_BYTE();
	
	switch ( cmd )	// director command byte 
	{ 
		case DRC_CMD_START	:	
							// now we have to do some things clientside, since the proxy doesn't know our mod 
							g_iPlayerClass = 0;
							g_iTeamNumber = 0;

							// fake a InitHUD & ResetHUD message
							gHUD.MsgFunc_InitHUD(NULL,0, NULL);
							gHUD.MsgFunc_ResetHUD(NULL, 0, NULL);
														
							break;

		case DRC_CMD_EVENT	:
							m_lastPrimaryObject		=	READ_WORD();
							m_lastSecondaryObject	=	READ_WORD();
							m_iObserverFlags		=	READ_LONG();
														
							if ( m_autoDirector->value )
							{
								if ( (g_iUser2 != m_lastPrimaryObject) || (g_iUser3 != m_lastSecondaryObject) )
									V_ResetChaseCam();	

								g_iUser2 = m_lastPrimaryObject;
								g_iUser3 = m_lastSecondaryObject;
							}

							// gEngfuncs.Con_Printf("Director Camera: %i %i\n", firstObject, secondObject);
							break;

		case DRC_CMD_MODE  :
							if ( m_autoDirector->value )
							{
								SetMode( READ_BYTE());
							}
							break;

		case DRC_CMD_CAMERA	:
							if ( m_autoDirector->value )
							{
								vJumpOrigin[0] = READ_COORD();	// position
								vJumpOrigin[1] = READ_COORD();
								vJumpOrigin[2] = READ_COORD();

								vJumpAngles[0] = READ_COORD();	// view angle
								vJumpAngles[1] = READ_COORD();
								vJumpAngles[2] = READ_COORD();

								gEngfuncs.SetViewAngles( vJumpAngles );

								iJumpSpectator = 1;
							}
							break;

		case DRC_CMD_MESSAGE:
							{
								client_textmessage_t * msg = &m_HUDMessages[m_lastHudMessage];
								
								msg->effect = READ_BYTE();		// effect

								UnpackRGB( (int&)msg->r1, (int&)msg->g1, (int&)msg->b1, READ_LONG() );		// color
								msg->r2 = msg->r1;
								msg->g2 = msg->g1;
								msg->b2 = msg->b1;
								msg->a2 = msg->a1 = 0xFF;	// not transparent
										
								msg->x = READ_FLOAT();	// x pos
								msg->y = READ_FLOAT();	// y pos
												
								msg->fadein		= READ_FLOAT();	// fadein
								msg->fadeout	= READ_FLOAT();	// fadeout
								msg->holdtime	= READ_FLOAT();	// holdtime
								msg->fxtime		= READ_FLOAT();	// fxtime;

								strncpy( m_HUDMessageText[m_lastHudMessage], READ_STRING(), 128 );
								m_HUDMessageText[m_lastHudMessage][127]=0;	// text 

								msg->pMessage = m_HUDMessageText[m_lastHudMessage];
								msg->pName	  = "HUD_MESSAGE";

								gHUD.m_Message.MessageAdd( msg->pName, gHUD.m_flTime);

								m_lastHudMessage++;
								m_lastHudMessage %= MAX_SPEC_HUD_MESSAGES;
			
							}

							break;

		case DRC_CMD_SOUND :
							string = READ_STRING();
							value =  READ_FLOAT();
							
							// gEngfuncs.Con_Printf("DRC_CMD_FX_SOUND: %s %.2f\n", string, value );
							gEngfuncs.pEventAPI->EV_PlaySound(0, v_origin, CHAN_BODY, string, value, ATTN_NORM, 0, PITCH_NORM );
							
							break;

		case DRC_CMD_TIMESCALE	:
							value = READ_FLOAT();
							break;



		case DRC_CMD_STATUS:
							READ_LONG(); // total number of spectator slots
							m_iSpectatorNumber = READ_LONG(); // total number of spectator
							READ_WORD(); // total number of relay proxies

							gViewPort->UpdateSpectatorPanel();
							break;

		case DRC_CMD_BANNER:
							// gEngfuncs.Con_DPrintf("GUI: Banner %s\n",READ_STRING() ); // name of banner tga eg gfx/temp/7454562234563475.tga
							gViewPort->m_pSpectatorPanel->m_TopBanner->LoadImage( READ_STRING() );
							gViewPort->UpdateSpectatorPanel();
							break;

		case DRC_CMD_FADE:		
							/*{
								screenfade_t sf;
								
								sf.fader = 255;
								sf.fadeg = 0;
								sf.fadeb = 0;
								sf.fadealpha = 128;
								sf.fadeFlags = FFADE_STAYOUT | FFADE_OUT;

								// gHUD.m_flTime = cl.time
				
								stream->ReadFloat();	// 	duration
								sf.stream->ReadFloat();	//	holdTime
								sf.fadeFlags = READ_SHORT();	//	flags
								stream->ReadLong();	// color	RGB

								CallEnghudSetScreenFade( &sf );
							}*/
							break;

		case DRC_CMD_STUFFTEXT:
							ClientCmd( READ_STRING() );
							break;

		default			:	gEngfuncs.Con_DPrintf("CHudSpectator::DirectorMessage: unknown command %i.\n", cmd );
	}
}

void CHudSpectator::FindNextPlayer(bool bReverse)
{
	// MOD AUTHORS: Modify the logic of this function if you want to restrict the observer to watching
	//				only a subset of the players. e.g. Make it check the target's team.

	int		iStart;
	cl_entity_t * pEnt = NULL;

	// if we are NOT in HLTV mode, spectator targets are set on server
	if ( !gEngfuncs.IsSpectateOnly() )
	{
		char cmdstring[32];
		// forward command to server
		sprintf(cmdstring,"follownext %i",bReverse?1:0);
		gEngfuncs.pfnServerCmd(cmdstring);
		return;
	}
	
	if ( g_iUser2 )
		iStart = g_iUser2;
	else
		iStart = 1;

	g_iUser2 = 0;

	int	    iCurrent = iStart;

	int iDir = bReverse ? -1 : 1; 

	// make sure we have player info
	gViewPort->GetAllPlayersInfo();


	do
	{
		iCurrent += iDir;

		// Loop through the clients
		if (iCurrent > MAX_PLAYERS)
			iCurrent = 1;
		if (iCurrent < 1)
			iCurrent = MAX_PLAYERS;

		pEnt = gEngfuncs.GetEntityByIndex( iCurrent );

		if ( !IsActivePlayer( pEnt ) )
			continue;

		// MOD AUTHORS: Add checks on target here.

		g_iUser2 = iCurrent;
		break;

	} while ( iCurrent != iStart );

	// Did we find a target?
	if ( !g_iUser2 )
	{
		gEngfuncs.Con_DPrintf( "No observer targets.\n" );
		// take save camera position 
		VectorCopy(m_cameraOrigin, vJumpOrigin);
		VectorCopy(m_cameraAngles, vJumpAngles);
	}
	else
	{
		// use new entity position for roaming
		VectorCopy ( pEnt->origin, vJumpOrigin );
		VectorCopy ( pEnt->angles, vJumpAngles );
	}
	iJumpSpectator = 1;
}

void CHudSpectator::HandleButtonsDown( int ButtonPressed )
{
    if ( !gViewPort )
        return;
    
    //Not in intermission.
    if ( gHUD.m_iIntermission )
        return;
    
    if ( !g_iUser1 )
        return; // dont do anything if not in spectator mode
    
    // don't handle buttons during normal demo playback
    if ( gEngfuncs.pDemoAPI->IsPlayingback() && !gEngfuncs.IsSpectateOnly() )
        return;

    int theNewMainMode = g_iUser1;

    // Jump changes main window modes
    if ( ButtonPressed & IN_JUMP )
    {
        bool theFirstPerson = (g_iUser1 == OBS_IN_EYE);
        bool theInOverviewMode = gHUD.m_Spectator.IsInOverviewMode();

        // NS
        if(gHUD.GetIsNSMode())
        {
            // First-person full -> chase camera full -> firstperson with overview -> chase camera with overview
            if(theFirstPerson && !theInOverviewMode)
            {
                gHUD.m_Spectator.SetMode(OBS_CHASE_LOCKED);
                //gHUD.m_Spectator.SetOverviewMode(false);
            }
            else if(!theFirstPerson && !theInOverviewMode)
            {
                gHUD.m_Spectator.SetMode(OBS_IN_EYE);
                gHUD.m_Spectator.SetOverviewMode(true);
            }
            else if(theFirstPerson && theInOverviewMode)
            {
                gHUD.m_Spectator.SetMode(OBS_CHASE_LOCKED);
                //gHUD.m_Spectator.SetOverviewMode(true);
            }
            else if(!theFirstPerson && theInOverviewMode)
            {
                gHUD.m_Spectator.SetMode(OBS_IN_EYE);
                gHUD.m_Spectator.SetOverviewMode(false);
            }
        }
        // Combat
        else
        {
            // First-person full -> chase camera full
            if(theFirstPerson)
            {
                gHUD.m_Spectator.SetMode(OBS_CHASE_LOCKED);
                gHUD.m_Spectator.SetOverviewMode(false);
            }
            else
            {
                gHUD.m_Spectator.SetMode(OBS_IN_EYE);
                gHUD.m_Spectator.SetOverviewMode(false);
            }
        }
    }
    
    //g_iUser1 = theNewMainMode;

    // Attack moves to the next player
    if ( ButtonPressed & (IN_MOVELEFT | IN_MOVERIGHT) )
    { 
        FindNextPlayer( (ButtonPressed & IN_MOVELEFT) ? true:false );
        
//        if ( g_iUser1 == OBS_ROAMING )
//        {
//            gEngfuncs.SetViewAngles( vJumpAngles );
//            iJumpSpectator = 1;
//            
//        }

        // lease directed mode if player want to see another player
        m_autoDirector->value = 0.0f;
    }

/*
	double time = gEngfuncs.GetClientTime();

	int newMainMode		= g_iUser1;
	int newInsetMode	= m_pip->value;

	// gEngfuncs.Con_Printf(" HandleButtons:%i\n", ButtonPressed );
	if ( !gViewPort )
		return;

	//Not in intermission.
	if ( gHUD.m_iIntermission )
		 return;

	if ( !g_iUser1 )
		return; // dont do anything if not in spectator mode

	// don't handle buttons during normal demo playback
	if ( gEngfuncs.pDemoAPI->IsPlayingback() && !gEngfuncs.IsSpectateOnly() )
		return;
	// Slow down mouse clicks. 
	if ( m_flNextObserverInput > time )
		return;

	// enable spectator screen
	if ( ButtonPressed & IN_DUCK )
	{
		gViewPort->m_pSpectatorPanel->ShowMenu(!gViewPort->m_pSpectatorPanel->m_menuVisible);
	}
	
	//  'Use' changes inset window mode
	if ( ButtonPressed & IN_USE )
	{
		newInsetMode = ToggleInset(true);
	}

	// if not in HLTV mode, buttons are handled server side
	if ( gEngfuncs.IsSpectateOnly() )
	{
		// changing target or chase mode not in overviewmode without inset window

		// Jump changes main window modes
		if ( ButtonPressed & IN_JUMP )
		{
			if ( g_iUser1 == OBS_CHASE_LOCKED )
				newMainMode = OBS_CHASE_FREE;

			else if ( g_iUser1 == OBS_CHASE_FREE )
				newMainMode = OBS_IN_EYE;

			else if ( g_iUser1 == OBS_IN_EYE )
				newMainMode = OBS_ROAMING;

			else if ( g_iUser1 == OBS_ROAMING )
				newMainMode = OBS_MAP_FREE;

			else if ( g_iUser1 == OBS_MAP_FREE )
				newMainMode = OBS_MAP_CHASE;

			else
				newMainMode = OBS_CHASE_FREE;	// don't use OBS_CHASE_LOCKED anymore
		}

		// Attack moves to the next player
		if ( ButtonPressed & (IN_ATTACK | IN_ATTACK2) )
		{ 
			FindNextPlayer( (ButtonPressed & IN_ATTACK2) ? true:false );

			if ( g_iUser1 == OBS_ROAMING )
			{
				gEngfuncs.SetViewAngles( vJumpAngles );
				iJumpSpectator = 1;
	
			}
			// lease directed mode if player want to see another player
			m_autoDirector->value = 0.0f;
		}
	}

	SetModes(newMainMode, newInsetMode);

	if ( g_iUser1 == OBS_MAP_FREE )
	{
		if ( ButtonPressed & IN_FORWARD )
			m_zoomDelta =  0.01f;

		if ( ButtonPressed & IN_BACK )
			m_zoomDelta = -0.01f;
		
		if ( ButtonPressed & IN_MOVELEFT )
			m_moveDelta = -12.0f;

		if ( ButtonPressed & IN_MOVERIGHT )
			m_moveDelta =  12.0f;
	}

	m_flNextObserverInput = time + 0.2;
*/

}

void CHudSpectator::HandleButtonsUp( int ButtonPressed )
{
	if ( !gViewPort )
		return;

	if ( !gViewPort->m_pSpectatorPanel->isVisible() )
		return; // dont do anything if not in spectator mode

	if ( ButtonPressed & (IN_FORWARD | IN_BACK) )
		m_zoomDelta = 0.0f;
	
	if ( ButtonPressed & (IN_MOVELEFT | IN_MOVERIGHT) )
		m_moveDelta = 0.0f;
}

void CHudSpectator::SetMode(int iNewMainMode)
{
	// if value == -1 keep old value
	if ( iNewMainMode == -1 )
		iNewMainMode = g_iUser1;

	// main modes ettings will override inset window settings
	if ( iNewMainMode != g_iUser1 )
	{
		// if we are NOT in HLTV mode, main spectator mode is set on server
		if ( !gEngfuncs.IsSpectateOnly() )
		{
			char cmdstring[32];
			// forward command to server
			sprintf(cmdstring,"specmode %i",iNewMainMode );
			gEngfuncs.pfnServerCmd(cmdstring);
			return;
		}
        else
        {
            if ( !g_iUser2 && (iNewMainMode !=OBS_ROAMING ) )	// make sure we have a target
            {
                // choose last Director object if still available
                if ( IsActivePlayer( gEngfuncs.GetEntityByIndex( m_lastPrimaryObject ) ) )
                {
                    g_iUser2 = m_lastPrimaryObject;
                    g_iUser3 = m_lastSecondaryObject;
                }
                else
                {
                    FindNextPlayer(false); // find any target
                }
            }
            
            switch ( iNewMainMode )
            {
            case OBS_CHASE_LOCKED:
                g_iUser1 = OBS_CHASE_LOCKED;
                break;
                
            case OBS_CHASE_FREE:
                g_iUser1 = OBS_CHASE_FREE;
                break;
                
            case OBS_ROAMING	:	// jump to current vJumpOrigin/angle
                g_iUser1 = OBS_ROAMING;
                if ( g_iUser2 )
                {
                    V_GetChasePos( g_iUser2, v_cl_angles, vJumpOrigin, vJumpAngles );
                    gEngfuncs.SetViewAngles( vJumpAngles );
                    iJumpSpectator = 1;
                }
                break;
                
            case OBS_IN_EYE:
                g_iUser1 = OBS_IN_EYE;
                break;
            
            /*
            case OBS_MAP_FREE	:	g_iUser1 = OBS_MAP_FREE;
                // reset user values
                m_mapZoom = m_OverviewData.zoom;
                m_mapOrigin = m_OverviewData.origin;
                break;
                
            case OBS_MAP_CHASE	:	g_iUser1 = OBS_MAP_CHASE;
                // reset user values
                m_mapZoom = m_OverviewData.zoom;
                m_mapOrigin = m_OverviewData.origin;
                break;
            */
            }
            
            if ( (g_iUser1 == OBS_IN_EYE) || (g_iUser1 == OBS_ROAMING) ) 
            {
                m_crosshairRect.left	 = 24;
                m_crosshairRect.top	 = 0;
                m_crosshairRect.right	 = 48;
                m_crosshairRect.bottom = 24;
                
                gHUD.SetCurrentCrosshair( m_hCrosshair, m_crosshairRect, 255, 255, 255 );
            }
            else
            {
                memset( &m_crosshairRect,0,sizeof(m_crosshairRect) );
                gHUD.SetCurrentCrosshair( 0, m_crosshairRect, 0, 0, 0 );
            } 
            
            //char string[128];
            //sprintf(string, "#Spec_Mode%d", g_iUser1 );
            //sprintf(string, "%c%s", HUD_PRINTCENTER, CHudTextMessage::BufferedLocaliseTextString( string ));
            //gHUD.m_TextMessage.MsgFunc_TextMsg(NULL, strlen(string)+1, string );
        }
    }    

    gViewPort->UpdateSpectatorPanel();
}

bool CHudSpectator::IsActivePlayer(cl_entity_t * ent)
{
	return ( ent && 
			 ent->player && 
			 ent->curstate.solid != SOLID_NOT &&
			 ent != gEngfuncs.GetLocalPlayer() &&
			 g_PlayerInfoList[ent->index].name != NULL
			);
}


bool CHudSpectator::ParseOverviewFile( )
{
	//char filename[255];
	//char levelname[255];
	//char token[1024];
	//float height;
	
	char *pfile  = NULL;

	memset( &m_OverviewData, 0, sizeof(m_OverviewData));

	// fill in standrd values
	m_OverviewData.insetWindowX = 4;	// upper left corner
	m_OverviewData.insetWindowY = 4 + SPECTATOR_PANEL_HEIGHT;
	m_OverviewData.insetWindowHeight = 180;
	m_OverviewData.insetWindowWidth = 240;
	m_OverviewData.origin[0] = 0.0f;
	m_OverviewData.origin[1] = 0.0f;
	m_OverviewData.origin[2] = 0.0f;
	m_OverviewData.zoom	= 1.0f;
	m_OverviewData.layers = 0;
	m_OverviewData.layersHeights[0] = 0.0f;
	strcpy( m_OverviewData.map, gEngfuncs.pfnGetLevelName() );

	if ( strlen( m_OverviewData.map ) == 0 )
		return false; // not active yet

    /*
	strcpy(levelname, m_OverviewData.map + 5);
	levelname[strlen(levelname)-4] = 0;
	
    sprintf(filename, "overviews/%s.txt", levelname );

	pfile = (char *)gEngfuncs.COM_LoadFile( filename, 5, NULL);

	if (!pfile)
	{
		gEngfuncs.Con_Printf("Couldn't open file %s. Using default values for overiew mode.\n", filename );
		return false;
	}
	
	while (true)
	{
		pfile = gEngfuncs.COM_ParseFile(pfile, token);

		if (!pfile)
			break;

		if ( !stricmp( token, "global" ) )
		{
			// parse the global data
			pfile = gEngfuncs.COM_ParseFile(pfile, token);
			if ( stricmp( token, "{" ) ) 
			{
				gEngfuncs.Con_Printf("Error parsing overview file %s. (expected { )\n", filename );
				return false;
			}

			pfile = gEngfuncs.COM_ParseFile(pfile,token);

			while (stricmp( token, "}") )
			{
				if ( !stricmp( token, "zoom" ) )
				{
					pfile = gEngfuncs.COM_ParseFile(pfile,token);
					m_OverviewData.zoom = atof( token );
				} 
				else if ( !stricmp( token, "origin" ) )
				{
					pfile = gEngfuncs.COM_ParseFile(pfile, token); 
					m_OverviewData.origin[0] = atof( token );
					pfile = gEngfuncs.COM_ParseFile(pfile,token); 
					m_OverviewData.origin[1] = atof( token );
					pfile = gEngfuncs.COM_ParseFile(pfile, token); 
					m_OverviewData.origin[2] = atof( token );
				}
				else if ( !stricmp( token, "rotated" ) )
				{
					pfile = gEngfuncs.COM_ParseFile(pfile,token); 
					m_OverviewData.rotated = atoi( token );
				}
				else if ( !stricmp( token, "inset" ) )
				{

                    // Removed by mmcguire.
                    // This isn't supported anymore.
					pfile = gEngfuncs.COM_ParseFile(pfile,token); 
					//m_OverviewData.insetWindowX = atof( token );
					pfile = gEngfuncs.COM_ParseFile(pfile,token); 
					//m_OverviewData.insetWindowY = atof( token );
					pfile = gEngfuncs.COM_ParseFile(pfile,token); 
					//m_OverviewData.insetWindowWidth = atof( token );
					pfile = gEngfuncs.COM_ParseFile(pfile,token); 
					//m_OverviewData.insetWindowHeight = atof( token );

				}
				else
				{
					gEngfuncs.Con_Printf("Error parsing overview file %s. (%s unkown)\n", filename, token );
					return false;
				}

				pfile = gEngfuncs.COM_ParseFile(pfile,token); // parse next token

			}
		}
		else if ( !stricmp( token, "layer" ) )
		{
			// parse a layer data

			if ( m_OverviewData.layers == OVERVIEW_MAX_LAYERS )
			{
				gEngfuncs.Con_Printf("Error parsing overview file %s. ( too many layers )\n", filename );
				return false;
			}

			pfile = gEngfuncs.COM_ParseFile(pfile,token);

				
			if ( stricmp( token, "{" ) ) 
			{
				gEngfuncs.Con_Printf("Error parsing overview file %s. (expected { )\n", filename );
				return false;
			}

			pfile = gEngfuncs.COM_ParseFile(pfile,token);

			while (stricmp( token, "}") )
			{
				if ( !stricmp( token, "image" ) )
				{
					pfile = gEngfuncs.COM_ParseFile(pfile,token);
					strcpy(m_OverviewData.layersImages[ m_OverviewData.layers ], token);
					
					
				} 
				else if ( !stricmp( token, "height" ) )
				{
					pfile = gEngfuncs.COM_ParseFile(pfile,token); 
					height = atof(token);
					m_OverviewData.layersHeights[ m_OverviewData.layers ] = height;
				}
				else
				{
					gEngfuncs.Con_Printf("Error parsing overview file %s. (%s unkown)\n", filename, token );
					return false;
				}

				pfile = gEngfuncs.COM_ParseFile(pfile,token); // parse next token
			}

			m_OverviewData.layers++;

		}
	}

	gEngfuncs.COM_FreeFile( pfile );
    */

	m_mapZoom = m_OverviewData.zoom;
	m_mapOrigin = m_OverviewData.origin;

	return true;

}

void CHudSpectator::LoadMapSprites()
{
	// right now only support for one map layer
	if (m_OverviewData.layers > 0 )
	{
		m_MapSprite = gEngfuncs.LoadMapSprite( m_OverviewData.layersImages[0] );
	}
	else
		m_MapSprite = NULL; // the standard "unkown map" sprite will be used instead
}

void CHudSpectator::DrawOverviewLayer()
{
	float screenaspect, xs, ys, xStep, yStep, x,y,z;
	int ix,iy,i,xTiles,yTiles,frame;

	qboolean	hasMapImage = m_MapSprite?TRUE:FALSE;
	model_t *   dummySprite = (struct model_s *)gEngfuncs.GetSpritePointer( m_hsprUnkownMap);

	if ( hasMapImage)
	{
		i = m_MapSprite->numframes / (4*3);
		i = sqrt((float)i);
		xTiles = i*4;
		yTiles = i*3;
	}
	else
	{
		xTiles = 8;
		yTiles = 6;
	}


	screenaspect = 4.0f/3.0f;	


	xs = m_OverviewData.origin[0];
	ys = m_OverviewData.origin[1];
	z  = ( 90.0f - v_angles[0] ) / 90.0f;		
	z *= m_OverviewData.layersHeights[0]; // gOverviewData.z_min - 32;	

	// i = r_overviewTexture + ( layer*OVERVIEW_X_TILES*OVERVIEW_Y_TILES );

	gEngfuncs.pTriAPI->RenderMode( kRenderTransTexture );
	gEngfuncs.pTriAPI->CullFace( TRI_NONE );
	gEngfuncs.pTriAPI->Color4f( 1.0, 1.0, 1.0, 1.0 );

	frame = 0;
	

	// rotated view ?
	if ( m_OverviewData.rotated )
	{
		xStep = (2*4096.0f / m_OverviewData.zoom ) / xTiles;
		yStep = -(2*4096.0f / (m_OverviewData.zoom* screenaspect) ) / yTiles;

		y = ys + (4096.0f / (m_OverviewData.zoom * screenaspect));

		for (iy = 0; iy < yTiles; iy++)
		{
			x = xs - (4096.0f / (m_OverviewData.zoom));

			for (ix = 0; ix < xTiles; ix++)
			{
				if (hasMapImage)
					gEngfuncs.pTriAPI->SpriteTexture( m_MapSprite, frame );
				else
					gEngfuncs.pTriAPI->SpriteTexture( dummySprite, 0 );

				gEngfuncs.pTriAPI->Begin( TRI_QUADS );
					gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
					gEngfuncs.pTriAPI->Vertex3f (x, y, z);

					gEngfuncs.pTriAPI->TexCoord2f( 1, 0 );
					gEngfuncs.pTriAPI->Vertex3f (x+xStep ,y,  z);

					gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
					gEngfuncs.pTriAPI->Vertex3f (x+xStep, y+yStep, z);

					gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
					gEngfuncs.pTriAPI->Vertex3f (x, y+yStep, z);
				gEngfuncs.pTriAPI->End();

				frame++;
				x+= xStep;
			}

			y+=yStep;
		}
	} 
	else
	{
		xStep = -(2*4096.0f / m_OverviewData.zoom ) / xTiles;
		yStep = -(2*4096.0f / (m_OverviewData.zoom* screenaspect) ) / yTiles;

				
		x = xs + (4096.0f / (m_OverviewData.zoom * screenaspect ));

		
		
		for (ix = 0; ix < yTiles; ix++)
		{
			
			y = ys + (4096.0f / (m_OverviewData.zoom));	
						
			for (iy = 0; iy < xTiles; iy++)	
			{
				if (hasMapImage)
					gEngfuncs.pTriAPI->SpriteTexture( m_MapSprite, frame );
				else
					gEngfuncs.pTriAPI->SpriteTexture( dummySprite, 0 );

				gEngfuncs.pTriAPI->Begin( TRI_QUADS );
					gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
					gEngfuncs.pTriAPI->Vertex3f (x, y, z);

					gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
					gEngfuncs.pTriAPI->Vertex3f (x+xStep ,y,  z);

					gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
					gEngfuncs.pTriAPI->Vertex3f (x+xStep, y+yStep, z);

					gEngfuncs.pTriAPI->TexCoord2f( 1, 0 );
					gEngfuncs.pTriAPI->Vertex3f (x, y+yStep, z);
				gEngfuncs.pTriAPI->End();

				frame++;
				
				y+=yStep;
			}

			x+= xStep;
			
		}
	}
}

void CHudSpectator::DrawOverviewEntities()
{
    /*
	int				i,ir,ig,ib;
	struct model_s *hSpriteModel;
	vec3_t			origin, angles, point, forward, right, left, up, world, screen, offset;
	float			x,y,z, r,g,b, sizeScale = 4.0f;
	cl_entity_t *	ent;
	float rmatrix[3][4];	// transformation matrix
	
	float			zScale = (90.0f - v_angles[0] ) / 90.0f;
	

	z = m_OverviewData.layersHeights[0] * zScale;
	// get yellow/brown HUD color
	//UnpackRGB(ir,ig,ib, RGB_YELLOWISH);
	gHUD.GetPrimaryHudColor(ir, ig, ib);
	r = (float)ir/255.0f;
	g = (float)ig/255.0f;
	b = (float)ib/255.0f;
	
	gEngfuncs.pTriAPI->CullFace( TRI_NONE );

	for (i=0; i < MAX_PLAYERS; i++ )
		m_vPlayerPos[i][2] = -1;	// mark as invisible 

    // draw all players
	
    float depthOffset = 0;

    for (i=MAX_OVERVIEW_ENTITIES - 1; i >= 0; i--)
	{
		if ( !m_OverviewEntities[i].hSprite )
			continue;

		hSpriteModel = (struct model_s *)gEngfuncs.GetSpritePointer( m_OverviewEntities[i].hSprite );
		ent = m_OverviewEntities[i].entity;

		int theSpriteFrame = m_OverviewEntities[i].mFrame;
		gEngfuncs.pTriAPI->SpriteTexture( hSpriteModel,  theSpriteFrame);
		
        gEngfuncs.pTriAPI->RenderMode(  kRenderTransAdd);

		// see R_DrawSpriteModel
		// draws players sprite

		AngleVectors(ent->angles, right, up, NULL );

		VectorCopy(ent->origin,origin);

		// Set origin of blip to just above map height, so blips are all drawn on map
		origin.z = m_OverviewData.layersHeights[0] + kOverviewEntityZHeight + depthOffset;

        gEngfuncs.pTriAPI->Begin( TRI_QUADS );

        float gammaSlope = gHUD.GetGammaSlope();

        gEngfuncs.pTriAPI->Color4f(
            m_OverviewEntities[i].mColorR / gammaSlope, 
            m_OverviewEntities[i].mColorG / gammaSlope,
            m_OverviewEntities[i].mColorB / gammaSlope,
            1);
		
		gEngfuncs.pTriAPI->TexCoord2f (1, 0);
		VectorMA (origin,  16.0f * sizeScale, up, point);
		VectorMA (point,   16.0f * sizeScale, right, point);
		point[2] *= zScale;
		gEngfuncs.pTriAPI->Vertex3fv (point);

		gEngfuncs.pTriAPI->TexCoord2f (0, 0);
		
		VectorMA (origin,  16.0f * sizeScale, up, point);
		VectorMA (point,  -16.0f * sizeScale, right, point);
		point[2] *= zScale;
		gEngfuncs.pTriAPI->Vertex3fv (point);

		gEngfuncs.pTriAPI->TexCoord2f (0,1);
		VectorMA (origin, -16.0f * sizeScale, up, point);
		VectorMA (point,  -16.0f * sizeScale, right, point);
		point[2] *= zScale;
		gEngfuncs.pTriAPI->Vertex3fv (point);

		gEngfuncs.pTriAPI->TexCoord2f (1,1);
		VectorMA (origin, -16.0f * sizeScale, up, point);
		VectorMA (point,   16.0f * sizeScale, right, point);
		point[2] *= zScale;
		gEngfuncs.pTriAPI->Vertex3fv (point);

		gEngfuncs.pTriAPI->End ();

		
		if ( !ent->player)
			continue;
		// draw line under player icons
		origin[2] *= zScale;

		gEngfuncs.pTriAPI->RenderMode( kRenderTransAdd );
		
		hSpriteModel = (struct model_s *)gEngfuncs.GetSpritePointer( m_hsprBeam );
		gEngfuncs.pTriAPI->SpriteTexture( hSpriteModel, 0 );
		
		gEngfuncs.pTriAPI->Color4f(r, g, b, 0.3);

		gEngfuncs.pTriAPI->Begin ( TRI_QUADS );
		gEngfuncs.pTriAPI->TexCoord2f (1, 0);
		gEngfuncs.pTriAPI->Vertex3f (origin[0]+4, origin[1]+4, origin[2]-zScale);
		gEngfuncs.pTriAPI->TexCoord2f (0, 0);
		gEngfuncs.pTriAPI->Vertex3f (origin[0]-4, origin[1]-4, origin[2]-zScale);
		gEngfuncs.pTriAPI->TexCoord2f (0, 1);
		gEngfuncs.pTriAPI->Vertex3f (origin[0]-4, origin[1]-4,z);
		gEngfuncs.pTriAPI->TexCoord2f (1, 1);
		gEngfuncs.pTriAPI->Vertex3f (origin[0]+4, origin[1]+4,z);
		gEngfuncs.pTriAPI->End ();

		gEngfuncs.pTriAPI->Begin ( TRI_QUADS );
		gEngfuncs.pTriAPI->TexCoord2f (1, 0);
		gEngfuncs.pTriAPI->Vertex3f (origin[0]-4, origin[1]+4, origin[2]-zScale);
		gEngfuncs.pTriAPI->TexCoord2f (0, 0);
		gEngfuncs.pTriAPI->Vertex3f (origin[0]+4, origin[1]-4, origin[2]-zScale);
		gEngfuncs.pTriAPI->TexCoord2f (0, 1);
		gEngfuncs.pTriAPI->Vertex3f (origin[0]+4, origin[1]-4,z);
		gEngfuncs.pTriAPI->TexCoord2f (1, 1);
		gEngfuncs.pTriAPI->Vertex3f (origin[0]-4, origin[1]+4,z);
		gEngfuncs.pTriAPI->End ();

		// calculate screen position for name and infromation in hud::draw()
		if ( gEngfuncs.pTriAPI->WorldToScreen(origin,screen) )
			continue;	// object is behind viewer

		screen[0] = XPROJECT(screen[0]);
		screen[1] = YPROJECT(screen[1]);
		screen[2] = 0.0f;

		// calculate some offset under the icon
		origin[0]+=32.0f;
		origin[1]+=32.0f;
		
		gEngfuncs.pTriAPI->WorldToScreen(origin,offset);

		offset[0] = XPROJECT(offset[0]);
		offset[1] = YPROJECT(offset[1]);
		offset[2] = 0.0f;
			
		VectorSubtract(offset, screen, offset );

		int playerNum = ent->index - 1;

		m_vPlayerPos[playerNum][0] = screen[0];	
		m_vPlayerPos[playerNum][1] = screen[1] + Length(offset);	
		m_vPlayerPos[playerNum][2] = 1;	// mark player as visible 


	}

	if ( !m_pip || !m_drawcone->value )
		return;

	// get current camera position and angle

	if ( m_pip == INSET_IN_EYE || g_iUser1 == OBS_IN_EYE )
	{ 
		V_GetInEyePos( g_iUser2, origin, angles );
	}
	else if ( m_pip == INSET_CHASE_FREE  || g_iUser1 == OBS_CHASE_FREE )
	{
		V_GetChasePos( g_iUser2, v_cl_angles, origin, angles );
	}
	else if ( g_iUser1 == OBS_ROAMING )
	{
		VectorCopy( v_sim_org, origin );
		VectorCopy( v_cl_angles, angles );
	}
	else
		V_GetChasePos( g_iUser2, NULL, origin, angles );

	
	// draw camera sprite

	x = origin[0];
	y = origin[1];
	z = origin[2];

	// Set origin of cone to just above map height, so blips are all drawn on map
	z = m_OverviewData.layersHeights[0] + kOverviewEntityZHeight;

	angles[0] = 0; // always show horizontal camera sprite

	hSpriteModel = (struct model_s *)gEngfuncs.GetSpritePointer( m_hsprCamera );
	gEngfuncs.pTriAPI->RenderMode( kRenderTransAdd );
	gEngfuncs.pTriAPI->SpriteTexture( hSpriteModel, 0 );
	
	
	gEngfuncs.pTriAPI->Color4f( r, g, b, 1.0 );

	AngleVectors(angles, forward, NULL, NULL );
	VectorScale (forward, 512.0f, forward);
	
	offset[0] =  0.0f; 
	offset[1] = 45.0f; 
	offset[2] =  0.0f; 

	AngleMatrix(offset, rmatrix );
	VectorTransform(forward, rmatrix , right );

	offset[1]= -45.0f;
	AngleMatrix(offset, rmatrix );
	VectorTransform(forward, rmatrix , left );

	gEngfuncs.pTriAPI->Begin (TRI_TRIANGLES);
		gEngfuncs.pTriAPI->TexCoord2f( 0, 0 );
		gEngfuncs.pTriAPI->Vertex3f (x+right[0], y+right[1], (z+right[2]) * zScale);

		gEngfuncs.pTriAPI->TexCoord2f( 0, 1 );
		gEngfuncs.pTriAPI->Vertex3f (x, y, z  * zScale);
		
		gEngfuncs.pTriAPI->TexCoord2f( 1, 1 );
		gEngfuncs.pTriAPI->Vertex3f (x+left[0], y+left[1], (z+left[2]) * zScale);
	gEngfuncs.pTriAPI->End ();
    */

}



void CHudSpectator::DrawOverview()
{
    /*
	// draw only in sepctator mode
	if ( !g_iUser1 )
		return;

	// Only draw the overview if Map Mode is selected for this view
	if ( m_iDrawCycle == 0 &&  ( (g_iUser1 != OBS_MAP_FREE) && (g_iUser1 != OBS_MAP_CHASE) ) ) 
		return;

	if ( m_iDrawCycle == 1 && m_pip->value < INSET_MAP_FREE )
		return;

	DrawOverviewLayer();
	DrawOverviewEntities();
	CheckOverviewEntities();
    */

}



void CHudSpectator::CheckOverviewEntities()
{
	double time = gEngfuncs.GetClientTime();

	// removes old entities from list
	for ( int i = 0; i< MAX_OVERVIEW_ENTITIES; i++ )
	{
		// remove entity from list if it is too old
		if ( m_OverviewEntities[i].killTime < time )
		{
			memset( &m_OverviewEntities[i], 0, sizeof (overviewEntity_t) );
		}
	}
}

bool CHudSpectator::AddOverviewEntity( int type, struct cl_entity_s *ent, const char *modelname)
{
	HSPRITE	hSprite = 0;
	double  duration = -1.0f;	// duration -1 means show it only this frame;
	int theFrame = 0;
	bool theSuccess = false;
    int theRenderMode;

	if ( ent )
	{

		if (ent->curstate.solid != SOLID_NOT)
		{
            gHUD.GetSpriteForUser3(AvHUser3(ent->curstate.iuser3), hSprite, theFrame, theRenderMode);
        }

		/*
        if ( type == ET_PLAYER )
		{
			if ( ent->curstate.solid != SOLID_NOT)
			{
				int thePlayerClass = g_PlayerExtraInfo[ent->index].playerclass;
				switch(thePlayerClass)
				{
				case PLAYERCLASS_ALIVE_MARINE:
					hSprite = this->m_hsprPlayerMarine;
					theFrame = 0;
					break;
				case PLAYERCLASS_ALIVE_HEAVY_MARINE:
					hSprite = this->m_hsprPlayerMarine;
					theFrame = 1;
					break;
				case PLAYERCLASS_COMMANDER:
					hSprite = this->m_hsprPlayerMarine;
					theFrame = 2;
					break;
				case PLAYERCLASS_ALIVE_LEVEL1:
					hSprite = this->m_hsprPlayerAlien;
					theFrame = 0;
					break;
				case PLAYERCLASS_ALIVE_LEVEL2:
					hSprite = this->m_hsprPlayerAlien;
					theFrame = 1;
					break;
				case PLAYERCLASS_ALIVE_LEVEL3:
					hSprite = this->m_hsprPlayerAlien;
					theFrame = 2;
					break;
				case PLAYERCLASS_ALIVE_LEVEL4:
					hSprite = this->m_hsprPlayerAlien;
					theFrame = 3;
					break;
				case PLAYERCLASS_ALIVE_LEVEL5:
					hSprite = this->m_hsprPlayerAlien;
					theFrame = 4;
					break;
				case PLAYERCLASS_ALIVE_GESTATING:
					hSprite = this->m_hsprPlayerAlien;
					theFrame = 5;
					break;
		
				case PLAYERCLASS_ALIVE_DIGESTING:
					break;
				}
			}
			else
			{
				// it's an spectator
			}
		}
		else if (type == ET_NORMAL)
		{
			// Now help icons
			if(hSprite == 0)
			{
				AvHUser3 theUser3 = AvHUser3(ent->curstate.iuser3);
				theFrame = gHUD.GetHelpIconFrameFromUser3(theUser3);
				if(theFrame != -1)
				{
					hSprite = gHUD.GetHelpSprite();
				}
			}
		}
        */
	}

	if(hSprite > 0)
	{
        
        int theTeam = ent->curstate.team;

        float theR = kFTeamColors[theTeam][0];
        float theG = kFTeamColors[theTeam][1];
        float theB = kFTeamColors[theTeam][2];

        theSuccess = AddOverviewEntityToList(hSprite, ent, gEngfuncs.GetClientTime() + duration, theFrame, theRenderMode, theR, theG, theB);
	
    }

	return theSuccess;
}

void CHudSpectator::DeathMessage(int victim)
{
	// find out where the victim is
	cl_entity_t *pl = gEngfuncs.GetEntityByIndex(victim);

	if (pl && pl->player)
		AddOverviewEntityToList(m_hsprPlayerDead, pl, gEngfuncs.GetClientTime() + 2.0f, 0, kRenderTransTexture, 1, 1, 1);
}

bool CHudSpectator::AddOverviewEntityToList(HSPRITE sprite, cl_entity_t *ent, double killTime, int inFrame, int inRenderMode, float r, float g, float b)
{
	for ( int i = 0; i< MAX_OVERVIEW_ENTITIES; i++ )
	{
		// find empty entity slot
		if ( m_OverviewEntities[i].entity == NULL)
		{
			m_OverviewEntities[i].entity = ent;
			m_OverviewEntities[i].hSprite = sprite;
			m_OverviewEntities[i].killTime = killTime;
			m_OverviewEntities[i].mFrame = inFrame;
            m_OverviewEntities[i].mRenderMode = inRenderMode;
            m_OverviewEntities[i].mColorR = r;
            m_OverviewEntities[i].mColorG = g;
            m_OverviewEntities[i].mColorB = b;
            return true;
		}
	}

	return false;	// maximum overview entities reached
}
void CHudSpectator::CheckSettings()
{
	// disallow same inset mode as main mode:

	//m_pip->value = floor(m_pip->value);
	
	// Removed by mmcguire.
    /*
    if ( ( g_iUser1 < OBS_MAP_FREE ) && ( m_pip->value == INSET_CHASE_LOCKED || m_pip->value == INSET_IN_EYE ) )
	{
		// otherwise both would show in World picures
		m_pip->value = INSET_OFF;
	}

	// disble in intermission screen
	if ( gHUD.m_iIntermission )
		m_pip->value = INSET_OFF;
    */

	// check chat mode
	if ( m_chatEnabled != (gHUD.m_SayText.m_HUD_saytext->value!=0) )
	{
		// hud_saytext changed
		m_chatEnabled = (gHUD.m_SayText.m_HUD_saytext->value!=0);

		if ( gEngfuncs.IsSpectateOnly() )
		{
			// tell proxy our new chat mode
			char chatcmd[32];
			sprintf(chatcmd, "ignoremsg %i", m_chatEnabled?0:1 );
			gEngfuncs.pfnServerCmd(chatcmd);
		}
	}

	// HL/TFC has no oberserver corsshair, so set it client side
	if ( g_iUser1 == OBS_IN_EYE ) 
	{
		m_crosshairRect.left	 = 24;
		m_crosshairRect.top	 = 0;
		m_crosshairRect.right	 = 48;
		m_crosshairRect.bottom = 24;
					
		gHUD.SetCurrentCrosshair( m_hCrosshair, m_crosshairRect, 255, 255, 255 );
	}
	else
	{
		memset( &m_crosshairRect,0,sizeof(m_crosshairRect) );
		gHUD.SetCurrentCrosshair( 0, m_crosshairRect, 0, 0, 0 );
	} 

    // Removed by mmcguire.
    /*
	// if we are a real player on server don't allow inset window
	// in First Person mode since this is our resticted forcecamera mode 2
	// team number 3 = SPECTATOR see player.h

	if ( ( (g_iTeamNumber == 1) || (g_iTeamNumber == 2)) && (g_iUser1 == OBS_IN_EYE) )
		m_pip->value = INSET_OFF;
    */

	// draw small border around inset view, adjust upper black bar
	//gViewPort->m_pSpectatorPanel->EnableInsetView( m_pip->value != INSET_OFF );
    gViewPort->m_pSpectatorPanel->EnableInsetView( IsInOverviewMode() );

}


void CHudSpectator::Reset()
{
	// Reset HUD
	if ( strcmp( m_OverviewData.map, gEngfuncs.pfnGetLevelName() ) )
	{
		// update level overview if level changed
		ParseOverviewFile();
		LoadMapSprites();
	}

	memset( &m_OverviewEntities, 0, sizeof(m_OverviewEntities));

	SetSpectatorStartPosition();
}

void CHudSpectator::InitHUDData()
{
	m_lastPrimaryObject = m_lastSecondaryObject = 0;
	m_flNextObserverInput = 0.0f;
	m_lastHudMessage = 0;
	m_iSpectatorNumber = 0;
	iJumpSpectator	= 0;
	g_iUser1 = g_iUser2 = 0;

	memset( &m_OverviewData, 0, sizeof(m_OverviewData));
	memset( &m_OverviewEntities, 0, sizeof(m_OverviewEntities));

	if ( gEngfuncs.IsSpectateOnly() || gEngfuncs.pDemoAPI->IsPlayingback() )
		m_autoDirector->value = 1.0f;
	else
		m_autoDirector->value = 0.0f;

	Reset();

	SetMode( OBS_CHASE_FREE);

	g_iUser2 = 0; // fake not target until first camera command

	// reset HUD FOV
	gHUD.m_iFOV =  CVAR_GET_FLOAT("default_fov");
}

