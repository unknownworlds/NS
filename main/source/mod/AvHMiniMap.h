#ifndef AVH_MINIMAP_H
#define AVH_MINIMAP_H

#include "types.h"

#ifdef AVH_CLIENT
#include "cl_dll/hud.h"
#endif

#include "mod/AvHMapExtents.h"

const int kSpriteWidth = 512;
const int kSpriteHeight = 512;
const int kSpriteDataPixels = kSpriteWidth*kSpriteHeight;

class AvHPlayer;

class AvHMiniMap
{
public:
					AvHMiniMap();
	virtual			~AvHMiniMap();
				
	bool			GetIsProcessing(float* outPercentageDone = NULL) const;
	
	#ifdef AVH_SERVER
	void			BuildMiniMap(const char* inMapName, AvHPlayer* inPlayer, const AvHMapExtents& inMapExtents);
	bool			Process();
	#endif

	#ifdef AVH_CLIENT
	// : 1064
	// Allow the caller to specify the use of the labelled minimap
	static string	GetSpriteNameFromMap(int inSpriteWidth, const string& inMapName, int useLabels);
	int				ReceiveFromNetworkStream(void* const buffer, const int size);
	bool			WriteSpritesIfJustFinished();
	#endif

		
private:
	#ifdef AVH_CLIENT
	void			DrawEdges(uint8* inMap, int width, int height);
	void			InitializePalette();
	bool			WriteMapToSprite();
	void			WriteMapToSprite(FILE* inFileHandle);
	#endif
					
	uint8*			mMap;
	int				mMapWidth;
	int				mMapHeight;

	#ifdef AVH_SERVER
	AvHPlayer*		mPlayer;
	#endif

	#ifdef AVH_CLIENT
	uint8			mPalette[256*3];
	uint8			mSpriteData[kSpriteDataPixels];
    uint8           mCommanderSpriteData[kSpriteDataPixels / 4];
	#endif
	
	float			mMinX;
	float			mMinY;
	float			mMaxX;
	float			mMaxY;
	float			mMinViewHeight;
	float			mMaxViewHeight;
					
	int				mNumSamplesToProcess;
	int				mNumSamplesProcessed;
	bool			mIsProcessing;

	string			mMapName;

	#ifdef AVH_CLIENT
	// Sprite stuff
	byte*			byteimage;
	byte*			lbmpalette;
	int				byteimagewidth;
	int				byteimageheight;
	byte*			lumpbuffer;
	byte*			plump;
	char			spritedir[1024];
	char			spriteoutname[1024];
	int				framesmaxs[2];
	int				framecount;
	#endif

};

#endif