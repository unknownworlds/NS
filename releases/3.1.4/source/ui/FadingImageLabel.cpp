//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: FadingImageLabel.cpp $
// $Date: 2002/09/09 19:46:34 $
//
//-------------------------------------------------------------------------------
// $Log: FadingImageLabel.cpp,v $
// Revision 1.16  2002/09/09 19:46:34  Flayra
// - Changes to try to make this draw properly in software
//
// Revision 1.15  2002/08/31 18:04:32  Flayra
// - Work at VALVe
//
// Revision 1.14  2002/07/23 16:52:59  Flayra
// - Added support for multiple sprite frames (for pie nodes and max sprite problem), added document headers
//
//===============================================================================
#include "ui/FadingImageLabel.h"
#include "ui/UIUtil.h"
#include "util/STLUtil.h"

#include <assert.h>
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "common/const.h"
#include "common/com_model.h"
#include "engine/studio.h"
#include "common/entity_state.h"
#include "common/cl_entity.h"
#include "common/dlight.h"
#include "common/triangleapi.h"

#include <stdio.h>
#include <string.h>
#include <memory.h>
#include <math.h>

#include "cl_dll/studio_util.h"
#include "cl_dll/r_studioint.h"

#include "cl_dll/StudioModelRenderer.h"
#include "cl_dll/GameStudioModelRenderer.h"

extern engine_studio_api_t	IEngineStudio;


const char* kFrameIndicator = "-frame";

//FadingImageLabel::FadingImageLabel(const char* inImageName, int inX, int inY)/*, int inWidth, int inHeight)*/ : CImageLabel(inImageName, inX, inY, 64, 32), mTextImage("Default text")
//{
//    this->Init();
//}

FadingImageLabel::FadingImageLabel(int inX, int inY)/*, int inWidth, int inHeight)*/ : /*Panel(inX, inY, 64, 32),*/ mTextImage("Default text")
{
    this->Init();
	this->mImageMode = false;
}

FadingImageLabel::~FadingImageLabel()
{
}

