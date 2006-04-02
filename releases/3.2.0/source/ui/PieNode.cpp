//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: PieNode.cpp $
// $Date: 2002/08/31 18:04:32 $
//
//-------------------------------------------------------------------------------
// $Log: PieNode.cpp,v $
// Revision 1.18  2002/08/31 18:04:32  Flayra
// - Work at VALVe
//
// Revision 1.17  2002/08/16 02:29:19  Flayra
// - Added document headers
// - Started to add support for pie nodes with both images and text
//
// Revision 1.16  2002/07/08 16:18:52  Flayra
// - Tried to turn off mouse capture so menu can be binded to keyboard and mouse properly
//
//===============================================================================
#include "ui/PieNode.h"
#include "util/Tokenizer.h"
#include "cl_dll/vgui_int.h"
#include "ui/UIUtil.h"

const float kTwoPI = 2.0f*3.141519f;
int PieNode::mDegrees[kNumNodes] = {90, 135, 180, 225, 270, 315, 0, 45};
PieNode* gNodeToTrack = NULL;
const int kDarkGreenColor = 130;
const int kDisabledColorComponent = 180;

PieNode::PieNode(const string& inNodeString, int inMessageID) : FadingImageLabel(0, 0) //, 640, 480)
{
    memset(this->mArray, 0, kNumNodes*sizeof(PieNode*));
	this->mBaseText = inNodeString;
	this->mPointCost = 0;
    this->setText(inNodeString.c_str());
    this->mNodeName = inNodeString;
    this->mParentPieNode = NULL;
    this->mMessageID = inMessageID;
    this->mDrawSelected = false;
	this->setVisible(false);
	this->mEnabled = true;
	this->mColorBias = 1.0f;
	
	this->ComputeAndSetLocalizedText();

	// Save defaults for resetting back to
	this->mDefaultText = this->mBaseText;
	this->mDefaultID = this->mMessageID;
	this->mConnectorSprite = 0;

    //this->SetFadeState(false);
}

PieNode::~PieNode()
{
    for(int i = 0; i < kNumNodes; i++)
    {
        delete this->mArray[i];
        this->mArray[i] = NULL;
    }
}

void PieNode::AddInputSignalForNodes(InputSignal* s)
{
    FadingImageLabel::addInputSignal(s);

    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
            theCurrentPieNode->AddInputSignalForNodes(s);
        }
    }
}

bool PieNode::AttachNode(PieNode* inNode, int inAngularOffset, float inNodeXDistance, float inNodeYDistance)
{
    bool theSuccess = false;

    if((inAngularOffset >= 0) && (inAngularOffset < kNumNodes))
    {
        if(this->mArray[inAngularOffset] == NULL)
        {
            this->mArray[inAngularOffset] = inNode;

            inNode->SetPosFromOffset(inAngularOffset, inNodeXDistance, inNodeYDistance);

            //this->addChild(inNode);
            
            //inNode->setParent(this);

            inNode->mParentPieNode = this;
            
            theSuccess = true;
        }
    }

    return theSuccess;
}

void PieNode::ChangeNode(int inMessageID, int inNewMessageID, const string& inNewText)
{
	if(this->mMessageID == inMessageID)
	{
		this->mMessageID = inNewMessageID;
		if(inNewText != "")
		{
			this->mBaseText = inNewText;
			this->ComputeAndSetLocalizedText();
		}
	}
    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
			theCurrentPieNode->ChangeNode(inMessageID, inNewMessageID, inNewText);
		}
	}
}

void PieNode::ChangeNode(int inNewMessageID, const string& inNewText)
{
	this->mMessageID = inNewMessageID;
	if(inNewText != "")
	{
		this->mBaseText = inNewText;
		this->ComputeAndSetLocalizedText();
	}
}


void PieNode::ComputeAndSetLocalizedText()
{
	string theString = this->mBaseText;

	char theLocalizedString[128];
	if(CHudTextMessage::LocaliseTextString(theString.c_str(), theLocalizedString, 128))
	{
		// Remove newlines and junk
		for(int i = 0; i < 128; i++)
		{
			char theCurrentChar = theLocalizedString[i];
			if((theCurrentChar == '\n') || (theCurrentChar == '\r'))
			{
				theLocalizedString[i] = '\0';
				break;
			}
		}
		theString = theLocalizedString;
	}

	// Save it
	this->mLocalizedText = theString;
	this->setText(theString.c_str());
}

void PieNode::DisableNodesNotInMessageList(const MessageIDList& inList)
{
	this->mEnabled = false;

	// If node is in list, enable it
	MessageIDList::const_iterator theFindIter = std::find(inList.begin(), inList.end(), this->mMessageID);
	if(theFindIter != inList.end())
	{
		this->mEnabled = true;
	}

	// Call recursively on children
    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
			theCurrentPieNode->DisableNodesNotInMessageList(inList);
		}
	}
}

