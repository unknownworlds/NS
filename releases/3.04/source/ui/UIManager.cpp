#include "ui/UIComponent.h"
#include "ui/UIManager.h"
#include "ui/UIFactory.h"
#include "ui/UITags.h"
#include "vgui_Menu.h"
#include "vgui_App.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "textrep/TRFactory.h"
#include "cl_dll/vgui_SchemeManager.h"
#include "vgui_TextPanel.h"
#include "vgui_Label.h"
#include "cl_dll/vgui_TeamFortressViewport.h"
#include "ui/GammaAwareComponent.h"
#include "ui/ReloadableComponent.h"

//using vgui::Label;
//extern CImageLabel *gTestLabel;

extern vgui::BitmapTGA *vgui_LoadTGA( const char* pImageName );

const int kTranslation = 1000;

// Stupid messy externs
extern "C"
{
    void* VGui_GetPanel();
}
extern int g_iVisibleMouse;

UIManager::UIManager(UIFactory* inFactory)
{
    this->mEditMode = false;
    this->mDraggingLMB = false;
	this->mUsingVGUI = false;

    this->mLMBDownX = this->mLMBDownY = -1;
    this->mLastMouseX = this->mLastMouseY = -1;

    this->mComponentMouseOver = NULL;
	this->mBlankCursor = NULL;

    this->mFactory = inFactory;
	this->mGammaSlope = 1.0f;
}

UIManager::~UIManager(void)
{
    delete this->mFactory;
    this->mFactory = NULL;

	delete this->mBlankCursor;
	this->mBlankCursor = NULL;
}

void UIManager::AddInputSignal(InputSignal* inInputSignal)
{
	UIComponentListType::iterator theCompIter;

	for(theCompIter = this->mComponentList.begin(); theCompIter != this->mComponentList.end(); theCompIter++)
    {
        UIComponent* theComponent = *theCompIter;
        ASSERT(theComponent);
        Panel* theVGUIComponent = theComponent->GetComponentPointer();
        ASSERT(theVGUIComponent);
        theVGUIComponent->addInputSignal(inInputSignal);
    }
}

bool UIManager::Clear(void)
{
	bool theSuccess = false;

    // Make sure we aren't in edit mode
    if(!this->mEditMode)
    {
        // Delete every component in the list
        UIComponentListType::iterator theCompIter;
        for(theCompIter = this->mComponentList.begin(); theCompIter != this->mComponentList.end(); theCompIter++)
        {
            delete *theCompIter;
            theSuccess = true;
        }
        
        // Delete the list
        this->mComponentList.clear();
    }
    else
    {
        // TODO: Emit error indicating manager can't be cleared in edit mode
    }

	return theSuccess;
}

UIComponent* UIManager::GetComponentFromPanel(Panel* inPanel)
{
    UIComponent* theComponent = NULL;

	UIComponentListType::iterator theCompIter;
	for(theCompIter = this->mComponentList.begin(); theCompIter != this->mComponentList.end(); theCompIter++)
	{
		if((*theCompIter)->GetComponentPointer() == inPanel)
		{
            theComponent = *theCompIter;
			break;
		}
	}
    return theComponent;
}

UIComponent* UIManager::GetComponentNamed(const string& inName)
{
	UIComponent* theResult = NULL;

	UIComponentListType::iterator theCompIter;
	for(theCompIter = this->mComponentList.begin(); theCompIter != this->mComponentList.end(); theCompIter++)
	{
		if((*theCompIter)->GetName() == inName)
		{
			theResult = *theCompIter;
			break;
		}
	}

	return theResult;
}

const UIComponent* UIManager::GetComponentNamed(const string& inName) const
{
	return NULL;
}

// TODO: Add list of components that are "hidden" and fail if the component is already hidden
bool UIManager::HideComponent(const string& inName)
{
	bool theSuccess = false;
	vgui::Panel* thePanel = NULL;

	if(this->GetVGUIComponentNamed(inName, thePanel))
	{
		ASSERT(thePanel != NULL);
		//this->TranslateComponent(thePanel, true);
		thePanel->setVisible(false);
		theSuccess = true;
	}

	return theSuccess;
}

