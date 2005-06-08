//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHParticleSystem.cpp $
// $Date: 2002/10/24 21:34:32 $
//
//-------------------------------------------------------------------------------
// $Log: AvHParticleSystem.cpp,v $
// Revision 1.20  2002/10/24 21:34:32  Flayra
// - Tried to hunt down particle crash on changelevel, this is slightly cleaner though
//
// Revision 1.19  2002/10/16 01:03:04  Flayra
// - Added new particle flag for particles to lie flat on ground ("faceup"), needed for scanner sweep
//
// Revision 1.18  2002/07/28 19:21:28  Flayra
// - Balance changes after/during RC4a
//
// Revision 1.17  2002/07/25 16:57:59  flayra
// - Linux changes
//
// Revision 1.16  2002/07/10 14:43:40  Flayra
// - Visibility fixes (too many PSs drawing, now they expire when not visible for awhile), removed cl_particleinfo drawing
//
// Revision 1.15  2002/06/10 20:01:24  Flayra
// - Updated extern references to drawing code (ugh)
//
// Revision 1.14  2002/05/28 17:58:44  Flayra
// - Temporary fix for bast.  It was creating crazy amounts of particle systems and should be investigated immediately.
//
// Revision 1.13  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHParticleSystem.h"
#include "mod/AvHParticleTemplate.h"

#ifdef AVH_CLIENT
  #include "cl_dll/cl_util.h"
  #include "cl_dll/util_vector.h"
  #include "common/renderingconst.h"
  #include "common/const.h"
  #include "engine/progdefs.h"
  #include "engine/edict.h"
  #include "pm_shared/pm_defs.h"
  #include "engine/cdll_int.h"
  #include "common/event_api.h"
  #include "common/cl_entity.h"
  #include "particles/particledefs.h"
  #include "particles/p_vector.h"
  #include "particles/papi.h"
  #include "usercmd.h"
  #include "pm_defs.h"
  #include "pm_shared.h"
  #include "pm_movevars.h"
  #include "pm_debug.h"
  #include "mod/AvHParticleSystemManager.h"
  #include "cl_dll/ev_hldm.h"
  #include "mod/AvHParticleTemplateClient.h"
  extern AvHParticleTemplateListClient	gParticleTemplateList;
  #include "mod/AvHClientVariables.h"
  void DrawScaledHUDSprite(int inSpriteHandle, int inMode, int inRowsInSprite, int inX, int inY, int inWidth, int inHeight, int inForceSpriteFrame, float inStartU = 0.0f, float inStartV = 0.0f, float inEndU = 1.0f, float inEndV = 1.0f, float inRotateUVRadians = 0.0f, bool inUVWrapsOverFrames = false);
  #include "mod/AvHClientUtil.h"
#else
  #include "extdll.h"
  #include "util.h"
  #include "particles/papi.h"

  #ifdef WIN32
  #include "common/cl_entity.h"
  #endif

#endif

#include "util/MathUtil.h"


const float kDefaultParticleSystemPhysicsUpdateRate = .05f;

AvHParticleSystem::AvHParticleSystem(AvHParticleTemplate* inTemplate, uint32 inIndex)
{
	this->mTemplateIndex = inIndex;
	this->mHandle = 0;
	this->mGroup = 0;
	this->mHasNormal = false;
	this->mNormal.x = this->mNormal.y = this->mNormal.z = 0;

	this->mGroupMaxParticles = inTemplate->GetMaxParticles();
	this->mGroup = pGenParticleGroups(1, this->mGroupMaxParticles);
	
	#ifdef AVH_CLIENT
	this->mSprite = 0;
	#endif

	memset(&this->mBaseEntityPos, 0, sizeof(this->mBaseEntityPos));
	//this->mEntity = -1;
	//memset(this->mOrigin, 0, sizeof(this->mOrigin));
	memset(this->mGenerationEntityAbsMin, 0, sizeof(this->mGenerationEntityAbsMin));
	memset(this->mGenerationEntityAbsMax, 0, sizeof(this->mGenerationEntityAbsMax));

	this->mTimeCreated = -1;
	this->mIsMarkedForDeletion = false;

	this->mNumSpriteFrames = inTemplate->GetNumSpriteFrames();
	this->mAnimationSpeed = inTemplate->GetAnimationSpeed();
	
	this->mGenerationEntityIndex = inTemplate->GetGenerationEntityIndex();
	this->mGenerationEntityParam = inTemplate->GetGenerationEntityParameter();
	this->mGenerationEntityVolumeFactor = 0.0f;
	
	this->mUpdateFirst = true;
	this->mFadeIn = inTemplate->GetFadeIn();
	this->mFadeOut = inTemplate->GetFadeOut();
	this->mUseWorldGravity = inTemplate->GetUseWorldGravity();
	inTemplate->GetGravity(this->mParticleGravity);
	this->mUseDensity = inTemplate->GetUseDensity();
	this->mUseTrisNotQuads = inTemplate->GetUseTrisNotQuads();
	this->mMinimizeEdges = inTemplate->GetMinimizeEdges();
	this->mMaxAlpha = inTemplate->GetMaxAlpha();
	this->mConstrainPitch = inTemplate->GetConstrainPitch();
	this->mFaceUp = inTemplate->GetFaceUp();
	this->mCollide = inTemplate->GetCollide();
	this->mParticleSystemIndexToGenerate = inTemplate->GetParticleSystemIndexToGenerate();

	this->mHasGeneratedParticles = false;
	this->mCustomData = 0;
	
#ifdef AVH_CLIENT
	this->LoadSpriteIfNeeded(inTemplate);
	this->mIsVisible = true;
	this->mLastTimeVisibilitySetTrue = -1;
#endif

#ifdef AVH_SERVER
	this->mTimeOfLastPhysicsUpdate = -1;
	this->mPhysicsUpdateTime = kDefaultParticleSystemPhysicsUpdateRate;
#endif

	this->LoadFromTemplate(inTemplate);
}