void PieNode::DisableNodesWhoseChildrenAreDisabled()
{
    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
			theCurrentPieNode->DisableNodesWhoseChildrenAreDisabled();
		}
	}

	// Now disable ourself we have children and they are all disabled
	bool theHasChildren = false;
	bool theHasEnabledChild = false;

    for(i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
			theHasChildren = true;
			if(theCurrentPieNode->GetEnabled())
			{
				theHasEnabledChild = true;
				break;
			}
		}
	}

	if(theHasChildren && !theHasEnabledChild)
	{
		this->mEnabled = false;
	}
}

//vgui::Color PieNode::GetColor() const
//{
//    return vgui::Color(0, (this->GetDrawSelected() ? 255 : kDarkGreenColor), 0, this->GetValveAlpha());
//    //return vgui::Color(0, kDarkGreenColor, 0, this->GetValveAlpha());
//}

void PieNode::getBgColor(int& r, int& g,int& b, int& a)
{
	Color theColor;
	this->getBgColor(theColor);

	theColor.getColor(r, g, b, a);
}

void PieNode::getBgColor(Color& outColor)
{
	Color theBGColor;
	FadingImageLabel::getBgColor(theBGColor);

	if(this->mEnabled)
	{
		if(this->mDrawSelected)
		{
			// Brighten color slightly
			int theR, theG, theB, theA;
			theBGColor.getColor(theR, theG, theB, theA);

			float theScalar = 1.4f;
			theBGColor.setColor(min(theScalar*theR, 255), min(theScalar*theG, 255), min(theScalar*theB, 255), theA);
		}
	}
	else
	{
		int theR, theG, theB, theA;
		theBGColor.getColor(theR, theG, theB, theA);

		theBGColor.setColor(kDisabledColorComponent, kDisabledColorComponent, kDisabledColorComponent, theA);
	}

	outColor = theBGColor;
}

// Override and provide no behavior so node visibility isn't changed.  This is needed so
// the node can still receive input signal events and be faded in when the mouse moves
// over it
//void PieNode::FadedIn()
//{
//}

//void PieNode::FadedOut()
//{
//}

bool PieNode::GetDrawHighlighted() const
{
	return this->GetDrawSelected();
}

bool PieNode::GetDrawSelected() const
{
    return this->mDrawSelected;
}

bool PieNode::GetEnabled() const
{
	return this->mEnabled;
}

bool PieNode::GetHasChild(const PieNode* inNode) const
{
    bool theHasChild = false;

    for(int i = 0; i < kNumNodes; i++)
    {
        if(this->mArray[i] == inNode)
        {
            theHasChild = true;
            break;
        }
    }

    return theHasChild;
}

bool PieNode::GetIsAbove(const PieNode* inNode) const
{
    bool thisIsAbove = false;

    const PieNode* thePieNode = inNode;

    while(thePieNode != NULL)
    {
        if(thePieNode == this)
        {
            thisIsAbove = true;
            break;
        }
        else
        {
            thePieNode = thePieNode->mParentPieNode;
        }
    }

    return thisIsAbove;
}

int PieNode::GetMessageID() const
{
    return this->mMessageID;
}

PieNode* PieNode::GetNodeAtAngularOffset(int inAngularOffset)
{
    PieNode* thePieNode = NULL;

    if((inAngularOffset >= 0) && (inAngularOffset < kNumNodes))
    {
        thePieNode = this->mArray[inAngularOffset];
    }

    return thePieNode;
}

const string& PieNode::GetNodeName() const
{
    return this->mNodeName;
}

int PieNode::GetPointCost() const
{
	return this->mPointCost;
}

PieNode* PieNode::GetRoot()
{
    PieNode* theRoot = this;

    while(theRoot->mParentPieNode != NULL)
    {
        theRoot = theRoot->mParentPieNode;
    }

    return theRoot;
}

bool PieNode::HasChildren(void) const
{
    bool theHasChildren = false;

    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
            theHasChildren = true;
            break;
        }
    }
    return theHasChildren;
}

bool PieNode::HighlightNode(void)
{
    bool theSuccess = false;

    // Only valid if parent is NULL, or parent is already selected
    // This prevents selecting from the edge, you must follow track
    //if(!this->mParentPieNode || this->mParentPieNode->GetFadeState())
    //{
        // Get root then set all of it's children as faded out
        PieNode* theRoot = this->GetRoot();
        theRoot->SetNodeAndAllChildrenFadeState(false);
        theRoot->SetFadeState(true);
        
        // For each non-root parent, set it's fade out state to true
        PieNode* theCurrentPieNode = this->mParentPieNode;
        while(theCurrentPieNode && (theCurrentPieNode != theRoot))
        {
            theCurrentPieNode->SetFadeState(true);
            theCurrentPieNode = theCurrentPieNode->mParentPieNode;
        }
        
        // Set self and adjacent children to fade in
        // If there are no nodes below us, parent node and its adjacents (comment
        // this out if you don't believe it looks better)
        PieNode* theBottomFullNode = /*this->HasChildren() ?*/ this/* : this->mParentPieNode*/;
        if(theBottomFullNode)
        {
            theBottomFullNode->SetNodeAndAdjacentChildrenFadeState(true);
        }

        theSuccess = true;
    //}

	this->setAsMouseCapture(false);
		
    return theSuccess;
}

