//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHPieMenuHandler.cpp $
// $Date: 2002/09/25 20:49:23 $
//
//-------------------------------------------------------------------------------
// $Log: AvHPieMenuHandler.cpp,v $
// Revision 1.23  2002/09/25 20:49:23  Flayra
// - New select sound for aliens
//
// Revision 1.22  2002/09/23 22:24:16  Flayra
// - Removed NSTR #ifdefs
//
// Revision 1.21  2002/09/09 20:00:56  Flayra
// - Pop-up menu now stays open if you open and close it really fast (ie, right click)
//
// Revision 1.20  2002/08/31 18:01:02  Flayra
// - Work at VALVe
//
// Revision 1.19  2002/08/16 02:40:51  Flayra
// - HUD sounds no longer cut each other off (they won't play instead of cutting off another sound)
//
// Revision 1.18  2002/08/09 01:08:45  Flayra
// - Debugging code
//
// Revision 1.17  2002/07/08 17:12:49  Flayra
// - Renamed pop-up menu command, reworked it to be a regular bind
//
//===============================================================================
#include "util/nowarnings.h"
#include "ui/PieMenu.h"
#include "ui/PieNode.h"
#include "mod/AvHPieMenuHandler.h"
#include "mod/AvHTeamHierarchy.h"
#include "mod/AvHMessage.h"
#include "mod/AvHClientVariables.h"
#include "mod/AvHCommandConstants.h"
#include "engine/cdll_int.h"
#include "types.h"
#include <string>
using std::string;

#include "cl_dll/demo.h"
#include "common/demo_api.h"

void IN_ResetMouse();

extern int g_weaponselect;
extern int in_impulse;
bool sTheDebugBool = false;

PieNode* AvHPieMenuHandler::sLastNodeHighlighted = NULL;
string AvHPieMenuHandler::sPieMenuName = "";
float AvHPieMenuHandler::sTimeLastNodeHighlighted = 0.0f;
float AvHPieMenuHandler::sTimeMenuOpened = 0.0f;
bool AvHPieMenuHandler::sPieMenuOpen = false;

bool AvHPieMenuHandler::GetIsPieMenuOpen(void)
{
    return sPieMenuOpen;
}

PieMenu* AvHPieMenuHandler::GetActivePieMenu()
{
    PieMenu* thePieMenu = NULL;

    gHUD.GetManager().GetVGUIComponentNamed(sPieMenuName, thePieMenu);

    return thePieMenu;
}

void AvHPieMenuHandler::ClosePieMenu(void)
{

    if (!sPieMenuOpen)
    {
        return;
    }

	//CenterPrint("AvHPieMenuHandler::closePieMenu.\n");

	// If the action was really quick, choose the highlighted node so overshooting isn't a problem
	PieNode* theNode = NULL;
	
	float theCurrentTime = gEngfuncs.GetClientTime();
	float theQuickThreshold = cl_quickselecttime->value;
	bool theReallyQuick = (theCurrentTime - sTimeLastNodeHighlighted < theQuickThreshold);
	if(theReallyQuick)
	{
		//CenterPrint("Quick mode.\n");
		PieMenu* thePieMenu = NULL;
		if(gHUD.GetManager().GetVGUIComponentNamed(sPieMenuName, thePieMenu))
		{
			thePieMenu->GetSelectedNode(theNode);
		}
	}
	else
	{
		//CenterPrint("Regular mode.\n");
		theNode = dynamic_cast<PieNode*>(sLastNodeHighlighted);
	}
	
	if(theNode)
	{
		NodeChosen(theNode->GetNodeName(), theNode->GetMessageID());
	}
	else
	{
		NodeCancelled();
	}

    // Reset the mouse cursor to the center of the screen so
    // that the view doesn't jog once the pie menu is closed.

	IN_ResetMouse();
    gHUD.ShowCrosshair();

    sPieMenuOpen = false;

}

void AvHPieMenuHandler::InternalClosePieMenu(void)
{
    PieMenu* theMarineMenu = NULL;

    if(gHUD.GetManager().GetVGUIComponentNamed(sPieMenuName, theMarineMenu))
    {
        // TODO: Select option on menu before closing it!
		if(!gHUD.GetInTopDownMode())
		{
			gHUD.GetManager().SetMouseVisibility(false);
		}

        theMarineMenu->SetFadeState(false);
        if(sLastNodeHighlighted)
        {
            sLastNodeHighlighted->SetDrawSelected(false);
        }
        sLastNodeHighlighted = NULL;

//        if(sTheDebugBool)
//        {
//            AvHTeamHierarchy* theHierarchyComponent = NULL;
//            if(gHUD.GetManager().GetVGUIComponentNamed(kHierarchy, theHierarchyComponent))
//            {
//                theHierarchyComponent->setVisible(true);
//            }
//        }

    }

}