#ifdef AVH_CLIENT
//extern "C"
//{
	extern playermove_t* pmove;
//}
#endif

void
AvHParticleSystem::Collide(float inTime)
{
	#ifdef AVH_CLIENT
	ParticleGroup* theCurrentGroup = pGetCurrentGroup();
	if(theCurrentGroup)
	{
		// See which particles bounce.
		for(int i = 0; i < theCurrentGroup->p_count; i++)
		{
			Particle& m = theCurrentGroup->list[i];
			
			// See if particle's current and next positions cross plane.
			// If not, couldn't bounce, so keep going.
			vec3_t pCurrent;
			pCurrent[0] = m.pos.x;
			pCurrent[1] = m.pos.y;
			pCurrent[2] = m.pos.z;

			pVector pnext(m.pos + m.vel * inTime);
			vec3_t pNext;
			pNext[0] = pnext.x;
			pNext[1] = pnext.y;
			pNext[2] = pnext.z;

			struct pmtrace_s* trace = pmove->PM_TraceLine(pCurrent, pNext, PM_TRACELINE_ANYVISIBLE, 2 /*point sized hull*/, -1);
			if(trace->fraction != 1.0)
			{
				// Play sound where particle hit 
				//pmove->PM_PlaySound( CHAN_BODY, "player/pl_wade1.wav", 1, ATTN_NORM, 0, PITCH_NORM );
				//EV_HLDM_DecalGunshot( trace, BULLET_PLAYER_9MM );

				//if(gEngfuncs.pfnRandomLong( 0, 3 ) == 1)
				//{
				//	gEngfuncs.pEventAPI->EV_PlaySound(-1, trace->endpos, CHAN_AUTO, "drop.wav", .5f, ATTN_STATIC, 0, 94 + gEngfuncs.pfnRandomLong( 0, 0xf ));
				//}
				
				// Mark the particle for deletion
				//m.age = -1;
				theCurrentGroup->Remove(i);
				
				// Generate new particle system at point of impact, if specified
				if(this->mParticleSystemIndexToGenerate != -1)
				{
					AvHParticleSystemManager::Instance()->CreateParticleSystem(this->mParticleSystemIndexToGenerate, trace->endpos);
				}
			}
		}
	}
	#endif
}

void
AvHParticleSystem::Draw(const pVector &inView)
{
	// only draw on client
	#ifdef AVH_CLIENT
	if(this->GetIsVisible())
	{
		AvHParticleTemplate* theTemplate = gParticleTemplateList.GetTemplateAtIndex(this->mTemplateIndex);
		if(theTemplate)
		{
			string theParticleSystemName = theTemplate->GetName();

			pCurrentGroup(this->mGroup);
			
			// Back face culling
			gEngfuncs.pTriAPI->CullFace( TRI_FRONT );
			
			// Set render mode
			gEngfuncs.pTriAPI->RenderMode( this->mRenderMode );
			
			// Draw it
			this->DrawGroup(inView);
			
			// Reset render mode
			gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
			
		}
	}
	#endif
}

