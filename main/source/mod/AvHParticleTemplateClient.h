#ifndef AVH_PARTICLETEMPLATECLIENT_H
#define AVH_PARTICLETEMPLATECLIENT_H

#include "mod/AvHParticleTemplate.h"

class AvHParticleTemplateListClient : public AvHParticleTemplateList
{
public:
	int						InitializeDemoPlayback(int inSize, unsigned char* inBuffer, int index);
	void					InitializeDemoRecording() const;
	void					Insert(const AvHParticleTemplate& inTemplate, int index);

private:

};

#endif
