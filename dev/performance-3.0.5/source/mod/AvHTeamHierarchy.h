#ifndef AVHTEAMHIERARCHY_H
#define AVHTEAMHIERARCHY_H

#include "ui/UIComponents.h"
#include "VGUI_TextPanel.h"
#include "mod/AvHEntityHierarchy.h"
#include "ui/StaticLabel.h"
#include "mod/AvHMapExtents.h"

using vgui::Panel;
using vgui::TextPanel;


class AvHTeamHierarchy : public StaticLabel
{										                                                                                                            
public:
									AvHTeamHierarchy(int wide,int tall);

	void							SetFullScreen(bool inFullScreen);
	void							GetWorldPosFromMouse(int inMouseX, int inMouseY, float& outWorldX, float& outWorldY);

protected:

    void                            GetDrawInfo(AvHOverviewMap::DrawInfo& outDrawInfo);
    virtual void					paint();
    virtual void                    paintBackground();

    bool                            mFullScreen;

};



class AvHUITeamHierarchy : public UIStaticLabel
{
public:
	                        AvHUITeamHierarchy(void);
                            
	                        // Destructor automatically removes component from the engine
	                        virtual ~AvHUITeamHierarchy(void);

	virtual Panel*			GetComponentPointer(void);

	virtual const string&	GetType(void) const;

	virtual bool			SetClassProperties(const TRDescription& inDescription, Panel* inComponent, CSchemeManager* inSchemeManager);

private:

	void                    AllocateComponent(const TRDescription& inDescription);

    AvHTeamHierarchy*       mHierarchyComponent;

    string                  mType;

};

#endif