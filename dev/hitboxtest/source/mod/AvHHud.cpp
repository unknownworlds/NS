//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Main NS NUD, also interface to client network messages
//
// $Workfile: AvHHud.cpp $
// $Date: 2002/10/28 20:35:32 $
//
//-------------------------------------------------------------------------------
// $Log: AvHHud.cpp,v $
// Revision 1.70  2002/10/28 20:35:32  Flayra
// - Fix for gamma reset with VAC
// - Info location fix after changelevel
//
// Revision 1.69  2002/10/25 21:49:10  Flayra
// - Updated skin to sit in pev->skin
// - Reset components every tick to fix problem with disappearing team resource label
//
// Revision 1.68  2002/10/24 21:29:49  Flayra
// - Moved help client-side
// - Fixed particle/changelevel crash
// - Reworked marine upgrade drawing
// - Added lots of utility functions for help system (mirrors functions on server)
// - Removed gamma message unless it failed or if maxplayers is 1
// - Fixed alien hive sight crash
// - Show players under reticle while in ready room and spectating
// - Removed ugly/too-prevalent user3 icons
//
// Revision 1.67  2002/10/18 22:19:49  Flayra
// - Added alien easter egg sayings
//
// Revision 1.66  2002/10/16 20:53:41  Flayra
// - Draw scan model specially so it looks right
//
// Revision 1.65  2002/10/16 00:58:02  Flayra
// - Removed hotgroups
// - Added "need order" alert
//
// Revision 1.64  2002/10/03 20:24:39  Flayra
// - Changes for "more resources required"
//
// Revision 1.63  2002/10/03 18:54:30  Flayra
// - Allow right-click to cancel building placement
// - Fixed help icons
// - Added a couple utility functions
// - Reworked order notification
// - Reworked blip network messages to avoid hard-coded limit
// - Sent max resources down with current resources
// - Countdown sound no longer prevents other hud sounds
// - Alien trigger sounds
// - New order sounds
// - No longer disable nodes out of our cost range
//
// Revision 1.62  2002/09/25 20:47:19  Flayra
// - Don't draw elements on HUD when dead
// - UI refactoring
// - Split reticle help into help text and reticle text
// - Removed use order
// - Added separate select sound for alien
// - Multiple move sounds
// - Only draw entity build/health status when under reticle (no more scanning around you)
// - Added 3 new sayings
//
// Revision 1.61  2002/09/23 22:18:25  Flayra
// - Added alien build circles
// - Game status changes so particles aren't sent every time
// - Demo playback changes (save restore basic data that HUD already has)
// - New alert sounds
// - Skin support
//
// Revision 1.60  2002/09/09 19:55:24  Flayra
// - Added hive info indicator
// - Fixed bug where reticle tooltip help text wasn't being set until a weapon was selected
// - Fixed release mode bug where tooltips weren't expiring
// - Fixed bug where marine upgrades blinked
// - "No commander" indicator now blinks
//
// Revision 1.59  2002/08/31 18:01:01  Flayra
// - Work at VALVe
//
// Revision 1.58  2002/08/16 02:37:49  Flayra
// - HUD sounds no longer cut each other off (they won't play instead of cutting off another sound)
// - Tooltip sounds
// - Selection issues
// - Draw rings around buildings that need to be built
// - Removed old overwatch code
//
// Revision 1.57  2002/08/09 01:02:40  Flayra
// - Added hooks for demo playback, removed prediction selection
//
// Revision 1.56  2002/08/02 21:59:12  Flayra
// - Added reticle help, new tooltip system and much nicer order drawing!  Refactored view model drawing a bit, hoping to make texture blending work for it.
//
// Revision 1.55  2002/07/26 23:05:01  Flayra
// - Generate numerical feedback for damage events
// - Refactoring for more info when looking at something (instead of bad-looking player names only)
//
// Revision 1.54  2002/07/24 18:45:41  Flayra
// - Linux and scripting changes
//
// Revision 1.53  2002/07/23 17:07:36  Flayra
// - Added visually-smooth energy level, added versatile location code, new hive sight info, refactored to remove extra sprites (128 HUD sprites bug), commander tech help fixes
//
// Revision 1.52  2002/07/10 14:41:55  Flayra
// - Fixed bug where non-sighted particle systems weren't being drawn for players on the ground (bug #127)
//
// Revision 1.51  2002/07/08 17:07:56  Flayra
// - Started to add display of marine upgrade sprite, fixed bug where building indicators aren't displayed after a map change, info_location drawing changes, primal scream color tweak, removed old hive drawing code
//
// Revision 1.50  2002/07/01 21:35:05  Flayra
// - Removed lots of outdated sprites and sprite code, added building ranges, fixed ghost building problem (bug #82)
//
// Revision 1.49  2002/06/25 18:03:09  Flayra
// - Added info_locations, removed old weapon help system, added smooth resource swelling, lots of alien UI usability changes, fixed problem with ghost building
//
// Revision 1.48  2002/06/10 19:55:36  Flayra
// - New commander UI (bindable via hotkeys, added REMOVE_SELECTION for when clicking menu options when no players selected)
//
// Revision 1.47  2002/06/03 16:48:45  Flayra
// - Help sprites moved into one animated sprite, select sound volume reduced (now that sound is normalized)
//
// Revision 1.46  2002/05/28 17:48:14  Flayra
// - Minimap refactoring, reinforcement refactoring, new hive sight fixes, recycling support
//
// Revision 1.45  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHHud.h"
#include "cl_dll/hud.h"
#include "cl_dll/parsemsg.h"
#include "cl_dll/cl_util.h"
#include "vgui_label.h"
#include "ui/PieMenu.h"
#include "mod/AvHTeamHierarchy.h"
#include "mod/AvHPieMenuHandler.h"
#include "mod/AvHParticleTemplateClient.h"
#include "mod/AvHParticleSystemManager.h"
#include "mod/AvHConstants.h"
#include "mod/AvHClientVariables.h"
#include "mod/AvHSpecials.h"
#include "ui/FadingImageLabel.h"
#include "mod/AvHScrollHandler.h"
#include "mod/AvHEvents.h"
#include "pm_shared/pm_shared.h"
#include "common/cl_entity.h"
#include "mod/AvHCommanderModeHandler.h"
#include "mod/AvHParticleEditorHandler.h"
#include "mod/AvHTechNodes.h"
#include "mod/AvHMovementUtil.h"
#include "mod/AvHTitles.h"
#include "mod/AvHSelectionHelper.h"
#include "mod/AvHActionButtons.h"
#include "pm_shared/pm_debug.h"
#include "util/MathUtil.h"
#include "util/STLUtil.h"
#include "mod/AvHSharedUtil.h"
#include "common/r_efx.h"
#include "cl_dll/eventscripts.h"
#include <stdlib.h>
#include "mod/AvHSprites.h"
#include "ui/UIUtil.h"
#include "mod/AvHMiniMap.h"
#include "types.h"
#include <signal.h>
#include "common/event_api.h"
#include "mod/AvHHulls.h"
#include "common/com_model.h"
#include "mod/AvHBasePlayerWeaponConstants.h"
#include "cl_dll/vgui_ScorePanel.h"
#include "mod/AvHAlienAbilityConstants.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHScriptManager.h"
#include "mod/AvHHudConstants.h"
#include "cl_dll/demo.h"
#include "common/demo_api.h"
#include "cl_dll/ammohistory.h"
#include "mod/AvHTechImpulsePanel.h"
#include "mod/AvHServerVariables.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHCommandConstants.h"
#include "mod/AvHDebugUtil.h" 
#include "engine/keydefs.h"
#include "mod/ChatPanel.h"
#include "cl_dll/r_studioint.h"
#include "util/Tokenizer.h"
#include <sstream>

//#include "cl_dll/studio_util.h"
//#include "cl_dll/r_studioint.h"

void IN_GetMousePos( int *mx, int *my );
extern playermove_t *pmove;
void RemoveAllDecals();
void ScorePanel_InitializeDemoRecording();

// Include windows for GDI and gamma functions
#include "windows.h"

extern engine_studio_api_t IEngineStudio;

AvHPieMenuHandler						gPieMenuHandler;
AvHScrollHandler						gScrollHandler;
AvHCommanderModeHandler					gCommanderHandler;
AvHParticleEditorHandler				gParticleEditorHandler;
extern AvHParticleTemplateListClient	gParticleTemplateList;
extern DebugPointListType				gTriDebugLocations;
extern extra_player_info_t				g_PlayerExtraInfo[MAX_PLAYERS+1];
extern WeaponsResource					gWR;

extern double							gClientTimeLastUpdate;
extern "C" Vector						gPredictedPlayerOrigin;
extern "C" Vector						gPredictedPlayerVOfs;
extern void	__CmdFunc_Close(void);
extern int CL_ButtonBits(int);
extern int g_iVisibleMouse;

GammaTable AvHHud::sPregameGammaTable;
GammaTable AvHHud::sGameGammaTable;

bool AvHHud::sShowMap = false;

// Global because of global HUD complilation error
AvHMiniMap	gMiniMap;

#include "VGUI_RepaintSignal.h"

//extern vec3_t		v_origin;
//extern vec3_t		v_angles;
//vec3_t				gPlayerOrigin;
//vec3_t				gPlayerAngles;

#ifdef AVH_PREDICT_SELECT
extern AvHSelectionHelper				gSelectionHelper;
#endif

//#if defined( AVH_CLIENT )
//extern "C"	float gOverwatchTargetRange;
extern float gOverwatchTargetRange;
//#endif

extern bool gResetViewAngles;
extern vec3_t gViewAngles;
extern char sDebugString[128];

float kOverwatchFlashInterval = 2.5f;
const float kReticleInfoMaxAlpha = 50;
int gVisibleMouse = 0;

//voogru: cvar pointers, these should always remain valid once they are set.

cvar_t *gl_monolights = NULL;
cvar_t *gl_overbright = NULL;
cvar_t *gl_clear = NULL;
cvar_t *hud_draw = NULL;
cvar_t *r_drawviewmodel = NULL;
extern cvar_t *cl_movespeedkey;
cvar_t *gl_d3dflip = NULL;
cvar_t *s_show = NULL;
cvar_t *lightgamma = NULL;
cvar_t *r_detailtextures = NULL;

const AvHMapExtents& GetMapExtents()
{
	return gHUD.GetMapExtents();
}

NumericalInfoEffect::NumericalInfoEffect(float inPosition[3], float inNumber, int inEventType, float inTimeCreated)
{
	this->mPosition[0] = inPosition[0];
	this->mPosition[1] = inPosition[1];
	this->mPosition[2] = inPosition[2];

	this->mNumber = inNumber;
	this->mEventType = inEventType;

	this->mTimeCreated = inTimeCreated;
}

void NumericalInfoEffect::GetPosition(float* outPosition) const
{
	outPosition[0] = this->mPosition[0];
	outPosition[1] = this->mPosition[1];
	outPosition[2] = this->mPosition[2];
}

float NumericalInfoEffect::GetNumber() const
{
	return this->mNumber;
}

int NumericalInfoEffect::GetEventType() const
{
	return this->mEventType;
}

float NumericalInfoEffect::GetTimeCreated() const
{
	return this->mTimeCreated;
}

void NumericalInfoEffect::SetPosition(float inPosition[3])
{
	this->mPosition[0] = inPosition[0];
	this->mPosition[1] = inPosition[1];
	this->mPosition[2] = inPosition[2];
}

void AvHHud::OnActivateSteamUI()
{
    // Set the normal gamma so the Steam UI looks correct.
    sPregameGammaTable.InitializeToVideoState();
    mSteamUIActive = true;
}

void AvHHud::OnDeactivateSteamUI()
{

    // Set the special NS gamma.
    SetGamma(mDesiredGammaSlope);
    mSteamUIActive = false;

    // The Steam UI screws up the mouse cursor so reset it.
    if (gViewPort != NULL)
    {
        gViewPort->UpdateCursorState();
    }

}

void AvHHud::OnLostFocus()
{
    sPregameGammaTable.InitializeToVideoState();
}

bool AvHHud::OnKeyEvent(int virtualKey, int scanCode, bool pressed)
{

    if (gViewPort != NULL && !mSteamUIActive)
    {

        ChatPanel* theChatPanel = gViewPort->GetChatPanel();

        if (theChatPanel && theChatPanel->isVisible())
        {
            if (pressed)
            {
                theChatPanel->KeyDown(virtualKey, scanCode);
                return true;        
            }
            else
            {
                // If the key wasn't pressed while the chat window was open,
                // the key up needs to go to HL.
                return theChatPanel->WasKeyPushed(virtualKey);
            }
        }
       
        if (virtualKey == VK_ESCAPE && GetInTopDownMode() && mGhostBuilding != MESSAGE_NULL)
        {
            if (pressed)
            {
                CancelBuilding();
            }
            return true;
        }
    
    }

    return false;

}

int AvHHud::GetGameTime() const
{
	int theGameTime = 0;

	if(this->mGameTime > 0)
	{
		theGameTime = (int)(this->mGameTime);
	}

	return theGameTime;
}

int AvHHud::GetGameTimeLimit() const
{	
	return this->mTimeLimit;
}

int AvHHud::GetCombatAttackingTeamNumber() const
{
	return this->mCombatAttackingTeamNumber;
}

bool AvHHud::GetShowingMap()
{
	return sShowMap;
}

bool AvHHud::GetGameStarted() const
{
	return (this->mGameTime >= 0) && !this->mGameEnded;
}

bool AvHHud::GetIsAlive(bool inIncludeSpectating) const
{
	bool theIsAlive = false;

	cl_entity_s* thePlayer = gEngfuncs.GetLocalPlayer();
	if(inIncludeSpectating)
	{
		thePlayer = this->GetVisiblePlayer();
	}
	
	if(thePlayer)
	{
		int thePlayerIndex = thePlayer->index;
		if((thePlayerIndex) && (thePlayerIndex <= MAX_PLAYERS))
		{
			int thePlayerClass = g_PlayerExtraInfo[thePlayerIndex].playerclass;
			switch(thePlayerClass)
			{
			case PLAYERCLASS_ALIVE_MARINE:
			case PLAYERCLASS_ALIVE_HEAVY_MARINE:
			case PLAYERCLASS_ALIVE_JETPACK_MARINE:
			case PLAYERCLASS_ALIVE_LEVEL1:
			case PLAYERCLASS_ALIVE_LEVEL2:
			case PLAYERCLASS_ALIVE_LEVEL3:
			case PLAYERCLASS_ALIVE_LEVEL4:
			case PLAYERCLASS_ALIVE_LEVEL5:
			case PLAYERCLASS_ALIVE_DIGESTING:
			case PLAYERCLASS_ALIVE_GESTATING:
			case PLAYERCLASS_COMMANDER:
				theIsAlive = true;
			}
		}
	}

	return theIsAlive;
}

bool GetIsWithinRegion(float inNormX, float inNormY, float inLowX, float inLowY, float inHighX, float inHighY)
{
	bool inRegion = false;
	
	if((inNormX >= inLowX) && (inNormY >= inLowY) && (inNormX <= inHighX) && (inNormY < inHighY))
	{
		inRegion = true;
	}
	
	return inRegion;
}

bool AvHHud::GetIsRegionBlockedByUI(float inNormX, float inNormY)
{
	bool theIsBlocked = true;
	
	if(	GetIsWithinRegion(inNormX, inNormY, 0, .061, .3017, .6797) ||
		GetIsWithinRegion(inNormX, inNormY, .248, .0791, .7753, .6823) ||
		GetIsWithinRegion(inNormX, inNormY, .748, .092, 1, .6575) ||
		GetIsWithinRegion(inNormX, inNormY, .751, .645, .870, .678) ||
		GetIsWithinRegion(inNormX, inNormY, .337, .679, .729, .754) ||
		GetIsWithinRegion(inNormX, inNormY, .337, .717, .703, .823) )
	{
		theIsBlocked = false;

		// Now check pending requests (the only HUD element not drawn as part of the outlying frame
		for(PendingRequestListType::const_iterator theIterator = this->mPendingRequests.begin(); theIterator != this->mPendingRequests.end(); theIterator++)
		{
			AvHMessageID theMessageID = theIterator->first;
			char theComponentName[256];
			sprintf(theComponentName, kPendingImpulseSpecifier, (int)theMessageID);
			
			AvHTechImpulsePanel* theTechImpulsePanel = NULL;
			if(this->GetManager().GetVGUIComponentNamed(theComponentName, theTechImpulsePanel))
			{
				int thePosX, thePosY;
				theTechImpulsePanel->getPos(thePosX, thePosY);
				
				int theWidth, theHeight;
				theTechImpulsePanel->getSize(theWidth, theHeight);
				
				int theHighX = thePosX + theWidth;
				int theHighY = thePosY + theHeight;

				float theScreenWidth = ScreenWidth();
				float theScreenHeight = ScreenHeight();

				if(GetIsWithinRegion(inNormX, inNormY, thePosX/theScreenWidth, thePosY/theScreenHeight, theHighX/theScreenWidth, theHighY/theScreenHeight))
				{
					theIsBlocked = true;
					break;
				}
			}
		}
	}
	
	return theIsBlocked;
}

bool AvHHud::GetIsShowingMap() const
{
	return sShowMap;
}

void AvHHud::ClearSelection()
{
	// Clear our selection, make sure the server knows.
//	this->mSelectionJustChanged = true;
//	this->mSelected.clear();
//
//	// Create effects
//	this->SetSelectionEffects(this->mSelected);
//
//	this->PlayHUDSound(HUD_SOUND_SELECT);
//
//	gCommanderHandler.SetSelectedUnits(this->mSelected);

	#ifdef AVH_PREDICT_SELECT
	gSelectionHelper.ClearSelection();
	#endif
	
	this->mGroupEvent = COMMANDER_REMOVESELECTION;
}


void CLinkGhostBuildingCallback( struct tempent_s *ent, float frametime, float currenttime)
{
	gHUD.GhostBuildingCallback(ent, frametime, currenttime);
}

// For easily adding message functions
#define BIND_MESSAGE(x) \
    int __MsgFunc_##x(const char *pszName, int iSize, void *pbuf) \
	{ \
    	return gHUD.##x(pszName, iSize, pbuf ); \
    }

AvHHud::AvHHud(const string& inFilename, UIFactory* inFactory) : UIHud(inFilename, inFactory)
{
	this->ClearData();
    mSteamUIActive = false;
}

void AvHHud::AddNumericalInfoMessage(float inOrigin[3], float inNumber, int inEventType)
{
	NumericalInfoEffect theEffect(inOrigin, inNumber, inEventType, this->mTimeOfLastUpdate);
	this->mNumericalInfoEffects.push_back(theEffect);
}

void AvHHud::AddTooltip(const char* inMessageText, bool inIsToolTip, float inTooltipWidth)
{
	if(!gEngfuncs.pDemoAPI->IsPlayingback() && (strlen(inMessageText) > 0))
	{
		if(gEngfuncs.pfnGetCvarFloat(kvAutoHelp) || !inIsToolTip)
		{
			AvHTooltip theNewTooltip;
			
			theNewTooltip.SetText(string(inMessageText));
			theNewTooltip.SetNormalizedScreenX(1.0f - inTooltipWidth - kHelpMessageLeftEdgeInset);
			theNewTooltip.SetNormalizedScreenY(0.01f);
			theNewTooltip.SetCentered(false);
			theNewTooltip.SetIgnoreFadeForLifetime(true);
			theNewTooltip.SetNormalizedMaxWidth(inTooltipWidth);
			
			if(inIsToolTip)
			{
				this->PlayHUDSound(HUD_SOUND_TOOLTIP);
			}
			
			this->mTooltips.push_back(theNewTooltip);
		}
	}
}

bool AvHHud::AddTooltipOnce(const char* inMessageText, bool inIsToolTip)
{
	bool theAddedTooltip = false;
	
	string theMessage(inMessageText);
	
	// Check if message is in sent list
	StringList::iterator theIter = std::find(this->mDisplayedToolTipList.begin(), this->mDisplayedToolTipList.end(), theMessage);
	if(theIter == this->mDisplayedToolTipList.end())
	{
		// If not
		// Call AddTooltip
		this->AddTooltip(inMessageText, inIsToolTip);
		theAddedTooltip = true;
		
		// Add message to list
		this->mDisplayedToolTipList.push_back(theMessage);
	}
	
	return theAddedTooltip;
}


void AvHHud::Cancel(void)
{
	ASSERT(this->mInTopDownMode);

	gCommanderHandler.CancelHit();
	//this->m_Ammo.UserCmd_Close();
}

void AvHHud::ClearData()
{
	this->mResources = 0;
	
	this->mHierarchy = NULL;
	this->mShowMapHierarchy = NULL;
	
	this->mCommanderResourceLabel = NULL;
	this->mGenericProgressBar = NULL;
	this->mResearchProgressBar = NULL;
	this->mAlienProgressBar = NULL;
	this->mResearchLabel = NULL;
	//this->mArmorLevel = ARMOR_BASE;
	this->mTimeOfLastUpdate = 0.0;
	this->mTimeOfNextHudSound = -1;
    this->mLastHUDSoundPlayed = HUD_SOUND_INVALID;
	this->mTimeOfLastEntityUpdate = -1;
	this->mInTopDownMode = false;
	this->mLeftMouseStarted = false;
	this->mLeftMouseEnded = false;
	this->mPlacingBuilding = false;
	this->mRightMouseStarted = false;
	this->mRightMouseEnded = false;
	//this->mOrderMode = ORDERTYPE_UNDEFINED;
	this->mTechEvent = MESSAGE_NULL;
	this->mAlienAbility = MESSAGE_NULL;
	this->mGroupEvent = MESSAGE_NULL;
	this->mTrackingEntity = 0;
	this->mNumLocalSelectEvents = 0;

	this->mSelected.clear();
	this->mSelectionJustChanged = false;
	this->mMouseOneDown = false;
	this->mMouseTwoDown = false;
	this->mMouseOneStartX = 0;
	this->mMouseOneStartY = 0;
	this->mMouseTwoStartX = 0;
	this->mMouseTwoStartY = 0;

	this->mMouseCursorX = this->mMouseCursorY = 0;
	this->mPieMenuControl = "";
	
	this->mPreviousHelpText = "";
	this->mTimeLastHelpTextChanged = -1;
	this->mCurrentCursorFrame = 0;

	this->mMapExtents.ResetMapExtents();
	this->mMapName = "";

	this->mGhostBuilding = MESSAGE_NULL;
	this->mValidatedBuilding = MESSAGE_NULL;
	this->mCreatedGhost = false;
	this->mCurrentGhostIsValid = false;

	this->mAmbientSounds.clear();

	if(gViewPort)
	{
		ScorePanel* theScoreBoard = gViewPort->GetScoreBoard();
		if(theScoreBoard)
			theScoreBoard->DeleteCustomIcons();
	}
}


AvHHud::~AvHHud(void)
{
	//this->ResetGamma();
	//delete [] sOriginalGammaTable;
	//delete [] sGammaTable;
	AvHHud::ResetGammaAtExit();
}

void DummyFunction()
{
}

#ifdef DEBUG
int gGlobalDebugAuth = 0;
void TestIcon()
{
	gGlobalDebugAuth = rand() % 7;
}

typedef struct alias_t {
	alias_t* next;
	char name[32];
 	char* cmds;
} alias_s;

void TestAlias()
{
	alias_s* alias = *(alias_s**)0x2D5929C;
	while(alias)
	{
		gEngfuncs.Con_Printf("name: %s\n%x - %x\n", alias->name, alias->name, gEngfuncs);
		alias = alias->next;
	}
}
#endif

// Used for console command
void AvHHud::PlayRandomSongHook()
{
	gHUD.PlayRandomSong();
}

void AvHHud::AddCommands()
{
    gEngfuncs.pfnAddCommand ("+popupmenu", AvHPieMenuHandler::OpenPieMenu);
	gEngfuncs.pfnAddCommand ("-popupmenu", AvHPieMenuHandler::ClosePieMenu);

    gEngfuncs.pfnAddCommand ("+mousepopupmenu", AvHPieMenuHandler::OpenPieMenu);
	gEngfuncs.pfnAddCommand ("-mousepopupmenu", AvHPieMenuHandler::ClosePieMenu);
	
	// Add scrolling commands
    gEngfuncs.pfnAddCommand ("+scrollup", AvHScrollHandler::ScrollUp);
    gEngfuncs.pfnAddCommand ("-scrollup", AvHScrollHandler::StopScroll);
	
    gEngfuncs.pfnAddCommand ("+scrolldown", AvHScrollHandler::ScrollDown);
    gEngfuncs.pfnAddCommand ("-scrolldown", AvHScrollHandler::StopScroll);
	
    gEngfuncs.pfnAddCommand ("+scrollleft", AvHScrollHandler::ScrollLeft);
    gEngfuncs.pfnAddCommand ("-scrollleft", AvHScrollHandler::StopScroll);
	
    gEngfuncs.pfnAddCommand ("+scrollright", AvHScrollHandler::ScrollRight);
    gEngfuncs.pfnAddCommand ("-scrollright", AvHScrollHandler::StopScroll);

    gEngfuncs.pfnAddCommand ("toggleeditps", AvHParticleEditorHandler::ToggleEdit);

    gEngfuncs.pfnAddCommand ("nexttrack", AvHHud::PlayRandomSongHook);

	gEngfuncs.pfnAddCommand ("+showmap", AvHHud::ShowMap);
	gEngfuncs.pfnAddCommand ("-showmap", AvHHud::HideMap);

    gEngfuncs.pfnAddCommand ("playstream", AvHHud::PlayStream);
    gEngfuncs.pfnAddCommand ("stopstream", AvHHud::StopStream);

	#ifdef DEBUG
	gEngfuncs.pfnAddCommand("testicon", TestIcon);
	gEngfuncs.pfnAddCommand("testalias", TestAlias);
	#endif

	int i = 0;
	char theBinding[128];
	for(i = (int)(RESOURCE_UPGRADE); i <= (int)(BUILD_RECYCLE); i++)
	{
		sprintf(theBinding, "%s%d", kHotKeyPrefix, i);
		gEngfuncs.pfnAddCommand(theBinding, DummyFunction);
	}

	for(i = (int)(MENU_BUILD); i <= (int)(MENU_EQUIP); i++)
	{
		sprintf(theBinding, "%s%d", kHotKeyPrefix, i);
		gEngfuncs.pfnAddCommand(theBinding, DummyFunction);
	}
}

void AvHHud::ClientProcessEntity(struct entity_state_s* inEntity)
{
	// Check if we need to create or destroy particle systems
	int theIndex = inEntity->number;
	bool theParticleOn = inEntity->iuser3 == AVH_USER3_PARTICLE_ON;
	bool theParticleOff = inEntity->iuser3 == AVH_USER3_PARTICLE_OFF;
	if(theParticleOn || theParticleOff)
	{
		int theHandle = -1;
		if(theParticleOn)
		{
			// Ent index and template index stored in fuser1
			int theValue = (int)(inEntity->fuser1);
			int theGenEntIndex = (0xFFFF0000 & theValue) >> 16;
			//int theTemplateIndex = (0x0000FFFF & theValue);
			int theTemplateIndex = (((int(inEntity->fuser1)) & 0x0000FF00) >> 8);
			
			//int theTemplateIndex = theValue;
			
			// Handle stored in fuser2
			theHandle = (int)(inEntity->fuser2);

			// Don't create particle systems marked as high-detail if we don't have that option set.  Note, this could cause collision
			// differences between the client and server if the particle system doesn't use this flag with care
			const AvHParticleTemplate* theTemplate = gParticleTemplateList.GetTemplateAtIndex(theTemplateIndex);
			if(theTemplate)
			{
				if(!theTemplate->GetHighDetailOnly() || gEngfuncs.pfnGetCvarFloat(kvHighDetail))
				{
					AvHParticleSystemManager::Instance()->CreateParticleSystemIfNotCreated(inEntity->number, theTemplateIndex, /*theEntIndex,*/ theHandle);
				
					// Update postion and visibility
					if(theGenEntIndex > 0)
					{
						cl_entity_s* theGenEntity = gEngfuncs.GetEntityByIndex(theGenEntIndex);
						if(theGenEntity)
						{
							AvHParticleSystemManager::Instance()->SetParticleSystemGenerationEntityExtents(theGenEntity->curstate.mins, theGenEntity->curstate.maxs, theHandle);
						}
					}
					else
					{
						AvHParticleSystemManager::Instance()->SetParticleSystemPosition(inEntity->origin, theHandle);
					}
					
					// Set the particle system custom data
					//uint16 theCustomData = (uint16)(((int)inEntity->fuser3) >> 16);
					//uint16 theCustomData = (uint16)(inEntity->fuser3);
					uint16 theCustomData = (uint16)(inEntity->weaponmodel);
					AvHParticleSystemManager::Instance()->SetParticleSystemCustomData(theCustomData, theHandle);
				}
			}
		}
		else if(theParticleOff)
		{
			theHandle = (int)(inEntity->fuser1);
			//AvHParticleSystemManager::Instance()->DestroyParticleSystemIfNotDestroyed(inEntity->number, theHandle);
			AvHParticleSystemManager::Instance()->MarkParticleSystemForDeletion(inEntity->number, theHandle);
		}

		// Always update visibility
		bool theVisibilityState = false;
		if(this->GetInTopDownMode())
		{
			if(GetHasUpgrade(inEntity->iuser4, MASK_VIS_SIGHTED))
			{
				theVisibilityState = true;
			}
			else
			{
				theVisibilityState = false;
			}
		}
		else
		{
			theVisibilityState = true;
		}
		
		AvHParticleSystemManager::Instance()->SetParticleSystemVisibility(theVisibilityState, theHandle);
	}
	else if((inEntity->iuser3 == AVH_USER3_AUDIO_ON) || (inEntity->iuser3 == AVH_USER3_AUDIO_OFF))
	{
		// Read values
		int theEntIndex = (int)(inEntity->fuser1) >> 16;
		//int theSoundIndex = (int)(inEntity->fuser1) & 0x0000FFFF;

		// memcpy so value isn't interpreted
		//int theSoundIndex = 0;
		//memcpy(&theSoundIndex, &inEntity->fuser1, sizeof(float));
		//theSoundIndex = theSoundIndex & 0x0000FFFF;
		int theSoundIndex = (((int(inEntity->fuser1)) & 0x0000FF00) >> 8);

		// Top byte is flags, next byte is volume, bottom two bytes are fade distance
		int theFlags = inEntity->iuser4 >> 24;
		int theVolume = (inEntity->iuser4 >> 16) & 0x00FF;
		int theFadeDistance = (inEntity->iuser4) & 0x0000FFFF;

		float theTimeOfAction = inEntity->fuser2;
		
		bool theSoundOn = (inEntity->iuser3 == AVH_USER3_AUDIO_ON);

		this->ModifyAmbientSoundEntryIfChanged(theSoundOn, theSoundIndex, theEntIndex, theTimeOfAction, theVolume, theFadeDistance, theFlags, inEntity->origin);
	}
}

