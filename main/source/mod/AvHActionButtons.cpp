//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHActionButtons.cpp$
// $Date: 2002/06/10 19:47:55 $
//
//-------------------------------------------------------------------------------
// $Log: AvHActionButtons.cpp,v $
// Revision 1.11  2002/06/10 19:47:55  Flayra
// - Removed accelerator character, now these are bindable via commands
//
// Revision 1.10  2002/05/23 02:34:00  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//
//===============================================================================
#include "mod/AvHActionButtons.h"
#include "ui/UITags.h"
#include "ui/UIUtil.h"
#include "util/STLUtil.h"
#include "mod/AvHMessage.h"
#include "mod/AvHClientUtil.h"
#include "mod/AvHClientVariables.h"
#include "mod/AvHSharedUtil.h"

const int	kLineWidth = 1;
//const char	kAcceleratorCharacter = '-';

ActionButton::ActionButton(const char* text,int x,int y) : StaticLabel(0, 0) //Button(text, x, y)
{
	this->mMessageID = MESSAGE_NULL;
	this->mCost = 0;
	this->mTechEnabled = true;
	this->mCostMet = true;
//	this->mResearched = false;
	this->mBusy = false;
	this->mMouseOver = false;
	this->mButtonIndex = 0;
}

void ActionButton::cursorEntered()
{
	this->mMouseOver = true;
}

void ActionButton::cursorExited()
{
	this->mMouseOver = false;
}

void ActionButton::getPos(int& x, int& y)
{
	Panel* theParent = this->getParent();

	theParent->getPos(x, y);

	int theLocalX, theLocalY;
	StaticLabel::getPos(theLocalX, theLocalY);

	x += theLocalX;
	y += theLocalY;
}

bool ActionButton::GetBusy() const
{
	return this->mBusy;
}

bool ActionButton::GetMouseOver() const
{
	return this->mMouseOver;
}

bool ActionButton::GetCostMet() const
{
	return this->mCostMet;
}

string ActionButton::GetHelpText() const
{
	return this->mHelpText;
}

bool ActionButton::GetTechEnabled() const
{
	return this->mTechEnabled;
}

AvHMessageID ActionButton::GetMessageID() const
{
	return this->mMessageID;
}

bool ActionButton::GetLabelForMessage(AvHMessageID inMessage, string& outLabel)
{
	bool theSuccess = false;

	// Localize prereq, without IT's prereq
	string theKey = string(kTechNodeLabelPrefix) + MakeStringFromInt((int)inMessage);
	if(LocalizeString(theKey.c_str(), outLabel))
	{
		// Remove it's accelerator if it has one
		//std::remove(outLabel.begin(), outLabel.end(), kAcceleratorCharacter);
		theSuccess = true;
	}

	return theSuccess;
}

void ActionButton::UpdateEnabledAndResearchState(const AvHTechTree& inTechNodes)
{
	// Get cost
	bool theResearchable;
	float theTime;
	inTechNodes.GetResearchInfo(this->mMessageID, theResearchable, this->mCost, theTime);
	this->mTechEnabled = theResearchable;
	
	this->mResearched = false;
	AvHTechID theTechID = TECH_NULL;
	if(inTechNodes.GetTechForMessage(this->mMessageID, theTechID))
	{
		this->mResearched = inTechNodes.GetIsTechResearched(theTechID);
	}
}

