#include "ui/UIComponents.h"
#include "ui/UITags.h"
#include "ui/UIUtil.h"

//extern "C"
//{
//   void* VGui_GetPanel();
//}


//////////////////////////
// Panel -> UIComponent //
//////////////////////////
void UIPanel::AllocateComponent(const TRDescription& inDescription)
{
    this->mUIPanel = new Panel();
}

bool UIPanel::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    bool theArgOne, theArgTwo;

    UIComponent::SetClassProperties(inDesc, inPanel, inSchemeManager);

    // Position (normalized screen coords)
    float theXPos = UIDefaultXPos;
    float theYPos = UIDefaultYPos;
    theArgOne = inDesc.GetTagValue(UITagXPos, theXPos);
    theArgTwo = inDesc.GetTagValue(UITagYPos, theYPos);
    if(theArgOne || theArgTwo)
    {
        inPanel->setPos(theXPos*ScreenWidth(), theYPos*ScreenHeight());
    }

    // Width and height (normalized screen coords)
    float theWidth = UIDefaultWidth;
    float theHeight = UIDefaultHeight;
    theArgOne = inDesc.GetTagValue(UITagWidth, theWidth);
    theArgTwo = inDesc.GetTagValue(UITagHeight, theHeight);
    if(theArgOne || theArgTwo)
    {
        inPanel->setSize(theWidth*ScreenWidth(), theHeight*ScreenHeight());
    }

    // Preferred size (normalized screen coords)
    float thePreferredWidth = UIDefaultPreferredWidth;
    float thePreferredHeight = UIDefaultPreferredHeight;
    theArgOne = inDesc.GetTagValue(UITagPreferredWidth, thePreferredWidth);
    theArgTwo = inDesc.GetTagValue(UITagPreferredHeight, thePreferredHeight);
    if(theArgOne || theArgTwo)
    {
        inPanel->setPreferredSize(thePreferredWidth*ScreenWidth(), thePreferredHeight*ScreenHeight());
    }

    // Background color (rgba)
    string theColorString;
    if(inDesc.GetTagValue(UITagBGColor, theColorString))
    {
        Color theColor;
        UIStringToColor(theColorString, theColor);
        inPanel->setBgColor(theColor);
    }

    // Foreground color (rgba)
    if(inDesc.GetTagValue(UITagFGColor, theColorString))
    {
        Color theColor;
        UIStringToColor(theColorString, theColor);
        inPanel->setFgColor(theColor);
    }

    // Default visibility (bool)
    bool theTempBool = UIDefaultVisibility;
    if(inDesc.GetTagValue(UITagVisible, theTempBool))
    {
        inPanel->setVisible(theTempBool);
    }

    // Remember the comp name
    this->SetName(inDesc.GetName());

    return true;
}

void UIPanel::Update(float theCurrentTime)
{
	UIComponent::Update(theCurrentTime);
}


////////////////////
// Label -> Panel //
////////////////////
void UILabel::AllocateComponent(const TRDescription& inDesc)
{
	string theTitle;
    inDesc.GetTagValue(UITagText, theTitle);

    string theDefaultText;
	LocalizeString(theTitle.c_str(), theDefaultText);
	
    this->mUILabel = new Label(theDefaultText.c_str());
}

bool UILabel::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    bool theSuccess = true;
	
    Label* theLabel = (Label*)inPanel;
	
    theSuccess = UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);
    
    if(theSuccess)
    {
        string theAlignmentString;
        if(inDesc.GetTagValue(UITagAlignment, theAlignmentString))
        {
            // Convert the alignment from a string to the label enum
            Label::Alignment theAlignment = vgui::Label::a_center;
            UIStringToAlignment(theAlignmentString, theAlignment);
            theLabel->setContentAlignment(theAlignment);
        }
		
        string theTitle;
        if(inDesc.GetTagValue(UITagText, theTitle))
        {
			string theDefaultText("");
			LocalizeString(theTitle.c_str(), theDefaultText);
			
			theLabel->setText((int)theDefaultText.length(), theDefaultText.c_str());
        }
		
		// Get font to use
		std::string theSchemeName;
		if(inDesc.GetTagValue(UITagScheme, theSchemeName))
		{
			const char* theSchemeCString = theSchemeName.c_str();
			SchemeHandle_t theSchemeHandle = inSchemeManager->getSchemeHandle(theSchemeCString);
			Font* theFont = inSchemeManager->getFont(theSchemeHandle);
			if(theFont)
			{
				theLabel->setFont(theFont);
			}
				
			theSuccess = true;
		}
    }
	
    return theSuccess;
}

