#include "util/nowarnings.h"
#include "mod/AvHParticleTemplate.h"
#include "common/renderingconst.h"

AvHParticleTemplate::AvHParticleTemplate()
{
	// Populate with reasonable defaults
	this->mMaxParticles = 10;
	this->mParticleSize = 1.0f;
	this->mBaseColor[0] = this->mBaseColor[1] = this->mBaseColor[2] = 1.0f;
	this->mGravity[0] = this->mGravity[1] = this->mGravity[2] = 0.0f;

	this->mGenerationShape = PS_Point;
	memset(&this->mGenerationParams, 0, sizeof(this->mGenerationParams));

	this->mDeathShape = PS_None;
	memset(&this->mDeathParams, 0, sizeof(this->mDeathParams));
	
	this->mCollisionShape = PS_None;
	memset(&this->mCollisionParams, 0, sizeof(this->mCollisionParams));

	this->mStartingVelocityShape = PS_None;
	memset(&this->mStartingVelocityParams, 0, sizeof(this->mStartingVelocityParams));

	this->mMinSpeed = 0.0f;
	this->mMaxSpeed = 100.0f;
	this->mParticleSystemLifetime = -1;
	this->mParticleLifetime = -1;
	this->mNumSpriteFrames = 1;
	this->mAnimationSpeed = 1.0f;
	this->mParticleScaling = 1.0f;
	this->mRenderMode = (int)kRenderNormal;
	//this->mInitialParticles = 0;
	this->mMaxAlpha = 1.0f;
	this->mFadeIn = false;
	this->mGenerationEntityParameter = 0.0f;
	this->mGenerationEntityIndex = -1;
	this->mFlags = 0;
	this->mParticleSystemIndexToGenerate = -1;
}

//AvHParticleTemplate::AvHParticleTemplate(const AvHParticleTemplate& inTemplate)
//{
//	this->mMaxParticles = inTemplate.mMaxParticles;
//	this->mName = inTemplate.mName;
//	memcpy(&this->mBaseColor, &inTemplate.mBaseColor, sizeof(this->mBaseColor));
//	memcpy(&this->mGravity, &inTemplate.mGravity, sizeof(this->mGravity));
//
//	this->mGenerationRate = inTemplate.mGenerationRate;
//	this->mParticleSize = inTemplate.mParticleSize;
//	this->mSprite = inTemplate.mSprite;
//
//	this->mGenerationShape = inTemplate.mGenerationShape;
//	memcpy(&this->mGenerationParams, &inTemplate.mGenerationParams, sizeof(this->mGenerationParams));
//
//	this->mDeathShape = inTemplate.mDeathShape;
//	memcpy(&this->mDeathParams, &inTemplate.mDeathParams, sizeof(this->mDeathParams));
//	
//	this->mCollisionShape = inTemplate.mCollisionShape;
//	memcpy(&this->mCollisionParams, &inTemplate.mCollisionParams, sizeof(this->mCollisionParams));
//	
//	this->mMinSpeed = inTemplate.mMinSpeed;
//	this->mMaxParticles = inTemplate.mMaxSpeed;
//}


bool
AvHParticleTemplate::GetFadeIn() const
{
	return this->mFlags & 4;
}

bool
AvHParticleTemplate::GetFadeOut() const
{
	return this->mFlags & 8;
}

bool 
AvHParticleTemplate::GetUseDensity() const
{
	return this->mFlags & 2;
}

bool
AvHParticleTemplate::GetUseWorldGravity() const
{
	return this->mFlags & 16;
}

bool
AvHParticleTemplate::GetUseTrisNotQuads() const
{
	return this->mFlags & 32;
}

bool
AvHParticleTemplate::GetMinimizeEdges() const
{
	return this->mFlags & 64;
}

bool
AvHParticleTemplate::GetConstrainPitch() const
{
	return this->mFlags & 128;
}

bool
AvHParticleTemplate::GetCollide() const
{
	return this->mFlags & 256;
}

bool
AvHParticleTemplate::GetHighDetailOnly() const
{
	return this->mFlags & 512;
}

bool
AvHParticleTemplate::GetFaceUp() const
{
	return this->mFlags & 1024;
}

int
AvHParticleTemplate::GetFlags() const
{
	return this->mFlags;
}

void
AvHParticleTemplate::SetFlags(int inFlags)
{
	this->mFlags = inFlags;
}

