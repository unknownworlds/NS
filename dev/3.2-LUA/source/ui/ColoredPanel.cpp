//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: ColoredPanel.cpp $
// $Date: 2002/08/16 02:28:55 $
//
//-------------------------------------------------------------------------------
// $Log: ColoredPanel.cpp,v $
// Revision 1.3  2002/08/16 02:28:55  Flayra
// - Added document headers
//
//===============================================================================
#include "ui/ColoredPanel.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"

ColoredPanel::ColoredPanel() : Panel()
{
}

ColoredPanel::ColoredPanel(int inXPos, int inYPos, int inWidth, int inHeight) : Panel(inXPos, inYPos, inWidth, inHeight)
{
}

void ColoredPanel::paint()
{
	Panel::paint();
}

void ColoredPanel::paintBackground()
{
	Panel::paintBackground();

	int r, g, b, a;
	this->getBgColor(r, g, b, a);

	int theXPos, theYPos;
	this->getPos(theXPos, theYPos);

	int theWidth, theHeight;
	this->getSize(theWidth, theHeight);

	//FillRGBA(0, 0, theWidth, theHeight, r, g, b, a);

	float theOptimalIncrement = (float)255/theHeight;
	int theStepSize = max(theOptimalIncrement, 1);
	a = 0;
	for(int i = 0; i < theHeight; i++)
	{
		// Fill lines across, getting darker
		FillRGBA(0, i, theWidth, 1, r, g, b, a);
		a = min(a+theStepSize, 255);
	}
}

