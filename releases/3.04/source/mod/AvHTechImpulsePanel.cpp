//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHTechImpulsePanel.cpp $
// $Date: 2002/08/16 02:28:55 $
//
//-------------------------------------------------------------------------------
// $Log: AvHTechImpulsePanel.cpp,v $
// Revision 1.3  2002/08/16 02:28:55  Flayra
// - Added document headers
//
//===============================================================================
#include "mod/AvHTechImpulsePanel.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "ui/UITags.h"

AvHTechImpulsePanel::AvHTechImpulsePanel() : Label(), mTextImage("")
{
	this->mMessageID = MESSAGE_NULL;
	this->mVisualNumber = -1;
	this->mDrawNumberRange = -1;
}

AvHTechImpulsePanel::AvHTechImpulsePanel(int inXPos, int inYPos, int inWidth, int inHeight) : Label(" ", inXPos, inYPos, inWidth, inHeight), mTextImage("")
{
}

AvHMessageID AvHTechImpulsePanel::GetMessageID() const
{
	return this->mMessageID;
}

void AvHTechImpulsePanel::SetDrawNumberRange(int inRange)
{
	this->mDrawNumberRange = inRange;
}

void AvHTechImpulsePanel::SetMessageID(AvHMessageID inImpulse)
{
	this->mMessageID = inImpulse;
}

void AvHTechImpulsePanel::SetVisualNumber(int inNumber)
{
	this->mVisualNumber = inNumber;
}


void AvHTechImpulsePanel::paint()
{
	//Label::paint();

	if((this->mDrawNumberRange >= 0) && (this->mVisualNumber >= this->mDrawNumberRange))
	{
		int thePosX, thePosY;
		this->getPos(thePosX, thePosY);
		
		int theWidth, theHeight;
		this->getSize(theWidth, theHeight);
		
		int theFlags = (this->mVisualNumber < 10) ? 0 : DHN_2DIGITS;

		char theNumberString[5];
		sprintf(theNumberString, "%d", this->mVisualNumber);
		int theLength = strlen(theNumberString);
		
		int theTextWidth, theTextHeight;
		this->GetTextExtents(theTextWidth, theTextHeight);

		this->drawSetTextColor(255, 255, 255, 0);

		this->drawPrintText(theWidth - theTextWidth, theHeight - theTextHeight, theNumberString, theLength);
	}
}

bool AvHTechImpulsePanel::GetTextExtents(int& outWidth, int& outHeight)
{
	bool theSuccess = false;

	if((this->mDrawNumberRange >= 0) && (this->mVisualNumber >= this->mDrawNumberRange))
	{
		int theFlags = (this->mVisualNumber < 10) ? 0 : DHN_2DIGITS;
		
		char theNumberString[5];
		sprintf(theNumberString, "%d", this->mVisualNumber);
		
		this->mTextImage.setText(theNumberString);
		this->mTextImage.getTextSize(outWidth, outHeight);
		
		theSuccess = true;
	}

	return theSuccess;
}

void AvHTechImpulsePanel::setFont(Scheme::SchemeFont schemeFont)
{
	Label::setFont(schemeFont);
	this->mTextImage.setFont(schemeFont);
}

void AvHTechImpulsePanel::setFont(Font* font)
{
	Label::setFont(font);
	this->mTextImage.setFont(font);
}

void AvHTechImpulsePanel::paintBackground()
{
	int theTextWidth, theTextHeight;
	if(this->GetTextExtents(theTextWidth, theTextHeight))
	{
		int theXPos, theYPos;
		this->getPos(theXPos, theYPos);
		
		int theWidth, theHeight;
		this->getSize(theWidth, theHeight);

		ASSERT(theWidth >= theTextWidth);
		ASSERT(theHeight >= theTextHeight);

		FillRGBA(0 + (theWidth - theTextWidth), 0 - (theHeight - theTextHeight), theTextWidth, theTextHeight, 0, 0, 0, 128);
		//FillRGBA(0, 0, theWidth, theHeight, 128, 128, 128, 128);
	}

//	Panel::paintBackground();
//
//	int r, g, b, a;
//	this->getBgColor(r, g, b, a);
//
//	int theXPos, theYPos;
//	this->getPos(theXPos, theYPos);
//
//	int theWidth, theHeight;
//	this->getSize(theWidth, theHeight);
//
//	//FillRGBA(0, 0, theWidth, theHeight, r, g, b, a);
//
//	float theOptimalIncrement = (float)255/theHeight;
//	int theStepSize = max(theOptimalIncrement, 1);
//	a = 0;
//	for(int i = 0; i < theHeight; i++)
//	{
//		// Fill lines across, getting darker
//		FillRGBA(0, i, theWidth, 1, r, g, b, a);
//		a = min(a+theStepSize, 255);
//	}
}


//////////////////////////////////////
// AvHUITechImpulsePanel -> UIPanel //
//////////////////////////////////////
void AvHUITechImpulsePanel::AllocateComponent(const TRDescription& inDesc)
{
    // Position (normalized screen coords)
    float theXPos = UIDefaultXPos;
    float theYPos = UIDefaultYPos;
    inDesc.GetTagValue(UITagXPos, theXPos);
    inDesc.GetTagValue(UITagYPos, theYPos);
	
    // Width and height (normalized screen coords)
    float theWidth = UIDefaultWidth;
    float theHeight = UIDefaultHeight;
    inDesc.GetTagValue(UITagWidth, theWidth);
    inDesc.GetTagValue(UITagHeight, theHeight);
	
	this->mComponent = new AvHTechImpulsePanel(theXPos*ScreenWidth(), theYPos*ScreenHeight(), theWidth*ScreenWidth(), theHeight*ScreenHeight());
}

Panel* AvHUITechImpulsePanel::GetComponentPointer(void)
{
	return this->mComponent;
}

bool AvHUITechImpulsePanel::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    AvHTechImpulsePanel* theComponent = (AvHTechImpulsePanel*)inPanel;
	
    bool theSuccess = UILabel::SetClassProperties(inDesc, inPanel, inSchemeManager);
    
    if(theSuccess)
    {
		int theImpulse = 0;
		if(inDesc.GetTagValue("impulse", theImpulse))
		{
			theComponent->SetMessageID(AvHMessageID(theImpulse));
		}

		int theDrawNumber = 0;
		if(inDesc.GetTagValue("drawnumber", theDrawNumber))
		{
			theComponent->SetDrawNumberRange(theDrawNumber);
		}

		theSuccess = true;
	}
	
	return theSuccess;
}






















