#include "AvHParticleTemplateClient.h"
#include "cl_dll/hud.h"
#include "cl_dll/parsemsg.h"
#include "cl_dll/cl_util.h"
#include "cl_dll/parsemsg.h"
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

// Clears out the templates and receives all new ones
int32 
AvHParticleTemplateListClient::ReceiveFromNetworkStream()
{
	int32 theBytesReceived = 0;

	//this->mTemplateList.clear();

	// Receive number of templates
	//int theNumTemplates = READ_LONG();
	//theBytesReceived += 4;

	//for(int theLoop = 0; theLoop < theNumTemplates; theLoop++)
	//{
		// Read each one
		AvHParticleTemplate theNewTemplate;

		// Read name
		char* theName = READ_STRING();
		string theStringName = string(theName);
		theNewTemplate.SetName(theStringName);
		theBytesReceived += theStringName.length();

		// Read max particles
		uint32 theMaxParticles = READ_LONG();
		theNewTemplate.SetMaxParticles(theMaxParticles);
		theBytesReceived += 4;

		// Read particle size
		float theParticleSize = READ_COORD();
		theNewTemplate.SetParticleSize(theParticleSize);
		theBytesReceived += 4;

		// Read sprite name
		char* theStr = READ_STRING();
		theStringName = theStr;
		theNewTemplate.SetSprite(theStringName);
		theBytesReceived += theStringName.length();

		// Read particle SYSTEM lifetime
		float theParticleSystemLifetime = (float)(READ_COORD());
		theNewTemplate.SetParticleSystemLifetime(theParticleSystemLifetime);
		theBytesReceived += 4;

		// Read particle lifetime
		float theParticleLifetime = (float)(READ_COORD());
		theNewTemplate.SetParticleLifetime(theParticleLifetime);
		theBytesReceived += 4;

		// Animation speed
		float theAnimSpeed = (float)READ_COORD();
		theNewTemplate.SetAnimationSpeed(theAnimSpeed);
		theBytesReceived += 4;

		// Num frames in sprite
		int theNumSpriteFrames = READ_BYTE();
		theNewTemplate.SetNumSpriteFrames(theNumSpriteFrames);
		theBytesReceived++;

		// Scale particle over course of its life
		float theParticleScaling = (float)READ_COORD();
		theNewTemplate.SetParticleScaling(theParticleScaling);
		theBytesReceived += 4;

		// Render mode
		int theRenderMode = READ_BYTE();
		theNewTemplate.SetRenderMode(theRenderMode);
		theBytesReceived++;

		// Read gen rate as long
		uint32 theGenRate = READ_LONG();
		theNewTemplate.SetGenerationRate(theGenRate);
		theBytesReceived += 4;

		// Read gen shape
		ShapeType theGenerationShape = READ_BYTE();
		theNewTemplate.SetGenerationShape(theGenerationShape);
		theBytesReceived += 1;

		// Read the generation params as 8 longs
		uint32 theInnerLoop;
		ParticleParams theGenParams;
		for(theInnerLoop = 0; theInnerLoop < 8; theInnerLoop++)
		{
			theGenParams[theInnerLoop] = READ_LONG();
		}
		theNewTemplate.SetGenerationParams(theGenParams);
		theBytesReceived += 32;

		// Read generation entity name (if any)
		int theGenerationEntityIndex = READ_LONG();
		theNewTemplate.SetGenerationEntityIndex(theGenerationEntityIndex);
		theBytesReceived += 4;

		// Read generation entity param
		float theGenerationEntityParam = READ_COORD();
		theNewTemplate.SetGenerationEntityParameter(theGenerationEntityParam);
		theBytesReceived += 4;
		
		// Read starting velocity shape
		ShapeType theStartingVelocityShape = READ_BYTE();
		theNewTemplate.SetStartingVelocityShape(theStartingVelocityShape);
		theBytesReceived += 1;
		
		// Read the starting velocity params as 8 longs
		ParticleParams theVelParams;
		for(theInnerLoop = 0; theInnerLoop < 8; theInnerLoop++)
		{
			theVelParams[theInnerLoop] = READ_LONG();
		}
		theNewTemplate.SetStartingVelocityParams(theVelParams);
		theBytesReceived += 32;
		
		// Read gravity
		PSVector theGravity;
		for(theInnerLoop = 0; theInnerLoop < 3; theInnerLoop++)
		{
			theGravity[theInnerLoop] = READ_COORD();
		}
		theNewTemplate.SetGravity(theGravity);
		theBytesReceived += 12;
		
		// Read number initial particles
		//int theNumInitialParticles = READ_LONG();
		//theNewTemplate.SetInitialParticles(theNumInitialParticles);
		//theBytesReceived += 4;
		
		float theMaxAlpha = READ_COORD();
		theNewTemplate.SetMaxAlpha(theMaxAlpha);
		theBytesReceived += 4;
		
		// Read particle system to generate
		int theParticleSystemIndexToGenerate = READ_LONG();
		theNewTemplate.SetParticleSystemIndexToGenerate(theParticleSystemIndexToGenerate);
		theBytesReceived += 4;
		
		// Read flags
		int theFlags = READ_LONG();
		theNewTemplate.SetFlags(theFlags);
		theBytesReceived += 4;
		
		// Add it on the end
		this->mTemplateList.insert(this->mTemplateList.end(), theNewTemplate);
	//}

	return theBytesReceived;
}