// Run through the tech nodes and look up their label and help text, storing them for drawing
void ActionButton::Localize(const AvHTechTree& inTechNodes)
{
	if(this->mMessageID == MESSAGE_NULL)
	{
		this->setText("");
		this->mHelpText = "";
		this->mCost = 0;
	}
	else
	{
		char theNumber[8];
		sprintf(theNumber, "%d", (int)this->mMessageID);
		string theNumberString(theNumber);

		// If localize string fails, set the label to "<no desc>"
		string theText;
		string theKey = string(kTechNodeLabelPrefix) + theNumberString;
		LocalizeString(theKey.c_str(), theText);
		
		// Localize help string
		this->mHelpText = "";

		if(gHUD.GetHelpForMessage(this->mMessageID, this->mHelpText))
		{
			// Add hotkey accelerator
			if(this->mButtonIndex >= 0)
			{
				char theHotkeyChar = ' ';
				int theCol = this->mButtonIndex % kNumActionButtonCols;
				int theRow = this->mButtonIndex / kNumActionButtonCols;
				if(AvHActionButtons::ButtonIndexToHotKey(theCol, theRow, theHotkeyChar))
				{
					// Display as caps, looks nicer
					theHotkeyChar = toupper(theHotkeyChar);
					string theHotkeyText = string("(") + theHotkeyChar + string(")");
					this->mHelpText += string(" ");
					this->mHelpText += theHotkeyText;
				}
			}

			// new line after hotkey
			this->mHelpText += " \n";

			bool theFirstPrereq = true;

			// If there is a prerequisite that isn't researched, add this to help message
			AvHTechID thePrereqID1;
			AvHTechID thePrereqID2;
			if(inTechNodes.GetPrequisitesForMessage(this->mMessageID, thePrereqID1, thePrereqID2))
			{
				for(int i = 0; i < 2; i++)
				{
					AvHTechID theCurrentPrereq = ((i == 0) ? thePrereqID1 : thePrereqID2);
					if(theCurrentPrereq != TECH_NULL)
					{
						if(!inTechNodes.GetIsTechResearched(theCurrentPrereq))
						{
							//this->mTechEnabled = false;
						
							// Localize "prerequisite:"
							string thePrequisiteText;
							if(LocalizeString(kPrerequisitePrefix, thePrequisiteText))
							{
								// Get message from this tech
								AvHMessageID theMessageWithThisTech = MESSAGE_NULL;
								if(inTechNodes.GetMessageForTech(theCurrentPrereq, theMessageWithThisTech))
								{
									string thePrereqTech;
									if(GetLabelForMessage(theMessageWithThisTech, thePrereqTech))
									{
										// Add extra blank line before first prereq
										if(theFirstPrereq)
										{
											this->mHelpText += " \n";
											this->mHelpText += kTooltipBoldPreString;
											this->mHelpText += thePrequisiteText;
											this->mHelpText += "\n";
											theFirstPrereq = false;
										}

										// Prereqs draw as bold
										this->mHelpText += kTooltipBoldPreString;
										this->mHelpText += " - ";
										this->mHelpText += thePrereqTech;
										this->mHelpText += "\n";
									}
								}
							}
						}
					}
				}
			}

			// Blank line between hotkey/prereq and description
			this->mHelpText += " \n";

			// Add description
			AvHUser3 theUser3 = AVH_USER3_NONE;
			if(AvHSHUMessageIDToUser3(this->mMessageID, theUser3))
			{
				string theDescription;
				if(gHUD.GetTranslatedUser3Description(theUser3, true, theDescription))
				{
					this->mHelpText += theDescription;

					this->mHelpText += " \n";
				}
			}
		}
	}	
}

void ActionButton::SetBusy(bool inBusy)
{
	this->mBusy = false;
}

void ActionButton::SetButtonIndex(int inButtonIndex)
{
	this->mButtonIndex = inButtonIndex;
}

void ActionButton::SetEnabledState(bool inEnabledState)
{
	this->mTechEnabled = inEnabledState;
}

void ActionButton::SetMessageID(AvHMessageID inMessageID)
{
	this->mMessageID = inMessageID;
}

void ActionButton::UpdateEnabledState(int inCurrentPoints, int inEnergy)
{
	bool theCostsEnergy = AvHSHUGetDoesTechCostEnergy(this->mMessageID);
	int theValue = theCostsEnergy ? inEnergy : inCurrentPoints;

	if(theValue >= this->mCost)
	{
		this->mCostMet = true;
	}
	else
	{
		this->mCostMet = false;
	}
}

// TODO: Change how it draws when enabled or not
void ActionButton::paint()
{
	//if(!this->mResearched && !this->mBusy)
	//{
		if(this->mMessageID != MESSAGE_NULL)
		{
			StaticLabel::paint();
		}
	//}
}

void ActionButton::paintBackground()
{
//	if(!this->mTechEnabled || !this->mEnoughPoints)
//	{
//		int theX, theY;
//		this->getPos(theX, theY);
//
//		int theWidth, theHeight;
//		this->getSize(theWidth, theHeight);
//
//		//FillRGBA(theX, theY, theWidth, theHeight, 200, 100, 100, 100);
//		FillRGBA(0, 0, theWidth, theHeight, 200, 100, 100, 100);
//		//vguiSimpleBox(theX, theY, theX + theWidth, theY + theHeight, 200, 100, 100, 100);
//	}
}



