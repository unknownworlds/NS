//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: ButtonChain.h $
// $Date: 2002/08/16 02:28:55 $
//
//-------------------------------------------------------------------------------
// $Log: ButtonChain.h,v $
// Revision 1.2  2002/08/16 02:28:55  Flayra
// - Added document headers
//
//===============================================================================
#ifndef ButtonChain_H
#define ButtonChain_H

#include "vgui_Panel.h"

class ButtonChain : public vgui::Panel
{
public:
					ButtonChain(int inButtonWidth, int inButtonHeight, int inNumButtons);

protected:
	virtual void	paint();
	virtual void	paintBackground();

private:
	int				mNumButtons;

};

#endif
