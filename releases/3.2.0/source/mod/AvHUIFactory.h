#ifndef AVH_UI_FACTORY_H
#define AVH_UI_FACTORY_H

#include "ui/UIFactory.h"
class CSchemeManager;

// Knows how to build custom mod-specific components
class AvHUIFactory : public UIFactory
{
public:
    virtual UIComponent*  BuildComponent(const TRDescription& inTextRep, CSchemeManager* inSchemeManager);

};

#endif