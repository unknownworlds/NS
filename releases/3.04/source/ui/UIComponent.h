#ifndef UICOMPONENT_H
#define UICOMPONENT_H

#include "textrep/TRDescription.h"
#include "VGUI_Panel.h"
class CSchemeManager;

using namespace vgui;

// Maintains the text representation of a component.  This is a base class that other UI components will be extended
// from.  It can read and write a description of the component, building the VGUI component in the process.  At any time
// it can be bound to the Half-life engine or removed from it.
class UIComponent 
{
public:
	UIComponent(void);

	// Destructor automatically removes component from the engine
	virtual ~UIComponent(void);

	bool			        AllocateAndSetProperties(const TRDescription& inDescription, CSchemeManager* inSchemeManager);

	virtual Panel*			GetComponentPointer(void) = 0;

	// Used to save component back out again
	TRDescription&	        GetDescription(void);

	const TRDescription&	GetDescription(void) const;

	const string&			GetName(void) const;

	virtual const string&	GetType(void) const = 0;

	virtual bool			SetClassProperties(const TRDescription& inDescription, Panel* inComponent, CSchemeManager* inSchemeManager);

	void					SetName(const string& inName);

    virtual void            Update(float theCurrentTime);

protected:
	string					mName;

	string					mType;

	TRDescription			mDescription;

private:

	virtual void AllocateComponent(const TRDescription& inDescription) = 0;


};

#endif
