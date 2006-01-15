//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: PieMenu.cpp $
// $Date: 2002/08/31 18:04:32 $
//
//-------------------------------------------------------------------------------
// $Log: PieMenu.cpp,v $
// Revision 1.13  2002/08/31 18:04:32  Flayra
// - Work at VALVe
//
// Revision 1.12  2002/08/16 02:29:19  Flayra
// - Added document headers
// - Started to add support for pie nodes with both images and text
//
//===============================================================================
#include "util/nowarnings.h"
#include "ui/PieMenu.h"
#include "ui/PieNode.h"
#include "cl_dll/cl_util.h"
#include "cl_dll/vgui_int.h"
using vgui::Panel;

const string PieMenuType("PieMenu");

PieMenu::PieMenu(const string& inRootName, int x,int y, int wide, int tall) : vgui::Panel(x, y, wide, tall)
{
	// Hard-code node distance to something "reasonable", hopefully it will be specified in ui.txt
    this->mNodeList = new PieNodeList(inRootName, .18f);
    this->mNodeList->GetRoot()->setParent(this);
    this->addChild(this->mNodeList->GetRoot());
    this->mFont = NULL;
}

PieMenu::~PieMenu()
{
    delete this->mNodeList;
}

void PieMenu::AddInputSignalForNodes(InputSignal* s)
{
    Panel::addInputSignal(s);

    this->mNodeList->AddInputSignalForNodes(s);
}

bool PieMenu::AddNode(const string& inNodeString)
{
    bool theSuccess = true;

    // Build new node using passed in string, use this->mPieNodeTarga as the image name
    // Tell node that this pie menu is its parent
    // Register pie menu input signal with it
    // Add it to list

	Color theFGColor;
	this->getFgColor(theFGColor);

	Color theBGColor;
	this->getBgColor(theBGColor);

    this->mNodeList->AddNode(inNodeString, this->mFont, theBGColor, theFGColor, this->mDefaultImage);

    return theSuccess;
}

void PieMenu::ChangeNode(int inMessageID, int inNewMessageID, const string& inNewText)
{
	this->mNodeList->ChangeNode(inMessageID, inNewMessageID, inNewText);
}

void PieMenu::DisableNodesWhoseChildrenAreDisabled()
{
	this->mNodeList->DisableNodesWhoseChildrenAreDisabled();
}

PieNode* PieMenu::GetRootNode(void)
{
    return this->mNodeList->GetRoot();
}

bool PieMenu::GetSelectedNode(PieNode*& outNode)
{
	return this->mNodeList->GetSelectedNode(outNode);
}

void PieMenu::mouseReleased(MouseCode code, Panel* panel)
{
    // TODO:
    // Cast panel to a PieNode
    // Get the pie node name
    // Trigger an event from it
}

// don't draw anything, nodes will draw instead
//void PieMenu::paint()
//{
//    Panel::paint();
//
//}

void PieMenu::paintBackground()
{
//    Panel::paintBackground();
}

void PieMenu::RecomputeVisibleSize(void)
{
	this->mNodeList->RecomputeVisibleSize();
    
	//// Recalculate base position, because root node probably just changed size to the
	//// size of the biggest node we have
    //int thePieX, thePieY;
    //this->getPos(thePieX, thePieY);
	//
	//int thePieWidth, thePieHeight;
    //this->getSize(thePieWidth, thePieHeight);
	//
    //this->mNodeList->SetBasePosition(thePieX, thePieY, thePieWidth, thePieHeight);
}

void PieMenu::ResetToDefaults()
{
	this->mNodeList->ResetToDefaults();
}

void PieMenu::NotifyGammaChange(float inGammaSlope)
{
	// TODO: Adjust font color?
	this->mNodeList->GetRoot()->SetColorBias(1.0f/inGammaSlope);
}

void PieMenu::SetConnectorName(const string& inConnectorName)
{
	this->mNodeList->SetConnectorName(inConnectorName);
}

void PieMenu::SetConstructionComplete()
{
    this->mNodeList->SetConstructionComplete();
}

void PieMenu::SetDefaultImage(const string& inDefaultImage)
{
	this->mDefaultImage = inDefaultImage;
	this->GetRootNode()->SetDefaultImage(inDefaultImage);
}

void PieMenu::SetFont(Font* inFont)
{
    this->mFont = inFont;
}

void PieMenu::SetNodeDistance(float inNewXDistance, float inNewYDistance)
{
    this->mNodeList->SetNodeDistance(inNewXDistance, inNewYDistance);
}

//void PieMenu::SetNodeTargaName(const string& inTarga)
//{
//    this->mPieNodeTarga = inTarga;
//}

// Set our own position, but also tell pie node list
void PieMenu::setPos(int x, int y)
{
    Panel::setPos(x, y);

    int thePieWidth, thePieHeight;
    this->getSize(thePieWidth, thePieHeight);
    this->mNodeList->SetBasePosition(x, y, thePieWidth, thePieHeight);
}

void PieMenu::setSize(int wide, int tall)
{
    Panel::setSize(wide, tall);

    int thePieX, thePieY;
    this->getPos(thePieX, thePieY);
    this->mNodeList->SetBasePosition(thePieX, thePieY, wide, tall);
}

void PieMenu::SetFadeState(bool inNewVisState)
{
    //Panel::setVisible(inNewVisState);

    this->mNodeList->SetFadeState(inNewVisState);
}


// update
void PieMenu::Update(float theCurrentTime)
{
    // Tell all nested components to Update
    this->mNodeList->Update(theCurrentTime);
}

void PieMenu::UpdateMenuFromTech(const AvHTechTree& inMenuCosts, int inPurchaselevel)
{
	this->mNodeList->UpdateMenuFromTech(inMenuCosts, inPurchaselevel);
}

void PieMenu::VidInit()
{
	this->mNodeList->VidInit();
}