void UILabel::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);
}


/////////////////////////
// ImageLabel -> Label //
/////////////////////////
void UIImageLabel::AllocateComponent(const TRDescription& inDesc)
{
    float theXPos, theYPos;
    int theWidth, theHeight;
    string theImageName("ru");

    UIGetPosition(inDesc, theXPos, theYPos, theWidth, theHeight);
    inDesc.GetTagValue(UITagImage, theImageName);

    // Construct it!
    this->mUIImageLabel = new CImageLabel(theImageName.c_str(), theXPos*ScreenWidth(), theYPos*ScreenHeight(), theWidth, theHeight);
}

bool UIImageLabel::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    return UILabel::SetClassProperties(inDesc, inPanel, inSchemeManager);
}

void UIImageLabel::Update(float theCurrentTime)
{
	UILabel::Update(theCurrentTime);
}



//////////////////////////
// ProgressBar -> Panel //
//////////////////////////
void UIProgressBar::AllocateComponent(const TRDescription& inDesc)
{
    int theSegmentCount = UIDefaultProgressBarSegments;

    inDesc.GetTagValue(UITagSegments, theSegmentCount);

    this->mUIProgressBar = new ProgressBar(theSegmentCount);
}

bool UIProgressBar::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    return UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);
}

void UIProgressBar::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);
}

///////////////////////////
// ConfigWizard -> Panel //
///////////////////////////
void UIConfigWizard::AllocateComponent(const TRDescription& inDesc)
{
    float theXPos, theYPos;
    int theWidth, theHeight;
    UIGetPosition(inDesc, theXPos, theYPos, theWidth, theHeight);

    this->mUIConfigWizard = new ConfigWizard(theXPos*ScreenWidth(), theYPos*ScreenHeight(), theWidth, theHeight);
}

bool UIConfigWizard::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    return UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);
}

void UIConfigWizard::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);
}



///////////////////////////
// TreeFolder -> Panel //
///////////////////////////
void UITreeFolder::AllocateComponent(const TRDescription& inDesc)
{
    string thePathName = UIDefaultPathName;
    inDesc.GetTagValue(UITagPathName, thePathName);

    this->mUITreeFolder = new TreeFolder(thePathName.c_str());
}

bool UITreeFolder::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    return UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);
}

void UITreeFolder::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);
}



///////////////////
// Menu -> Panel //
///////////////////
void UIMenu::AllocateComponent(const TRDescription& inDesc)
{
    float theXPos, theYPos;
    int theWidth, theHeight;
    UIGetPosition(inDesc, theXPos, theYPos, theWidth, theHeight);

    this->mUIMenu = new Menu(theXPos*ScreenWidth(), theYPos*ScreenHeight(), theWidth, theHeight);
}

bool UIMenu::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    return UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);
}

void UIMenu::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);
}


////////////////////////
// PopupMenu -> Panel //
////////////////////////
void UIPopupMenu::AllocateComponent(const TRDescription& inDesc)
{
    float theXPos, theYPos;
    int theWidth, theHeight;
    UIGetPosition(inDesc, theXPos, theYPos, theWidth, theHeight);

    this->mUIPopupMenu = new PopupMenu(theXPos*ScreenWidth(), theYPos*ScreenHeight(), theWidth, theHeight);
}

bool UIPopupMenu::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    return UIMenu::SetClassProperties(inDesc, inPanel, inSchemeManager);
}

void UIPopupMenu::Update(float theCurrentTime)
{
	UIMenu::Update(theCurrentTime);
}


/////////////////////////
// ImagePanel -> Panel //
/////////////////////////
void UIImagePanel::AllocateComponent(const TRDescription& inDesc)
{
    Image* theImage = NULL;

    this->mUIImagePanel = new ImagePanel(theImage);
}

bool UIImagePanel::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    return UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);
}

void UIImagePanel::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);
}


