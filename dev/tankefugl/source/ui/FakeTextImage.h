//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: FakeTextLabel.h $
// $Date: $
//
//-------------------------------------------------------------------------------
// $Log: $
//===============================================================================
#ifndef FAKETEXTIMAGE_H
#define FAKETEXTIMAGE_H

#include "VGUI_TextImage.h"

class FakeTextImage : public TextImage
{
public:
	FakeTextImage(const char* text) : TextImage(text) {}

	virtual void  setFont(vgui::Scheme::SchemeFont schemeFont)
	{
		TextImage::setFont(schemeFont);
	}

	virtual void  setFont(vgui::Font* font)
	{
		ASSERT(font != NULL);
		TextImage::setFont(font);
	}
};

#endif