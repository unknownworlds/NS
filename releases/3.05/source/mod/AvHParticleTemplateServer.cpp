#include "util/nowarnings.h"
#include "mod/AvHParticleTemplateServer.h"
#include "dlls/util.h"
#include "util/STLUtil.h"
#include "mod/AvHParticleConstants.h"

void
AvHParticleTemplateListServer::Clear()
{
	AvHParticleTemplateList::Clear();
	this->mCreatedTemplates = false;
}

void
AvHParticleTemplateListServer::AddTemplatesFromFile(const string& inRelativeFileName)
{
}

bool
AvHParticleTemplateListServer::AddAttributesToTemplate(uint32 inTemplateIndex, const KeyValueData* inData)
{
	// TODO: Implement this for avh_particles_custom entity
	return false;
}

bool
AvHParticleTemplateListServer::CreateTemplates(const TRDescriptionList& inDescriptions)
{
	bool theSuccess = false;

	TRDescriptionList::const_iterator	theIterator;
	for(theIterator = inDescriptions.begin(); theIterator != inDescriptions.end(); theIterator++)
	{
		if(theIterator->GetType() == kpscSystemName)
		{
			// Create named particle system
			AvHParticleTemplate theTemplate;
			theTemplate.SetName(theIterator->GetName());

			int theMaxParticles;
			if(theIterator->GetTagValue(kpscNumParticles, theMaxParticles))
			{
				theTemplate.SetMaxParticles((uint32)theMaxParticles);
			}

			int theFlags;
			if(theIterator->GetTagValue(kpscSpawnFlags, theFlags))
			{
				theTemplate.SetFlags(theFlags);
			}

			float theParticleSize;
			if(theIterator->GetTagValue(kpscSize, theParticleSize))
			{
				theTemplate.SetParticleSize(theParticleSize);
			}

			//string theBaseColor;
			//if(theIterator->GetTagValue(kpscBaseColor, theBaseColor))
			//{
			//}

			float theParticleAnimationSpeed;
			if(theIterator->GetTagValue(kpscAnimationSpeed, theParticleAnimationSpeed))
			{
				theTemplate.SetAnimationSpeed(theParticleAnimationSpeed);
			}

			float theParticleNumSpriteFrames;
			if(theIterator->GetTagValue(kpscSpriteNumFrames, theParticleNumSpriteFrames))
			{
				theTemplate.SetNumSpriteFrames(theParticleNumSpriteFrames);
			}

			float theParticleSystemLifetime;
			if(theIterator->GetTagValue(kpscSystemLifetime, theParticleSystemLifetime))
			{
				theTemplate.SetParticleSystemLifetime(theParticleSystemLifetime);
			}

			float theParticleLifetime;
			if(theIterator->GetTagValue(kpscParticleLifetime, theParticleLifetime))
			{
				theTemplate.SetParticleLifetime(theParticleLifetime);
			}

			string theSprite;
			if(theIterator->GetTagValue(kpscSprite, theSprite))
			{
				theTemplate.SetSprite(theSprite);
			}

			float theParticleScaling;
			if(theIterator->GetTagValue(kpscScale, theParticleScaling))
			{
				theTemplate.SetParticleScaling(theParticleScaling);
			}

			int theParticleRenderMode;
			if(theIterator->GetTagValue(kpscRendermode, theParticleRenderMode))
			{
				theTemplate.SetRenderMode(theParticleRenderMode);
			}

			int theParticleGenerationRate;
			if(theIterator->GetTagValue(kpscGenRate, theParticleGenerationRate))
			{
				theTemplate.SetGenerationRate(theParticleGenerationRate);
			}

			string theGenerationShape;
			if(theIterator->GetTagValue(kpscGenShape, theGenerationShape))
			{
				ShapeType theShape;
				if(this->GetShapeTypeFromValue(theGenerationShape, theShape))
				{
					theTemplate.SetGenerationShape(theShape);
				}
			}

			string theGenerationParameters;
			if(theIterator->GetTagValue(kpscGenShapeParams, theGenerationParameters))
			{
				ParticleParams theParticleParams;
				if(8 == sscanf(theGenerationParameters.c_str(), "%d,%d,%d,%d,%d,%d,%d,%d", theParticleParams+0, theParticleParams+1, theParticleParams+2,theParticleParams+3, theParticleParams+4, theParticleParams+5, theParticleParams+6, theParticleParams+7))
				{
					theTemplate.SetGenerationParams(theParticleParams);
				}
			}

			string theStartingVelocityShape;
			if(theIterator->GetTagValue(kpscVelocityShape, theStartingVelocityShape))
			{
				ShapeType theShape;
				if(this->GetShapeTypeFromValue(theStartingVelocityShape, theShape))
				{
					theTemplate.SetStartingVelocityShape(theShape);
				}
			}

			string theStartingVelocityParameters;
			if(theIterator->GetTagValue(kpscVelocityParams, theStartingVelocityParameters))
			{
				ParticleParams theParticleParams;
				if(8 == sscanf(theStartingVelocityParameters.c_str(), "%d,%d,%d,%d,%d,%d,%d,%d", theParticleParams+0, theParticleParams+1, theParticleParams+2,theParticleParams+3, theParticleParams+4, theParticleParams+5, theParticleParams+6, theParticleParams+7))
				{
					theTemplate.SetStartingVelocityParams(theParticleParams);
				}
			}

			float theMaxAlpha;
			if(theIterator->GetTagValue(kpscMaxAlpha, theMaxAlpha))
			{
				theTemplate.SetMaxAlpha(theMaxAlpha);
			}

//			float theParticleMinSpeed;
//			if(theIterator->GetTagValue("particleMinSpeed", theParticleMinSpeed))
//			{
//			}
//
//			float theParticleMaxSpeed;
//			if(theIterator->GetTagValue("particleMaxSpeed", theParticleMaxSpeed))
//			{
//			}

			//int theInitialParticles;
			//if(theIterator->GetTagValue("particleInitialParticles", theInitialParticles))
			//{
			//	theTemplate.SetInitialParticles(theInitialParticles);
			//}

			// TODO: Add flags into .ps
			//int theFadeIn;
			//if(theIterator->GetTagValue("particleFadeIn", theFadeIn))
			//{
			//	if(theFadeIn)
			//		theTemplate.SetFadeIn();
			//}
			
			// Add it on the end
			this->mTemplateList.insert(this->mTemplateList.end(), theTemplate);

			theSuccess = true;
		}
	}

	this->mCreatedTemplates = theSuccess;

	return theSuccess;
}