bool PieNode::IsAdjacentTo(const PieNode* inNode)
{
    bool isAdjacent = false;

    if(inNode == this->mParentPieNode)
    {
        isAdjacent = true;
    }
    else
    {
        isAdjacent = this->GetHasChild(inNode);

        // Check siblings
        if(!isAdjacent && (this->mParentPieNode))
        {
            isAdjacent = this->mParentPieNode->GetHasChild(inNode);
        }
    }

    return isAdjacent;
}

bool PieNode::IsChildOf(const PieNode* inNode)
{
    bool isChild = false;

    if(inNode && (inNode == this->mParentPieNode))
    {
        isChild = true;
    }

    return isChild;
}

bool PieNode::isVisible()
{
    //return (this->GetValveAlpha() < 255);
    return FadingImageLabel::isVisible();
}

// Important: Assumes both pie nodes are the same size
// The first coords it returns are the start of the connector, relative to this node
// The second coords it returns are the end of the connector (designated by inChildPieNode), relative to this node
bool PieNode::ComputeRelativeConnectorCoordinates(PieNode* inChildPieNode, int& theOutX0, int& theOutY0, int& theOutX1, int& theOutY1)
{
    ASSERT(this != inChildPieNode);

    // Check position of dest pie node relative to us
    bool theSuccess = false;

    int x0, y0;
    this->getPos(x0, y0);

    int x1, y1;
    inChildPieNode->getPos(x1, y1);

    int theFirstQuadrant, theSecondQuadrant;
    int theDX = x1 - x0;
    int theDY = y1 - y0;
    
    // directly above/below or left/right
    if((theDX == 0) || (theDY == 0))
    {
        if(theDY == 0)
        {
            // If left
            if(x1 < x0)
            {
                // x0/y0 -> left edge
                theFirstQuadrant = 2;
                // x1/y1 -> right edge
                theSecondQuadrant = 6;
            }
            // If right
            else
            {
                // x0/y0 -> right edge
                theFirstQuadrant = 6;
                // x1/y1 -> left edge
                theSecondQuadrant = 2;
            }
        }
        else
        {
            // If below
            if(y1 > y0)
            {
                // x0/y0 -> bottom edge
                theFirstQuadrant = 4;
                // x1/y1 -> top edge
                theSecondQuadrant = 0;
            }
            // If above
            else
            {
                // x0/y0 -> top edge
                theFirstQuadrant = 0;
                // x1/y1 -> bottom edge
                theSecondQuadrant = 4;
            }
        }
        theSuccess = true;
    }
    // otherwise draw from middle of nearest edge (not required yet)
    //else
    //{
    //    ASSERT(false);
    //}
    
    //ASSERT(theSuccess);

	//  Don't draw lines if we failed above
    if(theSuccess)
	{
		// Now compute the coords relative to each
	    //this->GetCenteredCoordinatesFromQuadrant(theFirstQuadrant, 0, 0, theOutX0, theOutY0);
		//this->GetCenteredCoordinatesFromQuadrant(theSecondQuadrant, theDX, theDY, theOutX1, theOutY1);
	    this->GetCenteredCoordinatesFromQuadrant(theFirstQuadrant, theOutX0, theOutY0);
		//this->GetCenteredCoordinatesFromQuadrant(theSecondQuadrant, theOutX1, theOutY1);
		inChildPieNode->GetCenteredCoordinatesFromQuadrant(theSecondQuadrant, theOutX1, theOutY1);
		theOutX1 += theDX;
		theOutY1 += theDY;
	}
    
    return theSuccess;
}

// inQuadrant = 0 -> top edge
// inQuadrant = 2 -> left edge
// inQuadrant = 4 -> bottom edge
// inQuadrant = 6 -> right edge
void PieNode::GetCenteredCoordinatesFromQuadrant(int inQuadrant, int& outX, int& outY)
{
	int theWidth, theHeight;
	this->getSize(theWidth, theHeight);

	int theVisibleWidth, theVisibleHeight;
	this->GetVisibleSize(theVisibleWidth, theVisibleHeight);

	int theVisibleXOffset, theVisibleYOffset;
	theVisibleXOffset = (theWidth - theVisibleWidth)/2;
	theVisibleYOffset = (theHeight - theVisibleHeight)/2;

	// Corner nodes no longer supported
	ASSERT(inQuadrant != 1);
	ASSERT(inQuadrant != 3);
	ASSERT(inQuadrant != 5);
	ASSERT(inQuadrant != 7);

    switch(inQuadrant)
    {
    case 0:
        outX = theWidth/2;
        outY = theVisibleYOffset;
        break;
    case 2:
        outX = theVisibleXOffset;
        outY = theHeight/2;
        break;
    case 4:
        outX = theWidth/2;
        outY = theVisibleYOffset + theVisibleHeight;
        break;
    case 6:
        outX = theVisibleXOffset + theVisibleWidth;
        outY = theHeight/2;
        break;
    }

}

// Draw nodes
void PieNode::DoPaint()
{
    FadingImageLabel::DoPaint();
}