string LookupAndTranslate(AvHMessageID inMessageID)
{
	string theKey = string(kTechNodeLabelPrefix) + MakeStringFromInt((int)inMessageID);

	string theTranslatedTechName;
	LocalizeString(theKey.c_str(), theTranslatedTechName);

	return theTranslatedTechName;
}

void AvHHud::DisplayCombatUpgradeMenu(bool inVisible)
{
	if(inVisible)
	{
		// Parse current tech nodes and set text
		const AvHTechID kLineStart[kNumUpgradeLines] = {TECH_ONE_LEVEL_ONE, TECH_TWO_LEVEL_ONE, TECH_THREE_LEVEL_ONE, TECH_FOUR_LEVEL_ONE, TECH_FIVE_LEVEL_ONE};

		// Add "you are now x"!
		string theYouAreNow;
		LocalizeString(kYouAreNowA, theYouAreNow);

		string theRankTitle = this->GetRankTitle(false);

		string theExclamation;
		LocalizeString(kExclamation, theExclamation);

		string theChooseAnUpgrade;
		LocalizeString(kChooseAnUpgrade, theChooseAnUpgrade);

		string theFinalText = theYouAreNow + string(" ") + theRankTitle + theExclamation + string("\n");
		theFinalText += theChooseAnUpgrade + string("\n\n");
		
		// Set the lines
		this->mCombatUpgradeMenu.SetText(theFinalText);
	}

	// Parse text above every time, but only set position once so it doesn't keep animating
	if(inVisible && !this->mDrawCombatUpgradeMenu)
	{
		// Start off screen, and scroll right
		const float kWidth = .4f;
		this->mCombatUpgradeMenu.SetNormalizedScreenX(-kWidth);
		this->mCombatUpgradeMenu.SetNormalizedScreenY(.25f);
		this->mCombatUpgradeMenu.SetNormalizedMaxWidth(kWidth);
	}

	this->mDrawCombatUpgradeMenu = inVisible;
}

void AvHHud::DisplayMessage(const char* inMessage)
{
	this->m_Message.MessageAdd(inMessage, this->m_flTime);

	// Remember the time -- to fix up level transitions
	//this->m_Message.m_parms.time = this->m_flTime;

	// Turn on drawing
	if ( !(this->m_Message.m_iFlags & HUD_ACTIVE) )
		this->m_Message.m_iFlags |= HUD_ACTIVE;
}

//int	AvHHud::GetArmorLevel(void) const
//{
//	return this->mArmorLevel;
//}

int	AvHHud::GetFrameForOrderType(AvHOrderType inOrderType) const
{
	int theFrame = 0;
	
	switch(inOrderType)
	{
	case ORDERTYPEL_DEFAULT:
		theFrame = 2;
		break;

	case ORDERTYPEL_MOVE:
		theFrame = 2;
		break;

	case ORDERTYPET_ATTACK:
		theFrame = 4;
		break;

	case ORDERTYPET_BUILD:
		theFrame = 5;
		break;

	case ORDERTYPET_GUARD:
		theFrame = 6;
		break;

	case ORDERTYPET_WELD:
		theFrame = 7;
		break;

	case ORDERTYPET_GET:
		theFrame = 8;
		break;
	}

	return theFrame;
}

AvHPlayMode	AvHHud::GetPlayMode(void) const
{
	AvHPlayMode thePlayMode = PLAYMODE_UNDEFINED;

	cl_entity_s* thePlayer = gEngfuncs.GetLocalPlayer();
	if(thePlayer)
	{
        if(gEngfuncs.IsSpectateOnly())
        {
            thePlayMode = PLAYMODE_OBSERVER;
        }
        else
        {
            thePlayMode = AvHPlayMode(thePlayer->curstate.playerclass);
        }
	}

	return thePlayMode;
}

AvHPlayMode	AvHHud::GetHUDPlayMode() const
{
	AvHPlayMode thePlayMode = this->GetPlayMode();
	
	cl_entity_s* thePlayer = this->GetVisiblePlayer();
	if(thePlayer)
	{
		thePlayMode = AvHPlayMode(thePlayer->curstate.playerclass);
	}
	
	return thePlayMode;
}

cl_entity_s* AvHHud::GetVisiblePlayer() const
{
	cl_entity_s* thePlayer = gEngfuncs.GetLocalPlayer();
	if(g_iUser1 == OBS_IN_EYE)
	{
		cl_entity_t* theEnt = gEngfuncs.GetEntityByIndex(g_iUser2);
		if(theEnt)
		{
			thePlayer = theEnt;
		}
	}

	return thePlayer;
}

int	AvHHud::GetLocalUpgrades() const
{
	static int theUpgrades = 0;
	
	cl_entity_s* thePlayer = this->GetVisiblePlayer();
	if(thePlayer)
	{
		theUpgrades = thePlayer->curstate.iuser4;
	}

	return theUpgrades;
}

// Players could hack their client dll and see all the orders on their team.  Minor cheat but definitely possible.
EntityListType AvHHud::GetDrawPlayerOrders() const
{
	EntityListType theList;

	cl_entity_s* theVisiblePlayer = this->GetVisiblePlayer();
	if(theVisiblePlayer)
	{
		int theVisiblePlayerIndex = theVisiblePlayer->index;

		if(this->GetHUDUser3() == AVH_USER3_MARINE_PLAYER)
		{
			// Only draw orders for us
			theList.push_back(theVisiblePlayerIndex);
		}
		else if(this->GetHUDUser3() == AVH_USER3_COMMANDER_PLAYER)
		{
			// Add everyone that he has selected!
			return this->mSelected;
		}
	}
	
	return theList;
}

bool AvHHud::GetInTopDownMode() const
{
	return this->mInTopDownMode;
}

bool AvHHud::GetIsSelecting() const
{
    return mSelectionBoxVisible;
}

OrderListType AvHHud::GetOrderList() const
{
	return this->mOrders;
}

//AvHOrderType AvHHud::GetOrderMode() const
//{
//	return this->mOrderMode;
//}

bool AvHHud::GetCenterPositionForGroup(int inGroupNumber, vec3_t& outCenterPosition) const
{
	bool theSuccess = false;
	
	if((inGroupNumber >= 1) && (inGroupNumber <= kNumHotkeyGroups))
	{
		vec3_t theCenterPosition;
		VectorClear(theCenterPosition);

		int theNumFound = 0;

		const EntityListType& theGroup = this->mGroups[inGroupNumber - 1];
		if(theGroup.size() > 0)
		{
			for(EntityListType::const_iterator theIter = theGroup.begin(); theIter != theGroup.end(); theIter++)
			{
				int theEntIndex = *theIter;
					
				Vector thePosition;
				cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(theEntIndex);
				if(theEntity)
				{
					thePosition = theEntity->curstate.origin;
				}
				
				if(AvHSHUGetEntityLocation(theEntIndex, thePosition))
				{
					theCenterPosition.x += thePosition.x;
					theCenterPosition.y += thePosition.y;
					theNumFound++;
				}
			}

			if(theNumFound > 0)
			{
				theCenterPosition.x /= theNumFound;
				theCenterPosition.y /= theNumFound;
				
				outCenterPosition = theCenterPosition;
				
				theSuccess = true;
			}
		}
	}

	return theSuccess;
}

void AvHHud::GetMousePos(int& outX, int& outY) const
{
	gEngfuncs.GetMousePosition(&outX, &outY);
	
	// Clip mouse to window (weird)
	outX = min(max(0, outX), ScreenWidth());
	outY = min(max(0, outY), ScreenHeight());
	
	//char theMouseMessage[256];
	//sprintf(theMouseMessage, "Mouse coords: %d, %d", outX, outY);
	//CenterPrint(theMouseMessage);
}

bool AvHHud::GetAndClearTopDownScrollAmount(int& outX, int& outY, int& outZ)
{
	bool theSuccess = false;
	outX = 0;
	outY = 0;
	outZ = 0;
	
    // Don't scroll if the the commander is dragging a selection box.
    
    if(this->GetInTopDownMode() && !GetIsSelecting())
	{
		const int kScreenWidth = ScreenWidth();
		const int kScreenHeight = ScreenHeight();
		const kScrollHorizontal = .0152f*kScreenWidth;
		const kScrollVertical = .015f*kScreenHeight;

		// Left side
		if(this->GetIsMouseInRegion(0, 0, kScrollHorizontal, kScreenHeight) || (gScrollHandler.GetXScroll() < 0))
		{
			outX = -1;
		}
		// Right side
		else if(this->GetIsMouseInRegion(kScreenWidth - kScrollHorizontal, 0, kScrollHorizontal, kScreenHeight) || (gScrollHandler.GetXScroll() > 0))
		{
			outX = 1;
		}

		// Top edge
		if(this->GetIsMouseInRegion(0, 0, kScreenWidth, kScrollVertical) || (gScrollHandler.GetYScroll() > 0))
		{
			outY = 1;
		}
		// Bottom edge
		else if(this->GetIsMouseInRegion(0, kScreenHeight - kScrollVertical, kScreenWidth, kScrollVertical) || (gScrollHandler.GetYScroll() < 0))
		{
			outY = -1;
		}

		// Only clear z scroll because of the way events work (invnext/invprev vs. holding a key down)
		//gScrollHandler.ClearScrollHeight();
		
		theSuccess = true;
	}
	
	return theSuccess;
}

bool AvHHud::GetAndClearSelectionEvent(vec3_t& outSelection, AvHMessageID& outMessageID)
{
	bool theSuccess = false;

	// Return a build event if there is one, else return a COMMANDER_MOUSECOORD event
	if(this->mLeftMouseStarted)
	{
		if(this->mValidatedBuilding == MESSAGE_NULL)
		{
			VectorCopy(this->mLeftMouseWorldStart, outSelection);
			outMessageID = COMMANDER_MOUSECOORD;
		}
		else
		{
			VectorCopy(this->mNormBuildLocation, outSelection);
			outMessageID = this->mValidatedBuilding;
			this->mValidatedBuilding = this->mGhostBuilding = MESSAGE_NULL;
			this->mPlacingBuilding = true;
		}
		theSuccess = true;
	}
	else if(this->mLeftMouseEnded)
	{
		if(!this->mPlacingBuilding)
		{
			outSelection = this->mLeftMouseWorldEnd;
			outMessageID = COMMANDER_MOUSECOORD;
			theSuccess = true;
		}
		this->mPlacingBuilding = false;
	}
	else if(this->mRightMouseStarted)
	{
		// Cancel building placement
		if(this->mGhostBuilding != MESSAGE_NULL)
		{
            CancelBuilding();
		}
		else
		{
			outSelection = this->mRightMouseWorldStart;
			outMessageID = COMMANDER_MOUSECOORD;
			theSuccess = true;
		}
	}
	else if(this->mRightMouseEnded)
	{
		outSelection = this->mRightMouseWorldEnd;
		outMessageID = COMMANDER_MOUSECOORD;
		theSuccess = true;
	}
	else
	{
		outSelection = this->mMouseWorldPosition;
		outMessageID = COMMANDER_MOUSECOORD;
		theSuccess = true;
	}
	
	return theSuccess;
}

EntityListType AvHHud::GetSelected() const
{
	return this->mSelected;
}

const AvHTechSlotManager& AvHHud::GetTechSlotManager() const
{
	return this->mTechSlotManager;
}

bool AvHHud::GetAndClearAlienAbility(AvHMessageID& outMessageID)
{
	bool theAlienAbilityWaiting = false;
	
	if(this->mAlienAbility != MESSAGE_NULL)
	{
		outMessageID = this->mAlienAbility;
		theAlienAbilityWaiting = true;
		this->mAlienAbility = MESSAGE_NULL;
	}
	
	return theAlienAbilityWaiting;
}

bool AvHHud::GetAndClearGroupEvent(AvHMessageID& outMessageID)
{
	bool theGroupEventWaiting = false;
	
	if(this->mGroupEvent != MESSAGE_NULL)
	{
		outMessageID = this->mGroupEvent;
		theGroupEventWaiting = true;
		
//		if(!this->mIsTracking)
//		{
			this->mGroupEvent = MESSAGE_NULL;
//		}
	}
	
	return theGroupEventWaiting;
}

int	AvHHud::GetTrackingEntity() const
{
	return this->mTrackingEntity;
}

void AvHHud::ClearTrackingEntity()
{
	this->mTrackingEntity = 0;
}

void AvHHud::SetSelectionEffects(EntityListType& inUnitList)
{
	// Make sure we have an effect created for each unit in this list.  If there are units that
	// have selection effects that aren't in this list, delete them.  This is called locally when the
	// selection is predicted, then it's called again when the selection is confirmed.
	this->mSelectionEffects.clear();
	
	for(EntityListType::iterator theIter = inUnitList.begin(); theIter != inUnitList.end(); theIter++)
	{
		SelectionEffect theNewEffect;
		theNewEffect.mEntIndex = *theIter;
		theNewEffect.mAngleOffset = 0;
		this->mSelectionEffects.push_back(theNewEffect);
	}
}

UIMode AvHHud::GetUIMode() const
{
	return this->mCurrentUIMode;
}

bool AvHHud::SwitchUIMode(UIMode inNewMode)
{
	bool theSuccess = false;

	// Only allow switching to a non-main mode when we're in main, always allow switching back to main mode
	if((inNewMode == MAIN_MODE) || (this->mCurrentUIMode == MAIN_MODE))
	{
		if(inNewMode != this->mCurrentUIMode)
		{
			// Move pop-up menu components away or back so they don't block other compoments...ugh
			if(inNewMode != MAIN_MODE)
			{
				gHUD.GetManager().TranslateComponent(kSoldierMenu, true);
				gHUD.GetManager().TranslateComponent(kSoldierCombatMenu, true);
				gHUD.GetManager().TranslateComponent(kAlienMenu, true);
				gHUD.GetManager().TranslateComponent(kAlienCombatMenu, true);
				gHUD.GetManager().TranslateComponent(kAlienMembrane, true);
				gHUD.GetManager().TranslateComponent(kScroller, true);
				gHUD.GetManager().TranslateComponent(kSelectionText, true);
				//CenterPrint("Pop-up controls moved off screen");
			}
			else
			{
				gHUD.GetManager().TranslateComponent(kSoldierMenu, false);
				gHUD.GetManager().TranslateComponent(kSoldierCombatMenu, false);
				gHUD.GetManager().TranslateComponent(kAlienMenu, false);
				gHUD.GetManager().TranslateComponent(kAlienCombatMenu, false);
				gHUD.GetManager().TranslateComponent(kAlienMembrane, false);
				gHUD.GetManager().TranslateComponent(kScroller, false);
				gHUD.GetManager().TranslateComponent(kSelectionText, false);
				//CenterPrint("Pop-up controls moved on screen");
			}

			this->mCurrentUIMode = inNewMode;
		}

		theSuccess = true;
	}

	return theSuccess;
}

bool AvHHud::Update(float inCurrentTime, string& outErrorString)
{
    bool theSuccess = false;

	if(inCurrentTime > this->mTimeOfLastUpdate)
	{

        this->mTimeOfCurrentUpdate = inCurrentTime;

		// Predict game time
		if(this->GetGameStarted())
		{
			this->mGameTime += (inCurrentTime - this->mTimeOfLastUpdate);
		}

		AvHParticleSystemManager::Instance()->Start();

		// This component must always be visible to allow us to hook mouse cursor sprite drawing
		this->ResetComponentsForUser3();

		this->UpdateDataFromVuser4(inCurrentTime);

		this->UpdateSpectating();

		this->GetManager().UnhideComponent(kLastComponent);

		this->UpdateDemoRecordPlayback();
		
		theSuccess = UIHud::Update(inCurrentTime, outErrorString);
        if(!theSuccess)
        {
            this->AddTooltip(outErrorString.c_str());
        }

		this->UpdateProgressBar();

		this->UpdateCommonUI();

		this->UpdateAlienUI(inCurrentTime);

		this->UpdateMarineUI(inCurrentTime);
		
		this->UpdateCountdown(inCurrentTime);

		this->UpdateExploitPrevention();
		this->UpdateFromEntities(inCurrentTime);
		
		this->UpdateEntityID(inCurrentTime);

		this->UpdateHelpText();

		this->UpdateTooltips(inCurrentTime);

		this->UpdateStructureNotification(inCurrentTime);

		this->UpdateMusic(inCurrentTime);

		this->UpdatePieMenuControl();

		// Reset cursor every tick, update selection may change it
		// This cursor is used when we're on the ground for pie menus as well
		this->SetCursor(ORDERTYPE_UNDEFINED);

		this->UpdateHierarchy();

		this->UpdateInfoLocation();

		if(this->GetInTopDownMode())
		{
			this->UpdateSelection();
			gCommanderHandler.Update(this->mTechNodes, this->mResources);

//			char theDebugString[128];
//			sprintf(theDebugString, "norm X/Y: %f, %f", (float)this->mMouseCursorX/ScreenWidth, (float)this->mMouseCursorY/ScreenHeight);
//			CenterPrint(theDebugString);
		}
		else
		{
			this->ResetTopDownUI();
		}
		
		// Update orders
		//for(OrderListType::iterator theIter = this->mOrders.begin(); theIter != this->mOrders.end(); theIter++)
		//{
		//	theIter->Update();
		//}
		
		this->UpdateTechNodes();
		
		this->UpdateAmbientSounds();

		this->UpdateViewModelEffects();

        mOverviewMap.UpdateOrders(mOrders, GetDrawPlayerOrders());
        mOverviewMap.Update(inCurrentTime);
		
		float theTimePassed = inCurrentTime - this->mTimeOfLastUpdate;
		
		AvHParticleSystemManager::Instance()->Update(theTimePassed);

		AvHScriptManager::Instance()->ClientUpdate(theTimePassed);

		this->UpdateResources(theTimePassed);

		if((this->GetHUDPlayMode() == PLAYMODE_PLAYING) && !this->GetIsAlive())
		{
			this->mCommanderResourceLabel->setVisible(false);
			this->mHierarchy->setVisible(false);
			this->mShowMapHierarchy->setVisible(false);
		}

		if(cl_particleinfo->value)
		{
			char theDebugText[128];
			int theNumVisible = AvHParticleSystemManager::Instance()->GetNumVisibleParticleSystems();
			int theNum = AvHParticleSystemManager::Instance()->GetNumberParticleSystems();
			int theNumTemplates = gParticleTemplateList.GetNumberTemplates();
			sprintf(theDebugText, "(vis, total, list): %d, %d, %d", theNumVisible, theNum, theNumTemplates);
			
			//sprintf(theDebugText, "step interval: %d", pmove->flTimeStepSound);

            /*
			if(this->mMarineResourceLabel)
			{
				this->mMarineResourceLabel->setText(theDebugText);
				this->mMarineResourceLabel->setVisible(true);
			}
            */

		}

		if(!gEngfuncs.pDemoAPI->IsPlayingback())
		{
			IN_GetMousePos(&this->mMouseCursorX, &this->mMouseCursorY);
		}

		// Update user3 and team
		this->mLastUser3 = this->GetHUDUser3();
		this->mLastTeamNumber = this->GetHUDTeam();
		this->mLastPlayMode = this->GetPlayMode();
		
		this->mTimeOfLastUpdate = inCurrentTime;
		
		// Save view origin and angles before we do crazy viewport stuff
		//		gPlayerOrigin = v_origin;
		//		gPlayerAngles = v_angles;
	}

    return theSuccess;
}

//void AvHHud::UpdateSelectionEffects(float inTimePassed)
//{
//	// Radians/sec
//	const float kSpinRate = 1.5f;
//	for(SelectionListType::iterator theIter = this->mSelectionEffects.begin(); theIter != this->mSelectionEffects.end(); theIter++)
//	{
//		theIter->mAngleOffset = (theIter->mAngleOffset += inTimePassed*kSpinRate) % 360;
//	}
//}

bool AvHHud::GetAndClearTechEvent(AvHMessageID& outMessageID)
{
	bool theTechEventWaiting = false;
	
	if(this->mTechEvent != MESSAGE_NULL)
	{
		outMessageID = this->mTechEvent;
		theTechEventWaiting = true;
		this->mTechEvent = MESSAGE_NULL;
	}
	
	return theTechEventWaiting;
}

bool AvHHud::GetLastHotkeySelectionEvent(AvHMessageID& outMessageID)
{
	bool theSuccess = false;

	switch(this->mLastHotkeySelectionEvent)
	{
	case GROUP_SELECT_1:
	case GROUP_SELECT_2:
	case GROUP_SELECT_3:
	case GROUP_SELECT_4:
	case GROUP_SELECT_5:
		outMessageID = this->mLastHotkeySelectionEvent;
		theSuccess = true;
		break;
	}

	return theSuccess;
}

void AvHHud::SetLastHotkeySelectionEvent(AvHMessageID inMessageID)
{
	switch(inMessageID)
	{
	case MESSAGE_NULL:
	case GROUP_CREATE_1:
	case GROUP_CREATE_2:
	case GROUP_CREATE_3:
	case GROUP_CREATE_4:
	case GROUP_CREATE_5:
	case GROUP_SELECT_1:
	case GROUP_SELECT_2:
	case GROUP_SELECT_3:
	case GROUP_SELECT_4:
	case GROUP_SELECT_5:
	case COMMANDER_REMOVESELECTION:
		this->mLastHotkeySelectionEvent = inMessageID;
		break;

	default:
		ASSERT(false);
		break;
	}
}

bool AvHHud::GetIsAlien() const
{
	bool theIsAlien = false;
	
	AvHUser3 theUser3 = this->GetHUDUser3();
	
	switch(theUser3)
	{
	case AVH_USER3_ALIEN_PLAYER1:
	case AVH_USER3_ALIEN_PLAYER2:
	case AVH_USER3_ALIEN_PLAYER3:
	case AVH_USER3_ALIEN_PLAYER4:
	case AVH_USER3_ALIEN_PLAYER5:
	case AVH_USER3_ALIEN_EMBRYO:
		theIsAlien = true;
		break;
	}
	
	return theIsAlien;
}

bool AvHHud::GetIsBeingDigested() const
{
	bool theIsBeingDigested = false;

	int theUpgrades = this->GetHUDUpgrades();
		
	if(GetHasUpgrade(theUpgrades, MASK_DIGESTING))
	{
		cl_entity_t* theVisiblePlayer = this->GetVisiblePlayer();
		if(theVisiblePlayer && (theVisiblePlayer->curstate.effects & EF_NODRAW))
		{
			theIsBeingDigested = true;
		}
	}

	return theIsBeingDigested;
}

bool AvHHud::GetIsEnsnared() const
{
	int theUpgrades = this->GetHUDUpgrades();
	return GetHasUpgrade(theUpgrades, MASK_ENSNARED);
}

bool AvHHud::GetIsStunned() const
{
    int theUpgrades = this->GetHUDUpgrades();
    return GetHasUpgrade(theUpgrades, MASK_PLAYER_STUNNED);
}

bool AvHHud::GetIsDigesting() const
{
	bool theIsDigesting = false;
	
	int theUpgrades = this->GetHUDUpgrades();
	
	if(GetHasUpgrade(theUpgrades, MASK_DIGESTING))
	{
		cl_entity_t* theVisiblePlayer = this->GetVisiblePlayer();
		if(theVisiblePlayer && !(theVisiblePlayer->curstate.effects & EF_NODRAW))
		{
			theIsDigesting = true;
		}
	}
	
	return theIsDigesting;
}

bool AvHHud::GetIsNotInControl() const
{
    return GetIsBeingDigested() || !IEngineStudio.IsHardware();
}

bool AvHHud::GetIsInTopDownMode() const
{
	bool theIsInTopDownMode = false;
	
	if(GetHasUpgrade(this->GetHUDUpgrades(), MASK_TOPDOWN))
	{
		theIsInTopDownMode = true;
	}
	
	return theIsInTopDownMode;
}

int AvHHud::GetCommanderIndex() const
{
    int theCommanderIndex = -1;

    for(int i = 1; i <= MAX_PLAYERS; i++)
    {
        extra_player_info_t* theExtraPlayerInfo = &g_PlayerExtraInfo[i];
        ASSERT(theExtraPlayerInfo);
        int thePlayerClass = theExtraPlayerInfo->playerclass;
        if(thePlayerClass == PLAYERCLASS_COMMANDER)
        {
            theCommanderIndex = i;
            break;
        }
    }

    return theCommanderIndex;
}


bool AvHHud::GetHasJetpack() const
{
	int theLocalUpgrades = this->GetHUDUpgrades();
	bool theHasJetpackUpgrade = GetHasUpgrade(theLocalUpgrades, MASK_UPGRADE_7) && this->GetIsMarine();
	
	return theHasJetpackUpgrade;
}

bool AvHHud::GetHasAlienUpgradesAvailable() const
{
	bool theHasUpgradesAvailable = false;

	if(this->GetIsAlien() && this->GetIsRelevant() && !this->GetIsBeingDigested())
	{
		int theUpgradeVar = this->GetLocalUpgrades();
		bool theHasDefensiveUpgradesAvailable = AvHGetHasUpgradeChoiceInCategory(ALIEN_UPGRADE_CATEGORY_DEFENSE, this->mUpgrades, theUpgradeVar);
		bool theHasMovementUpgradesAvailable = AvHGetHasUpgradeChoiceInCategory(ALIEN_UPGRADE_CATEGORY_MOVEMENT, this->mUpgrades, theUpgradeVar);
		bool theHasSensoryUpgradesAvailable = AvHGetHasUpgradeChoiceInCategory(ALIEN_UPGRADE_CATEGORY_SENSORY, this->mUpgrades, theUpgradeVar);

		theHasUpgradesAvailable = theHasDefensiveUpgradesAvailable || theHasMovementUpgradesAvailable || theHasSensoryUpgradesAvailable;
	}

	return theHasUpgradesAvailable;
}

bool AvHHud::GetIsMarine() const
{
	bool theIsMarine = false;

	AvHUser3 theUser3 = this->GetHUDUser3();

	switch(theUser3)
	{
	case AVH_USER3_MARINE_PLAYER:
	case AVH_USER3_COMMANDER_PLAYER:
		theIsMarine = true;
		break;
	}
	
	return theIsMarine;
}

bool AvHHud::GetIsRelevant() const
{
	bool theIsRelevant = false;
	
	if(this->GetIsAlive() && (this->GetPlayMode() == PLAYMODE_PLAYING) /*&& !this->GetIsSpectator()*/)
	{
		theIsRelevant = true;
	}
	
	return theIsRelevant;
}

vec3_t AvHHud::GetVisualOrigin() const
{
	vec3_t theVisualOrigin = gPredictedPlayerOrigin;

	theVisualOrigin.z += gPredictedPlayerVOfs[2];

	return theVisualOrigin;
}

AvHMessageID AvHHud::HotKeyHit(char inChar)
{
	return gCommanderHandler.HotKeyHit(inChar);
}

float AvHHud::GetGammaSlope() const
{
	return sGameGammaTable.GetGammaSlope();
}

string AvHHud::GetMapName(bool inLocalOnly) const
{
    string theMapName = this->mMapName;

    if((theMapName == "") && !inLocalOnly )
    {
        const char* theLevelName = gEngfuncs.pfnGetLevelName();
        if(theLevelName)
        {
            theMapName = string(theLevelName);

            // Remove maps/ from the beginning and .bsp from the end.
            StringVector theVector;
            Tokenizer::split(theMapName, "/.", theVector);
            if(theVector.size() >= 2)
            {
                theMapName = theVector[1];
            }
        }
    }

	return theMapName;
}

int AvHHud::GetNumActiveHives() const
{
	int theNumActiveHives = 0;

	if(this->GetIsAlien())
	{
		for(HiveInfoListType::const_iterator theIter = this->mHiveInfoList.begin(); theIter != this->mHiveInfoList.end(); theIter++)
		{
			if(theIter->mStatus == kHiveInfoStatusBuilt)
			{
				theNumActiveHives++;
			}
		}
	}

	return theNumActiveHives;
}

int AvHHud::GetMaxAlienResources() const
{
	int theMaxAlienResources = kMaxAlienResources;

	if(this->mMaxResources >= 0)
	{
		theMaxAlienResources = this->mMaxResources;
	}

	return theMaxAlienResources;
}

//void AvHHud::InitializeGammaTable()
//{
//	for(int j = 0; j < 3; j++)
//	{
//		for(int i = 0; i < 256; i++)
//		{
//			int theBaseOffset = j*256 + i;
//			uint8 theNewColor = (uint8)i;
//			uint16 theNewWord = theNewColor << 8;
//			((uint16*)sOriginalGammaTable)[theBaseOffset] = theNewWord;
//		}
//	}
//}
//
//void AvHHud::ProcessGammaTable(float inSlope)
//{
//	// Steepen and saturate, ala Q3
//	uint8 kMaxValue = uint8(-1);
//	for(int j = 0; j < 3; j++)
//	{
//		for(int i = 0; i < 256; i++)
//		{
//			int theBaseOffset = j*256 + i;
//			uint16 theWord = ((uint16*)sGammaTable)[theBaseOffset];
//			uint8 theBaseColor = (theWord & 0xFF00) >> 8;
//			uint8 theNewColor = min(theBaseColor*inSlope, kMaxValue);
//			uint16 theNewWord = theNewColor << 8;
//			((uint16*)sGammaTable)[theBaseOffset] = theNewWord;
//		}
//	}
//}

//void AvHHud::ToggleGamma(void)
//{
//	AvHHud::ResetGamma();
//
//	if(!sGammaAltered)
//	{
//        float theGammaToTry = 2.05f;
//        bool theSuccess = false;
//        while(!theSuccess && (theGammaToTry > 1.0f))
//        {
//            theGammaToTry -= .05f;
//            theSuccess = AvHHud::SetGamma(theGammaToTry);
//        }
//
//        char theMessage[256];
//        if(theSuccess)
//        {
//            sprintf(theMessage, "Gamma set to %f.", theGammaToTry);
//			gNewGammaSlope = theGammaToTry;
//        }
//        else
//        {
//            sprintf(theMessage, "This display device doesn't appear to support downloadable gamma ramps.");
//        }
//        CenterPrint(theMessage);
//	}
//	else
//	{
//		sGammaAltered = false;
//	}
//}

bool AvHHud::SetGamma(float inSlope)
{
    bool theSuccess = false;

	// Disable gamma stuff in debug for sanity
//	#ifndef DEBUG
	
	HDC theDC = GetDC(NULL);
	if(theDC != 0)
	{
		const float kGammaIncrement = 0.05f;
		float theGammaToTry = inSlope + kGammaIncrement;
		while(!theSuccess && (theGammaToTry > 1.0f))
		{
			theGammaToTry -= kGammaIncrement;

			sGameGammaTable.ProcessSlope(theGammaToTry);
			if(sGameGammaTable.InitializeToVideoState())
			{
				// Tell UI components so they can change shading to look the same
				this->GetManager().NotifyGammaChange(theGammaToTry);
				
				//float theColorBias = 1/this->mGammaSlope;
				//
				//if(this->mSoldierMenu)
				//{
				//	this->mSoldierMenu->SetColorBias(theColorBias);
				//}
				//
				//if(this->mAlienMenu)
				//{
				//	this->mAlienMenu->SetColorBias(theColorBias);
				//}
				
				// aww yeah
				theSuccess = true;
			}
		}
		
		char theMessage[256];
		if(theSuccess)
		{
			sprintf(theMessage, "Gamma set to %f.", theGammaToTry);
		}
		else
		{
			sprintf(theMessage, "Display doesn't support downloadable gamma ramps.");
		}

		if(!theSuccess || (gEngfuncs.GetMaxClients() == 1))
		{
			CenterPrint(theMessage);
		}
		
		if(!ReleaseDC(NULL, theDC))
		{
			// emit error about leak
		}
	}

	//#endif
	
    return theSuccess;
}