void FadingImageLabel::DoPaint()
{
	//const float thePercentToChopOffXEdge = .1f;
	//const float thePercentToChopOffYEdge = 0.0f;

	int theX, theY;
	this->getPos(theX, theY);

	//if(!IEngineStudio.IsHardware())
	//{
	//	theX += ScreenWidth/2;
	//	theY += ScreenHeight/2;
	//}
	
	int theWidth, theHeight;
	this->getSize(theWidth, theHeight);

	//ASSERT(this->mVisibleWidth <= theWidth);
	//ASSERT(this->mVisibleHeight <= theHeight);

	if((this->mVisibleWidth <= theWidth) && (this->mVisibleHeight <= theHeight))
	{
		int r, g, b, a;
		this->getBgColor(r, g, b, a);

		float theGammaSlope = gHUD.GetGammaSlope();
		r = r/theGammaSlope;
		g = g/theGammaSlope;
		b = b/theGammaSlope;
		
		// Don't take gamma slope into account for alpha
		a = 255 - a;

		////int theXBorder = thePercentToChopOffXEdge*ScreenWidth;
		////int theYBorder = thePercentToChopOffYEdge*ScreenHeight;
		int theXBorder = (theWidth - this->mVisibleWidth)/2;
		int theYBorder = (theHeight - this->mVisibleHeight)/2;

		//vguiSimpleBox(theXBorder, theYBorder, theWidth - theXBorder*2 + theXBorder, theHeight - theYBorder*2 + theYBorder, r, g, b, a);
		
		if(!this->mImageMode)
		{
			//FillRGBA(theXBorder, theYBorder, theWidth - theXBorder*2, theHeight - theYBorder*2, r, g, b, a);

			//int theSprite = Safe_SPR_Load("sprites/marinenode.spr");
			//DrawScaledHUDSprite(theSprite, kRenderTransAdd, 1, theX, theY, theWidth, theHeight, 0);
			
			gEngfuncs.pTriAPI->RenderMode(kRenderTransAlpha);
			gEngfuncs.pTriAPI->CullFace(TRI_NONE);
			//gEngfuncs.pTriAPI->Brightness(1);
			theX = theXBorder;
			theY = theYBorder;
			theWidth = theWidth - theXBorder*2;
			theHeight = theHeight - theYBorder*2;

			// 0 = selectable, valid
			// 1 = selectable, valid, current
			// 2 = selectable, invalid
			// 3 = selectable, invalid, current
			int theFrame = 0;

			if(this->GetEnabled() && this->GetDrawHighlighted())
			{
				theFrame = 1;
			}
			else if(!this->GetEnabled())
			{
				theFrame = this->GetFadeState() ? 3 : 2;
			}

			//char theFrameNumber[64];
			//sprintf(theFrameNumber, "Frame %d", theFrame);
			//this->mTextImage.setText(theFrameNumber);

			if(!this->mSprite)
			{
				this->mSprite = Safe_SPR_Load(this->mImageName.c_str());
				this->mSpriteWidth = SPR_Width(this->mSprite, this->mSpriteFrame);
				this->mSpriteHeight = SPR_Height(this->mSprite, this->mSpriteFrame);
				ASSERT(this->mSprite > 0);
			}
			
			if(this->mSprite && gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)gEngfuncs.GetSpritePointer(this->mSprite), theFrame))
			{
				gEngfuncs.pTriAPI->Begin(TRI_TRIANGLE_STRIP);
			
				vec3_t theVertex;
			
				gEngfuncs.pTriAPI->TexCoord2f(0, 1);
				theVertex.x = theX;
				theVertex.y = theY + theHeight;
				theVertex.z = 0;
				//gEngfuncs.pTriAPI->Color4ub(255, 255, 255, a);
				gEngfuncs.pTriAPI->Vertex3fv((float*)&theVertex);
			
				gEngfuncs.pTriAPI->TexCoord2f(0, 0);
				theVertex.x = theX;
				theVertex.y = theY;
				//gEngfuncs.pTriAPI->Color4ub(255, 255, 255, a);
				gEngfuncs.pTriAPI->Vertex3fv((float*)&theVertex);
			
				gEngfuncs.pTriAPI->TexCoord2f(1, 1);
				theVertex.x = theX + theWidth;
				theVertex.y = theY + theHeight;
				//gEngfuncs.pTriAPI->Color4ub(255, 255, 255, a);
				gEngfuncs.pTriAPI->Vertex3fv((float*)&theVertex);
			
				gEngfuncs.pTriAPI->TexCoord2f(1, 0);
				theVertex.x = theX + theWidth;
				theVertex.y = theY;
				//gEngfuncs.pTriAPI->Color4ub(255, 255, 255, a);
				gEngfuncs.pTriAPI->Vertex3fv((float*)&theVertex);
			
				gEngfuncs.pTriAPI->End();
			}

			gEngfuncs.pTriAPI->RenderMode(kRenderNormal);
		}	
		else
		{
			SPR_Set(this->mSprite, a, a, a);
			
			int theSpriteWidth = SPR_Width(this->mSprite, this->mSpriteFrame);
			int theSpriteHeight = SPR_Height(this->mSprite, this->mSpriteFrame);
			
			//SPR_DrawAdditive(theFrame, (theWidth - theSpriteWidth)/2, (theHeight - theSpriteHeight)/2, NULL);
			SPR_DrawHoles(this->mSpriteFrame, (theWidth - theSpriteWidth)/2, (theHeight - theSpriteHeight)/2, NULL);
		}
	}
}

void FadingImageLabel::FadedIn()
{
    Panel::setVisible(true);
}

void FadingImageLabel::FadedOut()
{
    Panel::setVisible(false);
}

//vgui::Color FadingImageLabel::GetColor() const
//{
//    return vgui::Color(255, 255, 255, this->GetValveAlpha());
//}

