//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHTooltip.cpp $
// $Date: 2002/09/25 20:52:10 $
//
//-------------------------------------------------------------------------------
// $Log: AvHTooltip.cpp,v $
// Revision 1.2  2002/09/25 20:52:10  Flayra
// - Refactoring
//
// Revision 1.1  2002/08/02 21:43:47  Flayra
// - New files to control tooltips on HUD
//
//===============================================================================
#include "mod/AvHTooltip.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "ui/UIUtil.h"
#include "util/Tokenizer.h"

AvHTooltip::AvHTooltip()
{
	this->mNormScreenX = -1;
	this->mNormScreenY = -1;
	this->mCentered = false;
	this->mIgnoreFadeForLifetime = false;
	this->mLifetime = -1;

	// White
	this->mColorR = this->mColorG = this->mColorB = 255;
	this->mColorA = 255;

	// Yellow
	this->mBoldColorR = 248;
	this->mBoldColorG = 252;
	this->mBoldColorB = 0;

	// Background color defaults to black
	this->mBackgroundColorR = 0;
	this->mBackgroundColorG = 0;
	this->mBackgroundColorB = 0;
	this->mBackgroundColorA = 25;
	
	this->mNeedsRecomputing = true;
	
	this->mScreenWidth = this->mScreenHeight = 0;
	this->mScreenLineHSpacing = 0;
	this->mScreenLineVSpacing = 0;
	this->mScreenLineHeight = 0;

	this->mDrawBorder = false;

	this->mNormMaxWidth = .3f;

	this->mFadeDownSpeed = -250;
	this->mFadeUpSpeed = 500;
}

AvHTooltip::AvHTooltip(string& inText, float inNormScreenX, float inNormScreenY, bool inCentered)
{
	this->mText = inText;
	this->mNormScreenX = inNormScreenX;
	this->mNormScreenY = inNormScreenY;
	this->mCentered = inCentered;
}

AvHTooltip::~AvHTooltip()
{
}


bool AvHTooltip::ChopStringOfMaxScreenWidth(int inMaxScreenWidth, string& ioBaseString, string& outChoppedString)
{
	// Loop backwards through the string, until we get a string that fits in this screen width
	size_t theCurrentLength = ioBaseString.length();
	size_t theMaxLength = ioBaseString.length();
	bool theSuccess = false;
	
	while(!theSuccess && (theCurrentLength > 0))
	{
		string theCurrentString = ioBaseString.substr(0, theCurrentLength);
		int theCurrentStringScreenWidth = gHUD.GetHudStringWidth(theCurrentString.c_str());
		if(theCurrentStringScreenWidth <= inMaxScreenWidth)
		{
			// Look for a word to break the line
			while((theCurrentLength > 0) && !theSuccess)
			{
				char theCurrentChar = ioBaseString[theCurrentLength-1];
				if((theCurrentChar == ' ') || (theCurrentLength == theMaxLength))
				{
					outChoppedString = ioBaseString.substr(0, theCurrentLength);
					ioBaseString = ioBaseString.substr(theCurrentLength, ioBaseString.length() - theCurrentLength);
					theSuccess = true;
					break;
				}
				else
				{
					theCurrentLength--;
				}
			}
		}
		else
		{
			theCurrentLength--;
		}
	}
	
	return theSuccess;
}

