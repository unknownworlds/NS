//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: PieNode.h $
// $Date: 2002/08/31 18:04:32 $
//
//-------------------------------------------------------------------------------
// $Log: PieNode.h,v $
// Revision 1.15  2002/08/31 18:04:32  Flayra
// - Work at VALVe
//
// Revision 1.14  2002/08/16 02:29:20  Flayra
// - Added document headers
// - Started to add support for pie nodes with both images and text
//
//===============================================================================
#ifndef PIENODE_H
#define PIENODE_H

#include "ui/FadingImageLabel.h"
#include "ui/GammaAwareComponent.h"
#include "mod/AvHSharedTypes.h"
#include "string"
using std::string;

const int kNumNodes = 8;

typedef vector<int> MessageIDList;

class PieNode : public FadingImageLabel
{
public:
    PieNode(const string& inNodeString, int inMessageID);
    virtual ~PieNode();

    // Call FadingImageLabel::addInputSignal, then call it on all our children
	void AddInputSignalForNodes(InputSignal* s);

    // Add and forget
    bool AttachNode(PieNode* inNode, int inAngularOffset, float inNodeXDistance, float inNodeYDistance);

	void DisableNodesGreaterThanCost(int inCost);

	void DisableNodesNotInMessageList(const MessageIDList& inList);

	void DisableNodesWhoseChildrenAreDisabled();

    //vgui::Color GetColor() const;

	virtual void getBgColor(int& r,int& g,int& b,int& a);
	virtual void getBgColor(Color& color);

    //virtual void FadedIn();

    //virtual void FadedOut();

	virtual bool GetDrawHighlighted() const;
	
    virtual bool GetDrawSelected() const;

	virtual bool GetEnabled() const;

    virtual bool GetHasChild(const PieNode* inNode) const;

    bool GetIsAbove(const PieNode* inNode) const;

	void GetMaxTextSize(int& outWidth, int& outHeight);

    int GetMessageID() const;

	int GetPointCost() const;

    PieNode* GetNodeAtAngularOffset(int inAngularOffset);

    const string& GetNodeName() const;

    PieNode* GetRoot();

	virtual bool GetSelectedNode(PieNode*& outNode);

    virtual bool HasChildren(void) const;

    virtual bool HighlightNode(void);

    virtual bool IsAdjacentTo(const PieNode* inNode);

    virtual bool IsChildOf(const PieNode* inNode);

	virtual bool isVisible();

	void MoveTree(int inDeltaX, int inDeltaY);

	void ChangeNode(int inMessageID, int inNewMessageID, const string& inNewText);

	void ChangeNode(int inNewMessageID, const string& inNewText);

	void ResetToDefaults();

	void SetColorBias(float inBias);

    void SetConnectorName(const string& inConnectorName);

	void SetDefaultImage(const string& inDefaultImage);

    void SetDrawSelected(bool inDrawSelected);

    void SetNodeAndAdjacentChildrenFadeState(bool inFadeState);

    void SetNodeAndAllChildrenFadeState(bool inNewVisState);

    void SetNodeDistance(float inNewXDistance, float inNewYDistance);

	//bool SetNodeEnabled(const string& inNodeName, bool inNewState);

    bool SetPosFromOffset(int inOffset, float inNodeXDistance, float inNodeYDistance);

	//virtual void setSize(int wide,int tall);

	virtual void SetSizeKeepCenter(int inWidth, int inHeight);

    //void SetTolerance(int inPercentage);

    virtual void SetFadeState(bool inNewFadeState);

	virtual void SetVisibleSize(int inVisWidth, int inVisHeight);

	void SetNodeAndChildrenVisible(bool inVisibilityState);

    virtual void Update(float theCurrentTime);

	void UpdateMenuFromTech(const AvHTechNodes& inMenuCosts, int inPurchaseLevel);

	virtual void VidInit(void);

private:

    bool            ComputeRelativeConnectorCoordinates(PieNode* theCurrentPieNode, int& x0, int& y0, int& x1, int& y1);

	void			ComputeAndSetLocalizedText();