void FadingImageLabel::getBgColor(int& r, int& g, int& b, int& a)
{
	Panel::getBgColor(r, g, b, a);
	a = this->GetValveAlpha();
}

void FadingImageLabel::getBgColor(Color& outColor)
{
	Panel::getBgColor(outColor);

	int r, g, b, a;
	outColor.getColor(r, g, b, a);

	a = this->GetValveAlpha();
	outColor.setColor(r, g, b, a);
}

void FadingImageLabel::getContentSize(int& wide, int& tall)
{
	if(!this->mImageMode)
	{
		this->getTextSize(wide, tall);
	}
	else
	{
		wide = this->mSpriteWidth;
		tall = this->mSpriteHeight;
	}
}

bool FadingImageLabel::GetDrawHighlighted() const
{
	return false;
}

bool FadingImageLabel::GetEnabled() const
{
	return true;
}

bool FadingImageLabel::GetFadeState() const
{
    return this->mFadeToVisibiltyState;
}

void FadingImageLabel::getTextSize(int& wide,int& tall)
{
    int theTextWidth, theTextHeight;
    this->mTextImage.getTextSize(theTextWidth, theTextHeight);

	wide = theTextWidth;
	tall = theTextHeight;

	if(this->mSpriteWidth > theTextWidth)
	{
		theTextWidth = this->mSpriteWidth;
	}
	if(this->mSpriteHeight > theTextHeight)
	{
		theTextHeight = this->mSpriteHeight;
	}
}

int FadingImageLabel::GetValveAlpha() const
{
    return this->mValveAlpha;
}

void FadingImageLabel::GetVisibleSize(int& outWidth, int& outHeight)
{
	outWidth = this->mVisibleWidth;
	outHeight = this->mVisibleHeight;
}

void FadingImageLabel::Init()
{
    // default time to fade in or out
    this->mBaseFadeTime = .3f;

    this->mTimeToFade = this->mBaseFadeTime;
    this->mTimeVisChanged = -1;
    this->mTimeScalar = 0.0f;
    this->mMaxAlpha = 1.0f;
	this->mVisibleWidth = 0;
	this->mVisibleHeight = 0;

    // this needs to be set to our visibility
    this->mFadeToVisibiltyState = false;
	this->mAlwaysDrawText = false;

    // Valve keeps alpha backwards
    this->mValveAlpha = (this->isVisible() ? 0 : 255);

	this->mImageMode = false;
	this->mSprite = 0;
	this->mSpriteWidth = this->mSpriteHeight = 0;
	this->mSpriteFrame = 0;
}

void FadingImageLabel::paint()
{
    // Calculate current translucency
    float theAlphaToUse = (this->mFadeToVisibiltyState ? this->mTimeScalar : 1.0f - this->mTimeScalar);

    // Bias in accelerated fashion for dramatic effect (maps to sin(0-pi/2) so it fades fast then slows down)
    theAlphaToUse = sin(theAlphaToUse*3.141519/2.0f);

    // For some reason Valve thinks alpha of 255 is transparent
    this->mValveAlpha = 255 - (int)(255.0f*theAlphaToUse);

	//int r, g, b, a;
	//theColor.getColor(r, g, b, a);
    //this->setFgColor(r, g, b, a);

    // Set translucency
    //if(this->m_pTGA)
    //{
    //    //this->m_pTGA->setColor(this->GetColor());
	//	vgui::Color theColor;
	//	this->getBgColor(theColor);
    //    //this->setBgColor(theColor);
    //    this->m_pTGA->setColor(theColor);
	//
	//	//int r, g, b, a;
	//	//theColor.getColor(r, g, b, a);
    //    //this->setFgColor(r, g, b, a);
    //}
    //else
    //{
    //    ASSERT(false);
    //}

	// Figure out whether we're in image mode or not

    this->DoPaint();

    // Draw text on top when faded in (check < 2 in case rounding makes it negative)
    if((this->mValveAlpha < 2 || this->mAlwaysDrawText) && (!this->mImageMode))
    {
		// Fade text out as well
		Color theColor;
		this->mTextImage.getColor(theColor);

		int r, g, b, a;
		theColor.getColor(r, g, b, a);
		//theColor.setColor(r, g, b, this->mValveAlpha);
		//this->mTextImage.setColor(theColor);
        //this->mTextImage.doPaint(this);

		int theX, theY;
		this->getPos(theX, theY);

		int theWidth, theHeight;
		this->getSize(theWidth, theHeight);
        
        const AvHFont& theFont = gHUD.GetSmallFont();
		
        float theTextX = (theWidth - theFont.GetStringWidth(mText.c_str())) / 2;
        float theTextY = (theHeight - theFont.GetStringHeight()) / 2;

        theFont.DrawString(theTextX, theTextY, mText.c_str(), r, g, b);
    
    }
}

