//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHTooltip.h $
// $Date: 2002/09/25 20:52:10 $
//
//-------------------------------------------------------------------------------
// $Log: AvHTooltip.h,v $
// Revision 1.2  2002/09/25 20:52:10  Flayra
// - Refactoring
//
// Revision 1.1  2002/08/02 21:43:47  Flayra
// - New files to control tooltips on HUD
//
//===============================================================================
#ifndef AVH_TOOLTIP_H
#define AVH_TOOLTIP_H

#include "types.h"

const string kTooltipBoldPreString = "<b>";

class AvHTooltip
{
public:
				AvHTooltip();
				AvHTooltip(string& inText, float inNormScreenX, float inNormScreenY, bool inCentered = false);

	virtual		~AvHTooltip();
				
	void		Draw();
				
	void		FadeText(float inTimePassed, bool inFadeDown);

	int			GetA() const;
	
	float		GetNormalizedScreenX() const;
	float		GetNormalizedScreenY() const;
	
	int			GetScreenWidth() const;
	int			GetScreenHeight() const;

	void		RecomputeIfNeccessary();

	void		SetCentered(bool inCentered);
	void		SetDrawBorder(bool inDrawBorder);
	void		SetFadeDownSpeed(int inFadeDownSpeed);
	void		SetFadeUpSpeed(int inFadeUpSpeed);
	void		SetIgnoreFadeForLifetime(bool inIgnoreFadeForLifetime);
	void		SetText(const string& inText);

	float		GetNormalizedMaxWidth() const;
	void		SetNormalizedMaxWidth(float inNormalizedMaxWidth);
				
	void		SetNormalizedScreenX(float inNormScreenX);
	void		SetNormalizedScreenY(float inNormScreenY);
				
	void		SetRGB(int inR, int inG, int inB);
	
	void		SetR(int inR);
	void		SetG(int inG);
	void		SetB(int inB);
	void		SetA(int inA);

	void		SetBackgroundR(int inR);
	void		SetBackgroundG(int inG);
	void		SetBackgroundB(int inB);
	void		SetBackgroundA(int inA);

	void		SetBoldR(int inR);
	void		SetBoldG(int inG);
	void		SetBoldB(int inB);
				
private:		
	bool		ChopStringOfMaxScreenWidth(int inMaxScreenWidth, string& ioBaseString, string& outChoppedString);
	void		RecomputeTextAndDimensions();

	// TODO: Add more info here to indicate what to point out in the world (either an entity, or a position)

	string		mText;
	string		mLocalizedText;
	StringList	mStringList;

	bool		mDrawBorder;

	int			mColorR;
	int			mColorG;
	int			mColorB;
	int			mColorA;

	int			mBoldColorR;
	int			mBoldColorG;
	int			mBoldColorB;

	int			mBackgroundColorR;
	int			mBackgroundColorG;
	int			mBackgroundColorB;
	int			mBackgroundColorA;

	bool		mIgnoreFadeForLifetime;
	float		mLifetime;
	bool		mCentered;

	bool		mNeedsRecomputing;
	
	float		mNormScreenX;
	float		mNormScreenY;

	float		mNormMaxWidth;

	int			mScreenWidth;
	int			mScreenHeight;
	int			mScreenLineHSpacing;
	int			mScreenLineVSpacing;
	int			mScreenLineHeight;

	int			mFadeDownSpeed;
	int			mFadeUpSpeed;

};

#endif