//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHCommanderModeHandler.cpp$
// $Date: 2002/10/24 21:22:19 $
//
//-------------------------------------------------------------------------------
// $Log: AvHCommanderModeHandler.cpp,v $
// Revision 1.25  2002/10/24 21:22:19  Flayra
// - Remove resource tower upgrade icon
//
// Revision 1.24  2002/10/16 20:51:27  Flayra
// - Phase gates are now recyclable
//
// Revision 1.23  2002/10/16 00:50:17  Flayra
// - Phase gates can be recycled now
//
// Revision 1.22  2002/09/23 22:11:28  Flayra
// - Command UI is now a 4x4 matrix
// - Updated for new droppable jetpacks and heavy armor
//
// Revision 1.21  2002/08/31 18:01:01  Flayra
// - Work at VALVe
//
// Revision 1.20  2002/08/09 00:55:56  Flayra
// - Fixed bug where pressing the equip menu button sometimes caused menu to reset (bug still exists when selecting some things)
//
// Revision 1.19  2002/07/23 17:00:40  Flayra
// - Lots of UI changes, new upgrades, selectable siege
//
// Revision 1.18  2002/06/25 17:45:07  Flayra
// - Renamed some buildings, armory is now upgraded
//
// Revision 1.17  2002/06/10 19:51:06  Flayra
// - New commander UI
//
// Revision 1.16  2002/06/03 16:40:24  Flayra
// - Renamed weapons factory and armory
//
// Revision 1.15  2002/05/28 17:37:43  Flayra
// - Added recycling, added fix for occasional crash when selecting command station with some lag
//
// Revision 1.14  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "util/nowarnings.h"
#include "VGUI_Panel.h"
#include "cl_dll/chud.h"
#include "engine/cdll_int.h"
#include "cl_dll/cl_util.h"
#include "mod/AvHCommanderModeHandler.h"
#include "mod/AvHConstants.h"
#include "mod/AvHLogoutComponent.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHTitles.h"
#include "mod/AvHTechImpulsePanel.h"
#include "mod/AvHCommandConstants.h"
#include "util/STLUtil.h"

AvHCommanderModeHandler::AvHCommanderModeHandler()
{
	this->Init();
}

void AvHCommanderModeHandler::Init()
{
	this->mTechNodePressed = false;
	this->mMoveToWorldX = this->mMoveToWorldY = 0;
	this->mHasMoveToPosition = false;
	this->mDefaultOrderWorldX = this->mDefaultOrderWorldY = 0;
	this->mHasDefaultOrderPosition = false;
	this->mMouseOneDown = false;
	this->mMouseTwoDown = false;
	this->mMouseOneDownOnActionButtons = false;
	this->mMouseOneDownOnLogoutButton = false;
	this->mDisplayMenu = AVH_USER3_MENU_BUILD;
	this->mDisplayMenuMessageID = MENU_BUILD;

	this->mSelected.clear();
	this->mTechHelpText = "";
	this->mBuildResearchText = "";
}

void AvHCommanderModeHandler::CancelHit()
{
//	this->SetBaseMenu();
}

void AvHCommanderModeHandler::ClearDefaultOrderPosition()
{
	this->mHasDefaultOrderPosition = false;
}

void AvHCommanderModeHandler::ClearMoveToPosition()
{
	this->mHasMoveToPosition = false;
}

bool AvHCommanderModeHandler::GetAndClearTechNodePressed(AvHMessageID& outMessageID, bool inClear)
{
	bool theSuccess = false;

	if(this->mTechNodePressed)
	{
		outMessageID = this->mLastTechPressed;
		theSuccess = true;

		if(inClear)
		{
			this->mTechNodePressed = false;
		}
	}

	return theSuccess;
}

AvHMessageID AvHCommanderModeHandler::GetDisplayMenuMessageID() const
{
	return this->mDisplayMenuMessageID;
}