void FadingImageLabel::paintBackground()
{
	// Do nothing, we don't draw in our bg color anymore, we only draw in FillRGBA
}

// Called
void FadingImageLabel::RecalculateTextPosition()
{
	int theLabelWidth, theLabelHeight;
	this->getSize(theLabelWidth, theLabelHeight);

    int theTextWidth, theTextHeight;
    this->mTextImage.getTextSize(theTextWidth, theTextHeight);

	// Center text in middle of label
    if((theTextWidth <= theLabelWidth) && (theTextHeight <= theLabelHeight))
    {
        this->mTextImage.setPos(theLabelWidth/2 - theTextWidth/2, theLabelHeight/2 - theTextHeight/2);
    }
    // If it can't be centered on image, align with upper left corner
    else
    {
        this->mTextImage.setPos(0, 0);
    }

	// Center image now
    //int theImageWidth = this->getImageWide();
    //int theImageHeight = this->getImageTall();
	//int theImageX = theLabelWidth/2 - theImageWidth/2;
	//int theImageY = theLabelHeight/2 - theImageHeight/2;
	//this->m_pTGA->setPos(theImageX, theImageY);
}

void FadingImageLabel::SetAlwaysDrawText(bool inState)
{
	this->mAlwaysDrawText = inState;
}

void FadingImageLabel::SetBaseFadeTime(float inSeconds)
{
    this->mBaseFadeTime = inSeconds;
}

void FadingImageLabel::SetFadeState(bool inNewState)
{
    // if new state is different
    if(this->mFadeToVisibiltyState != inNewState)
    {
        // set fade to state
        this->mFadeToVisibiltyState = inNewState;

        float theCurrentTime = ::gHUD.m_flTime;

        // set new fade to time in case a change was specified during a change
        this->mTimeToFade = min(this->mBaseFadeTime, (theCurrentTime - this->mTimeVisChanged));
        this->mMaxAlpha = min(1.0f, (theCurrentTime - this->mTimeVisChanged)/this->mBaseFadeTime);

        // remember time that it was set
        this->mTimeVisChanged = theCurrentTime;

        // if new state is visible, set vgui::Panel::setVisible(true)
        if(!this->isVisible() && inNewState)
        {
            this->FadedIn();
        }
    }
}

void FadingImageLabel::setFont(Scheme::SchemeFont inSchemeFont)
{
    this->mTextImage.setFont(inSchemeFont);
}

void FadingImageLabel::setFont(Font* inFont)
{
	ASSERT(inFont != NULL);
    this->mTextImage.setFont(inFont);

	// Sanity check
	Font* theFont = this->mTextImage.getFont();
}

void FadingImageLabel::setPos(int x,int y)
{
	// Don't set size smaller than we need for out text though!
    Panel::setPos(x, y);

    this->RecalculateTextPosition();
}

void FadingImageLabel::setSize(int wide,int tall)
{
    //int theTextWidth, theTextHeight;
    //this->mTextImage.getTextSize(theTextWidth, theTextHeight);
	//int theNewWidth = max(wide, theTextWidth);
	//int theNewHeight = max(tall, theTextHeight);
    //CImageLabel::setSize(theNewWidth, theNewHeight);

    Panel::setSize(wide, tall);
    this->RecalculateTextPosition();
}

