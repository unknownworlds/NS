
#include "ui/UIFactory.h"
#include "ui/UIComponents.h"
#include "ui/UIPieMenu.h"

UIComponent* UIFactory::BuildComponent(const TRDescription& inTextRep, CSchemeManager* inSchemeManager)
{
	UIComponent* theNewComp = NULL;
	string theCompType = inTextRep.GetType();

	// Switch on the type to see what kind of component to create
	if(theCompType == "Label")
	{
		theNewComp = new UILabel();
	}
	if(theCompType == "FadingImageLabel")
	{
		theNewComp = new UIFadingImageLabel();
	}
	if(theCompType == "MarqueeComponent")
	{
		theNewComp = new UIMarqueeComponent();
	}
    else if(theCompType == "ImageLabel")
    {
        theNewComp = new UIImageLabel();
    } 
    else if(theCompType == "ProgressBar")
    {
        theNewComp = new UIProgressBar();
    }
    else if(theCompType == "TreeFolder")
    {
        theNewComp = new UITreeFolder();
    }
    else if(theCompType == "ConfigWizard")
    {
        theNewComp = new UIConfigWizard();
    }
    else if(theCompType == "Menu")
    {
        theNewComp = new UIMenu();
    }
    else if(theCompType == "PopupMenu")
    {
        theNewComp = new UIPopupMenu();
    }
    else if(theCompType == "ImagePanel")
    {
        theNewComp = new UIImagePanel();
    }
    else if(theCompType == "Button")
    {
        theNewComp = new UIButton();
    } 
	#ifndef AVH_MAPPER_BUILD
    else if(theCompType == "PieMenu")
	{
		theNewComp = new UIPieMenu();
	}
	#endif
    else if(theCompType == "ColoredPanel")
	{
		theNewComp = new UIColoredPanel();
	}
    else if(theCompType == "InvisiblePanel")
	{
		theNewComp = new UIInvisiblePanel();
	}
    else if(theCompType == "Slider")
	{
		theNewComp = new UISlider();
	}
    else if(theCompType == "Slider2")
	{
		theNewComp = new UISlider2();
	}
    else if(theCompType == "SliderPlus")
	{
		theNewComp = new UISliderPlus();
	}
    else if(theCompType == "StaticLabel")
	{
		theNewComp = new UIStaticLabel();
	}
    else if(theCompType == "SpritePanel")
	{
		theNewComp = new UISpritePanel();
	}
    else if(theCompType == "DummyPanel")
	{
		theNewComp = new UIDummyPanel();
	}
	
	// TODO: More cases here

	// If we recognized the type, initialize it and return it
	if(theNewComp)
	{
		theNewComp->AllocateAndSetProperties(inTextRep, inSchemeManager);
	}

	return theNewComp;
}