bool AvHCommanderModeHandler::GetDefaultOrderPosition(float& outWorldX, float& outWorldY) const
{
	bool theSuccess = false;
	
	if(this->mHasDefaultOrderPosition)
	{
		outWorldX = this->mDefaultOrderWorldX;
		outWorldY = this->mDefaultOrderWorldY;
		theSuccess = true;
	}
	
	return theSuccess;
}

bool AvHCommanderModeHandler::GetMoveToWorldPosition(float& outWorldX, float& outWorldY) const
{
	bool theSuccess = false;
	
	if(this->mHasMoveToPosition)
	{
		outWorldX = this->mMoveToWorldX;
		outWorldY = this->mMoveToWorldY;
		theSuccess = true;
	}
	
	return theSuccess;
	
}

bool AvHCommanderModeHandler::GetMouseOneDown() const
{
	return this->mMouseOneDown;
}

string AvHCommanderModeHandler::GetBuildResearchText() const
{
	return this->mBuildResearchText;
}

string AvHCommanderModeHandler::GetTechHelpText() const
{
	return this->mTechHelpText;
}

AvHMessageID AvHCommanderModeHandler::HotKeyHit(char inChar)
{
	AvHMessageID theMessageID = MESSAGE_NULL;

	// Map hotkey to index
	int theCol, theRow;
	if(AvHActionButtons::HotKeyToButtonIndex(inChar, theCol, theRow))
	{
		AvHActionButtons* theActionButtons = NULL;
		if(gHUD.GetManager().GetVGUIComponentNamed(kActionButtonsComponents, theActionButtons))
		{
			//ActionButton* theActionButton = theActionButtons->GetActionButtonWithHotKey(inChar);
			ActionButton* theActionButton = theActionButtons->GetActionButtonAtPos(theCol, theRow);
			if(theActionButton)
			{
				this->ActionButtonActivated(theActionButton);
				theMessageID = theActionButton->GetMessageID();
			}
		}
	}

	return theMessageID;
}

void AvHCommanderModeHandler::DefaultOrderToLastMousePosition(AvHTeamHierarchy* inHierarchy)
{
	ASSERT(inHierarchy);
	inHierarchy->GetWorldPosFromMouse(this->mLastMouseX, this->mLastMouseY, this->mDefaultOrderWorldX, this->mDefaultOrderWorldY);
	this->mHasDefaultOrderPosition = true;
}

void AvHCommanderModeHandler::MoveToLastMousePosition(AvHTeamHierarchy* inHierarchy)
{
	ASSERT(inHierarchy);
	inHierarchy->GetWorldPosFromMouse(this->mLastMouseX, this->mLastMouseY, this->mMoveToWorldX, this->mMoveToWorldY);
	gHUD.GetOverviewMap().SetWorldPosition(this->mMoveToWorldX, this->mMoveToWorldY);
	this->mHasMoveToPosition = true;
}