AvHActionButtons::AvHActionButtons() : mTextImage("")
{
	this->mTechFont = NULL;
	this->mResources = 0;
	this->mEnergy = 0;
	this->mBusy = false;

	this->mButtonArray = new ActionButton*[kNumActionButtonRows*kNumActionButtonCols];
	for(int i = 0; i < kNumActionButtonRows*kNumActionButtonCols; i++)
	{
		// Every button is parented to the action buttons component
		string theText = string("tech: ") + MakeStringFromInt(i+1);
		this->mButtonArray[i] = new ActionButton(theText.c_str(), 0, 0);
		this->mButtonArray[i]->setParent(this);
		this->mButtonArray[i]->SetButtonIndex(i);
	}
}

AvHActionButtons::~AvHActionButtons()
{
	for(int i = 0; i < kNumActionButtonRows*kNumActionButtonCols; i++)
	{
		delete this->mButtonArray[i];
	}
	delete [] this->mButtonArray;
}

void AvHActionButtons::ClearButtons()
{
	for(int i = 0; i < kNumActionButtonRows*kNumActionButtonCols; i++)
	{
		this->mButtonArray[i]->SetMessageID(MESSAGE_NULL);
	}
}

// 4x3 array
const int kMaxNumActionButtons = 12;
char kHotKeyAKA[kMaxNumActionButtons] = {'q', 'w', 'e', 'r', 'a', 's', 'd', 'f', 'z', 'x', 'c', 'v'};

const char* AvHActionButtons::GetHotKeyAKA()
{
	char* theHotKeyAKA = kHotKeyAKA;

	char* theCustomHotKeyAKA = cl_cmhotkeys->string;
	if(strlen(theCustomHotKeyAKA) >= kMaxNumActionButtons)
	{
		theHotKeyAKA = theCustomHotKeyAKA;
	}

	return theHotKeyAKA;
}

bool AvHActionButtons::ButtonIndexToHotKey(int inCol, int inRow, char& outChar)
{
	bool theSuccess = false;

	const char* theHotKeyAKA = AvHActionButtons::GetHotKeyAKA();

	if((inCol >= 0) && (inCol < kNumActionButtonCols) && (inRow >= 0) && (inRow < kNumActionButtonRows))
	{
		int theIndex = inCol + inRow*kNumActionButtonCols;
		ASSERT(theIndex >= 0);
		ASSERT(theIndex < kMaxNumActionButtons);

		outChar = theHotKeyAKA[theIndex];

		theSuccess = true;
	}

	return theSuccess;
}

// Hard-code hotkeys to be in analagous key array
bool AvHActionButtons::HotKeyToButtonIndex(char inChar, int& outCol, int& outRow)
{
	bool theSuccess = false;

	int theChar = tolower(inChar);
	const char* theHotKeyAKA = AvHActionButtons::GetHotKeyAKA();
	
	for(int i = 0; i < kMaxNumActionButtons; i++)
	{
		if(theChar == theHotKeyAKA[i])
		{
			outCol = i % kNumActionButtonCols;
			outRow = i / kNumActionButtonCols;
			theSuccess = true;
			break;
		}
	}
	
	return theSuccess;
}

int	AvHActionButtons::GetNumCols() const
{
	return kNumActionButtonCols;
}

int	AvHActionButtons::GetNumRows() const
{
	return kNumActionButtonRows;
}

ActionButton* AvHActionButtons::GetActionButtonAtPos(int inCol, int inRow)
{
	ASSERT(inCol >= 0);
	ASSERT(inRow >= 0);
	ASSERT(inCol < kNumActionButtonCols);
	ASSERT(inRow < kNumActionButtonRows);

	int theIndex = inCol + inRow*kNumActionButtonCols;
	return this->mButtonArray[theIndex];
}


// Run through the tech nodes and look up their label and help text, storing them for drawing
void AvHActionButtons::Localize()
{
	for(int i = 0; i < kNumActionButtonRows*kNumActionButtonCols; i++)
	{
		this->mButtonArray[i]->Localize(this->mTechNodes);
	}
	this->SetButtonsToGrid();	
}

bool AvHActionButtons::SetButton(int inButtonOffset, AvHMessageID inMessageID)
{
	bool theSuccess = false;
	
	if(inButtonOffset < kNumActionButtonRows*kNumActionButtonCols)
	{
		ActionButton* theCurrentActionButton = this->mButtonArray[inButtonOffset];
		theCurrentActionButton->SetMessageID(inMessageID);
		theSuccess = true;
	}
	
	return theSuccess;
}

