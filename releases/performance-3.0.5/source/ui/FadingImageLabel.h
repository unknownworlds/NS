//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: FadingImageLabel.h $
// $Date: 2002/08/31 18:04:32 $
//
//-------------------------------------------------------------------------------
// $Log: FadingImageLabel.h,v $
// Revision 1.12  2002/08/31 18:04:32  Flayra
// - Work at VALVe
//
// Revision 1.11  2002/07/23 16:52:59  Flayra
// - Added support for multiple sprite frames (for pie nodes and max sprite problem), added document headers
//
//===============================================================================
#ifndef FADINGIMAGELABEL_H
#define FADINGIMAGELABEL_H

#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "cl_dll/vgui_TeamFortressViewport.h"
#include "ui/ReloadableComponent.h"
#include "ui/FakeTextImage.h"

//class FadingImageLabel : public CImageLabel
class FadingImageLabel : public Panel, public ReloadableComponent
{
public:
	// It's not an image label anymore until Valve fixes the setColor bug
	//FadingImageLabel(const char* inImageName, int inX, int inY);//, int inWidth, int inHeight);
	FadingImageLabel(int inX, int inY);//, int inWidth, int inHeight);

    virtual ~FadingImageLabel();

    virtual void FadedIn();

    virtual void FadedOut();

    //virtual vgui::Color GetColor() const;

	virtual void getContentSize(int& wide,int& tall);

	virtual bool GetDrawHighlighted() const;
	
    virtual bool GetEnabled() const;

    virtual bool GetFadeState() const;

	virtual void getBgColor(int& r,int& g,int& b,int& a);

	virtual void getBgColor(Color& color);

	virtual void getTextSize(int& wide,int& tall);

	virtual void GetVisibleSize(int& outWidth, int& outHeight);

	void SetAlwaysDrawText(bool inState);

    void SetBaseFadeTime(float inSeconds);

    virtual void SetFadeState(bool inState);

	virtual void SetSpriteName(const string& inSpriteName);

	virtual void setFont(Scheme::SchemeFont schemeFont);

	virtual void setFont(Font* font);

	virtual void setPos(int x,int y);

	virtual void setSize(int wide,int tall);

	virtual void SetSizeKeepCenter(int wide, int tall);

	virtual void setText(const char* text);

	virtual void setVisible(bool inVisibility);

	virtual void SetVisibleSize(int inVisWidth, int inVisHeight);

    void setVisibleWithoutFading(bool state);

    virtual void Update(float theCurrentTime);

	virtual void VidInit(void);

protected:

    virtual void DoPaint();

    virtual int GetValveAlpha() const;

	virtual void paint();

	virtual void paintBackground();

    virtual void RecalculateTextPosition();

private:
    void Init();

    FakeTextImage   mTextImage;
	string			mImageName;
	string			mText;
	bool			mImageMode;
	int				mSprite;
	int				mSpriteWidth;
	int				mSpriteHeight;

    float           mMaxAlpha;
    float           mBaseFadeTime;
    float           mTimeVisChanged;
    float           mTimeToFade;
    float           mTimeScalar;
    int             mValveAlpha;
    bool            mFadeToVisibiltyState;
	bool			mAlwaysDrawText;
	int				mVisibleWidth;
	int				mVisibleHeight;
	int				mSpriteFrame;
};

#endif