// Look at currently selected units and set our help text.  This is only used for research currently
void AvHCommanderModeHandler::RecalculateBuildResearchText()
{
	this->mBuildResearchText = "";

	if(this->mSelected.size() == 1)
	{
		// Get entity
		int theEntityIndex = *this->mSelected.begin();
		cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(theEntityIndex);
		if(theEntity)
		{
			bool theIsBuilding, theIsResearching;
			float thePercentage;
			string theHelpPrefix;

			AvHSHUGetBuildResearchState(theEntity->curstate.iuser3, theEntity->curstate.iuser4, theEntity->curstate.fuser1, theIsBuilding, theIsResearching, thePercentage);

			// Special-case for energy
			if(theIsBuilding && theIsResearching && (thePercentage > 0.0f))
			{
				LocalizeString(kEnergyPrefix, theHelpPrefix);
			
				// Read energy
				int theEnergy = thePercentage*kMarineStructureMaxEnergy;
				this->mBuildResearchText = theHelpPrefix + string(" ") + MakeStringFromInt(theEnergy);
			}
			// Check if it's recycling
			else if(!theIsBuilding && theIsResearching && GetHasUpgrade(theEntity->curstate.iuser4, MASK_RECYCLING))
			{
				LocalizeString("Recycling", this->mBuildResearchText);
			}
			// Check to see if it's being built
			else if((theIsBuilding || theIsResearching) && (thePercentage < 1.0f))
			{
				if(theIsBuilding)
				{
					// Set help text to "Building: <tech name>"
					LocalizeString(kBuildingPrefix, theHelpPrefix);
				}
				else if(theIsResearching)
				{
					// Set help text to "Researching: <tech name>"
					LocalizeString(kResearchingPrefix, theHelpPrefix);
				}
				
				string theHelpText;
				AvHMessageID theResearchID = (AvHMessageID)(theEntity->curstate.iuser2);
				if(theResearchID != MESSAGE_NULL)
				{
					if(ActionButton::GetLabelForMessage(theResearchID, theHelpText))
					{
						string theFinalMessage = theHelpPrefix + theHelpText;
						this->mBuildResearchText = theFinalMessage;
					}
				}
			}
			else
			{
				if(GetHasUpgrade(theEntity->curstate.iuser4, MASK_BUILDABLE))
				{
					// Look up name for entity and show it
					AvHMessageID theResearchID = (AvHMessageID)(theEntity->curstate.iuser2);
					if(theResearchID != MESSAGE_NULL)
					{
						ActionButton::GetLabelForMessage(theResearchID, this->mBuildResearchText);
					}
				}
				// If we have a resource selected, set the resource level
				else if(theEntity->curstate.iuser3 == AVH_USER3_FUNC_RESOURCE)
				{
					int theResources = (int)(theEntity->curstate.fuser1/kNormalizationNetworkFactor);
					if(theResources > 0)
					{
						if(LocalizeString(kResourcesTitle, this->mBuildResearchText))
						{
							char theResourceMessage[64];
							sprintf(theResourceMessage, "%d", theResources);
							this->mBuildResearchText += string(theResourceMessage);
						}
					}
					else
					{
						LocalizeString(kResourcesDepletedTitle, this->mBuildResearchText);
					}
				}
				else if(theEntity->curstate.iuser3 == AVH_USER3_INFANTRYPORTAL)
				{
//					int the = (int)(theEntity->curstate.fuser1/kNormalizationNetworkFactor);
//					if(theResources > 0)
//					{
						if(LocalizeString(kReinforcementsTitle, this->mBuildResearchText))
						{
							// TODO: Read this number correctly
							//char theMessage[64];
							//sprintf(theMessage, "%d", 100);
							//this->mBuildResearchText += string(theMessage);
						}
//					}
//					else
//					{
//						LocalizeString(kResourcesDepletedTitle, this->mBuildResearchText);
//					}
				}
			}
		}
	}
}

// Player isn't commanding, make sure UI is reset
void AvHCommanderModeHandler::Reset()
{
	//EntityListType theEmptySelection;
	//this->SetSelectedUnits(theEmptySelection);

	this->Init();

	this->RecalculateMenus();
}