const string&
AvHParticleTemplate::GetName() const
{
	return this->mName;
}

void
AvHParticleTemplate::SetName(const string& inName)
{
	this->mName = inName;
}

//int 
//AvHParticleTemplate::GetInitialParticles() const
//{
//	//return this->mInitialParticles;
//	return this->mMaxParticles;
//}

//void 
//AvHParticleTemplate::SetInitialParticles(int inInitialParticles)
//{
//	if(inInitialParticles < 0)
//		inInitialParticles = 0;
//
//	this->mInitialParticles = inInitialParticles;
//}

uint32
AvHParticleTemplate::GetMaxParticles() const
{
	return this->mMaxParticles;
}

float
AvHParticleTemplate::GetParticleSize() const
{
	return this->mParticleSize;
}

float			
AvHParticleTemplate::GetParticleSystemLifetime() const
{
	return this->mParticleSystemLifetime;
}

void
AvHParticleTemplate::SetParticleSystemLifetime(float inNewLifetime)
{
	this->mParticleSystemLifetime = inNewLifetime;
}

float
AvHParticleTemplate::GetParticleLifetime() const
{
	return this->mParticleLifetime;
}

void
AvHParticleTemplate::SetParticleLifetime(float inNewLifetime)
{
	this->mParticleLifetime = inNewLifetime;
}

void
AvHParticleTemplate::SetMaxParticles(uint32 inMaxParticles)
{
	this->mMaxParticles = inMaxParticles;
}

void
AvHParticleTemplate::SetParticleSize(float inSize)
{
	this->mParticleSize = inSize;
}

string
AvHParticleTemplate::GetSprite() const
{
	return this->mSprite;
}

void
AvHParticleTemplate::SetSprite(string& inSpriteName)
{
	this->mSprite = inSpriteName;
}

ShapeType
AvHParticleTemplate::GetGenerationShape() const
{
	return this->mGenerationShape;
}

void			
AvHParticleTemplate::SetGenerationShape(ShapeType inShape)
{
	this->mGenerationShape = inShape;
}

string 
AvHParticleTemplate::GetGenerationEntityName() const
{
	return this->mGenerationEntityName;
}

void
AvHParticleTemplate::SetGenerationEntityName(const string& inName)
{
	this->mGenerationEntityName = inName;
}

string
AvHParticleTemplate::GetParticleSystemToGenerate() const
{
	return this->mParticleSystemToGenerate;
}

void
AvHParticleTemplate::SetParticleSystemToGenerate(const string& inName)
{
	this->mParticleSystemToGenerate = inName;
}

int
AvHParticleTemplate::GetParticleSystemIndexToGenerate() const
{
	return this->mParticleSystemIndexToGenerate;
}

void
AvHParticleTemplate::SetParticleSystemIndexToGenerate(int inIndex)
{
	this->mParticleSystemIndexToGenerate = inIndex;
}

int
AvHParticleTemplate::GetGenerationEntityIndex() const
{
	return this->mGenerationEntityIndex;
}

void
AvHParticleTemplate::SetGenerationEntityIndex(int inIndex)
{
	this->mGenerationEntityIndex = inIndex;
}

const float	
AvHParticleTemplate::GetGenerationEntityParameter() const
{
	return this->mGenerationEntityParameter;
}

void
AvHParticleTemplate::SetGenerationEntityParameter(float inEntityParameter)
{
	this->mGenerationEntityParameter = inEntityParameter;
}

void
AvHParticleTemplate::GetGenerationParams(ParticleParams& outParams) const
{
	memcpy(outParams, this->mGenerationParams, sizeof(ParticleParams));
}

void			
AvHParticleTemplate::SetGenerationParams(const ParticleParams& inParms)
{
	memcpy(this->mGenerationParams, inParms, sizeof(inParms));
}

uint32			
AvHParticleTemplate::GetGenerationRate() const
{
	return this->mGenerationRate;
}

void			
AvHParticleTemplate::SetGenerationRate(uint32 inGenRate)
{
	this->mGenerationRate = inGenRate;
}

void
AvHParticleTemplate::GetGravity(PSVector& outGravity) const
{
	memcpy(&outGravity, &this->mGravity, sizeof(PSVector));
}

void			
AvHParticleTemplate::SetGravity(const PSVector& inGravity)
{
	memcpy(&this->mGravity, &inGravity, sizeof(inGravity));
}