#ifdef AVH_CLIENT
void
AvHParticleSystem::DrawGroup(const pVector &inView)
{
	bool draw_tex = (this->mSprite != 0);
	bool const_color = true;
	bool const_size = false;
	float size_scale = this->mParticleSize;

	// Only support one group right now
	int cnt = pGetGroupCount();
	
	if(cnt < 1)
		return;

    // TODO: Change these news to use vectors of maxParticleSize?  Get pGetParticleGroupRef working?
	pVector *ppos = new pVector[cnt];
	float* pAge = new float[cnt];
	float *color = const_color ? NULL : new float[cnt * 4];
	pVector *size = const_size ? NULL : new pVector[cnt];
	pGetParticles(0, cnt, (float *)ppos, color, NULL, (float *)size, (float*)pAge);

	//ParticleGroup* theParticleGroup = pGetParticleGroupRef(cnt - 1);
	//if(!theParticleGroup)
	//	return;
	//
	//pVector* ppos = NULL;
	//float* color = NULL;
	//pVector* size = NULL;

	//ppos = &(theParticleGroup->list->pos);
	//if(!const_color)
	//	color = (float*)&(theParticleGroup->list->color);
	//if(!const_size)
	//	size = &(theParticleGroup->list->size);
	
	// Compute the vectors from the particle to the corners of its tri.
	// 2
	// |\      The particle is at the center of the x.
	// |-\     V0, V1, and V2 go from there to the vertices.
	// |x|\    The texcoords are (0,0), (2,0), and (0,2) respectively.
	// 0-+-1   We clamp the texture so the rest is transparent.

	pVector up(0, 0, 1);
	pVector right = inView ^ up;
	right.normalize();
	pVector nup = right ^ inView;
	//pVector nup = inView ^ right;

	// The particles should face you unless constrained (this is for rain and the like)
	if(this->mConstrainPitch)
		nup = up;

	// Particle should draw facing up
	if(this->mFaceUp)
	{
		up = pVector(0, 1, 0);
		right = pVector(-1, 0, 0);
		nup  = pVector(0, -1, 0);
	}

	right *= size_scale/2.0f;
	nup *= size_scale/2.0f;

	// Quad draws v0, v1, v2, v3
	//pVector V0 = -(right + nup);

	// Lower left
	pVector V0 = nup - right;

	// Upper left
	pVector V1 = -(right + nup);

	// Upper right
	pVector V2 = right - nup;

	// Lower right
	pVector V3 = right + nup;

	// Tri draws v0, v4, v3
	//pVector V4 = nup;
	pVector V4 = -nup;

	for(int i = 0; i < cnt; i++)
	{
		int theTextureOffsetToUse = 0;
		float theAgeFactor = 0.0f;
		float theParticleLifetime = this->GetParticleLifetime();
		if(theParticleLifetime > 0)
		{
			// 0 - 1 value of nearness to death
			theAgeFactor = pAge[i]/theParticleLifetime;

			if(this->mNumSpriteFrames > 1)
			{
				// Read in number of frames, also read in speed so we can loop the same animation multiple times over their life!
				int theNumSpriteFrames = this->mNumSpriteFrames;
				theTextureOffsetToUse = this->mAnimationSpeed*(theAgeFactor)*(theNumSpriteFrames-1);
				theTextureOffsetToUse = min(max(0, theTextureOffsetToUse), theNumSpriteFrames-1);
			}
		}

		if(this->mSprite)
		{
			struct model_s* theSpriteModel = const_cast<struct model_s*>(gEngfuncs.GetSpritePointer(this->mSprite));
			if(theSpriteModel && gEngfuncs.pTriAPI->SpriteTexture(theSpriteModel, theTextureOffsetToUse))
			{
				pVector &p = ppos[i];
				
				pVector sV0 = V0;
				pVector sV1 = V1;
				pVector sV2 = V2;
				pVector sV3 = V3;
				pVector sV4 = V4;
				
				if(this->GetParticleScale() != 1.0f && (theAgeFactor != 0.0f))
				{
					float theScaleFactor = 1 + (this->GetParticleScale() - 1)*theAgeFactor;
					sV0 *= theScaleFactor;
					sV1 *= theScaleFactor;
					sV2 *= theScaleFactor;
					sV3 *= theScaleFactor;
					sV4 *= theScaleFactor;
				}
				
				// Set alpha to max
				float theAlpha = this->mMaxAlpha;
				
				// If the particles don't live forever and we can fade, calculate the alpha
				if((this->mFadeIn || this->mFadeOut) && (theParticleLifetime > 0))
				{
					float theHalfLifetime = theParticleLifetime/2.0f;
					if(this->mFadeIn && this->mFadeOut)
						theAlpha = this->mMaxAlpha - this->mMaxAlpha*(fabs(pAge[i] - theHalfLifetime)/theHalfLifetime);
					else if(this->mFadeIn)
						theAlpha = this->mMaxAlpha*pAge[i]/theParticleLifetime;
					else if(this->mFadeOut)
						theAlpha = this->mMaxAlpha - this->mMaxAlpha*(pAge[i]/theParticleLifetime);
					
					theAlpha = max(min(this->mMaxAlpha, theAlpha), 0.0f);
				}

//				if(cl_particleinfo->value)
//				{
//					pVector theUpperLeftWorldPos = p + sV1;
//					pVector theLowerRightWorldPos = p + sV3;
//					pVector theUpperLeftScreenPos;
//					pVector theLowerRightScreenPos;
//
//					AvHCUWorldToScreen((float*)&theUpperLeftWorldPos, (float*)&theUpperLeftScreenPos);
//					AvHCUWorldToScreen((float*)&theLowerRightWorldPos, (float*)&theLowerRightScreenPos);
//
//					int theScreenX = theUpperLeftWorldPos.x;
//					int theScreenY = theUpperLeftWorldPos.y;
//					int theScreenWidth = theLowerRightScreenPos.x - theScreenX;
//					int theScreenHeight = theLowerRightScreenPos.y - theScreenY;
//
//					DrawScaledHUDSprite(this->mSprite, this->mRenderMode, 1, theScreenX, theScreenY, theScreenWidth, theScreenHeight, theTextureOffsetToUse);
//				}
//				else
//				{
					if(this->mUseTrisNotQuads)
					{
						gEngfuncs.pTriAPI->Begin( TRI_TRIANGLES );
						
						gEngfuncs.pTriAPI->Color4f(1.0f, 1.0f, 1.0f, theAlpha);
						
						if(draw_tex) gEngfuncs.pTriAPI->TexCoord2f(0,0);
						pVector ver = p + sV0;
						gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
						
						if(draw_tex) gEngfuncs.pTriAPI->TexCoord2f(.5,1);
						ver = p + sV4;
						gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
						
						if(draw_tex) gEngfuncs.pTriAPI->TexCoord2f(1,0);
						ver = p + sV3;
						gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
						
						gEngfuncs.pTriAPI->End();
					}
					else
					{
						gEngfuncs.pTriAPI->Begin( TRI_QUADS );
						
						gEngfuncs.pTriAPI->Color4f(1.0f, 1.0f, 1.0f, theAlpha);
						
						if(draw_tex) gEngfuncs.pTriAPI->TexCoord2f(0,0);
						pVector ver = p + sV0;
						gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
						
						if(draw_tex) gEngfuncs.pTriAPI->TexCoord2f(1,0);
						ver = p + sV3;
						gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
						
						if(draw_tex) gEngfuncs.pTriAPI->TexCoord2f(1,1);
						ver = p + sV2;
						gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
						
						if(draw_tex) gEngfuncs.pTriAPI->TexCoord2f(0,1);
						ver = p + sV1;
						gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
						
						gEngfuncs.pTriAPI->End();
					}
//				}
			}
		}
	}

	delete [] ppos;
	if(color) delete [] color;
	if(size) delete [] size;
	if(pAge) delete [] pAge;
}