void AvHTooltip::Draw()
{
	this->RecomputeIfNeccessary();

	if(this->mLocalizedText != "")
	{
		int theFillStartX = (int)(this->mNormScreenX*ScreenWidth());
		int theFillStartY = (int)(this->mNormScreenY*ScreenHeight());
		
		if(this->mCentered)
		{
			theFillStartX -= this->mScreenWidth/2;	
			theFillStartY -= this->mScreenHeight/2;
		}
		
		// Draw nice border and shaded background
		float theNormalizedAlpha = (float)this->mColorA/255;
		int theAlphaComponent = theNormalizedAlpha*this->mBackgroundColorA;
		
		FillRGBA(theFillStartX, theFillStartY, this->mScreenWidth, this->mScreenHeight, this->mBackgroundColorR, this->mBackgroundColorG, this->mBackgroundColorB, theAlphaComponent);
		vguiSimpleBox(theFillStartX, theFillStartY, theFillStartX + this->mScreenWidth, theFillStartY + this->mScreenHeight, this->mColorR, this->mColorG, this->mColorB, theAlphaComponent);
		
		// Now draw each line, non-centered, left-aligned
		int theLineNumber = 0;
		StringList::iterator theStringListIter;
		for(theStringListIter = this->mStringList.begin(); theStringListIter != this->mStringList.end(); theStringListIter++)
		{
			// If the line starts with a marker, draw it in a special color
			//string theDamageMarker(kDamageMarker);
			//if(theStringListIter->substr(0, theDamageMarker.length()) == theDamageMarker)
			//{
			//	// Draw string in yellow
			//	theR = theG = 255;
			//	theB = 25;
			//}
			
			int theBaseY = theFillStartY + this->mScreenLineVSpacing + theLineNumber*this->mScreenLineHeight;
			
			int theR = this->mColorR;
			int theG = this->mColorG;
			int theB = this->mColorB;

			// If this line is bold, draw in bold color
			string theString = theStringListIter->c_str();
			int theToolTipPreStringLength = (int)kTooltipBoldPreString.length();
			if((int)theString.length() >= theToolTipPreStringLength)
			{
				if(!strncmp(theString.c_str(), kTooltipBoldPreString.c_str(), kTooltipBoldPreString.length()))
				{
					theR = this->mBoldColorR;
					theG = this->mBoldColorG;
					theB = this->mBoldColorB;

					// Now remove prefix
					theString = theString.substr(theToolTipPreStringLength, theString.length());
				}
			}

			// Draw message (DrawHudStringCentered only centers in x)
			gHUD.DrawHudString(theFillStartX + this->mScreenLineHSpacing, theBaseY /*- this->mScreenLineHeight/2*/, ScreenWidth(), theString.c_str(), theR*theNormalizedAlpha, theG*theNormalizedAlpha, theB*theNormalizedAlpha);
			
			theLineNumber++;
		}
	}
}

void AvHTooltip::FadeText(float inTimePassed, bool inFadeDown)
{
	// Fade reticle nicely
	int theFadeSpeed = inFadeDown ? this->mFadeDownSpeed : this->mFadeUpSpeed;

	float theNewAlpha = this->mColorA + inTimePassed*theFadeSpeed;

	if(inFadeDown && this->mIgnoreFadeForLifetime)
	{
		// Don't fade until a lifetime is set
	}
	else
	{
		// Our lifetime has been set, start counting it down
		if((this->mLifetime > 0) && inFadeDown)
		{
			this->mLifetime -= inTimePassed;
		}
		else
		{
			this->SetA(max(0, min(255, theNewAlpha)));
		}
	}
}

int AvHTooltip::GetA() const
{
	return this->mColorA;
}

float AvHTooltip::GetNormalizedScreenX() const
{
	return this->mNormScreenX;
}

float AvHTooltip::GetNormalizedScreenY() const
{
	return this->mNormScreenY;
}

int	AvHTooltip::GetScreenWidth() const
{
	return this->mScreenWidth;
}

int	AvHTooltip::GetScreenHeight() const
{
	return this->mScreenHeight;
}


void AvHTooltip::RecomputeIfNeccessary()
{
	if(this->mNeedsRecomputing)
	{
		this->RecomputeTextAndDimensions();
		this->mNeedsRecomputing = false;
	}
}


