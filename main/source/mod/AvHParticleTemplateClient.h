#ifndef AVH_PARTICLETEMPLATECLIENT_H
#define AVH_PARTICLETEMPLATECLIENT_H

#include "mod/AvHParticleTemplate.h"

class AvHParticleTemplateListClient : public AvHParticleTemplateList
{
public:
	int						InitializeDemoPlayback(int inSize, unsigned char* inBuffer);
	void					InitializeDemoRecording() const;
	
	// Clears out the templates and receives all new ones
	int32					ReceiveFromNetworkStream();

private:

};

#endif