// Look at currently selected units and set up our menus
void AvHCommanderModeHandler::RecalculateMenus()
{
	bool theSuccess = false;
	int theUser4 = 0;
	float theFuser1 = 0;

	// If we have only one thing selected, it can be a building
	int theNumUnits = (int)this->mSelected.size();
	if(theNumUnits > 0)
	{
		// Assumes all of selected is of same type
		int theIndex = *this->mSelected.begin();
		cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(theIndex);
		if(theEntity)
		{
			// Display icons for this type of structure
			AvHUser3 theUser3 = (AvHUser3)(theEntity->curstate.iuser3);
			
			if((theUser3 == AVH_USER3_MARINE_PLAYER) || GetHasUpgrade(theEntity->curstate.iuser4, MASK_RECYCLING))
			{
				// Set to base marine menu if current isn't compatible with it
				switch(this->mDisplayMenu)
				{
				case AVH_USER3_MENU_BUILD:
				case AVH_USER3_MENU_BUILD_ADVANCED:
				case AVH_USER3_MENU_ASSIST:
				case AVH_USER3_MENU_EQUIP:
					// Do nothing, keep current menu
					break;

				default:
					this->mDisplayMenu = AVH_USER3_MENU_BUILD;
				}
				theSuccess = true;
			}
			else
			{
				this->mDisplayMenu = theUser3;
				theUser4 = theEntity->curstate.iuser4;
				theFuser1 = theEntity->curstate.fuser1;
				theSuccess = true;
			}
		}
	}

	// Menu takes precedence over player selection, to allow ordering selected players to construct
//	else if(this->mDisplayMenu == AVH_USER3_NONE)
//	{
//		int theCommonUser4 = 0;
//		if(this->GetIsSelectionAllPlayers(&theCommonUser4))
//		{
//			this->mDisplayMenu = AVH_USER3_COMMANDER_PLAYER;
//			theUser4 = theCommonUser4;
//		}
//	}

	if(this->mDisplayMenu == AVH_USER3_NONE)
	{
		this->mDisplayMenu = AVH_USER3_MENU_BUILD;
	}

	// Recalculate iuser4 for special menu options
	int theMenuTechSlots = gHUD.GetMenuTechSlots();
	switch(this->mDisplayMenu)
	{
	case AVH_USER3_MENU_BUILD:
	//case AVH_USER3_COMMANDER_PLAYER:
		theUser4 = theMenuTechSlots << 3;
		break;
	case AVH_USER3_MENU_BUILD_ADVANCED:
		theUser4 = theMenuTechSlots >> 5;
		break;
	case AVH_USER3_MENU_ASSIST:
		theUser4 = theMenuTechSlots >> 13;
		break;
	case AVH_USER3_MENU_EQUIP:
		theUser4 = theMenuTechSlots >> 21;
		break;
	}

	// Show menu according to our current mode and data
	this->SetMenuFromUserVariables(this->mDisplayMenu, theUser4, theFuser1);
}

bool AvHCommanderModeHandler::GetIsSelectionAllPlayers(int* outCommonUser4) const
{
	bool theSuccess = false;

	if(outCommonUser4)
	{
		*outCommonUser4 = 0xFFFFFFFF;
	}

	for(EntityListType::const_iterator theIter = this->mSelected.begin(); theIter != this->mSelected.end(); theIter++)
	{
		theSuccess = true;

		// Lookup class name for player
//		string theClassName;
//		if(this->LookupClassNameForEntity(*theIter, theClassName))
//		{
//			if(theClassName != kAvHPlayerClassName)
//			{
		cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(*theIter);
		if(!theEntity || (((AvHUser3)theEntity->curstate.iuser3) != AVH_USER3_MARINE_PLAYER))
		{
			theSuccess = false;
			break;
		}

		// AND all the user4's together to see what actions are applicable to this group (may not be used)
		if(theEntity && outCommonUser4)
		{
			*outCommonUser4 &= theEntity->curstate.iuser4;
		}
	}

	return theSuccess;
}

void AvHCommanderModeHandler::SetSelectedUnits(const EntityListType& inUnits)
{
	this->mSelected = inUnits;

//	if(this->mSelected.size() > 0)
//	{
//		this->mDisplayMenu = AVH_USER3_NONE;
//	}

	this->RecalculateMenus();
}

void AvHCommanderModeHandler::SetResearchingMenu()
{
	AvHActionButtons* theActionButtons = NULL;
	if(gHUD.GetManager().GetVGUIComponentNamed(kActionButtonsComponents, theActionButtons))
	{
		theActionButtons->ClearButtons();

		theActionButtons->SetButton(0, MENU_BUILD);
		theActionButtons->SetButton(1, MENU_BUILD_ADVANCED);
		theActionButtons->SetButton(2, MENU_ASSIST);
		theActionButtons->SetButton(3, MENU_EQUIP);

		theActionButtons->SetButton(kNumTechSlots - 1, MESSAGE_CANCEL);

		theActionButtons->Localize();
	}
}