float AvHParticleSystem::GetLastTimeVisibilityLastSetTrue() const
{
	return this->mLastTimeVisibilitySetTrue;
}

#endif

ParticleSystemHandle
AvHParticleSystem::GetHandle() const
{
	return this->mHandle;
}

bool
AvHParticleSystem::GetHasGeneratedParticles() const
{
	return this->mHasGeneratedParticles;
}

bool
AvHParticleSystem::GetIsMarkedForDeletion(void) const
{
	return this->mIsMarkedForDeletion;
}

#ifdef AVH_CLIENT
bool
AvHParticleSystem::GetIsVisible() const
{
	return this->mIsVisible;
}
#endif

int
AvHParticleSystem::GetNumberOfParticles(void) const
{
	int theNumParticles = 0;

	pCurrentGroup(this->mGroup);
	theNumParticles = pGetGroupCount();

	return theNumParticles;
}

float
AvHParticleSystem::GetParticleSystemLifetime() const
{
	return this->mParticleSystemLifetime;
}

float
AvHParticleSystem::GetParticleLifetime() const
{
	return this->mParticleLifetime + this->mCustomData*.2f;
}

uint32 
AvHParticleSystem::GetTemplateIndex() const
{
	return this->mTemplateIndex;
}

float
AvHParticleSystem::GetTimeCreated() const
{
	return this->mTimeCreated;
}

// Updating this function?  Change ::Update below for max particles?
void
AvHParticleSystem::Kill()
{
	//pCurrentGroup(this->mGroup);
	pDeleteParticleGroups(this->mGroup);
}

void AvHParticleSystem::LoadFromTemplate(AvHParticleTemplate* inTemplate)
{
	this->mParticleSize = inTemplate->GetParticleSize();
	this->mParticleScaling = inTemplate->GetParticleScaling();
	this->mGenerationRate = inTemplate->GetGenerationRate();
	this->mParticleLifetime = inTemplate->GetParticleLifetime();
	this->mParticleSystemLifetime = inTemplate->GetParticleSystemLifetime();
	this->mMaxParticles = inTemplate->GetMaxParticles();
	this->mRenderMode = inTemplate->GetRenderMode();
	this->mGenerationShape = inTemplate->GetGenerationShape();
	inTemplate->GetGenerationParams(this->mGenerationParams);
	this->mStartingVelocityShape = inTemplate->GetStartingVelocityShape();
	inTemplate->GetStartingVelocityParams(this->mStartingVelocityParams);
}

#ifdef AVH_CLIENT
void
AvHParticleSystem::LoadSpriteIfNeeded(AvHParticleTemplate* inTemplate)
{
	if(!this->mSprite)
	{
		string theSprite = inTemplate->GetSprite();
		if(theSprite != "")
		{
			this->mSprite = Safe_SPR_Load(theSprite.c_str());
			if(this->mSprite != 0)
			{
				int theNumFrames = SPR_Frames(this->mSprite);
				ASSERT(this->mNumSpriteFrames <= theNumFrames);
			}
			else			
			{
				char theErrorMessage[512];
				sprintf(theErrorMessage, "Couldn't load sprite, deleting particle system: %s\n", theSprite.c_str());
				DrawConsoleString(0, 0, theErrorMessage);
				this->SetIsMarkedForDeletion();
			}
		}
	}
}
#endif

bool
AvHParticleSystem::GetGenerationEntity(int &outIndex) const
{
	bool theSuccess = false;

	if(this->mGenerationEntityIndex != -1)
	{
		outIndex = this->mGenerationEntityIndex;
		theSuccess = true;
	}

	return theSuccess;
}

//void
//AvHParticleSystem::SetEntity(int inEntity)
//{
//	//this->mEntity = inEntity;
//	this->mGenerationEntityIndex = inEntity;
//}

void
AvHParticleSystem::SetHandle(ParticleSystemHandle inHandle)
{
	this->mHandle = inHandle;
}

void
AvHParticleSystem::SetIsMarkedForDeletion(void)
{
	this->mIsMarkedForDeletion = true;
}

#ifdef AVH_CLIENT
void
AvHParticleSystem::SetIsVisible(bool inVisibilityState, float inTimeSet)
{
	this->mIsVisible = inVisibilityState;

	if(inVisibilityState)
	{
		this->mLastTimeVisibilitySetTrue = inTimeSet;
	}
	
	if(!this->mIsVisible)
	{
		AvHParticleTemplate* theTemplate = gParticleTemplateList.GetTemplateAtIndex(this->mTemplateIndex);
		if(theTemplate)
		{
			string theParticleSystemName = theTemplate->GetName();
			int a = 0;
		}
	}
}
#endif