void UIManager::HideComponents()
{
	typedef vector<UIComponent*> UIComponentListType;

	for(UIComponentListType::iterator theIter = this->mComponentList.begin(); theIter != this->mComponentList.end(); theIter++)
	{
		this->HideComponent((*theIter)->GetName());
	}
}

bool UIManager::Initialize(const TRDescriptionList& inDesc, CSchemeManager* inSchemeManager)
{
	bool theSuccess = false;

	// Clear out everything in case we have already been used once
	this->Clear();

	// Now loop through entities found 
	for(TRDescriptionList::const_iterator theListIter = inDesc.begin(); theListIter != inDesc.end(); theListIter++)
	{
		// See if the factory knows how to create such a thing. It is giving the memory to us forever so take care of it.
		UIComponent* theCurrentComponent = this->mFactory->BuildComponent(*theListIter, inSchemeManager);

		// Tell it to set all the tags it knows about
		if(theCurrentComponent)
		{
			// Check for named root tag, look up that component and set it
            Panel* theRoot = NULL;
            string theRootName;
            
            if(theListIter->GetTagValue("root", theRootName))
            {
                UIComponent* theUIComponent = NULL;
                theUIComponent = this->GetComponentNamed(theRootName);
                if(theUIComponent)
                {
                    theRoot = theUIComponent->GetComponentPointer();
                }
            }

            // If none specified or it couldn't be found, use default
            if(!theRoot)
            {
                theRoot = (Panel*)VGui_GetPanel();
            }

            // Set the root
   			theCurrentComponent->GetComponentPointer()->setParent(theRoot);

            // Add to menu if specified
            string theMenuName;
            if(theListIter->GetTagValue(UITagMenuAddItem, theMenuName))
            {
                Menu* theParentMenu = NULL;
                if(this->GetVGUIComponentNamed(theMenuName, theParentMenu))
                {
                    theParentMenu->addMenuItem(theCurrentComponent->GetComponentPointer());
                }
            }
            
            // Set up scheme if specified
            if(inSchemeManager)
            {
                this->SetSchemeValues(*theListIter, theCurrentComponent, inSchemeManager);
            }

            // <sigh> If we are currently using the regular VGUI instead of the manager, translate
			// this component out of the way so it doesn't suck up input
			if(this->mUsingVGUI)
			{
				this->TranslateComponent(theCurrentComponent->GetComponentPointer(), true);
			}
			
			// If gamma aware, tell it immediately
			GammaAwareComponent* theGammaAwareComponent = dynamic_cast<GammaAwareComponent*>(theCurrentComponent->GetComponentPointer());
			if(theGammaAwareComponent)
			{
				theGammaAwareComponent->NotifyGammaChange(this->mGammaSlope);
			}

            // Save it. It is now part of the world.
			this->mComponentList.push_back(theCurrentComponent);
			
			// Return success if we found at least one component
			theSuccess = true;
		}
	}
	
	// Build default blank cursor
	this->mBlankCursor = new Cursor(vgui_LoadTGA("blank"), 0, 0);
	
    // Register for notification for all input events
    //this->AddInputSignal(this);
	
	return theSuccess;
}

bool UIManager::InMouseMode(void) const
{
    return (g_iVisibleMouse ? true : false);
}

void UIManager::NotifyGammaChange(float inGammaSlope)
{
	UIComponentListType::iterator theCompIter;
	for(theCompIter = this->mComponentList.begin(); theCompIter != this->mComponentList.end(); theCompIter++)
    {
		GammaAwareComponent* theGammaAwareComponent = dynamic_cast<GammaAwareComponent*>((*theCompIter)->GetComponentPointer());
		if(theGammaAwareComponent)
		{
			theGammaAwareComponent->NotifyGammaChange(inGammaSlope);
		}
    }

	this->mGammaSlope = inGammaSlope;
}

