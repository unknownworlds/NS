#ifndef AVHSCROLLPANEL_H
#define AVHSCROLLPANEL_H

#include "vgui_Panel.h"
#include "ui/UIComponents.h"

class AvHScrollPanel : public InvisiblePanel
{
public:
					AvHScrollPanel();
	
protected:
	virtual void	paint();
	virtual void	paintBackground();
};

class AvHUIScrollPanel : public UIColoredPanel
{
public:
	void				AllocateComponent(const TRDescription& inDesc);

	virtual Panel*		GetComponentPointer(void);
	
private:
	AvHScrollPanel*		mScrollComponent;
	
};

#endif