void AvHParticleSystem::SetNormal(const vec3_t& inOrigin)
{
	this->mHasNormal = true;
	this->mNormal.x = inOrigin.x;
	this->mNormal.y = inOrigin.y;
	this->mNormal.z = inOrigin.z;
}

void
AvHParticleSystem::SetParticleSystemLifetime(float inLifetime)
{
	this->mParticleSystemLifetime = inLifetime;
}

void
AvHParticleSystem::SetParticleLifetime(float inLifetime)
{
	this->mParticleLifetime = inLifetime;
}

void
AvHParticleSystem::SetPosition(vec3_t inOrigin)
{
	//memcpy(this->mOrigin, inOrigin, sizeof(vec3_t));
	this->mBaseEntityPos.x = inOrigin.x;
	this->mBaseEntityPos.y = inOrigin.y;
	this->mBaseEntityPos.z = inOrigin.z;
}

void
AvHParticleSystem::SetTimeCreated(float inTime)
{
	this->mTimeCreated = inTime;
	
	#ifdef AVH_CLIENT
	if(this->mIsVisible)
	{
		this->mLastTimeVisibilitySetTrue = inTime;
	}
	#endif
}

void
AvHParticleSystem::GenerateParticles(int inNumberParticles)
{
	PDomainEnum theDomain = (PDomainEnum)this->mGenerationShape;
	if((inNumberParticles > 0) && (theDomain != PS_None))
	{
		ParticleParams theParams;
		memset(theParams, 0, sizeof(ParticleParams));
		float theEdgeInset = (this->mMinimizeEdges ? this->mParticleSize : 0.0f);
		
		if(this->mGenerationEntityIndex != -1)
		{
			if((theDomain == PS_Point) || (theDomain == PS_Blob) || (theDomain == PS_Sphere))
			{
				theParams[0] = (this->mGenerationEntityAbsMax[0] + this->mGenerationEntityAbsMin[0])/2.0f;
				theParams[1] = (this->mGenerationEntityAbsMax[1] + this->mGenerationEntityAbsMin[1])/2.0f;
				theParams[2] = (this->mGenerationEntityAbsMax[2] + this->mGenerationEntityAbsMin[2])/2.0f;

				if(theDomain == PS_Blob)
				{
					theParams[3] = this->mGenerationEntityParam;
				}
				else if(theDomain == PS_Sphere)
				{
					float theRadius1 = fabs(this->mGenerationEntityAbsMax[0] - this->mGenerationEntityAbsMin[0])/2.0f;
					theRadius1 = max(theRadius1 - theEdgeInset, 0);
					float theRadius2 = this->mGenerationEntityParam;
					theRadius2 = max(theRadius2 - theEdgeInset, 0);
					theParams[3] = theRadius1;
					theParams[4] = theRadius2;
				}
			}					  
			else if(theDomain == PS_Box)
			{
				theParams[0] = this->mGenerationEntityAbsMin[0] + theEdgeInset;
				theParams[1] = this->mGenerationEntityAbsMin[1] + theEdgeInset;
				theParams[2] = this->mGenerationEntityAbsMin[2] + theEdgeInset;
				theParams[3] = this->mGenerationEntityAbsMax[0] - theEdgeInset;
				theParams[4] = this->mGenerationEntityAbsMax[1] - theEdgeInset;
				theParams[5] = this->mGenerationEntityAbsMax[2] - theEdgeInset;
			}
		}
		else
		{
			memcpy(theParams, this->mGenerationParams, sizeof(ParticleParams));

			// Different domains have different meanings, generate points appropriately for each
			if(theDomain == PS_Point)
			{
				theParams[0] += this->mBaseEntityPos.x;
				theParams[1] += this->mBaseEntityPos.y;
				theParams[2] += this->mBaseEntityPos.z;
			}
			else if((theDomain == PS_Cone) || (theDomain == PS_Box))
			{
				theParams[0] += this->mBaseEntityPos.x;
				theParams[1] += this->mBaseEntityPos.y;
				theParams[2] += this->mBaseEntityPos.z;
				theParams[3] += this->mBaseEntityPos.x;
				theParams[4] += this->mBaseEntityPos.y;
				theParams[5] += this->mBaseEntityPos.z;
			}
		}

		pSource(inNumberParticles, theDomain, theParams[0], theParams[1], theParams[2], theParams[3], theParams[4], theParams[5], theParams[6], theParams[7]);
		this->mHasGeneratedParticles = true;
	}
}

float AvHParticleSystem::GetParticleScale() const
{
	return this->mParticleScaling + this->mCustomData*.3f;
}

void AvHParticleSystem::SetCustomData(uint16 inCustomData)
{
	this->mCustomData = inCustomData;
}

