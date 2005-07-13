//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: InvisiblePanel.h $
// $Date: 2002/08/16 02:28:55 $
//
//-------------------------------------------------------------------------------
// $Log: InvisiblePanel.h,v $
// Revision 1.2  2002/08/16 02:28:55  Flayra
// - Added document headers
//
//===============================================================================
#ifndef INVISIBLEPANEL_H
#define INVISIBLEPANEL_H

#include "vgui_Panel.h"

class InvisiblePanel : public vgui::Panel
{
public:
					InvisiblePanel();

protected:
	virtual void	paint();
	virtual void	paintBackground();

private:
};

#endif