bool UIManager::Save(const string& outFilename, const string& outHeader)
{
    // Build description list
    TRDescriptionList theDescriptionList;

    UIComponentListType::iterator theCompIter;
    for(theCompIter = this->mComponentList.begin(); theCompIter != this->mComponentList.end(); theCompIter++)
    {
        theDescriptionList.push_back((*theCompIter)->GetDescription());
    }

    // Write it out!
    TRFactory::WriteDescriptions(outFilename, theDescriptionList, outHeader);

	return true;
}

bool UIManager::SetLMBActionAbsolute(const TRTag& inTag)
{
	return true;
}

bool UIManager::SetLMBActionRelative(const TRTag& inTag)
{
	return true;
}

void UIManager::SetMouseVisibility(bool inState)
{
    // To change whether the mouse is visible, just change this variable
    g_iVisibleMouse = inState;

    // Update cursor
    if(g_iVisibleMouse)
    {
        //ClientCmd("say Entering mouse mode.");
        //App::getInstance()->setCursorOveride(App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_arrow));
		// Remove above line and put this line back in for sprite cursors
        App::getInstance()->setCursorOveride(this->mBlankCursor);
    }
    else
    {
        //ClientCmd("say Exiting mouse mode.");
        // Move mouse to center of screen so mouse look isn't changed

        // Only do this when in full screen
        App::getInstance()->setCursorPos(ScreenWidth()/2, ScreenHeight()/2);

        // Hide cursor again
        App::getInstance()->setCursorOveride( App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_none) );
    }

    //App::getInstance()->setCursorOveride( App::getInstance()->getScheme()->getCursor(Scheme::SchemeCursor::scu_none) );

}

// Set up default scheme values if a scheme was specified (overrides other tags specified)
void UIManager::SetSchemeValues(const TRDescription& inDesc, UIComponent* inComponent, CSchemeManager* inSchemeManager)
{
    std::string theSchemeName;
    if(inDesc.GetTagValue(UITagScheme, theSchemeName))
    {
        // Get the vgui panel inside
        Panel* thePanelPointer = inComponent->GetComponentPointer();

        // Get the scheme specified in the layout
        const char* theSchemeCString = theSchemeName.c_str();
        SchemeHandle_t theSchemeHandle = inSchemeManager->getSchemeHandle(theSchemeCString);
        int r, g, b, a;
        
        // Set fg color
        inSchemeManager->getFgColor(theSchemeHandle, r, g, b, a);
        thePanelPointer->setFgColor(r, g, b, a);

        // Set bg color
        inSchemeManager->getBgColor(theSchemeHandle, r, g, b, a);
        thePanelPointer->setBgColor(r, g, b, a);

        // Set font if applicable
    	vgui::Font* theFont = inSchemeManager->getFont(theSchemeHandle);
        vgui::TextPanel* theTextPanel = dynamic_cast<vgui::TextPanel*>(thePanelPointer);
        if(theFont && theTextPanel)
        {
            theTextPanel->setFont(theFont);
        }
    }
}

bool UIManager::SetRMBActionAbsolute(const TRTag& inTag)
{
	return true;
}

bool UIManager::SetRMBActionRelative(const TRTag& inTag)
{
	return true;
}

void UIManager::SetUsingVGUI(bool inState)
{
	if(inState)
	{
		if(!this->mUsingVGUI)
		{
			// Translate all components away
			this->TranslateComponents(true);
			this->mUsingVGUI = true;
		}
	}
	else
	{
		if(this->mUsingVGUI)
		{
			// Translate everything back
			this->TranslateComponents(false);
			this->mUsingVGUI = false;
		}
	}
}

bool UIManager::TranslateComponent(const string& inName, bool inAway)
{
	bool            theSuccess = false;

	UIComponent*    theComponent = this->GetComponentNamed(inName);
	if(theComponent)
	{
		this->TranslateComponent(theComponent->GetComponentPointer(), inAway);
		theSuccess = true;
	}
	return theSuccess;
	
}