bool AvHHud::SlotInput(int inSlot)
{
	bool theHandled = false;

	if(this->mInTopDownMode)
	{
		if((inSlot >= 0) && (inSlot < kNumHotkeyGroups))
		{
			// TODO: Read state of control/duck here
			bool theCreateGroup = false;
			
			int theButtonBits = CL_ButtonBits(0);
			if(theButtonBits & IN_DUCK)
			{
				theCreateGroup = true;
			
				//#ifdef AVH_PREDICT_SELECT
				//this->PlayHUDSound(AvHHUDSound(HUD_SOUND_SQUAD1 + inSlot));
				//#endif
			}
			
			int theBaseOffset = theCreateGroup ? GROUP_CREATE_1 : GROUP_SELECT_1;
			
			this->mGroupEvent = (AvHMessageID)(theBaseOffset + inSlot);
			
			theHandled = true;
		}
	}
//	else if(this->mDrawCombatUpgradeMenu)
//	{
//		if((inSlot >= 0) && (inSlot < kNumUpgradeLines))
//		{
//			AvHMessageID theImpulse = this->mMenuImpulses[inSlot];
//			if(theImpulse > 0)
//			{
//				// Make menu go away
//				char szbuf[32];
//				sprintf(szbuf, "impulse %d\n", theImpulse);
//				ClientCmd(szbuf);
//				
//				theHandled = true;
//			}
//		}
//	}

	return theHandled;
}

int AvHHud::Redraw( float flTime, int intermission )
{

	if (!gViewPort->IsOptionsMenuVisible() &&
        !gParticleEditorHandler.GetInEditMode())
	{
		Render();
	}

    int theRC = UIHud::Redraw(flTime, intermission);

    return theRC;
}

void AvHHud::ResetGammaAtExit()
{
	sPregameGammaTable.InitializeToVideoState();
}

int AvHHud::ResetGammaAtExitForOnExit()
{
	sPregameGammaTable.InitializeToVideoState();
	return TRUE;
}

void AvHHud::ResetGammaAtExit(int inSig)
{
	AvHHud::ResetGammaAtExit();
}

void AvHHud::ResetTopDownUI()
{
	this->mGhostBuilding = MESSAGE_NULL;
	this->mSelected.clear();
	this->mSelectionEffects.clear();
	
	gCommanderHandler.Reset();

	for(int i = 0; i < kNumHotkeyGroups; i++)
	{
		this->mGroups[i].clear();
		this->mGroupTypes[i] = AVH_USER3_NONE;
		this->mGroupAlerts[i] = ALERT_NONE;
	}

	this->mSelectAllGroup.clear();
}

void AvHHud::SetSelectingWeaponID(int inWeaponID, int inR, int inG, int inB)
{
	if(gEngfuncs.pfnGetCvarFloat(kvAutoHelp))
	{
		if(inR != -1)
		{
			this->mHelpMessage.SetR(inR);
		}
		if(inG != -1)
		{
			this->mHelpMessage.SetG(inG);
		}
		if(inB != -1)
		{
			this->mHelpMessage.SetB(inB);
		}
		
		this->mSelectingWeaponID = inWeaponID;
	}
}

void AvHHud::SetTechHelpText(const string& inTechHelpText)
{
	this->mTechHelpText = inTechHelpText;
}

BIND_MESSAGE(Countdown);
int AvHHud::Countdown(const char* pszName, int iSize, void* pbuf)
{
	BEGIN_READ(pbuf, iSize);
	byte theNumSecondsUntilStart = READ_BYTE();

	this->mNumTicksToPlay = theNumSecondsUntilStart;
	this->mLastTickPlayed = 1;
	this->mCountDownClock = this->m_flTime;

	return 1;
}

//BIND_MESSAGE(ChangeNode);
//int	AvHHud::ChangeNode(const char* pszName, int iSize, void* pbuf)
//{
//	BEGIN_READ(pbuf, iSize);
//    int theBytesRead = 1;
//
//	// Read new armor level
//	this->mArmorLevel = READ_BYTE();
//
//	// Read old message id
//	int theOldMessageID = READ_BYTE();
//
//	// Read new message id
//	int theNewMessageID = READ_BYTE();
//
//	// Read string
//	const char* theString = READ_STRING();
//
//	// Make the change
//	this->ChangeUpgradeCosts(theOldMessageID, theNewMessageID, theString);
//	
//	// TODO: Is it ok to return this here?  2 bytes for the ids, one for the string?
//	return 3;
//}

bool AvHHud::GetAmbientSoundNameFromIndex(string& outSoundName, int inSoundIndex) const
{
	bool theFoundName = false;

	if(inSoundIndex < (int)(this->mSoundNameList.size()))
	{
		outSoundName = this->mSoundNameList[inSoundIndex];
		theFoundName = true;
	}
	else
	{
		int a = 0;
	}

	return theFoundName;
}

void AvHHud::ModifyAmbientSoundEntryIfChanged(bool inSoundOn, int inSoundIndex, int inEntIndex, float inTimeStarted, int inVolume, int inFadeDistance, int inFlags, Vector inOrigin)
{
	bool theFoundSound = false;

	// Look up sound using inSoundIndex
	string theSoundName;
	if(this->GetAmbientSoundNameFromIndex(theSoundName, inSoundIndex))
	{
		// Loop through current sounds
		for(AmbientSoundListType::iterator theIter = this->mAmbientSounds.begin(); theIter != this->mAmbientSounds.end(); )
		{
			bool theErasedSound = false;
			if(theIter->GetEntityIndex() == inEntIndex)
			{
				// If found, remember that we found it
				theFoundSound = true;
				
				// Set position
				theIter->SetPosition(inOrigin);
				
				// If we're turning off sound, kill the sound
				if(!inSoundOn)
				{
					theIter->ClearData();
					theIter = this->mAmbientSounds.erase(theIter);
					theErasedSound = true;
				}
			}
			if(!theErasedSound)
			{
				theIter++;
			}
		}
		
		// If we're turning a sound on, and we didn't find one
		if(inSoundOn && !theFoundSound)
		{
			bool theLooping = inFlags & 2;
			float theTimeElapsed = this->mTimeOfLastUpdate - inTimeStarted;
			
			// Add new entry with these values
			this->mAmbientSounds.push_back(AvHAmbientSound(theSoundName, inVolume, inFadeDistance, theLooping, inOrigin, inEntIndex, theTimeElapsed));
		}
	}
	else
	{
		// We may not have the sound list yet, it's OK
		//ASSERT(false);
	}
}

// Look at incoming order.  If we are one of the receivers, play a HUD sound
// indicating our new order
void AvHHud::OrderNotification(const AvHOrder& inOrder)
{
	//if(!inOrder.GetOrderCompleted())
	//{
		// If we are commander, or we are in receiver list
		int theLocalPlayer = gEngfuncs.GetLocalPlayer()->index;
		if((this->GetHUDUser3() == AVH_USER3_COMMANDER_PLAYER) || (inOrder.GetHasReceiver(theLocalPlayer)))
		{
			// Do a switch on the order type
			AvHOrderType theOrderType = inOrder.GetOrderType();
			AvHHUDSound theSound = HUD_SOUND_INVALID;
			
			// Play HUD sound depending on order
			switch(theOrderType)
			{
			case ORDERTYPEL_MOVE:
				theSound = HUD_SOUND_ORDER_MOVE;
				break;
				
			case ORDERTYPET_ATTACK:
				theSound = HUD_SOUND_ORDER_ATTACK;
				break;
				
			case ORDERTYPET_BUILD:
				theSound = HUD_SOUND_ORDER_BUILD;
				break;
				
			case ORDERTYPET_GUARD:
				theSound = HUD_SOUND_ORDER_GUARD;
				break;
				
			case ORDERTYPET_WELD:
				theSound = HUD_SOUND_ORDER_WELD;
				break;
				
			case ORDERTYPET_GET:
				theSound = HUD_SOUND_ORDER_GET;
				break;
			}

			if((this->GetHUDUser3() == AVH_USER3_MARINE_PLAYER) && (inOrder.GetOrderCompleted()))
			{
				theSound = HUD_SOUND_ORDER_COMPLETE;
			}

			this->PlayHUDSound(theSound);
		}
	//}
}

void AvHHud::ResetComponentsForUser3()
{

	this->mPieMenuControl = "";
	
	this->GetManager().HideComponents();

	if(gParticleEditorHandler.GetInEditMode())
	{
		gHUD.GetManager().UnhideComponent(kPSESizeSlider);
		gHUD.GetManager().UnhideComponent(kPSESizeLabel);
		
		gHUD.GetManager().UnhideComponent(kPSEScaleSlider);
		gHUD.GetManager().UnhideComponent(kPSEScaleLabel);
		
		gHUD.GetManager().UnhideComponent(kPSEGenerationRateSlider);
		gHUD.GetManager().UnhideComponent(kPSEGenerationRateLabel);
		
		gHUD.GetManager().UnhideComponent(kPSEParticleLifetimeSlider);
		gHUD.GetManager().UnhideComponent(kPSEParticleLifetimeLabel);
		
		gHUD.GetManager().UnhideComponent(kPSEParticleSystemLifetimeSlider);
		gHUD.GetManager().UnhideComponent(kPSEParticleSystemLifetimeLabel);
		
		gHUD.GetManager().UnhideComponent(kPSEMaxParticlesSlider);
		gHUD.GetManager().UnhideComponent(kPSEMaxParticlesLabel);
		
		gHUD.GetManager().UnhideComponent(kPSEDrawModeSlider);
		gHUD.GetManager().UnhideComponent(kPSEDrawModeLabel);
		
		gHUD.GetManager().UnhideComponent(PSEGenVelToggleSlider);
		gHUD.GetManager().UnhideComponent(kPSEGenVelToggleLabel);
		
		gHUD.GetManager().UnhideComponent(kPSEGenVelShapeSlider);
		gHUD.GetManager().UnhideComponent(kPSEGenVelShapeLabel);
		
		gHUD.GetManager().UnhideComponent(kPSEGenVelParamNumSlider);
		gHUD.GetManager().UnhideComponent(kPSEGenVelParamNumLabel);
		
		gHUD.GetManager().UnhideComponent(kPSEGenVelParamValueSlider);
		gHUD.GetManager().UnhideComponent(kPSEGenVelParamValueLabel);
	}
	else
	{
		bool theIsCombatMode = (this->mMapMode == MAP_MODE_CO);
		bool theIsNSMode = (this->mMapMode == MAP_MODE_NS);
	
		if((this->GetHUDPlayMode() == PLAYMODE_PLAYING) && !this->GetIsNotInControl() && !gViewPort->IsOptionsMenuVisible())
		{
			switch(this->GetHUDUser3())
			{
			case AVH_USER3_MARINE_PLAYER:
				if(theIsCombatMode)
				{
					this->mPieMenuControl = kSoldierCombatMenu;
				}
				else if(theIsNSMode)
				{
					this->mPieMenuControl = kSoldierMenu;
				}

				if (g_iUser1 == OBS_NONE)
				{
					this->GetManager().UnhideComponent(mPieMenuControl.c_str());
				}

				// Removed these for recording footage until they look better
				//this->GetManager().UnhideComponent(kReinforcementsLabel);
				//this->GetManager().UnhideComponent(kResourceLabel);
				//this->GetManager().UnhideComponent(kMouseCursorLabel);
				//this->GetManager().UnhideComponent(kDebugCSPServerLabel);
				//this->GetManager().UnhideComponent(kDebugCSPClientLabel);
				break;
				
			case AVH_USER3_COMMANDER_PLAYER:
				if(this->mInTopDownMode)
				{
					this->GetManager().UnhideComponent(kSelectionBox);
					this->GetManager().UnhideComponent(kCommanderResourceLabel);
					//this->GetManager().UnhideComponent(kMouseCursorLabel);
					this->GetManager().UnhideComponent(kLeaveCommanderButton);
					this->GetManager().UnhideComponent(kScroller);
					this->GetManager().UnhideComponent(kTechHelpText);
					this->GetManager().UnhideComponent(kHierarchy);
					this->GetManager().UnhideComponent(kResearchBackgroundPanel);
					this->GetManager().UnhideComponent(kActionButtonsComponents);
					this->GetManager().UnhideComponent(kSelectAllImpulsePanel);
					//this->GetManager().UnhideComponent(kTopDownHUDTopSpritePanel);
					//this->GetManager().UnhideComponent(kTopDownHUDBottomSpritePanel);
				}
				//this->GetManager().UnhideComponent(kReinforcementsLabel);
				break;
				
			case AVH_USER3_ALIEN_PLAYER1:
			case AVH_USER3_ALIEN_PLAYER2:
			case AVH_USER3_ALIEN_PLAYER3:
			case AVH_USER3_ALIEN_PLAYER4:
			case AVH_USER3_ALIEN_PLAYER5:
				if(theIsCombatMode)
				{
					this->mPieMenuControl = kAlienCombatMenu;
				}
				else if(theIsNSMode)
				{
					this->mPieMenuControl = kAlienMenu;
				}
				
				if (g_iUser1 == OBS_NONE)
				{
					this->GetManager().UnhideComponent(mPieMenuControl.c_str());
				}

				//this->GetManager().UnhideComponent(kMouseCursorLabel);
				//this->GetManager().UnhideComponent(kDebugCSPServerLabel);
				//this->GetManager().UnhideComponent(kDebugCSPClientLabel);
				break;

			case AVH_USER3_ALIEN_EMBRYO:
				//this->GetManager().UnhideComponent(kAlienMembrane);
				break;
			}

			if(sShowMap)
			{
				this->GetManager().UnhideComponent(kShowMapHierarchy);
			}

			mOverviewMap.SetUser3(this->GetHUDUser3());
			
			// Update command hierarchy so it can potentially display differently
			if(this->mHierarchy)
			{
				this->mHierarchy->setPos(.0105f*ScreenWidth(), .728*ScreenHeight());
				this->mHierarchy->setSize(.265*ScreenWidth(), .247*ScreenHeight());
			}

        }
	}
}

#ifdef AVH_PLAYTEST_BUILD
BIND_MESSAGE(BalanceVar);
int AvHHud::BalanceVar(const char* pszName, int iSize, void* pbuf)
{
	int theBytesRead = 0;
	BEGIN_READ(pbuf, iSize);

	// Read string
	char* theCString = READ_STRING();
	theBytesRead += strlen(theCString);
	string theString(theCString);

	// Read byte indicating whether to be stored as a long or float
	int theFloatData = READ_BYTE();
	
	// Read data
    int theIntValue = READ_LONG();
    theBytesRead += 4;

	if(theFloatData)
	{
		float theFloatValue = theIntValue/kNormalizationNetworkFactor;
		this->mBalanceFloats[theString] = theFloatValue;
	}
	else
	{
		this->mBalanceInts[theString] = theIntValue;
	}

	if(gViewPort)
	{
		gViewPort->mBalanceChanged = true;
	}

	return theBytesRead;
}

const BalanceIntListType& AvHHud::GetBalanceInts() const
{
	return this->mBalanceInts;
}

const BalanceFloatListType&	AvHHud::GetBalanceFloats() const
{
	return this->mBalanceFloats;
}

#endif

BIND_MESSAGE(GameStatus);
int AvHHud::GameStatus(const char* pszName, int iSize, void* pbuf)
{
	int theBytesRead = 0;
	BEGIN_READ(pbuf, iSize);
	
	BYTE theStatusByte = READ_BYTE();
	theBytesRead++;

	this->mMapMode = (AvHMapMode)READ_BYTE();
	theBytesRead++;

	if((theStatusByte == kGameStatusReset) || (theStatusByte == kGameStatusResetNewMap))
	{
		if(this->mInTopDownMode)
		{
			this->ToggleMouse();

			// Reset angles?
//			gViewAngles[0] = READ_COORD();
//			gViewAngles[1] = READ_COORD();
//			gViewAngles[2] = READ_COORD();
//			gResetViewAngles = true;
		}
		
		bool theMapChange = false;
		if(theStatusByte == kGameStatusResetNewMap)
		{
			theMapChange = true;
		}

		this->ResetGame(theMapChange);
	}
	// Victor determined, but we are still in the cooldown time
	else if(theStatusByte == kGameStatusEnded)
	{
		// Stop research
		this->mGameEnded = true;
	}
	else if(theStatusByte == kGameStatusGameTime)
	{
		// Authoritative game time from server (which we then increment every tick)
		this->mGameTime = READ_SHORT();
		this->mTimeLimit = READ_SHORT();
		this->mCombatAttackingTeamNumber = READ_BYTE();
		theBytesRead += 5;
	}
	else if(theStatusByte == kGameStatusUnspentLevels)
	{
		this->mExperienceLevelSpent = READ_BYTE();
		theBytesRead++;
	}
	
	return theBytesRead;
}

BIND_MESSAGE(MiniMap);
int AvHHud::MiniMap(const char* pszName, int iSize, void* pbuf)
{
	int theBytesRead = 0;
	BEGIN_READ(pbuf, iSize);

	theBytesRead += gMiniMap.ReceiveFromNetworkStream();
	
	return theBytesRead;
}


BIND_MESSAGE(ServerVar);
int AvHHud::ServerVar(const char* pszName, int iSize, void* pbuf)
{
    BEGIN_READ(pbuf, iSize);

    std::string theVariableName  = READ_STRING();
    std::string theVariableValue = READ_STRING();

    mServerVariableMap[theVariableName] = theVariableValue;
	
	return 1;

}


//BIND_MESSAGE(NetSS);
//int	AvHHud::NetSS(const char* pszName, int iSize, void* pbuf)
//{
//	int theBytesRead = 0;
//	BEGIN_READ(pbuf, iSize);
//	
//	BYTE theStatusByte = READ_BYTE();
//	theBytesRead++;
//
//	ClientCmd("snapshot");
//
//	return theBytesRead;
//}

BIND_MESSAGE(Progress);
int AvHHud::Progress(const char* pszName, int iSize, void* pbuf)
{
	int theBytesRead = 0;
	BEGIN_READ(pbuf, iSize);

	this->mProgressBarEntityIndex = READ_SHORT();
	theBytesRead += 2;

	this->mProgressBarParam = READ_BYTE();
	theBytesRead++;
	
	return theBytesRead;
}

// Start the game over.  Called after the game ends and also after a map change
void AvHHud::ResetGame(bool inMapChanged)
{
	UIHud::ResetGame();

	this->mResources = 0;
	this->mMaxResources = -1;
	this->mVisualResources = 0;
	this->mUser2OfLastResourceMessage = 0;
	this->mTimeOfLastEntityUpdate = -1;
	this->mVisualEnergyLevel = 0;
	this->mUser2OfLastEnergyLevel = 0;

	// Don't use a menu yet
	//this->ResetUpgradeCosts();
	
	// Reset armor as well.
	//this->mArmorLevel = ARMOR_BASE;
	
	// Clear out all particle systems and templates
	if(inMapChanged)
	{
		gParticleTemplateList.Clear();
		this->mTimeOfLastUpdate = 0.0f;

		this->mInfoLocationList.clear();

		if(gViewPort)
		{
			ScorePanel* theScoreBoard = gViewPort->GetScoreBoard();
			if(theScoreBoard)
				theScoreBoard->DeleteCustomIcons();
		}
	}

	AvHParticleSystemManager::Instance()->Reset();

	this->mTechSlotManager.Clear();

    this->mTechNodes.Clear();
	
	this->mTimeOfCurrentUpdate = 0.0f;

	// On game reset, clear blips (happens on server as well)
    
	this->mEntityHierarchy.Clear();
	
	// Clear selection effects
	this->mSelectionEffects.clear();

	// End any jetpack effects
	//EndJetpackEffects();

	// Clear client scripts
	AvHScriptManager::Instance()->Reset();
	
	// Selection and commander variables
	this->mNumLocalSelectEvents = 0;
	this->mMapMode = MAP_MODE_UNDEFINED;
	this->mInTopDownMode = false;
	this->mLeftMouseStarted = false;
	this->mLeftMouseEnded = false;
	this->mPlacingBuilding = false;

	sShowMap = false;
	
	this->StopMusic();
	
	for(AmbientSoundListType::iterator theIter = this->mAmbientSounds.begin(); theIter != this->mAmbientSounds.end(); theIter++)
	{
		theIter->ClearData();
	}
	this->mAmbientSounds.clear();
	
	this->SetReinforcements(0);
	
	this->mOrders.clear();

	this->mCurrentCursorFrame = 0;
	this->mProgressBarEntityIndex = -1;
	this->mProgressBarParam = -1;

	this->mEnemyBlips.Clear();
	this->mFriendlyBlips.Clear();

	// Reset view angles (in case player was in commander mode)
	gViewAngles.z = 0.0f;
	gResetViewAngles = true;

	// Clear location
	this->mLocationText = "";

	this->mUpgrades.clear();

	this->mNumUpgradesAvailable = 0;
	int i;
	for(i = 0; i < ALIEN_UPGRADE_CATEGORY_MAX_PLUS_ONE; i++)
	{
		this->mCurrentUpgradeCategory[i] = ALIEN_UPGRADE_CATEGORY_INVALID;
	}

	// Remove all decals (no idea how to get the total decals)
	//for(i = 0; i < 1024; i++)
	//{
	//	gEngfuncs.pEfxAPI->R_DecalRemoveAll(i);
	//}
	RemoveAllDecals();

	// Remove temp ghost building
	if(this->mLastGhostBuilding)
	{
		this->mLastGhostBuilding->die = -1;
	}

	this->mNumericalInfoEffects.clear();
	this->mTimeOfNextHudSound = -1;
    this->mLastHUDSoundPlayed = HUD_SOUND_INVALID;

	//this->mTooltips.clear();

	this->mHiveInfoList.clear();

	this->mDesiredGammaSlope = kDefaultMapGamma;
	this->mRecordingLastFrame = false;
	this->mTimeOfLastHelpText = -1;
	this->mDisplayedToolTipList.clear();
	this->mCurrentWeaponID = -1;
	this->mCurrentWeaponEnabled = false;

	// Is this needed?
	//this->mCurrentUIMode = MAIN_MODE;

	this->mMenuTechSlots = 0;
	this->mPendingRequests.clear();

	for(i = 0; i < kNumHotkeyGroups; i++)
	{
		this->mGroups[i].clear();
		this->mGroupTypes[i] = AVH_USER3_NONE;
		this->mGroupAlerts[i] = ALERT_NONE;
	}
	this->mSelectAllGroup.clear();

	this->mCurrentSquad = 0;
	this->mBlinkingAlertType = 0;

	this->mLastTeamSpectated = TEAM_IND;

	this->mStructureNotificationList.clear();

	this->mGameTime = -1;
	this->mTimeLimit = -1;
	this->mCombatAttackingTeamNumber = 0;
	this->mGameEnded = false;

	this->mExperience = 0;
	this->mExperienceLevel = 1;
	this->mExperienceLevelLastDrawn = 1;
	this->mExperienceLevelSpent = 0;
	this->mTimeOfLastLevelUp = -1;

	memset(this->mMenuImpulses, MESSAGE_NULL, sizeof(AvHMessageID)*kNumUpgradeLines);
}

BIND_MESSAGE(SetGmma);
int	AvHHud::SetGmma(const char* pszName, int iSize, void* pbuf)
{
	int theNumBytesRead = 0;
	
	BEGIN_READ( pbuf, iSize);
	this->mDesiredGammaSlope = READ_COORD()/kNormalizationNetworkFactor;
	theNumBytesRead += 2;
	
	// Set gamma here
    if (!mSteamUIActive)
    {
	    this->SetGamma(this->mDesiredGammaSlope);
    }
	
	return theNumBytesRead;
}

BIND_MESSAGE(BlipList);
int	AvHHud::BlipList(const char* pszName, int iSize, void* pbuf)
{
	float theCurrentTime = gEngfuncs.GetClientTime();
	
    // Fetch new chain of command from network message
    BEGIN_READ(pbuf, iSize);
	int theBytesRead = 0;
	
	bool theReadingFriendlyBlips = READ_BYTE();
	theBytesRead++;
	
	if(!theReadingFriendlyBlips)
	{
		this->mEnemyBlips.Clear();
		
		// Read number enemy blips
		int theNumBlips = READ_BYTE();
		
		// For each enemy blip
		for(int i = 0; i < theNumBlips; i++)
		{
			// Read angle/distance
			float theX = READ_COORD();
			float theY = READ_COORD();
			float theZ = READ_COORD();
			int8 theStatus = READ_BYTE();
			this->mEnemyBlips.AddBlip(theX, theY, theZ, theStatus);
			
			// Increment bytes read
			theBytesRead += 7;
		}

		this->mEnemyBlips.SetTimeBlipsReceived(theCurrentTime);
	}
	else
	{
		this->mFriendlyBlips.Clear();

		// Read number friendly blips
		int theNumBlips = READ_BYTE();
		
		// For each friendly blip
		for(int i = 0; i < theNumBlips; i++)
		{
			// Read angle/distance
			float theX = READ_COORD();
			float theY = READ_COORD();
			float theZ = READ_COORD();
			int8 theStatus = READ_BYTE();
			int8 theInfo = READ_BYTE();
			this->mFriendlyBlips.AddBlip(theX, theY, theZ, theStatus, theInfo);
			
			// Increment bytes read
			theBytesRead += 8;
		}

		this->mFriendlyBlips.SetTimeBlipsReceived(theCurrentTime);
	}
			
	return theBytesRead;
}

BIND_MESSAGE(ClScript);
int	AvHHud::ClScript(const char *pszName, int iSize, void *pbuf)
{
	int theBytesRead = 0;

	BEGIN_READ(pbuf, iSize);
	int theNumScripts = READ_BYTE();
	theBytesRead++;

	for(int i = 0; i < theNumScripts; i++)
	{
		string theScriptName = READ_STRING();
		theBytesRead += theScriptName.length();

		string theExecutableScriptName = AvHSHUBuildExecutableScriptName(theScriptName, this->GetMapName());

		AvHScriptManager::Instance()->RunScript(theExecutableScriptName);
	}

	return theBytesRead;
}

BIND_MESSAGE(Particles);
int AvHHud::Particles(const char *pszName, int iSize, void *pbuf)
{
    // Fetch new chain of command from network message
    BEGIN_READ(pbuf, iSize);
	int theBytesRead = gParticleTemplateList.ReceiveFromNetworkStream();
	
    return theBytesRead;
}

BIND_MESSAGE(SoundNames);
int AvHHud::SoundNames(const char *pszName, int iSize, void *pbuf)
{
    // Fetch new chain of command from network message
    BEGIN_READ(pbuf, iSize);
	
	bool theClearSoundList = READ_BYTE();
	int theBytesRead = 1;
	
	if(theClearSoundList)
	{
		// Clear sound list
		this->mSoundNameList.clear();
	}
	else
	{
		char* theString = READ_STRING();
		theBytesRead += strlen(theString);
		
		string theSoundName(theString);
		int theSize = this->mSoundNameList.size();
		this->mSoundNameList.push_back(theString);
		theSize = this->mSoundNameList.size();
	}

    return theBytesRead;
}

BIND_MESSAGE(SetSelect);
int	AvHHud::SetSelect(const char* pszName, int iSize, void* pbuf)
{
	// Better not get this message unless it's appropriate
	//ASSERT(this->GetInTopDownMode());

	int theBytesRead = 0;

	EntityListType theGroup;

    BEGIN_READ(pbuf, iSize);

	// Read whether this is a hotgroup or the current selection
	int theHotGroupNumber = READ_BYTE();
	theBytesRead++;

	if((theHotGroupNumber <= (kNumHotkeyGroups)) || (theHotGroupNumber == kSelectAllHotGroup))
	{
		int theNumUnits = READ_BYTE();
		theBytesRead += 1;
		
		for(int i = 0; i < theNumUnits; i++)
		{
			int theEntIndex = READ_SHORT();
			theGroup.push_back(theEntIndex);
			theBytesRead += 2;
		}

		if(theHotGroupNumber == 0)
		{
			// set flag to indicate selection just changed
//			#ifdef AVH_PREDICT_SELECT
//			this->mNumLocalSelectEvents--;
//	
//			// This will only be negative when the server sets the selection for us, like when becoming commander
//			if(this->mNumLocalSelectEvents <= 0)
//			{
//			#endif

			bool theIsTracking = READ_BYTE();
			theBytesRead++;

			if(theIsTracking)
			{
				this->mTrackingEntity = READ_SHORT();
				theBytesRead += 2;
			}
			else
			{
				this->mTrackingEntity = 0;
			}
	
			if(theGroup != this->mSelected)
			{
				this->mSelectionJustChanged = true;
				this->mSelected = theGroup;
			}
//			this->mNumLocalSelectEvents = 0;
//			#ifdef AVH_PREDICT_SELECT
//			}
//#endif
		}
		else
		{
			if(theHotGroupNumber <= kNumHotkeyGroups)
			{
				this->mGroups[theHotGroupNumber-1] = theGroup;
				
				this->mGroupTypes[theHotGroupNumber-1] = (AvHUser3)READ_BYTE();
				theBytesRead++;
				
				mGroupAlerts[theHotGroupNumber-1] = (AvHAlertType)READ_BYTE();
				theBytesRead++;
			}
			else
			{
				// Select all group
				ASSERT(theHotGroupNumber == kSelectAllHotGroup);
				this->mSelectAllGroup = theGroup;
			}
		}
	}
	else
	{
		// Read request type
		AvHMessageID theRequestType = AvHMessageID(READ_BYTE());
		theBytesRead++;

		// Read either number of idle soldiers
		int theNumberPendingRequests = READ_BYTE();
		theBytesRead++;

		// Save it
		this->mPendingRequests[theRequestType] = theNumberPendingRequests;
	}
	
	return theBytesRead;
}

BIND_MESSAGE(SetOrder);
int	AvHHud::SetOrder(const char* pszName, int iSize, void* pbuf)
{
	int theBytesRead = 0;
	
    BEGIN_READ(pbuf, iSize);

	// Read status, changed or new
	//int theStatus = READ_BYTE();
	//theBytesRead++;
	
	AvHOrder theNewOrder;
	theBytesRead += theNewOrder.ReceiveFromNetworkStream();
	
	AvHChangeOrder(this->mOrders, theNewOrder);
	
	// Give feedback on order
	this->OrderNotification(theNewOrder);
	
	// Run through orders, deleting any that are complete
	for(OrderListType::iterator theIter = this->mOrders.begin(); theIter != this->mOrders.end(); /* no inc */)
	{
		if(theIter->GetOrderCompleted())
		{
			this->mOrders.erase(theIter);
		}
		else
		{
			theIter++;
		}
	}
	
	return theBytesRead;
}

