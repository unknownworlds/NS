//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: MarqueeComponent.cpp $
// $Date: 2002/08/16 02:28:55 $
//
//-------------------------------------------------------------------------------
// $Log: MarqueeComponent.cpp,v $
// Revision 1.6  2002/08/16 02:28:55  Flayra
// - Added document headers
//
//===============================================================================
#include "ui/MarqueeComponent.h"
#include "ui/UIUtil.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"

MarqueeComponent::MarqueeComponent()
{
	this->mX0 = this->mY0 = 0;
	this->mX1 = this->mY1 = 0;
	this->mGammaSlope = 1.0f;

	// Default color scheme
	this->setFgColor(0, 255, 0, 0);
	this->setBgColor(0, 255, 0, 230);
}

void MarqueeComponent::NotifyGammaChange(float inGammaSlope)
{
	this->mGammaSlope = inGammaSlope;
}

void MarqueeComponent::paint()
{
	// Draw outline
	int r, g, b, a;
	this->getFgColor(r, g, b, a);
	
	int theXPos, theYPos;
	this->getPos(theXPos, theYPos);
	
	int theWidth, theHeight;
	this->getSize(theWidth, theHeight);

	//vguiSimpleBox(0, 0, theWidth, theHeight, r/this->mGammaSlope, g/this->mGammaSlope, b/this->mGammaSlope, a/this->mGammaSlope);
	vguiSimpleBox(0, 0, theWidth, theHeight, r/this->mGammaSlope, g/this->mGammaSlope, b/this->mGammaSlope, a);

	// Top
//	vguiSimpleLine(0, 0, theWidth, 0, r, g, b, a);
//
//	// Left
//	vguiSimpleLine(0, 1, 0, theHeight, r, g, b, a);
//
//	// Right
//	vguiSimpleLine(theWidth, 1, theWidth, theHeight, r, g, b, a);
//
//	// Bottom
//	vguiSimpleLine(1, theHeight, theWidth, theHeight, r, g, b, a);
}

void MarqueeComponent::paintBackground()
{
	int r, g, b, a;
	this->getBgColor(r, g, b, a);

	int theXPos, theYPos;
	this->getPos(theXPos, theYPos);

	int theWidth, theHeight;
	this->getSize(theWidth, theHeight);

	//FillRGBA(0, 0, theWidth, theHeight, r/this->mGammaSlope, g/this->mGammaSlope, b/this->mGammaSlope, a/this->mGammaSlope);
	FillRGBA(0, 0, theWidth, theHeight, r/this->mGammaSlope, g/this->mGammaSlope, b/this->mGammaSlope, a);
}

void MarqueeComponent::ResetDimensions()
{
	int theMinX = min(this->mX0, this->mX1);
	int theMinY = min(this->mY0, this->mY1);

	int theMaxX = max(this->mX0, this->mX1);
	int theMaxY = max(this->mY0, this->mY1);

	int theWidth = theMaxX - theMinX;
	int theHeight = theMaxY - theMinY;

	ASSERT(theWidth >= 0);
	ASSERT(theHeight >= 0);

	this->setPos(theMinX, theMinY);
	this->setSize(theWidth, theHeight);
}

void MarqueeComponent::SetStartPos(int inX, int inY)
{
	this->mX0 = inX;
	this->mY0 = inY;
	this->ResetDimensions();
}

void MarqueeComponent::SetEndPos(int inX, int inY)
{
	this->mX1 = inX;
	this->mY1 = inY;
	this->ResetDimensions();
}

