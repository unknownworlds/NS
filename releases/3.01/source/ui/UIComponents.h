#ifndef UICOMPONENTS_H
#define UICOMPONENTS_H

#include "ui/UIComponent.h"
#include "textrep/TRDescription.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "cl_dll/vgui_TeamFortressViewport.h"
#include "VGUI_ProgressBar.h"
#include "VGUI_TreeFolder.h"
#include "VGUI_ConfigWizard.h"
#include "VGUI_Menu.h"
#include "VGUI_PopupMenu.h"
#include "VGUI_ImagePanel.h"
#include "VGUI_Slider.h"
#include "ui/SliderPlus.h"
#include "ui/FadingImageLabel.h"
#include "ui/MarqueeComponent.h"
#include "ui/ColoredPanel.h"
#include "ui/InvisiblePanel.h"
#include "ui/StaticLabel.h"
#include "ui/SpritePanel.h"
#include "ui/DummyPanel.h"
#include "game_shared/vgui_slider2.h"
class CSchemeManager;

using namespace vgui;

// This macro generates the classes used for all the vgui components.
// The key here is that the most derived components can override parent
// values.  Each new component must define an AllocateComponent() function
// and a SetClassProperties() function.  SetClassProperties must only read
// tags that its parent doesn't know about, or those that it wishes to override.

#define DEFINE_UICOMPONENT(className, parentName, typeName, vguiName) \
class className : public parentName \
{ \
public: \
	className(void) \
		{ \
            this->mType = typeName; \
            this->m##className = NULL; \
        } \
\
    virtual ~className(void) \
        { \
            delete this->m##className; \
            this->m##className = NULL; \
        } \
\
	virtual Panel* GetComponentPointer(void) \
		{ return this->m##className; } \
\
	virtual const string& GetType(void) const \
		{ return this->mType; } \
\
    virtual bool SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager); \
\
	virtual void Update(float theCurrentTime); \
\
private: \
\
	virtual void AllocateComponent(const TRDescription& inDescription); \
\
	vguiName* m##className; \
};


// Kinda ugly but good way to generate all these class such that they are defined to  
// call each other correctly.
DEFINE_UICOMPONENT(UIPanel, UIComponent, "Panel", Panel)
DEFINE_UICOMPONENT(UILabel, UIPanel, "Label", Label)
DEFINE_UICOMPONENT(UIImageLabel, UILabel, "ImageLabel", CImageLabel)
DEFINE_UICOMPONENT(UIProgressBar, UIPanel, "ProgressBar", ProgressBar)
DEFINE_UICOMPONENT(UIConfigWizard, UIPanel, "ConfigWizard", ConfigWizard)
DEFINE_UICOMPONENT(UITreeFolder, UIPanel, "TreeFolder", TreeFolder)
DEFINE_UICOMPONENT(UIMenu, UIPanel, "Menu", Menu)
DEFINE_UICOMPONENT(UIPopupMenu, UIMenu, "PopupMenu", PopupMenu)
DEFINE_UICOMPONENT(UIImagePanel, UIPanel, "ImagePanel", ImagePanel)
DEFINE_UICOMPONENT(UIButton, UILabel, "Button", Button)
DEFINE_UICOMPONENT(UIFadingImageLabel, UIPanel, "FadingImageLabel", FadingImageLabel)
DEFINE_UICOMPONENT(UIMarqueeComponent, UIPanel, "MarqueeComponent", MarqueeComponent)
DEFINE_UICOMPONENT(UIColoredPanel, UIPanel, "ColoredPanel", ColoredPanel)
DEFINE_UICOMPONENT(UIInvisiblePanel, UIPanel, "InvisiblePanel", InvisiblePanel)
DEFINE_UICOMPONENT(UISlider, UIPanel, "Slider", Slider)
DEFINE_UICOMPONENT(UISlider2, UIPanel, "Slider2", Slider2)
DEFINE_UICOMPONENT(UISliderPlus, UIPanel, "SliderPlus", SliderPlus)
DEFINE_UICOMPONENT(UIStaticLabel, UILabel, "StaticLabel", StaticLabel)
DEFINE_UICOMPONENT(UISpritePanel, UIPanel, "SpritePanel", SpritePanel)
DEFINE_UICOMPONENT(UIDummyPanel, UIPanel, "DummyPanel", DummyPanel)

#endif
