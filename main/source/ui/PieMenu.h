//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: PieMenu.h $
// $Date: 2002/08/16 02:29:19 $
//
//-------------------------------------------------------------------------------
// $Log: PieMenu.h,v $
// Revision 1.10  2002/08/16 02:29:19  Flayra
// - Added document headers
// - Started to add support for pie nodes with both images and text
//
//===============================================================================
#ifndef PIEMENU_H
#define PIEMENU_H

#include "VGUI_Panel.h"
#include "VGUI_Font.h"
#include "vector"
#include "string"
#include "cl_dll/chud.h"
#include "ui/FadingImageLabel.h"
#include "ui/PieNode.h"
#include "ui/GammaAwareComponent.h"
#include "ui/ReloadableComponent.h"

using std::string;
using vgui::Font;

class PieMenu : public vgui::Panel, public ReloadableComponent, public GammaAwareComponent
{
public:
    PieMenu(const string& inRootName, int x, int y, int wide, int tall);
    virtual ~PieMenu();

    // Adds an input signal to our nodes, not ourself.  Use this method to find out
    // when the mouse has been released over a node.  After calling this, the input signal
    // functions should only be called passing PieNodes as parameters, so cast them and
    // call GetNodeName() to determine action to perform.
	void AddInputSignalForNodes(InputSignal* s);

    bool AddNode(const string& inNodeString);

	void ChangeNode(int inMessageID, int inNewMessageID, const string& inNewText);

	void DisableNodesWhoseChildrenAreDisabled();

    PieNode* GetRootNode(void);

	bool GetSelectedNode(PieNode*& outNode);

    // Called when mouse released over one of our child image labels.  This
    // is how it determines which item was selected.
    virtual void mouseReleased(MouseCode code, Panel* panel);

	virtual void NotifyGammaChange(float inGammaSlope);
	
	void RecomputeVisibleSize(void);

	void ResetToDefaults();

    void SetConnectorName(const string& inConnectorName);

    void SetConstructionComplete();

	void SetDefaultImage(const string& inDefaultImage);

    void SetFont(Font* inFont);

    void SetNodeDistance(float inNewXDistance, float inNewYDistance);

    // Looks for node and sets its enabled state.  It changes how it draws accordingly.
	bool SetNodeEnabled(const string& inNodeName, bool inNewState);

    void SetNodeTargaName(const string& inTarga);

	virtual void setPos(int x,int y);

	virtual void setSize(int wide,int tall);

    void SetTolerance(int inPercentage);

    // Override so we can determine how long it has been open 
	virtual void SetFadeState(bool state);

    void Update(float theCurrentTime);

	void UpdateMenuFromTech(const AvHTechTree& inMenuCosts, int inPurchaselevel);

	void VidInit();

protected:

    // Override these to do nothing.  The only thing to be drawn is our pie nodes.
	//virtual void paint();
	virtual void paintBackground();

private:
    PieNodeList*            mNodeList;

    Font*                   mFont;

	string					mDefaultImage;

};


#endif
