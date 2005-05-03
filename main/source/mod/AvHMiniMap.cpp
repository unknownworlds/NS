#include "mod/AvHMiniMap.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHNetworkMessages.h"

#ifdef AVH_CLIENT
	#include "util/hl/spritegn.h"
#endif

#ifdef AVH_SERVER
	#include "mod/AvHPlayer.h"
#endif

const int kMaxScreenWidth = 1600;
const int kMaxScreenHeight = 1200;

const int kHitWorldPaletteIndex = 0;
const int kBorderPaletteIndexStart = 1;
const int kBorderPaletteIndexEnd = 2;
const int kGroundStartPaletteIndex = 3;
const int kGroundEndPaletteIndex = 254;
const int kHitNothingPaletteIndex = 255;

// Network message: 
//	0: means start processing, pass map name then num samples to process, map width, map height
//	1: means update, pass num pixels, then data
//	2: means end processing

void SafeWrite (FILE *f, void *buffer, int count)
{
	if (fwrite (buffer, 1, count, f) != (size_t)count)
	{
		ASSERT(false);
	}
}

// Init class members
AvHMiniMap::AvHMiniMap()
{
	this->mMap = NULL;

	this->mIsProcessing = false;
	this->mNumSamplesProcessed = 0;
	this->mNumSamplesToProcess = 0;

	this->mMinX = this->mMinY = this->mMaxX = this->mMaxY = 0;
	this->mMinViewHeight = this->mMaxViewHeight = 0;

	#ifdef AVH_SERVER
	this->mPlayer = NULL;
	#endif
}

AvHMiniMap::~AvHMiniMap()
{
	delete [] this->mMap;
	this->mMap = NULL;
}


bool AvHMiniMap::GetIsProcessing(float* outPercentageDone) const
{
	bool theIsProcessing = false;
	
	if(this->mIsProcessing)
	{
		if(outPercentageDone)
		{
			*outPercentageDone = (float)this->mNumSamplesProcessed/this->mNumSamplesToProcess;
		}
		theIsProcessing = true;
	}
	
	return theIsProcessing;
}




#ifdef AVH_SERVER
void AvHMiniMap::BuildMiniMap(const char* inMapName, AvHPlayer* inPlayer, const AvHMapExtents& inMapExtents)
{
	const int kNumGroundFloorColors = 249;

	this->mMapName = inMapName;
	this->mPlayer = inPlayer;

	// In case BuildMiniMap is called multiple times
	delete [] this->mMap;

	// New a hi-res version of the map (enough for the 1600 version)
	this->mMapWidth = kSpriteWidth;
	this->mMapHeight = kSpriteWidth;

	this->mNumSamplesToProcess = this->mMapWidth*this->mMapHeight;

	this->mMap = new uint8[this->mNumSamplesToProcess];
	this->mNumSamplesProcessed = 0;

	this->mMinX = inMapExtents.GetMinMapX();//inMinX;
	this->mMinY = inMapExtents.GetMinMapY();//inMinY;
	this->mMaxX = inMapExtents.GetMaxMapX();//inMaxX;
	this->mMaxY = inMapExtents.GetMaxMapY();//inMaxY;

	this->mMinViewHeight = inMapExtents.GetMinViewHeight();//inMinViewHeight;
	this->mMaxViewHeight = inMapExtents.GetMaxViewHeight();//inMaxViewHeight;
	
	this->mIsProcessing = true;

	// Tell player to rebuild minimap
	NetMsg_BuildMiniMap_Initialize( this->mPlayer->pev, this->mMapName, this->mNumSamplesToProcess, this->mMapWidth, this->mMapHeight );
}