void AvHPieMenuHandler::OpenPieMenu(void)
{
    PieMenu* theMarineMenu = NULL;

	//CenterPrint("AvHPieMenuHandler::openPieMenu.\n");
		
	// Pie menu only active when playing
	AvHUser3 theUser3 = gHUD.GetHUDUser3();
	if(theUser3 > AVH_USER3_NONE && theUser3 <= AVH_USER3_ALIEN_PLAYER5)
	{
		if(gHUD.GetPlayMode() == PLAYMODE_PLAYING)
		{
			if(gHUD.GetManager().GetVGUIComponentNamed(sPieMenuName, theMarineMenu))
			{
				if(!gHUD.GetInTopDownMode())
				{
					gHUD.GetManager().SetMouseVisibility(true);
				}

                gHUD.HideCrosshair();
                
				// Only do this when in full screen
				//App::getInstance()->setCursorPos(ScreenWidth/2, ScreenHeight/2);
	
				theMarineMenu->SetFadeState(true);
				sLastNodeHighlighted = theMarineMenu->GetRootNode();
				sLastNodeHighlighted->SetDrawSelected(true);
				sTimeMenuOpened = gEngfuncs.GetClientTime();
				sTimeLastNodeHighlighted = sTimeMenuOpened;
                sPieMenuOpen = true;

				
				//        if(sTheDebugBool)
				//        {
				//            AvHTeamHierarchy* theHierarchyComponent = NULL;
				//            if(gHUD.GetManager().GetVGUIComponentNamed(kHierarchy, theHierarchyComponent))
				//            {
				//                theHierarchyComponent->setVisible(false);
				//            }
				//        }
			}
		}
	}
}

void AvHPieMenuHandler::NodeCancelled()
{
    InternalClosePieMenu();
}

void AvHPieMenuHandler::NodeChosen(const string& /*inNodeName*/, int inMessageID)
{
//	char* theSound = kSelectSound;
//	if(gHUD.GetIsAlien())
//	{
//		theSound = kSelectAlienSound;
//	}
//	gHUD.PlayHUDSound(theSound, .3f);

	gHUD.PlayHUDSound(HUD_SOUND_SELECT);

	// Client-side effects
	switch(inMessageID)
	{
		case COMM_CHAT_PUBLIC:
			// TODO: Pop up message saying to hit enter to send or escape to cancel message
			ClientCmd(kcGlobalChat);
			break;
		
		case COMM_CHAT_TEAM:
			// TODO: Pop up message saying to hit enter to send or escape to cancel message
			ClientCmd(kcTeamChat);
			break;

		case COMM_CHAT_NEARBY:
			ClientCmd(kcNearbyChat);
			break;

		default:
		    in_impulse = inMessageID;
			break;
	}

    InternalClosePieMenu();
}

void AvHPieMenuHandler::NodeActivated(const string& inNodeName)
{
}

string AvHPieMenuHandler::GetPieMenuControl()
{
	return sPieMenuName;
}

void AvHPieMenuHandler::SetPieMenuControl(const string& inPieMenuName)
{
	sPieMenuName = inPieMenuName;
}

void AvHPieMenuHandler::cursorMoved(int x,int y,Panel* panel)
{
//	char theMessage[128];
//	sprintf(theMessage, "AvHPieMenuHandler::cursorMoved %d, %d (panel ptr: %d).\n", x, y, (int)panel);
//	CenterPrint(theMessage);
}