// Sets the button to the research tech, unless the tech has already been researched, in case it will set it to TECH_NULL
//bool AvHActionButtons::SetResearchButton(int inButtonOffset, AvHMessageID inMessageID)
//{
//	// If researched, set it to MESSAGE_NULL so it isn't displayed
//	AvHMessageID theAdjustedID = inMessageID;
//
//	bool theIsResearched = this->mTechNodes.GetIsTechResearched(inTechID);
//	if(theIsResearched)
//	{
//		theAdjustedID = MESSAGE_NULL;
//	}
//
//	return this->SetButton(inButtonOffset, theAdjustedID);
//}

//bool AvHActionButtons::SetTechIfOtherTechResearched(int inButtonOffset, AvHTechID inTechID, AvHTechID inResearchedTech)
//{
//	// If other tech isn't researched, don't display
//	bool theSuccess = false;
//	if(inButtonOffset < kNumActionButtonRows*kNumActionButtonCols)
//	{
//		ActionButton* theCurrentActionButton = this->mButtonArray[inButtonOffset];
//
//		bool theOtherTechIsResearched = this->mTechNodes.GetIsTechResearched(inResearchedTech);
//		theCurrentActionButton->SetTechID(inTechID, theOtherTechIsResearched);
//
//		theSuccess = true;
//	}
//
//	return theSuccess;
//}

void AvHActionButtons::SetTechNodes(const AvHTechTree& inTechNodes)
{
	if(this->mTechNodes != inTechNodes)
	{
		this->mTechNodes = inTechNodes;
		this->UpdateEnabledState();
	}
}

void AvHActionButtons::SetEnergy(int inEnergy)
{
	this->mEnergy = inEnergy;
}

void AvHActionButtons::SetResources(int inResources)
{
	this->mResources = inResources;
//	this->UpdateEnabledState();
}

void AvHActionButtons::UpdateEnabledState()
{
	for(int i = 0; i < kNumActionButtonRows*kNumActionButtonCols; i++)
	{
		this->mButtonArray[i]->UpdateEnabledState(this->mResources, this->mEnergy);
	}
}

void AvHActionButtons::UpdateEnabledAndResearchState()
{
	for(int i = 0; i < kNumActionButtonRows*kNumActionButtonCols; i++)
	{
		this->mButtonArray[i]->UpdateEnabledAndResearchState(this->mTechNodes);
	}
}


void AvHActionButtons::addInputSignal(InputSignal* s)
{
	Panel::addInputSignal(s);
	
	for(int i = 0; i < kNumActionButtonRows*kNumActionButtonCols; i++)
	{
		this->mButtonArray[i]->addInputSignal(s);
	}
}

void AvHActionButtons::setFgColor(int r,int g,int b,int a)
{
	Panel::setFgColor(r, g, b, a);
	
	for(int i = 0; i < kNumActionButtonRows*kNumActionButtonCols; i++)
	{
		this->mButtonArray[i]->setFgColor(r, g, b, a);
	}
}

void AvHActionButtons::setBgColor(int r,int g,int b,int a)
{
	Panel::setBgColor(r, g, b, a);
	
	for(int i = 0; i < kNumActionButtonRows*kNumActionButtonCols; i++)
	{
		this->mButtonArray[i]->setBgColor(r, g, b, a);
	}
}

void AvHActionButtons::setFont(Font* inFont)
{
	this->mTechFont = inFont;
	
	for(int i = 0; i < kNumActionButtonRows*kNumActionButtonCols; i++)
	{
		this->mButtonArray[i]->setFont(inFont);
	}
}

void AvHActionButtons::setFont(Scheme::SchemeFont schemeFont)
{
	for(int i = 0; i < kNumActionButtonRows*kNumActionButtonCols; i++)
	{
		this->mButtonArray[i]->setFont(schemeFont);
	}
}


void AvHActionButtons::setPos(int inX, int inY)
{
	Panel::setPos(inX, inY);
	
	// TODO: Tell every button what it's new pos is
}

void AvHActionButtons::SetButtonsToGrid()
{
	int theWidth, theHeight;
	Panel::getSize(theWidth, theHeight);

	int theButtonWidth = theWidth/kNumActionButtonCols;
	int theButtonHeight = theHeight/kNumActionButtonRows;
	
	// Tell every button what it's new size is
	for(int theRow = 0; theRow < kNumActionButtonRows; theRow++)
	{
		for(int theCol = 0; theCol < kNumActionButtonCols; theCol++)
		{
			int i = theCol + theRow*kNumActionButtonCols;
			this->mButtonArray[i]->SetStaticSize(theButtonWidth, theButtonHeight);
			//this->mButtonArray[i]->setSize(theButtonWidth, theButtonHeight);
			this->mButtonArray[i]->setPos(theCol*theButtonWidth, theRow*theButtonHeight);
		}
	}
}