bool AvHMiniMap::Process()
{
	bool theProcessingComplete = false;
	
	if(this->GetIsProcessing())
	{
		// Process x pixels
		// If we've calculated them all, return true
		
		// positive y on component is down, but that means negative y in world
		float theDiffY = this->mMaxY - this->mMinY;
		
		// left to right
		float theDiffX = this->mMaxX - this->mMinX;
		
		// Preserve map aspect ratio
		float theMapAspectRatio = (this->mMaxX - this->mMinX)/(this->mMaxY - this->mMinY);
		
		float theXScale, theYScale;
		if(theMapAspectRatio > 1.0f)
		{
			theXScale = 1.0f;
			theYScale = 1.0f/theMapAspectRatio;	
		}
		else
		{
			theXScale = 1.0f/theMapAspectRatio;	
			theYScale = 1.0f;
		}
		
		float theMapCenterX = (this->mMinX + this->mMaxX)/2.0f;
		float theMapCenterY = (this->mMinY + this->mMaxY)/2.0f;

		const int kNumPixelsPerCall = 50;
		uint8 theSampleArray[kNumPixelsPerCall];
		memset(theSampleArray, 0, kNumPixelsPerCall);
		
		int i=0;
		for(i = 0; (i < kNumPixelsPerCall) && (this->mNumSamplesProcessed < this->mNumSamplesToProcess); i++)
		{
			int theSampleIndex = this->mNumSamplesProcessed;
			int theX = theSampleIndex % this->mMapWidth;
			int theY = theSampleIndex/this->mMapWidth;
			
			// Initialize the value to outside the map
			int theValue = kHitNothingPaletteIndex;
			
			// Account for map center and aspect ratio
			float theXComponent = (theX/(float)this->mMapWidth) - .5f;
			float theYComponent = (theY/(float)this->mMapHeight) - .5f;
			float theCurrentX = theMapCenterX + theXComponent*theDiffX*theXScale;
			float theCurrentY = theMapCenterY - theYComponent*theDiffY*theYScale;
			
			// If the point is inside our map boundaries, do the trace
			if((theCurrentX >= this->mMinX) && (theCurrentX <= this->mMaxX) && (theCurrentY >= this->mMinY) && (theCurrentY <= this->mMaxY))
			{
				// If we hit nothing, draw with the off map index
				theValue = kHitNothingPaletteIndex;
				int theUserThree = 0;
				float theHitHeight;
				float theHeightGradient = 0.0f;
				
				if(AvHSHUTraceVerticalTangible(theCurrentX, theCurrentY, this->mMaxViewHeight, theUserThree, theHitHeight))
				{
					// TODO: Modify trace to return world brushes that are hit
					// Set color to "world brush hit", it will be changed if an entity was hit
					theValue = kHitWorldPaletteIndex;
					

					theHitHeight = min(mMaxViewHeight, max(theHitHeight, mMinViewHeight));
					theHeightGradient = 1.0f - (this->mMaxViewHeight - theHitHeight)/(this->mMaxViewHeight - this->mMinViewHeight);
					theValue = kGroundStartPaletteIndex + (kGroundEndPaletteIndex - kGroundStartPaletteIndex)*theHeightGradient;
			   
				}
			}
			
			int theIndex = theX + theY*this->mMapWidth;
			ASSERT(theIndex < this->mNumSamplesToProcess);
			this->mMap[theIndex] = theValue;

			theSampleArray[i] = theValue;
			
			this->mNumSamplesProcessed++;
		}

		// This could be less than kNumPixelsPerCall if it's the last time through
		int theNumSamples = i;

		// Tell player to rebuild minimap
		NetMsg_BuildMiniMap_Update( this->mPlayer->pev, theNumSamples, theSampleArray );
		
		if(this->mNumSamplesProcessed == this->mNumSamplesToProcess)
		{
			theProcessingComplete = true;
			this->mIsProcessing = false;

			NetMsg_BuildMiniMap_Complete( this->mPlayer->pev );
		}
	}
	
	return theProcessingComplete;
}

#endif


#ifdef AVH_CLIENT
string AvHMiniMap::GetSpriteNameFromMap(int inSpriteWidth, const string& inMapName)
{
	char theWidthString[128];
	sprintf(theWidthString, "%d", inSpriteWidth);
	
	string theMiniMapName = kMiniMapSpritesDirectory + string("/") /*+ string(theWidthString)*/ + inMapName + string(".spr");
	//string theMiniMapName = kMiniMapSpritesDirectory + string("/") + inMapName + string(".spr");
	return theMiniMapName;
}