void UIManager::TranslateComponent(vgui::Panel* inPanel, bool inAway)
{
	int theX, theY;
	inPanel->getPos(theX, theY);
	int theAmount = kTranslation*(inAway ? 1 : -1);
	inPanel->setPos(theX + theAmount, theY + theAmount);
}

void UIManager::TranslateComponents(bool inAway)
{
    UIComponentListType::iterator theCompIter;
    for(theCompIter = this->mComponentList.begin(); theCompIter != this->mComponentList.end(); theCompIter++)
	{
		vgui::Panel* theCurrentPanel = (*theCompIter)->GetComponentPointer();
		this->TranslateComponent(theCurrentPanel, inAway);
	}
}

bool UIManager::ToggleEditMode(void)
{
    bool theWasInEditMode = this->mEditMode;

    this->mEditMode = !this->mEditMode;
    
//    if(this->mEditMode)
//    {
//        ClientCmd("say Entering edit mode.");
//    } 
//    else
//    {
//        ClientCmd("say Exiting edit mode.");
//    }
    
    // If we enter edit mode, disable all components.  If we leave, reenable them.
    this->SetEnabledState(!this->mEditMode);
    
    //CLIENT_PRINTF( pEntity, print_console, UTIL_VarArgs( "\"fov\" is \"%d\"\n", (int)GetClassPtr((CBasePlayer *)pev)->m_iFOV ) );
    if(!this->mEditMode)
    {
        // Reset graphical layout variables
        this->mComponentMouseOver = NULL;
        this->mDraggingLMB = false;
        this->mLastMouseX = this->mLastMouseY = -1;
        this->mLMBDownX = this->mLMBDownY = -1;
    }

    return theWasInEditMode;
}

// TODO: Add list of components that are "hidden" and fail if the component is not currently hidden
bool UIManager::UnhideComponent(const string& inName)
{
	bool theSuccess = false;
	vgui::Panel* thePanel = NULL;

	if(this->GetVGUIComponentNamed(inName, thePanel))
	{
		ASSERT(thePanel != NULL);
		//this->TranslateComponent(thePanel, false);
		thePanel->setVisible(true);
		theSuccess = true;
	}

	return theSuccess;
}

void UIManager::Update(float inCurrentTime)
{
	UIComponentListType::iterator theCompIter;
	for(theCompIter = this->mComponentList.begin(); theCompIter != this->mComponentList.end(); theCompIter++)
    {
        (*theCompIter)->Update(inCurrentTime);
    }

//    int r, g, b, a;
//    r = g = b = a = 0;
//    //gTestLabel->getFgColor(r, g, b, a);
//    vgui::Color theColor;
//    gTestLabel->m_pTGA->getColor(theColor);
//    theColor.getColor(r, g, b, a);
//    r = (r + 1) % 255;
//    theColor.setColor(r, g, b, a);
//    gTestLabel->m_pTGA->setColor(theColor);
//    //gTestLabel->setFgColor(r, g, b, a);
//
//    gTestLabel->getBgColor(r, g, b, a);
//    a = (a + 1) % 255;
//    gTestLabel->setBgColor(r, g, b, a);
}

void UIManager::VidInit(void)
{
	UIComponentListType::iterator theCompIter;
	for(theCompIter = this->mComponentList.begin(); theCompIter != this->mComponentList.end(); theCompIter++)
    {
		ReloadableComponent* theReloadableComponent = dynamic_cast<ReloadableComponent*>((*theCompIter)->GetComponentPointer());
		if(theReloadableComponent)
		{
			theReloadableComponent->VidInit();
		}
    }
}


void UIManager::ToggleMouse(void)
{
	this->SetMouseVisibility(!g_iVisibleMouse);
}

