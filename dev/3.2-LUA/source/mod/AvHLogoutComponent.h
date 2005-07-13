#ifndef AVHLOGOUTCOMPONENT_H
#define AVHLOGOUTCOMPONENT_H

#include "ui/UIComponents.h"
#include "ui/StaticLabel.h"

class AvHLogoutComponent : public StaticLabel
{
public:
		AvHLogoutComponent(int wide, int tall) : StaticLabel(wide, tall) {}
};


class AvHUILogoutComponent : public UIStaticLabel
{
public:
	virtual Panel*			GetComponentPointer(void);

    virtual bool			SetClassProperties(const TRDescription& inDesc, Panel* inPanel, CSchemeManager* inSchemeManager);
	
private:
	
	void                    AllocateComponent(const TRDescription& inDescription);
	
    AvHLogoutComponent*		mLogoutComponent;
};



#endif