bool AvHCommanderModeHandler::SetMenuFromUserVariables(int inUser3, int inUser4, float inFuser1)
{
	bool theSuccess = false;

	AvHTechSlots theTechSlots;
	if(gHUD.GetTechSlotManager().GetTechSlotList((AvHUser3)inUser3, theTechSlots))
	{
		this->SetMenuFromTechSlots(theTechSlots, inUser3, inUser4, inFuser1);
		theSuccess = true;
	}

	return theSuccess;
}

void AvHCommanderModeHandler::SetMenuFromTechSlots(const AvHTechSlots& inTechSlots, int inUser3, int inUser4, float inFuser1)
{
	AvHActionButtons* theActionButtons = NULL;
	if(gHUD.GetManager().GetVGUIComponentNamed(kActionButtonsComponents, theActionButtons))
	{
		//string theNotFullyBuiltText;
		//if(LocalizeString(kNotFullyBuilt, theNotFullyBuiltText))

		theActionButtons->ClearButtons();

		bool theIsBuilding, theIsResearching;
		float thePercentage;
		AvHSHUGetBuildResearchState(inUser3, inUser4, inFuser1, theIsBuilding, theIsResearching, thePercentage);

		bool theDisplayingSpecialMenu = false;
		switch(inUser3)
		{
		case AVH_USER3_MENU_BUILD:
		case AVH_USER3_MENU_BUILD_ADVANCED:
		case AVH_USER3_MENU_ASSIST:
		case AVH_USER3_MENU_EQUIP:
			theDisplayingSpecialMenu = true;
			break;
		}

		// if recycling, draw nothing (check if something is selected, as otherwise inUser3 could be MENU_ something and MASK_RECYCLING won't make sense)
		if(!theDisplayingSpecialMenu && (this->mSelected.size() > 0) && GetHasUpgrade(inUser4, MASK_RECYCLING))
		{
		}
		// else if we're placing a building or researching, don't display anything but cancel
		else if((gHUD.GetGhostBuilding() != MESSAGE_NULL) || (theIsResearching && !theIsBuilding))
		{
			// Set last button as cancel
			theActionButtons->SetButton(0, MENU_BUILD);
			theActionButtons->SetButton(1, MENU_BUILD_ADVANCED);
			theActionButtons->SetButton(2, MENU_ASSIST);
			theActionButtons->SetButton(3, MENU_EQUIP);

			theActionButtons->SetButton(kNumActionButtonRows*kNumActionButtonCols - 1, MESSAGE_CANCEL);
		}
		// else use the menu the server specifies
		else
		{
			// Set hard-coded top bar
			theActionButtons->SetButton(0, MENU_BUILD);
			theActionButtons->SetButton(1, MENU_BUILD_ADVANCED);
			theActionButtons->SetButton(2, MENU_ASSIST);
			theActionButtons->SetButton(3, MENU_EQUIP);
			
			const int kBaseButtonIndex = 4;

			// else, query for rest of state
			for(int i=0; i < kNumTechSlots; i++)
			{
				AvHMessageID theMessageID = inTechSlots.mTechSlots[i];
				if(theMessageID != MESSAGE_NULL)
				{
					int theCurrentButtonIndex = kBaseButtonIndex + i;
					theActionButtons->SetButton(theCurrentButtonIndex, theMessageID);
			
					// Now set it's enabled state generically
					bool theEnabledState = AvHGetTechSlotEnabled(i, inUser4);
					theActionButtons->SetEnabledState(theCurrentButtonIndex, theEnabledState);
				}
			}
		}

		theActionButtons->Localize();
	}
}

bool AvHCommanderModeHandler::GetIsPointInPanel(Panel* inPanel, int x, int y) const
{

    int theWidth;
    int theHeight;

    inPanel->getSize(theWidth, theHeight);

    return (x >= 0 && y >= 0 && x < theWidth && y < theHeight);

}

