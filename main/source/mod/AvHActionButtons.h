//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHActionButtons.h $
// $Date: 2002/05/23 02:34:00 $
//
//-------------------------------------------------------------------------------
// $Log: AvHActionButtons.h,v $
// Revision 1.4  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVHACTIONBUTTONS_H
#define AVHACTIONBUTTONS_H

#include "vgui_Panel.h"
#include "ui/UIComponents.h"
#include "mod/AvHTechTree.h"
#include "ui/StaticLabel.h"

const int kNumActionButtonRows = 3;
const int kNumActionButtonCols = 4;

class ActionButton : public StaticLabel //public vgui::Button
{
public:
						ActionButton(const char* text,int x,int y);
	bool				GetBusy() const;
	bool				GetMouseOver() const;
	bool				GetCostMet() const;
	string				GetHelpText() const;
	bool				GetTechEnabled() const;
	AvHMessageID		GetMessageID() const;
	void				Localize(const AvHTechTree& inTechNodes);
	void				SetBusy(bool inBusy);
	void				SetButtonIndex(int inButtonIndex);
	void				SetEnabledState(bool inEnabledState);
	void				SetMessageID(AvHMessageID inMessageID);
	void				UpdateEnabledState(int inCurrentPoints, int inEnergy);
	void				UpdateEnabledAndResearchState(const AvHTechTree& inTechNodes);
	
	static bool			GetLabelForMessage(AvHMessageID inMessageID, string& outLabel);

	void				cursorEntered();
	void				cursorExited();
	virtual void		getPos(int& x, int& y);
	
protected:
	virtual void		paint();
	virtual void		paintBackground();
	
private:
	bool				mTechEnabled;
	bool				mCostMet;
	bool				mResearched;
	AvHMessageID		mMessageID;
	int					mCost;
	string				mHelpText;
	bool				mBusy;
	bool				mMouseOver;
	int					mButtonIndex;
};

class AvHActionButtons : public vgui::Panel
{
public:
	static const char*	GetHotKeyAKA();
	static bool			ButtonIndexToHotKey(int inCol, int inRow, char& outChar);
	static bool			HotKeyToButtonIndex(char inChar, int& outCol, int& outRow);

						AvHActionButtons();
						~AvHActionButtons();

	void				ClearButtons();
	void				Localize();
	bool				SetButton(int inButtonOffset, AvHMessageID inTechID);
	//bool				SetResearchButton(int inButtonOffset, AvHMessageID inTechID);
	//bool				SetTechIfOtherTechResearched(int inButtonOffset, AvHTechID inTechID, AvHTechID inResearchedTech);

	int					GetNumCols() const;
	int					GetNumRows() const;
	ActionButton*		GetActionButtonAtPos(int inCol, int inRow);
	
	virtual void		addInputSignal(InputSignal* s);
	void				setFont(Font* inFont);
	void				setFont(Scheme::SchemeFont schemeFont);

	virtual void		setFgColor(int r,int g,int b,int a);
	virtual void		setBgColor(int r,int g,int b,int a);
	
	virtual void		setPos(int inX, int inY);
	virtual void		setSize(int inWidth, int inHeight);
	virtual void		setVisible(bool inState);

	void				SetBusy(bool inBusy);
	void				SetEnabledState(int inButtonOffset, bool inEnabledState);

	void				SetTechNodes(const AvHTechTree& inTechNodes);
	void				SetResources(int inResources);
	void				SetEnergy(int inEnergy);
	void				UpdateEnabledState();
	void				UpdateEnabledAndResearchState();
	
protected:
	virtual void		paint();
	virtual void		paintBackground();
	void				SetButtonsToGrid();

	TextImage			mTextImage;
	Font*				mTechFont;
	ActionButton**		mButtonArray;
	AvHTechTree			mTechNodes;
	int					mResources;
	int					mEnergy;
	bool				mBusy;
};




class AvHUIActionButtons : public UIPanel
{
public:
							AvHUIActionButtons(void);
	
	// Destructor automatically removes component from the engine
	virtual					~AvHUIActionButtons(void);
	
	virtual Panel*			GetComponentPointer(void);
	
	virtual const string&	GetType(void) const;
	
	virtual bool			SetClassProperties(const TRDescription& inDescription, Panel* inComponent, CSchemeManager* inSchemeManager);
	
private:
	
	void                    AllocateComponent(const TRDescription& inDescription);
	
    AvHActionButtons*		mActionButtonsComponent;
	
    string                  mType;
	
};

#endif