bool
AvHParticleTemplateListServer::CreateTemplate(const KeyValueData* inData, uint32& outIndex)
{
	// TODO: Implement this for avh_particles_custom entity
	return false;
}


bool
AvHParticleTemplateListServer::GetCreatedTemplates(void) const
{
	return this->mCreatedTemplates;
}


bool
AvHParticleTemplateListServer::GetTemplateIndexWithName(const string& inName, uint32& outIndex) const
{
	ParticleTemplateListType::const_iterator	theIterator;
	uint32										theIndex = 0;
	bool										theSuccess = false;

	string theLowercaseInName = LowercaseString(inName);

	for(theIterator = this->mTemplateList.begin(); theIterator != this->mTemplateList.end(); theIterator++, theIndex++)
	{
		string theLowercaseTemplateName = LowercaseString(theIterator->GetName());

		// Make case-insensitive?
		if(theLowercaseInName == theLowercaseTemplateName)
		{
			outIndex = theIndex;
			theSuccess = true;
			break;
		}
	}
	
	return theSuccess;
}

bool
AvHParticleTemplateListServer::GetShapeTypeFromValue(const string& inValueName, ShapeType& outShape)
{
	bool theSuccess = false;
	
	if(inValueName == "Point")
	{
		outShape = PS_Point;
		theSuccess = true;
	}
	else if(inValueName == "Cone")
	{
		outShape = PS_Cone;
		theSuccess = true;
	}
	else if(inValueName == "Box")
	{
		outShape = PS_Box;
		theSuccess = true;
	}
				
	return theSuccess;
}

