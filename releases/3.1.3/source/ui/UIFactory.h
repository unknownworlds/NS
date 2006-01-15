#ifndef UI_FACTORY_H
#define UI_FACTORY_H

#include "textrep/TRDescription.h"
class UIComponent;
class CSchemeManager;

class UIFactory
{
public:
	// Uses name of entity to decided what type of UI component to build, uses the description to 
	// set all it's properties it recognizes
    virtual UIComponent*  BuildComponent(const TRDescription& inTextRep, CSchemeManager* inSchemeManager);

};

#endif