void FadingImageLabel::SetSizeKeepCenter(int inWidth, int inHeight)
{
	int theX, theY;
	this->getPos(theX, theY);

	int theOldWidth, theOldHeight;
	this->getSize(theOldWidth, theOldHeight);

	// Calculate center of label
	int theCenterX = theX + theOldWidth/2;
	int theCenterY = theY + theOldHeight/2;

	int theXDiff = (theOldWidth - inWidth)/2;
	int theYDiff = (theOldHeight - inHeight)/2;
	this->setSize(inWidth, inHeight);

	this->setPos(theX + theXDiff, theY + theYDiff);

	// Did it work?
	int theNewX, theNewY;
	this->getPos(theNewX, theNewY);
	int theNewCenterX = theNewX + inWidth/2;
	int theNewCenterY = theNewY + inHeight/2;

	// Does the new center equal the old center?
	ASSERT(abs(theNewCenterX - theCenterX) <= 1);
	ASSERT(abs(theNewCenterY - theCenterY) <= 1);
}

void FadingImageLabel::SetSpriteName(const string& inSpriteName)
{
	this->mImageName = inSpriteName;
}

void FadingImageLabel::setText(const char* inText)
{
	//if(this->mSprite == 0)
	//{
		if(*inText == '!')
		{
			this->mImageMode = true;
			this->mTextImage.setText("");
			this->mText = "";

			// If image name has '-frame' in it, then we draw a frame out of the sprite
			string theImageName = string(inText + 1);
			size_t theIndex = theImageName.find(kFrameIndicator);
			if(theIndex != std::string::npos)
			{
				// Parse out frame number
				string theFrameString = theImageName.substr(theIndex + strlen(kFrameIndicator));
				this->mSpriteFrame = MakeIntFromString(theFrameString);

				// Set theImageName
				theImageName = theImageName.substr(0, theIndex);
			}

			this->SetSpriteName(theImageName);
		}
		else
		{
			this->mTextImage.setText(inText);
			this->mText = inText;
			this->RecalculateTextPosition();
		}
	//}
}

void FadingImageLabel::setVisible(bool inVisibility)
{
	Panel::setVisible(inVisibility);
    this->mValveAlpha = (this->isVisible() ? 0 : 255);
}

void FadingImageLabel::SetVisibleSize(int inVisWidth, int inVisHeight)
{
	int theWidth, theHeight;
	this->getSize(theWidth, theHeight);

	this->mVisibleWidth = min(inVisWidth, theWidth);
	this->mVisibleHeight = min(inVisHeight, theHeight);
}

void FadingImageLabel::setVisibleWithoutFading(bool inNewState)
{
    this->mFadeToVisibiltyState = inNewState;
    this->mTimeToFade = 0.0f;

    Panel::setVisible(inNewState);

    // TODO: Call FadedIn or FadedOut?
}

void FadingImageLabel::Update(float theCurrentTime)
{
    bool theIsVisible = this->isVisible();
    
    // if fade-to state different than our current state
    //if((theIsVisible != this->mFadeToVisibiltyState) && (this->mTimeVisChanged != -1))
    if(this->mTimeVisChanged != -1)
    {
        // set current alpha scalar depending on time since fade invoked
        this->mTimeScalar = min((theCurrentTime - this->mTimeVisChanged)/this->mTimeToFade, 1);
        
        // if fade-to time has passed and we were fading out, set vgui::Panel::setVisible(false);
        if(!this->mFadeToVisibiltyState && this->mTimeScalar >= 1.0f)
        {
            if(theIsVisible)
            {
                this->FadedOut();
            }
        }
    }
}

void FadingImageLabel::VidInit(void)
{
	// Can we unload the sprite?
	this->mSprite = 0;
	this->mSpriteWidth = this->mSpriteHeight = 0;
}