void AvHCommanderModeHandler::Update(const AvHTechTree& inTechNodes, int inNumPoints)
{
	AvHActionButtons* theActionButtons = NULL;
	if(gHUD.GetManager().GetVGUIComponentNamed(kActionButtonsComponents, theActionButtons))
	{
		theActionButtons->SetTechNodes(inTechNodes);
		theActionButtons->SetResources(inNumPoints);

		int theEnergy = 0;
		if(this->mSelected.size() > 0)
		{
			int theEntityIndex = *this->mSelected.begin();
			cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(theEntityIndex);
			if(theEntity)
			{
				bool theIsBuilding, theIsResearching;
				float thePercentage;
				string theHelpPrefix;
				
				AvHSHUGetBuildResearchState(theEntity->curstate.iuser3, theEntity->curstate.iuser4, theEntity->curstate.fuser1, theIsBuilding, theIsResearching, thePercentage);
				
				// Special-case for energy
				if(theIsBuilding && theIsResearching && (thePercentage > 0.0f))
				{
					// Read energy
					theEnergy = thePercentage*kMarineStructureMaxEnergy;
				}
			}
		}
		theActionButtons->SetEnergy(theEnergy);

		theActionButtons->UpdateEnabledState();
		
		// Enable tech buttons
		theActionButtons->SetBusy(false);

		theActionButtons->UpdateEnabledAndResearchState();

		// Update prereqs and things, they could've changed if research just completed
		theActionButtons->Localize();
		
		this->RecalculateMenus();
	}
}

void AvHCommanderModeHandler::cursorMoved(int x, int y, Panel* inPanel)
{
	
    bool theFoundTechNode = false;

	this->mLastMouseX = x;
	this->mLastMouseY = y;

    if (!gHUD.GetIsSelecting())
    {

	    ActionButton* theActionButton = dynamic_cast<ActionButton*>(inPanel);

	    if(theActionButton)
	    {
		
            // We don't get proper exit notification when we've got the mouse captured
            // so generate it every mouse move.

            if (GetIsPointInPanel(inPanel, x, y))
            {
                theActionButton->cursorEntered();
            }
            else
            {
                theActionButton->cursorExited();
            }
            
            this->mTechHelpText = theActionButton->GetHelpText();
		    this->mLastTechNodeMouseX = x;
		    this->mLastTechNodeMouseY = y;
		    theFoundTechNode = true;

	    }
	    else
	    {
		    // Clear help text otherwise
		    this->mTechHelpText = "";
	    }

	    AvHTeamHierarchy* theHierarchy = dynamic_cast<AvHTeamHierarchy*>(inPanel);
	    if(theHierarchy)
	    {
		    if(this->mMouseOneDown)
		    {
			    //theHierarchy->setAsMouseArena(true);
			    this->MoveToLastMousePosition(theHierarchy);
		    }
	    }
    
    }
}

void AvHCommanderModeHandler::cursorEntered(Panel* inPanel)
{

    if (!gHUD.GetIsSelecting())
    {

	    ActionButton* theActionButton = dynamic_cast<ActionButton*>(inPanel);
	    if(theActionButton)
	    {
		    theActionButton->cursorEntered();
	    }
    
    }

}

void AvHCommanderModeHandler::cursorExited(Panel* inPanel)
{

    if (!gHUD.GetIsSelecting())
    {

	    ActionButton* theActionButton = dynamic_cast<ActionButton*>(inPanel);
	    if(theActionButton)
	    {
		    theActionButton->cursorExited();
	    }
    
    }

}

void AvHCommanderModeHandler::mousePressed(MouseCode code, Panel* inPanel)
{
    // Capture the mouse input so that we receive the mouseRelease event even
    // if the cursor is no longer on the panel.
    App::getInstance()->setMouseCapture(inPanel);

	if(code == MOUSE_LEFT)
	{
		this->mMouseOneDown = true;

		AvHTeamHierarchy* theHierarchy = dynamic_cast<AvHTeamHierarchy*>(inPanel);
		if(theHierarchy)
		{
			this->MoveToLastMousePosition(theHierarchy);
		}

		// Remember which component the mouse clicked so we don't allow any old mouse up
		if(dynamic_cast<ActionButton*>(inPanel) || dynamic_cast<AvHActionButtons*>(inPanel))
		{
			this->mMouseOneDownOnActionButtons = true;
		}
		if(dynamic_cast<AvHLogoutComponent*>(inPanel))
		{
			this->mMouseOneDownOnLogoutButton = true;
		}

		AvHTechImpulsePanel* theImpulsePanel = dynamic_cast<AvHTechImpulsePanel*>(inPanel);
		if(theImpulsePanel)
		{
			this->mLastTechPressed = theImpulsePanel->GetMessageID();
			this->mTechNodePressed = true;
		}
	}
	else if(code == MOUSE_RIGHT)
	{
		this->mMouseTwoDown = true;

		AvHTeamHierarchy* theHierarchy = dynamic_cast<AvHTeamHierarchy*>(inPanel);
		if(theHierarchy)
		{
			//this->DefaultOrderToLastMousePosition(theHierarchy);
		}
	}
}

