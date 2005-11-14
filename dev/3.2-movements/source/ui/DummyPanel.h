//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: DummyPanel.h $
// $Date: 2002/08/16 02:28:55 $
//
//-------------------------------------------------------------------------------
// $Log: DummyPanel.h,v $
// Revision 1.2  2002/08/16 02:28:55  Flayra
// - Added document headers
//
//===============================================================================
#ifndef DUMMYPANEL_H
#define DUMMYPANEL_H

#include "vgui_Panel.h"
#include "cl_dll/chud.h"
#include "ui/FadingImageLabel.h"
#include "ui/PieNode.h"

class DummyPanel : public vgui::Panel
{
public:
					DummyPanel() {}

protected:
	// Evil awful hack that must be done :(
	virtual void	paint();
	virtual void	paintBackground();

private:
};

#endif
