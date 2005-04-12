//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: UIPieMenu.cpp $
// $Date: 2002/08/16 02:29:20 $
//
//-------------------------------------------------------------------------------
// $Log: UIPieMenu.cpp,v $
// Revision 1.9  2002/08/16 02:29:20  Flayra
// - Added document headers
// - Started to add support for pie nodes with both images and text
//
//===============================================================================
#include "ui/UIPieMenu.h"
#include "ui/UIUtil.h"
#include "ui/UITags.h"
#include "ui/PieMenu.h"

const string kPieMenuNodeXDistance("nodexspacing");
const string kPieMenuNodeYDistance("nodeyspacing");
const string kNodeTarga("nodeimage");
const string kPieMenuNodePrefix("node");
const string kPieMenuDefaultImage("defaultimage");

UIPieMenu::UIPieMenu(void)
{
    this->mType = "PieMenu";
}

// Destructor automatically removes component from the engine
UIPieMenu::~UIPieMenu(void)
{
}

void UIPieMenu::AllocateComponent(const TRDescription& inDesc)
{
    float theXPos, theYPos;
    int theWidth, theHeight;
    string theImageName("default");

    UIGetPosition(inDesc, theXPos, theYPos, theWidth, theHeight);

	string theRootName;
	inDesc.GetTagValue(UIRootName, theRootName);

    this->mPieMenu = new PieMenu(theRootName, theXPos*ScreenWidth(), theYPos*ScreenHeight(), theWidth*ScreenWidth(), theHeight*ScreenHeight());
}

vgui::Panel* UIPieMenu::GetComponentPointer(void)
{
    return this->mPieMenu;
}

const string& UIPieMenu::GetType(void) const
{
    return this->mType;
}

// Not virtual, this is meant to act only on the class specified.  Operate on panel because that is
// the lowest level of hierarchy (not that we're going to do anything with it anyways)
bool UIPieMenu::SetClassProperties(const TRDescription& inDesc, Panel* inComponent, CSchemeManager* inSchemeManager)
{
    bool theSuccess;
	
    // Let parent classes go first
    theSuccess = UIPanel::SetClassProperties(inDesc, inComponent, inSchemeManager);
	
    if(theSuccess)
    {
        // Dynamic_cast inComponent to an PieMenu (will always succeed)
        PieMenu* thePieMenu = (PieMenu*)inComponent;
		
        // Read font if specified
        std::string theSchemeName;
        if(inDesc.GetTagValue(UITagScheme, theSchemeName))
        {
            if(thePieMenu)
            {
                const char* theSchemeCString = theSchemeName.c_str();
                SchemeHandle_t theSchemeHandle = inSchemeManager->getSchemeHandle(theSchemeCString);
                Font* theFont = inSchemeManager->getFont(theSchemeHandle);
                if(theFont)
                {
                    thePieMenu->SetFont(theFont);
					thePieMenu->GetRootNode()->setFont(theFont);
                }
            }
        }
		
		// Set colors of root node to that of the pie menu
		Color theColor;
		thePieMenu->getFgColor(theColor);
		thePieMenu->GetRootNode()->setFgColor(theColor);
		
		thePieMenu->getBgColor(theColor);
		thePieMenu->GetRootNode()->setBgColor(theColor);
		
        // Read node distances
        float theNodeXDistance = 0.0f;
        inDesc.GetTagValue(kPieMenuNodeXDistance, theNodeXDistance);
        float theNodeYDistance = 0.0f;
        inDesc.GetTagValue(kPieMenuNodeYDistance, theNodeYDistance);
        thePieMenu->SetNodeDistance(theNodeXDistance, theNodeYDistance);

		// Set pop-up menu default image
		string theDefaultImage;
		if(inDesc.GetTagValue(kPieMenuDefaultImage, theDefaultImage))
		{
			thePieMenu->SetDefaultImage(theDefaultImage);
		}
		
        // Read specified image to use
        //string theNodeTargaName;
        //if(inDesc.GetTagValue(kNodeTarga, theNodeTargaName))
        //{
        //    thePieMenu->SetNodeTargaName(theNodeTargaName);
        //}
		
        // Now read in the nodes until there are no more.  Assumes first node is root.
		thePieMenu->GetRootNode()->SetSizeKeepCenter(theNodeXDistance*ScreenWidth(), theNodeYDistance*ScreenHeight());
		
        StringVector theNodeList;
        inDesc.GetTagStringList(kPieMenuNodePrefix, theNodeList);
        for(StringVector::iterator theIter = theNodeList.begin(); theIter != theNodeList.end(); theIter++)
        {
            thePieMenu->AddNode(*theIter);
        }

		// Set the connector, if any
		string theConnectorName;
		if(inDesc.GetTagValue(UIConnectorName, theConnectorName))
		{
			if(theConnectorName != "")
			{
				this->mPieMenu->SetConnectorName(theConnectorName);
			}
		}

		// Now have the piemenu recompute visible size for all nodes
		thePieMenu->RecomputeVisibleSize();

//        for(int i = 0; ; i++)
//        {
//            char theNum[4];
//            sprintf(theNum, "%d", i);
//            string theNodeName(kPieMenuNodePrefix + string(theNum));
//
//            string theNodeString;
//            if(inDesc.GetTagValue(theNodeName, theNodeString))
//            {
//                thePieMenu->AddNode(theNodeString);
//            }
//            else
//            {
//                break;
//            }
//        }

        // Tell it we're done
        thePieMenu->SetConstructionComplete();
        
        // Make sure to propagate new position to all children.  Children may have been added
        // after the position was set so just make sure to re-set the position before we leave now
        // that all children have been added.
    }

    return theSuccess;
}

void UIPieMenu::Update(float theCurrentTime)
{
    this->mPieMenu->Update(theCurrentTime);
}
