#include "AvHParticleTemplateClient.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "cl_dll/demo.h"
#include "common/demo_api.h"

int	AvHParticleTemplateListClient::InitializeDemoPlayback(int inSize, unsigned char* inBuffer)
{
	// Read one particle template and add it to the list
	int theBytesRead = 0;

	AvHParticleTemplate theTemplate;

	// Read all fields
	string theTemplateName;
	LoadStringData(theTemplateName, inBuffer, theBytesRead);
	theTemplate.SetName(theTemplateName);

	uint32 theMaxParticles;
	LOAD_DATA(theMaxParticles);
	theTemplate.SetMaxParticles(theMaxParticles);

	float theParticleSize;
	LOAD_DATA(theParticleSize);
	theTemplate.SetParticleSize(theParticleSize);

	float theParticleSystemLifetime;
	LOAD_DATA(theParticleSystemLifetime);
	theTemplate.SetParticleSystemLifetime(theParticleSystemLifetime);
	
	float theParticleLifetime;
	LOAD_DATA(theParticleLifetime);
	theTemplate.SetParticleLifetime(theParticleLifetime);
	
	string theSpriteName;
	LoadStringData(theSpriteName, inBuffer, theBytesRead);
	theTemplate.SetSprite(theSpriteName);

	ShapeType theGenerationShape;
	LOAD_DATA(theGenerationShape);
	theTemplate.SetGenerationShape(theGenerationShape);

	string theGenerationEntityName;
	LoadStringData(theGenerationEntityName, inBuffer, theBytesRead);
	theTemplate.SetGenerationEntityName(theGenerationEntityName);

	string theParticleSystemToGenerate;
	LoadStringData(theParticleSystemToGenerate, inBuffer, theBytesRead);
	theTemplate.SetParticleSystemToGenerate(theParticleSystemToGenerate);

	int theParticleSystemIndexToGenerate;
	LOAD_DATA(theParticleSystemIndexToGenerate);
	theTemplate.SetParticleSystemIndexToGenerate(theParticleSystemIndexToGenerate);

	int theGenerationEntityIndex;
	LOAD_DATA(theGenerationEntityIndex);
	theTemplate.SetGenerationEntityIndex(theGenerationEntityIndex);
	
	float theGenerationEntityParameter;
	LOAD_DATA(theGenerationEntityParameter);
	theTemplate.SetGenerationEntityParameter(theGenerationEntityParameter);

	ShapeType theStartingVelocityShape;
	LOAD_DATA(theStartingVelocityShape);
	theTemplate.SetStartingVelocityShape(theStartingVelocityShape);

	ParticleParams theGenerationParams;
	LOAD_DATA(theGenerationParams);
	theTemplate.SetGenerationParams(theGenerationParams);

	ParticleParams theStartingVelocityParams;
	LOAD_DATA(theStartingVelocityParams);
	theTemplate.SetStartingVelocityParams(theStartingVelocityParams);

	uint32 theGenerationRate;
	LOAD_DATA(theGenerationRate);
	theTemplate.SetGenerationRate(theGenerationRate);

	int theParticleFlags;
	LOAD_DATA(theParticleFlags);
	theTemplate.SetFlags(theParticleFlags);

	PSVector theGravity;
	LOAD_DATA(theGravity);
	theTemplate.SetGravity(theGravity);

	float theAnimationSpeed;
	LOAD_DATA(theAnimationSpeed);
	theTemplate.SetAnimationSpeed(theAnimationSpeed);

	int theNumSpriteFrames;
	LOAD_DATA(theNumSpriteFrames);
	theTemplate.SetNumSpriteFrames(theNumSpriteFrames);

	float theParticleScaling;
	LOAD_DATA(theParticleScaling);
	theTemplate.SetParticleScaling(theParticleScaling);

	int theRenderMode;
	LOAD_DATA(theRenderMode);
	theTemplate.SetRenderMode(theRenderMode);

	float theMaxAlpha;
	LOAD_DATA(theMaxAlpha);
	theTemplate.SetMaxAlpha(theMaxAlpha);

	// Save the template
	this->mTemplateList.push_back(theTemplate);

	return theBytesRead;
}