void
AvHParticleTemplateListServer::LinkToEntities(AvHParticleTemplate* inTemplate)
{
	// Look up entity names and matchup to indices	
	string theEntityName = inTemplate->GetGenerationEntityName();
	const char* theEntityNameCStr = theEntityName.c_str();
	if(!FStrEq(theEntityNameCStr, ""))
	{
		edict_t* theEdict = FIND_ENTITY_BY_TARGETNAME(NULL, theEntityNameCStr);
		if(!FNullEnt(theEdict))
		{
			int theEntityIndex = ENTINDEX(theEdict);
			inTemplate->SetGenerationEntityIndex(theEntityIndex);
		}
	}

	string theParticleSystemToGenerate = inTemplate->GetParticleSystemToGenerate();
	if(!FStrEq(theParticleSystemToGenerate.c_str(), ""))
	{
		// Lookup particle system and remember index
		uint32 theIndex = -1;
		if(this->GetTemplateIndexWithName(theParticleSystemToGenerate, theIndex))
		{
			inTemplate->SetParticleSystemIndexToGenerate(theIndex);
		}
	}
}

bool
AvHParticleTemplateListServer::SendToNetworkStream()
{
	bool theSuccess = false;

	if(this->mCreatedTemplates)
	{
		int theNumTemplates = this->mTemplateList.size();
		
		// Send number of particle templates
		WRITE_LONG(theNumTemplates);
		
		// For each one
		for(int theLoop = 0; theLoop < theNumTemplates; theLoop++)
		{
			// Send it
			const AvHParticleTemplate* theTemplate = this->GetTemplateAtIndex(theLoop);
			if(theTemplate)
			{
				this->SendTemplateToNetworkStream(theTemplate);
			}
			else
			{
				// Emit some error
			}
			theSuccess = true;
		}
	}
	return theSuccess;
}


void
AvHParticleTemplateListServer::SendTemplateToNetworkStream(const AvHParticleTemplate* inTemplate)
{
	int theLoop;

	// send name of PS...necessary?
	WRITE_STRING(inTemplate->GetName().c_str());

	// Send max particles
	WRITE_LONG(inTemplate->GetMaxParticles());

	// Send particle size
	WRITE_COORD(inTemplate->GetParticleSize());

	// Send sprite
	WRITE_STRING(inTemplate->GetSprite().c_str());

	// Write system lifetime
	WRITE_COORD(inTemplate->GetParticleSystemLifetime());

	// Write particle lifetime
	WRITE_COORD(inTemplate->GetParticleLifetime());

	// Animation speed
	WRITE_COORD(inTemplate->GetAnimationSpeed());

	// Num frames in sprite
	WRITE_BYTE(inTemplate->GetNumSpriteFrames());

	// Particle scaling
	WRITE_COORD(inTemplate->GetParticleScaling());

	// Render mode
	WRITE_BYTE(inTemplate->GetRenderMode());

	// Write gen rate as long
	WRITE_LONG(inTemplate->GetGenerationRate());
	
	// Write shape as char
	WRITE_BYTE((char)(inTemplate->GetGenerationShape()));

	// Write the generation params as 8 longs
	ParticleParams theGenParams;
	inTemplate->GetGenerationParams(theGenParams);
	for(theLoop = 0; theLoop < 8; theLoop++)
	{
		WRITE_LONG(theGenParams[theLoop]);
	}

	// Send generation entity name (if any)
	WRITE_LONG(inTemplate->GetGenerationEntityIndex());

	// Send generation entity param
	WRITE_COORD(inTemplate->GetGenerationEntityParameter());

	// Send starting velocity shape
	WRITE_BYTE(inTemplate->GetStartingVelocityShape());

	// Send starting velocity params
	ParticleParams theVelParams;
	inTemplate->GetStartingVelocityParams(theVelParams);
	for(theLoop = 0; theLoop < 8; theLoop++)
	{
		WRITE_LONG(theVelParams[theLoop]);
	}

	// Write gravity as three floats
	PSVector theGravity;
	inTemplate->GetGravity(theGravity);
	for(theLoop = 0; theLoop < 3; theLoop++)
	{
		WRITE_COORD(theGravity[theLoop]);
	}

	// Write number initial particles
	//WRITE_LONG(inTemplate->GetInitialParticles());

	float theMaxAlpha = inTemplate->GetMaxAlpha();
	WRITE_COORD(theMaxAlpha);

	// Send index of ps to generate
	int theParticleSystemIndexToGenerate = inTemplate->GetParticleSystemIndexToGenerate();
	WRITE_LONG(theParticleSystemIndexToGenerate);

	// Write flags
	int theFlags = inTemplate->GetFlags();
	WRITE_LONG(theFlags);
}