//BIND_MESSAGE(CplteOrder);
//int	AvHHud::CplteOrder(const char* pszName, int iSize, void* pbuf)
//{
//	int theBytesRead = 0;
//	
//    BEGIN_READ(pbuf, iSize);
//
//	int theNumPlayers = READ_BYTE();
//	theBytesRead++;
//
//	for(int i = 0; i < theNumPlayers; i++)
//	{
//		int thePlayerIndex = READ_BYTE();
//		theBytesRead++;
//
//		AvHRemovePlayerFromOrders(this->mOrders, thePlayerIndex);
//	}
//
//	return theBytesRead;
//}	

BIND_MESSAGE(SetupMap);
int	AvHHud::SetupMap(const char* pszName, int iSize, void* pbuf)
{
	int theBytesRead = 0;
	
    BEGIN_READ(pbuf, iSize);

	int theMessageType = READ_BYTE();
	theBytesRead++;

	ASSERT((theMessageType == 0) || (theMessageType == 1));
	if(theMessageType == 0)
	{
		// Read map name
		char* theMapName = READ_STRING();
		this->mMapName = string(theMapName);
		theBytesRead += strlen(theMapName);
		
		// Read map extents if entering top down mode
		theBytesRead += this->mMapExtents.ReceiveFromNetworkStream();
	}
	else if(theMessageType == 1)
	{
		// Receiving an info location
		// Read string
		char* theInfoLocationName = READ_STRING();
		theBytesRead += strlen(theInfoLocationName);

		// Read max extents
		vec3_t theMaxExtent;
		theMaxExtent.x = READ_COORD();
		theMaxExtent.y = READ_COORD();
		theMaxExtent.z = 0;//READ_COORD();
		theBytesRead += 4;

		// Read min extents
		vec3_t theMinExtent;
		theMinExtent.x = READ_COORD();
		theMinExtent.y = READ_COORD();
		theMinExtent.z = 0;//READ_COORD();
		theBytesRead += 4;

		// Save info location
		string theLocationName(theInfoLocationName);
		AvHBaseInfoLocation theLocation(theLocationName, theMaxExtent, theMinExtent);
		this->mInfoLocationList.push_back(theLocation);
	}
	
	return theBytesRead;
}

  
BIND_MESSAGE(SetTopDown);
int	AvHHud::SetTopDown(const char* pszName, int iSize, void* pbuf)
{
	int theBytesRead = 0;
	
    BEGIN_READ(pbuf, iSize);

	bool theSendingTechSlots = READ_BYTE();

	if(!theSendingTechSlots)
	{
		BYTE theTopDownMode = READ_BYTE();
		theBytesRead += 1;
		
		if(this->mInTopDownMode && !theTopDownMode)
		{
			// Switch away from top down mode
			this->mInTopDownMode = false;
			this->ToggleMouse();
		
			// Reset angles
			gViewAngles[0] = READ_COORD();
			gViewAngles[1] = READ_COORD();
			gViewAngles[2] = READ_COORD();
			theBytesRead += 6;
		
			gResetViewAngles = true;
		
			this->mSelectionEffects.clear();
		}
		else if(!this->mInTopDownMode && theTopDownMode)
		{
			// Switch to top down mode!
			this->mInTopDownMode = true;
			this->ToggleMouse();
		}
		
		if(theTopDownMode)
		{
			// Read new PAS
			this->mCommanderPAS.x = READ_COORD();
			this->mCommanderPAS.y = READ_COORD();
			this->mCommanderPAS.z = READ_COORD();
			theBytesRead += 6;
		}
	}
	else
	{
		// Read data for commander menus
		this->mMenuTechSlots = READ_LONG();
		theBytesRead += 4;
	}

	return theBytesRead;
}


BIND_MESSAGE(EntHier);
int AvHHud::EntHier(const char *pszName, int iSize, void *pbuf)
{
    // Fetch new chain of command from network message
	int theBytesRead = this->mEntityHierarchy.ReceiveFromNetworkStream(iSize, pbuf);

	// Print how much data we just received
    //char theMessage[256];
    //sprintf(theMessage, "EntityHierarchy, received %d bytes.", theBytesRead);
	//CenterPrint(theMessage);

    return theBytesRead;
}

BIND_MESSAGE(EditPS);
int	AvHHud::EditPS(const char* pszName, int iSize, void* pbuf)
{
	int theNumBytesRead = 0;
	
    BEGIN_READ( pbuf, iSize);
	
	uint32 theEditParticleSystemIndex = (uint32)(READ_SHORT());
	theNumBytesRead += 2;

	AvHParticleEditorHandler::SetEditIndex(theEditParticleSystemIndex);

	return theNumBytesRead;
}

BIND_MESSAGE(Fog);
int	AvHHud::Fog(const char* pszName, int iSize, void* pbuf)
{
	int theNumBytesRead = 0;
	
    BEGIN_READ( pbuf, iSize);

	// Read fog values
	this->mFogActive = READ_BYTE();
	theNumBytesRead++;

	if(this->mFogActive)
	{
		this->mFogColor.x = READ_BYTE();
		this->mFogColor.y = READ_BYTE();
		this->mFogColor.z = READ_BYTE();
		theNumBytesRead += 3;

		this->mFogStart = READ_COORD();
		this->mFogEnd = READ_COORD();
		theNumBytesRead += 4;
	}

	return theNumBytesRead;
}

BIND_MESSAGE(ListPS);
int	AvHHud::ListPS(const char* pszName, int iSize, void* pbuf)
{
	int theNumBytesRead = 0;
	
    BEGIN_READ( pbuf, iSize);
	
	//int theNumTemplates = READ_SHORT();
	//theNumBytesRead += 2;

	//for(int i = 0; i < theNumTemplates; i++)
	//{
		string theTemplateName = READ_STRING();
		theNumBytesRead += theTemplateName.length();

		this->m_SayText.SayTextPrint(theTemplateName.c_str(), 256, 0);
	//}

	return theNumBytesRead;
}

BIND_MESSAGE(Reinfor);
int	AvHHud::Reinfor(const char* pszName, int iSize, void* pbuf)
{
	int theNumBytesRead = 0;
	
    BEGIN_READ( pbuf, iSize);
	
	int theReinforcements = READ_SHORT();
	theNumBytesRead += 2;

	this->SetReinforcements(theReinforcements);
	
	return theNumBytesRead;
}

BIND_MESSAGE(PlayHUDNot);
int AvHHud::PlayHUDNot(const char* pszName, int iSize, void* pbuf)
{
	int theBytesRead = 0;
    BEGIN_READ( pbuf, iSize);

	int theHUDNotType = READ_BYTE();
	theBytesRead++;

	int theHUDNotData = READ_BYTE();
	theBytesRead++;

	if(theHUDNotType == 0)
	{
		AvHHUDSound theSound = (AvHHUDSound)theHUDNotData;

		// Added by mmcguire.
        float theX = READ_COORD();
		float theY = READ_COORD();
		theBytesRead += 4;
		
		// Hack to avoid adding another network message (at max)
		if(!this->GetInTopDownMode())
		{
			switch(theSound)
			{
			case HUD_SOUND_SQUAD1:
				this->mCurrentSquad = 1;
				break;
			case HUD_SOUND_SQUAD2:
				this->mCurrentSquad = 2;
				break;
			case HUD_SOUND_SQUAD3:
				this->mCurrentSquad = 3;
				break;
			case HUD_SOUND_SQUAD4:
				this->mCurrentSquad = 4;
				break;
			case HUD_SOUND_SQUAD5:
				this->mCurrentSquad = 5;
				break;
			}
		}
		else
		{
			switch(theSound)
			{
			// Danger
			case HUD_SOUND_MARINE_SOLDIER_UNDER_ATTACK:
			case HUD_SOUND_MARINE_BASE_UNDER_ATTACK:
			case HUD_SOUND_MARINE_SENTRYDAMAGED:
			case HUD_SOUND_MARINE_SOLDIERLOST:
			case HUD_SOUND_MARINE_CCUNDERATTACK:
				this->mBlinkingAlertType = 2;
				AddMiniMapAlert(theX, theY);
				break;
		
			// Just research or something like that
			case HUD_SOUND_MARINE_UPGRADE_COMPLETE:
			case HUD_SOUND_MARINE_RESEARCHCOMPLETE:
				this->mBlinkingAlertType = 1;
				AddMiniMapAlert(theX, theY);
				break;
			}
		}
		
        this->PlayHUDSound(theSound);

		//char theMessage[512];
		//sprintf(theMessage, "Playing sound %d in HUD\n", theSound );
		//ConsolePrint(theMessage);
	
	}
	else
	{
		// Read byte for data (AvHUser3)
		AvHMessageID theMessageID = AvHMessageID(theHUDNotData);

		// Read byte for source player
		//int theSourcePlayer = READ_BYTE();
		//theBytesRead++;

		// Location
		float theX = READ_COORD();
		float theY = READ_COORD();
		theBytesRead += 4;

		// Push back icon
		HUDNotificationType theNotification;
		theNotification.mStructureID = theMessageID;
		theNotification.mTime = this->mTimeOfCurrentUpdate;
		//theNotification.mPlayerIndex = theSourcePlayer;
		theNotification.mLocation = Vector(theX, theY, 0.0f);

		if(CVAR_GET_FLOAT(kvBuildMessages))
		{
			this->mStructureNotificationList.push_back(theNotification);
		}
	}

	return theBytesRead;
}

BIND_MESSAGE(AlienInfo);
int	AvHHud::AlienInfo(const char* pszName, int iSize, void* pbuf)
{
	int theNumBytesRead = 0;
	
    BEGIN_READ( pbuf, iSize);

	// 0 means upgrades, 1 means hive info
	int theMessageType = READ_BYTE();
	theNumBytesRead++;

	if(theMessageType == 0)
	{
		// Read upgrades
		int theNumUpgrades = READ_BYTE();
		theNumBytesRead++;
		
		this->mUpgrades.clear();
		for(int i = 0; i < theNumUpgrades; i++)
		{
			AvHAlienUpgradeCategory theUpgradeCategory = AvHAlienUpgradeCategory(READ_BYTE());
			this->mUpgrades.push_back(theUpgradeCategory);
			theNumBytesRead++;
		}
	}
	else if(theMessageType == 1)
	{
		// Read num hives
		int theNumHives = READ_BYTE();
		theNumBytesRead++;

		// For each hive, read if it changed or not
		for(int i = 0; i < theNumHives; i++)
		{
			int theHiveInfoChanged = READ_BYTE();
			theNumBytesRead++;

			if(theHiveInfoChanged)
			{
				float theX = 0;
				float theY = 0;
				float theZ = 0;

				// Sending coords for hive?
				int theSentCoords = READ_BYTE();
				theNumBytesRead++;

				if(theSentCoords)
				{
					theX = READ_COORD();
					theY = READ_COORD();
					theZ = READ_COORD();
					theNumBytesRead += 6;
				}
				else if((int)this->mHiveInfoList.size() > i)
				{
					// Preserve existing coords
					theX = this->mHiveInfoList[i].mPosX;
					theY = this->mHiveInfoList[i].mPosY;
					theZ = this->mHiveInfoList[i].mPosZ;
				}
				else
				{
					int a = 0;
				}

				AvHHiveInfo theHiveInfo;
				theHiveInfo.mPosX = theX;
				theHiveInfo.mPosY = theY;
				theHiveInfo.mPosZ = theZ;

				theHiveInfo.mStatus = READ_BYTE();
				theHiveInfo.mUnderAttack = READ_BYTE();
				theHiveInfo.mTechnology = READ_SHORT();
				theNumBytesRead += 4;
				
				theHiveInfo.mHealthPercentage = READ_BYTE();
				theNumBytesRead += 1;

				
				// Add or replace
				if((int)this->mHiveInfoList.size() <= i)
				{
					this->mHiveInfoList.push_back(theHiveInfo);
				}
				else
				{
					this->mHiveInfoList[i] = theHiveInfo;
				}
			}
		}
	}
	else
	{
		ASSERT(false);
	}
	
	return theNumBytesRead;
}

void AvHHud::PlayHUDSound(const char *szSound, float vol, float inSoundLength) 
{ 
	if((this->mTimeOfNextHudSound == -1) || (this->mTimeOfCurrentUpdate > this->mTimeOfNextHudSound))
	{
		if(szSound)
		{
			char theSoundName[512];
			strcpy(theSoundName, szSound);
			gEngfuncs.pfnPlaySoundByName(theSoundName, vol); 
			this->mTimeOfNextHudSound = this->mTimeOfCurrentUpdate + inSoundLength;
		}
	}
}

void AvHHud::PlayHUDSound(int iSound, float vol, float inSoundLength) 
{ 
	if((this->mTimeOfNextHudSound == -1) || (this->mTimeOfCurrentUpdate > this->mTimeOfNextHudSound))
	{
		gEngfuncs.pfnPlaySoundByIndex(iSound, vol); 
		this->mTimeOfNextHudSound = this->mTimeOfCurrentUpdate + inSoundLength;
	}
}

void AvHHud::PlayHUDSound(AvHHUDSound inSound)
{
	char* theSoundPtr = NULL;
	float theVolume = 1.0f;

    // Some sounds are forced, but don't allow them to be spammed or cut themselves off
    bool theForceSound = AvHSHUGetForceHUDSound(inSound) && (inSound != this->mLastHUDSoundPlayed);

	// tankefugl: 0000407
	bool theAutoHelpEnabled = gEngfuncs.pfnGetCvarFloat(kvAutoHelp);
	// :tankefugl

	if((this->mTimeOfNextHudSound == -1) || (this->mTimeOfCurrentUpdate >= this->mTimeOfNextHudSound) || theForceSound)
	{
		float theSoundLength = 2.0f;

		switch(inSound)
		{
		case HUD_SOUND_POINTS_SPENT:
			theSoundPtr = kPointsSpentSound;
			theSoundLength = .2f;
			break;
		case HUD_SOUND_COUNTDOWN:
			theSoundPtr = kCountdownSound;
			theSoundLength = 0.0f;
			break;
		case HUD_SOUND_SELECT:
			if(gHUD.GetIsAlien())
				theSoundPtr = kSelectAlienSound;
			else
				theSoundPtr = kSelectSound;
			
			// Set to 0 so it never blocks other sounds
			theVolume = .2f;
			theSoundLength = 0.0f;
			break;
		case HUD_SOUND_SQUAD1:
			if(this->GetInTopDownMode())
			{
				theSoundPtr = kSquad1Sound;
				theSoundLength = 1.2f;
			}
			else
			{
				theSoundPtr = kMarineSquad1Sound;
				theSoundLength = 2.0f;
			}
			break;
		case HUD_SOUND_SQUAD2:
			if(this->GetInTopDownMode())
			{
				theSoundPtr = kSquad2Sound;
				theSoundLength = 1.2f;
			}
			else
			{
				theSoundPtr = kMarineSquad2Sound;
				theSoundLength = 2.0f;
			}
			break;
		case HUD_SOUND_SQUAD3:
			if(this->GetInTopDownMode())
			{
				theSoundPtr = kSquad3Sound;
				theSoundLength = 1.2f;
			}
			else
			{
				theSoundPtr = kMarineSquad3Sound;
				theSoundLength = 2.0f;
			}
			break;
		case HUD_SOUND_SQUAD4:
			if(this->GetInTopDownMode())
			{
				theSoundPtr = kSquad4Sound;
				theSoundLength = 1.2f;
			}
			else
			{
				theSoundPtr = kMarineSquad4Sound;
				theSoundLength = 2.0f;
			}
			break;
		case HUD_SOUND_SQUAD5:
			if(this->GetInTopDownMode())
			{
				theSoundPtr = kSquad5Sound;
				theSoundLength = 1.2f;
			}
			else
			{
				theSoundPtr = kMarineSquad5Sound;
				theSoundLength = 2.0f;
			}
			break;
		case HUD_SOUND_PLACE_BUILDING:
			theSoundPtr = kPlaceBuildingSound;
			theSoundLength = .2f;
			break;

		case HUD_SOUND_MARINE_POINTS_RECEIVED:
			theSoundPtr = kMarinePointsReceivedSound;
			theSoundLength = 1.42f;
			break;
		
		case HUD_SOUND_MARINE_RESEARCHCOMPLETE:
			theSoundPtr = kMarineResearchComplete;
			theSoundLength = 2.0f;
			break;
		case HUD_SOUND_MARINE_SOLDIER_UNDER_ATTACK:
			theSoundPtr = kMarineSoldierUnderAttack;
			theSoundLength = 3.0f;
			break;
		case HUD_SOUND_MARINE_CCONLINE:
			if(rand() % 2)
			{
				theSoundPtr = kMarineCCOnline1;
			}
			else
			{
				theSoundPtr = kMarineCCOnline2;
			}
			theSoundLength = 2.3f;
			break;
		case HUD_SOUND_MARINE_CCUNDERATTACK:
			if(rand() % 2)
			{
				theSoundPtr = kMarineCCUnderAttack1;
			}
			else
			{
				theSoundPtr = kMarineCCUnderAttack2;
			}
			theSoundLength = 3.0f;
			break;
		case HUD_SOUND_MARINE_COMMANDER_EJECTED:
			theSoundPtr = kMarineCommanderEjected;
			theSoundLength = 3.0f;
			break;
		case HUD_SOUND_MARINE_BASE_UNDER_ATTACK:
			if(rand() % 2)
			{
				theSoundPtr = kMarineBaseUnderAttack1;
			}
			else
			{
				theSoundPtr = kMarineBaseUnderAttack2;
			}
			theSoundLength = 3.0f;
			break;
		case HUD_SOUND_MARINE_UPGRADE_COMPLETE:
			theSoundPtr = kMarineUpgradeComplete;
			theSoundLength = 1.0f;
			break;
		case HUD_SOUND_MARINE_MORE:
			theSoundPtr = kMarineMoreResources;
			theSoundLength = 1.8f;
			break;
		case HUD_SOUND_MARINE_RESOURCES_LOW:
			theSoundPtr = kMarineLowResources;
			theSoundLength = 2.0f;
			break;
		case HUD_SOUND_MARINE_NEEDS_AMMO:
			if(rand() % 2)
				theSoundPtr = kMarineNeedsAmmo1;
			else
				theSoundPtr = kMarineNeedsAmmo2;
			theSoundLength = 1.5f;
			break;
		case HUD_SOUND_MARINE_NEEDS_HEALTH:
			if(rand() % 2)
				theSoundPtr = kMarineNeedsHealth1;
			else
				theSoundPtr = kMarineNeedsHealth2;
			theSoundLength = 1.3f;
			break;

		case HUD_SOUND_MARINE_NEEDS_ORDER:
			if(rand() % 2)
				theSoundPtr = kMarineNeedsOrder1;
			else
				theSoundPtr = kMarineNeedsOrder2;
			theSoundLength = 1.5f;
			break;

		case HUD_SOUND_MARINE_SOLDIER_LOST:
			if(rand() % 2)
				theSoundPtr = kMarineSoldierLost1;
			else
				theSoundPtr = kMarineSoldierLost2;
			theSoundLength = 1.3f;
			break;
		case HUD_SOUND_MARINE_SENTRYFIRING:
			if(rand() % 2)
				theSoundPtr = kMarineSentryFiring1;
			else
				theSoundPtr = kMarineSentryFiring2;
			theSoundLength = 1.3f;
			break;
		case HUD_SOUND_MARINE_SENTRYDAMAGED:
			if(rand() % 2)
				theSoundPtr = kMarineSentryTakingDamage1;
			else
				theSoundPtr = kMarineSentryTakingDamage2;
			theSoundLength = 1.5f;
			break;

		case HUD_SOUND_MARINE_GIVEORDERS:
			// tankefugl: 0000407
			if (theAutoHelpEnabled) 
			{
				theSoundPtr = kMarineGiveOrders;
				theSoundLength = 2.2f;
			}
			// :tankefugl
			break;

		case HUD_SOUND_MARINE_NEEDPORTAL:
			// tankefugl: 0000407
			if (theAutoHelpEnabled) 
			{
				if(rand() % 2)
					theSoundPtr = kMarineNeedPortal1;
				else
					theSoundPtr = kMarineNeedPortal2;
				theSoundLength = 1.8f;
			}
			// :tankefugl
			break;

		case HUD_SOUND_MARINE_GOTOALERT:
			// tankefugl: 0000407
			if (theAutoHelpEnabled) 
			{
				theSoundPtr = kMarineGotoAlert;
				theSoundLength = 2.2f;
			}
			// :tankefugl
			break;

		case HUD_SOUND_MARINE_COMMANDERIDLE:
			// tankefugl: 0000407
			if (theAutoHelpEnabled) 
			{
				if(rand() % 2)
					theSoundPtr = kMarineCommanderIdle1;
				else
					theSoundPtr = kMarineCommanderIdle2;
				theSoundLength = 1.5f;
			}
			// :tankefugl
			break;

		case HUD_SOUND_MARINE_ARMORYUPGRADING:
			theSoundPtr = kMarineArmoryUpgrading;
			theSoundLength = 3.4;
			break;

		case HUD_SOUND_ALIEN_ENEMY_APPROACHES:
			if(rand() %2)
				theSoundPtr = kAlienEnemyApproaches1;
			else
				theSoundPtr = kAlienEnemyApproaches2;
			theSoundLength = 1.6;
			break;

		case HUD_SOUND_ALIEN_GAMEOVERMAN:
			theSoundPtr = kAlienGameOverMan;
			theSoundLength = 2.2f;
			break;

		case HUD_SOUND_ALIEN_HIVE_ATTACK:
			theSoundPtr = kAlienHiveAttack;
			theSoundLength = 1.6f;
			break;
		case HUD_SOUND_ALIEN_HIVE_COMPLETE:
			if(rand() % 2)
				theSoundPtr = kAlienHiveComplete1;
			else
				theSoundPtr = kAlienHiveComplete2;
			theSoundLength = 2.1f;
			break;
		case HUD_SOUND_ALIEN_HIVE_DYING:
			if(rand() % 2)
			{
				theSoundPtr = kAlienHiveDying1;
				theSoundLength = 1.7f;
			}
			else
			{
				theSoundPtr = kAlienHiveDying2;
				theSoundLength = 2.4f;
			}
			break;
		case HUD_SOUND_ALIEN_LIFEFORM_ATTACK:
			if(rand() % 2)
				theSoundPtr = kAlienLifeformAttack1;
			else
				theSoundPtr = kAlienLifeformAttack2;
			theSoundLength = 1.8f;
			break;
		case HUD_SOUND_ALIEN_RESOURCES_LOW:
			theSoundPtr = kAlienLowResources;
			theSoundLength = 1.7f;
			break;
		case HUD_SOUND_ALIEN_MESS:
			theSoundPtr = kAlienMess;
			theSoundLength = 2.0f;
			break;
		case HUD_SOUND_ALIEN_MORE:
			if(rand() % 2)
				theSoundPtr = kAlienMoreResources1;
			else
				theSoundPtr = kAlienMoreResources2;
			theSoundLength = 1.8f;
			break;
		case HUD_SOUND_ALIEN_NEED_BUILDERS:
			if(rand() % 2)
				theSoundPtr = kAlienNeedBuilders1;
			else
				theSoundPtr = kAlienNeedBuilders2;
			theSoundLength = 1.4f;
			break;

		case HUD_SOUND_ALIEN_NEED_BETTER:
			theSoundPtr = kAlienNeedBetter;
			theSoundLength = 2.5f;
			break;

		case HUD_SOUND_ALIEN_NOW_DONCE:
			theSoundPtr = kAlienNowDonce;
			theSoundLength = 2.1f;
			break;

		case HUD_SOUND_ALIEN_NEW_TRAIT:
			if(rand() % 2)
				theSoundPtr = kAlienNewTrait1;
			else
				theSoundPtr = kAlienNewTrait2;
			theSoundLength = 1.5f;
			break;

		case HUD_SOUND_ALIEN_POINTS_RECEIVED:
			theSoundPtr = kAlienPointsReceivedSound;
			theSoundLength = 1.57f;
			break;

		case HUD_SOUND_ALIEN_RESOURCES_ATTACK:
			if(rand() % 2)
				theSoundPtr = kAlienResourceAttack1;
			else
				theSoundPtr = kAlienResourceAttack2;
			theSoundLength = 2.1f;
			break;
		case HUD_SOUND_ALIEN_STRUCTURE_ATTACK:
			if(rand() % 2)
				theSoundPtr = kAlienStructureAttack1;
			else
				theSoundPtr = kAlienStructureAttack2;
			theSoundLength = 1.9f;
			break;
		case HUD_SOUND_ALIEN_UPGRADELOST:
			theSoundPtr = kAlienUpgradeLost;
			theSoundLength = 1.5f;
			break;
		
		case HUD_SOUND_ORDER_MOVE:
			switch(rand() % 4)
			{
			case 0:
				theSoundPtr = kSoundOrderMove1;
				theSoundLength = 1.8f;
				break;
			case 1:
				theSoundPtr = kSoundOrderMove2;
				theSoundLength = 2.3f;
				break;
			case 2:
				theSoundPtr = kSoundOrderMove3;
				theSoundLength = 1.9f;
				break;
			case 3:
				theSoundPtr = kSoundOrderMove4;
				theSoundLength = 2.3f;
				break;
			}
			break;
		case HUD_SOUND_ORDER_ATTACK:
			theSoundPtr = kSoundOrderAttack;
			break;
		case HUD_SOUND_ORDER_BUILD:
			theSoundPtr = kSoundOrderBuild;
			break;
		case HUD_SOUND_ORDER_WELD:
			theSoundPtr = kSoundOrderWeld;
			break;
		case HUD_SOUND_ORDER_GUARD:
			theSoundPtr = kSoundOrderGuard;
			break;
		case HUD_SOUND_ORDER_GET:
			theSoundPtr = kSoundOrderGet;
			break;

		case HUD_SOUND_ORDER_COMPLETE:
			switch(rand() % 6)
			{
			case 0:
				theSoundPtr = kSoundOrderComplete1;
				theSoundLength = 1.6f;
				break;
			case 1:
				theSoundPtr = kSoundOrderComplete2;
				theSoundLength = 1.9f;
				break;
			case 2:
				theSoundPtr = kSoundOrderComplete3;
				theSoundLength = 1.9f;
				break;
			case 3:
				theSoundPtr = kSoundOrderComplete4;
				theSoundLength = 1.4f;
				break;
			case 4:
				theSoundPtr = kSoundOrderComplete5;
				theSoundLength = 1.6f;
				break;
			case 5:
				theSoundPtr = kSoundOrderComplete6;
				theSoundLength = 1.4f;
				break;
			}
			break;

		case HUD_SOUND_GAMESTART:
			if(this->GetIsMarine())
			{
				if(rand() % 2)
					theSoundPtr = kMarineGameStart1;
				else
					theSoundPtr = kMarineGameStart2;
				theSoundLength = 1.9f;
			}
			else if(this->GetIsAlien())
			{
				if(rand() % 2)
					theSoundPtr = kAlienGameStart1;
				else
					theSoundPtr = kAlienGameStart2;
				theSoundLength = 2.2f;
			}
			break;
			
		case HUD_SOUND_YOU_WIN:
			theSoundPtr = kYouWinSound;
			theSoundLength = 6.0f;
			break;
		case HUD_SOUND_YOU_LOSE:
			theSoundPtr = kYouLoseSound;
			theSoundLength = 6.0f;
			break;
		case HUD_SOUND_TOOLTIP:
			theSoundPtr = kTooltipSound;
			// Tooltip sounds should never stop other sounds
			theSoundLength = -1.0f;
			theVolume = .6f;
			break;
		// joev: bug 0000767
		case HUD_SOUND_PLAYERJOIN:
			theSoundPtr = kPlayerJoinedSound;
			theSoundLength = 3.0f;
			theVolume = 1.1;
			break;
		// :joev
		}
		
		if(theSoundPtr)
		{
			gEngfuncs.pfnPlaySoundByName(theSoundPtr, theVolume);
			// char theMessage[512];
			// sprintf(theMessage, "Playing HUD sound %s, volume %f in HUD using gEngfuncs.pfnPlaySoundByName\n", theSoundPtr, theVolume );
			// ConsolePrint(theMessage);
			if(theSoundLength >= 0.0f)
			{
				this->mTimeOfNextHudSound = this->mTimeOfCurrentUpdate + theSoundLength;
			}
            this->mLastHUDSoundPlayed = inSound;
		}
	}
}

//BIND_MESSAGE(SetRole);
//int AvHHud::SetRole(const char* pszName, int iSize, void* pbuf)
//{
//	int theBytesRead = 0;
//
//	// Fetch play mode from network message
//    BEGIN_READ( pbuf, iSize);
//    int theNewRole = READ_BYTE();
//	this->mRole = AvHRole(theNewRole);
//	theBytesRead++;
//	
//	AvHTeamNumber theTeam = (AvHTeamNumber)(READ_BYTE());
//
//	// When switching sides, zero out visible resources (prevents weird behavior when going from marine team to aliens especially)
////	if(this->mTeam != theTeam)
////	{
////		this->mVisualResources = 0;
////		this->mUser2OfLastResourceMessage = 0;
////
////		this->mVisualEnergyLevel = 0;
////		this->mUser2OfLastEnergyLevel = 0;
////
////		// Remove HUD indication of current squad
////		this->mCurrentSquad = 0;
////	}
////
////	this->mTeam = theTeam;
//	theBytesRead++;
//
//	// Resources always visible these days
//	//if(this->mResourceLabel)
//	//	this->mResourceLabel->setVisible(true);
//
//	this->mGhostBuilding = MESSAGE_NULL;
//	
//	this->mReticleInfoText = "";
//
//	// Set default color for tooltip
////	int theR, theG, theB;
////	this->GetPrimaryHudColor(theR, theG, theB);
////	this->mReticleTooltip.SetR(theR);
////	this->mReticleTooltip.SetG(theG);
////	this->mReticleTooltip.SetB(theB);
//	
//	this->mTooltips.clear();
//
//	return theBytesRead;
//}

BIND_MESSAGE(SetTech);
int	AvHHud::SetTech(const char* pszName, int iSize, void* pbuf)
{
	int theBytesRead = 0;

    BEGIN_READ( pbuf, iSize);

	//AvHTechNodes theTechNodes;
	//theBytesRead += theTechNodes.ReceiveFromNetworkStream();
	//
	//this->mTechNodes = theTechNodes;

	int theIndex = READ_SHORT();
	theBytesRead += 2;

	AvHTechNode theTechNode;
	theBytesRead += theTechNode.ReceiveFromNetworkStream();

	if(!this->mTechNodes.SetTechNode(theIndex, theTechNode))
	{
		ASSERT(false);
	}

	//AvHActionButtons* theActionButtons = NULL;
	//if(this->GetManager().GetVGUIComponentNamed(kActionButtonsComponents, theActionButtons))
	//{
	//	theActionButtons->SetTechNodes(theTechNodes);
	//}

//	AvHTechButtons* theTechButtons = NULL;
//	if(this->GetManager().GetVGUIComponentNamed(theCategoryName, theTechButtons))
//	{
//		theTechButtons->SetTechNodes(theTechNodes);
//	}

	return theBytesRead;
}

