#include "mod/AvHFont.h"
#include "mod/AvHSpriteAPI.h"
#include "mod/AvHConstants.h"

#include "cl_dll/cl_util.h"

// Number of characters per row in the font sprite.
const int kNumCharsPerRow = 16;

AvHFont::AvHFont()
{
    mSprite = 0;
    mSpriteWidth = 0;
    mSpriteHeight = 0;
}

bool AvHFont::Load(const char* inFileName)
{
    
    std::string theSpriteFileName;
    theSpriteFileName  = inFileName;
    theSpriteFileName += ".spr";

    std::string theWidthFileName;
    theWidthFileName  = getModDirectory();
    theWidthFileName += "/";
    theWidthFileName += inFileName;
    theWidthFileName += ".dat";
    
    FILE* file = fopen(theWidthFileName.c_str(), "rb");

    if (file != NULL)
    {

        struct LABC
        {
            long a;
            long b;
            long c;
        };

        LABC labc[256];
        fread(labc, sizeof(LABC), 256, file);

        for (int i = 0; i < 256; ++i)
        {
            mCharWidth[i].a = labc[i].a;
            mCharWidth[i].b = labc[i].b;
            mCharWidth[i].c = labc[i].c;
        }
        
        fclose(file);
    }
    else
    {
        return false;
    }

    mSprite = Safe_SPR_Load(theSpriteFileName.c_str());

    if (mSprite != 0)
    {
        mSpriteWidth  = SPR_Width(mSprite, 0);
        mSpriteHeight = SPR_Height(mSprite, 0);
    }
    
    return mSprite != 0;

}

int AvHFont::GetStringWidth(const char* inString) const
{

    int theWidth = 0;

    for (int i = 0; inString[i] != 0 && inString[i] != '\n'; ++i)
    {
        char c = inString[i];
        
        if (c < 32)
        {
            // Unprintable.
            continue;
        }

        theWidth += mCharWidth[c].a + mCharWidth[c].b + mCharWidth[c].c;
    }

    return theWidth;

}

int AvHFont::GetCharacterWidth(char c) const
{
    return mCharWidth[c].a + mCharWidth[c].b + mCharWidth[c].c;
}

int AvHFont::GetStringHeight() const
{
    return 16;
}

int AvHFont::DrawString(int inX, int inY, const char* inString, int r, int g, int b, int inRenderMode) const
{

    int theX = inX;
    int theY = inY;
        
    int theCharHeight = GetStringHeight();

    AvHSpriteBeginFrame();

    AvHSpriteSetVGUIOffset(0,0);

    AvHSpriteSetRenderMode(inRenderMode);
    AvHSpriteSetColor(r / 256.0f, g / 256.0f, b / 256.0f);

    int charWidth  = mSpriteWidth / 16;
    int charHeight = mSpriteHeight / 16;

    for (int i = 0; inString[i] != 0 && inString[i] != '\n'; ++i)
    {

        unsigned char c = inString[i];

        if (c < 32)
        {
            // Unprintable.
            continue;
        }

        theX += mCharWidth[c].a;
        
        float theU = ((c % kNumCharsPerRow) * charWidth) / float(mSpriteWidth);
        float theV = ((c / kNumCharsPerRow) * charHeight) / float(mSpriteHeight);

        AvHSpriteDraw(mSprite, 0, theX, theY, theX + mCharWidth[c].b, theY + theCharHeight,
            theU, theV, theU + mCharWidth[c].b / 256.0f, theV + theCharHeight / 256.0f);
        
        theX += mCharWidth[c].b + mCharWidth[c].c;

    }
    
    AvHSpriteEndFrame();
    AvHSpriteSetColor(1, 1, 1);

    return theX;

}

int AvHFont::DrawStringReverse(int inX, int inY, const char* inString, int r, int g, int b, int inRenderMode) const
{

    int length = (int)strlen(inString) - 1;

    int theX = inX;
    int theY = inY;
        
    int theCharHeight = GetStringHeight();

    AvHSpriteBeginFrame();
    AvHSpriteSetRenderMode(inRenderMode);
    AvHSpriteSetColor(r / 256.0f, g / 256.0f, b / 256.0f);

    int charWidth  = mSpriteWidth / 16;
    int charHeight = mSpriteHeight / 16;

    for (int i = length; i >= 0; --i)
    {
        
        char c = inString[i];

        if (c < 32)
        {
            // Unprintable.
            continue;
        }

        theX -= mCharWidth[c].c + mCharWidth[c].b;
        
        float theU = ((c % kNumCharsPerRow) * charWidth) / float(mSpriteWidth);
        float theV = ((c / kNumCharsPerRow) * charHeight) / float(mSpriteHeight);

        AvHSpriteDraw(mSprite, 0, theX, theY, theX + mCharWidth[c].b, theY + theCharHeight,
            theU, theV, theU + mCharWidth[c].b / 256.0f, theV + theCharHeight / 256.0f);

        theX -= mCharWidth[c].a;

    }

    AvHSpriteEndFrame();

    return theX;

}