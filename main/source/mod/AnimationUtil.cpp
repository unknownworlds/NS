#include <math.h>
#include "types.h"

#ifdef AVH_CLIENT
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#include "cl_dll/r_studioint.h"
#include "common/com_model.h"
#include "common/cl_entity.h"
#include "common/vec_op.h"
#include "cl_dll/studio_util.h"

extern engine_studio_api_t IEngineStudio;

#endif

#ifdef AVH_SERVER
#include "common/mathlib.h"
#include "common/const.h"
#include "engine/eiface.h"
#include "engine/edict.h"
#include "dlls/enginecallback.h"
#endif

#include "mod/AnimationUtil.h"
#include "mod/AvHSpecials.h"
#include "util/MathUtil.h"

#define PITCH   0
#define YAW     1
#define ROLL    2 

//-----------------------------------------------------------------------------

void NS_AngleMatrix (const float *angles, float (*matrix)[4] )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;
	
	angle = angles[YAW] * (float(M_PI)*2 / 360);
	sy = sinf(angle);
	cy = cosf(angle);
	angle = angles[PITCH] * (float(M_PI)*2 / 360);
	sp = sinf(angle);
	cp = cosf(angle);
	angle = angles[ROLL] * (float(M_PI)*2 / 360);
	sr = sinf(angle);
	cr = cosf(angle);

	// matrix = (YAW * PITCH) * ROLL
	matrix[0][0] = cp*cy;
	matrix[1][0] = cp*sy;
	matrix[2][0] = -sp;
	matrix[0][1] = sr*sp*cy+cr*-sy;
	matrix[1][1] = sr*sp*sy+cr*cy;
	matrix[2][1] = sr*cp;
	matrix[0][2] = (cr*sp*cy+-sr*-sy);
	matrix[1][2] = (cr*sp*sy+-sr*cy);
	matrix[2][2] = cr*cp;
	matrix[0][3] = 0.0;
	matrix[1][3] = 0.0;
	matrix[2][3] = 0.0;
}

//-----------------------------------------------------------------------------

#ifdef AVH_SERVER

bool NS_GetEntityAnimationData(int inEntityIndex, NS_AnimationData& outAnimationData)
{

    edict_t* theEdict = g_engfuncs.pfnPEntityOfEntIndex(inEntityIndex);
    
    if (theEdict == NULL)
    {
        return false;
    }

    vec3_t theAngles;

    if (theEdict->v.iuser3 == AVH_USER3_ALIEN_PLAYER1)  
    {  
        VectorCopy(theEdict->v.vuser1, theAngles);
    }
    else
    {
        VectorCopy(theEdict->v.angles, theAngles);
    }

    NS_AngleMatrix(theAngles, outAnimationData.mMatrix);

    outAnimationData.mMatrix[0][3] = theEdict->v.origin[0];
    outAnimationData.mMatrix[1][3] = theEdict->v.origin[1];
    outAnimationData.mMatrix[2][3] = theEdict->v.origin[2];

	outAnimationData.mTime         = theEdict->v.animtime;
    outAnimationData.mFrame        = theEdict->v.frame;
    outAnimationData.mFrameRate    = theEdict->v.framerate;
    outAnimationData.mModelHeader  = (studiohdr_t*)(GET_MODEL_PTR(theEdict));
    outAnimationData.mSequence     = theEdict->v.sequence;
    outAnimationData.mGaitSequence = theEdict->v.gaitsequence;

    // Get the bounding box for the sequence.

    studiohdr_t* theModelHeader = outAnimationData.mModelHeader;

    if (outAnimationData.mModelHeader != NULL)
    {
        
        mstudioseqdesc_t* theSequence = (mstudioseqdesc_t*)((byte*)theModelHeader + theModelHeader->seqindex) + outAnimationData.mSequence;

        VectorCopy(theSequence->bbmin, outAnimationData.mMins);
        VectorCopy(theSequence->bbmax, outAnimationData.mMaxs);
    
    }

    return true;

}

#endif

//-----------------------------------------------------------------------------

#ifdef AVH_CLIENT