void
AvHParticleSystem::SetGenerationEntityExtents(vec3_t& inMin, vec3_t& inMax)
{
	ASSERT(inMin.x <= inMax.x);
	ASSERT(inMin.y <= inMax.y);
	ASSERT(inMin.z <= inMax.z);
	
	//memcpy(&this->mGenerationEntityAbsMin, &inMin, sizeof(vec3_t));
	//memcpy(&this->mGenerationEntityAbsMax, &inMax, sizeof(vec3_t));
	this->mGenerationEntityAbsMin.x = inMin.x;
	this->mGenerationEntityAbsMin.y = inMin.y;
	this->mGenerationEntityAbsMin.z = inMin.z;

	this->mGenerationEntityAbsMax.x = inMax.x;
	this->mGenerationEntityAbsMax.y = inMax.y;
	this->mGenerationEntityAbsMax.z = inMax.z;
	
//	// Setting the generation entity also sets our base position
////	this->mBaseEntityPos.x = (this->mGenerationEntityAbsMax[0] + this->mGenerationEntityAbsMin[0])/2.0f;
////	this->mBaseEntityPos.y = (this->mGenerationEntityAbsMax[1] + this->mGenerationEntityAbsMin[1])/2.0f;
////	this->mBaseEntityPos.z = (this->mGenerationEntityAbsMax[2] + this->mGenerationEntityAbsMin[2])/2.0f;
//	this->mBaseEntityPos.x = (inMax.x + inMin.x)/2.0f;
//	this->mBaseEntityPos.y = (inMax.y + inMin.y)/2.0f;
//	this->mBaseEntityPos.z = (inMax.z + inMin.z)/2.0f;
	
	// Calculate approximate volume
	float xDiff = fabs(this->mGenerationEntityAbsMax[0] - this->mGenerationEntityAbsMin[0]);
	float yDiff = fabs(this->mGenerationEntityAbsMax[1] - this->mGenerationEntityAbsMin[1]);
	float zDiff = fabs(this->mGenerationEntityAbsMax[2] - this->mGenerationEntityAbsMin[2]);
	
	this->mGenerationEntityVolumeFactor = (xDiff/100.0f)*(yDiff/100.0f)*(zDiff/100.0f);
}

void
AvHParticleSystem::SetStartingVelocity()
{
	// Set starting velocity
	PDomainEnum theDomain = (PDomainEnum)this->mStartingVelocityShape;
	if(theDomain != PS_None)
	{
		// Attention: adding in mBaseEntityPos won't work for all domains...fix this up
		pVelocityD(theDomain, this->mStartingVelocityParams[0], this->mStartingVelocityParams[1], this->mStartingVelocityParams[2], this->mStartingVelocityParams[3],
			this->mStartingVelocityParams[4], this->mStartingVelocityParams[5], this->mStartingVelocityParams[6], this->mStartingVelocityParams[7]);
	}
}

void
AvHParticleSystem::Update(float inTime)
{
	// I could foresee problems with not updating unless we can see the system, so update always.  Check performance.
	//#ifdef AVH_CLIENT
	//if(this->GetIsVisible())
	//{
	//#endif
		// If max particles changed, recreate particle group.  This should only happen during editing, so if
		// it's a bit flaky or we lose memory or have glitches it might be OK
		if(this->mGroupMaxParticles != this->mMaxParticles)
		{
			this->Kill();
			
			this->mGroupMaxParticles = this->mMaxParticles;
			this->mGroup = pGenParticleGroups(1, this->mGroupMaxParticles);
			
			this->mUpdateFirst = true;
		}
		
		// Set the current particle group
		pCurrentGroup(this->mGroup);
		
		//this->UpdateFromWorld();
		
		if(this->mUpdateFirst)
		{
			this->UpdateFirst();
			this->mUpdateFirst = false;
		}
		
		// TODO: There should be nine params, not eight
		// TODO: Some of this stuff should only be set once at creation
		
		// Set time passed
		pTimeStep((float)inTime);
		
		// Generate particles as long as the system is alive
		if(!this->GetIsMarkedForDeletion())
		{
			// Generate particles based on rate or density
			int theGenerationRate = this->mGenerationRate + 15*this->mCustomData;
			int theNumParticlesToGenerate = (this->mUseDensity ? this->mGenerationEntityVolumeFactor*theGenerationRate : theGenerationRate);
			this->SetStartingVelocity();
			this->GenerateParticles(theNumParticlesToGenerate);
		}
		
		// Gravity.
		if(this->mUseWorldGravity)
		{
			// Apply world gravity to particles
			float flGravity = CVAR_GET_FLOAT( "sv_gravity" );
			this->mParticleGravity[2] = -flGravity;
		}
		pGravity(this->mParticleGravity[0], this->mParticleGravity[1], this->mParticleGravity[2]);
		
		// Kill off particles that have lived their life
		if(this->GetParticleLifetime() != -1)
		{
			pKillOld(this->GetParticleLifetime());
		}
		
		// Kill particles that were marked for deletion
		pKillOld(0, true);
		
		if(this->mCollide)
		{
			this->Collide(inTime);
		}
		
		//if(this->mVortex)
		//{
		//	pVortex(this->mBaseEntityPos.x, this->mBaseEntityPos.y, this->mBaseEntityPos.z, 0, 0, 1, 10, P_EPS, 300);
		//}
		
		// Move particles to their new positions.
		pMove();
		
		//#ifdef AVH_CLIENT
		//	}
		//#endif
}