void AvHPieMenuHandler::cursorEntered(Panel* panel)
{
    PieNode* theNode = dynamic_cast<PieNode*>(panel);
    if(theNode)
    {
        if(theNode->GetFadeState())
        {
            if((theNode->IsAdjacentTo(sLastNodeHighlighted)) || (theNode->GetIsAbove(sLastNodeHighlighted)))
            {
                //char theTempBuffer[256];
                //sprintf(theTempBuffer, "Cursor entered %s.\n", theNode->GetNodeName().c_str());
                //CenterPrint(theTempBuffer);

				// Check if enabled
				//if(theNode->GetEnabled())
				//{
					//theNode->SetAllChildrenFadeState(false);
					if(theNode->HighlightNode())
					{
						if(!theNode->GetIsAbove(sLastNodeHighlighted))
						{
							if(gHUD.GetIsAlien())
							{
								gHUD.PlayHUDSound(kPieSelectForwardAlienSound, kHUDSoundVolume);
							}
							else
							{
								gHUD.PlayHUDSound(kPieSelectForwardSound, kHUDSoundVolume);
							}
						}
						else
						{
							if(gHUD.GetIsAlien())
							{
								gHUD.PlayHUDSound(kPieSelectBackwardAlienSound, kHUDSoundVolume);
							}
							else
							{
								gHUD.PlayHUDSound(kPieSelectBackwardSound, kHUDSoundVolume);
							}
						}
						
						sLastNodeHighlighted->SetDrawSelected(false);
						
						sLastNodeHighlighted = theNode;

						sTimeLastNodeHighlighted = gEngfuncs.GetClientTime();
						
						theNode->SetDrawSelected(true);
					}
				//}
            }
        }
    }
}

void AvHPieMenuHandler::cursorExited(Panel* panel)
{
//	CenterPrint("AvHPieMenuHandler::cursorExited.\n");
	
//    PieNode* theNode = dynamic_cast<PieNode*>(panel);
//    if(theNode)
//    {
//        char theTempBuffer[256];
//        sprintf(theTempBuffer, "Cursor exited %s.\n", theNode->GetNodeName().c_str());
//        CenterPrint(theTempBuffer);
//
//        theNode->SetNodeAndAdjacentChildrenFadeState(false);
//    }
}

void AvHPieMenuHandler::mousePressed(MouseCode code,Panel* panel)
{
//	CenterPrint("AvHPieMenuHandler::mousePressed.\n");
}

void AvHPieMenuHandler::mouseDoublePressed(MouseCode code,Panel* panel)
{
//	CenterPrint("AvHPieMenuHandler::mouseDoublePressed.\n");
}

void AvHPieMenuHandler::mouseReleased(MouseCode code, Panel* inPanel)
{

    //	CenterPrint("AvHPieMenuHandler::mouseReleased.\n");
	


//
//    if(code == MOUSE_RIGHT)
//    {
//		// If the action was really quick, choose the highlighted node so overshooting isn't a problem
		PieMenu* thePieMenu = NULL;
		PieNode* theNode = NULL;

		float theCurrentTime = gEngfuncs.GetClientTime();
		float kQuickThreshold = cl_quickselecttime->value;
		bool theReallyQuick = (theCurrentTime - sTimeLastNodeHighlighted < kQuickThreshold);
		if(theReallyQuick)
		{
			//CenterPrint("Quick mode.\n");
			if(gHUD.GetManager().GetVGUIComponentNamed(sPieMenuName, thePieMenu))
			{
				thePieMenu->GetSelectedNode(theNode);
			}
		}
		else
		{
			//CenterPrint("Regular mode.\n");
			theNode = dynamic_cast<PieNode*>(inPanel);
		}

		if(theNode)
		{
			// Don't close menu if they released over the root node and it was really quick
			if(!thePieMenu || !(theNode == thePieMenu->GetRootNode()) || !((theCurrentTime -  sTimeMenuOpened) < .3f))
			{
				NodeChosen(theNode->GetNodeName(), theNode->GetMessageID());
			}
		}
        else
        {
            NodeCancelled();
        }

	// puzl : 983 releasing a mouse closes the popup menu
	if ( code == MOUSE_RIGHT || code == MOUSE_LEFT || code == MOUSE_MIDDLE)	
	{
		ClientCmd("-popupmenu");
		ClosePieMenu();
	}
//    }
}

void AvHPieMenuHandler::mouseWheeled(int delta,Panel* panel)
{
}

void AvHPieMenuHandler::keyPressed(KeyCode code,Panel* panel)
{
}

void AvHPieMenuHandler::keyTyped(KeyCode code,Panel* panel)
{
}

void AvHPieMenuHandler::keyReleased(KeyCode code,Panel* panel)
{
}

void AvHPieMenuHandler::keyFocusTicked(Panel* panel)
{
}


