#include "mod/AvHUIFactory.h"
#include "mod/AvHTeamHierarchy.h"
#include "mod/AvHActionButtons.h"
#include "mod/AvHScrollPanel.h"
#include "mod/AvHLogoutComponent.h"
#include "mod/AvHTechImpulsePanel.h"

// Knows how to build custom mod-specific components
UIComponent* AvHUIFactory::BuildComponent(const TRDescription& inTextRep, CSchemeManager* inSchemeManager)
{
    UIComponent* theNewComponent = NULL;

    // If base class can't create it, maybe it's a custom component
    theNewComponent = UIFactory::BuildComponent(inTextRep, inSchemeManager);
    if(!theNewComponent)
    {
    	string theCompType = inTextRep.GetType();

        if(theCompType == "TeamHierarchy")
        {
            theNewComponent = new AvHUITeamHierarchy();
        }
		else if(theCompType == "ActionButtons")
		{
			theNewComponent = new AvHUIActionButtons();
		}
		else if(theCompType == "ScrollPanel")
		{
			theNewComponent = new AvHUIScrollPanel();
		}
		else if(theCompType == "LogoutComponent")
		{
			theNewComponent = new AvHUILogoutComponent();
		}
		else if(theCompType == "TechImpulsePanel")
		{
			theNewComponent = new AvHUITechImpulsePanel();
		}
		
        // Initialize it
	    if(theNewComponent)
	    {
	    	theNewComponent->AllocateAndSetProperties(inTextRep, inSchemeManager);
	    }
    }
    return theNewComponent;
}