#ifdef AVH_SERVER
void
AvHParticleSystem::UpdatePhysics(entvars_t* inPEV)
{
	ASSERT(inPEV);
	float theCurrentTime = (float)gpGlobals->time;

	// First thing, either update the particle system entity postion with the generation entity, or update ourself with 
	// the position of the entity (for particle systems that don't use generation entities)
	if(this->mGenerationEntityIndex != -1)
	{
		// Set position of particle system entity to be at the center of the generation entity
		entvars_t* theEntity = VARS(INDEXENT(this->mGenerationEntityIndex));
		if(!FNullEnt(theEntity))
		{
			// I don't understand why theEntity->origin isn't the center of theEntity->absmin and theEntity->absmax.
			vec3_t theEntityOrigin;
			theEntityOrigin.x = (theEntity->absmin.x + theEntity->absmax.x)/2.0f;
			theEntityOrigin.y = (theEntity->absmin.y + theEntity->absmax.y)/2.0f;
			theEntityOrigin.z = (theEntity->absmin.z + theEntity->absmax.z)/2.0f;

			UTIL_SetOrigin(inPEV, theEntityOrigin);

			// Update our bounding area
			this->SetGenerationEntityExtents(theEntity->absmin, theEntity->absmax);
		}
	} 
	// Now update our position from entity.  When the particle system doesn't use a generation entity, the 
	// position of the particle system is given by the "parent" entity.  Otherwise, our base entity position
	// is given from the position of our generation entity
	else
	{
		this->mBaseEntityPos.x = inPEV->origin.x;
		this->mBaseEntityPos.y = inPEV->origin.y;
		this->mBaseEntityPos.z = inPEV->origin.z;
	}
	
	// Every x seconds, update bounding box
	if((this->mTimeOfLastPhysicsUpdate == -1) || (theCurrentTime > this->mTimeOfLastPhysicsUpdate + this->mPhysicsUpdateTime))
	{
		// Only support one group right now
		pCurrentGroup(this->mGroup);
		
		int theNumParticles = pGetGroupCount();
		if(theNumParticles >= 1)
		{
			// Get all the particles
			pVector* pPos = new pVector[theNumParticles];
			pGetParticles(0, theNumParticles, (float*)pPos, NULL, NULL, NULL, NULL);
			
			// Make the default size bigger than a point
			vec3_t theDefaultRadius(64, 64, 64);
			vec3_t theMinCoord = inPEV->origin - theDefaultRadius;
			vec3_t theMaxCoord = inPEV->origin + theDefaultRadius;

			// Now set the size of the particle system.
//			if(this->mGenerationEntityIndex != -1)
//			{
//				theMinCoord = this->mGenerationEntityAbsMin;
//				theMaxCoord = this->mGenerationEntityAbsMax;
//			} 
			
			// Run through particles (choose only every nth one?)
			for(int i = 0; i < theNumParticles; i++)
			{
				// Get position of each, change the bounding box accordingly
				pVector& theParticle = pPos[i];

				float theX = theParticle.x;
				float theY = theParticle.y;
				float theZ = theParticle.z;

				// Assumes particles are at max size
				float theHalfSize = this->GetParticleScale()*(this->mParticleSize/2.0f);

				theMinCoord[0] = min(theMinCoord[0], (theX - theHalfSize));
				theMinCoord[1] = min(theMinCoord[1], (theY - theHalfSize));
				theMinCoord[2] = min(theMinCoord[2], (theZ - theHalfSize));

				theMaxCoord[0] = max(theMaxCoord[0], (theX + theHalfSize));
				theMaxCoord[1] = max(theMaxCoord[1], (theY + theHalfSize));
				theMaxCoord[2] = max(theMaxCoord[2], (theZ + theHalfSize));
			}

			// Set our new size
			vec3_t theMinSize = theMinCoord - inPEV->origin;
			vec3_t theMaxSize = theMaxCoord - inPEV->origin;
			UTIL_SetSize(inPEV, theMinSize, theMaxSize);

			// Update time
			this->mTimeOfLastPhysicsUpdate = theCurrentTime;

			delete [] pPos;
		}
	}

//	// Now update our position from entity.  When the particle system doesn't use a generation entity, the 
//	// position of the particle system is given by the "parent" entity.  Otherwise, our base entity position
//	// is given from the position of our generation entity
//	if(this->mGenerationEntityIndex == -1)
//	{
//		this->mBaseEntityPos.x = inPEV->origin.x;
//		this->mBaseEntityPos.y = inPEV->origin.y;
//		this->mBaseEntityPos.z = inPEV->origin.z;
//	}
//	else
//	{
//		//entvars_t* theEntity = VARS(INDEXENT(this->mEntity));
//		entvars_t* theEntity = VARS(INDEXENT(this->mGenerationEntityIndex));
//		if(!FNullEnt(theEntity))
//		//if(theEntity)
//		{
//			// Set position to center of entity bounding box
//			this->SetGenerationEntityExtents(theEntity->absmin, theEntity->absmax);
//			
//			// Update "parent" entity to follow the generation entity
//			inPEV->origin.x = this->mBaseEntityPos.x;
//			inPEV->origin.y = this->mBaseEntityPos.y;
//			inPEV->origin.z = this->mBaseEntityPos.z;
//		}
//	}
}
#endif