/////////////////////
// Button -> Label //
/////////////////////
void UIButton::AllocateComponent(const TRDescription& inDesc)
{
    float theXPos, theYPos;
    int theWidth, theHeight;
    UIGetPosition(inDesc, theXPos, theYPos, theWidth, theHeight);
	
	string theTitle;
    inDesc.GetTagValue(UITagText, theTitle);

    string theDefaultText;
	LocalizeString(theTitle.c_str(), theDefaultText);
	
    this->mUIButton = new Button(theDefaultText.c_str(), theXPos*ScreenWidth(), theYPos*ScreenHeight(), theWidth, theHeight);
}

bool UIButton::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    return UILabel::SetClassProperties(inDesc, inPanel, inSchemeManager);
}

void UIButton::Update(float theCurrentTime)
{
	UILabel::Update(theCurrentTime);
}

/////////////////////////////////////
// FadingImageLabel -> CImageLabel //
/////////////////////////////////////
void UIFadingImageLabel::AllocateComponent(const TRDescription& inDesc)
{
    float theXPos, theYPos;
    int theWidth = ScreenWidth();
    int theHeight = ScreenHeight();
    string theImageName("default");

    UIGetPosition(inDesc, theXPos, theYPos, theWidth, theHeight);
    inDesc.GetTagValue(UITagImage, theImageName);

    // Construct it!
    //this->mUIFadingImageLabel = new FadingImageLabel(theImageName.c_str(), theXPos*ScreenWidth, theYPos*ScreenHeight, theWidth, theHeight);
    //this->mUIFadingImageLabel = new FadingImageLabel(theImageName.c_str(), theXPos*ScreenWidth, theYPos*ScreenHeight);
    //this->mUIFadingImageLabel = new FadingImageLabel(theXPos*ScreenWidth, theYPos*ScreenHeight);
    this->mUIFadingImageLabel = new FadingImageLabel(0, 0);
}

bool UIFadingImageLabel::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    bool theSuccess = true;

    // TODO: Add special tags
    // fade time 
    // image 
    return UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);
}

void UIFadingImageLabel::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);

	this->mUIFadingImageLabel->Update(theCurrentTime);
}



void UIMarqueeComponent::AllocateComponent(const TRDescription& inDesc)
{
    float theXPos, theYPos;
    int theWidth = ScreenWidth();
    int theHeight = ScreenHeight();
	
    UIGetPosition(inDesc, theXPos, theYPos, theWidth, theHeight);
	
    // Construct it!
    this->mUIMarqueeComponent = new MarqueeComponent();
}

bool UIMarqueeComponent::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    bool theSuccess = true;
	
    // TODO: Add special tags
    // fade time 
    // image 
    return UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);
}

void UIMarqueeComponent::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);
}





///////////////////////////
// ColoredPanel -> Panel //
///////////////////////////
void UIColoredPanel::AllocateComponent(const TRDescription& inDesc)
{
    // Position (normalized screen coords)
    float theXPos = UIDefaultXPos;
    float theYPos = UIDefaultYPos;
    inDesc.GetTagValue(UITagXPos, theXPos);
    inDesc.GetTagValue(UITagYPos, theYPos);
	
    // Width and height (normalized screen coords)
    float theWidth = UIDefaultWidth;
    float theHeight = UIDefaultHeight;
    inDesc.GetTagValue(UITagWidth, theWidth);
    inDesc.GetTagValue(UITagHeight, theHeight);
	
    this->mUIColoredPanel = new ColoredPanel(theXPos*ScreenWidth(), theYPos*ScreenHeight(), theWidth*ScreenWidth(), theHeight*ScreenHeight());
}

bool UIColoredPanel::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    return UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);
}

void UIColoredPanel::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);
}

/////////////////////////////
// InvisiblePanel -> Panel //
/////////////////////////////
void UIInvisiblePanel::AllocateComponent(const TRDescription& inDesc)
{
    this->mUIInvisiblePanel = new InvisiblePanel();
}

bool UIInvisiblePanel::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    return UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);
}

void UIInvisiblePanel::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);
}