BIND_MESSAGE(TechSlots);
int AvHHud::TechSlots(const char* pszName, int iSize, void* pbuf)
{
	int theBytesRead = 0;

    BEGIN_READ( pbuf, iSize);

	//int theIndex = READ_SHORT();
	//theBytesRead += 2;

	AvHTechSlots theNewTechSlots;
	theBytesRead += theNewTechSlots.ReceiveFromNetworkStream();

	this->mTechSlotManager.AddTechSlots(theNewTechSlots);

	return theBytesRead;
}

BIND_MESSAGE(DebugCSP);
int	AvHHud::DebugCSP(const char* pszName, int iSize, void* pbuf)
{
	int theBytesRead = 0;

    BEGIN_READ( pbuf, iSize);

	int theID = READ_LONG();
	int theClip = READ_LONG();
	theBytesRead += 8;

	float theNextPrimaryAttack = READ_COORD();
	float theTimeWeaponIdle = READ_COORD();
	theBytesRead += 4;

	float theNextAttack = READ_COORD();
	theBytesRead += 2;

	char theServerInfoString[512];
	sprintf(theServerInfoString, "Server: id: %d, clip: %d, prim attack: %f, idle: %f, next attack: %f", theID, theClip, theNextPrimaryAttack, theTimeWeaponIdle, theNextAttack);
		
	vgui::Label* theLabel = NULL;
	if(this->GetManager().GetVGUIComponentNamed(kDebugCSPServerLabel, theLabel))
	{
		theLabel->setText(theServerInfoString);
	}
	
	return theBytesRead;
}

//BIND_MESSAGE(GivePlayerItem);
//int AvHHud::GivePlayerItem(const char* pszName, int iSize, void *pbuf)
//{
//    BEGIN_READ( pbuf, iSize);
//    int theItem = READ_LONG();
//
//    // Give player item
//
//    return 4;
//}

AvHVisibleBlipList&	AvHHud::GetEnemyBlipList()
{
	return this->mEnemyBlips;
}

AvHEntityHierarchy& AvHHud::GetEntityHierarchy()
{
	return this->mEntityHierarchy;
}

AvHVisibleBlipList&	AvHHud::GetFriendlyBlipList()
{
	return this->mFriendlyBlips;
}

bool AvHHud::GetMouseOneDown() const
{
	return this->mMouseOneDown;
}

bool AvHHud::GetMouseTwoDown() const
{
	return this->mMouseTwoDown;
}

void AvHHud::HideProgressStatus()
{
	if(this->mGenericProgressBar)
	{
		this->mGenericProgressBar->setVisible(false);
	}

	if(this->mAlienProgressBar)
	{
		this->mAlienProgressBar->setVisible(false);
	}
}

void AvHHud::HideResearchProgressStatus()
{
	if(this->mResearchProgressBar)
	{
		this->mResearchProgressBar->setVisible(false);
		this->mResearchLabel->setVisible(false);
	}
}

void AvHHud::Init(void)
{
    UIHud::Init();

    HOOK_MESSAGE(EntHier);
	HOOK_MESSAGE(Particles);
	HOOK_MESSAGE(SoundNames);
	HOOK_MESSAGE(PlayHUDNot);
//	HOOK_MESSAGE(ChangeNode);

	#ifdef AVH_PLAYTEST_BUILD
	HOOK_MESSAGE(BalanceVar);
	#endif

	HOOK_MESSAGE(GameStatus);
	//HOOK_MESSAGE(NetSS);
	HOOK_MESSAGE(Progress);
	HOOK_MESSAGE(Countdown);
	HOOK_MESSAGE(MiniMap);
	HOOK_MESSAGE(SetOrder);
	//HOOK_MESSAGE(CplteOrder);
	HOOK_MESSAGE(SetSelect);
	HOOK_MESSAGE(SetupMap);
	HOOK_MESSAGE(SetTopDown);
	HOOK_MESSAGE(SetTech);
	HOOK_MESSAGE(EditPS);
	HOOK_MESSAGE(Fog);
	HOOK_MESSAGE(ListPS);
	HOOK_MESSAGE(Reinfor);
	HOOK_MESSAGE(SetGmma);
	HOOK_MESSAGE(BlipList);
	HOOK_MESSAGE(ClScript);
	HOOK_MESSAGE(AlienInfo);
	HOOK_MESSAGE(DebugCSP);
	HOOK_MESSAGE(TechSlots);

    HOOK_MESSAGE(ServerVar);
	
	this->AddCommands();

	this->mCountDownClock = -1;

    mOverviewMap.Clear();

	this->mEntityHierarchy.Clear();
	//this->mUpgradeCosts.clear();

	sPregameGammaTable.InitializeFromVideoState();
	sGameGammaTable.InitializeFromVideoState();

	int theRC = atexit(AvHHud::ResetGammaAtExit);
	_onexit_t theExit = _onexit(AvHHud::ResetGammaAtExitForOnExit);
	
	signal(SIGILL, AvHHud::ResetGammaAtExit);
	signal(SIGFPE, AvHHud::ResetGammaAtExit);
	signal(SIGSEGV, AvHHud::ResetGammaAtExit);
	signal(SIGTERM, AvHHud::ResetGammaAtExit);
	signal(SIGBREAK, AvHHud::ResetGammaAtExit);
	signal(SIGABRT, AvHHud::ResetGammaAtExit);

	//memset(this->mAlienUILifeforms, 0, sizeof(HSPRITE)*kNumAlienLifeforms);
	this->mAlienUIUpgrades = 0;
	this->mAlienUIUpgradeCategories = 0;
	this->mOrderSprite = 0;
	this->mCursorSprite = 0;
	this->mMarineCursor = 0;
	this->mAlienCursor = 0;

	this->mMarineUIJetpackSprite = 0;
	this->mMembraneSprite = 0;
	this->mBackgroundSprite = 0;

	this->mProgressBarEntityIndex = -1;
	this->mProgressBarParam = -1;
	this->mSelectedNodeResourceCost = -1;
	this->mCurrentUseableEnergyLevel = 0;
	this->mVisualEnergyLevel = 0.0f;

	this->mFogActive = false;
	this->mFogColor.x = this->mFogColor.y = this->mFogColor.z = 0;
	this->mFogStart = 0;
	this->mFogEnd = 0;

	this->mLocationText = "";
	this->mInfoLocationList.clear();
	
	this->mLastHotkeySelectionEvent = MESSAGE_NULL;
	this->mUpgrades.clear();

	this->mLastGhostBuilding = NULL;
	this->mReticleInfoText = "";
	this->mSelectingWeaponID = -1;
	this->mSelectingNodeID = MESSAGE_NULL;
	this->mDesiredGammaSlope = 1;
	this->mTimeOfLastHelpText = -1;
	this->mCurrentWeaponID = -1;
	this->mCurrentWeaponEnabled = false;
	this->mCurrentUIMode = MAIN_MODE;
	this->mMenuTechSlots = 0;
	this->mBlinkingAlertType = 0;
	this->mLastUser3 = AVH_USER3_NONE;
	this->mLastTeamNumber = TEAM_IND;
	this->mLastPlayMode = PLAYMODE_UNDEFINED;

    this->mCrosshairShowCount = 1;
    this->mCrosshairSprite    = 0;
    this->mCrosshairR         = 0;
    this->mCrosshairG         = 0;
    this->mCrosshairB         = 0;

	this->mDrawCombatUpgradeMenu = false;

	// Initialize viewport
	this->mViewport[0] = this->mViewport[1] = this->mViewport[2] = this->mViewport[3] = 0;

	gl_monolights = gEngfuncs.pfnGetCvarPointer("gl_monolights");
	gl_overbright = gEngfuncs.pfnGetCvarPointer("gl_overbright");
	gl_clear = gEngfuncs.pfnGetCvarPointer("gl_clear");
	hud_draw = gEngfuncs.pfnGetCvarPointer("hud_draw");
	r_drawviewmodel = gEngfuncs.pfnGetCvarPointer("r_drawviewmodel");
	gl_d3dflip = gEngfuncs.pfnGetCvarPointer("gl_d3dflip");
	s_show = gEngfuncs.pfnGetCvarPointer("s_show");
	lightgamma = gEngfuncs.pfnGetCvarPointer("lightgamma");
	r_detailtextures = gEngfuncs.pfnGetCvarPointer("r_detailtextures");
}

// This gives the HUD a chance to draw after the VGUI.  A component must allow itself to be hooked by calling this function
// at the end of it's paint() routine.  This is done so the mouse cursor can draw on top of the other components.
void AvHHud::ComponentJustPainted(Panel* inPanel)
{
//	if(this->GetInTopDownMode())
//	{
//		AvHTeamHierarchy* theComponent = dynamic_cast<AvHTeamHierarchy*>(inPanel);
//		if(theComponent)
//		{
//			int theBasePosX;
//			int theBasePosY;
//			theComponent->getPos(theBasePosX, theBasePosY);
//			this->DrawMouseCursor(theBasePosX, theBasePosY);
//		}
//	}
//	else
//	{
//		PieMenu* theComponent = dynamic_cast<PieMenu*>(inPanel);
//		if(theComponent)
//		{
//			int theBasePosX;
//			int theBasePosY;
//			theComponent->getPos(theBasePosX, theBasePosY);
//			this->DrawMouseCursor(theBasePosX, theBasePosY);
//		}
//	}


	DummyPanel* theComponent = dynamic_cast<DummyPanel*>(inPanel);
	if(theComponent)
	{
		int theBasePosX;
		int theBasePosY;
		theComponent->getPos(theBasePosX, theBasePosY);
		this->DrawMouseCursor(theBasePosX, theBasePosY);
	}

}

bool AvHHud::SetCursor(AvHOrderType inOrderType)
{
	bool theSuccess = false;

	if(!this->GetIsAlien())
	{
		this->mCursorSprite = this->mMarineCursor;

		if((inOrderType >= 0) && (inOrderType < ORDERTYPE_MAX))
		{
			this->mCurrentCursorFrame = (int)(inOrderType);
			theSuccess = true;
		}
		// Change cursor when over a valid choice
		if(this->mSelectingNodeID > 0)
		{
			this->mCurrentCursorFrame = 1;
			theSuccess = true;
		}
	}
	else
	{
		this->mCursorSprite = this->mAlienCursor;
		this->mCurrentCursorFrame = 0;

		if(this->mSelectingNodeID > 0)
		{
			// Set cursor to lifeform to evolve to
			switch(this->mSelectingNodeID)
			{
			case ALIEN_LIFEFORM_ONE:
			case ALIEN_LIFEFORM_TWO:
			case ALIEN_LIFEFORM_THREE:
			case ALIEN_LIFEFORM_FOUR:
			case ALIEN_LIFEFORM_FIVE:
				this->mCurrentCursorFrame = 2 + (int)this->mSelectingNodeID - (int)ALIEN_LIFEFORM_ONE;
				break;
			
			default:
				this->mCurrentCursorFrame = 1;
				break;
			}
		}

		theSuccess = true;
	}
	
	//	Scheme::SchemeCursor theSchemeCursor = Scheme::SchemeCursor::scu_arrow;
	//	//Scheme::SchemeCursor theSchemeCursor = Scheme::SchemeCursor::scu_no;
	//		
	//	switch(inOrderType)
	//	{
	////	case ORDERTYPE_UNDEFINED:
	////		theSchemeCursor = Scheme::SchemeCursor::scu_no;
	////		theSuccess = true;
	////		break;
	//
	//	//case ORDERTYPEL_MOVE:
	//
	//	case ORDERTYPET_ATTACK:
	//	case ORDERTYPET_GUARD:
	//	case ORDERTYPET_WELD:
	//	case ORDERTYPET_BUILD:
	//	case ORDERTYPEL_USE:
	//	case ORDERTYPET_GET:
	//		theSchemeCursor = Scheme::SchemeCursor::scu_hand;
	//		theSuccess = true;
	//		break;
	//	}
	//
	//	App::getInstance()->setCursorOveride( App::getInstance()->getScheme()->getCursor(theSchemeCursor));
	
	return theSuccess;
}

void AvHHud::GetCursor(HSPRITE& outSprite, int& outFrame)
{

    if (g_iUser1 == 0)
    {
        outSprite = mCursorSprite;
        outFrame  = mCurrentCursorFrame;
    }
    else
    {
        // Always use the marine cursor in spectator mode.
        outSprite = mMarineCursor;
        outFrame  = 0;
    }

}

void AvHHud::SetHelpMessage(const string& inHelpText, bool inForce, float inNormX, float inNormY)
{
	if(inForce || gEngfuncs.pfnGetCvarFloat(kvAutoHelp))
	{
		float theReticleX = kHelpMessageLeftEdgeInset;
		float theReticleY = kHelpMessageTopEdgeInset;
		bool theCentered = false;

		if(this->GetInTopDownMode())
		{
			int theMouseX, theMouseY;
			this->GetMousePos(theMouseX, theMouseY);
			theReticleX = theMouseX/(float)ScreenWidth();
			theReticleY = theMouseY/(float)ScreenHeight();
			
			// Move text up a bit so it doesn't obscure
			theReticleY -= .1f;
			
			theCentered = true;
		}
		// Alien HUD forces this to be inset a bit
		else 
		{
			if(this->GetIsAlien())
			{
				theReticleX = kHelpMessageAlienLeftedgeInset;
				theReticleY = kHelpMessageAlienTopEdgeInset;
			}
			
			if(inNormX != -1)
			{
				theReticleX = inNormX;
			}
			if(inNormY != -1)
			{
				theReticleY = inNormY;
			}
		}
		
		this->mHelpMessage.SetText(inHelpText);
		
		this->mHelpMessage.SetNormalizedScreenX(theReticleX);
		this->mHelpMessage.SetNormalizedScreenY(theReticleY);
		this->mHelpMessage.SetCentered(theCentered);
		this->mHelpMessage.SetNormalizedMaxWidth(kReticleMaxWidth);
		//this->mHelpMessage.SetIgnoreFadeForLifetime(true);
		
		float theTimePassed = (this->mTimeOfCurrentUpdate - this->mTimeOfLastUpdate);
		this->mHelpMessage.FadeText(theTimePassed, false);
		
		// Set color
		int theR, theG, theB;
		this->GetPrimaryHudColor(theR, theG, theB, true, false);
		this->mHelpMessage.SetRGB(theR, theG, theB);
	}
}

void AvHHud::SetActionButtonHelpMessage(const string& inHelpText)
{
	this->mTopDownActionButtonHelp.SetText(inHelpText);

	const float kNormX = .73f;
	this->mTopDownActionButtonHelp.SetNormalizedScreenX(kNormX);

	// Treat this as the bottom of the tooltip.  Scale the tooltip so it's bottom is here.
	const float kNormY = .68f;
	int theHeight = this->mTopDownActionButtonHelp.GetScreenHeight();
	float theNormalizedHeight = (float)theHeight/ScreenHeight();
	this->mTopDownActionButtonHelp.SetNormalizedScreenY(kNormY - theNormalizedHeight);

	this->mTopDownActionButtonHelp.SetBackgroundA(128);

	this->mTopDownActionButtonHelp.SetCentered(false);
	this->mTopDownActionButtonHelp.SetNormalizedMaxWidth(1.0f - kNormX - .01f);
	//this->mTopDownActionButtonHelp.SetIgnoreFadeForLifetime(true);
	
	float theTimePassed = (this->mTimeOfCurrentUpdate - this->mTimeOfLastUpdate);
	this->mTopDownActionButtonHelp.FadeText(theTimePassed, false);
	
	// Set color
	int theR, theG, theB;
	this->GetPrimaryHudColor(theR, theG, theB, true, false);
	this->mTopDownActionButtonHelp.SetRGB(theR, theG, theB);
}

void AvHHud::SetReticleMessage(const string& inHelpText)
{
	float theReticleX;
	float theReticleY;
	bool theIsCentered;
	this->GetReticleTextDrawingInfo(theReticleX, theReticleY, theIsCentered);
	
	this->mReticleMessage.SetText(inHelpText);
	
	this->mReticleMessage.SetNormalizedScreenX(theReticleX);
	this->mReticleMessage.SetNormalizedScreenY(theReticleY);
	this->mReticleMessage.SetCentered(theIsCentered);
	this->mReticleMessage.SetNormalizedMaxWidth(kReticleMaxWidth);

	// Need instant fade-in and slow fade down for player names and info
	this->mReticleMessage.SetFadeDownSpeed(-100);
	this->mReticleMessage.SetFadeUpSpeed(10000);
	//this->mReticleMessage.SetIgnoreFadeForLifetime(true);
	
	float theTimePassed = (this->mTimeOfCurrentUpdate - this->mTimeOfLastUpdate);
	this->mReticleMessage.FadeText(theTimePassed, false);
}

void AvHHud::SetCurrentUseableEnergyLevel(float inEnergyLevel)
{
	this->mCurrentUseableEnergyLevel = inEnergyLevel;
}

const AvHMapExtents& AvHHud::GetMapExtents()
{
	return this->mMapExtents;
}

void AvHHud::InitCommanderMode()
{
	Panel* thePanel = NULL;

	if(this->GetManager().GetVGUIComponentNamed(kLeaveCommanderButton, thePanel))
	{
		thePanel->addInputSignal(&gCommanderHandler);
	}

	if(this->GetManager().GetVGUIComponentNamed(kHierarchy, thePanel))
	{
		thePanel->addInputSignal(&gCommanderHandler);
	}

	if(this->GetManager().GetVGUIComponentNamed(kActionButtonsComponents, thePanel))
	{
		thePanel->addInputSignal(&gCommanderHandler);
	}

	if(this->GetManager().GetVGUIComponentNamed(kSelectAllImpulsePanel, thePanel))
	{
		thePanel->addInputSignal(&gCommanderHandler);
	}

	// Add handler for all pending request buttons
	for(int i = 0; i < MESSAGE_LAST; i++)
	{
		AvHMessageID theMessageID = AvHMessageID(i);

		char theComponentName[256];
		sprintf(theComponentName, kPendingImpulseSpecifier, (int)theMessageID);

		AvHTechImpulsePanel* theTechImpulsePanel = NULL;
		if(this->GetManager().GetVGUIComponentNamed(theComponentName, theTechImpulsePanel))
		{
			theTechImpulsePanel->addInputSignal(&gCommanderHandler);
		}
	}
	
	//this->GetManager().GetVGUIComponentNamed(kScroller, thePanel);
	//if(thePanel)
	//{
	//	thePanel->addInputSignal(&gScrollHandler);
	//}

	// Get input from every control
	this->GetManager().AddInputSignal(&gScrollHandler);
	
	// TODO: Add others here
}

// Read in base state from stream (called by Demo_ReadBuffer)
int AvHHud::InitializeDemoPlayback(int inSize, unsigned char* inBuffer)
{
	// Read in base state
	int theBytesRead = 0;
	
	// Read in num upgrades
	int theNumUpgrades = 0;
	//this->mUpgradeCosts.clear();
	LoadData(&theNumUpgrades, inBuffer, sizeof(int), theBytesRead);

	for(int i = 0; i < theNumUpgrades; i++)
	{
		// Read in upgrades (for backwards-compatibility)
		int theFirst = 0;
		LoadData(&theFirst, inBuffer, sizeof(int), theBytesRead);

		int theSecond = 0;
		LoadData(&theSecond, inBuffer, sizeof(int), theBytesRead);
	}
	
	// Read in gamma
	LoadData(&this->mDesiredGammaSlope, inBuffer, sizeof(this->mDesiredGammaSlope), theBytesRead);

    if (!mSteamUIActive)
    {
        this->SetGamma(this->mDesiredGammaSlope);
    }

	// Read in resources
	LoadData(&this->mResources, inBuffer, sizeof(this->mResources), theBytesRead);
	
	// Read in commander (TODO: REMOVE)
    int theCommander;
	LoadData(&theCommander, inBuffer, sizeof(theCommander), theBytesRead);
	
	// Read in number of hive infos
	this->mHiveInfoList.clear();

	int theNumHiveInfos = 0;
	LoadData(&theNumHiveInfos, inBuffer, sizeof(int), theBytesRead);

	// For each one, add a new hive info
	for(i = 0; i < theNumHiveInfos; i++)
	{
		AvHHiveInfo theHiveInfo;
		LoadData(&theHiveInfo, inBuffer, sizeof(AvHHiveInfo), theBytesRead);
		this->mHiveInfoList.push_back(theHiveInfo);
	}

	// Load and set current pie menu control
	int thePieMenuControlLength = 0;
	LoadData(&thePieMenuControlLength, inBuffer, sizeof(int), theBytesRead);

	char thePieMenuControl[256];
	memset(thePieMenuControl, 0, 256);
	LoadData(thePieMenuControl, inBuffer, thePieMenuControlLength, theBytesRead);
	this->mPieMenuControl = string(thePieMenuControl);
	
	// Read in selected units size
	this->mSelected.clear();
	int theNumSelected = 0;
	LoadData(&theNumSelected, inBuffer, sizeof(theNumSelected), theBytesRead);

	for(i = 0; i < theNumSelected; i++)
	{
		// Read in selected units
		EntityInfo theSelectedEntity = 0;
		LoadData(&theSelectedEntity, inBuffer, sizeof(theSelectedEntity), theBytesRead);
	}

	ASSERT((theBytesRead + (int)sizeof(int)) == inSize);

	// Clear existing particle system templates
	gParticleTemplateList.Clear();
	AvHParticleSystemManager::Instance()->Reset();

	// Clear weapon info
	gWR.Reset();

	return theBytesRead;
}

int AvHHud::InitializeDemoPlayback2(int inSize, unsigned char* inBuffer)
{
	// Read in base state 2
	int theBytesRead = 0;

	LOAD_DATA(this->mTimeOfLastUpdate);
	LOAD_DATA(this->mTimeOfNextHudSound);
	LOAD_DATA(this->mTimeOfCurrentUpdate);
	LOAD_DATA(this->mCountDownClock);
	LOAD_DATA(this->mLastTickPlayed);
	LOAD_DATA(this->mNumTicksToPlay);
	LoadStringData(this->mMapName, inBuffer, theBytesRead);

	float theMinViewHeight;
	LOAD_DATA(theMinViewHeight);
	this->mMapExtents.SetMinViewHeight(theMinViewHeight);

	float theMaxViewHeight;
	LOAD_DATA(theMaxViewHeight);
	this->mMapExtents.SetMaxViewHeight(theMaxViewHeight);
	
	float theMinMapX;
	LOAD_DATA(theMinMapX);
	this->mMapExtents.SetMinMapX(theMinMapX);
	
	float theMaxMapX;
	LOAD_DATA(theMaxMapX);
	this->mMapExtents.SetMaxMapX(theMaxMapX);
	
	float theMinMapY;
	LOAD_DATA(theMinMapY);
	this->mMapExtents.SetMinMapY(theMinMapY);
	
	float theMaxMapY;
	LOAD_DATA(theMaxMapY);
	this->mMapExtents.SetMaxMapY(theMaxMapY);
	
	bool theDrawMapBG;
	LOAD_DATA(theDrawMapBG);
	this->mMapExtents.SetDrawMapBG(theDrawMapBG);

	// Clear then load sound names
	int theSoundNameListSize;
	LOAD_DATA(theSoundNameListSize);
	this->mSoundNameList.clear();

	for(int i = 0; i < theSoundNameListSize; i++)
	{
		string theCurrentSoundName;
		LoadStringData(theCurrentSoundName, inBuffer, theBytesRead);
		this->mSoundNameList.push_back(theCurrentSoundName);
	}

	ASSERT((theBytesRead + (int)sizeof(int)) == inSize);
	
	return theBytesRead;
}	

// Write out base HUD data to stream
void AvHHud::InitializeDemoRecording()
{
	// Figure out total size of buffer needed

	// Write number of upgrades, then each upgrade
	// No longer done, but need to add in upgrades for backwards compatibility
	int theUpgrades = 0;
	int theUpgradesSize = sizeof(theUpgrades);

	// Gamma, resources
	int theGammaSize = sizeof(this->mDesiredGammaSlope);
	int theResourcesSizes = sizeof(this->mResources);
    
    // Save commander index (TODO: REMOVE)
    int theCommanderIndex = this->GetCommanderIndex();
	int theCommanderSize = sizeof(theCommanderIndex);

	int theNumHiveInfoRecords = this->mHiveInfoList.size();
	int theHiveInfoSize = sizeof(int) + theNumHiveInfoRecords*sizeof(AvHHiveInfo);

	string thePieMenuControl = gPieMenuHandler.GetPieMenuControl();
	int theCurrentPieMenuControlSize = sizeof(int) + thePieMenuControl.size();
	
	int theSelectedSize = sizeof(int) + this->mSelected.size()*sizeof(EntityInfo);

	int theTotalSize = theUpgradesSize + theGammaSize + theResourcesSizes + theCommanderSize + theHiveInfoSize + theCurrentPieMenuControlSize + theSelectedSize;

	// New a char array of this size
	int theCounter = 0;

	unsigned char* theCharArray = new unsigned char[theTotalSize];
	if(theCharArray)
	{
		// Write out number of upgrades (for backwards-compatibility)
		int theNumUpgradeCosts = 0;
		SaveData(theCharArray, &theNumUpgradeCosts, sizeof(theNumUpgradeCosts), theCounter);

		// Write out gamma
		SaveData(theCharArray, &this->mDesiredGammaSlope, theGammaSize, theCounter);
		SaveData(theCharArray, &this->mResources, theResourcesSizes, theCounter);
		SaveData(theCharArray, &theCommanderIndex, theCommanderSize, theCounter);

		// Write out num hive info records
		SaveData(theCharArray, &theNumHiveInfoRecords, sizeof(int), theCounter);
		for(HiveInfoListType::iterator theHiveInfoIter = this->mHiveInfoList.begin(); theHiveInfoIter != this->mHiveInfoList.end(); theHiveInfoIter++)
		{
			SaveData(theCharArray, &(*theHiveInfoIter), sizeof(AvHHiveInfo), theCounter);
		}

		// Save length of pie menu control name
		int thePieMenuControlNameLength = thePieMenuControl.size();
		SaveData(theCharArray, &thePieMenuControlNameLength, sizeof(int), theCounter);
		SaveData(theCharArray, thePieMenuControl.c_str(), thePieMenuControlNameLength, theCounter);

		// Save out size of selected
		int theNumSelected = this->mSelected.size();
		SaveData(theCharArray, &theNumSelected, sizeof(theNumSelected), theCounter);

		for(EntityListType::const_iterator theSelectedIter = this->mSelected.begin(); theSelectedIter != this->mSelected.end(); theSelectedIter++)
		{
			EntityInfo theCurrentInfo = *theSelectedIter;
			SaveData(theCharArray, &theCurrentInfo, sizeof(EntityInfo), theCounter);
		}

		ASSERT(theCounter == theTotalSize);

		// Write it out
		Demo_WriteBuffer(TYPE_BASESTATE, theTotalSize, theCharArray);

		// Delete char array
		delete [] theCharArray;
		theCharArray = NULL;

		// Save out particle templates
		gParticleTemplateList.InitializeDemoRecording();
	}

	theTotalSize = theCounter = 0;
	theTotalSize += sizeof(this->mTimeOfLastUpdate);
	theTotalSize += sizeof(this->mTimeOfNextHudSound);
	theTotalSize += sizeof(this->mTimeOfCurrentUpdate);
	theTotalSize += sizeof(this->mCountDownClock);
	theTotalSize += sizeof(this->mLastTickPlayed);
	theTotalSize += sizeof(this->mNumTicksToPlay);
	theTotalSize += GetDataSize(this->mMapName);
	theTotalSize += sizeof(this->mMapExtents.GetMinViewHeight());
	theTotalSize += sizeof(this->mMapExtents.GetMaxViewHeight());
	theTotalSize += sizeof(this->mMapExtents.GetMinMapX());
	theTotalSize += sizeof(this->mMapExtents.GetMaxMapX());
	theTotalSize += sizeof(this->mMapExtents.GetMinMapY());
	theTotalSize += sizeof(this->mMapExtents.GetMaxMapY());
	theTotalSize += sizeof(this->mMapExtents.GetDrawMapBG());

	// Save sound names
	int theSoundNameListSize = this->mSoundNameList.size();
	theTotalSize += sizeof(theSoundNameListSize);

	for(int i = 0; i < theSoundNameListSize; i++)
	{
		string theCurrentSoundName = this->mSoundNameList[i];
		theTotalSize += GetDataSize(theCurrentSoundName);
	}
	
	theCharArray = new unsigned char[theTotalSize];
	if(theCharArray)
	{
		SAVE_DATA(this->mTimeOfLastUpdate);
		SAVE_DATA(this->mTimeOfNextHudSound);
		SAVE_DATA(this->mTimeOfCurrentUpdate);
		SAVE_DATA(this->mCountDownClock);
		SAVE_DATA(this->mLastTickPlayed);
		SAVE_DATA(this->mNumTicksToPlay);
		SaveStringData(theCharArray, this->mMapName, theCounter);

		float theMinViewHeight = this->mMapExtents.GetMinViewHeight();
		SAVE_DATA(theMinViewHeight);
		float theMaxViewHeight = this->mMapExtents.GetMaxViewHeight();
		SAVE_DATA(theMaxViewHeight);
		float theMinMapX = this->mMapExtents.GetMinMapX();
		SAVE_DATA(theMinMapX);
		float theMaxMapX = this->mMapExtents.GetMaxMapX();
		SAVE_DATA(theMaxMapX);
		float theMinMapY = this->mMapExtents.GetMinMapY();
		SAVE_DATA(theMinMapY);
		float theMaxMapY = this->mMapExtents.GetMaxMapY();
		SAVE_DATA(theMaxMapY);
		bool theDrawMapBG = this->mMapExtents.GetDrawMapBG();
		SAVE_DATA(theDrawMapBG);

		SAVE_DATA(theSoundNameListSize);
		for(int i = 0; i < theSoundNameListSize; i++)
		{
			string theCurrentSoundName = this->mSoundNameList[i];
			SaveStringData(theCharArray, theCurrentSoundName, theCounter);
		}

		// TODO: Save out locations?

		ASSERT(theCounter == theTotalSize);

		// Write out TYPE_BASESTATE2 chunk separately for backwards-compatibility
		Demo_WriteBuffer(TYPE_BASESTATE2, theTotalSize, theCharArray);

		delete [] theCharArray;
		theCharArray = NULL;
	}

	ScorePanel_InitializeDemoRecording();

	// Write out weapon info
	for(i = 0; i < MAX_WEAPONS; i++)
	{
		WEAPON* theWeapon = gWR.GetWeapon(i);

		theTotalSize = sizeof(theWeapon->szName) + sizeof(theWeapon->iAmmoType) + sizeof(theWeapon->iAmmo2Type) + sizeof(theWeapon->iMax1) + sizeof(theWeapon->iMax2) + sizeof(theWeapon->iSlot) + sizeof(theWeapon->iSlotPos) + sizeof(theWeapon->iFlags) + sizeof(theWeapon->iId) + sizeof(theWeapon->iClip) + sizeof(theWeapon->iCount);// + sizeof(int); // last one is for ammo
		theCharArray = new unsigned char[theTotalSize];

		int theWeaponCoreDataLength = theTotalSize;// - sizeof(int);
		memcpy(theCharArray, theWeapon, theWeaponCoreDataLength); // Everything but ammo
		//int theAmmo = gWR.GetAmmo(theWeapon->iId);
		//memcpy(theCharArray + theWeaponCoreDataLength, &theAmmo, sizeof(int));

		Demo_WriteBuffer(TYPE_WEAPONINFO, theTotalSize, (unsigned char*)theWeapon);

		delete [] theCharArray;
		theCharArray = NULL;
	}
}

