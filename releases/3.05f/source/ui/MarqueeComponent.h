//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: MarqueeComponent.h $
// $Date: 2002/08/16 02:28:55 $
//
//-------------------------------------------------------------------------------
// $Log: MarqueeComponent.h,v $
// Revision 1.3  2002/08/16 02:28:55  Flayra
// - Added document headers
//
//===============================================================================
#ifndef MARQUEECOMPONENT_H
#define MARQUEECOMPONENT_H

#include "vgui_Panel.h"
#include "ui/GammaAwareComponent.h"

class MarqueeComponent : public vgui::Panel, public GammaAwareComponent
{
public:
					MarqueeComponent();
	virtual void	NotifyGammaChange(float inGammaSlope);
	void			SetStartPos(int inX, int inY);
	void			SetEndPos(int inX, int inY);
	
protected:
	virtual void	paint();
	virtual void	paintBackground();

private:
	void			ResetDimensions();
	int				mX0, mY0;
	int				mX1, mY1;

	float			mGammaSlope;
};

#endif