///////////////////////////
// Slider -> Panel //
///////////////////////////
void UISlider::AllocateComponent(const TRDescription& inDesc)
{
	float theX = 0;
	float theY = 0;
	float theWidth = 50;
	float theHeight = 50;
	bool theIsVertical = false;

	inDesc.GetTagValue(UITagXPos, theX);
	inDesc.GetTagValue(UITagYPos, theY);
	inDesc.GetTagValue(UITagWidth, theWidth);
	inDesc.GetTagValue(UITagHeight, theHeight);
	inDesc.GetTagValue(UIIsVertical, theIsVertical);

	int theScreenWidth = ScreenWidth();
	int theScreenHeight = ScreenHeight();
    this->mUISlider = new Slider(theX*theScreenWidth, theY*theScreenHeight, theWidth*theScreenWidth, theHeight*theScreenHeight, theIsVertical);
}

bool UISlider::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);

	// Read lower and upper limits
	int theLowerValue = 0;
	int theUpperValue = 20;

    inDesc.GetTagValue(UILowerValue, theLowerValue);
    inDesc.GetTagValue(UIUpperValue, theUpperValue);

	Slider* theSlider = dynamic_cast<Slider*>(inPanel);
	ASSERT(theSlider);
	theSlider->setRange(theLowerValue, theUpperValue);

	int theRangeWindow;
	if(inDesc.GetTagValue(UIRangeWindow, theRangeWindow))
	{
		// The range window is in pixels, the lower and upper values are in their own arbitrary scale
		//int theValueRange = theUpperValue - theLowerValue;
		//ASSERT(theRangeWindow < theValueRange);

		theSlider->setRangeWindowEnabled(true);
		theSlider->setRangeWindow(theRangeWindow);
	}

	int theStartingValue;
	if(inDesc.GetTagValue(UIDefaultIntValue, theStartingValue))
	{
		theSlider->setValue(theStartingValue);
	}
		
    return true;
}

void UISlider::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);
}

///////////////////////////
// Slider2 -> Panel //
///////////////////////////
void UISlider2::AllocateComponent(const TRDescription& inDesc)
{
	float theX = 0;
	float theY = 0;
	float theWidth = 50;
	float theHeight = 50;
	bool theIsVertical = false;
	
	inDesc.GetTagValue(UITagXPos, theX);
	inDesc.GetTagValue(UITagYPos, theY);
	inDesc.GetTagValue(UITagWidth, theWidth);
	inDesc.GetTagValue(UITagHeight, theHeight);
	inDesc.GetTagValue(UIIsVertical, theIsVertical);
	
	int theScreenWidth = ScreenWidth();
	int theScreenHeight = ScreenHeight();
    this->mUISlider2 = new Slider2(theX*theScreenWidth, theY*theScreenHeight, theWidth*theScreenWidth, theHeight*theScreenHeight, theIsVertical);
}

bool UISlider2::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);
	
	// Read lower and upper limits
	int theLowerValue = 0;
	int theUpperValue = 20;
	
    inDesc.GetTagValue(UILowerValue, theLowerValue);
    inDesc.GetTagValue(UIUpperValue, theUpperValue);
	
	Slider2* theSlider2 = dynamic_cast<Slider2*>(inPanel);
	ASSERT(theSlider2);
	theSlider2->setRange(theLowerValue, theUpperValue);
	
	int theRangeWindow;
	if(inDesc.GetTagValue(UIRangeWindow, theRangeWindow))
	{
		// The range window is in pixels, the lower and upper values are in their own arbitrary scale
		//int theValueRange = theUpperValue - theLowerValue;
		//ASSERT(theRangeWindow < theValueRange);
		
		theSlider2->setRangeWindowEnabled(true);
		theSlider2->setRangeWindow(theRangeWindow);
	}
	
	int theStartingValue;
	if(inDesc.GetTagValue(UIDefaultIntValue, theStartingValue))
	{
		theSlider2->setValue(theStartingValue);
	}
	
    return true;
}

void UISlider2::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);
}


//////////////////////////
// StaticLabel -> Label //
//////////////////////////
void UIStaticLabel::AllocateComponent(const TRDescription& inDesc)
{
	string theTitle;
    inDesc.GetTagValue(UITagText, theTitle);

	string theDefaultText("");
	LocalizeString(theTitle.c_str(), theDefaultText);
	
    // Position (normalized screen coords)
    float theXPos = UIDefaultXPos;
    float theYPos = UIDefaultYPos;
    inDesc.GetTagValue(UITagXPos, theXPos);
    inDesc.GetTagValue(UITagYPos, theYPos);
	
    // Width and height (normalized screen coords)
    float theWidth = UIDefaultWidth;
    float theHeight = UIDefaultHeight;
    inDesc.GetTagValue(UITagWidth, theWidth);
    inDesc.GetTagValue(UITagHeight, theHeight);
	
    this->mUIStaticLabel = new StaticLabel(theDefaultText.c_str(), theXPos*ScreenWidth(), theYPos*ScreenHeight(), theWidth*ScreenWidth(), theHeight*ScreenHeight());
}

