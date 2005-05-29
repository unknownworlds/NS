//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHCommanderModeHandler.h $
// $Date: 2002/10/16 00:50:17 $
//
//-------------------------------------------------------------------------------
// $Log: AvHCommanderModeHandler.h,v $
// Revision 1.15  2002/10/16 00:50:17  Flayra
// - Phase gates can be recycled now
//
// Revision 1.14  2002/06/25 17:45:07  Flayra
// - Renamed some buildings, armory is now upgraded
//
// Revision 1.13  2002/06/10 19:51:06  Flayra
// - New commander UI
//
// Revision 1.12  2002/06/03 16:40:24  Flayra
// - Renamed weapons factory and armory
//
// Revision 1.11  2002/05/28 17:37:43  Flayra
// - Added recycling, added fix for occasional crash when selecting command station with some lag
//
// Revision 1.10  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVHCOMMANDERMODEHANDLER_H
#define AVHCOMMANDERMODEHANDLER_H

#include <VGUI_InputSignal.h>
#include "types.h"
#include "mod/AvHTeamHierarchy.h"
#include "mod/AvHActionButtons.h"
#include "mod/AvHTechSlotManager.h"

using namespace vgui;

class AvHCommanderModeHandler : public InputSignal
{
public:
					AvHCommanderModeHandler();

			void	ActionButtonActivated(ActionButton* inActionButton);
			void	CancelHit();
			void	ClearDefaultOrderPosition();
			void	ClearMoveToPosition();
			bool	GetAndClearTechNodePressed(AvHMessageID& outMessageID, bool inClear = true);
	AvHMessageID	GetDisplayMenuMessageID() const;
			bool	GetDefaultOrderPosition(float& outWorldX, float& outWorldY) const;
			bool	GetMoveToWorldPosition(float& outWorldX, float& outWorldY) const;
			bool	GetMouseOneDown() const;
			string	GetBuildResearchText() const;
			string	GetTechHelpText() const;
	AvHMessageID	HotKeyHit(char inChar);
			void	RecalculateBuildResearchText();
			void	Reset();
			void	SetSelectedUnits(const EntityListType& inUnits);
			void	Update(const AvHTechNodes& inTechNodes, int inNumPoints);

	virtual void	cursorMoved(int x,int y,Panel* panel);
	virtual void	cursorEntered(Panel* panel);
	virtual void	cursorExited(Panel* panel);
	virtual void	mousePressed(MouseCode code,Panel* panel);
	virtual void	mouseDoublePressed(MouseCode code,Panel* panel);
	virtual void	mouseReleased(MouseCode code,Panel* panel);
	virtual void	mouseWheeled(int delta,Panel* panel);
	virtual void	keyPressed(KeyCode code,Panel* panel);
	virtual void	keyTyped(KeyCode code,Panel* panel);
	virtual void	keyReleased(KeyCode code,Panel* panel);
	virtual void	keyFocusTicked(Panel* panel);

private:
	void			Init();
	void			DefaultOrderToLastMousePosition(AvHTeamHierarchy* inHierarchy);
	void			MoveToLastMousePosition(AvHTeamHierarchy* inHierarchy);
	bool			GetIsSelectionAllPlayers(int* outCommonUser4 = NULL) const;

//	bool			LookupClassNameForEntity(int inEntityIndex, string& outClassName) const;
	void			RecalculateMenus();
	void			SetResearchingMenu();
	void			SetBaseMenu(bool inForce = false);
	
	void			SetGenericBuildingMenu();

	void			SetMenuFromTechSlots(const AvHTechSlots& inTechSlots, int inUser3, int inUser4, float inFuser1);
    bool			SetMenuFromUserVariables(int inUser3, int inUser4 = 0, float inFuser1 = 0);

    bool            GetIsPointInPanel(Panel* inPanel, int x, int y) const; // Client space.

	int				mLastTechNodeMouseX;
	int				mLastTechNodeMouseY;

	int				mLastMouseX;
	int				mLastMouseY;

	bool			mMouseOneDown;
	bool			mMouseOneDownOnActionButtons;
	bool			mMouseOneDownOnLogoutButton;

	bool			mMouseTwoDown;

	bool			mHasMoveToPosition;
	float			mMoveToWorldX;
	float			mMoveToWorldY;

	bool			mHasDefaultOrderPosition;
	float			mDefaultOrderWorldX;
	float			mDefaultOrderWorldY;

	EntityListType	mSelected;

	//AvHTechNode		mLastTechNodePressed;
	AvHMessageID	mLastTechPressed;
	bool			mTechNodePressed;

	string			mTechHelpText;
	string			mBuildResearchText;

	AvHUser3		mDisplayMenu;
	AvHMessageID	mDisplayMenuMessageID;

};

#endif
