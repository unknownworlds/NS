//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: LabelButton.h $
// $Date: 2002/08/16 02:28:55 $
//
//-------------------------------------------------------------------------------
// $Log: LabelButton.h,v $
// Revision 1.2  2002/08/16 02:28:55  Flayra
// - Added document headers
//
//===============================================================================
#ifndef LABELBUTTON_H
#define LABELBUTTON_H

#include "vgui_Label.h"

class LabelButton : public vgui::Label
{
public:
					LabelButton(const char* text,int x,int y,int wide,int tall) : mOriginalWidth(wide), mOriginalHeight(tall), Label(text, x, y, wide, tall)
					{
					}
	
	virtual void	setSize(int wide,int tall)
					{
						Label::setSize(this->mOriginalWidth, this->mOriginalHeight);
					}

private:
	int				mOriginalWidth;
	int				mOriginalHeight;
};

#endif