////////////////////////////////
// Functions from InputSignal //
////////////////////////////////
void UIManager::cursorMoved(int inX, int inY, Panel* inPanel)
{
    if(!inPanel)
    {
        ClientCmd("say cursorMoved with null inPanel!  Yeah!\n");
    }

    // x,y are local to the upper left of the panel. 
    int theLocalX = inX;
    int theLocalY = inY;

    // Get screen coordinates
    int theScreenX, theScreenY;
    this->GetScreenCoords(theScreenX, theScreenY);

    // This function should only be called when mouse visible and we are in edit mode
    if(this->mEditMode)
    {
        // We can't assume that a cursorEntered will always get called before a cursor moved, so 
        // call cursorEntered if we don't have a component yet (happens when toggling between the two modes
        if(!this->mComponentMouseOver)
        {
            this->cursorEntered(inPanel);
        }
        
        // Number of pixels to move with the mouse button down before we start dragging
        const int START_DRAG_PIXEL_DIST = 6;
        const int MIN_DRAG_PIXEL_DIST = 4;
        static int theStartDragLocalXOffset = 0;
        static int theStartDragLocalYOffset = 0;
        
        // Check how far we have moved, are we dragging yet?
        if((!this->mDraggingLMB) &&
            (this->mLMBDownX != -1) && 
            (this->mLMBDownY != -1))
        {
            int theXDiff = theScreenX - this->mLMBDownX;
            int theYDiff = theScreenY - this->mLMBDownY;
            
            // Requires more movement in diagonal direction, probably not
            // worth the extra complexity to change
            if((abs(theXDiff) > START_DRAG_PIXEL_DIST) ||
                (abs(theYDiff) > START_DRAG_PIXEL_DIST))
            {
                //ClientCmd("say Starting drag");
                
                // Save the offset from the component's upper left.  Preserve
                // this when for more intuitive dragging (so dragging doesn't
                // suddenly move the component relative to the mouse).
                theStartDragLocalXOffset = theLocalX;
                theStartDragLocalYOffset = theLocalY;

                //inPanel->setAsMouseArena(true);
                
                this->mDraggingLMB = true;
            }
        }
        
        // Are we dragging?
        if(this->mDraggingLMB)
        {
            // If so, set the component's new position. The position corresponds to the upper
            // left corner of the component, so subtract out the local offset.
            // The component's new position is equal to the current screen pos of the mouse, MINUS
            // the local component offset which we started the drag.  If we were dragging the component
            // around by the upper left corner, this would be like setting the component's position equal
            // to whatever the mouse was at during a mouse move.  If we were dragging it by the center,
            // it would set the component's position to the current screen mouse pos minus half the component
            // width and height. Make sense?
            //ClientCmd("say Setting new component position");
            int theChangeX = theScreenX - theStartDragLocalXOffset;
            int theChangeY = theScreenY - theStartDragLocalYOffset;

            // Make sure we move at least a few pixels, because we're bound by control and we 
            // could've grabbed it near one of the edges
            theChangeX = max(theChangeX, MIN_DRAG_PIXEL_DIST);
            theChangeY = max(theChangeY, MIN_DRAG_PIXEL_DIST);
            this->SetPanelPosition(inPanel, theChangeX, theChangeY);
        }
    }

    // Update new mouse position
    this->mLastMouseX = theScreenX;
    this->mLastMouseY = theScreenY;
}