bool NS_GetEntityAnimationData(int inEntityIndex, NS_AnimationData& outAnimationData)
{

    cl_entity_t* theEntity = gEngfuncs.GetEntityByIndex(inEntityIndex);

    if (theEntity == NULL || theEntity->model == NULL)
    {
        return false;
    }

    vec3_t theAngles;

    if (theEntity->curstate.iuser3 == AVH_USER3_ALIEN_PLAYER1)  
    {  
        VectorCopy(theEntity->curstate.vuser1, theAngles);
    }
    else
    {
        VectorCopy(theEntity->curstate.angles, theAngles);
    }

    NS_AngleMatrix(theAngles, outAnimationData.mMatrix);

    outAnimationData.mMatrix[0][3] = theEntity->curstate.origin[0];
    outAnimationData.mMatrix[1][3] = theEntity->curstate.origin[1];
    outAnimationData.mMatrix[2][3] = theEntity->curstate.origin[2];
    
    outAnimationData.mTime         = theEntity->curstate.animtime;
    outAnimationData.mFrame        = theEntity->curstate.frame;
    outAnimationData.mFrameRate    = theEntity->curstate.framerate;
    outAnimationData.mModelHeader  = (studiohdr_t *)IEngineStudio.Mod_Extradata(theEntity->model);
    outAnimationData.mSequence     = theEntity->curstate.sequence;
    outAnimationData.mGaitSequence = theEntity->curstate.gaitsequence;

    // Get the bounding box for the sequence.

    studiohdr_t* theModelHeader = outAnimationData.mModelHeader;

    if (outAnimationData.mModelHeader != NULL)
    {

        mstudioseqdesc_t* theSequence = (mstudioseqdesc_t*)((byte*)theModelHeader + theModelHeader->seqindex) + outAnimationData.mSequence;

        VectorCopy(theSequence->bbmin, outAnimationData.mMins);
        VectorCopy(theSequence->bbmax, outAnimationData.mMaxs);
    
    }

    return true;

}

#endif

//-----------------------------------------------------------------------------

void NS_AngleQuaternion( float *angles, vec4_t quaternion )
{
	float		angle;
	float		sr, sp, sy, cr, cp, cy;

	// FIXME: rescale the inputs to 1/2 angle
	angle = angles[2] * 0.5f;
	sy = sinf(angle);
	cy = cosf(angle);
	angle = angles[1] * 0.5f;
	sp = sinf(angle);
	cp = cosf(angle);
	angle = angles[0] * 0.5f;
	sr = sinf(angle);
	cr = cosf(angle);

	quaternion[0] = sr*cp*cy-cr*sp*sy; // X
	quaternion[1] = cr*sp*cy+sr*cp*sy; // Y
	quaternion[2] = cr*cp*sy-sr*sp*cy; // Z
	quaternion[3] = cr*cp*cy+sr*sp*sy; // W
}

//-----------------------------------------------------------------------------

void NS_QuaternionSlerp( vec4_t p, vec4_t q, float t, vec4_t qt )
{
    
    int i;
	float	omega, cosom, sinom, sclp, sclq;

	// decide if one of the quaternions is backwards
	float a = 0;
	float b = 0;

	for (i = 0; i < 4; i++)
	{
		a += (p[i]-q[i])*(p[i]-q[i]);
		b += (p[i]+q[i])*(p[i]+q[i]);
	}
	if (a > b)
	{
		for (i = 0; i < 4; i++)
		{
			q[i] = -q[i];
		}
	}

	cosom = p[0]*q[0] + p[1]*q[1] + p[2]*q[2] + p[3]*q[3];

	if ((1.0 + cosom) > 0.000001f)
	{
		if ((1.0 - cosom) > 0.000001f)
		{
			omega = acosf( cosom );
			sinom = sinf( omega );
			sclp = sinf( (1.0f - t)*omega) / sinom;
			sclq = sinf( t*omega ) / sinom;
		}
		else
		{
			sclp = 1.0f - t;
			sclq = t;
		}
		for (i = 0; i < 4; i++) {
			qt[i] = sclp * p[i] + sclq * q[i];
		}
	}
	else
	{
		qt[0] = -q[1];
		qt[1] = q[0];
		qt[2] = -q[3];
		qt[3] = q[2];
		sclp = sinf( (1.0f - t) * (0.5f * float(M_PI)));
		sclq = sinf( t * (0.5f * float(M_PI)));
		for (i = 0; i < 3; i++)
		{
			qt[i] = sclp * p[i] + sclq * qt[i];
		}
	}
}

//-----------------------------------------------------------------------------