float			
AvHParticleTemplate::GetAnimationSpeed() const
{
	return this->mAnimationSpeed;
}

void			
AvHParticleTemplate::SetAnimationSpeed(float inSpeed)
{
	this->mAnimationSpeed = inSpeed;
}

int				
AvHParticleTemplate::GetNumSpriteFrames(void) const
{
	return this->mNumSpriteFrames;
}

void			
AvHParticleTemplate::SetNumSpriteFrames(int inFrames)
{
	this->mNumSpriteFrames = inFrames;
}

float 
AvHParticleTemplate::GetMaxAlpha() const
{
	return this->mMaxAlpha;
}

void
AvHParticleTemplate::SetMaxAlpha(float inMaxAlpha)
{
	if(inMaxAlpha > 1.0f)
		inMaxAlpha = 1.0f;
	if(inMaxAlpha < 0.0f)
		inMaxAlpha = 0.0f;

	this->mMaxAlpha = inMaxAlpha;
}



float
AvHParticleTemplate::GetParticleScaling() const
{
	return this->mParticleScaling;
}

void
AvHParticleTemplate::SetParticleScaling(float inScaling)
{
	this->mParticleScaling = inScaling;
}

int
AvHParticleTemplate::GetRenderMode(void) const
{
	return this->mRenderMode;
}

void
AvHParticleTemplate::SetRenderMode(int inRenderMode)
{
	this->mRenderMode = inRenderMode;
}


ShapeType
AvHParticleTemplate::GetStartingVelocityShape() const
{
	return this->mStartingVelocityShape;
}

void
AvHParticleTemplate::SetStartingVelocityShape(ShapeType inShape)
{
	this->mStartingVelocityShape = inShape;
}

void
AvHParticleTemplate::GetStartingVelocityParams(ParticleParams& outParams) const
{
	memcpy(outParams, this->mStartingVelocityParams, sizeof(ParticleParams));
}

void
AvHParticleTemplate::SetStartingVelocityParams(const ParticleParams& inParms)
{
	memcpy(this->mStartingVelocityParams, inParms, sizeof(inParms));
}


AvHParticleTemplateList::AvHParticleTemplateList()
{
}

void 
AvHParticleTemplateList::Clear()
{
	//for(ParticleTemplateListType::iterator theIter = this->mTemplateList.begin(); theIter != this->mTemplateList.end(); theIter++)
	this->mTemplateList.clear();
}

int
AvHParticleTemplateList::CreateTemplateFromIndex(int inBaseIndex)
{
	AvHParticleTemplate theNewTemplate;

	if(inBaseIndex != -1)
	{
		ASSERT(inBaseIndex >= 0);
		ASSERT(inBaseIndex < (signed)this->mTemplateList.size());
		theNewTemplate = this->mTemplateList[inBaseIndex];
	}

	this->mTemplateList.push_back(theNewTemplate);
	int theNewIndex = (int)this->mTemplateList.size() - 1;
		
	return theNewIndex;
}

AvHParticleTemplate*
AvHParticleTemplateList::GetTemplateAtIndex(uint32 inIndex)
{
	AvHParticleTemplate* theTemplate = NULL;

	if(inIndex < this->mTemplateList.size())
	{
		theTemplate = &(this->mTemplateList[inIndex]);
	}

	return theTemplate;
}

const AvHParticleTemplate*
AvHParticleTemplateList::GetTemplateAtIndex(uint32 inIndex) const
{
	const AvHParticleTemplate* theTemplate = NULL;

	if(inIndex < this->mTemplateList.size())
	{
		theTemplate = &(this->mTemplateList[inIndex]);
	}

	return theTemplate;
}

bool
AvHParticleTemplateList::GetTemplateIndexWithName(const string& inName, uint32& outIndex) const
{
	bool theSuccess = false;
	uint32 theIndex = 0;
	
	ParticleTemplateListType::const_iterator theIter;

	for(theIter = this->mTemplateList.begin(); theIter != this->mTemplateList.end(); theIter++, theIndex++)
	{
		if(theIter->GetName() == inName)
		{
			outIndex = theIndex;
			theSuccess = true;
			break;
		}
	}

	return theSuccess;
}

uint32
AvHParticleTemplateList::GetNumberTemplates() const
{
	return (uint32)this->mTemplateList.size();
}

