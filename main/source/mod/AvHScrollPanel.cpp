#include "vgui_Panel.h"
#include "ui/UIComponents.h"
#include "mod/AvHScrollPanel.h"

AvHScrollPanel::AvHScrollPanel()
{
}
	
void AvHScrollPanel::paint() 
{
	InvisiblePanel::paint();
}
	
void AvHScrollPanel::paintBackground()
{
	InvisiblePanel::paintBackground();
}

void AvHUIScrollPanel::AllocateComponent(const TRDescription& inDesc)
{
	this->mScrollComponent = new AvHScrollPanel();
}

Panel* AvHUIScrollPanel::GetComponentPointer(void)
{
	return this->mScrollComponent;
}