void NS_QuaternionMatrix( vec4_t quaternion, float (*matrix)[4] )
{
	matrix[0][0] = 1.0f - 2.0f * quaternion[1] * quaternion[1] - 2.0f * quaternion[2] * quaternion[2];
	matrix[1][0] = 2.0f * quaternion[0] * quaternion[1] + 2.0f * quaternion[3] * quaternion[2];
	matrix[2][0] = 2.0f * quaternion[0] * quaternion[2] - 2.0f * quaternion[3] * quaternion[1];

	matrix[0][1] = 2.0f * quaternion[0] * quaternion[1] - 2.0f * quaternion[3] * quaternion[2];
	matrix[1][1] = 1.0f - 2.0f * quaternion[0] * quaternion[0] - 2.0f * quaternion[2] * quaternion[2];
	matrix[2][1] = 2.0f * quaternion[1] * quaternion[2] + 2.0f * quaternion[3] * quaternion[0];

	matrix[0][2] = 2.0f * quaternion[0] * quaternion[2] + 2.0f * quaternion[3] * quaternion[1];
	matrix[1][2] = 2.0f * quaternion[1] * quaternion[2] - 2.0f * quaternion[3] * quaternion[0];
	matrix[2][2] = 1.0f - 2.0f * quaternion[0] * quaternion[0] - 2.0f * quaternion[1] * quaternion[1];
}

//-----------------------------------------------------------------------------

void NS_ConcatTransforms (const float in1[3][4], const float in2[3][4], float out[3][4])
{
	out[0][0] = in1[0][0] * in2[0][0] + in1[0][1] * in2[1][0] +
				in1[0][2] * in2[2][0];
	out[0][1] = in1[0][0] * in2[0][1] + in1[0][1] * in2[1][1] +
				in1[0][2] * in2[2][1];
	out[0][2] = in1[0][0] * in2[0][2] + in1[0][1] * in2[1][2] +
				in1[0][2] * in2[2][2];
	out[0][3] = in1[0][0] * in2[0][3] + in1[0][1] * in2[1][3] +
				in1[0][2] * in2[2][3] + in1[0][3];
	out[1][0] = in1[1][0] * in2[0][0] + in1[1][1] * in2[1][0] +
				in1[1][2] * in2[2][0];
	out[1][1] = in1[1][0] * in2[0][1] + in1[1][1] * in2[1][1] +
				in1[1][2] * in2[2][1];
	out[1][2] = in1[1][0] * in2[0][2] + in1[1][1] * in2[1][2] +
				in1[1][2] * in2[2][2];
	out[1][3] = in1[1][0] * in2[0][3] + in1[1][1] * in2[1][3] +
				in1[1][2] * in2[2][3] + in1[1][3];
	out[2][0] = in1[2][0] * in2[0][0] + in1[2][1] * in2[1][0] +
				in1[2][2] * in2[2][0];
	out[2][1] = in1[2][0] * in2[0][1] + in1[2][1] * in2[1][1] +
				in1[2][2] * in2[2][1];
	out[2][2] = in1[2][0] * in2[0][2] + in1[2][1] * in2[1][2] +
				in1[2][2] * in2[2][2];
	out[2][3] = in1[2][0] * in2[0][3] + in1[2][1] * in2[1][3] +
				in1[2][2] * in2[2][3] + in1[2][3];
}

//-----------------------------------------------------------------------------

void NS_CalcBonePosition(int frame, float s, mstudiobone_t* pbone, mstudioanim_t* panim, float* adj, float* pos)
{
	
    // This is ripped out of StudioModelRenderer.
        
    int					j, k;
	mstudioanimvalue_t	*panimvalue;

	for (j = 0; j < 3; j++)
	{
		pos[j] = pbone->value[j]; // default;
		if (panim->offset[j] != 0)
		{
			panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j]);
			/*
			if (i == 0 && j == 0)
				Con_DPrintf("%d  %d:%d  %f\n", frame, panimvalue->num.valid, panimvalue->num.total, s );
			*/
			
			k = frame;
			// DEBUG
			if (panimvalue->num.total < panimvalue->num.valid)
				k = 0;
			// find span of values that includes the frame we want
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
  				// DEBUG
				if (panimvalue->num.total < panimvalue->num.valid)
					k = 0;
			}
			// if we're inside the span
			if (panimvalue->num.valid > k)
			{
				// and there's more data in the span
				if (panimvalue->num.valid > k + 1)
				{
					pos[j] += (panimvalue[k+1].value * (1.0f - s) + s * panimvalue[k+2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[k+1].value * pbone->scale[j];
				}
			}
			else
			{
				// are we at the end of the repeating values section and there's another section with data?
				if (panimvalue->num.total <= k + 1)
				{
					pos[j] += (panimvalue[panimvalue->num.valid].value * (1.0f - s) + s * panimvalue[panimvalue->num.valid + 2].value) * pbone->scale[j];
				}
				else
				{
					pos[j] += panimvalue[panimvalue->num.valid].value * pbone->scale[j];
				}
			}
		}
		if ( pbone->bonecontroller[j] != -1 && adj )
		{
			pos[j] += adj[pbone->bonecontroller[j]];
		}
	}
}