void AvHCommanderModeHandler::mouseDoublePressed(MouseCode code, Panel* inPanel)
{
}

void AvHCommanderModeHandler::ActionButtonActivated(ActionButton* inActionButton)
{
	if(!inActionButton->GetBusy())
	{
		gHUD.PlayHUDSound(HUD_SOUND_SELECT);

		bool theSuccess = false;
		
		AvHMessageID theMessageID = inActionButton->GetMessageID();
		switch(theMessageID)
		{
		case MENU_BUILD:
			this->mDisplayMenu = AVH_USER3_MENU_BUILD;
			theSuccess = true;
			break;
		case MENU_BUILD_ADVANCED:
			this->mDisplayMenu = AVH_USER3_MENU_BUILD_ADVANCED;
			theSuccess = true;
			break;
		case MENU_ASSIST:
			this->mDisplayMenu = AVH_USER3_MENU_ASSIST;
			theSuccess = true;
			break;
		case MENU_EQUIP:
			this->mDisplayMenu = AVH_USER3_MENU_EQUIP;
			theSuccess = true;
			break;
		default:
			// See if they pressed a button
			if(inActionButton->GetTechEnabled())
			{
				this->mLastTechPressed = theMessageID;
				this->mTechNodePressed = true;
			}
			break;
		}

		if(theSuccess)
		{
			this->mDisplayMenuMessageID = theMessageID;

			// If our selection isn't all players, remove the selection.
			if(!this->GetIsSelectionAllPlayers())
			{
				gHUD.ClearSelection();
			}
		}
	}
}

void AvHCommanderModeHandler::mouseReleased(MouseCode code, Panel* inPanel)
{

    App::getInstance()->setMouseCapture(NULL);

    if(code == MOUSE_LEFT)
	{

        if (GetIsPointInPanel(inPanel, mLastMouseX, mLastMouseY))
        {
        
            ActionButton* theActionButton = dynamic_cast<ActionButton*>(inPanel);

		    if(theActionButton && this->mMouseOneDownOnActionButtons)
		    {
			    this->ActionButtonActivated(theActionButton);
		    }
		    else if(dynamic_cast<AvHLogoutComponent*>(inPanel) && this->mMouseOneDownOnLogoutButton)
		    {
			    if(gHUD.GetUIMode() == MAIN_MODE)
			    {
				    ClientCmd(kcStopCommandMode);
				    this->mDisplayMenu = AVH_USER3_MENU_BUILD;
				    this->mDisplayMenuMessageID = MENU_BUILD;
			    }
		    }
        
        }

		this->mMouseOneDown = false;
		this->mMouseOneDownOnActionButtons = false;
		this->mMouseOneDownOnLogoutButton = false;
	}
	else if(code == MOUSE_RIGHT)
	{
		this->mMouseTwoDown = false;
	}
}		

void AvHCommanderModeHandler::mouseWheeled(int delta, Panel* inPanel)
{
}

void AvHCommanderModeHandler::keyPressed(KeyCode code, Panel* inPanel)
{
}

void AvHCommanderModeHandler::keyTyped(KeyCode code, Panel* inPanel)
{
}

void AvHCommanderModeHandler::keyReleased(KeyCode code, Panel* inPanel)
{
}

void AvHCommanderModeHandler::keyFocusTicked(Panel* inPanel)
{
}
