#include "ui/UIComponent.h"

UIComponent::UIComponent(void)
{
}

UIComponent::~UIComponent(void)
{
}

bool UIComponent::AllocateAndSetProperties(const TRDescription& inDescription, CSchemeManager* inSchemeManager)
{
	bool theSuccess = false;

	this->AllocateComponent(inDescription);

    this->mDescription = inDescription;

	theSuccess = this->SetClassProperties(inDescription, this->GetComponentPointer(), inSchemeManager);
	if(!theSuccess)
	{
		/* TODO: Emit error*/
	}

	return theSuccess;
}
TRDescription& UIComponent::GetDescription(void)
{
    return this->mDescription;
}

const TRDescription& UIComponent::GetDescription(void) const
{
    return this->mDescription;
}

const string& UIComponent::GetName(void) const
{
	return this->mName;
}

void UIComponent::SetName(const string& inName)
{
	this->mName = inName;
}

bool UIComponent::SetClassProperties(const TRDescription& inDescription, Panel* inComponent, CSchemeManager* inSchemeManager)
{
	return true;
}

void UIComponent::Update(float theCurrentTime)
{
    theCurrentTime;
}