int	AvHHud::InitializeWeaponInfoPlayback(int inSize, unsigned char* inBuffer)
{
	// Make sure weapons are cleared out first
	WEAPON theWeapon;
	memset(&theWeapon, 0, sizeof(theWeapon));

	int theWeaponCoreDataLength = inSize;// - sizeof(int);
	memcpy(&theWeapon, inBuffer, theWeaponCoreDataLength);

	if(theWeapon.iId)
	{
		gWR.AddWeapon( &theWeapon);
		//int theAmmo = 0;
		//memcpy(&theAmmo, inBuffer + theWeaponCoreDataLength, sizeof(int));
		//gWR.SetAmmo(theWeapon.iId, theAmmo);
	}

	return inSize;
}

void AvHHud::InitMenu(const string& inMenuName)
{
	PieMenu* theMenu = NULL;
    if(this->GetManager().GetVGUIComponentNamed(inMenuName, theMenu))
	{
		theMenu->AddInputSignalForNodes(&gPieMenuHandler);
		//outMenu->DisableNodesGreaterThanCost(this->mResources);
		this->GetManager().HideComponent(inMenuName);
	}
}

void AvHHud::PostUIInit(void)
{
    this->InitMenu(kSoldierMenu);
	this->InitMenu(kSoldierCombatMenu);
	this->InitMenu(kAlienMenu);
	this->InitMenu(kAlienCombatMenu);
	
	this->InitCommanderMode();
	
    this->GetManager().GetVGUIComponentNamed(kCommanderResourceLabel, this->mCommanderResourceLabel);
    this->GetManager().GetVGUIComponentNamed(kGenericProgress, this->mGenericProgressBar);
    this->GetManager().GetVGUIComponentNamed(kResearchProgress, this->mResearchProgressBar);
	this->GetManager().GetVGUIComponentNamed(kAlienProgress, this->mAlienProgressBar);
    this->GetManager().GetVGUIComponentNamed(kSelectionBox, this->mSelectionBox);
	this->GetManager().GetVGUIComponentNamed(kResearchingLabel, this->mResearchLabel);

    // Init particle editor
	gParticleEditorHandler.Setup();
	
    if(this->GetManager().GetVGUIComponentNamed(kHierarchy, this->mHierarchy))
    {
        this->mHierarchy->setEnabled(false);
    }
	if(this->GetManager().GetVGUIComponentNamed(kShowMapHierarchy, this->mShowMapHierarchy))
    {
        this->mShowMapHierarchy->setEnabled(false);
    }
	
	// Don't turn on gamma by default, it is annoying for testing
	//this->SetGamma();
}

AvHMessageID AvHHud::GetGhostBuilding() const
{
	return this->mGhostBuilding;
}

void AvHHud::SetGhostBuildingMode(AvHMessageID inGhostBuilding)
{
	this->mGhostBuilding = inGhostBuilding;
	this->mCreatedGhost = false;
}

void AvHHud::SetClientDebugCSP(weapon_data_t* inWeaponData, float inNextPlayerAttack)
{
	char theClientInfoString[512];
	sprintf(theClientInfoString, "Client: id: %d, clip: %d, prim attack: %f, idle: %f, next attack: %f", inWeaponData->m_iId, inWeaponData->m_iClip, inWeaponData->m_flNextPrimaryAttack, inWeaponData->m_flTimeWeaponIdle, inNextPlayerAttack);
	
	vgui::Label* theLabel = NULL;
	if(this->GetManager().GetVGUIComponentNamed(kDebugCSPClientLabel, theLabel))
	{
		theLabel->setText(theClientInfoString);
	}
}

void AvHHud::SetCurrentWeaponData(int inCurrentWeaponID, bool inEnabled)
{
	this->mCurrentWeaponID = inCurrentWeaponID;
	this->mCurrentWeaponEnabled = inEnabled;
}

int AvHHud::GetCurrentWeaponID(void)
{
	return this->mCurrentWeaponID;
}

void AvHHud::SetAlienAbility(AvHMessageID inAlienAbility)
{
	this->mAlienAbility = inAlienAbility;
}

void AvHHud::SetProgressStatus(float inPercentage)
{
	if(this->mGenericProgressBar)
	{
		this->mGenericProgressBar->setVisible(false);
	}

	if(this->mAlienProgressBar)
	{
		this->mAlienProgressBar->setVisible(false);
	}

	ProgressBar* theProgressBar = this->mGenericProgressBar;
	if(this->GetIsAlien())
	{
		theProgressBar = this->mAlienProgressBar;
	}

	if(theProgressBar)
	{
		theProgressBar->setVisible(true);
		
		int theNumSegments = theProgressBar->getSegmentCount();
		int theSegment = inPercentage*theNumSegments;
		theProgressBar->setProgress(theSegment);
	}
}

void AvHHud::SetReinforcements(int inReinforcements)
{
	Label* theLabel = NULL;
	if(this->GetManager().GetVGUIComponentNamed(kReinforcementsLabel, theLabel))
	{
		string thePrefix;
		if(LocalizeString(kReinforcementsText, thePrefix))
		{
			string theText = thePrefix + string(" ") + MakeStringFromInt(inReinforcements);
			theLabel->setText(theText.c_str());
		}
	}
}

void AvHHud::SetResearchProgressStatus(float inPercentage)
{
	if(this->mResearchProgressBar)
	{
		this->mResearchProgressBar->setVisible(true);
		
		int theNumSegments = this->mResearchProgressBar->getSegmentCount();
		int theSegment = inPercentage*theNumSegments;
		this->mResearchProgressBar->setProgress(theSegment);
		
		this->mResearchLabel->setVisible(true);
		
		// Center research label
		int theX, theY;
		this->mResearchLabel->getPos(theX, theY);
		int theTextWidth, theTextHeight;
		this->mResearchLabel->getTextSize(theTextWidth, theTextHeight);
		this->mResearchLabel->setPos(ScreenWidth()/2 - theTextWidth/2, theY);
	}
}

void AvHHud::UpdateDemoRecordPlayback()
{
	// If the mouse is visible, allow it to work with demos
	if(gEngfuncs.pDemoAPI->IsRecording())
	{
		// Write out first frame if needed
		if(!this->mRecordingLastFrame)
		{
			this->InitializeDemoRecording();
			this->mRecordingLastFrame = true;
		}

		// Write view origin (and angles?)
		//Demo_WriteVector(TYPE_VIEWANGLES, v_angles);
		//Demo_WriteVector(TYPE_VIEWORIGIN, v_origin);
//		Demo_WriteByte(TYPE_MOUSEVIS, g_iVisibleMouse);
//		
//		if(g_iVisibleMouse)
//		{
//			int theMouseScreenX, theMouseScreenY;
//			
//			IN_GetMousePos(&theMouseScreenX, &theMouseScreenY);
//			
//			//theMouseScreenX += ScreenWidth/2;
//			//theMouseScreenY += ScreenHeight/2;
//			
//			// Write mouse position
//			float theNormX = (float)theMouseScreenX/ScreenWidth;
//			float theNormY = (float)theMouseScreenY/ScreenHeight;
//			
//			Demo_WriteFloat(TYPE_MOUSEX, theNormX);
//			Demo_WriteFloat(TYPE_MOUSEY, theNormY);
//			
//			//char theBuffer[256];
//			//sprintf(theBuffer, "Saving mouse coords %f %f\n", theNormX, theNormY);
//			//CenterPrint(theBuffer);
//		}
	}
	else if(gEngfuncs.pDemoAPI->IsPlayingback())
	{
		//char theBuffer[256];
		//sprintf(theBuffer, "Restoring mouse coords %f %f\n", gNormMouseX, gNormMouseY);
		//CenterPrint(theBuffer);
		
		//int theCurrentMouseX = gNormMouseX*ScreenWidth;
		//int theCurrentMouseY = gNormMouseY*ScreenHeight;
		//
		////App::getInstance()->setCursorPos(theCurrentMouseX, theCurrentMouseY);
		//
		//SetCursorPos(theCurrentMouseX, theCurrentMouseY);
		//
		//this->mMouseCursorX = theCurrentMouseX;
		//this->mMouseCursorY = theCurrentMouseY;

		this->mRecordingLastFrame = false;
	}
	else
	{
		this->mRecordingLastFrame = false;
	}
}

void AvHHud::UpdateDataFromVuser4(float inCurrentTime)
{
	cl_entity_s* theLocalPlayer = gEngfuncs.GetLocalPlayer();
	if(theLocalPlayer)
	{
		// Fetch data from vuser4
		vec3_t theVUser = theLocalPlayer->curstate.vuser4;

		//int theVUserVar0 = 0;
		//int theVUserVar1 = 0;
		//int theVUserVar2 = 0;
		//
		//memcpy(&theVUserVar0, (int*)(&theVUser.x), 4);
		//memcpy(&theVUserVar1, (int*)(&theVUser.y), 4);
		//memcpy(&theVUserVar2, (int*)(&theVUser.z), 4);

		// Fetch new resource level
		//theVUserVar2;// & 0x0000FFFF;

		if(this->GetIsCombatMode())
		{
			this->mExperience = (int)ceil(theVUser.z/kNumericNetworkConstant);					
			this->mExperienceLevel = AvHPlayerUpgrade::GetPlayerLevel(this->mExperience);

			const float kShowMenuInterval = 5.0f;

			// If we are at a level greater then we've drawn, set visible again to reparse
			if(this->mExperienceLevel > this->mExperienceLevelLastDrawn)
			{
				this->DisplayCombatUpgradeMenu(true);
				this->mTimeOfLastLevelUp = inCurrentTime;
				this->mExperienceLevelLastDrawn = this->mExperienceLevel;
			}
			// else if we have no more levels to spend or if we've been displaying it longer then the time out (+1 because we start at level 1)
			else if((this->mExperienceLevel == (this->mExperienceLevelSpent + 1)) || (inCurrentTime > (this->mTimeOfLastLevelUp + kShowMenuInterval)))
			{
				// stop displaying it
				this->DisplayCombatUpgradeMenu(false);
			}
		}
		else
		{
			int theNewValue = (int)ceil(theVUser.z/kNumericNetworkConstant);

			if(theNewValue != this->mResources)
			{
				this->mResources = theNewValue;
			}
			
			// Don't smooth resources nicely when switching targets
			if((g_iUser1 == OBS_IN_EYE) && (g_iUser2 != this->mUser2OfLastResourceMessage))
			{
				this->mVisualResources = this->mResources;
			}
		}
		this->mUser2OfLastResourceMessage = g_iUser2;
	}
}

void AvHHud::UpdateSpectating()
{
    // If we're spectating and the team of our target switched, delete all blips
	if((this->GetPlayMode() == PLAYMODE_AWAITINGREINFORCEMENT) || (this->GetPlayMode() == PLAYMODE_OBSERVER))
	{
		AvHTeamNumber theCurrentTargetTeam = TEAM_IND;
		
		int theCurrentTarget = g_iUser2;//thePlayer->curstate.iuser2;
		cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(theCurrentTarget);
		if(theEntity)
		{
			theCurrentTargetTeam = AvHTeamNumber(theEntity->curstate.team);
		}

		if((theCurrentTargetTeam != this->mLastTeamSpectated) && (theCurrentTargetTeam != TEAM_IND))
		{
			this->mEnemyBlips.Clear();
			this->mFriendlyBlips.Clear();
			this->mLastTeamSpectated = theCurrentTargetTeam;
			//CenterPrint("Clearing blips.");
		}
	}
}

void AvHHud::UpdateCommonUI()
{
	// Find currently selected node and draw help text if it's been open for a little bit
	PieMenu* thePieMenu = NULL;
	AvHMessageID theCurrentNodeID = MESSAGE_NULL;

	this->mSelectingNodeID = MESSAGE_NULL;

	if(this->GetManager().GetVGUIComponentNamed(this->mPieMenuControl, thePieMenu))
	{
		this->UpdateUpgradeCosts();

		PieNode* theCurrentNode = NULL;
		if(thePieMenu && thePieMenu->GetSelectedNode(theCurrentNode))
		{
			theCurrentNodeID = (AvHMessageID)theCurrentNode->GetMessageID();
		
			if(theCurrentNodeID > 0)
			{
				this->mSelectingNodeID = theCurrentNodeID;
			}
		}
	}

	// Clear squad on team change
	if(!this->GetIsMarine())
	{
		this->mCurrentSquad = 0;
	}
}

#define FORCE_CVAR(a,b) if(a)a->value = b;

void AvHHud::UpdateExploitPrevention()
{
	//Note: Sometimes some clients will not have these cvars, so be sure to check that they are not null.
	FORCE_CVAR(gl_monolights, 0.0f);
	FORCE_CVAR(gl_overbright, 0.0f);
	FORCE_CVAR(gl_clear, 0.0f);
	FORCE_CVAR(hud_draw, 1.0f);
	FORCE_CVAR(r_drawviewmodel, 1.0f);
	FORCE_CVAR(cl_movespeedkey, AvHMUGetWalkSpeedFactor(this->GetHUDUser3()));
	FORCE_CVAR(gl_d3dflip, 1.0f);
	FORCE_CVAR(s_show, 0.0f);
	FORCE_CVAR(r_detailtextures, 0.0f);

	if(lightgamma && lightgamma->value < 2.0)
		lightgamma->value = 2.0f;
}

void AvHHud::UpdateAlienUI(float inCurrentTime)
{
	// Always hide it by default
	this->GetManager().HideComponent(kPieHelpText);
	
	if(this->GetIsAlien() && this->GetIsAlive())
	{
		float theTimePassed = inCurrentTime - this->mTimeOfLastUpdate;
//		int theNumFrames = SPR_Frames(this->mAlienUIHiveSprite);
//			
//		for(AnimatedSpriteListType::iterator theIter = this->mAlienUIHiveList.begin(); theIter != this->mAlienUIHiveList.end(); theIter++)
//		{
//			const float kAnimationSpeed = 16.0f;
//		
//			float theCurrentFrame = theIter->mCurrentFrame + theTimePassed*kAnimationSpeed;
//			if(theCurrentFrame >= theNumFrames)
//			{
//				theCurrentFrame -= theNumFrames;
//			}
//			theIter->mCurrentFrame = theCurrentFrame;
//		}
		
		// Check to see if we have any unspent upgrades.  If so, change the pie menu and HUD state to reflect this
		this->mNumUpgradesAvailable = 0;
		for(int i = 0; i < ALIEN_UPGRADE_CATEGORY_MAX_PLUS_ONE; i++)
		{
			this->mCurrentUpgradeCategory[i] = ALIEN_UPGRADE_CATEGORY_INVALID;
		}

		int theUpgradeVar = this->GetLocalUpgrades();

		if(AvHGetHasUpgradeChoiceInCategory(ALIEN_UPGRADE_CATEGORY_SENSORY, this->mUpgrades, theUpgradeVar))
		{
			this->mCurrentUpgradeCategory[this->mNumUpgradesAvailable] = ALIEN_UPGRADE_CATEGORY_SENSORY;
			this->mNumUpgradesAvailable += 1;
		}
		if(AvHGetHasUpgradeChoiceInCategory(ALIEN_UPGRADE_CATEGORY_MOVEMENT, this->mUpgrades, theUpgradeVar))
		{
			this->mCurrentUpgradeCategory[this->mNumUpgradesAvailable] = ALIEN_UPGRADE_CATEGORY_MOVEMENT;
			this->mNumUpgradesAvailable += 1;
		}
		if(AvHGetHasUpgradeChoiceInCategory(ALIEN_UPGRADE_CATEGORY_OFFENSE, this->mUpgrades, theUpgradeVar))
		{
			this->mCurrentUpgradeCategory[this->mNumUpgradesAvailable] = ALIEN_UPGRADE_CATEGORY_OFFENSE;
			this->mNumUpgradesAvailable += 1;
		}
		if(AvHGetHasUpgradeChoiceInCategory(ALIEN_UPGRADE_CATEGORY_DEFENSE, this->mUpgrades, theUpgradeVar))
		{
			this->mCurrentUpgradeCategory[this->mNumUpgradesAvailable] = ALIEN_UPGRADE_CATEGORY_DEFENSE;
			this->mNumUpgradesAvailable += 1;
		}
	}
}

bool AvHHud::GetCommanderLabelText(std::string& outCommanderName) const
{

    int theCommander = this->GetCommanderIndex();
	bool theHasCommander = (theCommander > 0) && (theCommander <= gEngfuncs.GetMaxClients());
	
	if(!theHasCommander)
	{
		LocalizeString(kNoCommander, outCommanderName);
        return false; // No commander
	}
	else
	{

        std::stringstream theStream;

		string theCommanderText;
        LocalizeString(kCommander, theCommanderText);

        theStream << theCommanderText;
        theStream << ": ";
        
		hud_player_info_t thePlayerInfo;
		gEngfuncs.pfnGetPlayerInfo(theCommander, &thePlayerInfo);

		if(thePlayerInfo.name)
		{
			const int kMaxCommNameLen = 8;
			theStream << string(thePlayerInfo.name).substr(0, kMaxCommNameLen);
		}

        outCommanderName = theStream.str();

        return true;

    }

}

void AvHHud::UpdateMarineUI(float inCurrentTime)
{ 
	// Find commander label
	Label* theCommanderStatusLabel = NULL;


	if(this->mMapMode == MAP_MODE_NS)
	{

		// Add handler for all pending request buttons (this does hotgroups too)
		for(int i = 0; i < MESSAGE_LAST; i++)
		{
			AvHMessageID theMessageID = AvHMessageID(i);
				
			char theComponentName[256];
			sprintf(theComponentName, kPendingImpulseSpecifier, (int)theMessageID);
				
			AvHTechImpulsePanel* theTechImpulsePanel = NULL;
			if(this->GetManager().GetVGUIComponentNamed(theComponentName, theTechImpulsePanel))
			{
				bool theVisibility = false;
		
				// Do we have any requests pending?
				if(this->GetIsInTopDownMode())
				{
					PendingRequestListType::iterator theIterator;
					for(theIterator = this->mPendingRequests.begin(); theIterator != this->mPendingRequests.end(); theIterator++)
					{
						if(theIterator->first == theMessageID)
						{
							if(theIterator->second > 0)
							{
								theVisibility = true;
							}
						}
					}

					switch(theMessageID)
					{
					case GROUP_SELECT_1:
					case GROUP_SELECT_2:
					case GROUP_SELECT_3:
					case GROUP_SELECT_4:
					case GROUP_SELECT_5:
					case COMMANDER_SELECTALL:
						theVisibility = true;
						break;
					}
				}
		
				theTechImpulsePanel->setVisible(theVisibility);
			}
		}
	}
}


void AvHHud::UpdateCountdown(float inCurrentTime)
{
	if(this->mCountDownClock != -1)
	{
		if(inCurrentTime - this->mCountDownClock > this->mLastTickPlayed)
		{
			// Play tick
			this->PlayHUDSound(HUD_SOUND_COUNTDOWN);
			this->mLastTickPlayed++;
		}

		if(this->mLastTickPlayed > this->mNumTicksToPlay)
		{
			this->AddTooltip(kGameBegun, false);
			this->mCountDownClock = -1;
		}
	}
}

void AvHHud::UpdateHierarchy()
{

    // Start the starting point on the map to our local position
	this->mOverviewMap.SetMapExtents(this->GetMapName(), this->mMapExtents);
	this->mOverviewMap.SetWorldPosition(gPredictedPlayerOrigin[0], gPredictedPlayerOrigin[1]);
    
    if(this->mHierarchy)
	{
		this->mHierarchy->SetFullScreen(this->GetHUDUser3() == AVH_USER3_COMMANDER_PLAYER);
	}
	if(this->mShowMapHierarchy)
	{
		this->mShowMapHierarchy->SetFullScreen(true);
	}

}

string AvHHud::GetNameOfLocation(vec3_t inLocation) const
{
	string theLocationName;

	AvHSHUGetNameOfLocation(this->mInfoLocationList, inLocation, theLocationName);

	return theLocationName;
}


void AvHHud::UpdateInfoLocation()
{
	if(this->GetIsAlive() && this->GetHUDUser3() != AVH_USER3_ALIEN_EMBRYO)
	{
		// Don't clear our location, disconcerting to see our location disappear
		//this->mLocationText = "";
		
		this->mLocationText = this->GetNameOfLocation(gPredictedPlayerOrigin);
	}
	else
	{
		this->mLocationText = "";
	}
}

bool AvHHud::GetIsCombatMode() const
{
	return (this->mMapMode == MAP_MODE_CO);
}

bool AvHHud::GetIsNSMode() const
{
    return (this->mMapMode == MAP_MODE_NS);
}

bool AvHHud::GetIsMouseInRegion(int inX, int inY, int inWidth, int inHeight)
{
	bool theMouseIsInRegion = false;

	if(this->GetInTopDownMode())
	{
		int theMouseX, theMouseY;
		this->GetMousePos(theMouseX, theMouseY);
		
		if((theMouseX >= inX) && (theMouseX <= (inX + inWidth)))
		{
			if((theMouseY >= inY) && (theMouseY <= (inY + inHeight)))
			{
				theMouseIsInRegion = true;
			}
		}
	}

	return theMouseIsInRegion;
}

void AvHHud::TraceEntityID(int& outEntityID)
{
	pmtrace_t tr;
	vec3_t up, right, forward;

	// Trace forward to see if we see a player
	gEngfuncs.pEventAPI->EV_PushPMStates();

	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers (-1);	

	gEngfuncs.pEventAPI->EV_SetTraceHull(2);

	cl_entity_s* theLocalPlayer = gEngfuncs.GetLocalPlayer();
	int theLocalPlayerIndex = theLocalPlayer->index;
	//AngleVectors(theLocalPlayer->curstate.angles, forward, right, up);

	pVector theRealView;
	gEngfuncs.pfnAngleVectors(pmove->angles, forward, right, up);

	Vector theStartTrace;
	//VectorMA(gPredictedPlayerOrigin, kMaxPlayerHullWidth, forward, theStartTrace);
	VectorCopy(gPredictedPlayerOrigin, theStartTrace);

	Vector theEndTrace;
	VectorMA(gPredictedPlayerOrigin, 8192, forward, theEndTrace);
	bool theDone = false;

	do
	{
		gEngfuncs.pEventAPI->EV_PlayerTrace(theStartTrace, theEndTrace, PM_NORMAL, -1, &tr);
		
		// Ignore local player, and ignore the player we're spectating
		int theHit = gEngfuncs.pEventAPI->EV_IndexFromTrace(&tr);
		if(theHit == theLocalPlayerIndex)
		{
			VectorMA(tr.endpos, kHitOffsetAmount, forward, theStartTrace);
		}
		// We hit something
		else if(tr.fraction < 1.0)
		{
			physent_t* pe = gEngfuncs.pEventAPI->EV_GetPhysent( tr.ent ); 
			if(pe)
			{
				int thePotentialEntity = pe->info;
				//if((thePotentialEntity >= 1) && (thePotentialEntity < gEngfuncs.GetMaxClients()))
				//{
					outEntityID = pe->info;
				//}
			}
			theDone = true;
		}
	} while(!theDone);
	
	gEngfuncs.pEventAPI->EV_PopPMStates();
}

bool AvHHud::GetAlienHelpForMessage(int inMessageID, string& outHelpText, int& outPointCost) const
{
	bool theSuccess = false;

	char theNumber[8];
	sprintf(theNumber, "%d", inMessageID);
	string theNumberString(theNumber);
	
	string theTechHelpText;
	string theKey = string(kTechNodeHelpPrefix) + theNumberString;
	if(LocalizeString(theKey.c_str(), theTechHelpText))
	{
		string thePointString;
		if(LocalizeString(kPointsSuffix, thePointString))
		{
/*
			// Lookup point cost
			for(UpgradeCostListType::const_iterator theIter = this->mPersonalUpgradeCosts.begin(); theIter != this->mPersonalUpgradeCosts.end(); theIter++)
			{
				if(theIter->first == inMessageID)
				{
					char theCostSuffix[128];
					sprintf(theCostSuffix, " (%d %s)", theIter->second, thePointString.c_str());
					outPointCost = theIter->second;
					theTechHelpText += string(theCostSuffix);
					outHelpText = theTechHelpText;
					theSuccess = true;
					break;
				}
			}
*/
		}
	}
	return theSuccess;
}

bool AvHHud::GetDoesPlayerHaveOrder() const
{
	bool thePlayerHasOrder = false;
	
	for(OrderListType::const_iterator theIter = this->mOrders.begin(); theIter != this->mOrders.end(); theIter++)
	{
		// Draw the order if the order is for any plays that are in our draw player list
		EntityInfo theReceiverPlayer = theIter->GetReceiver();
		
		cl_entity_s* thePlayer = gEngfuncs.GetLocalPlayer();
		if(thePlayer)
		{
			int thePlayerIndex = thePlayer->index;
			if (thePlayerIndex == theReceiverPlayer )
			{
				thePlayerHasOrder = true;
				break;
			}
		}
	}

	return thePlayerHasOrder;
}

		
bool AvHHud::GetHelpForMessage(int inMessageID, string& outHelpText) const
{
	bool theSuccess = false;
	
	char theNumber[8];
	sprintf(theNumber, "%d", (int)inMessageID);
	string theNumberString(theNumber);

	string theKey = string(kTechNodeLabelPrefix) + theNumberString;
	string theTechNodeLabel;
	if(LocalizeString(theKey.c_str(), theTechNodeLabel))
	{
		theKey = string(kTechNodeHelpPrefix) + theNumberString;
		string theTechNodeHelp;
		if(LocalizeString(theKey.c_str(), theTechNodeHelp))
		{
			outHelpText = /*theTechNodeLabel + " " +*/ theTechNodeLabel;
			theSuccess = true;
			
			int theCost;
			bool theResearchable;
			float theTime;
			this->mTechNodes.GetResearchInfo((AvHMessageID)(inMessageID), theResearchable, theCost, theTime);
			
			// Add cost
			if(theCost > 0)
			{
				string theCostString;
				if(AvHSHUGetDoesTechCostEnergy((AvHMessageID)inMessageID))
				{
					LocalizeString(kEnergyPrefix, theCostString);
				}
				else
				{
					LocalizeString(kMessageButtonCost, theCostString);
				}

				outHelpText += "  ";
				outHelpText += theCostString;
				outHelpText += " ";
				outHelpText += MakeStringFromInt(theCost);

				// Draw description below
				//outHelpText += "\n";
				//outHelpText += theTechNodeHelp;
			}
		}
	}

	return theSuccess;
}

void AvHHud::UpdateMusic(float inCurrentTime)
{
	bool theMusicEnabled = false;

	if(this->GetGameStarted())
	{
		// If we're entering play mode and music is enabled, allow playing of music
		if(this->GetHUDPlayMode() != PLAYMODE_READYROOM && (cl_musicenabled->value == 1.0f) && (cl_musicvolume->value > 0))
		{
			// Disable music when debugging
//			#ifndef DEBUG
			theMusicEnabled = true;
//			#endif
		}
        else
        {
            int a = 0;
        }
	}

	this->SetMusicAllowed(theMusicEnabled);

	UIHud::UpdateMusic(inCurrentTime);
}

void AvHHud::UpdatePieMenuControl()
{
	// Set which pie menu to use ("" for none)
	bool theScoreBoardIsOpen = false;
	
	ScorePanel* theScoreBoard = gViewPort->GetScoreBoard();
	if(theScoreBoard && theScoreBoard->isVisible())
	{
		theScoreBoardIsOpen = true;
	}
	
	if(theScoreBoardIsOpen)
	{
		AvHPieMenuHandler::SetPieMenuControl("");
	}
	else
	{
		AvHPieMenuHandler::SetPieMenuControl(this->mPieMenuControl);
	}

    // Clear all nodes in case VGUI didn't catch events (seems to happen with lag)
    if(!gPieMenuHandler.GetIsPieMenuOpen())
    {
        PieMenu* theCurrentPieMenu = gPieMenuHandler.GetActivePieMenu();
        if(theCurrentPieMenu)
        {
            theCurrentPieMenu->SetFadeState(false);
        }
    }

    // If we're dead, make sure the popup menu is closed
    if(!this->GetIsAlive(false))
    {
        gPieMenuHandler.ClosePieMenu();
    }
}

bool AvHHud::GetEntityInfoString(int inEntityID, string& outEntityInfoString, bool& outIsEnemy)
{
	bool theSuccess = false;
	bool theIsEnemy = false;

	cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(inEntityID);
	if(theEntity)
	{
		AvHTeamNumber theTeam = this->GetHUDTeam();

		if((inEntityID >= 1) && (inEntityID <= gEngfuncs.GetMaxClients()))
		{
			hud_player_info_t thePlayerInfo;
			gEngfuncs.pfnGetPlayerInfo(inEntityID, &thePlayerInfo);

			string thePrePendString;

			// Don't show cloaked enemies
			if((theTeam != TEAM_IND) && !this->GetInTopDownMode() && ((theEntity->curstate.team == theTeam) || (theEntity->curstate.rendermode == kRenderNormal)))
			{
				//string thePostPendString;
				if((theEntity->curstate.team == theTeam))
				{
					//LocalizeString(kFriendText, thePrePendString);
					//sprintf(thePostPendString, " (health: %d)", theEntity->curstate.health);
				}
				else
				{
					LocalizeString(kEnemyText, thePrePendString);
					theIsEnemy = true;
				}

				if(thePrePendString != "")
				{
					outEntityInfoString = thePrePendString + " - ";
				}
			}

			if(thePlayerInfo.name)
			{
				outEntityInfoString += thePlayerInfo.name;// + thePostPendString;

				// Get string from status bar and append it
				const char* theStatusCStr = this->m_StatusBar.GetStatusString();
				if(strlen(theStatusCStr) > 0)
				{
					outEntityInfoString += string(theStatusCStr);
				}

				outIsEnemy = theIsEnemy;
				theSuccess = true;
			}
			
			//	sprintf(thePlayerName, "%s (health: %d)", thePlayerInfo.name, thePlayer->curstate.health);
			//}
		}
		else
		{
			bool theAutoHelpEnabled = gEngfuncs.pfnGetCvarFloat(kvAutoHelp);

			// Return the type of thing it is
			AvHUser3 theUser3 = (AvHUser3)(theEntity->curstate.iuser3);

			if(this->GetTranslatedUser3Name(theUser3, outEntityInfoString))
			{
				if((theEntity->curstate.team != TEAM_IND) && (theEntity->curstate.team != theTeam))
				{
					outIsEnemy = true;
				}

				// Upper case first character
				if(outEntityInfoString.length() > 0)
				{
					int theFirstChar = outEntityInfoString[0];
					int theUpperFirstChar = toupper(theFirstChar);
					outEntityInfoString[0] = theUpperFirstChar;
				}

				// Assume status bar set to health/armor/info data
				const char* theStatusCStr = this->m_StatusBar.GetStatusString();
				bool theHasStatusString = (strlen(theStatusCStr) > 0);
				if(theHasStatusString)
				{
					outEntityInfoString += string(theStatusCStr);
				}

				if(theAutoHelpEnabled)
				{
					string theDescription;
					bool theIsFriendly = (this->GetHUDTeam() == theEntity->curstate.team);
					if(this->GetTranslatedUser3Description(theUser3, theIsFriendly, theDescription))
					{
						outEntityInfoString += " - " + theDescription;
					}
				}

				// Only display help when asked for or important
				if(theAutoHelpEnabled || theHasStatusString)
				{
					theSuccess = true;
				}
			}
		}
	}
	
	return theSuccess;
}