void UIManager::SetPanelPosition(Panel* inPanel, int inX, int inY)
{
    // Look up the component
    UIComponent* theUIComp = this->GetComponentFromPanel(inPanel);
    if(theUIComp)
    {
        // Update run-time version
        // Set the vgui panel's position (pixel coords)
        // Clip so component never goes off the screen in any way
        int theCompWidth = 0;
        int theCompHeight = 0;
        inPanel->getSize(theCompWidth, theCompHeight);

        int theClippedScreenX = max(min(inX, ScreenWidth() - theCompWidth), 0);
        int theClippedScreenY = max(min(inY, ScreenHeight() - theCompHeight), 0);
        inPanel->setPos(theClippedScreenX, theClippedScreenY);
        
        // repaint parent if present
        Panel* inPanelParent = inPanel->getParent();
        if(inPanelParent != NULL)
        {			
            inPanelParent->repaint();
        }

        // Convert to normalized coords for text representation.
        float theClippedNormX = (float)theClippedScreenX/ScreenWidth();
        float theClippedNormY = (float)theClippedScreenY/ScreenHeight();

        // Change the text represntation so it has the update coordinates (normalized coords)
        // This means it can save out again with the player's changes!
        TRDescription& theCompDesc = theUIComp->GetDescription();
        theCompDesc.SetTagValue(UITagXPos, theClippedNormX);
        theCompDesc.SetTagValue(UITagYPos, theClippedNormY);

    }
    else
    {
        //ClientCmd("say Can't find UIComponent that you're dragging, can't save changes to it.");
    }
}

void UIManager::cursorEntered(Panel* inPanel)
{
    // Handle stacking of ui components.  Only enter component if we aren't already on a component
    if(!this->mComponentMouseOver)
    {
        if(!this->mDraggingLMB)
        {
            //ClientCmd("say Cursor entered component");
            this->mComponentMouseOver = inPanel;
        }
    }
}

void UIManager::cursorExited(Panel* inPanel)
{
    // Only leave the component we are currently on for stacking purposes
    if(this->mComponentMouseOver == inPanel)
    {
        if(!this->mDraggingLMB)
        {
            //ClientCmd("say Cursor exited component");
            this->mComponentMouseOver = NULL;
        }
    }
}

void UIManager::mousePressed(MouseCode inCode, Panel* inPanel)
{
    // Track dragging state
    if(inCode == MOUSE_LEFT)
    {
        if(this->mComponentMouseOver)
        {
            //ClientCmd("say Left mouse pressed on component");
            this->mLMBDownX = this->mLastMouseX;
            this->mLMBDownY = this->mLastMouseY;
        }
    }
}

void UIManager::mouseDoublePressed(MouseCode inCode, Panel* inPanel)
{
}

// Take into account the mouse offset into the component 
// Note: This doesn't work in windowed mode, as it returns the cursor position in pixels,
// and the desktop resolution is different than HL resolution in windowed mode.  If there
// are function to tell if we are in windowed mode, and to find the upper left x,y this
// could be fixed.
void UIManager::GetScreenCoords(int& outLocalX, int& outLocalY)
{
    int theX, theY;
    App::getInstance()->getCursorPos(theX, theY);

//    int theLocalX = theX;
//    int theLocalY = theY;
//	inPanel->screenToLocal(theLocalX, theLocalY);

//    theX += ioLocalX;
//    theY += ioLocalY;

    outLocalX = theX;
    outLocalY = theY;
}

void UIManager::SetEnabledState(bool inState)
{
    UIComponentListType::iterator theCompIter;
    for(theCompIter = this->mComponentList.begin(); theCompIter != this->mComponentList.end(); theCompIter++)
    {
        (*theCompIter)->GetComponentPointer()->setEnabled(inState);
    }
}


void UIManager::mouseReleased(MouseCode code, Panel* inPanel)
{
    // Track dragging state
    if(code == MOUSE_LEFT)
    {
        //ClientCmd("say Left mouse released on component");

        //ALERT(at_console, "Down xy = -1\n");
        this->mLMBDownX = -1;
        this->mLMBDownY = -1;
        this->mDraggingLMB = false;

        //App::getInstance()->setMouseArena(NULL);
        //inPanel->setAsMouseArena(false);
        //App::getInstance()->setMouseArena(NULL);
    }
}

void UIManager::mouseWheeled(int delta,Panel* panel)
{
}

void UIManager::keyPressed(KeyCode code,Panel* panel)
{
}

void UIManager::keyTyped(KeyCode code,Panel* panel)
{
}

void UIManager::keyReleased(KeyCode code,Panel* panel)
{
}

void UIManager::keyFocusTicked(Panel* panel)
{
}