// Draw node lines
void PieNode::paintBackground()
{
    // For each child
    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
            // Draw line from us to them indicating pie node in that direction
            if(theCurrentPieNode->GetValveAlpha() < 255)
            {
                // If node above us isn't highlighted, don't draw line to us
                //if(theCurrentPieNode->HasSelectedNodeAbove() || theCurrentPieNode->HasSelectedNodeBelow())
				
				// All those lines are getting confusing.  Only draw the lines around this pie node that
				// connect to a selected parent or selected child
                if(theCurrentPieNode->HasSelectedNodeBelow() || this->GetDrawSelected())
                {
                    // Find correct line to draw from center of edge of source pie node to center
                    // of edge of dest pie node
                    int theRelativeLineX0, theRelativeLineY0, theRelativeLineX1, theRelativeLineY1;
                    if(this->ComputeRelativeConnectorCoordinates(theCurrentPieNode, theRelativeLineX0, theRelativeLineY0, theRelativeLineX1, theRelativeLineY1))
                    {
                        // What color to draw the line in?
                        //vgui::Color theCurrentColor = this->GetColor();
                        vgui::Color theCurrentColor;
						//theCurrentPieNode->getBgColor(theCurrentColor);
						
						if(this->mEnabled)
						{
							theCurrentPieNode->getFgColor(theCurrentColor);
						}
						else
						{
							theCurrentPieNode->getBgColor(theCurrentColor);
						}
						
                        int r, g, b, a;
                        theCurrentColor.getColor(r, g, b, a);

						// Take gamma into account
						//r *= this->mColorBias;
						//g *= this->mColorBias;
						//b *= this->mColorBias;
						a = this->GetValveAlpha();

//						//if(theCurrentPieNode->GetEnabled())
//						//{
//						g = (theCurrentPieNode->HasSelectedNodeBelow() ? 255 : kDarkGreenColor);
//						//g = (theCurrentPieNode->HasSelectedNodeAbove() ? 255 : kDarkGreenColor);
//						//}
//						// ...else this will draw lines in disabled color too, using current fade alpha
                        
                        // Stupid Valve-alpha, vguiSimpleLine wants normal alpha of course
						a = 255 - a;
						a *= this->mColorBias;
						
						// Only draw if child isn't invisible.  Draw line
						// relative from current node.

						if(this->mConnectorSprite == 0 && (this->mConnectorSpriteName != ""))
						{
							this->mConnectorSprite = Safe_SPR_Load(this->mConnectorSpriteName.c_str());
						}

						if(this->mConnectorSprite > 0)
						{
							// Approximate alpha
							float theAlpha = a/255.0f;
							int theSpriteRed = theAlpha*r;
							int theSpriteGreen = theAlpha*g;
							int theSpriteBlue = theAlpha*b;
							
							SPR_Set(this->mConnectorSprite, theSpriteRed, theSpriteGreen, theSpriteBlue);
							int theLineWidth = abs(theRelativeLineX1 - theRelativeLineX0);
							int theLineHeight = abs(theRelativeLineY1 - theRelativeLineY0);
							
							// Use second frame if vertical
							int theConnectorFrame = 0;
							if(theLineHeight > 0)							
							{
								theConnectorFrame = 1;
							}
							
							int theConnectorSpriteWidth = SPR_Width(this->mConnectorSprite, theConnectorFrame);
							int theConnectorSpriteHeight = SPR_Height(this->mConnectorSprite, theConnectorFrame);

							int thePieNodeWidth, thePieNodeHeight;
							this->getContentSize(thePieNodeWidth, thePieNodeHeight);
							
							int thePieNodeX, thePieNodeY;
							this->getPos(thePieNodeX, thePieNodeY);
							
							int theStartX = 0;
							int theStartY = 0;

							if(theConnectorFrame == 0)
							{
								int theXOffset = (theLineWidth - theConnectorSpriteWidth)/2;
								//if(theXOffset < 0)
								//	theXOffset = 0;
								
								theStartX = min(theRelativeLineX0, theRelativeLineX1) + theXOffset;
								theStartY = min(theRelativeLineY0, theRelativeLineY1) - theConnectorSpriteHeight/2;

//								int theScissorStartX = thePieNodeX + min(theRelativeLineX0, theRelativeLineX1) + thePieNodeWidth/2;
//								int theScissorStartY = thePieNodeY + theStartY;
//								int theScissorEndX = theScissorStartX + theLineWidth - thePieNodeWidth/2;
//								int theScissorEndY = theScissorStartY + theConnectorSpriteHeight;
								
								//vguiSimpleBox(theScissorStartX - thePieNodeX, theScissorStartY - thePieNodeY, theScissorEndX - thePieNodeX, theScissorEndY - thePieNodeY, 255, 255, 0, 128);

//								SPR_EnableScissor(theScissorStartX, theScissorStartY, theLineWidth, theConnectorSpriteHeight);
							}
							else
							{
								int theYOffset = (theLineHeight - theConnectorSpriteHeight)/2;
								//if(theYOffset < 0)
								//	theYOffset = 0;

								theStartX = min(theRelativeLineX0, theRelativeLineX1) - theConnectorSpriteWidth/2;
								theStartY = min(theRelativeLineY0, theRelativeLineY1) + theYOffset;

//								int theScissorStartX = thePieNodeX + theStartX;
//								int theScissorStartY = thePieNodeY + min(theRelativeLineY0, theRelativeLineY1);
//								int theScissorEndX = theScissorStartX + theConnectorSpriteWidth;
//								int theScissorEndY = theScissorStartY + theLineHeight - thePieNodeHeight;
								
								//vguiSimpleBox(theScissorStartX - thePieNodeX, theScissorStartY - thePieNodeY, theScissorEndX - thePieNodeX, theScissorEndY - thePieNodeY, 255, 255, 0, 128);

//								SPR_EnableScissor(theScissorStartX, theScissorStartY, theConnectorSpriteWidth, theLineHeight);
							}

							// Take into account our position, including our parent(s), when drawing
//							Panel* theParent = this->getParent();
//							while(theParent)
//							{
//								int theX, theY;
//								theParent->getPos(theX, theY);
//								theStartX += theX;
//								theStartY += theY;
//								theParent = theParent->getParent();
//							}

							// Draw it
							//SPR_DrawAdditive(theConnectorFrame, theStartX, theStartY, NULL);
							SPR_DrawHoles(theConnectorFrame, theStartX, theStartY, NULL);
//							gEngfuncs.pfnSPR_DisableScissor();
							
						}
						else
						{
							vguiSimpleLine(theRelativeLineX0, theRelativeLineY0, theRelativeLineX1, theRelativeLineY1, r, g, b, a);
						}

						vguiSimpleBox(theRelativeLineX0, theRelativeLineY0, theRelativeLineX1, theRelativeLineY1, 255, 255, 0, 128);
					}
				}
			}
		}
	}
}

