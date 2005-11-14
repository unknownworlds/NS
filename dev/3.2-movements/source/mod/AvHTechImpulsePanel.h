//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: TechImpulse.h $
// $Date: 2002/08/16 02:28:55 $
//
//-------------------------------------------------------------------------------
// $Log: TechImpulse.h,v $
// Revision 1.3  2002/08/16 02:28:55  Flayra
// - Added document headers
//
//===============================================================================
#ifndef AVHTECHIMPULSEPANEL_H
#define AVHTECHIMPULSEPANEL_H

#include "vgui_Label.h"
#include "mod/AvHMessage.h"
#include "ui/UIComponents.h"
#include "VGUI_TextImage.h"
#include "ui/FakeTextImage.h"

class AvHTechImpulsePanel : public vgui::Label
{
public:
					AvHTechImpulsePanel();
					AvHTechImpulsePanel(int inXPos, int inYPos, int inWidth, int inHeight);

	AvHMessageID	GetMessageID() const;

	void			SetDrawNumberRange(int inRange);
	void			SetMessageID(AvHMessageID inImpulse);
	void			SetVisualNumber(int inNumber);

	virtual void	setFont(Scheme::SchemeFont schemeFont);
	virtual void	setFont(Font* font);
					
protected:
	bool			GetTextExtents(int& outWidth, int& outHeight);
	virtual void	paint();
	virtual void	paintBackground();

private:
	AvHMessageID	mMessageID;
	int				mVisualNumber;
    FakeTextImage   mTextImage;
	int				mDrawNumberRange;
};

class AvHUITechImpulsePanel : public UILabel
{
public:
	virtual Panel*			GetComponentPointer(void);
	
    virtual bool			SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager);
	
private:
	
	void                    AllocateComponent(const TRDescription& inDescription);
	
    AvHTechImpulsePanel*	mComponent;
};


#endif