//-----------------------------------------------------------------------------

void NS_CalcBoneAngles( int frame, float s, mstudiobone_t *pbone, mstudioanim_t *panim, float *adj, float *q )
{
	int					j, k;
	vec4_t				q1, q2;
	vec3_t				angle1, angle2;
	mstudioanimvalue_t	*panimvalue;

	for (j = 0; j < 3; j++)
	{
		if (panim->offset[j+3] == 0)
		{
			angle2[j] = angle1[j] = pbone->value[j+3]; // default;
		}
		else
		{
			panimvalue = (mstudioanimvalue_t *)((byte *)panim + panim->offset[j+3]);
			k = frame;
			// DEBUG
			if (panimvalue->num.total < panimvalue->num.valid)
				k = 0;
			while (panimvalue->num.total <= k)
			{
				k -= panimvalue->num.total;
				panimvalue += panimvalue->num.valid + 1;
				// DEBUG
				if (panimvalue->num.total < panimvalue->num.valid)
					k = 0;
			}
			// Bah, missing blend!
			if (panimvalue->num.valid > k)
			{
				angle1[j] = panimvalue[k+1].value;

				if (panimvalue->num.valid > k + 1)
				{
					angle2[j] = panimvalue[k+2].value;
				}
				else
				{
					if (panimvalue->num.total > k + 1)
						angle2[j] = angle1[j];
					else
						angle2[j] = panimvalue[panimvalue->num.valid+2].value;
				}
			}
			else
			{
				angle1[j] = panimvalue[panimvalue->num.valid].value;
				if (panimvalue->num.total > k + 1)
				{
					angle2[j] = angle1[j];
				}
				else
				{
					angle2[j] = panimvalue[panimvalue->num.valid + 2].value;
				}
			}
			angle1[j] = pbone->value[j+3] + angle1[j] * pbone->scale[j+3];
			angle2[j] = pbone->value[j+3] + angle2[j] * pbone->scale[j+3];
		}

        /*
		if (pbone->bonecontroller[j+3] != -1)
		{
			angle1[j] += adj[pbone->bonecontroller[j+3]];
			angle2[j] += adj[pbone->bonecontroller[j+3]];
		}
        */

	}

	if (!VectorCompare( angle1, angle2 ))
	{
		NS_AngleQuaternion( angle1, q1 );
		NS_AngleQuaternion( angle2, q2 );
		NS_QuaternionSlerp( q1, q2, s, q );
	}
	else
	{
		NS_AngleQuaternion( angle1, q );
	}
}

//-----------------------------------------------------------------------------

float NS_StudioEstimateFrame( mstudioseqdesc_t *pseqdesc, const NS_AnimationData& inAnimationData, float time, float inFrame)
{
	
    float dfdt;
    float f;

	if ( /*m_fDoInterp*/ 1 )
	{
		if ( time < inAnimationData.mTime  )
		{
			dfdt = 0;
		}
		else
		{
			dfdt = (time - inAnimationData.mTime) * inAnimationData.mFrameRate * pseqdesc->fps;

		}
	}
	else
	{
		dfdt = 0;
	}

	if (pseqdesc->numframes <= 1)
	{
		f = 0;
	}
	else
	{
		f = (inFrame * (pseqdesc->numframes - 1)) / 256.0f;
	}
 	
	f += dfdt;
	
	if (pseqdesc->flags & STUDIO_LOOPING) 
	{
		if (pseqdesc->numframes > 1)
		{
			f -= (int)(f / (pseqdesc->numframes - 1)) *  (pseqdesc->numframes - 1);
		}
		if (f < 0) 
		{
			f += (pseqdesc->numframes - 1);
		}
	}
	else 
	{
		if (f >= pseqdesc->numframes - 1.001f) 
		{
			f = pseqdesc->numframes - 1.001f;
		}
		if (f < 0.0) 
		{
			f = 0.0;
		}
	}
	
    // This logic is from CStudioModelRenderer::StudioCalcRotations.

	if (f > pseqdesc->numframes - 1)
	{
		f = 0;
	}
	else if (f < -0.01f)
	{
		f = -0.01f;
	}    
    
    return f;

}

//-----------------------------------------------------------------------------

mstudioanim_t* NS_GetAnimation(studiohdr_t* inModelHeader, mstudioseqdesc_t* inSequence)
{
	mstudioseqgroup_t* theSequenceGroup = (mstudioseqgroup_t*)((byte *)inModelHeader + inModelHeader->seqgroupindex) + inSequence->seqgroup;
	// : 0000573
	// Unless we actually check for null, we can get null references... 
	if (theSequenceGroup) { 
		return (mstudioanim_t*)((byte*)inModelHeader + theSequenceGroup->data + inSequence->animindex);
	}
	else {
		return NULL;
	}
	// :
}