void PieNode::GetMaxTextSize(int& outWidth, int& outHeight)
{
	int theTempWidth, theTempHeight;
	const float kTextHeightBias = 1.5f;

	// Use the text extents to dictate desired node size
	// Bias text size a bit for usability
	this->getTextSize(theTempWidth, theTempHeight);

	theTempHeight *= kTextHeightBias;
	if(theTempWidth > outWidth)
	{
		outWidth = theTempWidth;
	}
	if(theTempHeight > outHeight)
	{
		outHeight = theTempHeight;
	}
	
	for(int i = 0; i < kNumNodes; i++)
	{
		PieNode* theCurrentNode = this->mArray[i];
		if(theCurrentNode)
		{
			theCurrentNode->GetMaxTextSize(theTempWidth, theTempHeight);
			if(theTempWidth > outWidth)
			{
				outWidth = theTempWidth;
			}
			if(theTempHeight > outHeight)
			{
				outHeight = theTempHeight;
			}
		}
	}
}

bool PieNode::GetSelectedNode(PieNode*& outNode)
{
	bool theFoundNode = false;

	if(this->mDrawSelected)
	{
		outNode = this;
		theFoundNode = true;
	}
	else
	{
		for(int i = 0; i < kNumNodes; i++)
		{
			PieNode* theCurrentNode = this->mArray[i];
			if(theCurrentNode)
			{
				theFoundNode = theCurrentNode->GetSelectedNode(outNode);
				if(theFoundNode)
				{
					break;
				}
			}
		}
	}

	return theFoundNode;
}

bool PieNode::HasSelectedNodeAbove(void) const
{
    bool hasSelectedNodeAbove = false;

    const PieNode* thePieNode = this;

    while(thePieNode != NULL)
    {
        if(thePieNode->GetDrawSelected())
        {
            hasSelectedNodeAbove = true;
            break;
        }
        else
        {
            thePieNode = thePieNode->mParentPieNode;
        }
    }

    return hasSelectedNodeAbove;
}


bool PieNode::HasSelectedNodeBelow(void) const
{
    bool hasSelectedNodeBelow = false;

    // For each of our children
    if(this->GetDrawSelected())
    {
        hasSelectedNodeBelow = true;
    }
    else
    {
        for(int i = 0; i < kNumNodes; i++)
        {
            PieNode* theCurrentNode = this->mArray[i];
            if(theCurrentNode)
            {
                // If child has selected node below
                if(theCurrentNode->HasSelectedNodeBelow())
                {
                    // break
                    hasSelectedNodeBelow = true;
                    break;
                }
            }
        }
    }

    return hasSelectedNodeBelow;
}

void PieNode::MoveTree(int inDeltaX, int inDeltaY)
{
	int theCurrentX, theCurrentY;
	this->getPos(theCurrentX, theCurrentY);
	this->setPos(theCurrentX + inDeltaX, theCurrentY + inDeltaY);

    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
            theCurrentPieNode->MoveTree(inDeltaX, inDeltaY);
        }
    }
}

void PieNode::ResetToDefaults()
{
	this->mMessageID = this->mDefaultID;
	this->mBaseText = this->mDefaultText;
	this->ComputeAndSetLocalizedText();
}

void PieNode::SetColorBias(float inBias)
{
	this->mColorBias = inBias;

    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
			theCurrentPieNode->SetColorBias(this->mColorBias);
		}
	}
}