bool UIStaticLabel::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    return UILabel::SetClassProperties(inDesc, inPanel, inSchemeManager);
}

void UIStaticLabel::Update(float theCurrentTime)
{
	UILabel::Update(theCurrentTime);
}




//////////////////////////
// SpritePanel -> Panel //
//////////////////////////
void UISpritePanel::AllocateComponent(const TRDescription& inDesc)
{
	string theBaseSpriteName;
    inDesc.GetTagValue(UIBaseSprite, theBaseSpriteName);

	string theRenderMode;
    inDesc.GetTagValue(UIRenderMode, theRenderMode);
	
    this->mUISpritePanel = new SpritePanel(theBaseSpriteName, theRenderMode);

	string theVAlignment;
    if(inDesc.GetTagValue(UIVAlignment, theVAlignment))
	{
		this->mUISpritePanel->SetVAlignment(theVAlignment);
	}
}

bool UISpritePanel::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    bool theSuccess = true;
	
    SpritePanel* theSpritePanel = (SpritePanel*)inPanel;
	
    theSuccess = UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);
    
    if(theSuccess)
    {
		// Add any of our own properties here
    }
	
    return theSuccess;
}

void UISpritePanel::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);
}


//////////////////////////
// DummyPanel -> Panel //
//////////////////////////
void UIDummyPanel::AllocateComponent(const TRDescription& inDesc)
{
    this->mUIDummyPanel = new DummyPanel();
}

bool UIDummyPanel::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    bool theSuccess = true;

    theSuccess = UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);
	
    return theSuccess;
}

void UIDummyPanel::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);
}



/////////////////////////
// SliderPlus -> Panel //
/////////////////////////
void UISliderPlus::AllocateComponent(const TRDescription& inDesc)
{
	float theX = 0;
	float theY = 0;
	float theWidth = 50;
	float theHeight = 50;
	bool theIsVertical = false;
	
	inDesc.GetTagValue(UITagXPos, theX);
	inDesc.GetTagValue(UITagYPos, theY);
	inDesc.GetTagValue(UITagWidth, theWidth);
	inDesc.GetTagValue(UITagHeight, theHeight);
	inDesc.GetTagValue(UIIsVertical, theIsVertical);
	
	int theScreenWidth = ScreenWidth();
	int theScreenHeight = ScreenHeight();
    this->mUISliderPlus = new SliderPlus(theX*theScreenWidth, theY*theScreenHeight, theWidth*theScreenWidth, theHeight*theScreenHeight, theIsVertical);
}

bool UISliderPlus::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
	UIPanel::SetClassProperties(inDesc, inPanel, inSchemeManager);
	
	// Read lower and upper limits
	int theLowerValue = 0;
	int theUpperValue = 20;
	
    inDesc.GetTagValue(UILowerValue, theLowerValue);
    inDesc.GetTagValue(UIUpperValue, theUpperValue);
	
	SliderPlus* theSlider = dynamic_cast<SliderPlus*>(inPanel);
	ASSERT(theSlider);
	theSlider->setRange(theLowerValue, theUpperValue);
	
	int theRangeWindow;
	if(inDesc.GetTagValue(UIRangeWindow, theRangeWindow))
	{
		// The range window is in pixels, the lower and upper values are in their own arbitrary scale
		//int theValueRange = theUpperValue - theLowerValue;
		//ASSERT(theRangeWindow < theValueRange);
		//theSlider->setRangeWindowEnabled(true);
		theSlider->setRangeWindow(theRangeWindow);
	}
	
	int theStartingValue;
	if(inDesc.GetTagValue(UIDefaultIntValue, theStartingValue))
	{
		theSlider->setValue(theStartingValue);
	}

	return true;
}

void UISliderPlus::Update(float theCurrentTime)
{
	UIPanel::Update(theCurrentTime);
}

