#ifndef AVH_PARTICLETEMPLATESERVER_H
#define AVH_PARTICLETEMPLATESERVER_H

#include "mod/AvHParticleTemplate.h"
#include "textrep/TRDescription.h"
#include "dlls/extdll.h"
#include "common/const.h"
#include "engine/eiface.h"

class AvHParticleTemplateListServer : public AvHParticleTemplateList
{
public:
	void		Clear();
	
	void		AddTemplatesFromFile(const string& inRelativeFileName);

	bool		AddAttributesToTemplate(uint32 inTemplateIndex, const KeyValueData* inData);

	bool		CreateTemplates(const TRDescriptionList& inDescriptions);

	bool		CreateTemplate(const KeyValueData* inData, uint32& outIndex);

	bool		GetCreatedTemplates(void) const;

	bool		GetTemplateIndexWithName(const string& inName, uint32& outIndex) const;

	void		LinkToEntities(AvHParticleTemplate* inTemplate);

    bool        SendToNetworkStream();

	void		SendTemplateToNetworkStream(const AvHParticleTemplate* inTemplate);

private:

	bool		GetShapeTypeFromValue(const string& inValueName, ShapeType& outType);

	bool		mCreatedTemplates;


};

#endif