void PieNode::SetConnectorName(const string& inConnectorName)
{
	string theNewSpriteName = UINameToSprite(inConnectorName, ScreenWidth());
	if(theNewSpriteName != this->mConnectorSpriteName)
	{
		// Mark it to be reloaded
		this->mConnectorSpriteName = theNewSpriteName;
		this->mConnectorSprite = 0;

		for(int i = 0; i < kNumNodes; i++)
		{
			PieNode* theCurrentPieNode = this->mArray[i];
			if(theCurrentPieNode)
			{
				theCurrentPieNode->SetConnectorName(inConnectorName);
			}
		}
	}
}

void PieNode::SetDefaultImage(const string& inDefaultImage)
{
	this->mDefaultImage = inDefaultImage;
	this->SetSpriteName(this->mDefaultImage);
}

void PieNode::SetDrawSelected(bool inDrawSelected)
{
	if(this->mEnabled)
	{
		this->mDrawSelected = inDrawSelected;
	}
}

void PieNode::SetNodeAndAllChildrenFadeState(bool inNewFadeState)
{
    this->SetFadeState(inNewFadeState);

    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
            theCurrentPieNode->SetNodeAndAllChildrenFadeState(inNewFadeState);
        }
    }
}

void PieNode::SetNodeDistance(float inNewXDistance, float inNewYDistance)
{
    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
            theCurrentPieNode->SetPosFromOffset(i, inNewXDistance, inNewYDistance);
        }
    }
}

bool PieNode::SetPosFromOffset(int inAngularOffset, float inNodeXDistance, float inNodeYDistance)
{
    bool theSuccess = false;

    if((inAngularOffset >= 0) && (inAngularOffset < kNumNodes))
    {
        // 0 corresponds to north (90 degrees), 90 corresponds to west (180 degrees)
        // Invert y because half-life uses +Y towards top of screen
        float theDegrees = kTwoPI*(this->mDegrees[inAngularOffset]/360.0f);

        // Calculate spacing in pixels from inNodeDistance.  Use same number of pixels
        // as base for both x and y offset so the spacing is even (as opposed to using
        // inNodeDistance*ScreenHeight for the y-component)
        float theNodeXPixelDist = inNodeXDistance*ScreenWidth();
        int xDiff = (int)(cos(theDegrees)*theNodeXPixelDist);
        float theNodeYPixelDist = inNodeYDistance*ScreenHeight();
        int yDiff = -(int)(sin(theDegrees)*theNodeYPixelDist);
        this->setPos(xDiff, yDiff);

        theSuccess = true;
    }
    return theSuccess;
}

//void PieNode::SetTolerance(int inPercentage)
//{
//}

//void PieNode::setSize(int wide, int tall)
//{
//    // set size for self
//    FadingImageLabel::setSize(wide, tall);
//
//    // set size for all children
//    for(int i = 0; i < kNumNodes; i++)
//    {
//        PieNode* theCurrentPieNode = this->mArray[i];
//        if(theCurrentPieNode)
//        {
//            theCurrentPieNode->setSize(wide, tall);
//        }
//    }
//}

void PieNode::SetSizeKeepCenter(int inWidth, int inHeight)
{
    // set size for self
    FadingImageLabel::SetSizeKeepCenter(inWidth, inHeight);

    // set size for all children
    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
            theCurrentPieNode->SetSizeKeepCenter(inWidth, inHeight);
        }
    }
}

void PieNode::SetNodeAndAdjacentChildrenFadeState(bool inFadeState)
{
    this->SetFadeState(inFadeState);

    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
            theCurrentPieNode->SetFadeState(inFadeState);
        }
    }
}

void PieNode::SetFadeState(bool inNewFadeState)
{
    FadingImageLabel::SetFadeState(inNewFadeState);
}

void PieNode::SetVisibleSize(int inVisWidth, int inVisHeight)
{
	FadingImageLabel::SetVisibleSize(inVisWidth, inVisHeight);

    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
            theCurrentPieNode->SetVisibleSize(inVisWidth, inVisHeight);
        }
    }

}

void PieNode::SetNodeAndChildrenVisible(bool inVisibilityState)
{
	this->setVisible(inVisibilityState);

    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
            theCurrentPieNode->SetNodeAndChildrenVisible(inVisibilityState);
        }
    }
}

void PieNode::Update(float theCurrentTime)
{
    FadingImageLabel::Update(theCurrentTime);

    // Update all children
    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
            theCurrentPieNode->Update(theCurrentTime);
        }
    }
}

