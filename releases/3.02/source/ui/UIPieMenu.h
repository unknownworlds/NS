#ifndef UIPIEMENU_H
#define UIPIEMENU_H

#include "ui/UIComponents.h"

class PieMenu;

class UIPieMenu : public UIPanel
{
public:
	UIPieMenu(void);

	// Destructor automatically removes component from the engine
	virtual ~UIPieMenu(void);

    virtual vgui::Panel*    GetComponentPointer(void);

	virtual const string&	GetType(void) const;

	virtual bool			SetClassProperties(const TRDescription& inDescription, Panel* inComponent, CSchemeManager* inSchemeManager);

    virtual void            Update(float theCurrentTime);

private:

    static const string     UIPieMenuType;

	virtual void            AllocateComponent(const TRDescription& inDescription);

    PieMenu*                mPieMenu;

};

#endif