void AvHActionButtons::setSize(int inWidth,int inHeight)
{
	Panel::setSize(inWidth, inHeight);

	this->SetButtonsToGrid();	
}

void AvHActionButtons::setVisible(bool inState)
{
	Panel::setVisible(inState);
	
	for(int i = 0; i < kNumActionButtonRows*kNumActionButtonCols; i++)
	{
		this->mButtonArray[i]->setVisible(inState);
	}
}

void AvHActionButtons::paint()
{
	//	for(int i = 0; i < kNumActionButtonRows*kNumActionButtonCols; i++)
	//	{
	//		this->mButtonArray[i]->paint();
	//	}
}

void AvHActionButtons::paintBackground()
{
	//	Color theColor;
	//	this->getBgColor(theColor);
	//	
	//	int r, g, b, a;
	//	theColor.getColor(r, g, b, a);
	//  
	//	a = 255 - a;
	//
	//	int theWidth, theHeight;
	//	this->getSize(theWidth, theHeight);
	//
	//	// Draw grid, but not edges, only middle lines (not first or last)
	//	for(int theCol = 1; theCol < kNumActionButtonCols; theCol++)
	//	{
	//		int theX = ((float)theCol/kNumActionButtonCols)*theWidth;
	//		FillRGBA(theX, 0, 1, theHeight, r, g, b, a);
	//	}
	//	
	//	for(int theRow = 1; theRow < kNumActionButtonRows; theRow++)
	//	{
	//		int theY = ((float)theRow/kNumActionButtonRows)*theHeight;
	//		FillRGBA(0, theY, theWidth, 1, r, g, b, a);
	//	}
}

void AvHActionButtons::SetBusy(bool inBusy)
{
	this->mBusy = inBusy;
	
	for(int i = 0; i < kNumActionButtonRows*kNumActionButtonCols; i++)
	{
		ActionButton* theButton = this->mButtonArray[i];
		ASSERT(theButton);
		theButton->SetBusy(inBusy);
	}
}

void AvHActionButtons::SetEnabledState(int inButtonOffset, bool inEnabledState)
{
	if(inButtonOffset < kNumActionButtonRows*kNumActionButtonCols)
	{
		ActionButton* theCurrentActionButton = this->mButtonArray[inButtonOffset];
		theCurrentActionButton->SetEnabledState(inEnabledState);
	}
}



AvHUIActionButtons::AvHUIActionButtons(void)
{
    this->mType = "ActionButtons";
}

void AvHUIActionButtons::AllocateComponent(const TRDescription& inDescription)
{
    this->mActionButtonsComponent = new AvHActionButtons();
}
                            
// Destructor automatically removes component from the engine
AvHUIActionButtons::~AvHUIActionButtons(void)
{
    delete this->mActionButtonsComponent;
    this->mActionButtonsComponent = NULL;
}

Panel* AvHUIActionButtons::GetComponentPointer(void)
{
    return this->mActionButtonsComponent;
}

const string& AvHUIActionButtons::GetType(void) const
{
    return this->mType;
}

bool AvHUIActionButtons::SetClassProperties(const TRDescription& inDesc, Panel* inComponent, CSchemeManager* inSchemeManager)
{
    bool theSuccess = false;
    AvHActionButtons* theActionButtons = dynamic_cast<AvHActionButtons*>(inComponent);
	ASSERT(theActionButtons);

    // read custom attributes here
    UIPanel::SetClassProperties(inDesc, inComponent, inSchemeManager);

    // Get tech font to use
    std::string theSchemeName;
    if(inDesc.GetTagValue(UITagScheme, theSchemeName))
    {
		const char* theSchemeCString = theSchemeName.c_str();
		SchemeHandle_t theSchemeHandle = inSchemeManager->getSchemeHandle(theSchemeCString);
		Font* theFont = inSchemeManager->getFont(theSchemeHandle);
		if(theFont)
		{
			theActionButtons->setFont(theFont);
		}
		
		theSuccess = true;
    }
	
    return theSuccess;
}