    //void            GetCenteredCoordinatesFromQuadrant(int inQuadrant, int inRelativeToX, int inRelativeToY, int& outX, int& outY);

    void            GetCenteredCoordinatesFromQuadrant(int inQuadrant, int& outX, int& outY);

    bool            HasSelectedNodeAbove(void) const;

    bool            HasSelectedNodeBelow(void) const;

    virtual void    DoPaint();

	virtual void	paintBackground();

    static int      mDegrees[kNumNodes];

    string          mNodeName;
	float			mColorBias;

    // Count counter-clockwise
    PieNode*        mArray[kNumNodes];

    PieNode*        mParentPieNode;

	string			mConnectorSpriteName;
	int				mConnectorSprite;

	string			mLocalizedText;
	string			mBaseText;
	string			mDefaultText;
	string			mDefaultImage;

    int             mMessageID;
	int				mDefaultID;

	int				mPointCost;

	bool			mEnabled;

    bool            mDrawSelected;

};

class PieNodeList //: public InputSignal
{
public:
    PieNodeList(const string& inRootName, float inDist);
    virtual ~PieNodeList();

    // Adds a new node using the string from the description file (node name, relative position). It uses
    // the same texture name as the root node.  Fails if SetConstructionComplete was already called.
    bool AddNode(const string& inNodeString, Font* inFont, const Color& inBGColor, const Color& inFGColor, const string& inDefaultImage);
		
    void AddInputSignalForNodes(InputSignal* s);

	void ChangeNode(int inMessageID, int inNewMessageID, const string& inNewText);

	void DisableNodesGreaterThanCost(int inCost);

	void DisableNodesNotInMessageList(const MessageIDList& inList);

	void DisableNodesWhoseChildrenAreDisabled();

    PieNode* GetRoot();

	bool GetSelectedNode(PieNode*& outNode);

	void RecomputeVisibleSize(void);

	void ResetToDefaults();

    void SetBasePosition(int inX, int inY, int inPieWidth, int inPieHeight);

    void SetConnectorName(const string& inConnectorName);

    // Indicate that there are no more nodes.  Set self as input signal for all nodes.
    void SetConstructionComplete();

    void setParent(vgui::Panel* inPanel);
    
    void SetNodeDistance(float inNewXDistance, float inNewYDistance);

	//bool SetNodeEnabled(const string& inNodeName, bool inNewState);

    // Set root visible only, until mouse moves over other components
    //void SetRootVisible(bool inNewVisibility);

    //void SetTolerance(int inPercentage);

    // When hiding pie menu, hide all nodes
    // When showing pie menu, show only root
    virtual void SetFadeState(bool inNewVisibility);

	virtual void SetSizeKeepCenter(int inWidth, int inHeight);

    void Update(float inTime);

	void UpdateMenuFromTech(const AvHTechNodes& inMenuCosts, int inPurchaseLevel);

	void VidInit();

    // Mark node and its first-level children visible
	//virtual void cursorEntered(Panel* panel);

	//virtual void cursorExited(Panel* panel);

    // If right mouse button released over node, mark all nodes invisible
    // and hide the control.  Don't select a node, that is assumed to be handled by the outer 
    // PieMenu::AddInputSignalForNodes().  If mouse button was released off the control entirely,
    // it should generated a ClosePieMenu command that sets whole thing invisible again.
	//virtual void mouseReleased(MouseCode code, Panel* panel);

    // Irrelevant events, override so we can instantiate
//    virtual void cursorMoved(int x,int y,Panel* panel);
//	virtual void mousePressed(MouseCode code,Panel* panel);
//	virtual void mouseDoublePressed(MouseCode code,Panel* panel);
//	virtual void mouseWheeled(int delta,Panel* panel);
//	virtual void keyPressed(KeyCode code,Panel* panel);
//	virtual void keyTyped(KeyCode code,Panel* panel);
//	virtual void keyReleased(KeyCode code,Panel* panel);
//	virtual void keyFocusTicked(Panel* panel);

private:
    
    PieNode*			mRoot;
    float				mNodeXDistance;
    float				mNodeYDistance;

};

#endif