void PieNode::UpdateMenuFromTech(const AvHTechTree& inMenuCosts, int inPurchaseLevel)
{
	// Set our next text
	//string theNewText = this->mBaseText;
	string theNewText = this->mLocalizedText;
	this->mPointCost = 0;

    AvHMessageID theMessageID = (AvHMessageID)this->mMessageID;

	// Lookup message id and append onto it if it exists
    bool theResearchable = false;
    int theCost = 0;
    float theTime = 0;
    if(inMenuCosts.GetResearchInfo(theMessageID, theResearchable, theCost, theTime))
    {
        // Only append point cost if it isn't a sprite and there is a point cost greater than 0
        this->mPointCost = theCost;
        if(this->mPointCost > 0 && (theNewText[0] != '!'))
        {
            char theCharArray[6];
            sprintf(theCharArray, " (%d)", this->mPointCost);
            theNewText += string(theCharArray);
        }
    }

	// Set it
	this->setText(theNewText.c_str());

    // Disable or enable node
    bool theMessageIsAvailable = inMenuCosts.GetIsMessageAvailable(theMessageID);

    this->mEnabled = true;
	if((theCost >= 0) && ((this->mPointCost > inPurchaseLevel) /*|| (this->mPointCost == -1)*/ || !theResearchable || !theMessageIsAvailable))
    {
        this->mEnabled = false;
    }

    // do the same for our children
    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
			theCurrentPieNode->UpdateMenuFromTech(inMenuCosts, inPurchaseLevel);
        }
    }
}

void PieNode::VidInit(void)
{
	FadingImageLabel::VidInit();

	if(this->mConnectorSprite > 0)
	{
		// TODO: Can we unload it?
	}
	
	// Mark it to be reloaded next draw
	this->mConnectorSprite = 0;

    // do the same for our children
    for(int i = 0; i < kNumNodes; i++)
    {
        PieNode* theCurrentPieNode = this->mArray[i];
        if(theCurrentPieNode)
        {
			theCurrentPieNode->VidInit();
        }
    }
}




PieNodeList::PieNodeList(const string& inRootName, float inDist)
{
    this->mRoot = new PieNode(inRootName, 0);
    this->mRoot->setVisible(false);
    this->mNodeXDistance = this->mNodeYDistance = inDist;
}

PieNodeList::~PieNodeList()
{
    delete this->mRoot;
}

void PieNodeList::AddInputSignalForNodes(InputSignal* s)
{
    this->mRoot->AddInputSignalForNodes(s);
}

// Accepts input in the form of:
//  #/#/#/#/nodename/msgID
bool PieNodeList::AddNode(const string& inNodeString, Font* inFont, const Color& inBGColor, const Color& inFGColor, const string& inDefaultImage)
{
    bool theSuccess = false;

    // Parse inNodeString to get node offsets and node name 
    StringVector theTokens;
    string theDelimiters("/");
    Tokenizer::split(inNodeString, theDelimiters, theTokens);

    if(theTokens.size() > 1)
    {
        // Grab directional indicies
        typedef vector<int> DirectionalListType;
        DirectionalListType theDirectionalList;
        
        // For each direction until the last, get the sub node (skip the last two nodes, should be the node name and msg id)
        for(StringVector::iterator theStringIter = theTokens.begin(); ((theStringIter != theTokens.end()) && (theStringIter + 2 != theTokens.end())) ; theStringIter++)
        {
            string theCurrentToken = *theStringIter;
            int theCurrentOffset;
            if(sscanf(theCurrentToken.c_str(), "%d", &theCurrentOffset) == 1)
            {
                theDirectionalList.push_back(theCurrentOffset);
            }
        }
        
        // Grab node name (second to last in the list)
        string theNodeName = *(theTokens.end() - 2);

        // Grab the message id (last in list)
        string theCurrentToken = *(theTokens.end() - 1);
        int theMessageID;
        if(sscanf(theCurrentToken.c_str(), "%d", &theMessageID) != 1)
        {
        }

        // Build the new node
        PieNode* theCurrentNode = this->mRoot;
        ASSERT(theCurrentNode);

        for(DirectionalListType::iterator theIter = theDirectionalList.begin(); ((theIter != theDirectionalList.end()) && (theIter + 1 != theDirectionalList.end()) && (theCurrentNode != NULL)); theIter++)
        {
            theCurrentNode = theCurrentNode->GetNodeAtAngularOffset(*theIter);
        }

        if(theCurrentNode != NULL)
        {
            int theLastOffset = *(theDirectionalList.end() - 1);

            // At this last node, add the new node with the last index
            PieNode* theNewNode = new PieNode(theNodeName, theMessageID);

            if(inFont)
            {
                theNewNode->setFont(inFont);
            }

			theNewNode->setFgColor(inFGColor);
			theNewNode->setBgColor(inBGColor);
			theNewNode->setAsMouseCapture(false);
			theNewNode->SetDefaultImage(inDefaultImage);

            // Set size to be the default size for the rest of the nodes
            //int theNodeListSizeX, theNodeListSizeY;
            //this->mRoot->getSize(theNodeListSizeX, theNodeListSizeY);
            //theNewNode->setSize(theNodeListSizeX, theNodeListSizeY);
            
            theCurrentNode->AttachNode(theNewNode, theLastOffset, this->mNodeXDistance, this->mNodeYDistance);

            // Set x/y as it's current x/y plus the node "before" it in the pie hierarchy
            int thePreviousMenuX, thePreviousMenuY;
            theCurrentNode->getPos(thePreviousMenuX, thePreviousMenuY);

            int theNewNodeRelX, theNewNodeRelY;
            theNewNode->getPos(theNewNodeRelX, theNewNodeRelY);
            theNewNode->setPos(thePreviousMenuX + theNewNodeRelX, thePreviousMenuY + theNewNodeRelY);

			// TODO: Just added this.
			theNewNode->setSize(this->mNodeXDistance*ScreenWidth(), this->mNodeYDistance*ScreenHeight());

            // Set new node's parent as root node's parent
            theNewNode->setParent(this->mRoot->getParent());

            theSuccess = true;
        }
    }

    return theSuccess;
}

