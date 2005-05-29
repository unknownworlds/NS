#include "ui/SpritePanel.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"

const string kTopAlignment = "top";
const string kBottomAlignment = "bottom";

SpritePanel::SpritePanel(const string& inBaseSpriteName, const string& inRenderMode)
{
	this->mBaseSpriteName = inBaseSpriteName;
	ASSERT(this->mBaseSpriteName.c_str() != "");
	
	this->mRenderMode = inRenderMode;
	this->mSpriteHandle = 0;
	this->mGammaSlope = 1.0f;
}

int	SpritePanel::GetNumSpritesAcross() 
{
	ASSERT(this->mSpriteHandle);
	int theFrame = 0;
	
	int theSpriteWidth = SPR_Width(this->mSpriteHandle, theFrame);
	
	int theCompWidth;
	int theCompHeight;
	this->getSize(theCompWidth, theCompHeight);
	
	int theNumSpritesAcross = max(1, ((theCompWidth + theSpriteWidth - 1)/theSpriteWidth));
	
	return theNumSpritesAcross;
}

int	SpritePanel::GetNumSpritesDown() 
{
	ASSERT(this->mSpriteHandle);
	int theFrame = 0;
	
	int theSpriteHeight = SPR_Height(this->mSpriteHandle, theFrame);
	
	int theCompWidth;
	int theCompHeight;
	this->getSize(theCompWidth, theCompHeight);
	
	int theNumSpritesDown = max(1, ((theCompHeight + theSpriteHeight - 1)/theSpriteHeight));
	
	return theNumSpritesDown;
	
}

void SpritePanel::NotifyGammaChange(float inGammaSlope)
{
	this->mGammaSlope = inGammaSlope;
}

void SpritePanel::paint()
{
	int theScreenWidth = ScreenWidth();
	int theScreenHeight = ScreenHeight();

	if(!this->mSpriteHandle)
	{
		// Load it up
		char theSpriteName[256];
		sprintf(theSpriteName, "sprites/%d%s.spr", theScreenWidth, this->mBaseSpriteName.c_str());
		this->mSpriteHandle = Safe_SPR_Load(theSpriteName);

		// If loaded, reposition component if bottom aligned
		if(this->mSpriteHandle)
		{
			if(this->mVAlignment == kBottomAlignment)
			{
				int theX, theY;
				this->getPos(theX, theY);

				int theWidth, theHeight;
				this->getSize(theWidth, theHeight);
				
				int theSpriteHeight = SPR_Height(this->mSpriteHandle, 0);
				int theNumSpritesDown = this->GetNumSpritesDown();

				int theAdjustedHeightDiff = (theSpriteHeight*theNumSpritesDown - theHeight);
				this->setPos(theX, theY - theAdjustedHeightDiff);

				int theNewHeight = theHeight + theAdjustedHeightDiff;
				this->setSize(theWidth, theNewHeight);
			}
		}
	}
	
	if(this->mSpriteHandle)
	{
		// NOTE: Assumes that all frames are the same size
		int theFrame = 0;
		
		int theCompWidth;
		int theCompHeight;
		this->getSize(theCompWidth, theCompHeight);
		
		int theNumSpritesAcross = this->GetNumSpritesAcross();
		int theNumSpritesDown = this->GetNumSpritesDown();
		int theNumFramesToFillArea = theNumSpritesAcross*theNumSpritesDown;
		int theCurrentFrame = 0;
		
		int theNumFrames = SPR_Frames(this->mSpriteHandle);
		int theSpriteWidth = SPR_Width(this->mSpriteHandle, theFrame);
		int theSpriteHeight = SPR_Height(this->mSpriteHandle, theFrame);

		int theAlignBottomOffset = 0;
		//if(this->mVAlignment == kBottomAlignment)
		//{
		//	theAlignBottomOffset = min(0, theCompHeight - theNumSpritesDown*theSpriteHeight);
		//}

		for(int theY = 0; theY < theNumSpritesDown; theY++)
		{
			for(int theX = 0; theX < theNumSpritesAcross; theX++)
			{
				if(theCurrentFrame < theNumFrames)
				{
					int theGammaAwareColorComponent = (int)(255.0f/this->mGammaSlope);
					SPR_Set(this->mSpriteHandle, theGammaAwareColorComponent, theGammaAwareColorComponent, theGammaAwareColorComponent);
					int theFinalX = theX*theSpriteWidth;
					int theFinalY = theY*theSpriteHeight + theAlignBottomOffset;
					//if(!theAlignmentIsTop)
					//{
					//	theFinalY = theScreenHeight - (theNumSpritesDown - theY)*theSpriteHeight;
					//}

					if(this->mRenderMode == "alphatest")
					{
						SPR_DrawHoles(theCurrentFrame, theFinalX, theFinalY, NULL);
					}
					else if(this->mRenderMode == "additive")
					{
						SPR_DrawAdditive(theCurrentFrame, theFinalX, theFinalY, NULL);
					}
					else
					{
						SPR_Draw(theCurrentFrame, theFinalX, theFinalY, NULL);
					}

					theCurrentFrame++;
				}
			}
		}
	}
}

void SpritePanel::paintBackground()
{
}

void SpritePanel::SetVAlignment(const string& inAlignment)
{
	ASSERT((inAlignment == kTopAlignment) || (inAlignment == kBottomAlignment));

	this->mVAlignment = inAlignment;
}

void SpritePanel::VidInit(void)
{
	this->mSpriteHandle = 0;
}