void AvHHud::UpdateEntityID(float inCurrentTime)
{
	this->mBuildingEffectsEntityList.clear();

	bool theSetHelpMessage = false;
	bool theSetReticleMessage = false;

	//if(this->GetIsAlive())
	//{
		//char* theNewPlayerName = NULL;
		int theEntityIndex = -1;
		
		if(this->GetHUDUser3() == AVH_USER3_COMMANDER_PLAYER)
		{
			int theCurrentX, theCurrentY;
			this->GetMousePos(theCurrentX, theCurrentY);

			// Don't show help when mouse is over the UI
			float theNormX = ((float)theCurrentX)/(ScreenWidth());
			float theNormY = ((float)theCurrentY)/(ScreenHeight());
			if(!this->GetIsRegionBlockedByUI(theNormX, theNormY))
			{
				Vector theNormRay;
				
				CreatePickingRay(theCurrentX, theCurrentY, theNormRay);
				
				// Look for player and entities under/near the mouse
				AvHSHUGetEntityAtRay(this->GetVisualOrigin(), theNormRay, theEntityIndex);
			}
			else
			{
				int a = 0;
			}
		}
		else if(g_iUser1 != OBS_IN_EYE)
		{
			// Trace entity id in front of us
			this->TraceEntityID(theEntityIndex);
		}
		
		if(this->mSelectingWeaponID != -1)
		{
			// Look up help, set our current help to it
			string theWeaponHelpKey;
			sprintf(theWeaponHelpKey, kWeaponHelpText, this->mSelectingWeaponID);
			
			string theHelpText;
			if(LocalizeString(theWeaponHelpKey.c_str(), theHelpText))
			{
				// Get damage amount from weapon
				ASSERT(this->mSelectingWeaponID >= 0);
				ASSERT(this->mSelectingWeaponID < 32);
				WEAPON* theWeapon = gWR.GetWeapon(this->mSelectingWeaponID);
				int theDamage = theWeapon->iMax2;

				char theHelpTextWithDamage[1024];
				sprintf(theHelpTextWithDamage, theHelpText.c_str(), theDamage);

				this->SetHelpMessage(theHelpTextWithDamage);
				theSetHelpMessage = true;
			}
		}
		else if(this->mTechHelpText != "")
		{
			this->SetHelpMessage(this->mTechHelpText, false, .8f, .6f);
			theSetHelpMessage = true;
		}
		else if(this->mSelectingNodeID > 0)
		{
			char theNumber[8];
			sprintf(theNumber, "%d", this->mSelectingNodeID);
			string theNumberString(theNumber);
			
			string theKey = string(kTechNodeHelpPrefix) + theNumberString;
			if(LocalizeString(theKey.c_str(), this->mReticleInfoText))
			{
				string theCostString;
				if(LocalizeString(kCostMarker, theCostString))
				{
					string thePointsString;
					if(LocalizeString(kPointsSuffix, thePointsString))
					{
/*
						// Lookup point cost
						for(UpgradeCostListType::iterator theIter = this->mPersonalUpgradeCosts.begin(); theIter != this->mPersonalUpgradeCosts.end(); theIter++)
						{
							if(theIter->first == this->mSelectingNodeID)
							{
								this->mSelectedNodeResourceCost = theIter->second;
								
								char theCostSuffix[128];
								sprintf(theCostSuffix, "  %s: %d %s", theCostString.c_str(), this->mSelectedNodeResourceCost, thePointsString.c_str());
								this->mReticleInfoText += string(theCostSuffix);
								break;
							}
						}
*/
					}
					
					// Don't draw marine sayings, as they are printed on the controls and the aliens are using the titles.txt entries
					if(this->GetIsMarine())
					{
						switch(this->mSelectingNodeID)
						{
						case SAYING_1:
						case SAYING_2:
						case SAYING_3:
						case SAYING_4:
						case SAYING_5:
						case SAYING_6:
						case SAYING_7:
						case SAYING_8:
						case SAYING_9:
							this->mReticleInfoText = "";
							break;
						}
					}
					
					this->SetHelpMessage(this->mReticleInfoText);
					theSetHelpMessage = true;
				}
			}
		}
		
		if(theEntityIndex > 0)
		{
			// Don't draw info for cloaked structures
			cl_entity_s* theProgressEntity = gEngfuncs.GetEntityByIndex(theEntityIndex);
			if(theProgressEntity)
			{
				if((theProgressEntity->curstate.rendermode != kRenderTransTexture) || (theProgressEntity->curstate.renderamt > kAlienStructureCloakAmount))
				{
					if(std::find(this->mBuildingEffectsEntityList.begin(), this->mBuildingEffectsEntityList.end(), theEntityIndex) == this->mBuildingEffectsEntityList.end())
					{
						this->mBuildingEffectsEntityList.push_back(theEntityIndex);
					}
											
					bool theEntityIsPlayer = ((theEntityIndex > 0) && (theEntityIndex <= gEngfuncs.GetMaxClients()));
					
					string theHelpText;
					bool theIsEnemy = false;
					
					if(this->GetEntityInfoString(theEntityIndex, theHelpText, theIsEnemy))
					{
						// Set color to red if enemy, otherise to our HUD color
						int theR, theG, theB;
						if(theIsEnemy)
						{
							theR = 255;
							theG = theB = 0;
						}
						else
						{
							this->GetPrimaryHudColor(theR, theG, theB, true, false);
						}

						// If we have auto help on, or we're in top down mode, display as help
						if(gEngfuncs.pfnGetCvarFloat(kvAutoHelp) || this->GetInTopDownMode())
						{
							this->SetHelpMessage(theHelpText);
							
							this->mHelpMessage.SetRGB(theR, theG, theB);

							theSetHelpMessage = true;
						}
						// else, display as reticle message
						else
						{
							this->SetReticleMessage(theHelpText);
							
							this->mReticleMessage.SetRGB(theR, theG, theB);
							
							theSetReticleMessage = true;
						}
					}
				}
				else
				{
					//char theMessage[128];
					//sprintf(theMessage, "Entity %d cloaked, not drawing circle.", theProgressEntity->curstate.iuser3);
					//CenterPrint(theMessage);
				}
			}
			else
			{
				int a = 0;
			}
		}
		
		//if(this->mReticleInfoColorA == 0)
		//{
		//	this->mReticleInfoText = "";
		//}
	//}

	float theTimePassed = (inCurrentTime - this->mTimeOfLastUpdate);
	if(!theSetHelpMessage)
	{
		this->mHelpMessage.FadeText(theTimePassed, true);
	}
	if(!theSetReticleMessage)
	{
		this->mReticleMessage.FadeText(theTimePassed, true);
	}
}

int	AvHHud::GetMenuTechSlots() const
{
	return this->mMenuTechSlots;
}

const AvHTechNodes& AvHHud::GetTechNodes() const
{
	return this->mTechNodes;
}

void AvHHud::GetTooltipDrawingInfo(float& outNormX, float& outNormY) const
{
	outNormX = kHelpMessageLeftEdgeInset;
	outNormY = kHelpMessageTopEdgeInset;

	if(this->GetPlayMode() == PLAYMODE_READYROOM)
	{
		outNormY = 1.0f;
	}
	else if(this->GetInTopDownMode())
	{
		outNormY = kHelpMessageCommanderTopEdgeInset;
	}
	else if(this->GetIsAlien())
	{
		outNormY = kHelpMessageAlienTopEdgeInset;
	}

	outNormY -= kToolTipVerticalSpacing;
}

string AvHHud::GetRankTitle(bool inShowUnspentLevels) const
{
	string theText;

	char* theTeamName = this->GetIsMarine() ? "Marine" : "Alien";
	int theCurrentLevel = this->GetHUDExperienceLevel();

	char theCharArray[512];
	sprintf(theCharArray, kRankText, theTeamName, theCurrentLevel);
	LocalizeString(theCharArray, theText);

	// Add unspent levels, if any
	int theUnspentLevels = (this->mExperienceLevel - this->mExperienceLevelSpent - 1);
	if(inShowUnspentLevels && (theUnspentLevels > 0))
	{
		// We can't accurately draw player's unspent levels when we're dead, as we only know our own
		if(this->GetIsAlive(false))
		{
			string theUnspentLevelText = "(+" + MakeStringFromInt(theUnspentLevels) + ")";
			theText += theUnspentLevelText;
		}
	}
	
	return theText;
}

void AvHHud::GetReticleTextDrawingInfo(float& outNormX, float& outNormY, bool& outCentered) const
{
	outCentered = false;
	
	if(this->GetInTopDownMode())
	{
		int theCurrentX, theCurrentY;
		this->GetMousePos(theCurrentX, theCurrentY);
		outNormX = theCurrentX/(float)ScreenWidth();
		outNormY = theCurrentY/(float)ScreenHeight();

		// Move text up a bit so it doesn't obscure
		outNormY -= .1f;

		outCentered = true;
	}
	else
	{
		if(gEngfuncs.pfnGetCvarFloat(kvCenterEntityID))
		{
			outNormX = .5f;
			outNormY = .5f;
			outCentered = true;
		}
		else
		{
			outNormX = kHelpMessageLeftEdgeInset;
			outNormY = kHelpMessageTopEdgeInset - kReticleMessageHeight;

			// Alien HUD forces this to be inset a bit
			if(this->GetIsAlien())
			{
				outNormX = kHelpMessageAlienLeftedgeInset + kReticleMessageAlienLeftInset;
				outNormY = kHelpMessageAlienTopEdgeInset - kReticleMessageHeight;
			}
		}
	}
}

// Assumes that the tooltips aren't centered
void AvHHud::UpdateTooltips(float inCurrentTime)
{
	float theReticleX;
	float theReticleY;
	this->GetTooltipDrawingInfo(theReticleX, theReticleY);
	
	float theBottomMostY = theReticleY;
	
	float theTimePassed = inCurrentTime - this->mTimeOfLastUpdate;
	float kMovement = 1.0f*theTimePassed;
	
	int theBottomScreenY = theBottomMostY*ScreenHeight();
	
	// Clear all tooltips on a role change, or on a playmode change
	AvHUser3 theCurrentUser3 = this->GetHUDUser3();
	AvHTeamNumber theCurrentTeam = this->GetHUDTeam();
	AvHPlayMode thePlayMode = this->GetPlayMode();

	if((theCurrentUser3 != this->mLastUser3) || (this->mLastTeamNumber != theCurrentTeam) || ((thePlayMode != this->mLastPlayMode) && (this->mLastPlayMode != PLAYMODE_UNDEFINED)) )
	{
		this->mTooltips.clear();
	}

	// Stuff to get reset on a team change
	if(this->mLastTeamNumber != theCurrentTeam)
	{
		this->mExperienceLevelLastDrawn = 1;
	}
	
	// Run through list, dropping them down as far as they can go.  Assumes the first one in the list is the bottommost one
	for(AvHTooltipListType::iterator theIter = this->mTooltips.begin(); theIter != this->mTooltips.end(); /* no inc */)
	{
		// Recompute settings, if it hasn't been updated before
		theIter->RecomputeIfNeccessary();
		
		// Get height of current tool tip
		int theTooltipScreenHeight = theIter->GetScreenHeight();
		
		// Move it down towards the current limit
		float theCurrentMinScreenY = theIter->GetNormalizedScreenY();
		int theCurrentTipScreenBottom = (int)(theCurrentMinScreenY) + theTooltipScreenHeight;
		
		float theCurrentMaxNormY = (float)(theBottomScreenY - theTooltipScreenHeight)/ScreenHeight();
		float theNewNormY = min(theCurrentMaxNormY, (theCurrentMinScreenY + kMovement));

		// Now this is a crazy bug!  In release mode without the following two statements, theNewNormY isn't > theCurrentMinScreenY when it should be.
		// It's as if a little time is needed between the computation of theNewNormY and using theNewNormY for it to work
		char theMessage[256];
		sprintf(theMessage, "theNewNormY %f, minScreenY: %f)", theNewNormY, theCurrentMinScreenY);
		//CenterPrint(theMessage);
		
		// If we moved it down
		if(theNewNormY > theCurrentMinScreenY)
		{
			// Fade it in while it's dropping
			theIter->FadeText(theTimePassed, false);
		}
		else
		{
			if(theIter == this->mTooltips.begin())
			{
				// If it's at the bottom of the list, start fading it out
				theIter->FadeText(theTimePassed, true);
			}
		}
		
		// Set the new position
		theIter->SetNormalizedScreenY(theNewNormY);
		
		// Subtract it's height to the current limit
		theBottomScreenY -= theTooltipScreenHeight;

		// Subtract out small spacing amount
		theBottomScreenY -= max(1, kToolTipVerticalSpacing*ScreenHeight());
		
		// If it's totally faded out, remove it from the list, else process next
		int theAlpha = theIter->GetA();
		if(theAlpha == 0)
		{
			theIter = this->mTooltips.erase(theIter);
		}
		else
		{
			theIter++;
		}

		//char theTempBuffer[256];
		//sprintf(theTempBuffer, "UpdateTooltips, alpha: %d\n", theAlpha);
		//CenterPrint(theTempBuffer);
	}

	// Update combat upgrade menu too
	if(this->GetIsCombatMode())
	{
		const int kScrollDirection = this->mDrawCombatUpgradeMenu ? 1 : -1;
		const float kScrollingSpeed = .7f*kScrollDirection;
		
		float theXDiff = (theTimePassed*kScrollingSpeed);
		const float kNormalizedWidth = this->mCombatUpgradeMenu.GetNormalizedMaxWidth();

		const float kLeftEdgeInset = .02f;
		float theNewX = this->mCombatUpgradeMenu.GetNormalizedScreenX() + theXDiff;
		theNewX = max(-kNormalizedWidth, theNewX);
		theNewX = min(theNewX, kLeftEdgeInset);

		this->mCombatUpgradeMenu.SetNormalizedScreenX(theNewX);
	}
}

void AvHHud::UpdateStructureNotification(float inCurrentTime)
{
	const float kTimeToDisplayIcon = 6.0f;
	const int kMaxIcons = 5;

	for(StructureHUDNotificationListType::iterator theIter = this->mStructureNotificationList.begin(); theIter != this->mStructureNotificationList.end(); /* no inc */)
	{
		if((inCurrentTime > (theIter->mTime + kTimeToDisplayIcon)) || (this->mStructureNotificationList.size() > kMaxIcons))
		{
			theIter = this->mStructureNotificationList.erase(theIter);
		}
		else
		{
			theIter++;
		}
	}
}


//void AvHHud::FadeText(float inCurrentTime, bool inFadeDown)
//{
//	const float kReticleInfoFadeUpSpeed = 500;
//	const float kReticleInfoFadeDownSpeed = -150;
//	
//	// Fade reticle nicely
//	int theFadeSpeed = inFadeDown ? kReticleInfoFadeDownSpeed : kReticleInfoFadeUpSpeed;
//
//	float theTimePassed = (inCurrentTime - this->mTimeOfLastUpdate);
//	float theNewAlpha = this->mReticleInfoColorA + theTimePassed*theFadeSpeed;
//	this->mReticleInfoColorA = max(0, min(255, theNewAlpha));
//}

float AvHHud::GetTimeOfLastUpdate() const
{
	return this->mTimeOfLastUpdate;
}

void AvHHud::UpdateProgressBar()
{
	this->HideProgressStatus();
	
	float thePercentage;
	if(gMiniMap.GetIsProcessing(&thePercentage))
	{
		if(gMiniMap.WriteSpritesIfJustFinished())
		{
			this->HideProgressStatus();
		}
		else
		{
			this->SetProgressStatus(thePercentage);
		}
	}
//	else if(this->mProgressBarEntityIndex == -1)
//	{
//		this->HideGenericProgressStatus();
//	}
	else
	{
		// Look up entity, and set progress according to it's fuser1
		cl_entity_s* theProgressEntity = gEngfuncs.GetEntityByIndex(this->mProgressBarEntityIndex);
		if(theProgressEntity)
		{
			ASSERT(this->mProgressBarParam >= 1);
			ASSERT(this->mProgressBarParam <= 4);

			float theProgress = 0.0f;
			switch(this->mProgressBarParam)
			{
			case 1:
				theProgress = theProgressEntity->curstate.fuser1;
				break;
			case 2:
				theProgress = theProgressEntity->curstate.fuser2;
				break;
			case 3:
				theProgress = theProgressEntity->curstate.fuser3;
				break;
			case 4: // NOTE: check delta.lst for fuser4, it isn't propagated currently
				theProgress = theProgressEntity->curstate.fuser4;
				break;
			}

			if((this->GetHUDUser3() == AVH_USER3_ALIEN_EMBRYO) || this->GetIsDigesting())
			{
				theProgress = pmove->fuser3;
			}
			
			thePercentage = theProgress/kNormalizationNetworkFactor;
			if(thePercentage < 1.0f)
			{
				this->SetProgressStatus(thePercentage);
			}
//			else
//			{
//				this->HideGenericProgressStatus();
//			}
		}
		else
		{
			// Look at selection.  If selection has research and research isn't done, draw research bar.  Else, hide research bar
			if(this->mSelected.size() == 1)
			{
				cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(*this->mSelected.begin());
				if(theEntity)
				{
					this->HideProgressStatus();
					this->HideResearchProgressStatus();

					bool theIsBuilding, theIsResearching;
					float thePercentage;
					AvHSHUGetBuildResearchState(theEntity->curstate.iuser3, theEntity->curstate.iuser4, theEntity->curstate.fuser1, theIsBuilding, theIsResearching, thePercentage);
					
					if(theIsBuilding && (thePercentage > 0.0f) && (thePercentage < 1.0f))
					{
						// Turned off progress bar now that we have circular build icons
						//this->SetGenericProgressStatus(thePercentage);
					}
					else if(theIsResearching && (thePercentage > 0) && (thePercentage < 1.0f))
					{
						this->SetResearchProgressStatus(thePercentage);
					}
				}
			}
		}
	}
}

void AvHHud::GhostBuildingCallback(struct tempent_s* inEntity, float inFrametime, float inCurrentTime)
{
	if(this->mGhostBuilding != MESSAGE_NULL)
	{
		// Don't let it die
		inEntity->die = gClientTimeLastUpdate + 2.0f;
		
		// Update position to be where mouse is
		VectorCopy(this->mGhostWorldLocation, inEntity->entity.origin);
		
		// Visually indicate whether this is a valid position or not
		if(this->mCurrentGhostIsValid)
		{
			inEntity->entity.curstate.renderfx = kRenderFxGlowShell;
			inEntity->entity.curstate.rendercolor.r = 0;
			inEntity->entity.curstate.rendercolor.g = 255;
			inEntity->entity.curstate.rendercolor.b = 0;
			inEntity->entity.curstate.renderamt = kShellRenderAmount;
		}
		else
		{
			inEntity->entity.curstate.renderfx = kRenderFxGlowShell;
			inEntity->entity.curstate.rendercolor.r = 255;
			inEntity->entity.curstate.rendercolor.g = 0;
			inEntity->entity.curstate.rendercolor.b = 0;
			inEntity->entity.curstate.renderamt = kShellRenderAmount;
		}
	}
	else
	{
		// Kill it off immediately
		inEntity->die = gClientTimeLastUpdate;
	}
}

void AvHHud::UpdateBuildingPlacement()
{
	if(this->mGhostBuilding != MESSAGE_NULL)
	{
		// Fetch current mouse up/down state from gScrollHandler and store
		bool theMouseOneDown = gScrollHandler.GetMouseOneDown() && !gCommanderHandler.GetMouseOneDown();
		bool theMouseTwoDown = gScrollHandler.GetMouseTwoDown();
		
		// If we haven't created the temp entity, create it
		if(!this->mCreatedGhost)
		{
			// Create the temporary entity
			int theModelIndex;
			char* theModelName = AvHSHUGetBuildTechModelName(this->mGhostBuilding);
			if(theModelName)
			{
				if(this->mLastGhostBuilding)
				{
					this->mLastGhostBuilding->die = -1;
					this->mLastGhostBuilding = NULL;
				}

				vec3_t theOrigin = this->GetVisualOrigin();
				struct model_s* theModel = gEngfuncs.CL_LoadModel(theModelName, &theModelIndex);
				TEMPENTITY* theTempEntity = gEngfuncs.pEfxAPI->CL_TentEntAllocCustom(gPredictedPlayerOrigin, theModel, 0, CLinkGhostBuildingCallback);
				if(theTempEntity)
				{
					theTempEntity->die += 10.0f;
					theTempEntity->flags |= FTENT_PERSIST;
					// Temp entities interpret baseline origin as velocity.
					VectorCopy(vec3_origin, theTempEntity->entity.baseline.origin);

					// Set special properties for some models
					if(this->mGhostBuilding == BUILD_SCAN)
					{
						theTempEntity->entity.curstate.rendermode = kRenderTransAdd;
						theTempEntity->entity.curstate.renderamt = 255;

						theTempEntity->entity.baseline.rendermode = kRenderTransAdd;
						theTempEntity->entity.baseline.renderamt = 255;
					}

					this->mCreatedGhost = true;
				}

				this->mLastGhostBuilding = theTempEntity;
			}
		}

		// Update location we draw ghosted entity
		int theMouseX, theMouseY;
		this->GetMousePos(theMouseX, theMouseY);

		Vector theNormMousePos;
		CreatePickingRay(theMouseX, theMouseY, theNormMousePos);

        //char theMessage[256];
        //sprintf(theMessage, "Ghost: %f, %f, %f", this->mGhostWorldLocation[0], this->mGhostWorldLocation[1],this->mGhostWorldLocation[2]);
		//CenterPrint(theMessage);
		
		// Was either mouse button pressed?
		bool theMouseOneReleased = (theMouseOneDown && !this->mMouseOneDown);
		bool theMouseTwoReleased = (theMouseTwoDown && !this->mMouseTwoDown);
		if(theMouseOneReleased)
		{
			VectorCopy(this->mLeftMouseWorldStart, this->mNormBuildLocation);
		}
		else if(theMouseTwoReleased)
		{
			VectorCopy(this->mRightMouseWorldStart, this->mNormBuildLocation);
		}

		// Test to see if we're in a valid position
		this->mCurrentGhostIsValid = false;
		
		vec3_t theLocation;
		if(AvHSHUTraceAndGetIsSiteValidForBuild(this->mGhostBuilding, this->GetVisualOrigin(), theNormMousePos, &theLocation))
		{
			// Update with cost and research info
			bool theIsResearchable;
			int theCost;
			float theTime;
			if(this->mTechNodes.GetResearchInfo(this->mGhostBuilding, theIsResearchable, theCost, theTime))
			{
				// Ghost is valid if message available and 
					// we have enough resources OR 
					// tech takes energy and we have enough energy
				this->mCurrentGhostIsValid = false;

				if(this->mTechNodes.GetIsMessageAvailable(this->mGhostBuilding))
				{
					bool theTakesEnergy = AvHSHUGetDoesTechCostEnergy(this->mGhostBuilding);
					if((theCost <= this->mResources) || (theTakesEnergy))
					{
						this->mCurrentGhostIsValid = true;
					}
				}
			}
		}

		// Draw at selection start range * invalid range multiplier if invalid placement, draw on at target location if valid
		//VectorMA(this->GetVisualOrigin(), kSelectionStartRange*kBuildInvalidRangeMultiplier, theNormMousePos, this->mGhostWorldLocation);
		VectorMA(this->GetVisualOrigin(), kSelectionStartRange*8, theNormMousePos, this->mGhostWorldLocation);
		//if(this->mCurrentGhostIsValid)
		//{
			VectorCopy(theLocation, this->mGhostWorldLocation);
		//}

		if((theMouseOneReleased) || (theMouseTwoReleased))
		{
			// If this is a valid location
			if(this->mCurrentGhostIsValid)
			{
				// Play sound
				this->PlayHUDSound(HUD_SOUND_PLACE_BUILDING);

				// Remember it for input to grab
				this->mValidatedBuilding = this->mGhostBuilding;
			}
		}
	}
}

void AvHHud::CancelBuilding()
{
    SetGhostBuildingMode(MESSAGE_NULL);
    this->mValidatedBuilding = MESSAGE_NULL;
}

void AvHHud::UpdateSelection()
{
	// Fetch current mouse up/down state from gScrollHandler and store
	bool theMouseOneDown = gScrollHandler.GetMouseOneDown() && !gCommanderHandler.GetMouseOneDown();
	bool theMouseTwoDown = gScrollHandler.GetMouseTwoDown();

	int theCurrentX, theCurrentY;
	this->GetMousePos(theCurrentX, theCurrentY);

	CreatePickingRay(theCurrentX, theCurrentY, this->mMouseWorldPosition);
	//ASSERT(this->mMouseWorldPosition.z < 0.0f);

	// Left mouse button // 
	// If mouse just pressed, set starting point
	if(theMouseOneDown && !this->mMouseOneDown)
	{
		this->mMouseOneStartX = theCurrentX;
		this->mMouseOneStartY = theCurrentY;
		this->mLeftMouseStarted = true;

		//CreatePickingRay(theCurrentX, theCurrentY, this->mLeftMouseWorldStart);
		//ASSERT(this->mLeftMouseWorldStart.z < 0.0f);
		VectorCopy(this->mMouseWorldPosition, this->mLeftMouseWorldStart);
	}
	else
	{
		this->mLeftMouseStarted = false;
	}

	// If mouse just released, set flag to indicate selection just changed
	if(!theMouseOneDown && this->mMouseOneDown)
	{
		//CreatePickingRay(theCurrentX, theCurrentY, this->mLeftMouseWorldEnd);
		//ASSERT(this->mLeftMouseWorldEnd.z < 0.0f);
		VectorCopy(this->mMouseWorldPosition, this->mLeftMouseWorldEnd);
		
		//this->mSelectionJustChanged = true;
		this->mLeftMouseEnded = true;
	}
	else
	{
		this->mLeftMouseEnded = false;
	}

	// Right mouse button // 
	// If mouse two just pressed, set starting point
	if(theMouseTwoDown && !this->mMouseTwoDown)
	{
		this->mMouseTwoStartX = theCurrentX;
		this->mMouseTwoStartY = theCurrentY;
		this->mRightMouseStarted = true;
		
		//CreatePickingRay(theCurrentX, theCurrentY, this->mRightMouseWorldStart);
		//ASSERT(this->mRightMouseWorldStart.z < 0.0f);
		VectorCopy(this->mMouseWorldPosition, this->mRightMouseWorldStart);
	}
	else
	{
		this->mRightMouseStarted = false;
	}
	
	// If mouse just released, set flag to indicate selection just changed
	if(!theMouseTwoDown && this->mMouseTwoDown)
	{
		//CreatePickingRay(theCurrentX, theCurrentY, this->mRightMouseWorldEnd);
		//ASSERT(this->mRightMouseWorldEnd.z < 0.0f);
		VectorCopy(this->mMouseWorldPosition, this->mRightMouseWorldEnd);
		
		//this->mSelectionJustChanged = true;
		this->mRightMouseEnded = true;
	}
	else
	{
		this->mRightMouseEnded = false;
	}

	// Set extents of marquee control
	this->mSelectionBox->SetStartPos(this->mMouseOneStartX, this->mMouseOneStartY);
	this->mSelectionBox->SetEndPos(theCurrentX, theCurrentY);
	
	// Set visibility state of marquee control
	//this->mSelectionBox->setVisible(theMouseOneDown);
    this->mSelectionBox->setVisible(false);
    
    this->mSelectionBoxX1 = mMouseOneStartX;
    this->mSelectionBoxY1 = mMouseOneStartY;
    this->mSelectionBoxX2 = theCurrentX;
    this->mSelectionBoxY2 = theCurrentY;
    
    this->mSelectionBoxVisible = theMouseOneDown;

	// If we're just selecting stuff, don't want to hit this button by mistake
	//gCommanderHandler.SetActive(!theMouseOneDown);

	// Change context sensitive cursor depending on current position
	//if(this->mSelected.size() > 0)
	//{
	if(this->mGhostBuilding == MESSAGE_NULL)
	{
		Vector theCurrentMouseRay;
 		CreatePickingRay(theCurrentX, theCurrentY, theCurrentMouseRay);

		int theTargetIndex;
		AvHOrderTargetType theTargetType;
		Vector theTargetLocation;
		AvHUser3 theUser3 = AVH_USER3_NONE;
		AvHOrderType theOrderType = AvHGetDefaultOrderType(this->GetHUDTeam(), this->GetVisualOrigin(), theCurrentMouseRay, theTargetIndex, theTargetLocation, theUser3, theTargetType);
		// Test UI blocking
//		theOrderType = ORDERTYPEL_DEFAULT;
//		if(!AvHSHUGetIsRegionBlockedByUI((float)theCurrentX/ScreenWidth, (float)theCurrentY/ScreenHeight))
//		{
//			theOrderType = ORDERTYPET_GUARD;
//		}
//		this->SetCursor(theOrderType);

		// Change cursor depending on order type
		//if(theOrderType != ORDERTYPEL_MOVE && this->mSelected.size() > 0 )
		//{
//			if(!this->GetIsRegionBlockedByUI(theCurrentX/ScreenWidth(), theCurrentY/ScreenHeight()))
//			{
				//this->SetCursor(theOrderType);
//			}
		//}
	}
	//}

	#ifdef AVH_PREDICT_SELECT
	if(this->mLeftMouseEnded)
	{

// Debug code to draw little triangles in the area that you marquee select.  Uncomment to test again.
//		// Loop through set of points near our origin, adding an effect for each one if it is in the area
//		gTriDebugLocations.clear();
//		Vector theBasePoint;
//		VectorCopy(gEngfuncs.GetLocalPlayer()->origin, theBasePoint);
//		float thePlaneABCD[4];
//		thePlaneABCD[0] = thePlaneABCD[1] = thePlaneABCD[4] = 0;
//		thePlaneABCD[2] = 1;
//
//		const int kRange = 50;
//		const float kMultiplier = 5.0f;
//		for(int i = -kRange; i <= kRange; i++)
//		{
//			for(int j = -kRange; j <= kRange; j++)
//			{
//				float theZ = theBasePoint.z - 100;
//				Vector theCurrentPoint(theBasePoint.x + i*kMultiplier, theBasePoint.y + j*kMultiplier, theZ);
//
//				Vector theEyeToEntity(theCurrentPoint.x - theBasePoint.x, theCurrentPoint.y - theBasePoint.y, theCurrentPoint.z - theBasePoint.z);
//				VectorNormalize(theEyeToEntity);
//
//				if(IsVectorBetweenBoundingVectors(theBasePoint, theEyeToEntity, this->mLeftMouseWorldStart, this->mLeftMouseWorldEnd, thePlaneABCD))
//				{
//					DebugPoint theDebugPoint(theCurrentPoint.x, theCurrentPoint.y, theCurrentPoint.z);
//					gTriDebugLocations.push_back(theDebugPoint);
//				}
//			}
//		}


		// Select all units at this click or in this area (but don't select when clicking a building down)
		if(!this->mPlacingBuilding)
		{
			gSelectionHelper.QueueSelection(this->GetVisualOrigin(), this->mLeftMouseWorldStart, this->mLeftMouseWorldEnd, this->GetHUDTeam());
			gSelectionHelper.ProcessPendingSelections();
		}
	}

	if(gSelectionHelper.SelectionResultsWaiting())
	{
		EntityListType theNewSelection;
		gSelectionHelper.GetAndClearSelection(theNewSelection);
//		this->mNumLocalSelectEvents++;
		
		if(theNewSelection != this->mSelected)
		{
			this->mSelected = theNewSelection;
			this->mSelectionJustChanged = true;
		}

		this->ClearTrackingEntity();
	}
	#endif

	// If selection just changed, make sure we have selection effects for everyone
	if(this->mSelectionJustChanged)
	{
		// Create effects
		this->SetSelectionEffects(this->mSelected);

		this->PlayHUDSound(HUD_SOUND_SELECT);

		gCommanderHandler.SetSelectedUnits(this->mSelected);

		// Clear flag
		this->mSelectionJustChanged = false;
		
//		// Set default order mode
//		if(this->mSelected.size() > 0)
//		{
//			this->mOrderMode = ORDERTYPEL_DEFAULT;
//		}
//		else
//		{
//			this->mOrderMode = ORDERTYPE_UNDEFINED;
//		}
	}

	this->UpdateBuildingPlacement();
				
	// Store current mouse state
	this->mMouseOneDown = theMouseOneDown;
	this->mMouseTwoDown = theMouseTwoDown;
}