void PieNodeList::ChangeNode(int inMessageID, int inNewMessageID, const string& inNewText)
{
	this->mRoot->ChangeNode(inMessageID, inNewMessageID, inNewText);
	this->RecomputeVisibleSize();
}

void PieNodeList::DisableNodesNotInMessageList(const MessageIDList& inList)
{
	this->mRoot->DisableNodesNotInMessageList(inList);
}

void PieNodeList::DisableNodesWhoseChildrenAreDisabled()
{
	this->mRoot->DisableNodesWhoseChildrenAreDisabled();
}

PieNode* PieNodeList::GetRoot()
{
    return this->mRoot;
}

bool PieNodeList::GetSelectedNode(PieNode*& outNode)
{
	return this->mRoot->GetSelectedNode(outNode);
}

void PieNodeList::RecomputeVisibleSize(void)
{
	int theMaxWidth  = 0;
	int theMaxHeight = 0;

	// run through all pie nodes and find the max text extents of any of them
	this->mRoot->GetMaxTextSize(theMaxWidth, theMaxHeight);

	// set all sizes to accomodate that max size
	//this->mRoot->SetSizeKeepCenter(theMaxWidth, theMaxHeight);
	this->mRoot->SetVisibleSize(theMaxWidth, theMaxHeight);
}

void PieNodeList::ResetToDefaults()
{
	this->mRoot->ResetToDefaults();
}

void PieNodeList::SetBasePosition(int x, int y, int inPieWidth, int inPieHeight)
{
    // Center root in middle of pie menu, take into account image size so the image is centered.
    // All children are relative to its parent, so they assume the same image width/height
    //int xImage = this->mRoot->getImageWide();
    //int yImage = this->mRoot->getImageTall();
	//int theTextWidth, theTextHeight;
	//this->mRoot->getTextSize(theTextWidth, theTextHeight);
	int theWidth, theHeight;
	this->mRoot->getSize(theWidth, theHeight);

    //this->mRoot->setPos(inPieWidth/2 - xImage/2, inPieHeight/2 - yImage/2);
	int theCurrentX, theCurrentY;
	this->mRoot->getPos(theCurrentX, theCurrentY);
	int theDeltaX = (inPieWidth/2 - theWidth/2) - theCurrentX;
	int theDeltaY = (inPieHeight/2 - theHeight/2) - theCurrentY;
    //this->mRoot->setPos(inPieWidth/2 - theWidth/2, inPieHeight/2 - theHeight/2);

	// Run through child nodes and move them all by this amount
	this->mRoot->MoveTree(theDeltaX, theDeltaY);

    // Make sure the nodes are always as big as the pie so they are free to
    // draw child nodes outside their own extents
    //this->mRoot->setSize(inPieWidth, inPieHeight);
}

void PieNodeList::SetConnectorName(const string& inConnectorName)
{
	this->mRoot->SetConnectorName(inConnectorName);
}

void PieNodeList::SetConstructionComplete()
{
}

void PieNodeList::setParent(vgui::Panel* inPanel)
{
    this->mRoot->setParent(inPanel);
}

void PieNodeList::SetNodeDistance(float inNewXDistance, float inNewYDistance)
{
    this->mRoot->SetNodeDistance(inNewXDistance, inNewYDistance);
    this->mNodeXDistance = inNewXDistance;
    this->mNodeYDistance = inNewYDistance;
}

//bool PieNodeList::SetNodeEnabled(const string& inNodeName, bool inNewState)
//{
//    return false;
//}

//void PieNodeList::SetRootVisible(bool inNewVisibility)
//{
//    // Set just the root not visibility
//}
//

//void PieNodeList::SetTolerance(int inPercentage)
//{
//    // Loop through all nodes
//        // Call SetTolerance on each
//}

void PieNodeList::SetFadeState(bool inNewFadeState)
{
    if(inNewFadeState)
    {
        this->mRoot->SetNodeAndAdjacentChildrenFadeState(true);
    }
    else
    {
        this->mRoot->SetNodeAndAllChildrenFadeState(false);
    }
}

void PieNodeList::SetSizeKeepCenter(int inWidth, int inHeight)
{
	this->mRoot->SetSizeKeepCenter(inWidth, inHeight);
}

void PieNodeList::Update(float inTime)
{
    this->mRoot->Update(inTime);
}

void PieNodeList::UpdateMenuFromTech(const AvHTechTree& inMenuCosts, int inPurchaseLevel)
{
	this->mRoot->UpdateMenuFromTech(inMenuCosts, inPurchaseLevel);
}

void PieNodeList::VidInit()
{
	this->mRoot->VidInit();
}