void
AvHParticleSystem::UpdateFirst()
{
	this->SetStartingVelocity();
	
	//  Set size
	pSize(this->mParticleSize);

	// Set initial lifetime to be random so we don't have a huge mass of particles dying at once
	int theParticleLifetime = this->GetParticleLifetime();
	if(theParticleLifetime > 0)
	{
		pStartingAge(theParticleLifetime/2.0f, theParticleLifetime/2.0f);
	}
	
	// Why was this here?
	//int theNumParticles = inTemplate->GetMaxParticles();
	//ASSERT(theNumParticles > 0);
	//this->GenerateParticles(theNumParticles);
	
	// Reset starting age
	if(theParticleLifetime > 0)
	{
		pStartingAge(0.0f, 0.0f);
	}
	
	if(this->mHasNormal)
	{
		// Assumes that 0, 0, 1 is the base (ie, particle systems were defined assuming that they hit a normal of 0,0,1)

		PDomainEnum theGenDomain = (PDomainEnum)this->mGenerationShape;

		Vector theXAxis, theYAxis;
		CreateOrthoNormalBasis(this->mNormal, theXAxis, theYAxis);

		if((theGenDomain == PS_Point) || (theGenDomain == PS_Blob) || (theGenDomain == PS_Sphere) || (theGenDomain == PS_Box))
		{
			// Rotate the first three parameters as a point
			vec3_t theGenerationVector((float)this->mGenerationParams[0], (float)this->mGenerationParams[1], (float)this->mGenerationParams[2]);
			TransformVector(theGenerationVector, theXAxis, theYAxis, this->mNormal, theGenerationVector);
			this->mGenerationParams[0] = (int)(theGenerationVector.x);
			this->mGenerationParams[1] = (int)(theGenerationVector.y);
			this->mGenerationParams[2] = (int)(theGenerationVector.z);
		}

		if((theGenDomain == PS_Box) || (theGenDomain == PS_Cone))
		{
			// Rotate the second three parameters as a point
			vec3_t theGenerationVector((float)this->mGenerationParams[3], (float)this->mGenerationParams[4], (float)this->mGenerationParams[5]);
			TransformVector(theGenerationVector, theXAxis, theYAxis, this->mNormal, theGenerationVector);
			this->mGenerationParams[3] = (int)(theGenerationVector.x);
			this->mGenerationParams[4] = (int)(theGenerationVector.y);
			this->mGenerationParams[5] = (int)(theGenerationVector.z);
		}

		// If a normal was specified, rotate the starting velocity params by this amount
		PDomainEnum theVelDomain = (PDomainEnum)this->mStartingVelocityShape;
		if((theVelDomain == PS_Point) || (theVelDomain == PS_Blob) || (theVelDomain == PS_Sphere) || (theVelDomain == PS_Box))
		{
			// Rotate the first three parameters as a point
			vec3_t theStartingVelocityVector((float)this->mStartingVelocityParams[0], (float)this->mStartingVelocityParams[1], (float)this->mStartingVelocityParams[2]);
			TransformVector(theStartingVelocityVector, theXAxis, theYAxis, this->mNormal, theStartingVelocityVector);
			this->mStartingVelocityParams[0] = (int)(theStartingVelocityVector.x);
			this->mStartingVelocityParams[1] = (int)(theStartingVelocityVector.y);
			this->mStartingVelocityParams[2] = (int)(theStartingVelocityVector.z);
		}
		
		if((theVelDomain == PS_Box) || (theVelDomain == PS_Cone))
		{
			// Rotate the second three parameters as a point
			vec3_t theStartingVelocityVector((float)this->mStartingVelocityParams[3], (float)this->mStartingVelocityParams[4], (float)this->mStartingVelocityParams[5]);
			TransformVector(theStartingVelocityVector, theXAxis, theYAxis, this->mNormal, theStartingVelocityVector);
			this->mStartingVelocityParams[3] = (int)(theStartingVelocityVector.x);
			this->mStartingVelocityParams[4] = (int)(theStartingVelocityVector.y);
			this->mStartingVelocityParams[5] = (int)(theStartingVelocityVector.z);
		}
	}
}

//void
//AvHParticleSystem::UpdateFromWorld()
//{
////	if(this->mEntity != -1)
////	{
////#ifdef AVH_CLIENT
////
////		cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(this->mEntity);
////		if(theEntity)
////		{
////			// Set position to center of entity bounding box
////			this->mBaseEntityPos.x = theEntity->origin.x;
////			this->mBaseEntityPos.y = theEntity->origin.y;
////			this->mBaseEntityPos.z = theEntity->origin.z;
////		}
////
////		// Update generation entity if there is one
////		if(this->mGenerationEntityIndex != -1)
////		{
////			theEntity = gEngfuncs.GetEntityByIndex(this->mGenerationEntityIndex);
////			if(theEntity)
////			{
////				this->SetGenerationEntityExtents(theEntity->curstate.mins, theEntity->curstate.mins);
////			}
////		}
////		
////#else
//		
//		entvars_t* theEntity = VARS(INDEXENT(this->mEntity));
//		if(theEntity)
//		{
//			// Set position to center of entity bounding box
//			this->SetGenerationEntityExtents(theEntity->absmin, theEntity->absmax);
//			
//			this->mBaseEntityPos.x = (this->mGenerationEntityAbsMax[0] + this->mGenerationEntityAbsMin[0])/2.0f;
//			this->mBaseEntityPos.y = (this->mGenerationEntityAbsMax[1] + this->mGenerationEntityAbsMin[1])/2.0f;
//			this->mBaseEntityPos.z = (this->mGenerationEntityAbsMax[2] + this->mGenerationEntityAbsMin[2])/2.0f;
//		}
//		
////#endif
////	}
////	else
////	{
////		this->mBaseEntityPos.x = this->mOrigin[0];
////		this->mBaseEntityPos.y = this->mOrigin[1];
////		this->mBaseEntityPos.z = this->mOrigin[2];
////	}
//}