void AvHMiniMap::InitializePalette()
{
	//	// Test data
	//	memset(this->mMap, kTransparentPaletteIndex, theNumSamples);
	//	for(int i = 0; i < this->mMapHeight; i++)
	//	{
	//		char theFillChar = i % 256;
	//		memset(this->mMap + i*this->mMapWidth, theFillChar, this->mMapWidth);
	//	}
	//	
	
	// Set colors in image to use palette
	memset(this->mPalette, 0, 256*3);

	float theGradient = 0.0f;
	
	for(int i = 0; i < 256; i++)
	{
		const int kHitWorldR = 29;
		const int kHitWorldG = 59;
		const int kHitWorldB = 121;

		const int kBorderR = 144;
		const int kBorderG = 159;
		const int kBorderB = 189;

		uint8* theColor = this->mPalette + i*3;
		
		if (i >= kGroundStartPaletteIndex && i <= kGroundEndPaletteIndex)
		{
			// Ground start to end
			
			// Set color according to height, blending to hit world color
			theGradient = (float)(i - kGroundStartPaletteIndex)/(kGroundEndPaletteIndex - kGroundStartPaletteIndex);
			theColor[0] = (int)(theGradient*kHitWorldR);
			theColor[1] = (int)(theGradient*kHitWorldG);
			theColor[2] = (int)(theGradient*kHitWorldB);

		}
		else if (i >= kBorderPaletteIndexStart && i <= kBorderPaletteIndexEnd)
		{

			theGradient = (float)(i - kBorderPaletteIndexStart)/(kBorderPaletteIndexEnd - kBorderPaletteIndexStart);
			theColor[0] = (int)(theGradient*kBorderR);
			theColor[1] = (int)(theGradient*kBorderG);
			theColor[2] = (int)(theGradient*kBorderB);

		}
		else
		{
		
			switch(i)
			{
			// On map but inaccessible
			case kHitNothingPaletteIndex:
				theColor[0] = 255;
                theColor[1] = 0;
                theColor[2] = 0;
				break;
				
			case kHitWorldPaletteIndex:
				theColor[0] = kHitWorldR;
				theColor[1] = kHitWorldG;
				theColor[2] = kHitWorldB;
				break;

			}
		}
	}
}

int AvHMiniMap::ReceiveFromNetworkStream(void* const buffer, const int size)
{
	bool finished;

	NetMsg_BuildMiniMap( buffer, size, 
		this->mMapName, 
		this->mNumSamplesToProcess, 
		this->mNumSamplesProcessed, 
		this->mMapWidth, 
		this->mMapHeight,
		&this->mMap,
		finished
	);

	this->mIsProcessing = !finished;

	return 1;
}

bool AvHMiniMap::WriteMapToSprite()
{
	bool theSuccess = false;

	if(!this->GetIsProcessing())
	{
		// Open file
		string theSpriteFileName = string(getModDirectory()) + string("/") + GetSpriteNameFromMap(0, this->mMapName);
		FILE* theFile = fopen(theSpriteFileName.c_str(), "wb");
		if(theFile)
		{
			// Clear sprite data to transparent
			memset(this->mSpriteData, 0, kSpriteDataPixels);

			// Copy data
			memcpy(this->mSpriteData, this->mMap, kSpriteWidth*kSpriteHeight);
			
			int theNumFrames = 1;
			this->WriteMapToSprite(theFile);
			
			fclose(theFile);
			
			theSuccess = true;
		}
	}
	
	return theSuccess;	
}

void AvHMiniMap::WriteMapToSprite(FILE* inFileHandle)
{

	// Compute the number for frames based on the size of the sprite.

    const int spriteWidth  = 256;
    const int spriteHeight = 256;
    
    int numXFrames = mMapWidth / spriteWidth;
    int numYFrames = mMapHeight / spriteHeight;

    // The extra frame is the commander mode version of the map.
    int numFrames = numXFrames * numYFrames + 1;

    //
	// write out the sprite header
	//
	dsprite_t spritetemp;
	spritetemp.type = SPR_SINGLE;
	spritetemp.texFormat = SPR_ALPHTEST;
	spritetemp.boundingradius = sqrt((float)kSpriteWidth*kSpriteWidth);
	spritetemp.width = spriteWidth;
	spritetemp.height = spriteHeight;
	spritetemp.numframes = numFrames;
	spritetemp.beamlength = 0;// LittleFloat (this->sprite.beamlength);
	spritetemp.synctype = ST_SYNC;
	spritetemp.version = SPRITE_VERSION;
	spritetemp.ident = IDSPRITEHEADER;
	
	SafeWrite(inFileHandle, &spritetemp, sizeof(spritetemp));
	
	short cnt = 256;
	SafeWrite(inFileHandle, (void *) &cnt, sizeof(cnt));
	SafeWrite(inFileHandle, this->mPalette, cnt*3);

    for (int y = 0; y < numYFrames; ++y)
    {
        for (int x = 0; x < numXFrames; ++x)
        {

		    spriteframetype_t theType = SPR_SINGLE;
		    SafeWrite ( inFileHandle, &theType,  sizeof(theType));
		    
		    dspriteframe_t	frametemp;
		    
		    frametemp.origin[0] = 0;
		    frametemp.origin[1] = 0;
		    frametemp.width = spriteWidth;
		    frametemp.height = spriteHeight;
		    
		    SafeWrite (inFileHandle, &frametemp, sizeof (frametemp));

            for (int i = 0; i < spriteHeight; ++i)
            {
                SafeWrite (inFileHandle, mSpriteData + (y * spriteHeight + i) * mMapWidth + x * spriteWidth, spriteWidth);
            }

        }
    }

	spriteframetype_t theType = SPR_SINGLE;
	SafeWrite ( inFileHandle, &theType,  sizeof(theType));
	
	dspriteframe_t	frametemp;
	
	frametemp.origin[0] = 0;
	frametemp.origin[1] = 0;
	frametemp.width = kSpriteWidth / 2;
	frametemp.height = kSpriteHeight / 2;
	
	SafeWrite (inFileHandle, &frametemp, sizeof (frametemp));
    SafeWrite (inFileHandle, mCommanderSpriteData, kSpriteDataPixels / 4);


}

