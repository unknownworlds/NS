#ifndef UIMANAGER_H
#define UIMANAGER_H

// Main ideas: 
//   1) The full UI is specified from a text file
//   2) Changing or adding UI components doesn't require changing the layout parser
//   3) Globals aren't needed in the client code, it can look up components by name when it needs them.  This makes
//      the code much neater and easier to maintain.
// 
// Components are defined as a set of tags, many of them common for all components.  A new UI component
// can define its own custom tags if it so desires.  
// 

#include "textrep/TRTag.h"
#include "textrep/TRDescription.h"
#include "vgui_InputSignal.h"
#include "ui/UIComponent.h"

class CSchemeManager;

using namespace vgui;

class UIComponent;
class UIFactory;

// Singleton or global?
class UIManager : public InputSignal
{
public:
    // Only one way to construct.  Not meaningful to use until Initialize() is called.
					    UIManager(UIFactory* inFactory);

    // Make destructor virtual just in case this is subclassed someday
    virtual			    ~UIManager(void);

    void                AddInputSignal(InputSignal* inInputSignal);

    // Deletes all components that have been previously read in.  Clients should not hold onto UI component
    // pointers, because this function invalidates them.  This will typically be called from within VGUI_Shutdown.
    // It is also called at the beginning of Initialize().  Returns true if at least one component was deleted.
    bool			    Clear(void);

	// Find a component that has been created with the specified name.  The text files lay out the components, this
	// is how the mod finds them after they have been created.  The calling code shouldn't keep around the pointer
	// to the component, it should query it every tick in case it goes away.
	UIComponent*		GetComponentNamed(const string& inName);
	const UIComponent*	GetComponentNamed(const string& inName) const;

    // Quick way to fetch the vgui component you're looking for.  Returns null if type is wrong or it doesn't exist.
    template <class T>
    bool                GetVGUIComponentNamed(const string& inName, T& outValue)
    {
        bool            theSuccess = false;
        T               theReturnComponent = NULL;
		outValue = NULL;
        UIComponent*    theComponent = this->GetComponentNamed(inName);
        if(theComponent)
        {
            outValue = dynamic_cast<T>(theComponent->GetComponentPointer());
            if(outValue)
            {
                theSuccess = true;
            }
        }
        return theSuccess;
    }

	bool				HideComponent(const string& inName);
	void				HideComponents();

    // Load up all UI components specified by the text description.  Any components that have already been loaded 
    // are cleared first.  Returns false if the file couldn't be opened or no components could be built from the file.  
    // Typically this will be called once when the mod is initialized and never again.
    bool			    Initialize(const TRDescriptionList& inDesc, CSchemeManager* inSchemeManager = NULL);

    bool                InMouseMode(void) const;

	void				NotifyGammaChange(float inGammaSlope);

    // Saves the current UI layout back out the file that it was read in from. This is only meaningful after
    // the layout has been edited by the player.  Returns false if the file couldn't be opened, if a write fails.
    // or if the manager is currently in edit mode.
    bool			    Save(const string& outFilename, const string& outHeader);

    // Sets the left mouse button action to 
    bool                SetLMBActionAbsolute(const TRTag& inTag);
                        
    bool                SetLMBActionRelative(const TRTag& inTag);

    void                SetMouseVisibility(bool inState);
	
    bool                SetRMBActionAbsolute(const TRTag& inTag);
                        
    bool                SetRMBActionRelative(const TRTag& inTag);

	void				SetUsingVGUI(bool inState);

    // Call to toggle editing current layout with the mouse.  Starts in non-edit mode.
    // Returns true if we were in edit mode and just left.
    bool                ToggleEditMode(void);

    void                ToggleMouse(void);
	
	bool				TranslateComponent(const string& inName, bool inAway);
	
	bool				UnhideComponent(const string& inName);

    virtual void        Update(float inCurrentTime);

	// Called every time the hud is re-inited
	virtual void		VidInit(void);
	
    // Input signal functions for graphical editing
	virtual void        cursorMoved(int x,int y,Panel* panel);
	virtual void        cursorEntered(Panel* panel);
	virtual void        cursorExited(Panel* panel);
	virtual void        mousePressed(MouseCode code,Panel* panel);
	virtual void        mouseDoublePressed(MouseCode code,Panel* panel);
	virtual void        mouseReleased(MouseCode code,Panel* panel);
	virtual void        mouseWheeled(int delta,Panel* panel);
	virtual void        keyPressed(KeyCode code,Panel* panel);
	virtual void        keyTyped(KeyCode code,Panel* panel);
	virtual void        keyReleased(KeyCode code,Panel* panel);
	virtual void        keyFocusTicked(Panel* panel);

private:
    UIManager(void);

    UIComponent*        GetComponentFromPanel(Panel* inPanel);
    void                GetScreenCoords(int& ioLocalX, int& ioLocalY);
    void                SetEnabledState(bool inState);
    void                SetPanelPosition(Panel* inPanel, int inX, int inY);
    void                SetSchemeValues(const TRDescription& inDesc, UIComponent* inComponent, CSchemeManager* inManager);

	void				TranslateComponents(bool inAway);
	void				TranslateComponent(vgui::Panel* inPanel, bool inAway);
	
    bool                mEditMode;
    bool                mDraggingLMB;
	bool				mUsingVGUI;

    int                 mLastMouseX, mLastMouseY;
    int                 mLMBDownX, mLMBDownY;
    Panel*              mComponentMouseOver;
	Cursor*				mBlankCursor;

    UIFactory*          mFactory;
	float				mGammaSlope;

    // List of text representations associated with UIComponents.
    // These are all currently loaded in the game.
	typedef vector<UIComponent*> UIComponentListType;
	UIComponentListType mComponentList;

};

#endif

