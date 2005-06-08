#include "mod/AvHLogoutComponent.h"
#include "ui/UITags.h"

////////////////////////////////////
// AvHLogoutComponent -> StaticLabel //
////////////////////////////////////
void AvHUILogoutComponent::AllocateComponent(const TRDescription& inDesc)
{
    // Width and height (normalized screen coords)
    float theWidth = UIDefaultWidth;
    float theHeight = UIDefaultHeight;
    inDesc.GetTagValue(UITagWidth, theWidth);
    inDesc.GetTagValue(UITagHeight, theHeight);
	
    this->mLogoutComponent = new AvHLogoutComponent(theWidth*ScreenWidth(), theHeight*ScreenHeight());
}

Panel* AvHUILogoutComponent::GetComponentPointer(void)
{
	return this->mLogoutComponent;
}

bool AvHUILogoutComponent::SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager)
{
    AvHLogoutComponent* theLogoutComponent = (AvHLogoutComponent*)inPanel;
	
    bool theSuccess = UIStaticLabel::SetClassProperties(inDesc, inPanel, inSchemeManager);
    
    if(theSuccess)
    {
		// Get font to use
		std::string theSchemeName;
		if(inDesc.GetTagValue(UITagScheme, theSchemeName))
		{
			const char* theSchemeCString = theSchemeName.c_str();
			SchemeHandle_t theSchemeHandle = inSchemeManager->getSchemeHandle(theSchemeCString);
			Font* theFont = inSchemeManager->getFont(theSchemeHandle);
			if(theFont)
			{
				theLogoutComponent->setFont(theFont);
			}
		}
		theSuccess = true;
	}

	return theSuccess;
}

