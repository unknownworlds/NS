//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: ColoredPanel.h $
// $Date: 2002/08/16 02:28:55 $
//
//-------------------------------------------------------------------------------
// $Log: ColoredPanel.h,v $
// Revision 1.3  2002/08/16 02:28:55  Flayra
// - Added document headers
//
//===============================================================================
#ifndef COLOREDPANEL_H
#define COLOREDPANEL_H

#include "vgui_Panel.h"
#include "vgui_Label.h"

class ColoredPanel : public vgui::Panel //public vgui::Label
{
public:
					ColoredPanel();
					ColoredPanel(int inXPos, int inYPos, int inWidth, int inHeight);
					
protected:
	virtual void	paint();
	virtual void	paintBackground();

private:
};

#endif