bool AvHMiniMap::WriteSpritesIfJustFinished()
{
	bool theSuccess = false;

	if(this->GetIsProcessing() && (this->mNumSamplesProcessed == this->mNumSamplesToProcess))
	{
		this->mIsProcessing = false;

		this->InitializePalette();

        // Create the commander mode version of the sprite.

        for (int x = 0; x < kSpriteWidth / 2; ++x)
        {
            for (int y = 0; y < kSpriteHeight / 2; ++y)
            {
                mCommanderSpriteData[x + y * (kSpriteWidth / 2)] = 
                    mMap[(x * 2) + (y * 2) * kSpriteWidth];
            }
        }
        
        this->DrawEdges(mMap, kSpriteWidth, kSpriteHeight);
        this->DrawEdges(mCommanderSpriteData, kSpriteWidth / 2, kSpriteHeight / 2);

		if(this->WriteMapToSprite())
		{
			theSuccess = true;
		}

		this->mNumSamplesProcessed = this->mNumSamplesToProcess = 0;
		this->mIsProcessing = false;
	}
	
	// For each resolution
	return theSuccess;
}

void AvHMiniMap::DrawEdges(uint8* inMap, int width, int height)
{

	const int numPixels = width * height;
	uint8* newMap = new uint8[numPixels];

    memset(newMap, kHitNothingPaletteIndex, numPixels);
    
    for (int y = 1; y < width - 1; ++y)
	{
		for (int x = 1; x < height - 1; ++x)
		{

			int baseIndex = x + y * width;
			int color = inMap[baseIndex]; 

			if (color == kHitNothingPaletteIndex)
			{

				int count = 0;

				if (inMap[(x-1) + (y-1)*width] != kHitNothingPaletteIndex) ++count;
				if (inMap[(x+0) + (y-1)*width] != kHitNothingPaletteIndex) ++count;
				if (inMap[(x+1) + (y-1)*width] != kHitNothingPaletteIndex) ++count;

				if (inMap[(x-1) + (y+0)*width] != kHitNothingPaletteIndex) ++count;
				if (inMap[(x+1) + (y+0)*width] != kHitNothingPaletteIndex) ++count;

				if (inMap[(x-1) + (y+1)*width] != kHitNothingPaletteIndex) ++count;
				if (inMap[(x+0) + (y+1)*width] != kHitNothingPaletteIndex) ++count;
				if (inMap[(x+1) + (y+1)*width] != kHitNothingPaletteIndex) ++count;


				if (count > 0)
				{
					float i = pow((count / 8.0f), 0.5f);
					//color = i * (kBorderPaletteIndexEnd - kBorderPaletteIndexStart) + kBorderPaletteIndexStart;
                    color = kBorderPaletteIndexEnd;
				}


				/*
				if (mMap[(x-1) + (y-1)*mMapWidth] != kHitNothingPaletteIndex ||
					mMap[(x+0) + (y-1)*mMapWidth] != kHitNothingPaletteIndex ||
					mMap[(x+1) + (y-1)*mMapWidth] != kHitNothingPaletteIndex ||
					mMap[(x-1) + (y+0)*mMapWidth] != kHitNothingPaletteIndex ||
					mMap[(x+1) + (y+0)*mMapWidth] != kHitNothingPaletteIndex ||
					mMap[(x-1) + (y+1)*mMapWidth] != kHitNothingPaletteIndex ||
					mMap[(x+0) + (y+1)*mMapWidth] != kHitNothingPaletteIndex ||
					mMap[(x+1) + (y+1)*mMapWidth] != kHitNothingPaletteIndex)
				{
					color = kBorderPaletteIndex;
				}
				*/

			}
			
			newMap[baseIndex] = color;

		}
	}

	memcpy(inMap, newMap, numPixels);
	delete [] newMap;

}

#endif