void AvHParticleTemplateListClient::InitializeDemoRecording() const
{
	// Loop through all our particle templates, and write out each one 
	int theNumTemplates = this->GetNumberTemplates();
	for(int i = 0; i < theNumTemplates; i++)
	{
		const AvHParticleTemplate* theTemplate = this->GetTemplateAtIndex(i);
		ASSERT(theTemplate);

		int theTotalSize = 0;

		// Calculate total size needed to store template
		string theTemplateName = theTemplate->GetName();
		theTotalSize += GetDataSize(theTemplateName);

		uint32 theMaxParticles = theTemplate->GetMaxParticles();
		theTotalSize += sizeof(theMaxParticles);

		float theParticleSize = theTemplate->GetParticleSize();
		theTotalSize += sizeof(theParticleSize);

		float theParticleSystemLifetime = theTemplate->GetParticleSystemLifetime();
		theTotalSize += sizeof(theParticleSystemLifetime);

		float theParticleLifetime = theTemplate->GetParticleLifetime();
		theTotalSize += sizeof(theParticleLifetime);

		string theSpriteName = theTemplate->GetSprite();
		theTotalSize += GetDataSize(theSpriteName);

		ShapeType theGenerationShape = theTemplate->GetGenerationShape();
		theTotalSize += sizeof(theGenerationShape);

		string theGenerationEntityName = theTemplate->GetGenerationEntityName();
		theTotalSize += GetDataSize(theGenerationEntityName);

		string theParticleSystemToGenerate = theTemplate->GetParticleSystemToGenerate();
		theTotalSize += GetDataSize(theParticleSystemToGenerate);

		int theParticleSystemIndexToGenerate = theTemplate->GetParticleSystemIndexToGenerate();
		theTotalSize += sizeof(theParticleSystemIndexToGenerate);

		int theGenerationEntityIndex = theTemplate->GetGenerationEntityIndex();
		theTotalSize += sizeof(theGenerationEntityIndex);

		float theGenerationEntityParam = theTemplate->GetGenerationEntityParameter();
		theTotalSize += sizeof(theGenerationEntityParam);

		ShapeType theStartingVelocityShape = theTemplate->GetStartingVelocityShape();
		theTotalSize += sizeof(theStartingVelocityShape);

		ParticleParams theGenerationParams;
		theTemplate->GetGenerationParams(theGenerationParams);
		theTotalSize += sizeof(theGenerationParams);

		ParticleParams theStartingVelocityParams;
		theTemplate->GetStartingVelocityParams(theStartingVelocityParams);
		theTotalSize += sizeof(theStartingVelocityParams);

		uint32 theGenerationRate = theTemplate->GetGenerationRate();
		theTotalSize += sizeof(theGenerationRate);

		int theParticleFlags = theTemplate->GetFlags();
		theTotalSize += sizeof(theParticleFlags);

		PSVector theGravity;
		theTemplate->GetGravity(theGravity);
		theTotalSize += sizeof(theGravity);

		float theAnimationSpeed = theTemplate->GetAnimationSpeed();
		theTotalSize += sizeof(theAnimationSpeed);

		int theNumSpriteFrames = theTemplate->GetNumSpriteFrames();
		theTotalSize += sizeof(theNumSpriteFrames);

		float theParticleScaling = theTemplate->GetParticleScaling();
		theTotalSize += sizeof(theParticleScaling);

		int theRenderMode = theTemplate->GetRenderMode();
		theTotalSize += sizeof(theRenderMode);

		float theMaxAlpha = theTemplate->GetMaxAlpha();
		theTotalSize += sizeof(theMaxAlpha);

		// New memory 
		unsigned char* theCharArray = new unsigned char[theTotalSize];
		ASSERT(theCharArray);
		int theCounter = 0;

		// Write out each field
		SaveStringData(theCharArray, theTemplateName, theCounter);
		SAVE_DATA(theMaxParticles);
		SAVE_DATA(theParticleSize);
		SAVE_DATA(theParticleSystemLifetime);
		SAVE_DATA(theParticleLifetime);
		SaveStringData(theCharArray, theSpriteName, theCounter);
		SAVE_DATA(theGenerationShape);
		SaveStringData(theCharArray, theGenerationEntityName, theCounter);
		SaveStringData(theCharArray, theParticleSystemToGenerate, theCounter);
		SAVE_DATA(theParticleSystemIndexToGenerate);
		SAVE_DATA(theGenerationEntityIndex);
		SAVE_DATA(theGenerationEntityParam);
		SAVE_DATA(theStartingVelocityShape);
		SAVE_DATA(theGenerationParams);
		SAVE_DATA(theStartingVelocityParams);
		SAVE_DATA(theGenerationRate);
		SAVE_DATA(theParticleFlags);
		SAVE_DATA(theGravity);
		SAVE_DATA(theAnimationSpeed);
		SAVE_DATA(theNumSpriteFrames);
		SAVE_DATA(theParticleScaling);
		SAVE_DATA(theRenderMode);
		SAVE_DATA(theMaxAlpha);

		// Make sure we wrote exact amount of data we were supposed to
		ASSERT(theTotalSize == theCounter);
		
		Demo_WriteBuffer(TYPE_PARTICLES, theTotalSize, theCharArray);
	}
}

void AvHParticleTemplateListClient::Insert(const AvHParticleTemplate& inTemplate)
{
	this->mTemplateList.insert(this->mTemplateList.end(), inTemplate);
}