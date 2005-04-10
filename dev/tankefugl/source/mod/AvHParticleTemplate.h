#ifndef AVH_PARTICLE_TEMPLATE_H
#define AVH_PARTICLE_TEMPLATE_H

//
// Describes a type of particle system
// Exists both on client and server
// Reads and writes from network stream
// A list of these are created on the server after parsing modname.ps and mapname.ps
// They are sent to the client on connect (they don't change during the game)
// Order of list is identical on both client and server
// Data and networking only, no particle behavior
//
#include "types.h"
//#include "utils/common/mathlib.h"

class AvHParticleTemplateList;

// Data only, no behavior
typedef enum
{
	PS_Point = 0,		// Single point
	PS_Line = 1,		// Line segment
	PS_Triangle = 2,	// Triangle
	PS_Plane = 3,		// Arbitrarily-oriented plane
	PS_Box = 4,			// Axis-aligned box
	PS_Sphere = 5,		// Sphere
	PS_Cylinder = 6,	// Cylinder
	PS_Cone = 7,		// Cone
	PS_Blob = 8,		// Gaussian blob
	PS_Disc = 9,		// Arbitrarily-oriented disc
	PS_Rectangle = 10,	// Rhombus-shaped planar region
	PS_None = 11
} ParticleShape;

typedef	char		ShapeType;
typedef	int32		ParticleParams[8];
typedef float		PSVector[3];

class AvHParticleTemplate
{
public:
					AvHParticleTemplate();
					//AvHParticleTemplate(const AvHParticleTemplate& inTemplate);

	const string&	GetName() const;
	void			SetName(const string& inName);

	uint32			GetMaxParticles() const;
	void			SetMaxParticles(uint32 inMaxParticles);

	float			GetParticleSize() const;
	void			SetParticleSize(float inSize);

	float			GetParticleSystemLifetime() const;
	void			SetParticleSystemLifetime(float inNewLifetime);

	float			GetParticleLifetime() const;
	void			SetParticleLifetime(float inNewLifetime);

	string			GetSprite() const;
	void			SetSprite(string& inSpriteName);

	ShapeType		GetGenerationShape() const;
	void			SetGenerationShape(ShapeType inShape);

	string			GetGenerationEntityName() const;
	void			SetGenerationEntityName(const string& inName);

	string			GetParticleSystemToGenerate() const;
	void			SetParticleSystemToGenerate(const string& inName);

	int				GetParticleSystemIndexToGenerate() const;
	void			SetParticleSystemIndexToGenerate(int inIndex);
	
	int				GetGenerationEntityIndex() const;
	void			SetGenerationEntityIndex(int inIndex);

	const float		GetGenerationEntityParameter() const;
	void			SetGenerationEntityParameter(float inEntityParameter);
	
	ShapeType		GetStartingVelocityShape() const;
	void			SetStartingVelocityShape(ShapeType inShape);

	void			GetGenerationParams(ParticleParams& outParams) const;
	void			SetGenerationParams(const ParticleParams& inParms);

	void			GetStartingVelocityParams(ParticleParams& outParams) const;
	void			SetStartingVelocityParams(const ParticleParams& inParms);

	//int				GetInitialParticles() const;
	//void			SetInitialParticles(int inInitialParticles);

	uint32			GetGenerationRate() const;
	void			SetGenerationRate(uint32 inGenRate);

	bool			GetFadeIn() const;
	bool			GetFadeOut() const;
	bool			GetUseDensity() const;
	bool			GetUseWorldGravity() const;
	bool			GetUseTrisNotQuads() const;
	bool			GetMinimizeEdges() const;
	bool			GetConstrainPitch() const;
	bool			GetCollide() const;
	bool			GetHighDetailOnly() const;
	bool			GetFaceUp() const;

	int				GetFlags() const;
	void			SetFlags(int inFlags);

	void			GetGravity(PSVector& outGravity) const;
	void			SetGravity(const PSVector& inGravity);

	float			GetAnimationSpeed() const;
	void			SetAnimationSpeed(float inSpeed);

	int				GetNumSpriteFrames(void) const;
	void			SetNumSpriteFrames(int inFrames);

	float			GetParticleScaling() const;
	void			SetParticleScaling(float inScaling);

	int				GetRenderMode(void) const;
	void			SetRenderMode(int inRenderMode);

	float			GetMaxAlpha() const;
	void			SetMaxAlpha(float inMaxAlpha);

private:
	string				mName;

	uint32				mMaxParticles;

	PSVector			mBaseColor;

	string				mSprite;

	float				mParticleSize;

	float				mParticleSystemLifetime;

	float				mParticleLifetime;

	PSVector			mGravity;

	uint32				mGenerationRate;

	ShapeType			mGenerationShape;
	ParticleParams		mGenerationParams;
	int					mGenerationEntityIndex;
	float				mGenerationEntityParameter;
	string				mGenerationEntityName;
	
	ShapeType			mStartingVelocityShape;
	ParticleParams		mStartingVelocityParams;

	ShapeType			mDeathShape;
	ParticleParams		mDeathParams;

	ShapeType			mCollisionShape;
	ParticleParams		mCollisionParams;

	float				mMinSpeed;
	float				mMaxSpeed;

	int					mNumSpriteFrames;
	float				mAnimationSpeed;
	float				mParticleScaling;
	int					mRenderMode;
	float				mMaxAlpha;

	//int					mInitialParticles;
	bool				mFadeIn;
	int					mFlags;
	string				mParticleSystemToGenerate;
	int					mParticleSystemIndexToGenerate;
};

// the template list creates all templates
class AvHParticleTemplateList
{
public:
								AvHParticleTemplateList();

	virtual void				Clear();

	int							CreateTemplateFromIndex(int inBaseIndex = -1);

	AvHParticleTemplate*		GetTemplateAtIndex(uint32 inIndex);

	const AvHParticleTemplate*	GetTemplateAtIndex(uint32 inIndex) const;

	bool						GetTemplateIndexWithName(const string& inName, uint32& outIndex) const;

	uint32						GetNumberTemplates() const;

protected:

	typedef vector<AvHParticleTemplate>		ParticleTemplateListType;
	ParticleTemplateListType				mTemplateList;
};

#endif