void AvHTooltip::RecomputeTextAndDimensions()
{
	this->mStringList.clear();
	this->mLocalizedText = this->mText;
	
	LocalizeString(this->mText.c_str(), this->mLocalizedText);

	if(this->mLocalizedText != "")
	{
		// If localization failed (ie, it was already localized), remove the dang delimiter
		if(this->mLocalizedText[0] == '#')
		{
			this->mLocalizedText = this->mLocalizedText.substr(1, this->mLocalizedText.length() - 1);
		}

		int kMaxScreenWidth = this->mNormMaxWidth*ScreenWidth();
		
		// Build list of strings that end in newline, using mLocalizedText
		StringList theNewlines;
		Tokenizer::split(this->mLocalizedText, "\n", theNewlines);

		StringList::iterator theStringListIter;
		for(theStringListIter = theNewlines.begin(); theStringListIter != theNewlines.end(); theStringListIter++)
		{
			string theHelpString = *theStringListIter;

			// For each of these, chop them up into more lines that fit the box
			do
			{
				string theNewString;
				if(this->ChopStringOfMaxScreenWidth(kMaxScreenWidth, theHelpString, theNewString))
				{
					this->mStringList.push_back(theNewString);
				}
				else
				{
					theHelpString = "";
				}
			} 
			while(theHelpString != "");
		}
		
		// For each line, if the line contains any special markers, move them to their own lines
		this->mScreenWidth = 0;
		for(theStringListIter = this->mStringList.begin(); theStringListIter != this->mStringList.end(); theStringListIter++)
		{
			// Compute max width of all the strings, add some extra for a frame
			int theCurrentScreenWidth = gHUD.GetHudStringWidth(theStringListIter->c_str());
			this->mScreenWidth = max(this->mScreenWidth, theCurrentScreenWidth);
		}
		this->mScreenLineHSpacing = .01f*ScreenWidth();
		this->mScreenWidth += 2*this->mScreenLineHSpacing;
		
		// Compute max height needed to contain all the strings, add some extra for a frame
		this->mScreenLineVSpacing = .01f*ScreenHeight();
		this->mScreenLineHeight = gHUD.GetHudStringHeight();
		this->mScreenHeight = 2*this->mScreenLineVSpacing + ((int)this->mStringList.size()*this->mScreenLineHeight);
	}	
}

void AvHTooltip::SetCentered(bool inCentered)
{
	this->mCentered = inCentered;
}

void AvHTooltip::SetDrawBorder(bool inDrawBorder)
{
	this->mDrawBorder = inDrawBorder;
}

void AvHTooltip::SetFadeDownSpeed(int inFadeDownSpeed)
{
	this->mFadeDownSpeed = inFadeDownSpeed;
}

void AvHTooltip::SetFadeUpSpeed(int inFadeUpSpeed)
{
	this->mFadeUpSpeed = inFadeUpSpeed;
}

void AvHTooltip::SetIgnoreFadeForLifetime(bool inIgnoreFadeForLifetime)
{
	this->mIgnoreFadeForLifetime = inIgnoreFadeForLifetime;
}

void AvHTooltip::SetText(const string& inText)
{
	this->mText = inText;
	this->mNeedsRecomputing = true;
}

float AvHTooltip::GetNormalizedMaxWidth() const
{
	return this->mNormMaxWidth;
}

void AvHTooltip::SetNormalizedMaxWidth(float inNormalizedMaxWidth)
{
	this->mNormMaxWidth = inNormalizedMaxWidth;
}

void AvHTooltip::SetNormalizedScreenX(float inNormScreenX)
{
	this->mNormScreenX = inNormScreenX;
}

void AvHTooltip::SetNormalizedScreenY(float inNormScreenY)
{
	this->mNormScreenY = inNormScreenY;
}

void AvHTooltip::SetRGB(int inR, int inG, int inB)
{
	this->mColorR = inR;
	this->mColorG = inG;
	this->mColorB = inB;
}

void AvHTooltip::SetR(int inR)
{
	this->mColorR = inR;
}

void AvHTooltip::SetG(int inG)
{
	this->mColorG = inG;
}

void AvHTooltip::SetB(int inB)
{
	this->mColorB = inB;
}

void AvHTooltip::SetA(int inA)
{
	this->mColorA = inA;

	// Once fully faded in, and we're ignoring fade for lifetime, set our lifetime to expire
	if((inA >= 255) && this->mIgnoreFadeForLifetime)
	{
		// Increase lifetime with message length
		this->mLifetime = max(3.0f, this->mLocalizedText.length()/12.0f);
		this->mIgnoreFadeForLifetime = false;
	}
}

void AvHTooltip::SetBoldR(int inR)
{
	this->mBoldColorR = inR;
}

void AvHTooltip::SetBoldG(int inG)
{
	this->mBoldColorG = inG;
}

void AvHTooltip::SetBoldB(int inB)
{
	this->mBoldColorB = inB;
}

void AvHTooltip::SetBackgroundR(int inR)
{
	this->mBackgroundColorR = inR;
}

void AvHTooltip::SetBackgroundG(int inG)
{
	this->mBackgroundColorG = inG;
}

void AvHTooltip::SetBackgroundB(int inB)
{
	this->mBackgroundColorB = inB;
}

void AvHTooltip::SetBackgroundA(int inA)
{
	this->mBackgroundColorA = inA;
}