void AvHHud::UpdateBuildResearchText()
{
	// Hide unchanging ("unhelpful"?) help text after this amount of time
	const float kHelpTextInterval = 2.5f;

	if(this->GetHUDUser3() == AVH_USER3_COMMANDER_PLAYER)
	{
		Label* theHelpTextLabel = NULL;
		if(this->GetManager().GetVGUIComponentNamed(kTechHelpText, theHelpTextLabel))
		{
			gCommanderHandler.RecalculateBuildResearchText();

			// Display build/research text
			string theBuildResearchText = gCommanderHandler.GetBuildResearchText();
			theHelpTextLabel->setText(theBuildResearchText.c_str());
				
			// Center it
			int theWidth, theHeight;
			theHelpTextLabel->getTextSize(theWidth, theHeight);
			
			int theX, theY;
			theHelpTextLabel->getPos(theX, theY);
			
			int theScreenWidth = ScreenWidth();
			theHelpTextLabel->setPos(theScreenWidth/2 - theWidth/2, theY);
			
			// Vanish if no text (but keep build/research text visible)
			theHelpTextLabel->setVisible(true);
			if(theBuildResearchText.length() == 0)// || ((this->mTimeLastHelpTextChanged != -1) && (this->mTimeLastHelpTextChanged + kHelpTextInterval < this->mTimeOfLastUpdate)))
			{
				theHelpTextLabel->setVisible(false);
			}

			// Display action button tool tip
			string theTechHelpText = gCommanderHandler.GetTechHelpText();
			if(theTechHelpText != "")
			{
				this->SetActionButtonHelpMessage(theTechHelpText);
			}

			if(theTechHelpText != this->mPreviousHelpText)
			{
				this->mTimeLastHelpTextChanged = this->mTimeOfLastUpdate;
			}

			this->mPreviousHelpText = theTechHelpText;
		}
	}
}

void AvHHud::UpdateTechNodes()
{
	this->UpdateBuildResearchText();
	
	// Don't get new node until existing one has been processed
	if(this->mTechEvent == MESSAGE_NULL)
	{
		//if(this->mGameStarted)
		//{
			//AvHTechNode theTechNode;
			//if(gCommanderHandler.GetAndClearTechNodePressed(theTechNode))
			AvHMessageID theMessageID;
			if(gCommanderHandler.GetAndClearTechNodePressed(theMessageID))
			{
				// Check for grouping and request events and handle separately
				if(this->mTechNodes.GetIsMessageAvailable(theMessageID))
				{
					bool theIsResearchable;
					int theCost;
					float theTime;
					
					if(this->mTechNodes.GetResearchInfo(theMessageID, theIsResearchable, theCost, theTime))
					{
						if(AvHSHUGetIsBuildTech(theMessageID))
						{
							// Don't check for enough points yet, they might get enough points before they put it down

							// Get ready to build it, don't send a research message
							this->SetGhostBuildingMode(theMessageID);
						}
						else if(AvHSHUGetIsResearchTech(theMessageID) && theIsResearchable)
						{
							// If we hit cancel, and we're in building mode, get out of build mode and throw event away
							if((theMessageID == MESSAGE_CANCEL) && (this->GetGhostBuilding() != MESSAGE_NULL))
							{
								CancelBuilding();
							}
							else if(theCost <= this->mResources)
							{
								this->mTechEvent = theMessageID;
							}
							else
							{
								// Emit error message that you don't have the resources
								this->PlayHUDSound(HUD_SOUND_MARINE_MORE);
							}
						}
						else if(theMessageID == BUILD_RECYCLE)
						{
							this->mTechEvent = theMessageID;
						}
					}
					else
					{
						this->mTechEvent = theMessageID;
					}
				}
				else if((theMessageID >= SAYING_1) && (theMessageID <= SAYING_9))
				{
					this->mTechEvent = theMessageID;
				}
			}
			//}
	}
}

void AvHHud::UpdateAmbientSounds()
{
	Vector theListenerPosition;
	VectorCopy(gPredictedPlayerOrigin, theListenerPosition);
	
	// Commanders have a different PAS then themselves
	if(this->mInTopDownMode)
	{
		VectorCopy(this->mCommanderPAS, theListenerPosition);
	}
	
	for(AmbientSoundListType::iterator theIter = this->mAmbientSounds.begin(); theIter != this->mAmbientSounds.end(); theIter++)
	{
		theIter->StartPlayingIfNot();
		theIter->UpdateVolume(theListenerPosition);
	}
}

void AvHHud::UpdateFromEntities(float inCurrentTime)
{
	// Only update every so often for performance reasons
	const float kEntityUpdateInterval = .4f;
	if(inCurrentTime > (this->mTimeOfLastEntityUpdate + kEntityUpdateInterval))
	{
		this->mHelpIcons.clear();
		this->mHelpEnts.clear();
		
		bool theAutoHelpEnabled = gEngfuncs.pfnGetCvarFloat(kvAutoHelp);

		// Clear help icons
		this->mHelpEnts.clear();
		
		// Clear building effects
		//this->mBuildingEffectsEntityList.clear();
		
		this->mTimeOfLastEntityUpdate = inCurrentTime;
		
		// Scan for entities, adding help icons
		// Only draw if enabled
		const int kHelpDistance = 350;
		const int kBuildDistance = 500;
		const int kHealthDistance = 125;
			
		// Look for entities that have help icons in front of us.  Don't search to far away, it could give players away.
		EntityListType theHelpEntities;
		AvHSHUGetEntities(-1, theHelpEntities);
		
		cl_entity_t* theLocalPlayer = gEngfuncs.GetLocalPlayer();
		
		for(EntityListType::iterator theIter = theHelpEntities.begin(); theIter != theHelpEntities.end(); theIter++)
		{
			vec3_t theLocation;
			if(AvHSHUGetEntityLocation(*theIter, theLocation))
			{
				float theDistance = VectorDistance((float*)&theLocation, theLocalPlayer->origin);
				if(theDistance < kHelpDistance)
				{
					// If iuser3 isn't 0, try looking up an icon for it
					physent_t* theEntity = gEngfuncs.pEventAPI->EV_GetPhysent(*theIter);
					if(theEntity)
					{
						// Don't add cloaked entities (I wish this could be more general purpose)
						if((theEntity->team == this->GetHUDTeam()) || (theEntity->rendermode == kRenderNormal))
						{
							int theUser3 = theEntity->iuser3;
							vec3_t theEntityOrigin = AvHSHUGetRealLocation(theEntity->origin, theEntity->mins, theEntity->maxs);

							// Some entities always draw
							bool theAlwaysDraw = false;
							switch(theUser3)
							{
							case AVH_USER3_WELD:
							//case AVH_USER3_COMMANDER_STATION:
							//case AVH_USER3_HIVE:
								theAlwaysDraw = true;
								break;
							}

							if(theAutoHelpEnabled || theAlwaysDraw)
							{
								this->mHelpIcons.push_back( make_pair(theEntityOrigin, theUser3) );
								
								// Push back entity for displaying helpful tooltips
								this->mHelpEnts.push_back(*theIter);
							}
							
//							// If entity is buildable and within a certain distance, add it to have it's building effects potentially drawn
//							if(!this->GetInTopDownMode())
//							{
//								// Draw if in large radius and unbuilt, or if close by whether it's built or not
//								if((GetHasUpgrade(this->GetLocalUpgrades(), MASK_BUILDABLE) && (theDistance < kBuildDistance)) || (theDistance < kHealthDistance))
//								{
//									// Only put it in if it doesn't exist already
//									int theEntity = *theIter;
//									if(std::find(this->mBuildingEffectsEntityList.begin(), this->mBuildingEffectsEntityList.end(), theEntity) == this->mBuildingEffectsEntityList.end())
//									{
//										this->mBuildingEffectsEntityList.push_back(theEntity);
//									}
//								}
//							}
						}
						else
						{
							int a = 0;
						}
					}
				}
			}
		}
	}
}

void AvHHud::UpdateViewModelEffects()
{
	cl_entity_t* theViewModel = GetViewEntity();
	if(theViewModel)
	{
		int theRenderMode = kRenderNormal;
		int theRenderAmount = 0;
		int theRenderFx = theViewModel->curstate.renderfx;
		color24 theRenderColor = theViewModel->curstate.rendercolor;
		short theSkin = 0;

		// Set the skin, stored in playerclass
		
        //cl_entity_s* theLocalPlayer = gEngfuncs.GetLocalPlayer();

        // Use the visible player so that when we are spectating we can tell
        // when the player is cloaked.
        cl_entity_s* theLocalPlayer = GetVisiblePlayer();

		if(theLocalPlayer)
		{
			
			//theViewModel->curstate.skin = theLocalPlayer->curstate.skin;
			theRenderMode = theLocalPlayer->curstate.rendermode;
			theRenderAmount = theLocalPlayer->curstate.renderamt;
			theRenderFx = theLocalPlayer->curstate.renderfx;
			theRenderColor = theLocalPlayer->curstate.rendercolor;
			theSkin = theLocalPlayer->curstate.skin;
			// Hack to make cloaking work for viewmodels (if only view models rendered in additive properly).  
			// Draw view model normally unless fully cloaked.
			//bool theIsCloakedViaUpgrade = GetHasUpgrade(theLocalPlayer->curstate.iuser4, MASK_ALIEN_CLOAKED);
			int old=theRenderAmount;
			if((theRenderMode == kRenderTransTexture) /*|| theIsCloakedViaUpgrade*/)
			{
				theRenderFx = kRenderFxNone;
				theRenderColor.r = theRenderColor.g = theRenderColor.b = 0;

				if ( theRenderAmount ==  kAlienSelfCloakingBaseOpacity ) 
				{
					theRenderMode = kAlienCloakViewModelRenderMode;
					theRenderAmount = 10;
				}
				else if( theRenderAmount > kAlienSelfCloakingBaseOpacity && theRenderAmount < 186)
				{
					theRenderMode = kAlienCloakViewModelRenderMode;
					theRenderAmount = 40;
				}
				else if ( theRenderAmount == 186 )  {
					theRenderMode = kAlienCloakViewModelRenderMode;
					theRenderAmount = 50;
				}
				else
				{
					theRenderMode = kRenderNormal;
					theRenderAmount = 255;
				}
			}


			//char theMessage[128];
			//sprintf(theMessage, "Setting view model mode: %d amount: %d\n", theRenderMode, theRenderAmount);
			//CenterPrint(theMessage);
		}

//		if(GetHasUpgrade(this->GetLocalUpgrades(), MASK_ALIEN_CLOAKED))
//		{
//			theRenderMode = kAlienCloakViewModelRenderMode;
//			int theCloakingLevel = AvHGetAlienUpgradeLevel(this->GetLocalUpgrades(), MASK_UPGRADE_7);
//			theRenderAmount = kAlienCloakViewModelAmount - theCloakingLevel*kAlienCloakViewModelLevelAmount;
//		
//			// Put in color, because texture rendering needs it
//			theRenderFx = kRenderFxNone;
//			theRenderColor.r = theRenderColor.g = theRenderColor.b = 0;
//		}
		
		if(this->mInTopDownMode)
		{
			theRenderMode = kRenderTransAdd;
			theRenderAmount = 1;
		}

		theViewModel->curstate.skin = theSkin;
		theViewModel->curstate.rendermode = theRenderMode;
		theViewModel->curstate.renderamt = theRenderAmount;
		theViewModel->curstate.renderfx = theRenderFx;
		theViewModel->curstate.rendercolor = theRenderColor;
	}
}

void AvHHud::UpdateResources(float inTimePassed)
{
	const float kResourceRate = this->GetMaxAlienResources()/50.0f;
	int thePointsToMove = max(inTimePassed*kResourceRate, 1);

	// Update visual resources if different this resources
	if(this->mVisualResources != this->mResources)
	{
		if(abs(this->mVisualResources - this->mResources) <= thePointsToMove)
		{
			this->mVisualResources = this->mResources;
		}
		else
		{
			if(this->mVisualResources < this->mResources)
			{
				this->mVisualResources += thePointsToMove;
			}
			else
			{
				this->mVisualResources -= thePointsToMove;
			}
		}
	}

	// Smoothly adjust energy level
	float theCurrentEnergyLevel = pmove->fuser3/kNormalizationNetworkFactor;

	if((g_iUser1 == OBS_IN_EYE) && (g_iUser2 != this->mUser2OfLastEnergyLevel))
	{
		// This isn't working yet
		this->mVisualEnergyLevel = theCurrentEnergyLevel;
		this->mUser2OfLastEnergyLevel = g_iUser2;
	}
	else
	{
		float kEnergyRate = 1.0f;
		float theEnergyToMove = inTimePassed*kEnergyRate;
		if(this->mVisualEnergyLevel != theCurrentEnergyLevel)
		{
			float theDiff = fabs(this->mVisualEnergyLevel - theCurrentEnergyLevel);
			if(theDiff <= theEnergyToMove)
			{
				this->mVisualEnergyLevel = theCurrentEnergyLevel;
			}
			else
			{
				if(this->mVisualEnergyLevel < theCurrentEnergyLevel)
				{
					this->mVisualEnergyLevel += theEnergyToMove;
				}
				else
				{
					this->mVisualEnergyLevel -= theEnergyToMove;
				}
			}
		}
	}
	
	string theResourceText;
	char theResourceBuffer[64];
	
	if(this->GetInTopDownMode() && this->mCommanderResourceLabel)
	{
		LocalizeString(kMarineResourcePrefix, theResourceText);
		sprintf(theResourceBuffer, "%s %d", theResourceText.c_str(), this->mVisualResources);
		this->mCommanderResourceLabel->setText(64, theResourceBuffer);
	}

	// Update visual resource indicators, expiring old ones
	const float kNumericalInfoEffectLifetime = 1.1f;
	const float kNumericalInfoScrollSpeed = 24;
	for(NumericalInfoEffectListType::iterator theIter = this->mNumericalInfoEffects.begin(); theIter != this->mNumericalInfoEffects.end(); /* no inc */)
	{
		if((theIter->GetTimeCreated() + kNumericalInfoEffectLifetime) < this->mTimeOfLastUpdate)
		{
			theIter = this->mNumericalInfoEffects.erase(theIter);
		}
		else
		{
			// Update position
			float thePosition[3];
			theIter->GetPosition(thePosition);

			thePosition[2] += inTimePassed*kNumericalInfoScrollSpeed;

			theIter->SetPosition(thePosition);

			// Next
			theIter++;
		}
	}
}




//void AvHHud::ChangeUpgradeCosts(int inOldMessageID, int inNewMessageID, const char* inText)
//{
//	this->ChangeUpgradeCostsForMenu(this->mSoldierMenu, inOldMessageID, inNewMessageID, inText);
//	this->ChangeUpgradeCostsForMenu(this->mLeaderMenu, inOldMessageID, inNewMessageID, inText);
//	this->ChangeUpgradeCostsForMenu(this->mCommanderMenu, inOldMessageID, inNewMessageID, inText);
//
//	this->UpdateUpgradeCosts();
//}
//
//void AvHHud::ChangeUpgradeCostsForMenu(PieMenu* inMenu, int inOldMessageID, int inNewMessageID, const char* inText)
//{
//	if(inMenu)
//	{
//		inMenu->ChangeNode(inOldMessageID, inNewMessageID, string(inText));
//	}
//}
//
//void AvHHud::ResetUpgradeCosts()
//{
//	this->ResetUpgradeCostsForMenu(this->mSoldierMenu);
//	this->ResetUpgradeCostsForMenu(this->mLeaderMenu);
//	this->ResetUpgradeCostsForMenu(this->mCommanderMenu);
//
//	this->UpdateUpgradeCosts();
//}

//void AvHHud::ResetUpgradeCostsForMenu(PieMenu* inMenu)
//{
//	if(inMenu)
//	{
//		inMenu->ResetToDefaults();
//	}
//}

bool AvHHud::GetParticlesVisible() const
{
    
    if (g_iUser1 == OBS_NONE)
    {
        return true;
    }
    
    if (m_Spectator.IsInOverviewMode())
    {
        return m_Spectator.m_iDrawCycle == 1;
    }
    else
    {
        return true;
    }

}

// Sprite drawing on a level change is problematic and can cause crashing or disconcerting "no such sprite" error messages
bool AvHHud::GetSafeForSpriteDrawing() const
{
	bool theSafeForDrawing = false;

	const char* theLevelName = gEngfuncs.pfnGetLevelName();
	string theCurrentMapName = this->GetMapName(true);
	if(theLevelName && (theCurrentMapName != ""))
	{
		string theLevelNameString(theLevelName);
		int thePos = theLevelNameString.find(theCurrentMapName);
		if(thePos != string::npos)
		{
			theSafeForDrawing = true;
		}
		else
		{
			int a = 0;
		}
	}

	return theSafeForDrawing;
}

bool AvHHud::GetShouldDisplayUser3(AvHUser3 inUser3) const
{
	bool theShouldDisplay = false;

	if((inUser3 > AVH_USER3_NONE) && (inUser3 < AVH_USER3_MAX))
	{
		theShouldDisplay = true;

		switch(inUser3)
		{
		case AVH_USER3_BREAKABLE:
		case AVH_USER3_USEABLE:
		case AVH_USER3_PARTICLE_ON:
		case AVH_USER3_PARTICLE_OFF:
		case AVH_USER3_ALPHA:
		case AVH_USER3_WAYPOINT:
		case AVH_USER3_NOBUILD:
		case AVH_USER3_SPAWN_TEAMONE:
		case AVH_USER3_SPAWN_TEAMTWO:
			theShouldDisplay = false;
			break;
		}
	}
	
	return theShouldDisplay;
}

bool AvHHud::GetTranslatedUser3Name(AvHUser3 inUser3, string& outString) const
{
	bool theSuccess = false;

	if(this->GetShouldDisplayUser3(inUser3))
	{
		char theUser3String[512];
		sprintf(theUser3String, "#%s%d", kUser3Name, inUser3);
		theSuccess = LocalizeString(theUser3String, outString);
	}
	
	return theSuccess;
}

bool AvHHud::GetTranslatedUser3Description(AvHUser3 inUser3, bool inFriendly, string& outString) const
{
	bool theSuccess = false;

	if(this->GetShouldDisplayUser3(inUser3))
	{
		char theUser3String[512];
		sprintf(theUser3String, "#%s%d", kUser3Description, inUser3);
		theSuccess = LocalizeString(theUser3String, outString);

		// If we're commanding, look for that description if it exists
		if(this->GetInTopDownMode())
		{
			string theCommanderDescription;
			sprintf(theUser3String, "#%s%d", kUser3CommanderDescription, inUser3);
			if(LocalizeString(theUser3String, theCommanderDescription))
			{
				outString = theCommanderDescription;
				theSuccess = true;
			}
		}
		// Else look for a message that tell us what to do with this thing (assumes we're not commanding though)
		else if(inFriendly)
		{
			string theFriendlyDescription;
			sprintf(theUser3String, "#%s%d", kUser3FriendlyDescription, inUser3);
			if(LocalizeString(theUser3String, theFriendlyDescription))
			{
				outString = theFriendlyDescription;
				theSuccess = true;
			}
		}
	}

	return theSuccess;
}


void AvHHud::UpdateUpgradeCosts()
{
    PieMenu* theCurrentMenu = NULL;
    if(this->GetManager().GetVGUIComponentNamed(this->mPieMenuControl, theCurrentMenu))
    {
        this->UpdateEnableState(theCurrentMenu);
	}
}

void AvHHud::AddMiniMapAlert(float x, float y)
{
    mOverviewMap.AddAlert(x, y);
}

void AvHHud::UpdateEnableState(PieMenu* inMenu)
{
	if(inMenu)
	{
        int thePurchaseLevel = this->GetIsCombatMode() ? max(0, this->mExperienceLevel - this->mExperienceLevelSpent - 1) : this->mResources;
        
        inMenu->UpdateMenuFromTech(this->mTechNodes, thePurchaseLevel);

//        if(this->GetIsNSMode())
        //{
            // Now disable any nodes whose children are all disabled (in NS, only end nodes can be chosen)
            //inMenu->DisableNodesWhoseChildrenAreDisabled();
        //}

        inMenu->RecomputeVisibleSize();
	}
}

void AvHHud::ShowMap()
{
    if (!sShowMap && gHUD.GetIsNSMode())
    {	
        sShowMap = true;
        gHUD.HideCrosshair();
	    gHUD.GetManager().UnhideComponent(kShowMapHierarchy);
    }
}

void AvHHud::HideMap()
{
    if (sShowMap)
    {
        sShowMap = false;
	    gHUD.GetManager().HideComponent(kShowMapHierarchy);
	    gHUD.ShowCrosshair();
    }
}

void AvHHud::GetSpriteForUser3(AvHUser3 inUser3, int& outSprite, int& outFrame, int& outRenderMode)
{

    switch (inUser3)
    {

    // Marines
    case AVH_USER3_WAYPOINT:
		outSprite = Safe_SPR_Load(kSmallOrderSprite);
		outFrame  = 2;
        outRenderMode = kRenderTransAdd;
        break;
    case AVH_USER3_MARINE_PLAYER:
		outSprite = Safe_SPR_Load(kMarinePlayersSprite);
		outFrame  = 0;
        break;
    case AVH_USER3_HEAVY: // This really means a marine with heavy armor, not a heavy armor object.
		outSprite = Safe_SPR_Load(kMarinePlayersSprite);
		outFrame  = 1;
        break;
    case AVH_USER3_COMMANDER_STATION:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 5;
        break;
    case AVH_USER3_TURRET_FACTORY: 
    case AVH_USER3_ADVANCED_TURRET_FACTORY:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 6;
        break;
    case AVH_USER3_ARMORY:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 7;
        break;
	case AVH_USER3_ADVANCED_ARMORY:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 8;
        break;
    case AVH_USER3_ARMSLAB:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 9;
        break;
    case AVH_USER3_PROTOTYPE_LAB:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 10;
        break;
    case AVH_USER3_OBSERVATORY:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 11;
        break;
    case AVH_USER3_TURRET:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 12;
        break;
    case AVH_USER3_SIEGETURRET:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 13;
        break;
    case AVH_USER3_RESTOWER:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 14;
        break;        
    case AVH_USER3_INFANTRYPORTAL:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 15;
        break;        
    case AVH_USER3_PHASEGATE:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 16;
        break;        
        
	// Aliens
    case AVH_USER3_DEFENSE_CHAMBER:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 17;
        break;
    case AVH_USER3_MOVEMENT_CHAMBER:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 18;
        break;
    case AVH_USER3_OFFENSE_CHAMBER:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 19;
        break;
    case AVH_USER3_SENSORY_CHAMBER:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 20;
        break;
    case AVH_USER3_ALIENRESTOWER:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 21;
        break;
	case AVH_USER3_HIVE:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame = 3;
		break;
	case AVH_USER3_ALIEN_PLAYER1:
		outSprite = Safe_SPR_Load(kAlienPlayersSprite);
		outFrame  = 0;
        break;
	case AVH_USER3_ALIEN_PLAYER2:
		outSprite = Safe_SPR_Load(kAlienPlayersSprite);
		outFrame  = 1;
        break;
	case AVH_USER3_ALIEN_PLAYER3:
		outSprite = Safe_SPR_Load(kAlienPlayersSprite);
		outFrame  = 2;
        break;
	case AVH_USER3_ALIEN_PLAYER4:
		outSprite = Safe_SPR_Load(kAlienPlayersSprite);
		outFrame  = 3;
        break;
	case AVH_USER3_ALIEN_PLAYER5:
		outSprite = Safe_SPR_Load(kAlienPlayersSprite);
		outFrame  = 4;
        break;
	case AVH_USER3_ALIEN_EMBRYO	:
		outSprite = Safe_SPR_Load(kAlienPlayersSprite);
		outFrame  = 5;
        break;

	case AVH_USER3_FUNC_RESOURCE:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame = 4;
		break;
    case AVH_USER3_WELD:
		outSprite = Safe_SPR_Load(kStructuresSprite);
		outFrame  = 0;
        break;
    
    default:
        outSprite = 0;
        outFrame  = 0;

    }

}

int AvHHud::GetCurrentSquad() const
{
    return mCurrentSquad;
}
    
AvHOverviewMap& AvHHud::GetOverviewMap()
{
    return mOverviewMap;
}

void AvHHud::ShowCrosshair()
{
    
    ++mCrosshairShowCount;

    if (mCrosshairShowCount > 0)
    {
        SetCrosshair(mCrosshairSprite, mCrosshairRect, mCrosshairR, mCrosshairG, mCrosshairB);
    }

}

void AvHHud::HideCrosshair()
{

    --mCrosshairShowCount;

    if (mCrosshairShowCount <= 0)
    {
        wrect_t nullrect = { 0, 0, 0, 0 };
        SetCrosshair(0, nullrect, 0, 0, 0);
    }

}

void AvHHud::SetCurrentCrosshair(HSPRITE hspr, wrect_t rc, int r, int g, int b)
{
    mCrosshairSprite = hspr;
    mCrosshairRect   = rc;
    mCrosshairR      = r;
    mCrosshairG      = g;
    mCrosshairB      = b;

    if (mCrosshairShowCount > 0)
    {
        SetCrosshair(mCrosshairSprite, mCrosshairRect, mCrosshairR, mCrosshairG, mCrosshairB);
    }

}

void AvHHud::SetViewport(const int inViewport[4])
{
    if (!m_Spectator.IsInOverviewMode())
    {
        mViewport[0] = inViewport[0];
        mViewport[1] = inViewport[1];
        mViewport[2] = inViewport[2];
        mViewport[3] = inViewport[3];
    }
    else
    {
        mSpecialViewport[0] = inViewport[0];
        mSpecialViewport[1] = inViewport[1];
        mSpecialViewport[2] = inViewport[2];
        mSpecialViewport[3] = inViewport[3];

        mViewport[0] = 0;
        mViewport[1] = 0;
        mViewport[2] = ScreenWidth();
        mViewport[3] = ScreenHeight();

    }
}

void AvHHud::GetViewport(int outViewport[4]) const
{
    outViewport[0] = mViewport[0];
    outViewport[1] = mViewport[1];
    outViewport[2] = mViewport[2];
    outViewport[3] = mViewport[3];
}

const AvHFont& AvHHud::GetSmallFont() const
{
    return mSmallFont;
}

void AvHHud::PlayStream()
{
    if(gEngfuncs.Cmd_Argc() <= 1)
    {
        gEngfuncs.Con_Printf( "usage:  playstream <url>\n" );
    }
    else
    {
        if ( gEngfuncs.Cmd_Argc() >= 2 )
        {
            // Read URL
            string theURL;
			theURL = string("http://") + string(gEngfuncs.Cmd_Argv(1));
            
            string theError;
            if(!gHUD.PlayInternetStream(theURL, theError))
			{
				gHUD.AddTooltip(theError.c_str());
			}
        }
    }
}

void AvHHud::StopStream()
{
    gHUD.StopInternetStream();
}

float AvHHud::GetServerVariableFloat(const char* inName) const
{
    ServerVariableMapType::const_iterator iterator;
    iterator = mServerVariableMap.find(inName);

    if ( iterator == mServerVariableMap.end() )
    {
        return 0;
    }
    else
    {
        return atof( iterator->second.c_str() );
    }

}


/**
 * Prints the call stack when an unhandled exception occurs.
 */
LONG WINAPI ExceptionFilter(EXCEPTION_POINTERS* pExp) 
{

    /*
    // E-mail the exception log to the programmers.

    std::stringstream buffer;
    LogException(buffer, pExp);

    const char* serverName  = "66.111.4.62";
    const char* fromAddress = "noreply@overmind.com";

    const char* programmerAddress[] =
        {
            "max_mcguire@yahoo.com",
        };
    
    for (int i = 0; i < sizeof(programmerAddress) / sizeof(char*); ++i)
    {
        SendMail(serverName, fromAddress, fromAddress, programmerAddress[i],
            "Exception Log", buffer.str().c_str());
    }    
    */

    AvHHud::ResetGammaAtExit();

    return EXCEPTION_EXECUTE_HANDLER;

}

// Added DLL entry point to try to reset gamma properly under VAC
BOOL WINAPI DllMain(HINSTANCE hinstDLL,
					DWORD fdwReason,
					LPVOID lpvReserved)
{
	if (fdwReason == DLL_PROCESS_ATTACH)
    {

        // Install a crash handler.
        //SetUnhandledExceptionFilter(ExceptionFilter);

    }
	else if (fdwReason == DLL_PROCESS_DETACH)
    {
		AvHHud::ResetGammaAtExit();
    }
	return TRUE;
}
