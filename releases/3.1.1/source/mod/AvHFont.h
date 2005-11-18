#ifndef AVH_FONT_H
#define AVH_FONT_H

#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"

class AvHFont
{

public:

    /**
     * Constructor.
     */
    AvHFont();

    /**
     * Loads the font. Returns true if the font was loaded or false if
     * otherwise.
     */
    bool Load(const char* inFileName);

    /**
     * Returns the number of pixels wide a particlar string is when
     * rendered to the screen.
     */
    int GetStringWidth(const char* inString) const;

    /**
     * Returns the number of pixels wide a particlar c is when
     * rendered to the screen.
     */
    int GetCharacterWidth(char c) const;

    /**
     * Returns the number of pixels wide a string is when rendered to
     * the screen.
     */
    int GetStringHeight() const;

    /**
     *
     */
    int DrawString(int inX, int inY, const char* inString, int r, int g, int b, int inRenderMode = kRenderTransAdd) const;

    /**
     *
     */
    int DrawStringReverse(int inX, int inY, const char* inString, int r, int g, int b, int inRenderMode = kRenderTransAdd) const;

private:

    HSPRITE mSprite;

    int mSpriteWidth;
    int mSpriteHeight;

    struct CharWidth
    {
        int a;
        int b;
        int c;
    };

    CharWidth mCharWidth[256];

};

#endif