//-----------------------------------------------------------------------------

void NS_GetBoneMatrices(const NS_AnimationData& inAnimationData, float time, NS_Matrix3x4 outBoneMatrix[])
{
	if (!inAnimationData.mModelHeader || inAnimationData.mSequence < 0 || inAnimationData.mFrame < 0)
    {
        return;
    }

    studiohdr_t* theModelHeader = inAnimationData.mModelHeader;

    // Get the world to object space transformation for the entity.

    mstudioseqdesc_t* theSequence = (mstudioseqdesc_t*)((byte*)theModelHeader + theModelHeader->seqindex) + inAnimationData.mSequence;

	if (!theSequence) {
		return;
	}

    float f = NS_StudioEstimateFrame(theSequence, inAnimationData, time, inAnimationData.mFrame);

	int frame = (int)f;
	float s = (f - frame);

    mstudiobone_t* theBones	= (mstudiobone_t*)((byte*)theModelHeader + theModelHeader->boneindex);
    mstudiobbox_t* theHitBoxes = (mstudiobbox_t*)((byte*)theModelHeader + theModelHeader->hitboxindex);
    
	// : 0000573
	// Unless we actually check for null, we can get null references... 
	// Regardless if the model is borked, the server shouldn't crash.
	// Also, why have NS_GetAnimation when it's not used? 
	mstudioanim_t* theAnimation = NS_GetAnimation(theModelHeader,theSequence);

	if (!theBones|| !theHitBoxes|| !theAnimation) 
	{
		return;
	}
	// :

    // Get the position and orientation of all of the bones in the skeleton.
    
    vec3_t theBonePos[MAXSTUDIOBONES];
    vec4_t theBoneAngles[MAXSTUDIOBONES];

    int i;

    for (i = 0; i < theModelHeader->numbones; ++i)
    {
        NS_CalcBonePosition(frame, s, &theBones[i], &theAnimation[i], NULL, theBonePos[i]);
        NS_CalcBoneAngles(frame, s, &theBones[i], &theAnimation[i], NULL, theBoneAngles[i]);
    }

    // Take the gait sequence into account.

	if (inAnimationData.mGaitSequence != 0 && inAnimationData.mGaitSequence != 255)
	{

        int theGaitSequenceIndex = max(min(inAnimationData.mGaitSequence, theModelHeader->numseq - 1), 0);

        mstudioseqdesc_t* theGaitSequence = (mstudioseqdesc_t*)((byte*)theModelHeader + theModelHeader->seqindex) + theGaitSequenceIndex;
        mstudioanim_t* theGaitAnimation = NS_GetAnimation(theModelHeader, theGaitSequence);

        // Compute the frame in the gait animation.

        float theGaitFrame = time * theGaitSequence->fps;

        while (theGaitFrame >= theGaitSequence->numframes)
        {
            theGaitFrame -= theGaitSequence->numframes;
        }

        theGaitFrame = theGaitFrame * 256 / (theGaitSequence->numframes - 1);
        
        float f = NS_StudioEstimateFrame(theGaitSequence, inAnimationData, time, theGaitFrame);

    	int frame = (int)f;
    	float s = (f - frame);

		for (i = 0; i < theModelHeader->numbones; i++)
		{

            if (strcmp(theBones[i].name, "Bip01 Spine") == 0)
            {
				break;
            }

            NS_CalcBonePosition(frame, s, &theBones[i], &theGaitAnimation[i], NULL, theBonePos[i]);
            NS_CalcBoneAngles(frame, s, &theBones[i], &theGaitAnimation[i], NULL, theBoneAngles[i]);
			
		}        
        
	}

    for (i = 0; i < theModelHeader->numbones; i++) 
	{
        
        NS_Matrix3x4 theRelMatrix;
		NS_QuaternionMatrix(theBoneAngles[i], theRelMatrix);

		theRelMatrix[0][3] = theBonePos[i][0];
		theRelMatrix[1][3] = theBonePos[i][1];
    	theRelMatrix[2][3] = theBonePos[i][2];

		if (theBones[i].parent == -1) 
		{
            NS_ConcatTransforms(inAnimationData.mMatrix, theRelMatrix, outBoneMatrix[i]);
		}
		else 
		{
			NS_ConcatTransforms(outBoneMatrix[theBones[i].parent], theRelMatrix, outBoneMatrix[i]);
		}
        
    }

}