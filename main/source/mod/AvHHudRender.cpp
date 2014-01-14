//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: All graphical HUD operations
//
// $Workfile: AvHHudRender.cpp $
// $Date: 2002/10/24 21:31:13 $
//
//-------------------------------------------------------------------------------
// $Log: AvHHudRender.cpp,v $
// Revision 1.25  2002/10/24 21:31:13  Flayra
// - Removed some parts of energy drawing (unneeded clutter)
// - New marine HUD artwork (mainhud.spr)
// - Reworked upgrade drawing to only draw one of a type of upgrade (this was the last thing for v1.0, so it's a bit yucky)
//
// Revision 1.24  2002/10/18 22:20:14  Flayra
// - Fixed alien mass crash when a player left the server
//
// Revision 1.23  2002/10/16 00:59:33  Flayra
// - Added titles for umbra and primal scream
// - Don't draw building circles for entities on the other team (not even for commander)
// - Tried drawing building circles for ghost buildings, but it was confusing
//
// Revision 1.22  2002/10/03 18:56:10  Flayra
// - Moved alien energy to fuser3
// - Changed limits for energy and resources
// - Draw order icons centered around order position
// - Don't draw health rings for opposing teams
//
// Revision 1.21  2002/09/25 20:48:37  Flayra
// - Allow more UI to draw when gestating
// - Only draw text for blip closest to reticle
// - Don't draw stuff when dead
// - Changed order blinking
//
// Revision 1.20  2002/09/23 22:19:58  Flayra
// - Added "paralyzed" indicator
// - HUD element repositioning and refactoring
// - Added alien build circles
// - Added visible motion-tracking sprite to marine HUD
// - Removed special siege sprite
//
// Revision 1.19  2002/09/09 19:57:33  Flayra
// - Fixed black edges in D3D
// - Added blinking "webbed" indicator
// - Refactored UI constants
// - Fixed help icons
// - Added hive info indicator
// - Draw more info as spectator
//
// Revision 1.18  2002/08/31 18:01:01  Flayra
// - Work at VALVe
//
// Revision 1.17  2002/08/16 02:38:44  Flayra
// - Draw "webbed" message
// - Draw health for buildings and players
// - Removed old overwatch code
//
// Revision 1.16  2002/08/09 01:03:36  Flayra
// - Started refactoring for moving variable sprite hole drawing into TriAPI
//
// Revision 1.15  2002/08/02 21:56:54  Flayra
// - Added reticle help, new tooltip system and much nicer order drawing!  Also changed jetpack label to use font, and refactored some sprite names.
//
// Revision 1.14  2002/07/26 23:05:06  Flayra
// - Generate numerical feedback for damage events
//
// Revision 1.13  2002/07/23 17:09:41  Flayra
// - Add ability to centered, translated strings, visually-smooth energy level, new hive sight info, draw parasited message, draw marine upgrades, new method of drawing alien upgrades (overlapping and offset)
//
// Revision 1.12  2002/07/10 14:42:26  Flayra
// - Removed cl_particleinfo drawing differences
//
// Revision 1.11  2002/07/08 17:07:56  Flayra
// - Started to add display of marine upgrade sprite, fixed bug where building indicators aren't displayed after a map change, info_location drawing changes, primal scream color tweak, removed old hive drawing code
//
// Revision 1.10  2002/07/01 21:36:23  Flayra
// - Added primal scream effect, added building ranges for ghost buildings, removed outdated code, removed mapping build sprite, call vidinit() on hive sight sprites
//
// Revision 1.9  2002/06/25 18:03:09  Flayra
// - Added info_locations, removed old weapon help system, added smooth resource swelling, lots of alien UI usability changes, fixed problem with ghost building
//
// Revision 1.8  2002/06/10 19:57:01  Flayra
// - Allow drawing just a portion of a texture when scaling it, draw team hierarchy for soldiers
//
// Revision 1.7  2002/06/03 16:49:20  Flayra
// - Help sprites moved into one animated sprite
//
// Revision 1.6  2002/05/28 17:49:06  Flayra
// - Hive sight sprite changes
//
// Revision 1.5  2002/05/23 02:33:42  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHHud.h"
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "mod/AvHConstants.h"
#include "mod/AvHClientVariables.h"
#include "mod/AvHSpecials.h"
#include "common/cl_entity.h"
#include "mod/AvHTitles.h"
#include "pm_shared/pm_debug.h"
#include "util/MathUtil.h"
#include "common/r_efx.h"
#include "cl_dll/eventscripts.h"
#include "mod/AvHSprites.h"
#include "ui/UIUtil.h"
#include "types.h"
#include <signal.h>
#include "common/com_model.h"
#include "cl_dll/studio_util.h"
#include "cl_dll/r_studioint.h"
#include "mod/AvHMiniMap.h"
#include "mod/AvHActionButtons.h"
#include "util/STLUtil.h"
#include "mod/AvHSharedUtil.h"
#include "common/event_api.h"
#include "mod/AvHScriptManager.h"
#include <p_vector.h>
#include <papi.h>
#include "mod/AvHParticleSystemManager.h"
#include "mod/AvHTeamHierarchy.h"
#include "mod/AvHClientUtil.h"
#include "mod/AvHTooltip.h"
#include "cl_dll/demo.h"
#include "common/demo_api.h"
#include "mod/AvHHudConstants.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHCommanderModeHandler.h"
#include "common/ref_params.h"
#include "mod/AvHTechImpulsePanel.h"
#include "mod/AvHServerVariables.h"
#include "mod/AvHSpriteAPI.h"
#include "mod/AvHParticleEditorHandler.h"
#include "mod/AvHAlienAbilityConstants.h"
#include <list>
#include "common/entity_types.h"

void IN_GetMousePos( int *mx, int *my );

// Externs
extern int					g_iVisibleMouse;
extern playermove_t*		pmove;
extern engine_studio_api_t	IEngineStudio;
extern "C" Vector			gPredictedPlayerOrigin;
extern vec3_t				v_origin;
extern vec3_t				v_angles;

//extern vec3_t gPlayerOrigin;
//extern vec3_t gPlayerAngles;
extern ref_params_s* pDemoRefParams;
extern vec3_t gPlaybackViewOrigin;
extern AvHCommanderModeHandler	gCommanderHandler;
extern AvHParticleEditorHandler	gParticleEditorHandler;
float kD3DErrorValue = 0.01f;


vec3_t GetViewOrigin()
{
	vec3_t theOrigin = v_origin;

	//cl_entity_t* theViewEntity = gEngfuncs.GetLocalPlayer();//gEngfuncs.GetViewModel();
	//if(theViewEntity && /*pDemoRefParams &&*/ gEngfuncs.pDemoAPI->IsPlayingback())
	//{
	//	//theOrigin = pDemoRefParams->vieworg;
	//	theOrigin = theViewEntity->origin;
	//}

	if(gEngfuncs.pDemoAPI->IsPlayingback())
	{
		theOrigin = gPlaybackViewOrigin;
	}

	return theOrigin;
	//	return v_origin;
}

void BuildLerpedPoint(float inXPercentage, float inYPercentage, const Vector& inUpperLeft, const Vector& inUpperRight, const Vector& inLowerLeft, Vector& outPoint)
{
	ASSERT(inXPercentage >= 0.0f);
	ASSERT(inXPercentage <= 1.0f);
	ASSERT(inYPercentage >= 0.0f);
	ASSERT(inYPercentage <= 1.0f);
	
	Vector theUpperLeftToUpperRight;
	VectorSubtract(inUpperRight, inUpperLeft, theUpperLeftToUpperRight);
	
	Vector theUpperLeftToLowerLeft;
	VectorSubtract(inLowerLeft, inUpperLeft, theUpperLeftToLowerLeft);
	
	Vector theXComp;
	VectorScale(theUpperLeftToUpperRight, inXPercentage, theXComp);
	
	Vector theYComp;
	VectorScale(theUpperLeftToLowerLeft, inYPercentage, theYComp);
	
	outPoint = inUpperLeft + theXComp + theYComp;
	
	//float theXPercentage = (outPoint.x - inUpperLeft.x)/(theXComp.x + theYComp.x);
	//float theYPercentage = (outPoint.y - inUpperLeft.y)/(theXComp.y + theYComp.y);
}

void CalculatePlaneInfo(const Vector& inUpperLeft, const Vector& inUpperRight, const Vector& inLowerLeft, float& outD, Vector& outPlaneNormal)
{
	// Cross two vectors for plane normal
	Vector theUpperRightToUpperLeft;
	VectorSubtract(inUpperLeft, inUpperRight, theUpperRightToUpperLeft);
	theUpperRightToUpperLeft = theUpperRightToUpperLeft.Normalize();
	
	Vector theUpperLeftToLowerLeft;
	VectorSubtract(inLowerLeft, inUpperLeft, theUpperLeftToLowerLeft);
	theUpperLeftToLowerLeft = theUpperLeftToLowerLeft.Normalize();

	// Calculate plane normal
	CrossProduct(theUpperRightToUpperLeft, theUpperLeftToLowerLeft, outPlaneNormal);
	
	// Plug in one of the points for D (Ax + By + Cz = -D)
	outD = -(outPlaneNormal.x*inUpperLeft.x + outPlaneNormal.y*inUpperLeft.y + outPlaneNormal.z*inUpperLeft.z);
}

void CalculatePointOnPlane(int inXPos, int inYPos, const Vector& inOrigin, const Vector& inPlaneNormal, float inPlaneD, Vector& outPoint)
{
	Vector theRay;
	CreatePickingRay(inXPos, inYPos, theRay);
	
	// Solve for parametric t
	float thePlaneA = inPlaneNormal.x;
	float thePlaneB = inPlaneNormal.y;
	float thePlaneC = inPlaneNormal.z;
	
	float theT = -(thePlaneA*inOrigin.x + thePlaneB*inOrigin.y + thePlaneC*inOrigin.z + inPlaneD)/(thePlaneA*theRay.x + thePlaneB*theRay.y + thePlaneC*theRay.z);
	
	// Now we have t, solve for the endpoint
	outPoint.x = inOrigin.x + theT*theRay.x;
	outPoint.y = inOrigin.y + theT*theRay.y;
	outPoint.z = inOrigin.z + theT*theRay.z;
}

void ProjectPointFromViewOrigin(int inDistanceToProject, int inScreenX, int inScreenY, Vector& outResult)
{
	Vector theRay;
	CreatePickingRay(inScreenX, inScreenY, theRay);
	
	// project default distance along picking ray
	VectorMA(GetViewOrigin(), inDistanceToProject, theRay, outResult);
}

void AvHHud::DrawTranslatedString(int inX, int inY, const char* inStringToTranslate, bool inCentered, bool inIgnoreUpgrades, bool inTrimExtraInfo, float alpha)
{
	// Translate
	string theTranslatedText;
	LocalizeString(inStringToTranslate, theTranslatedText);
	if(theTranslatedText[0] == '#')
	{
		theTranslatedText = theTranslatedText.substr(1, theTranslatedText.size());
	}

	if(inTrimExtraInfo)
	{
		AvHCUTrimExtraneousLocationText(theTranslatedText);
	}

	// Draw it
	if(theTranslatedText != "")
	{
		int theR, theG, theB;
		this->GetPrimaryHudColor(theR, theG, theB, inIgnoreUpgrades, false);

		char theCharBuffer[512];
		sprintf(theCharBuffer, "%s", theTranslatedText.c_str());

		theR *= alpha;
		theB *= alpha;
		theG *= alpha;

		if(inCentered)
		{
			this->DrawHudStringCentered(inX, inY, ScreenWidth(), theCharBuffer, theR, theG, theB);
		}
		else
		{
			this->DrawHudString(inX, inY, ScreenWidth(), theCharBuffer, theR, theG, theB);
		}
	}
}

bool gWarpHUDSprites = false;
float gWarpXAmount = 0.0f;
float gWarpYAmount = 0.0f;
float gWarpXSpeed = 0.0f;
float gWarpYSpeed = 0.0f;

void SetWarpHUDSprites(bool inMode, float inWarpXAmount = 0.0f, float inWarpYAmount = 0.0f, float inWarpXSpeed = 0.0f, float inWarpYSpeed = 0.0f)
{
	gWarpHUDSprites = inMode;
	gWarpXAmount = inWarpXAmount;
	gWarpYAmount = inWarpYAmount;
	gWarpXSpeed = inWarpXSpeed;
	gWarpYSpeed = inWarpYSpeed;
}

void DrawScaledHUDSprite(int inSpriteHandle, int inMode, int inRowsInSprite = 1, int inX = 0, int inY = 0, int inWidth = ScreenWidth(), int inHeight = ScreenHeight(), int inForceSpriteFrame = -1, float inStartU = 0.0f, float inStartV = 0.0f, float inEndU = 1.0f, float inEndV = 1.0f, float inRotateUVRadians = 0.0f, bool inUVWrapsOverFrames = false)
{
	// Count number of frames
	int theNumFrames = SPR_Frames(inSpriteHandle);
	
	//int theSpriteWidth = SPR_Width(inSpriteHandle, 0);
	//int theSpriteHeight = SPR_Height(inSpriteHandle, 0);
	//float theAspectRatio = theSpriteWidth/theSpriteHeight;
	
	// ASSERT that the the number of rows makes sense for the number of frames 
	ASSERT(theNumFrames > 0);
	float theFloatNumCols = (float)theNumFrames/inRowsInSprite;
	int theNumCols = (int)theFloatNumCols;
	ASSERT(theNumCols == theFloatNumCols);

	//int theNumRows = theNumFrames/theNumCols;
	int theNumRows = inRowsInSprite;

	// Allow scaling of one frame, without tiling
	if(inForceSpriteFrame != -1)
	{
		theNumRows = theNumCols = 1;
	}

	// Make sure coords are bounded to allowable ranges
	
	inStartU = min(max(inStartU, kD3DErrorValue), 1.0f - kD3DErrorValue);
	inStartV = min(max(inStartV, kD3DErrorValue), 1.0f - kD3DErrorValue);
	inEndU = min(max(inEndU, kD3DErrorValue), 1.0f - kD3DErrorValue);
	inEndV = min(max(inEndV, kD3DErrorValue), 1.0f - kD3DErrorValue);

	if(inRotateUVRadians != 0.0f)
	{
		// Rotate all the UV coords
		vec3_t theAngles(0.0f, 0.0f, inRotateUVRadians);
		float theMatrix[3][4];
		AngleMatrix(theAngles, theMatrix);

		float theRotatedValues[3];

		float theStartValues[3] = {inStartU, inStartV, 0.0f};
		VectorRotate(theStartValues, theMatrix, theRotatedValues);
		inStartU = theRotatedValues[0];
		inStartV = theRotatedValues[1];

		float theEndValues[3] = {inEndU, inEndV, 0.0f};
		VectorRotate(theEndValues, theMatrix, theRotatedValues);
		inEndU = theRotatedValues[0];
		inEndV = theRotatedValues[1];
	}
	
	// Calculate width and height of each quad
	int theQuadScreenWidth = inWidth/theNumCols;
	int theQuadScreenHeight = inHeight/theNumRows;
	
	//Vector thePickRay;
	//int theHalfWidth = ScreenWidth/2;
	//int theHalfHeight = ScreenHeight/2;
	//CreatePickingRay(theHalfWidth, theHalfHeight, thePickRay);
	
	//char gDebugMessage[256];
	//sprintf(gDebugMessage, "(%d, %d): %f, %f, %f", theHalfWidth, theHalfHeight, thePickRay.x, thePickRay.y, thePickRay.z);
	//CenterPrint(gDebugMessage);
	
	//float theRenderOrigin[3];
	//pVector theUp;
	//pVector theRight;
	//pVector theNormal;
	//IEngineStudio.GetViewInfo(theRenderOrigin, (float*)&theUp, (float*)&theRight, (float*)&theNormal);

	//Demo_WriteVector(TYPE_VIEWANGLES, v_angles);
	//Demo_WriteVector(TYPE_VIEWORIGIN, v_origin);
	
	vec3_t theRenderOrigin;
	VectorCopy(GetViewOrigin(), theRenderOrigin);

	vec3_t theForward, theRight, theUp;
	AngleVectors(v_angles, theForward, theRight, theUp);
	
	Vector theRenderOriginVector;
	theRenderOriginVector.x = theRenderOrigin[0];
	theRenderOriginVector.y = theRenderOrigin[1];
	theRenderOriginVector.z = theRenderOrigin[2];

	// This is the smallest value that displays and it still clips with the view model a little
	const int kDistanceToProject = 10;
	
	// create picking ray for upper left of quad
	Vector theUpperLeftRay;
	//CreatePickingRay(inX, inY, theUpperLeftRay);
	CreatePickingRay(0, 0, theUpperLeftRay);
	
	// create picking ray for lower left of quad
	Vector theLowerLeftRay;
	//CreatePickingRay(inX, inY+inHeight, theLowerLeftRay);
	CreatePickingRay(0, ScreenHeight(), theLowerLeftRay);
	
	// create picking ray for upper right of quad
	Vector theUpperRightRay;
	//CreatePickingRay(inX+inWidth, inY, theUpperRightRay);
	CreatePickingRay(ScreenWidth(), 0, theUpperRightRay);
	
	// project default distance along picking ray
	Vector theUpperLeftWorldPos;
	VectorMA(theRenderOrigin, kDistanceToProject, theUpperLeftRay, theUpperLeftWorldPos);
	
	Vector theUpperRightWorldPos;
	VectorMA(theRenderOrigin, kDistanceToProject, theUpperRightRay, theUpperRightWorldPos);

	Vector theLowerLeftWorldPos;
	VectorMA(theRenderOrigin, kDistanceToProject, theLowerLeftRay, theLowerLeftWorldPos);

	// Create formula for plane
	float thePlaneD;
	Vector thePlaneNormal;
	CalculatePlaneInfo(theUpperLeftWorldPos, theUpperRightWorldPos, theLowerLeftWorldPos, thePlaneD, thePlaneNormal);
	
	// loop through screen height
	int theCurrentFrame = 0;
	
	// Allow drawing of just one frame
	if(inForceSpriteFrame != -1)
	{
		theCurrentFrame = inForceSpriteFrame;
	}

	for(int i = 0; i < theNumRows; i++)
	{
		// loop through screen width
		for(int j = 0; j < theNumCols; j++)
		{
			// draw quad
			gEngfuncs.pTriAPI->RenderMode(inMode);
			gEngfuncs.pTriAPI->CullFace(TRI_NONE);
			//gEngfuncs.pTriAPI->Brightness(1);
			
			if(gEngfuncs.pTriAPI->SpriteTexture((struct model_s*)gEngfuncs.GetSpritePointer(inSpriteHandle), theCurrentFrame))
			{
				gEngfuncs.pTriAPI->Begin( TRI_TRIANGLE_STRIP );
				//gEngfuncs.pTriAPI->Color4f(1.0f, 1.0f, 1.0f, 1.0f);

				bool theIsFirstRow = (i == 0);
				bool theIsFirstCol = (j == 0);
				bool theIsLastRow = (i == (theNumRows-1));
				bool theIsLastCol = (j == (theNumCols-1));

				float theStartU = inStartU;
				float theStartV = inStartV;
				float theEndU = inEndU;
				float theEndV = inEndV;

				if(inUVWrapsOverFrames)
				{
					if((theNumCols > 1) && !theIsFirstCol)
					{
						theStartU = 0.0f;
					}
					if((theNumRows > 1) && !theIsFirstRow)
					{
						theStartV = 0.0f;
					}
					if((theNumCols > 1) && !theIsLastCol)
					{
						theEndU = 1.0f;
					}
					if((theNumRows > 1) && !theIsLastRow)
					{
						theEndV = 1.0f;
					}
				}

				// Optionally warp XY coords using current time
				int theWarpXStartAmount = 0;
				int theWarpYStartAmount = 0;
				int theWarpXEndAmount = 0;
				int theWarpYEndAmount = 0;

				if(gWarpHUDSprites)
				{
					float theCurrentTime = gHUD.GetTimeOfLastUpdate();
					float theNormXAmount = theCurrentTime*gWarpXSpeed - (int)(theCurrentTime*gWarpXSpeed); // Get fractional part of second
					float theNormYAmount = theCurrentTime*gWarpYSpeed - (int)(theCurrentTime*gWarpYSpeed);
					float theSinusoidalNormXAmount = cos(theNormXAmount*2.0f*M_PI);
					float theSinusoidalNormYAmount = sin(theNormYAmount*2.0f*M_PI);
					float theXAmount = theSinusoidalNormXAmount*gWarpXAmount;// - gWarpUAmount/2.0f;
					float theYAmount = theSinusoidalNormYAmount*gWarpYAmount;// - gWarpVAmount/2.0f;

					if(!theIsFirstCol)
					{
						theWarpXStartAmount = theXAmount*ScreenWidth();
					}
					if(!theIsLastCol)
					{
						theWarpXEndAmount = theXAmount*ScreenWidth();
					}
					if(!theIsFirstRow)
					{
						theWarpYStartAmount = theYAmount*ScreenHeight();
					}
					if(!theIsLastRow)
					{
						theWarpYEndAmount = theYAmount*ScreenHeight();
					}
				}
	
				// Compensate for gamma
				float theGammaSlope = gHUD.GetGammaSlope();
				float theColorComponent = 1.0f/theGammaSlope;
				gEngfuncs.pTriAPI->Color4f(theColorComponent, theColorComponent, theColorComponent, 1.0f);
				
				Vector thePoint;
	
				//float theMinXPercentage = (float)j/theNumCols;
				//float theMaxXPercentage = (float)(j+1)/theNumCols;
	
				//float theMinYPercentage = (float)i/theNumRows;
				//float theMaxYPercentage = (float)(i+1)/theNumRows;
	
				int theMinXPos = inX + ((float)j/theNumCols)*inWidth + theWarpXStartAmount;
				int theMinYPos = inY + ((float)i/theNumRows)*inHeight + theWarpYStartAmount;
	
				int theMaxXPos = inX + ((float)(j+1)/theNumCols)*inWidth + theWarpXEndAmount;
				int theMaxYPos = inY + ((float)(i+1)/theNumRows)*inHeight + theWarpYEndAmount;
				
				// Lower left
				Vector thePointOne;
				//BuildLerpedPoint(theMinXPercentage, theMaxYPercentage, theUpperLeftWorldPos, theUpperRightWorldPos, theLowerLeftWorldPos, thePointOne);
				CalculatePointOnPlane(theMinXPos, theMaxYPos, theRenderOriginVector, thePlaneNormal, thePlaneD, thePointOne);
				float theU = theStartU;
				float theV = theEndV;
				gEngfuncs.pTriAPI->TexCoord2f(theU, theV);// 0,1
				gEngfuncs.pTriAPI->Vertex3fv((float*)&thePointOne);
				
				// Upper left
				Vector thePointTwo;
				//BuildLerpedPoint(theMinXPercentage, theMinYPercentage, theUpperLeftWorldPos, theUpperRightWorldPos, theLowerLeftWorldPos, thePointTwo);
				CalculatePointOnPlane(theMinXPos, theMinYPos, theRenderOriginVector, thePlaneNormal, thePlaneD, thePointTwo);
				theU = theStartU;
				theV = theStartV;
				//gEngfuncs.pTriAPI->TexCoord2f(inStartU, inStartV); // 0,0
				gEngfuncs.pTriAPI->TexCoord2f(theU, theV);// 0,1
				gEngfuncs.pTriAPI->Vertex3fv((float*)&thePointTwo);
				
				// Lower right
				Vector thePointFour;
				//BuildLerpedPoint(theMaxXPercentage, theMaxYPercentage, theUpperLeftWorldPos, theUpperRightWorldPos, theLowerLeftWorldPos, thePointFour);
				CalculatePointOnPlane(theMaxXPos, theMaxYPos, theRenderOriginVector, thePlaneNormal, thePlaneD, thePointFour);
				theU = theEndU;
				theV = theEndV;
				//gEngfuncs.pTriAPI->TexCoord2f(inEndU, inEndV);// 1,1
				gEngfuncs.pTriAPI->TexCoord2f(theU, theV);// 0,1
				gEngfuncs.pTriAPI->Vertex3fv((float*)&thePointFour);

				// Upper right
				Vector thePointThree;
				//BuildLerpedPoint(theMaxXPercentage, theMinYPercentage, theUpperLeftWorldPos, theUpperRightWorldPos, theLowerLeftWorldPos, thePointThree);
				CalculatePointOnPlane(theMaxXPos, theMinYPos, theRenderOriginVector, thePlaneNormal, thePlaneD, thePointThree);
				theU = theEndU;
				theV = theStartV;
				//gEngfuncs.pTriAPI->TexCoord2f(inEndU, inStartV); // 1,0
				gEngfuncs.pTriAPI->TexCoord2f(theU, theV);// 0,1
				gEngfuncs.pTriAPI->Vertex3fv((float*)&thePointThree);
	
				gEngfuncs.pTriAPI->End();
			}
	
			// Increment frame
			theCurrentFrame++;
			
			// Allow drawing of just one frame
			if(inForceSpriteFrame != -1)
			{
				theCurrentFrame = inForceSpriteFrame;
			}
			
			// increment current x and y
			//theCurrentScreenX += theQuadScreenWidth;
		}
		//theCurrentScreenX = 0;
		//theCurrentScreenY += theQuadScreenHeight;
	}
}

void DrawVariableScaledHUDSprite(float inFactor, int inSpriteHandle, int inMode, int inX, int inY, int inWidth, int inHeight)
{
	// Draw as two scaled sprites, one for the level and one for the "empty" level
	// Assumes that sprite has two frames, with the empty level being frame 0 and the full frame being frame 1
	int theWidth = inWidth;
	float theStartU = 0.0f;
	float theEndU = 1.0f;

	int theHeight = inFactor*inHeight;
	float theStartV = 1.0f - inFactor;
	float theEndV = 1.0f;
	int theX = inX;
	int theY = inY + inHeight - theHeight;
	DrawScaledHUDSprite(inSpriteHandle, inMode, 1, theX, theY, theWidth, theHeight, 1, theStartU, theStartV, theEndU, theEndV);

	// Draw background
	theHeight = (1.0f - inFactor)*inHeight;
	theY = inY;
	theStartV = 0.0f;
	theEndV = 1.0f - inFactor;
	DrawScaledHUDSprite(inSpriteHandle, inMode, 1, theX, theY, theWidth, theHeight, 0, theStartU, theStartV, theEndU, theEndV);
}

void DrawSpriteOnGroundAtPoint(vec3_t inOrigin, int inRadius, HSPRITE inSprite, int inRenderMode = kRenderNormal, int inFrame = 0, float inAlpha = 1.0f)
{
	if(gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer(inSprite), inFrame))
	{
		gEngfuncs.pTriAPI->CullFace(TRI_NONE);
		
		gEngfuncs.pTriAPI->RenderMode(inRenderMode);
		
		//gEngfuncs.pTriAPI->RenderMode( kRenderTransTexture );
		//gEngfuncs.pTriAPI->Color4f(inR, inG, inB, inA);
		
		gEngfuncs.pTriAPI->Begin(TRI_TRIANGLE_STRIP);
		
		// Draw one quad
		vec3_t thePoint = inOrigin;
		
		float theGammaSlope = gHUD.GetGammaSlope();
		ASSERT(theGammaSlope > 0.0f);
		float theColorComponent = 1.0f/theGammaSlope;
		
		gEngfuncs.pTriAPI->Color4f(theColorComponent, theColorComponent, theColorComponent, inAlpha);
		gEngfuncs.pTriAPI->Brightness(1.6f);
		
		thePoint[0] = inOrigin[0] - inRadius;
		thePoint[1] = inOrigin[1] - inRadius;
		gEngfuncs.pTriAPI->TexCoord2f(0, 0);
		gEngfuncs.pTriAPI->Vertex3f(thePoint[0], thePoint[1], thePoint[2]);
		
		thePoint[0] = inOrigin[0] - inRadius;
		thePoint[1] = inOrigin[1] + inRadius;
		gEngfuncs.pTriAPI->TexCoord2f(0, 1);
		gEngfuncs.pTriAPI->Vertex3f(thePoint[0], thePoint[1], thePoint[2]);

		thePoint[0] = inOrigin[0] + inRadius;
		thePoint[1] = inOrigin[1] - inRadius;
		gEngfuncs.pTriAPI->TexCoord2f(1, 0);
		gEngfuncs.pTriAPI->Vertex3f(thePoint[0], thePoint[1], thePoint[2]);
		
		thePoint[0] = inOrigin[0] + inRadius;
		thePoint[1] = inOrigin[1] + inRadius;
		gEngfuncs.pTriAPI->TexCoord2f(1, 1);
		gEngfuncs.pTriAPI->Vertex3f(thePoint[0], thePoint[1], thePoint[2]);
		
		gEngfuncs.pTriAPI->End();
		gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
	}
}

void AvHHud::DrawPlayerNames()
{
	bool inReadyRoom = false;//(this->GetPlayMode() == PLAYMODE_READYROOM);
	bool inTopDownMode = this->GetInTopDownMode();

	if(inTopDownMode || inReadyRoom /*&& !gEngfuncs.pDemoAPI->IsPlayingback()*/)
	{
		cl_entity_s* theLocalPlayer = gEngfuncs.GetLocalPlayer();
		if(theLocalPlayer)
		{
			int theLocalPlayerIndex = theLocalPlayer->index;

			// Loop through all players
			for(int i = 1; i <= kMaxPlayers; i++)
			{
				if((i != theLocalPlayerIndex) && AvHCUGetIsEntityInPVSAndVisible(i))
				{
					cl_entity_s* theCurrentPlayer = gEngfuncs.GetEntityByIndex(i);
					bool theDrawEntity = (inReadyRoom || (theCurrentPlayer && theCurrentPlayer->player && (AvHTeamNumber(theCurrentPlayer->curstate.team) == this->GetHUDTeam()) && (i != theLocalPlayerIndex)) );
					if(theDrawEntity)
					{
						string theEntityName;
						bool theIsEnemy;
						if(this->GetEntityInfoString(i, theEntityName, theIsEnemy))
						{
							vec3_t theEntityOrigin;
							VectorCopy(theCurrentPlayer->curstate.origin, theEntityOrigin);
							theEntityOrigin.z += AvHCUGetIconHeightForPlayer((AvHUser3)theCurrentPlayer->curstate.iuser3);
						
							// If they are on screen
							Vector theScreenPos;
							if(AvHCUWorldToScreen(theEntityOrigin, (float*)&theScreenPos))
							{
								// Set color
								int theR, theG, theB;
								this->GetPrimaryHudColor(theR, theG, theB, true, false);
						
								// If selected, draw in different color
								if(inTopDownMode)
								{
									bool theIsSelected = (std::find(this->mSelected.begin(), this->mSelected.end(), i) != this->mSelected.end());
									if(theIsSelected)
									{
										// Selected color
										UnpackRGB(theR, theG, theB, RGB_MARINE_SELECTED);
									
										if(GetHasUpgrade(theCurrentPlayer->curstate.iuser4, MASK_PARASITED))
										{
											string thePrePendString;
											LocalizeString(kParasited, thePrePendString);
											theEntityName = string(theEntityName + " (" + thePrePendString + ")");
									
											// Set color to parasited color
											UnpackRGB(theR, theG, theB, RGB_MARINE_PARASITED);
										}
									}
								}
						
								// Set text color draw in different color
								this->mTopDownPlayerNameMessage.SetRGB(theR, theG, theB);
								this->mTopDownPlayerNameMessage.SetIgnoreFadeForLifetime(true);
						
								// Set the message info and draw it
								this->mTopDownPlayerNameMessage.SetText(theEntityName);
						
								// Set position
								Vector theNormPos;
								float theNormX = theScreenPos.x/ScreenWidth();
								
								int theBoxHeight = this->mTopDownPlayerNameMessage.GetScreenHeight();
								float theNormY = (theScreenPos.y - theBoxHeight)/ScreenHeight();
						
								if((inTopDownMode && !this->GetIsRegionBlockedByUI(theNormX, theNormY)) || inReadyRoom)
								{
									this->mTopDownPlayerNameMessage.SetNormalizedScreenX(theNormX);
									this->mTopDownPlayerNameMessage.SetNormalizedScreenY(theNormY);
									this->mTopDownPlayerNameMessage.SetCentered(true);
									this->mTopDownPlayerNameMessage.SetNormalizedMaxWidth(kReticleMaxWidth);
									this->mTopDownPlayerNameMessage.Draw();
								}
							}
						}
					}
				}
			}
		}
	}
}

//bool AvHHud::ChopStringOfMaxScreenWidth(int inMaxScreenWidth, string& ioBaseString, string& outChoppedString)
//{
//	// Loop backwards through the string, until we get a string that fits in this screen width
//	int theCurrentLength = ioBaseString.length();
//	int theMaxLength = ioBaseString.length();
//	bool theSuccess = false;
//
//	while(!theSuccess)
//	{
//		string theCurrentString = ioBaseString.substr(0, theCurrentLength);
//		int theCurrentStringScreenWidth = this->GetHudStringWidth(theCurrentString.c_str());
//		if(theCurrentStringScreenWidth <= inMaxScreenWidth)
//		{
//			// Look for a word to break the line
//			while((theCurrentLength > 0) && !theSuccess)
//			{
//				char theCurrentChar = ioBaseString[theCurrentLength-1];
//				if((theCurrentChar == ' ') || (theCurrentLength == theMaxLength))
//				{
//					outChoppedString = ioBaseString.substr(0, theCurrentLength);
//					ioBaseString = ioBaseString.substr(theCurrentLength, ioBaseString.length() - theCurrentLength);
//					theSuccess = true;
//					break;
//				}
//				else
//				{
//					theCurrentLength--;
//				}
//			}
//		}
//		else
//		{
//			theCurrentLength--;
//		}
//	}
//
//	return theSuccess;
//}

void AvHHud::DrawReticleInfo()
{
	this->mReticleMessage.Draw();
	
	//	if(this->mReticleInfoText != "")
	//	{
	//		const float kMaxWidth = .3f;
	//		int kMaxScreenWidth = kMaxWidth*ScreenWidth;
	//
	//		StringList theStringList;
	//		string theHelpString = this->mReticleInfoText;
	//
	//		do
	//		{
	//			string theNewString;
	//			if(ChopStringOfMaxScreenWidth(kMaxScreenWidth, theHelpString, theNewString))
	//			{
	//				theStringList.push_back(theNewString);
	//			}
	//			else
	//			{
	//				theHelpString = "";
	//			}
	//		} 
	//		while(theHelpString != "");
	//		
	//		// For each line, if the line contains any special markers, move them to their own lines
	//		
	//		// Compute max width of all the strings, add some extra for a frame
	//		int theBoxWidth = 0;
	//		StringList::iterator theStringListIter;
	//		for(theStringListIter = theStringList.begin(); theStringListIter != theStringList.end(); theStringListIter++)
	//		{
	//			int theCurrentScreenWidth = this->GetHudStringWidth(theStringListIter->c_str());
	//			theBoxWidth = max(theBoxWidth, theCurrentScreenWidth);
	//		}
	//		int theLineHSpacing = .01f*ScreenWidth;
	//		theBoxWidth += 2*theLineHSpacing;
	//
	//		// Compute max height needed to contain all the strings, add some extra for a frame
	//		int theLineVSpacing = .01f*ScreenHeight();
	//		int theLineHeight = this->GetHudStringHeight();
	//		int theBoxHeight = 2*theLineVSpacing + (theStringList.size()*theLineHeight);
	//
	//		int theFillStartX = this->mReticleInfoScreenX;
	//		int theFillStartY = this->mReticleInfoScreenY;
	//
	//		theFillStartX -= theBoxWidth/2;	
	//		theFillStartY -= theBoxHeight/2;
	//
	//		// Draw nice border and shaded background
	//		const float kReticleInfoMaxAlpha = 25;
	//		float theNormalizedAlpha = this->mReticleInfoColorA/255;
	//		int theAlphaComponent = theNormalizedAlpha*kReticleInfoMaxAlpha;
	//
	//		FillRGBA(theFillStartX, theFillStartY, theBoxWidth, theBoxHeight, this->mReticleInfoColorR, this->mReticleInfoColorG, this->mReticleInfoColorB, theAlphaComponent);
	//		vguiSimpleBox(theFillStartX, theFillStartY, theFillStartX + theBoxWidth, theFillStartY + theBoxHeight, this->mReticleInfoColorR, this->mReticleInfoColorG, this->mReticleInfoColorB, theAlphaComponent);
	//
	//		// Now draw each line, non-centered, left-aligned
	//		int theLineNumber = 0;
	//		for(theStringListIter = theStringList.begin(); theStringListIter != theStringList.end(); theStringListIter++)
	//		{
	//			int theR = this->mReticleInfoColorR;
	//			int theG = this->mReticleInfoColorG;
	//			int theB = this->mReticleInfoColorB;
	//
	//			// If the line starts with a marker, draw it in a special color
	//			//string theDamageMarker(kDamageMarker);
	//			//if(theStringListIter->substr(0, theDamageMarker.length()) == theDamageMarker)
	//			//{
	//			//	// Draw string in yellow
	//			//	theR = theG = 255;
	//			//	theB = 25;
	//			//}
	//
	//			int theBaseY = theFillStartY + theLineVSpacing + theLineNumber*theLineHeight;
	//			
	//			// Draw message (DrawHudStringCentered only centers in x)
	//			this->DrawHudString(theFillStartX + theLineHSpacing, theBaseY /*- theLineHeight/2*/, ScreenWidth, theStringListIter->c_str(), theR*theNormalizedAlpha, theG*theNormalizedAlpha, theB*theNormalizedAlpha);
	//
	//			theLineNumber++;
	//		}
	//	}
}

void AvHHud::DrawToolTips()
{
	if(!gEngfuncs.pDemoAPI->IsPlayingback())
	{
		this->mHelpMessage.Draw();
		
		// Draw each one
		for(AvHTooltipListType::iterator theIter = this->mTooltips.begin(); theIter != this->mTooltips.end(); theIter++)
		{
			int theR, theG, theB;
			this->GetPrimaryHudColor(theR, theG, theB, true, false);
			theIter->SetRGB(theR, theG, theB);
			theIter->Draw();
		}
	}
}

void AvHHud::DrawWorldSprite(int inSpriteHandle, int inRenderMode, vec3_t inWorldPosition, int inFrame, float inWorldSize, float inAlpha)
// : added inAlpha
{
	vec3_t theUpperLeft;
	vec3_t theLowerRight;

	vec3_t theForward, theRight, theUp;
	AngleVectors(v_angles, theForward, theRight, theUp);

	vec3_t theToUpperLeft;
	VectorAdd(-theRight, theUp, theToUpperLeft);
	VectorNormalize(theToUpperLeft);

	VectorMA(inWorldPosition, inWorldSize, theToUpperLeft, theUpperLeft);

	vec3_t theToLowerRight;
	VectorAdd(theRight, -theUp, theToLowerRight);
	VectorNormalize(theToLowerRight);

	VectorMA(inWorldPosition, inWorldSize, theToLowerRight, theLowerRight);

	vec3_t theScreenUpperLeft;
	vec3_t theScreenLowerRight;

	// World to screen returns true if the world pos is behind the viewer
	if(!gEngfuncs.pTriAPI->WorldToScreen((float*)theUpperLeft, (float*)theScreenUpperLeft))
	{
		if(!gEngfuncs.pTriAPI->WorldToScreen((float*)theLowerRight, (float*)theScreenLowerRight))
		{
			// If the sprite is behind you, push it to the bottom or top of the screen
//			cl_entity_t* theLocalPlayer = gEngfuncs.GetLocalPlayer();
//			ASSERT(theLocalPlayer);
//			
//			vec3_t theDirectionToOrder;
//			VectorSubtract(inWorldPosition, theLocalPlayer->origin, theDirectionToOrder);
			
			//		float theDotProduct = DotProduct(theDirectionToOrder, theLocalPlayer->angles);
			//		if(theDotProduct < 0)
			//		{
			//			if(theWorldPos.z < theLocalPlayer->origin.z)
			//			{
			//				theY = theScreenHeight - theSpriteHeight - theScreenBorder;
			//			}
			//			else
			//			{
			//				theY = theScreenBorder;
			//			}
			
			//			vec3_t theCrossProduct;
			//			theCrossProduct = CrossProduct(theLocalPlayer->angles, theDirectionToOrder);
			//
			//			// It's to our right
			//			if(theCrossProduct.z > 0)
			//			{
			//				theX = theScreenWidth - theSpriteWidth - theScreenBorder;
			//			}
			//			else
			//			{
			//				theX = theScreenBorder;
			//			}
			//		}
			
//			float theDistanceToLocation = (float)VectorDistance(inWorldPosition, theLocalPlayer->origin);
//			const theMaxDistance = 1500;
//			float theEffectiveDistance = min(theDistanceToLocation, theMaxDistance);
//			const int theMinColorComponent = 100;
//			int theColorComponent = max(theMinColorComponent, 255 - ((theEffectiveDistance/theMaxDistance)*255));
			
			//SPR_Set(inSpriteHandle, theColorComponent, theColorComponent, theColorComponent);
			////SPR_DrawHoles((int)0, theX, theY, NULL);
			//if(inRenderMode == kRenderNormal)
			//{
			//	SPR_Draw(inFrame, theX, theY, NULL);
			//}
			//else if(inRenderMode == kRenderTransAdd)
			//{
			//	SPR_DrawAdditive(inFrame, theX, theY, NULL);
			//}

			float theScreenX = XPROJECT(theScreenUpperLeft.x);
			float theScreenY = YPROJECT(theScreenUpperLeft.y);
			float theWidth = XPROJECT(theScreenLowerRight.x) - theScreenX;
			float theHeight = YPROJECT(theScreenLowerRight.y) - theScreenY;

            //DrawScaledHUDSprite(inSpriteHandle, inRenderMode, 1, theScreenX, theScreenY, theWidth, theHeight, inFrame);
            
            AvHSpriteSetColor(1, 1, 1, inAlpha);
            AvHSpriteSetRenderMode(inRenderMode);
            AvHSpriteDraw(inSpriteHandle, inFrame, theScreenX, theScreenY, theScreenX + theWidth, theScreenY + theHeight, 0, 0, 1, 1);
            

            
		}
	}
}

void AvHHud::DrawOrderIcon(const AvHOrder& inOrder)
{
	if(this->mOrderSprite)
	{
		int theNumFrames = SPR_Frames(this->mOrderSprite);
		int theCurrentFrame = this->GetFrameForOrderType(inOrder.GetOrderType());
		
		if((theCurrentFrame >= 0) && (theCurrentFrame < theNumFrames))
		{
			vec3_t theWorldPos;
			inOrder.GetLocation(theWorldPos);
			if ( inOrder.GetOrderType() == ORDERTYPET_ATTACK  ) {
				theWorldPos[2]+=kAttackOrderZOffset;
			}

			// Draw icon above pos, text below
			theWorldPos.z += BALANCE_VAR(kOrderIconDrawSize);
			
			this->DrawWorldSprite(this->mOrderSprite, kRenderTransAdd, theWorldPos, theCurrentFrame, BALANCE_VAR(kOrderIconDrawSize));
			
			// If the order is our own order, draw the order indicator around it
			if((this->GetHUDPlayMode() == PLAYMODE_PLAYING) && this->GetIsMarine() && !this->GetInTopDownMode())
			{
				this->DrawWorldSprite(this->mMarineOrderIndicator, kRenderTransAdd, theWorldPos, 0, BALANCE_VAR(kOrderIconDrawSize));
				//DrawScaledHUDSprite(theSpriteHandle, kRenderNormal, 1, thePosX, thePosY, theWidth, theHeight, theFrame, theStartU, theStartV, theEndU, theEndV);
				
			}

			vec3_t orderDir;
			inOrder.GetLocation(orderDir);
			this->GetOrderDirection(orderDir, 2);
		}
	}
}

void AvHHud::DrawOrderText(const AvHOrder& inOrder)
{
	int theIndex = (int)(inOrder.GetOrderType());

	// Now draw text describing the waypoint
	string theTitle;
	sprintf(theTitle, "Order%d", theIndex);
	
	string theLocalizedTitle(" ");
	LocalizeString(theTitle.c_str(), theLocalizedTitle);

	if((theIndex == ORDERTYPET_BUILD) || (theIndex == ORDERTYPET_GUARD) || (theIndex == ORDERTYPET_GET))
	{
		AvHUser3 theUser3 = inOrder.GetTargetUser3Type();
		string theUser3Name;
		if(this->GetTranslatedUser3Name(theUser3, theUser3Name))
		{
			// "Get %s" -> "Get heavy machine gun"
			// "Guard %s -> "Guard soldier"
			// "Build %s" -> "Build sentry turret"
			string theTitleWithTarget;
			sprintf(theTitleWithTarget, theLocalizedTitle.c_str(), theUser3Name.c_str());
			theLocalizedTitle = theTitleWithTarget;
		}
	}

	vec3_t theOrderLocation;
	inOrder.GetLocation(theOrderLocation);
	
	// Because the commander may not have information about the players heading to this waypoint (outside of his PVS), we
	// can't draw a range for the commander
	string theRangeDisplayString;
	if(!this->GetInTopDownMode())
	{
		float theDistanceToWaypoint = VectorDistance(gPredictedPlayerOrigin, theOrderLocation);
		int theVisibleDistance = max(1, (int)theDistanceToWaypoint/100);
		
		string theVisibleUnits;
		if(LocalizeString("Range", theVisibleUnits))
		{
			sprintf(theRangeDisplayString, theVisibleUnits.c_str(), theVisibleDistance);
		}
	}
	
	string theLocationOfOrder;
	theLocationOfOrder = this->GetNameOfLocation(theOrderLocation);

	// It's OK if this fails, as only official maps will have these translated
	string theTranslatedLocation = theLocationOfOrder;
	LocalizeString(theLocationOfOrder.c_str(), theTranslatedLocation);
	
	// : 0000992	
	string theFirstLine = theLocalizedTitle;
	if(theRangeDisplayString != "")
	{
		theFirstLine += string(" : ") + theRangeDisplayString;
	}
	// :
	
	Vector theScreenPos;
	if(AvHCUWorldToScreen((float*)theOrderLocation, (float*)&theScreenPos))
	{
		float theNormX = theScreenPos.x/ScreenWidth();
		float theNormY = theScreenPos.y/ScreenHeight();

		if(!this->GetIsRegionBlockedByUI(theNormX, theNormY))
		{
			int theR, theG, theB;
			this->GetPrimaryHudColor(theR, theG, theB, false, false);
			
			string theFirstLine = theLocalizedTitle;
			if(theRangeDisplayString != "")
			{
				theFirstLine += string(" : ") + theRangeDisplayString;
			}
			
			// Draw order (icon above world position, text below it)
			int theBaseX = theScreenPos.x;
			int theBaseY = theScreenPos.y;
			int theStringHeight = this->GetHudStringHeight();
			this->DrawHudStringCentered(theBaseX, theBaseY + theStringHeight, ScreenWidth(), theFirstLine.c_str(), theR, theG, theB);
			
			// Draw location below it
			this->DrawHudStringCentered(theBaseX, theBaseY + 2*theStringHeight, ScreenWidth(), theTranslatedLocation.c_str(), theR, theG, theB);
		}
	}
	// : 0000992
	if (this->mDisplayOrderType == 2)
	{
		// this->mDisplayOrderText1 = "The commander issued an order:";
		this->mDisplayOrderText1 = theFirstLine.c_str();
		this->mDisplayOrderText2 = theTranslatedLocation.c_str();
	}
	// :	
}

// :
#define CENTER_TEXT_LENGTH	10
#define CENTER_TEXT_FADEOUT	2
void AvHHud::DrawCenterText()
{
	if ((this->mCenterTextTime > -1) && (this->mTimeOfLastUpdate < this->mCenterTextTime + CENTER_TEXT_LENGTH + CENTER_TEXT_FADEOUT))
	{
		int theR, theG, theB;
		this->GetPrimaryHudColor(theR, theG, theB, false, false);

		if (this->mTimeOfLastUpdate > this->mCenterTextTime + CENTER_TEXT_LENGTH) 
		{
			float fraction = this->mTimeOfLastUpdate - (this->mCenterTextTime + CENTER_TEXT_LENGTH);
			fraction = 1 - fraction / CENTER_TEXT_FADEOUT;
			theR *= fraction;
			theG *= fraction;
			theB *= fraction;
		}

		int posX = 0.5 * ScreenWidth() - this->mFont.GetStringWidth(this->mCenterText.c_str()) / 2;
		int posY = 0.4 * ScreenHeight();

		this->mFont.DrawString(posX, posY, this->mCenterText.c_str(), theR, theG, theB);
	}
}
// :

// : 0000992
void AvHHud::SetDisplayOrder(int inOrderType, int inOrderIndex, string inText1, string inText2, string inText3)
{
	this->mDisplayOrderTime = this->mTimeOfLastUpdate;
	this->mDisplayOrderType = inOrderType;
	this->mDisplayOrderIndex = inOrderIndex;
	this->mDisplayOrderText1 = inText1;
	this->mDisplayOrderText2 = inText2;
	this->mDisplayOrderText3 = inText3;
}

void AvHHud::DrawDisplayOrder()
{
	const float flashLength = 1.0f;
	const float fadeLimit	= 6.0f;
	const float fadeEnd		= 2.0f;
	
	if ((this->mDisplayOrderType > 0) && (this->mDisplayOrderTime + fadeLimit + fadeEnd) > this->mTimeOfLastUpdate && (this->GetInTopDownMode() == false))
	{
		float theFade = 1.0f;
		if ((this->mDisplayOrderTime + fadeLimit) < this->mTimeOfLastUpdate)
		{
			theFade = 1.0f - (this->mTimeOfLastUpdate - (this->mDisplayOrderTime + fadeLimit)) / fadeEnd;
			if(theFade < 0.0f)
			{
				this->mDisplayOrderType = 0;
				return;
			}
		}

		// flash the icon for the first second
		if ((this->mDisplayOrderTime + flashLength) > this->mTimeOfLastUpdate)
		{
			if (((int)((this->mTimeOfLastUpdate - this->mDisplayOrderTime) * 8)) % 2)
			{
				theFade = 0.0f;
			}
		}

		// draw the panel
//        int sprite = Safe_SPR_Load(kWhiteSprite);
        
        int r, g, b;
        GetPrimaryHudColor(r, g, b, true, false);

		int theStringHeight = this->GetHudStringHeight();

		float mIconX1 = 0.47f * ScreenWidth();
		float mIconY1 = 0.10f * ScreenHeight();
		float mIconX2 = mIconX1 + 0.06f * ScreenWidth();
		float mIconY2 = mIconY1 + 0.06f * ScreenWidth();
		float mLeftX  = mIconX1 - 0.06f * ScreenWidth();
		float mRightX = mIconX2 + 0.06f * ScreenWidth();

		float mTextX1 = 0.50f * ScreenWidth();

		AvHSpriteSetRenderMode(kRenderTransAdd);
        AvHSpriteSetDrawMode(kSpriteDrawModeFilled);
		AvHSpriteSetColor(1, 1, 1, 1 * theFade);

		int theTeamAdd = 0;
		if (this->GetIsAlien())
            theTeamAdd = 2;

		if (this->mDisplayOrderDirection == 1)
			AvHSpriteDraw(this->mTeammateOrderSprite, TEAMMATE_MARINE_LEFT_ARROW + theTeamAdd, 
				mLeftX, mIconY1, mIconX1, mIconY2, 0, 0, 1, 1); // Left
		else if (this->mDisplayOrderDirection == 2)
			AvHSpriteDraw(this->mTeammateOrderSprite, TEAMMATE_MARINE_RIGHT_ARROW + theTeamAdd, 
				mIconX2, mIconY1, mRightX, mIconY2, 0, 0, 1, 1); // Right

		if (this->mDisplayOrderType == 1)
		{
			AvHSpriteDraw(this->mTeammateOrderSprite, this->mDisplayOrderIndex + 8, mIconX1, mIconY1, mIconX2, mIconY2, 0, 0, 1, 1);
			this->DrawHudStringCentered(mTextX1, mIconY2, ScreenWidth(), this->mDisplayOrderText1.c_str(), r, g, b);
		}
		else if (this->mDisplayOrderType == 2)
		{
			AvHSpriteDraw(this->mOrderSprite, this->mDisplayOrderIndex, mIconX1, mIconY1, mIconX2, mIconY2, 0, 0, 1, 1);
			this->DrawHudStringCentered(mTextX1, mIconY2, ScreenWidth(), this->mDisplayOrderText1.c_str(), r, g, b);
			this->DrawHudStringCentered(mTextX1, mIconY2 + theStringHeight, ScreenWidth(), this->mDisplayOrderText2.c_str(), r, g, b);
		}

//		float mTextX1 = mIconX2 + 0.02 * ScreenWidth();
//		this->DrawHudString(mTextX1, mIconY1, ScreenWidth(), this->mDisplayOrderText1.c_str(), r, g, b);
//		this->DrawHudString(mTextX1, mIconY1 + theStringHeight, ScreenWidth(), this->mDisplayOrderText2.c_str(), r, g, b);
//		this->DrawHudString(mTextX1, mIconY1 + theStringHeight * 2, ScreenWidth(), this->mDisplayOrderText3.c_str(), r, g, b);
	}
}
// :

// : 0000971
void AvHHud::GetOrderDirection(vec3_t inTarget, int inOrderType)
{
	if (this->mDisplayOrderType == inOrderType)
	{
		// find left/right/none direction for the order popup notificator
		vec3_t theForward, theRight, theUp, theDir;
		AngleVectors(v_angles, theForward, theRight, theUp);
		VectorSubtract(inTarget, v_origin, theDir);
		theForward[2] = theDir[2] = 0;
		VectorNormalize(theForward);
		VectorNormalize(theDir);

		this->mDisplayOrderDirection = 0;
		// if it is too close to screen center, ignore it
		if (DotProduct(theForward, theDir) < 0.9f)
		{
			// Determine left and right
			vec3_t theCrossProd = CrossProduct(theForward, theDir);
			if (theCrossProd[2] > 0.0f)
				this->mDisplayOrderDirection = 1; // Left
			else if (theCrossProd[2] < 0.0f)
				this->mDisplayOrderDirection = 2; // Right
		}
	}
}

void AvHHud::DrawTeammateOrders()
{
	TeammateOrderListType::iterator toErase = NULL;
	cl_entity_s* theLocalPlayer = gEngfuncs.GetLocalPlayer();

	const float flashLength = 1.0f;
	const float fadeLimit	= 6.0f;
	const float fadeEnd		= 2.0f;

	for(TeammateOrderListType::iterator theIter = this->mTeammateOrder.begin(); theIter != this->mTeammateOrder.end(); theIter++)
	{
		float lastOrder = 0;
		TeammateOrderType theOrder = (*theIter).second;
		int theEntIndex = (*theIter).first;
		float theFade = 1.0f;

		// remove the order if it has expired
		if((theOrder.second + fadeEnd + fadeLimit) < this->mTimeOfLastUpdate)
		{
			toErase = theIter;
			continue;
		}
		// draw the order fading away
		else if((theOrder.second + fadeLimit) < this->mTimeOfLastUpdate)
		{
			theFade = 1.0f - (this->mTimeOfLastUpdate - (theOrder.second + fadeLimit)) / fadeEnd;
			if(theFade < 0.0f)
				theFade = 0.0f;
		}
		// else, draw the order normally

		cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(theEntIndex);
		if (theEntity && (theEntIndex < MAX_PLAYERS && theEntIndex >= 0) && (theEntity->index != theLocalPlayer->index))
		{
			if (AvHTraceLineAgainstWorld(theLocalPlayer->origin, theEntity->origin) == 1.0f)
			{
				vec3_t theVec;
				VectorCopy(theEntity->origin, theVec);
				theVec[2] += AvHCUGetIconHeightForPlayer((AvHUser3)theEntity->curstate.iuser3);
				this->DrawWorldSprite(this->mTeammateOrderSprite, kRenderTransAdd, theVec, theOrder.first, kHelpIconDrawSize, theFade);
				
				if (lastOrder < theOrder.second)
				{
					lastOrder = theOrder.second;
					this->GetOrderDirection(theVec, 1);
				}
			}
		}
	}

	if (toErase != NULL)
		this->mTeammateOrder.erase(toErase);

	// flash target player
	if (((this->mCurrentOrderTime + flashLength) > this->mTimeOfLastUpdate) && (this->mCurrentOrderTarget > 0))
	{
		if (((int)((this->mTimeOfLastUpdate - (this->mCurrentOrderTime + flashLength)) * 8)) % 2)
		{
			cl_entity_s* theTargetEntity = gEngfuncs.GetEntityByIndex(this->mCurrentOrderTarget);

			vec3_t theVec;
			VectorCopy(theTargetEntity->origin, theVec);
			theVec[2] += AvHCUGetIconHeightForPlayer((AvHUser3)theTargetEntity->curstate.iuser3);
			this->DrawWorldSprite(this->mTeammateOrderSprite, kRenderTransAdd, theVec, this->mCurrentOrderType, kHelpIconDrawSize, 1.0f);

		}
	}


}
// :

void AvHHud::DrawOrders()
{
	if(1/*!this->mIsRenderingSelectionView*/)
	{
		// Draw them blinking for soldiers, but always for commanders
		float theFractionalLastUpdate = this->mTimeOfLastUpdate - (int)this->mTimeOfLastUpdate;
		if((theFractionalLastUpdate > .25f) || (this->GetHUDUser3() == AVH_USER3_COMMANDER_PLAYER))
		{
			OrderListType theOrders = this->GetOrderList();
			
			EntityListType theDrawPlayerList = this->GetDrawPlayerOrders();
			
			// Run through the order list type
			for(OrderListType::iterator theIter = theOrders.begin(); theIter != theOrders.end(); theIter++)
			{
				// For each one, if the order is for a player in the theDrawPlayerList, draw it
				vec3_t theOrderLocation;
				theIter->GetLocation(theOrderLocation);
				
				if(theIter->GetOrderTargetType() == ORDERTARGETTYPE_TARGET)
				{
					int theTargetIndex = theIter->GetTargetIndex();
					cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(theTargetIndex);
					if(theEntity)
					{
						//: dont follow if they are cloaked, leave the waypoint active so they have a clue where they may be at, the wp should snap back to the baddy
						//once they are spotted again.

						if(theEntity->curstate.rendermode != kRenderTransTexture && theEntity->curstate.renderamt > 128)
							VectorCopy(theEntity->origin, theOrderLocation);
					}
				}
			
				// Draw the order if the order is for any plays that are in our draw player list
				bool theDrawWaypoint = false;
				EntityInfo theReceiverPlayer = theIter->GetReceiver();
				EntityListType::iterator theSearchIter = std::find(theDrawPlayerList.begin(), theDrawPlayerList.end(), theReceiverPlayer);
				if(theSearchIter != theDrawPlayerList.end() && *theSearchIter == theReceiverPlayer)
				{
					//gEngfuncs.pEfxAPI->R_ParticleLine((float*)theEntity->origin, (float*)theOrderLocation, 0, 255, 0, .05f);
					theDrawWaypoint = true;
				}
				if(theDrawWaypoint)
				{
					this->DrawOrderIcon(*theIter);
					this->DrawOrderText(*theIter);
				}
			}
		}
	}
}

int AvHHud::GetHelpIconFrameFromUser3(AvHUser3 inUser3)
{
	int theFrame = -1;

	switch(inUser3)
	{
	case AVH_USER3_WELD:
		theFrame = 0;
		break;
	case AVH_USER3_MARINEITEM:
		theFrame = 1;
		break;
	case AVH_USER3_HIVE:
		theFrame = 2;
		break;
	//case AVH_USER3_USEABLE:
	//	theFrame = 3;
	//	break;
	case AVH_USER3_COMMANDER_STATION:
		theFrame = 4;
		break;
	//case AVH_USER3_BREAKABLE:
	//	theFrame = 5;
	//	break;
	}

	return theFrame;
}

HSPRITE	AvHHud::GetHelpSprite() const
{
	return this->mHelpSprite;
}

void AvHHud::DrawHelpIcons()
{
	// Lookup table

	// Only draw if enabled
	//if(gEngfuncs.pfnGetCvarFloat(kvAutoHelp))
	//{
		// Iterate through help icons, drawing each one if we have an sprite for it
		for(HelpIconListType::iterator theIter = this->mHelpIcons.begin(); theIter != this->mHelpIcons.end(); theIter++)
		{
			int theUser3 = theIter->second;
			int theFrame = GetHelpIconFrameFromUser3(AvHUser3(theUser3));
						
			// Lookup sprite to see if it's loaded
//			if(this->mHelpSprites[theUser3] == 0)
//			{
//				string theIconName = string(kHelpIconPrefix) + MakeStringFromInt(theUser3) + ".spr";
//				this->mHelpSprites[theUser3] = Safe_SPR_Load(theIconName.c_str());
//			}
//			
//			int theSpriteHandle = this->mHelpSprites[theUser3];
//			if(theSpriteHandle > 0)
//			{
//				// Draw icon at entity center
//				this->DrawWorldSprite(theSpriteHandle, kRenderTransAdd, theIter->first, 0);
//			}

			if((theFrame >= 0) && this->mHelpSprite)
			{
				this->DrawWorldSprite(this->mHelpSprite, kRenderTransAdd, theIter->first, theFrame, kHelpIconDrawSize);
			}
		}
	//}
}

// inDrawMode determines if we're drawing text or sprites
void AvHHud::DrawHUDStructureNotification()
{
	const float	kHUDStructureNotificationStartX = .02f;
	const float	kHUDStructureNotificationStartY = .11f;
	const float	kHUDStructureNotificationIconWidth = .03f;
	const float	kHUDStructureNotificationIconHeight = kHUDStructureNotificationIconWidth*1.333f;
	const float	kHUDStructureNotificationIconHorizontalSpacing = .01f;
	const float	kHUDStructureNotificationIconVerticalSpacing = kHUDStructureNotificationIconHorizontalSpacing*1.333f;
	const float	kHUDStructureNotificationMaxTextWidth = .2f;

	// Draw them all in order
	if(this->GetIsAlive() && CVAR_GET_FLOAT(kvBuildMessages))
	{
		// Get starting coords
		float theCurrentX = kHUDStructureNotificationStartX;
		float theCurrentY = kHUDStructureNotificationStartY;

		
		for(StructureHUDNotificationListType::iterator theIter = this->mStructureNotificationList.begin(); theIter != this->mStructureNotificationList.end(); theIter++)
		{
			// Draw icon
			AvHMessageID theIconTech = theIter->mStructureID;
			int theFrame = 0;
			this->DrawTechTreeSprite(theIconTech, theCurrentX*ScreenWidth(), theCurrentY*ScreenHeight(), kHUDStructureNotificationIconWidth*ScreenWidth(), kHUDStructureNotificationIconHeight*ScreenHeight(), theFrame);

			string theLocationName = this->GetNameOfLocation(theIter->mLocation);
			if(theLocationName != "")
			{
				int theStartX = (theCurrentX + kHUDStructureNotificationIconWidth + kHUDStructureNotificationIconHorizontalSpacing)*ScreenWidth();
				this->DrawTranslatedString(theStartX, theCurrentY*ScreenHeight(), theLocationName.c_str(), false, true);
			}
            
			// Increment coords
			theCurrentY += (kHUDStructureNotificationIconHeight + kHUDStructureNotificationIconVerticalSpacing);
		}
		
	}
}

void AvHHud::DrawInfoLocationText()
{
	string theTimeLeftText;

	// Get drawing color and position
	int theR, theG, theB;
	this->GetPrimaryHudColor(theR, theG, theB, false, false);
			
	// Get position of health and draw to the right of it (must be left-justified)
	int theX = 0;
	int theY = 0;
	if(this->GetInTopDownMode())
	{
		const float kLeftInset = .4f;
		theX = mViewport[0] + kLeftInset*ScreenWidth();
		theY = mViewport[1] + mViewport[3] - (1-.78f)*ScreenHeight();
	}
	// Draw info location text the same for aliens and marines
	else if(this->GetHUDUser3() != AVH_USER3_ALIEN_EMBRYO)
	{
		const float kLeftInset = .08f;
		theX = mViewport[0] + kLeftInset*ScreenWidth();
		theY = mViewport[1] + mViewport[3] - (1-.88f)*ScreenHeight();
	}
	int theLeftEdge = theX;
	
	// Draw location text, translation if possible
	string theLocalizedLocationText;
	if(this->mLocationText != "")
	{
		LocalizeString(this->mLocationText.c_str(), theLocalizedLocationText);
		if(theLocalizedLocationText[0] == '#')
		{
			theLocalizedLocationText = theLocalizedLocationText.substr(1, theLocalizedLocationText.size());
		}

		// Draw handicap text as well
		int theHandicap = (int)this->GetHUDHandicap();
		if(theHandicap < 100)
		{
			// draw "(handicap 70%)"
			string theHandicapString;
			if(LocalizeString(kHandicap, theHandicapString))
			{
				char theFormattedHandicapString[256];
				sprintf(theFormattedHandicapString, " (%d%% %s)", theHandicap, theHandicapString.c_str());
				theLocalizedLocationText += string(theFormattedHandicapString);
			}
		}

		// Draw info location
		if(theLocalizedLocationText != "")
		{
			char theCharArray[512];
			sprintf(theCharArray, "%s", theLocalizedLocationText.c_str());
			
			this->DrawHudString(theX, theY, ScreenWidth(), theCharArray, theR, theG, theB);
		}
	}

	// Don't draw time when playing back, it isn't right and not worth breaking backward-compatibility over.
	// TODO: Draw time next time demo version changes
	if(!gEngfuncs.pDemoAPI->IsPlayingback() && this->GetHUDUser3() != AVH_USER3_ALIEN_EMBRYO)
	{
		// Whether we draw first line or not, increment for second line below
		theY += this->GetHudStringHeight();

		// Optionally draw time left below it
		int theTimeLimitSeconds = (int)this->GetGameTimeLimit();
		int theTimeElapsed = this->GetGameTime();
		int theMinutesElapsed = theTimeElapsed/60;
		int theSecondsElapsed = theTimeElapsed%60;
		int theTimeLeftSeconds = theTimeLimitSeconds - theTimeElapsed;
		int theMinutesLeft = theTimeLeftSeconds/60;
		int theSecondsLeft = theTimeLeftSeconds%60;
		int theMinutesLimit = theTimeLimitSeconds/60;
		int theSecondsLimit = theTimeLimitSeconds%60;

		// If we're in tournament mode or playing Combat, draw the timelimit
		bool theTournyMode = (gHUD.GetServerVariableFloat(kvTournamentMode) > 0);
		bool theDisplayTimeLimit = theTournyMode || gHUD.GetIsCombatMode();

		string theGameTimeText;
		if(LocalizeString(kGameTime, theGameTimeText))
		{
			bool theTimeVisible = true;

			// Flash time when we're almost out of time
			if((theMinutesLeft < 1) && this->GetGameStarted() && theDisplayTimeLimit)
			{
				float theTime = gHUD.GetTimeOfLastUpdate();
				float theTimeFraction = theTime - floor(theTime);
				if(theTimeFraction < .5f)
				{
					theTimeVisible = false;
				}
			}

			// Game time - 12:43
			char theGameTimeCStr[256];
			sprintf(theGameTimeCStr, " - %02d:%02d", theMinutesElapsed, theSecondsElapsed);
			theGameTimeText += string(theGameTimeCStr);

			if(theTimeVisible)
			{
				this->DrawHudString(theX, theY, ScreenWidth(), theGameTimeText.c_str(), theR, theG, theB);
			}

			// Increment X so time limit is drawn properly
			theX += this->GetHudStringWidth(theGameTimeText.c_str());
		}

		if(theDisplayTimeLimit)
		{
			string theTimeLimitString;
			if(LocalizeString(kTimeLimit, theTimeLimitString))
			{
				// Format: Time limit - 60:00
				char theTimeLimitCStr[256];
				sprintf(theTimeLimitCStr, "  %s - %02d:%02d", theTimeLimitString.c_str(), theMinutesLimit, theSecondsLimit);
				
				string theTimeLimitText = string(theTimeLimitCStr);

				this->DrawHudString(theX, theY, ScreenWidth(), theTimeLimitText.c_str(), theR, theG, theB);
			}

			if(gHUD.GetIsCombatMode())
			{
				theY += this->GetHudStringHeight();
				
				// Draw "attacking" or "defending"
				AvHTeamNumber theTeamNumber = this->GetHUDTeam();
				string theDisplayString = "";
				if(theTeamNumber != TEAM_IND)
				{
					if(theTeamNumber == this->GetCombatAttackingTeamNumber())
					{
						LocalizeString(kAttacking, theDisplayString);
					}
					else
					{
						LocalizeString(kDefending, theDisplayString);
					}

					this->DrawHudString(theLeftEdge, theY, ScreenWidth(), theDisplayString.c_str(), theR, theG, theB);
				}
			}
		}
	}
}

void AvHHud::DrawMouseCursor(int inBaseX, int inBaseY)
{
	if ( g_iVisibleMouse && !(this->GetInTopDownMode() && gEngfuncs.pDemoAPI->IsPlayingback()) )
	{

        HSPRITE theCursorSprite;
        int theCursorFrame;

        GetCursor(theCursorSprite, theCursorFrame);

		if (theCursorSprite > 0)
		{
			float theGammaSlope = this->GetGammaSlope();
			ASSERT(theGammaSlope > 0.0f);

            /*
			int theColorComponent = 255/theGammaSlope;
			SPR_Set(this->mCursorSprite, theColorComponent, theColorComponent, theColorComponent);
			
			// Draw the logo at 20 fps
			//SPR_DrawAdditive( 0, this->mMouseCursorX - inBaseX, this->mMouseCursorY - inBaseY, NULL );
			const int kMouseHotSpotX = -1;
			const int kMouseHotSpotY = -1;
			SPR_DrawHoles(this->mCurrentCursorFrame, this->mMouseCursorX - inBaseX - kMouseHotSpotX, this->mMouseCursorY - inBaseY - kMouseHotSpotY, NULL );
            */

            // Draw the mouse cursor.

			const int kMouseHotSpotX = -1;
			const int kMouseHotSpotY = -1;
            
            AvHSpriteBeginFrame();

            AvHSpriteEnableVGUI(true);
            AvHSpriteSetVGUIOffset(inBaseX, inBaseY);

            float x = this->mMouseCursorX - kMouseHotSpotX;
            float y = this->mMouseCursorY - kMouseHotSpotY;

            float w = SPR_Width(theCursorSprite, theCursorFrame);
            float h = SPR_Height(theCursorSprite, theCursorFrame);

            AvHSpriteSetRenderMode(kRenderTransAlpha);
			AvHSpriteDraw(theCursorSprite, theCursorFrame, x, y, x + w, y + h, 0, 0, 1, 1);
            
            // Draw the marquee if it's visible.
			
            if (mSelectionBoxVisible)
            {
				
                int sprite = Safe_SPR_Load(kWhiteSprite);
                
                int r, g, b;
                GetPrimaryHudColor(r, g, b, true, false);
				
                AvHSpriteSetRenderMode(kRenderTransAdd);
                AvHSpriteSetColor(r / 255.0, g / 255.0, b / 255.0, 0.3);
				
                AvHSpriteSetDrawMode(kSpriteDrawModeFilled);
                AvHSpriteDraw(sprite, 0, mSelectionBoxX1 + 1, mSelectionBoxY1 + 1, mSelectionBoxX2 - 1, mSelectionBoxY2 - 1, 0, 0, 1, 1);
				
                AvHSpriteSetRenderMode(kRenderNormal);
                AvHSpriteSetColor(1, 1, 1, 1);
				
                AvHSpriteSetDrawMode(kSpriteDrawModeBorder);
                AvHSpriteDraw(sprite, 0, mSelectionBoxX1, mSelectionBoxY1, mSelectionBoxX2, mSelectionBoxY2, 0, 0, 1, 1);
				
            }            
            
            AvHSpriteEndFrame();

        }
	}
}



void AvHHud::DrawTopDownBG()
{
	// If we're in top down mode, draw a black background
	float theDrawBackgroundHeight = -1;//cl_drawbg->value;
	
	// Draw the bottom plane at the map's min view height, unless overridden by cl_drawbg (this variable is for for testing purposes only)
	if(theDrawBackgroundHeight == -1)
	{
		if(this->mMapExtents.GetDrawMapBG())
		{
			theDrawBackgroundHeight = this->mMapExtents.GetMinViewHeight();
		}
	}
	
	if(theDrawBackgroundHeight != -1)
	{
		if(this->mBackgroundSprite)
		{
			// Build three points on plane described by max world dimensions at the draw background height
			Vector theUpperLeftWorldPos(-kMaxMapDimension, kMaxMapDimension, theDrawBackgroundHeight);
			Vector theUpperRightWorldPos(kMaxMapDimension, kMaxMapDimension, theDrawBackgroundHeight);
			Vector theLowerLeftWorldPos(-kMaxMapDimension, -kMaxMapDimension, theDrawBackgroundHeight);

			// Calculate plane info
			float thePlaneD;
			Vector thePlaneNormal;
			CalculatePlaneInfo(theUpperLeftWorldPos, theUpperRightWorldPos, theLowerLeftWorldPos, thePlaneD, thePlaneNormal);

			gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
			gEngfuncs.pTriAPI->CullFace( TRI_NONE );
			gEngfuncs.pTriAPI->Brightness( 1 );
			
			if(gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer(this->mBackgroundSprite), 0))
			{
				gEngfuncs.pTriAPI->Begin( TRI_TRIANGLE_STRIP );
				
				gEngfuncs.pTriAPI->Color4f(1.0f, 1.0f, 1.0f, 1.0f);

				Vector thePoint;

				CalculatePointOnPlane(0, ScreenHeight(), GetViewOrigin(), thePlaneNormal, thePlaneD, thePoint);
				gEngfuncs.pTriAPI->TexCoord2f(0, 1);
				gEngfuncs.pTriAPI->Vertex3fv((float*)&thePoint);

				CalculatePointOnPlane(0, 0, GetViewOrigin(), thePlaneNormal, thePlaneD, thePoint);
				gEngfuncs.pTriAPI->TexCoord2f(0, 0);
				gEngfuncs.pTriAPI->Vertex3fv((float*)&thePoint);
				
				CalculatePointOnPlane(ScreenWidth(), ScreenHeight(), GetViewOrigin(), thePlaneNormal, thePlaneD, thePoint);
				gEngfuncs.pTriAPI->TexCoord2f(1, 1);
				gEngfuncs.pTriAPI->Vertex3fv((float*)&thePoint);

				CalculatePointOnPlane(ScreenWidth(), 0, GetViewOrigin(), thePlaneNormal, thePlaneD, thePoint);
				gEngfuncs.pTriAPI->TexCoord2f(1, 0);
				gEngfuncs.pTriAPI->Vertex3fv((float*)&thePoint);
				
				// Add in some buffer because of perspective
//				pVector ver;
//				ver.z = theDrawBackgroundHeight;
//				
//				// Lower left
//				gEngfuncs.pTriAPI->TexCoord2f(0,1);
//				ver.x = -kMaxMapDimension;
//				ver.y = -kMaxMapDimension;
//				gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
//				
//				// Upper left
//				gEngfuncs.pTriAPI->TexCoord2f(0,0);
//				ver.x = -kMaxMapDimension;
//				ver.y = kMaxMapDimension;
//				gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
//				
//				// Upper right
//				gEngfuncs.pTriAPI->TexCoord2f(1,0);
//				ver.x = kMaxMapDimension;
//				ver.y = kMaxMapDimension;
//				gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
//				
//				// Lower right
//				gEngfuncs.pTriAPI->TexCoord2f(1,1);
//				ver.x = kMaxMapDimension;
//				ver.y = -kMaxMapDimension;
//				gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
				
				gEngfuncs.pTriAPI->End();
			}
		}
	}
}

void AvHHud::PostModelRender(char* inModelName)
{

	// Get our view model name
	cl_entity_t* theViewEntity = gEngfuncs.GetViewModel();
	if(theViewEntity)
	{
		// If this is our view model, that means we can now render our own stuff in screen space without z-buffering on top of everything else
		if(theViewEntity->model)
		{
			if(!strcmp(theViewEntity->model->name, inModelName))
			{
				this->RenderNoZBuffering();
			}
		}
	}
    
}

float AvHHud::GetHUDExperience() const
{
	vec3_t theVUser4;
	theVUser4.z = 0.0f;
	
	cl_entity_s* theLocalPlayer = gEngfuncs.GetLocalPlayer();
	if(theLocalPlayer)
	{
		theVUser4 = theLocalPlayer->curstate.vuser4;
	}
	
	if(g_iUser1 == OBS_IN_EYE)
	{
		cl_entity_t* theEnt = gEngfuncs.GetEntityByIndex(g_iUser2);
		if(theEnt)
		{
			theVUser4 = theEnt->curstate.vuser4;
		}
	}

	float theExperience = theVUser4.z/kNumericNetworkConstant;
	
	return theExperience;
}

int AvHHud::GetHUDExperienceLevel() const
{
	float theExperience = this->GetHUDExperience();
	int theLevel = AvHPlayerUpgrade::GetPlayerLevel(theExperience);
	return theLevel;
}

float AvHHud::GetHUDHandicap() const
{
	float theHandicap = 100.0f;

    AvHTeamNumber theTeamNumber = this->GetHUDTeam();
	switch(theTeamNumber)
	{
	case TEAM_ONE:
		theHandicap = this->GetServerVariableFloat(kvTeam1DamagePercent);
		break;
		
	case TEAM_TWO:
		theHandicap = this->GetServerVariableFloat(kvTeam2DamagePercent);
		break;
	case TEAM_THREE:
		theHandicap = this->GetServerVariableFloat(kvTeam3DamagePercent);
		break;
	case TEAM_FOUR:
		theHandicap = this->GetServerVariableFloat(kvTeam4DamagePercent);
		break;
	}
	
	return theHandicap;
}

AvHUser3 AvHHud::GetHUDUser3() const
{
	AvHUser3 theUser3 = AVH_USER3_NONE;

	cl_entity_s* theLocalPlayer = gEngfuncs.GetLocalPlayer();
	if(theLocalPlayer)
	{
		theUser3 = (AvHUser3)(theLocalPlayer->curstate.iuser3);
	}

	if(g_iUser1 == OBS_IN_EYE)
	{
		cl_entity_t* theEnt = gEngfuncs.GetEntityByIndex(g_iUser2);
		if(theEnt)
		{
			theUser3 = (AvHUser3)(theEnt->curstate.iuser3);
		}
	}

	return theUser3;
}

AvHTeamNumber AvHHud::GetHUDTeam() const
{
	AvHTeamNumber theTeamNumber = TEAM_IND;

	cl_entity_s* thePlayer = this->GetVisiblePlayer();
	if(thePlayer)
	{
		theTeamNumber = AvHTeamNumber(thePlayer->curstate.team);
	}

	return theTeamNumber;
}

int AvHHud::GetHUDUpgrades() const
{
	int theUpgrades = 0;

	cl_entity_s* thePlayer = this->GetVisiblePlayer();
	if(thePlayer)
	{
		theUpgrades = thePlayer->curstate.iuser4;
	}

	return theUpgrades;
}

int AvHHud::GetHUDMaxArmor() const
{
	int theHUDUpgrades = this->GetHUDUpgrades();
	AvHUser3 theUser3 = this->GetHUDUser3();

	int theMaxArmor = AvHPlayerUpgrade::GetMaxArmorLevel(theHUDUpgrades, theUser3);

	return theMaxArmor;
}

int AvHHud::GetHUDMaxHealth()	const
{
	int theHUDUpgrades = this->GetHUDUpgrades();
	AvHUser3 theUser3 = this->GetHUDUser3();
	int theHUDExperienceLevel = this->GetHUDExperienceLevel();

	int theMaxHealth = AvHPlayerUpgrade::GetMaxHealth(theHUDUpgrades, theUser3, theHUDExperienceLevel);

	return theMaxHealth;
}

void AvHHud::GetPrimaryHudColor(int& outR, int& outG, int& outB, bool inIgnoreUpgrades, bool gammaCorrect) const
{
	if(this->GetIsMarine())
	{
		UnpackRGB(outR, outG, outB, RGB_MARINE_BLUE);
	}
	else
	{
		// HUD turns green when we're frenzying
		//if(GetHasUpgrade(this->GetHUDUpgrades(), MASK_PRIMALSCREAM) && !inIgnoreUpgrades)
		//{
		//	UnpackRGB(outR, outG, outB, RGB_GREENISH);
		//}
		//else
		//{
			UnpackRGB(outR, outG, outB, RGB_YELLOWISH);
		//}
	}

    if (gammaCorrect)
    {
	    // Take into account current gamma?
	    float theGammaSlope = this->GetGammaSlope();
	    outR /= theGammaSlope;
	    outG /= theGammaSlope;
	    outB /= theGammaSlope;
    }

}

void AvHHud::HandleFog()
{
	float theFogColor[3];
	theFogColor[0] = this->mFogColor.x;
	theFogColor[1] = this->mFogColor.y;
	theFogColor[2] = this->mFogColor.z;

	gEngfuncs.pTriAPI->Fog(theFogColor, this->mFogStart, this->mFogEnd, this->mFogActive);
}

void AvHHud::PreRenderFrame()
{
	bool theRenderForTopDown = this->mInTopDownMode /*&& !this->mIsRenderingSelectionView*/;

	if(!theRenderForTopDown)
	{
		this->HandleFog();
	}
	else
	{
		this->DrawTopDownBG();
		
		// Now draw commander HUD scaled
		//void CreatePickingRay( int mousex, int mousey, Vector& outVecPickingRay )
//		static int theCommanderHUDSprite = 0;
//		if(!theCommanderHUDSprite)
//		{
//			theCommanderHUDSprite = Safe_SPR_Load("sprites/.spr");
//		}
//		
//		if(theCommanderHUDSprite)
//		{
//			gEngfuncs.pTriAPI->RenderMode( kRenderNormal );
//			gEngfuncs.pTriAPI->CullFace( TRI_NONE );
//			gEngfuncs.pTriAPI->Brightness( 1 );
//			
//			if(gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer(theCommanderHUDSprite), 0))
//			{
//				gEngfuncs.pTriAPI->Begin( TRI_QUADS );
//				
//				gEngfuncs.pTriAPI->Color4f(1.0f, 1.0f, 1.0f, .5f);
//				
//				pVector ver;
//				
//				// Lower left
//				const float kMaxMapDimension = 4096;
//				//ver.z = -kMaxMapDimension;
//				ver.z = theDrawBackgroundHeight;
//				
//				gEngfuncs.pTriAPI->TexCoord2f(0,1);
//				ver.x = -kMaxMapDimension;
//				ver.y = -kMaxMapDimension;
//				gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
//				
//				// Upper left
//				gEngfuncs.pTriAPI->TexCoord2f(0,0);
//				ver.x = -kMaxMapDimension;
//				ver.y = kMaxMapDimension;
//				gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
//				
//				// Upper right
//				gEngfuncs.pTriAPI->TexCoord2f(1,0);
//				ver.x = kMaxMapDimension;
//				ver.y = kMaxMapDimension;
//				gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
//				
//				// Lower right
//				gEngfuncs.pTriAPI->TexCoord2f(1,1);
//				ver.x = kMaxMapDimension;
//				ver.y = -kMaxMapDimension;
//				gEngfuncs.pTriAPI->Vertex3fv((float*)&ver);
//				
//				gEngfuncs.pTriAPI->End();
//			}
//		}
	}

	AvHScriptManager::Instance()->DrawNormal();
}

void AvHHud::PostRenderFrame()
{
	// Restore player angles and view position
//	v_origin = gPlayerOrigin;
//	v_angles = gPlayerAngles;
}

void AvHHud::DrawActionButtons()
{
	// Look up AvHActionButtons component
	AvHActionButtons* theActionButtons = NULL;
	if(this->GetManager().GetVGUIComponentNamed(kActionButtonsComponents, theActionButtons))
	{
		int theNumCols = theActionButtons->GetNumCols();
		int theNumRows = theActionButtons->GetNumRows();

		// Iterate through num cols
		for(int theCol = 0; theCol < theNumCols; theCol++)
		{
			// Iterate through num rows
			for(int theRow = 0; theRow < theNumRows; theRow++)
			{
				// Get current ActionButton
				ActionButton* theActionButton = theActionButtons->GetActionButtonAtPos(theCol, theRow);
				ASSERT(theActionButton);

				// Get message ID for button
				AvHMessageID theMessageID = theActionButton->GetMessageID();

				// Find the group that it belongs to (20, 30, 40, etc.)
				int theMessageNumber = (int)theMessageID - (theMessageID % 10);

//				// Load sprite if not loaded
//				bool theSpriteLoaded = (this->mActionButtonSprites[theMessageNumber] > 0);
//				if(!theSpriteLoaded)
//				{
//					// Build sprite name for message ID
//					char theMessageNumberString[16];
//					sprintf(theMessageNumberString, "%d", (int)theMessageNumber);
//					//string theSpriteName = kTechTreeSpriteDirectory + string("/") + kTechTreeSpritePrefix + string(theMessageIDString) + string(".spr");
//					string theSpriteName = kTechTreeSpriteDirectory + string("/") + kTechTreeSpritePrefix + string(theMessageNumberString) + string("s.spr");
//					int theSpriteHandle = Safe_SPR_Load(theSpriteName.c_str());
//
//					// Sprite handle can be 0, as I don't have sprites for all tech yet
//					this->mActionButtonSprites[theMessageNumber] = theSpriteHandle;
//				}

				// Get pos and size for component
				int thePosX, thePosY;
				theActionButton->getPos(thePosX, thePosY);
				
				int theWidth, theHeight;
				theActionButton->getSize(theWidth, theHeight);
				
				// Set sprite frame depending if button is available, active
				bool theTechEnabled = theActionButton->GetTechEnabled();
				bool theMouseOver = theActionButton->GetMouseOver();
				bool theCostMet = theActionButton->GetCostMet();

				// 0 = default
				// 1 = highlighted
				// 2 = dark
				// 3 = red
				int theFrame = 2;

				// If it's enabled, or if it's an icon in the "menu navigation" category, allow it to be highlighted
				if(theTechEnabled || (theMessageNumber == 80))
				{
					if(theCostMet)
					{
						if(theMouseOver)
						{
							theFrame = 1;
						}
						else
						{
							theFrame = 0;
						}
					}
					else
					{
						theFrame = 3;
					}
				}

				// Also highlight menu category headings for open menus, and for active nodes just pressed
				if(gCommanderHandler.GetDisplayMenuMessageID() == theMessageID)
				{
					theFrame = 1;
				}
				else
				{
					AvHMessageID theMessageJustActivated = MESSAGE_NULL;
					if(gCommanderHandler.GetAndClearTechNodePressed(theMessageJustActivated, false))
					{
						if(theMessageJustActivated == theMessageID)
						{
							if(theTechEnabled)
							{
								theFrame = 1;
							}
						}
					}
				}
				
				this->DrawTechTreeSprite(theMessageID, thePosX, thePosY, theWidth, theHeight, theFrame);
			}
		}
	}
}

int AvHHud::GetTechTreeSprite(AvHMessageID inMessageID)
{
	// Find the group that it belongs to (20, 30, 40, etc.)
	int theMessageNumber = (int)inMessageID - (inMessageID % 10);
	
	// Load sprite if not loaded
	bool theSpriteLoaded = (this->mActionButtonSprites[theMessageNumber] > 0);
	if(!theSpriteLoaded && (theMessageNumber != 0))
	{
		// Build sprite name for message ID
		char theMessageNumberString[16];
		sprintf(theMessageNumberString, "%d", (int)theMessageNumber);
		//string theSpriteName = kTechTreeSpriteDirectory + string("/") + kTechTreeSpritePrefix + string(theMessageIDString) + string(".spr");
		string theSpriteName = kTechTreeSpriteDirectory + string("/") + kTechTreeSpritePrefix + string(theMessageNumberString) + string("s.spr");
		int theSpriteHandle = Safe_SPR_Load(theSpriteName.c_str());
		
		// Sprite handle can be 0, as I don't have sprites for all tech yet
		this->mActionButtonSprites[theMessageNumber] = theSpriteHandle;
	}
	
	// Fetch sprite handle
	int theSpriteHandle = this->mActionButtonSprites[theMessageNumber];

	return theSpriteHandle;
}

void AvHHud::DrawTechTreeSprite(AvHMessageID inMessageID, int inPosX, int inPosY, int inWidth, int inHeight, int inFrame)
{
	if(inMessageID != MESSAGE_NULL)
	{
		// Check for alien sprites
		bool theIsAlienSprite = false;
		int theSpriteHandle = 0;
		int theRenderMode = kRenderTransAlpha; // kRenderNormal
		
		switch(inMessageID)
		{
		case ALIEN_BUILD_DEFENSE_CHAMBER:
			theIsAlienSprite = true;
			inFrame = 0;
			break;
		case ALIEN_BUILD_OFFENSE_CHAMBER:
			theIsAlienSprite = true;
			inFrame = 1;
			break;
		case ALIEN_BUILD_MOVEMENT_CHAMBER:
			theIsAlienSprite = true;
			inFrame = 2;
			break;
		case ALIEN_BUILD_SENSORY_CHAMBER:
			theIsAlienSprite = true;
			inFrame = 3;
			break;
		case ALIEN_BUILD_RESOURCES:
			theIsAlienSprite = true;
			inFrame = 4;
			break;
		case ALIEN_BUILD_HIVE:
			theIsAlienSprite = true;
			inFrame = 5;
			break;
		}

		float theStartU = 0.0f;
		float theStartV = 0.0f;
		float theEndU = 1.0f;
		float theEndV = 1.0f;

		if(theIsAlienSprite)
		{
			theRenderMode = kRenderTransAlpha;
			theSpriteHandle = this->mAlienUIUpgradeCategories;
		}
		else
		{
			// Fetch sprite handle
			theSpriteHandle = this->GetTechTreeSprite(inMessageID);

			int theIndex = inMessageID % 10;
			int theXIndex = theIndex % 4;
			int theYIndex = (theIndex / 4);
			theStartU = theXIndex*.25f;
			theStartV = theYIndex*.25f;
			theEndU = (theXIndex+1)*.25f;
			theEndV = (theYIndex+1)*.25f;
		}

		if(theSpriteHandle > 0)
		{
			// Draw sprite scaled here
			AvHSpriteSetRenderMode(theRenderMode);
			AvHSpriteDraw(theSpriteHandle, inFrame, inPosX, inPosY, inPosX + inWidth, inPosY + inHeight, theStartU, theStartV, theEndU, theEndV);
		}
	}
}

void AvHHud::DrawHotgroups()
{
	AvHMessageID theHotgroups[kNumHotkeyGroups] = {GROUP_SELECT_1, GROUP_SELECT_2, GROUP_SELECT_3, GROUP_SELECT_4, GROUP_SELECT_5};

	// Run through list of hotgroups, drawing them if they exist
	for(int i = 0; i < kNumHotkeyGroups; i++)
	{
		EntityListType& theHotgroup = this->mGroups[i];

		if(theHotgroup.size() > 0)
		{
			// Get message ID to draw
			AvHUser3 theGroupTypeUser3 = this->mGroupTypes[i];

			// Default is marine face
			AvHMessageID theHotgroupIcon = MENU_SOLDIER_FACE;
			AvHSHUUser3ToMessageID(theGroupTypeUser3, theHotgroupIcon);

			// Build component name ("PendingImpulseXPanel", where X is the impulse)
			AvHMessageID theHotgroupMessageID = theHotgroups[i];
			char theComponentName[256];
			sprintf(theComponentName, kPendingImpulseSpecifier, (int)theHotgroupMessageID);
			
			AvHTechImpulsePanel* theTechImpulsePanel = NULL;
			if(this->GetManager().GetVGUIComponentNamed(theComponentName, theTechImpulsePanel))
			{
				int thePosX, thePosY;
				theTechImpulsePanel->getPos(thePosX, thePosY);

				int theWidth, theHeight;
				theTechImpulsePanel->getSize(theWidth, theHeight);

				// Highlight if mouse is in region or if it's our current selection
				bool theIsHighlighted = this->GetIsMouseInRegion(thePosX, thePosY, theWidth, theHeight) || (this->mSelected == theHotgroup);
				int theFrame =  theIsHighlighted ? 1 : 0;

				int theSecondOfLastUpdate = (int)this->mTimeOfLastUpdate;
				if((this->mGroupAlerts[i] == ALERT_UNDER_ATTACK) && (theSecondOfLastUpdate % 2))
				{
					// Play blinking red frame when being attacked
					theFrame = 3;
				}

				this->DrawTechTreeSprite(theHotgroupIcon, thePosX, thePosY, theWidth, theHeight, theFrame);

				theTechImpulsePanel->SetVisualNumber((int)theHotgroup.size());
			}
		}
	}
}

// Draw pending requests
void AvHHud::DrawPendingRequests()
{
	// Run through list of requests and delete any entries with
	PendingRequestListType::iterator theIterator;
	for(theIterator = this->mPendingRequests.begin(); theIterator != this->mPendingRequests.end(); /* nothing */)
	{
		if(theIterator->second == 0)
		{
            PendingRequestListType::iterator theSavedIter = theIterator;
            theSavedIter++;
            this->mPendingRequests.erase(theIterator);
            theIterator = theSavedIter;
		}
		else
		{
			theIterator++;
		}
	}

	int theNumRequestsToDraw = (int)this->mPendingRequests.size();
	int theCounter = 0;
	for(theIterator = this->mPendingRequests.begin(); theIterator != this->mPendingRequests.end(); theIterator++)
	{
		// Draw each one above the minimap
		AvHMessageID theMessageID = theIterator->first;
		int theNumRequests = theIterator->second;

		// Build component name ("PendingImpulseXPanel", where X is the impulse)
		char theComponentName[256];
		sprintf(theComponentName, kPendingImpulseSpecifier, (int)theMessageID);

		AvHTechImpulsePanel* theTechImpulsePanel = NULL;
		if(this->GetManager().GetVGUIComponentNamed(theComponentName, theTechImpulsePanel))
		{
			// Get size of panel and draw tech sprite there
			int thePosX, thePosY;
			theTechImpulsePanel->getPos(thePosX, thePosY);

			int theWidth, theHeight;
			theTechImpulsePanel->getSize(theWidth, theHeight);

			AvHMessageID theMessageToDraw = theMessageID;
			switch(theMessageID)
			{
			case COMMANDER_NEXTIDLE:
				theMessageToDraw = MENU_SOLDIER_FACE;
				break;
			case COMMANDER_NEXTAMMO:
				theMessageToDraw = BUILD_AMMO;
				break;
			case COMMANDER_NEXTHEALTH:
				theMessageToDraw = BUILD_HEALTH;
				break;
			}

			int theFrame = this->GetIsMouseInRegion(thePosX, thePosY, theWidth, theHeight) ? 1 : 0;
			this->DrawTechTreeSprite(theMessageToDraw, thePosX, thePosY, theWidth, theHeight, theFrame);

			theTechImpulsePanel->SetVisualNumber(theNumRequests);
			if(theNumRequests > 0)
			{
				theTechImpulsePanel->setVisible(true);
			}
			else
			{
				theTechImpulsePanel->setVisible(false);
			}
			
			//this->DrawHUDNumber(thePosX + kTileWidth, thePosY + kTileHeight, DHN_2DIGITS, theNumRequests);
		}

		// Draw indicator on sprite showing how many requests there are
		theCounter++;
	}
}


// : 0000988 
void AvHHud::DrawBuildHealthEffectsForEntity(int inEntityIndex, float inAlpha)
// :
{
	if ( this->GetHUDPlayMode() == PLAYMODE_READYROOM )
		return;

	// Get entity
	int theUser3 = 0;
	int theUser4 = 0;
	float theFuser1 = 0.0f;
	int theEntityTeam = 0;
	bool theIsOnOurTeam=false;
	vec3_t theOrigin;
	vec3_t theMins;
	vec3_t theMaxs;
	bool theContinue = false;
	float theHealthPercentage = 0.0f;
	double theDistanceToEntity = 0;
	

	cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(inEntityIndex);
	bool theEntityIsPlayer = ((inEntityIndex > 0) && (inEntityIndex <= gEngfuncs.GetMaxClients()));
	
	if(theEntity)
	{
		theUser3 = theEntity->curstate.iuser3;
		theUser4 = theEntity->curstate.iuser4;
		theFuser1 = theEntity->curstate.fuser1;
		theEntityTeam = theEntity->curstate.team;
		theIsOnOurTeam = (theEntityTeam == (int)this->GetHUDTeam());	
		//theOrigin = theEntity->curstate.origin;
		theOrigin = AvHSHUGetRealLocation(theEntity->origin, theEntity->curstate.mins, theEntity->curstate.maxs);
		if(theEntity->player)
		{
			// Subtract half-height to be at feet
			float theHeight = theEntity->curstate.maxs[2] - theEntity->curstate.mins[2];
			theOrigin[2] -= theHeight/2.0f;
		}
		theDistanceToEntity = VectorDistance(gPredictedPlayerOrigin, theOrigin);

		theMins = theEntity->curstate.mins;
		theMaxs = theEntity->curstate.maxs;
		theHealthPercentage = theEntity->curstate.fuser2/kNormalizationNetworkFactor;
		// : 991 transmit armour and health for marines
		if ( GetIsMarine() && theEntityIsPlayer && theIsOnOurTeam ) {
			int tmpPercent=theEntity->curstate.fuser2;
			if ( GetInTopDownMode() ) {
				theHealthPercentage = (float)(tmpPercent&0x7F)/100;
			}
			else {
				theHealthPercentage = (float)(tmpPercent >> 7)/100;
			}
		}
		// :
		theContinue = true;

		// 991: 
		// Do not display health rings for enemy players unless we are commander
		if ( !GetInTopDownMode()  && theEntityIsPlayer && !theIsOnOurTeam ) {
			theContinue=false;
		}
	}

	// Get local player
	cl_entity_s* theLocalPlayer = gEngfuncs.GetLocalPlayer();
	
	bool theDrewBuildInProgress = false;
	float theOversizeScalar = 1.0f;
	
	if(AvHSHUGetDrawRingsForUser3((AvHUser3)theUser3, theOversizeScalar))
	{
		if(theContinue && theLocalPlayer)
		{
			const kDrawEnemyBuildingDistance = 200;

			// Draw effects if we are in top-down mode OR
			if(	this->GetInTopDownMode() ||

				// It's an unfriendly building that's very close OR
				(!theEntityIsPlayer && (theDistanceToEntity < kDrawEnemyBuildingDistance)) ||

				// It's a friendly entity and we're a builder OR
				(theIsOnOurTeam && (this->GetHUDUser3() == AVH_USER3_ALIEN_PLAYER2)) ||

				// welder/healing spray is selected
				(this->mCurrentWeaponID == 18 || this->mCurrentWeaponID == 27)

				)
			{
				
			// If they're not on opposite teams
			//if((theEntityTeam == theLocalPlayer->curstate.team) || (theEntityTeam == 0))
			//{
				// Get entity build/research state
				bool theIsBuilding = false;
				bool theIsResearching = false;
				float theNormalizedPercentage = 0.0f;
		
				// Calculate percentage full
				AvHSHUGetBuildResearchState(theUser3, theUser4, theFuser1, theIsBuilding, theIsResearching, theNormalizedPercentage);
		
				bool theDrawHealth = true;
				int theSpriteToUse = this->GetIsAlien() ? this->mAlienHealthSprite : this->mMarineHealthSprite;
                bool theDrawAsRecyling = (GetHasUpgrade(theUser4, MASK_RECYCLING) && theIsOnOurTeam);

				if((theIsOnOurTeam && theIsBuilding && (GetHasUpgrade(theUser4, MASK_BUILDABLE))) || theDrawAsRecyling)
				{
					theSpriteToUse = this->GetIsAlien() ? this->mAlienBuildSprite : this->mMarineBuildSprite;
					theDrawHealth = false;

                    // Draw progress inverted, as we're "undoing" the structure
                    if(theDrawAsRecyling)
                    {
                        theNormalizedPercentage = (1.0f - theNormalizedPercentage);
                    }
				}
				else
				{
					theNormalizedPercentage = theHealthPercentage;
				}
		
				if(theDrawHealth || (theEntityTeam == theLocalPlayer->curstate.team))
				{
					// Draw it
					int theNumFrames = SPR_Frames(theSpriteToUse);
					ASSERT(theNumFrames > 0);
					
					int theCurrentFrame;
					if ((theHealthPercentage < 1.0f) || theDrawAsRecyling)
					{
						theCurrentFrame = theNormalizedPercentage * (theNumFrames - 1);
						// : 0000893
						// quick hack to eliminate 1 red bar shown for dead players
						if (theEntity->player)
                            theCurrentFrame = min(max(0, theCurrentFrame), theNumFrames - 1);
						else
							theCurrentFrame = min(max(1, theCurrentFrame), theNumFrames - 1);
						// :
					}
					else 
					{
						theCurrentFrame = theNumFrames - 1;
					}
					
					// Get real position of entity
					vec3_t thePosition;
					thePosition = AvHSHUGetRealLocation(theOrigin, theMins, theMaxs);
					
					float theMaxX = theMaxs[0] - theMins[0];
					float theMaxY = theMaxs[1] - theMins[1];
					
					float theRadius = max(theMaxX, theMaxY)*theOversizeScalar;

					// Bring position off ground just a little to prevent z-fighting
					thePosition.z += kZFightingOffset;
					
					//GetHasUpgrade(this->GetHUDUpgrades(), MASK_PARASITED)

					DrawSpriteOnGroundAtPoint(thePosition, theRadius, theSpriteToUse, kRenderTransAdd, theCurrentFrame, inAlpha);
					
					theDrewBuildInProgress = true;
				}
			}
		}
	}
}

void AvHHud::DrawHUDNumber(int inX, int inY, int inFlags, int inNumber)
{
	// Draw number on HUD, in our HUD color
	int theR, theG, theB;
	this->GetPrimaryHudColor(theR, theG, theB, false, false);

	int theGammaSlope = this->GetGammaSlope();
	theR /= theGammaSlope;
	theG /= theGammaSlope;
	theB /= theGammaSlope;

	this->DrawHudNumber(inX, inY, inFlags, inNumber, theR, theG, theB);
}

void AvHHud::DrawSelectionAndBuildEffects()
{
// : 0000988 
	list<int> theSelectedList;
// :
	
	// Draw build effects
	for(SelectionListType::iterator theSelectIter = this->mSelectionEffects.begin(); theSelectIter != this->mSelectionEffects.end(); theSelectIter++)
	{
		// Draw selection effect around the entity
		int theEntIndex = theSelectIter->mEntIndex;
		this->DrawBuildHealthEffectsForEntity(theEntIndex);
		// : 0000988 
		theSelectedList.push_back(theEntIndex);
		// :
	}
	
	bool theDrawBuildingEffect = false;

	for(EntityListType::iterator theBuildingIter = this->mBuildingEffectsEntityList.begin(); theBuildingIter != this->mBuildingEffectsEntityList.end(); theBuildingIter++)
	{
		int theEntIndex = *theBuildingIter;
		this->DrawBuildHealthEffectsForEntity(theEntIndex);
		// : 0000988 
		theSelectedList.push_back(theEntIndex);
		// :
	}

	// : 0000988 & 0000991
	bool maintanceWeaponSelected = (this->mCurrentWeaponID == 18 || this->mCurrentWeaponID == 27);
	bool isCommander = this->GetInTopDownMode();
	if (isCommander || maintanceWeaponSelected) {		
		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
		gEngfuncs.pEventAPI->EV_PushPMStates();
		gEngfuncs.pEventAPI->EV_SetSolidPlayers(-1);

		int localPlayerIndex = gEngfuncs.GetLocalPlayer()->index;
		int currentteam = this->GetHUDTeam();
		int maxclients = gEngfuncs.GetMaxClients();

		
		int theNumEnts = pmove->numphysent;
		physent_t* theEntity = NULL;
		for (int i = 0; i < theNumEnts; i++)
		{
			theEntity = gEngfuncs.pEventAPI->EV_GetPhysent(i);
			if(theEntity)
			{
				if (localPlayerIndex != theEntity->info)
				{
					int theEntityIndex = theEntity->info;
					list<int>::iterator theSelectedIterator = find(theSelectedList.begin(), theSelectedList.end(), theEntityIndex);
					if (theSelectedIterator == theSelectedList.end()) 
					{
						bool theIsPlayer = ((theEntityIndex >= 1) && (theEntityIndex <= maxclients));
						bool theSameTeam = (theEntity->team == currentteam );
						if (isCommander && (theIsPlayer || theSameTeam))
						{
							this->DrawBuildHealthEffectsForEntity(theEntityIndex, 0.2);
						}
						else if (maintanceWeaponSelected && theSameTeam && !theIsPlayer)
						{
							if (AvHTraceLineAgainstWorld(gEngfuncs.GetLocalPlayer()->origin, theEntity->origin) == 1.0f)
							{
								this->DrawBuildHealthEffectsForEntity(theEntityIndex, 0.3);
							}
						}
					}
				}
			}
		}
		gEngfuncs.pEventAPI->EV_PopPMStates();
	}
	// :
}


void AvHHud::Render()
{
    if (!IEngineStudio.IsHardware())
    {

        // Don't show anything in software mode.
        
        const char* theMessage = "Software mode is not supported by Natural Selection";

        int theWidth;
        int theHeight;

	    float gammaScale = 1.0f / GetGammaSlope();

        gEngfuncs.pfnDrawSetTextColor(0, gammaScale, 0);    
        gEngfuncs.pfnDrawConsoleStringLen(theMessage, &theWidth, &theHeight);
        gEngfuncs.pfnDrawConsoleString((ScreenWidth() - theWidth) / 2, (ScreenHeight() - theHeight) / 2, (char*)theMessage);
        
        return;

    }

    if (m_Spectator.IsInOverviewMode())
    {
        AvHSpriteSetViewport(mSpecialViewport[0], mSpecialViewport[1],
            mSpecialViewport[0] + mSpecialViewport[2], mSpecialViewport[1] + mSpecialViewport[3]);
    }

    AvHSpriteBeginFrame();

    if (mSteamUIActive)
    {
        // Use the old 2D method if we're in the Steam UI.
        AvHSpriteEnableVGUI(false);
    }
    else
    {
        AvHSpriteEnableVGUI(true);
        AvHSpriteSetVGUIOffset(0, 0);
    }

#ifdef DEBUG
    if ( CVAR_GET_FLOAT( "hud_hideview" ) != 0 )
    {

        // Black out the screen
        
        int sprite = Safe_SPR_Load(kWhiteSprite);
        AvHSpriteSetColor(0, 0, 0, 1);
        AvHSpriteDraw(sprite, 0, 0, 0, ScreenWidth(), ScreenHeight(), 0, 0, 1, 1);
        AvHSpriteSetColor(1, 1, 1, 1);

    }
#endif

	// Don't draw the HUD while being digested.

    if (GetIsBeingDigested())
	{
    
		if (this->mDigestingSprite)
        {
            AvHSpriteSetColor(1,1,1);
            AvHSpriteSetRenderMode(kRenderNormal);
            DrawWarpedOverlaySprite(mDigestingSprite, 4, 3, .02, .02, .3, .15);
        }        

		RenderProgressBar(kProgressBarSprite);
	}
    else
    {
   
        if (this->GetHUDPlayMode() == PLAYMODE_PLAYING ||
            this->GetHUDPlayMode() == PLAYMODE_READYROOM)
        {

            if (!mSteamUIActive)
            {
		        this->DrawReticleInfo();
	    	    this->DrawPlayerNames();
		        this->DrawToolTips();
				this->DrawCenterText();
            }

        }

        if (this->GetHUDPlayMode() == PLAYMODE_PLAYING)
        {

            RenderCommonUI();

            if (GetIsMarine())
            {
                if (GetInTopDownMode())
                {
                    RenderCommanderUI();
                }
                else
                {
                    RenderMarineUI();
                }
            }
            else if (GetIsAlien())
            {
                RenderAlienUI();
            }

			RenderProgressBar(kProgressBarSprite);
        }

    }

    AvHSpriteEndFrame();
    AvHSpriteClearViewport();

}

void AvHHud::RenderCommonUI()
{
	static bool speedMeasured=false;
    if (!mSteamUIActive)
    {
        
		if (gHUD.GetServerVariableFloat("sv_cheats") != 0 ) {
			static int maxSpeed=0, maxGroundSpeed=0, maxClimb=0, maxDive=0;
			if ( CVAR_GET_FLOAT("cl_showspeed") != 0) {

				// Draw the speedometer.
				int theR, theG, theB;
				this->GetPrimaryHudColor(theR, theG, theB, true, false);

				extern playermove_s* pmove;
	        
				char buffer[1024];

				maxClimb=max(maxClimb, (int)pmove->velocity[2]);
				maxDive=min(maxDive, (int)pmove->velocity[2]);

				int speed=(int)Length(pmove->velocity);

				maxSpeed=max(speed, maxSpeed);
				sprintf(buffer, "Speed = %d (%d) %d/%d", speed, maxSpeed, maxClimb, maxDive);
				mFont.DrawString(10, 10, buffer, theR, theG, theB);

				float theGroundSpeed = sqrtf(pmove->velocity[0] * pmove->velocity[0] + pmove->velocity[1] * pmove->velocity[1]);
				maxGroundSpeed=max(theGroundSpeed, maxGroundSpeed);
				sprintf(buffer, "Ground speed = %d (%d)", (int)theGroundSpeed, maxGroundSpeed);
				mFont.DrawString(10, 12 + mFont.GetStringHeight(), buffer, theR, theG, theB);
				speedMeasured = true;
			}
			else if ( speedMeasured == true ) {
				char msg[256];
				sprintf(msg, "Current Speed(%d)\tCurrent Ground Speed(%d) Max Speed(%d)\t Max Ground Speed (%d)\tMax Climb (%d)\tMax Dive(%d)\n", 
					(int)Length(pmove->velocity), (int)sqrtf(pmove->velocity[0] * pmove->velocity[0] + pmove->velocity[1] * pmove->velocity[1]),
					maxSpeed, maxGroundSpeed, maxClimb, maxDive);
				ConsolePrint(msg);
				maxSpeed=0, maxGroundSpeed=0, maxClimb=0, maxDive=0;
				speedMeasured = false;
			}
         }

        DrawInfoLocationText();
        DrawHUDStructureNotification();

        this->DrawOrders();
        this->DrawHelpIcons();
		// : 0000971
		this->DrawTeammateOrders();
		// : 0000992
		this->DrawDisplayOrder();
		// :

        if (this->GetIsCombatMode())
        {
	        // If we're in combat mode, draw our rank
	        const float kTitleYInset = (kPlayerStatusVerticalInset+5*kPlayerStatusStatusSpacing);

	        string theTitleText = this->GetRankTitle(true);

	        char theCharArray[512];
	        sprintf(theCharArray, theTitleText.c_str());

	        // Draw it

	        int theX = mViewport[0] + kPlayerStatusHorizontalCenteredInset*(mViewport[2] - mViewport[0]);
	        int theY = mViewport[1] + mViewport[3] - (1-kTitleYInset)*ScreenHeight();

	        int theR, theG, theB;
	        this->GetPrimaryHudColor(theR, theG, theB, true, false);

	        this->DrawHudStringCentered(theX, theY, ScreenWidth(), theCharArray, theR, theG, theB);
        }

	    // If we're parasited, draw a message as such
	    if (this->GetIsAlive())
	    {
		    string theText;
		    char theCharArray[512];
		    
		    int theR, theG, theB;
		    //UnpackRGB(theR, theG, theB, RGB_YELLOWISH);
		    this->GetPrimaryHudColor(theR, theG, theB, true, false);

		    int theSecondOfLastUpdate = (int)this->mTimeOfLastUpdate;
		    
		    // Draw blinking primal scream message
		    if(GetHasUpgrade(this->GetHUDUpgrades(), MASK_BUFFED) && (theSecondOfLastUpdate % 2))
		    {
                if(this->GetIsAlien())
                {
                    LocalizeString(kPrimalScreamed, theText);
                }
                else if(this->GetIsMarine())
                {
                    LocalizeString(kCatalysted, theText);
                }

                // Draw it
                sprintf(theCharArray, "%s", theText.c_str());
                this->DrawHudStringCentered(kPlayerStatusHorizontalCenteredInset*ScreenWidth(), kPlayerStatusVerticalInset*ScreenHeight(), ScreenWidth(), theCharArray, theR, theG, theB);
		    }

		    // Draw parasited indicator
		    if(GetHasUpgrade(this->GetHUDUpgrades(), MASK_PARASITED))
		    {
			    LocalizeString(kParasited, theText);
			    sprintf(theCharArray, "%s", theText.c_str());
			    
			    // Draw it
			    this->DrawHudStringCentered(kPlayerStatusHorizontalCenteredInset*ScreenWidth(), (kPlayerStatusVerticalInset+kPlayerStatusStatusSpacing)*ScreenHeight(), ScreenWidth(), theCharArray, theR, theG, theB);
		    }

		    // Draw umbraed indicator
		    if(GetHasUpgrade(this->GetHUDUpgrades(), MASK_UMBRA))
		    {
			    LocalizeString(kUmbraed, theText);
			    sprintf(theCharArray, "%s", theText.c_str());
			    
			    // Draw it
			    this->DrawHudStringCentered(kPlayerStatusHorizontalCenteredInset*ScreenWidth(), (kPlayerStatusVerticalInset+2*kPlayerStatusStatusSpacing)*ScreenHeight(), ScreenWidth(), theCharArray, theR, theG, theB);
		    }

		    // Draw blinking "webbed" message
		    if(GetHasUpgrade(this->GetHUDUpgrades(), MASK_ENSNARED) && (theSecondOfLastUpdate % 2))
		    {
			    LocalizeString(kWebbed, theText);
			    sprintf(theCharArray, "%s", theText.c_str());
			    
			    // Draw it
			    this->DrawHudStringCentered(kPlayerStatusHorizontalCenteredInset*ScreenWidth(), (kPlayerStatusVerticalInset+3*kPlayerStatusStatusSpacing)*ScreenHeight(), ScreenWidth(), theCharArray, theR, theG, theB);
		    }

		    // Draw "stunned" message (it's so fast, try not blinking it)
		    if(GetHasUpgrade(this->GetHUDUpgrades(), MASK_PLAYER_STUNNED) /*&& (theSecondOfLastUpdate % 2)*/)
		    {
			    LocalizeString(kStunned, theText);
			    sprintf(theCharArray, "%s", theText.c_str());
			    
			    // Draw it
			    this->DrawHudStringCentered(kPlayerStatusHorizontalCenteredInset*ScreenWidth(), (kPlayerStatusVerticalInset+4*kPlayerStatusStatusSpacing)*ScreenHeight(), ScreenWidth(), theCharArray, theR, theG, theB);
		    }

		    // Draw "being digested" or "digesting" message
		    if(this->GetIsBeingDigested() && (theSecondOfLastUpdate % 2))
		    {
			    LocalizeString(kDigested, theText);
			    sprintf(theCharArray, "%s", theText.c_str());
			    
			    // Draw it
			    this->DrawHudStringCentered(kPlayerStatusHorizontalCenteredInset*ScreenWidth(), (kPlayerStatusVerticalInset+5*kPlayerStatusStatusSpacing)*ScreenHeight(), ScreenWidth(), theCharArray, theR, theG, theB);
		    }
		    else if(this->GetIsDigesting() && (theSecondOfLastUpdate % 2))
		    {
			    // Look up player in progress entity
			    if((this->mProgressBarEntityIndex >= 1) && (this->mProgressBarEntityIndex <= gEngfuncs.GetMaxClients()))
			    {
				    hud_player_info_t thePlayerInfo;
				    gEngfuncs.pfnGetPlayerInfo(this->mProgressBarEntityIndex, &thePlayerInfo);

				    char* thePlayerName = thePlayerInfo.name;
				    if(thePlayerName)
				    {
					    LocalizeString(kDigestingPlayer, theText);
					    sprintf(theCharArray, theText.c_str(), thePlayerName);
				    }
			    }
			    
			    // Draw it
			    this->DrawHudStringCentered(kPlayerStatusHorizontalCenteredInset*ScreenWidth(), (kPlayerStatusVerticalInset+5*kPlayerStatusStatusSpacing)*ScreenHeight(), ScreenWidth(), theCharArray, theR, theG, theB);
		    }
		    
		    // If we are in a squad, draw it on our HUD
		    if(this->mCurrentSquad > 0)
		    {
			    string theSquadIndicator;
			    if(LocalizeString(kSquadIndicator, theSquadIndicator))
			    {
				    sprintf(theCharArray, theSquadIndicator.c_str(), this->mCurrentSquad);
				    
				    // Draw it
				    this->DrawHudStringCentered(kPlayerStatusHorizontalCenteredInset*ScreenWidth(), (kPlayerStatusVerticalInset+6*kPlayerStatusStatusSpacing)*ScreenHeight(), ScreenWidth(), theCharArray, theR, theG, theB);
			    }
		    }
	    }

	    // Draw the numerical effects.

	    if(this->GetIsAlive())
	    {
		    for(NumericalInfoEffectListType::iterator theIter = this->mNumericalInfoEffects.begin(); theIter != this->mNumericalInfoEffects.end(); theIter++)
		    {
			    // Draw it
			    Vector theScreenPos;
			    float theWorldPosition[3];
			    theIter->GetPosition(theWorldPosition);
			    if(AvHCUWorldToScreen(theWorldPosition, (float*)&theScreenPos))
			    {
				    float theNormX = theScreenPos.x/ScreenWidth();
				    float theNormY = theScreenPos.y/ScreenHeight();
				    if(!this->GetInTopDownMode() || !this->GetIsRegionBlockedByUI(theNormX, theNormY))
				    {
					    float theNumber = theIter->GetNumber();
					    string thePrependString("+");
					    
					    int theR, theG, theB;
					    this->GetPrimaryHudColor(theR, theG, theB, true, false);
					    if(theNumber < 0)
					    {
						    // Red, minus for negatives
						    theR = 255;
						    theG = 0;
						    theB = 0;
					    
						    // The "-" is prepended by MakeStringFromInt when negative
						    thePrependString = string("");
					    }
					    
					    const char* thePostPendStringToTranslate = NULL;
					    string thePostPendString;
					    
					    switch(theIter->GetEventType())
					    {
					    case kNumericalInfoResourcesEvent:
						    thePostPendStringToTranslate = kNumericalEventResources;
						    break;
					    case kNumericalInfoHealthEvent:
						    thePostPendStringToTranslate = kNumericalEventHealth;
						    break;
					    case kNumericalInfoResourcesDonatedEvent:
						    thePostPendStringToTranslate = kNumericalEventResourcesDonated;
						    break;
					    case kNumericalInfoAmmoEvent:
						    thePostPendStringToTranslate = kNumericalEventAmmo;
						    break;
					    }
					    
					    if(thePostPendStringToTranslate)
					    {
						    LocalizeString(thePostPendStringToTranslate, thePostPendString);
					    }
					    
					    // Don't draw 0, so we can have other messages
					    string theNumberString;
					    if(theNumber != 0)
					    {
						    theNumberString = thePrependString + MakeStringFromFloat(theNumber) + " ";
					    }
					    
					    string theDisplayString = theNumberString + thePostPendString;
					    
					    char theCharBuffer[512];
					    sprintf(theCharBuffer, "%s", theDisplayString.c_str());
					    
					    this->DrawHudStringCentered(theScreenPos.x, theScreenPos.y, ScreenWidth(), theCharBuffer, theR, theG, theB);
				    }
			    }
		    }
	    }
    
    }
	
	// Draw the combat HUD.
	if (this->GetIsCombatMode())
	{
		// Now draw our current experience level, so people know how close they are to the next level
		// Load alien resource and energy sprites
		string theSpriteName = UINameToSprite(kCombatExperienceSprite, ScreenWidth());
		int theExperienceSprite = Safe_SPR_Load(theSpriteName.c_str());
		
		if(theExperienceSprite)
		{
			const float kNormalizedWidth = .1f;
			const float kNormalizedYInset = .96f;
			const float kNormalizedHeight = .025f;
			const int kBaseIndex = this->GetIsMarine() ? 2 : 0;
			
			// Draw full background
			const int kXStart = mViewport[0] + (.5f - kNormalizedWidth/2.0f)*(mViewport[2] - mViewport[0]);
			const int kYStart = mViewport[1] + mViewport[3] - (1 - kNormalizedYInset)*ScreenHeight();
			

            AvHSpriteSetColor(1,1,1);
            AvHSpriteSetRenderMode(kRenderTransAlpha);

			AvHSpriteDraw(theExperienceSprite, kBaseIndex + 1, kXStart, kYStart, kXStart + kNormalizedWidth*ScreenWidth(), kYStart + kNormalizedHeight*ScreenHeight(), 0, 0, 1, 1);
			
			// Draw overlay showing amount of experience
			float thePercentToNextLevel = AvHPlayerUpgrade::GetPercentToNextLevel(this->GetHUDExperience());
			if((thePercentToNextLevel >= 0.0f) && (thePercentToNextLevel <= 1.0f))
            {
    			AvHSpriteDraw(theExperienceSprite, kBaseIndex, kXStart, kYStart, kXStart + thePercentToNextLevel*kNormalizedWidth*ScreenWidth(), kYStart + kNormalizedHeight*ScreenHeight(), 0, 0, thePercentToNextLevel, 1.0f);
            }
		}
		
		if(this->GetIsAlive(false))
		{
			int theR, theG, theB;
			this->GetPrimaryHudColor(theR, theG, theB, false, false);
			this->mCombatUpgradeMenu.SetR(theR);
			this->mCombatUpgradeMenu.SetG(theG);
			this->mCombatUpgradeMenu.SetB(theB);

			this->mCombatUpgradeMenu.Draw();
		}
	}

	// Draw top down help
	if(this->GetInTopDownMode())
	{
		this->mTopDownActionButtonHelp.Draw();
	}

}

void AvHHud::RenderProgressBar(char *spriteName)
{
	// Draw the progress bars
	const float progressBarStayTime = 0.2f;
	if (this->mProgressBarLastDrawn + progressBarStayTime > this->GetTimeOfLastUpdate())
	{
		HSPRITE currentSprite=0;
		if ( spriteName && ( strcmp(spriteName, kExperienceBarSprite) == 0 ) ) {
			currentSprite=this->mExperienceBarSprite;
		}
		if ( spriteName && ( strcmp(spriteName, kProgressBarSprite) == 0 ) ) {
			currentSprite=this->mProgressBarSprite;
		}
		if (currentSprite)
		{
			const float kNormalizedWidth = .1f;
			const float kNormalizedYInset = .89f;
			const float kNormalizedHeight = .025f;
			
			// Draw full background
			const int kXStart = mViewport[0] + (.5f - kNormalizedWidth/2.0f)*(mViewport[2] - mViewport[0]);
			const int kYStart = mViewport[1] + mViewport[3] - (1 - kNormalizedYInset)*ScreenHeight();
			
            AvHSpriteSetColor(1,1,1);
            AvHSpriteSetRenderMode(kRenderTransAlpha);

			AvHSpriteDraw(currentSprite, this->mProgressBarDrawframe + 1, kXStart, kYStart, kXStart + kNormalizedWidth*ScreenWidth(), kYStart + kNormalizedHeight*ScreenHeight(), 0, 0, 1, 1);
			
			// Draw overlay showing progress
			float theProgress = this->mProgressBarStatus;
			if((theProgress >= 0.0f) && (theProgress <= 1.0f))
            {
    			AvHSpriteDraw(currentSprite, this->mProgressBarDrawframe, kXStart, kYStart, kXStart + theProgress*kNormalizedWidth*ScreenWidth(), kYStart + kNormalizedHeight*ScreenHeight(), 0, 0, theProgress, 1.0f);
            }
		}
	}
}

void AvHHud::RenderMiniMap(int inX, int inY, int inWidth, int inHeight)
{
    
    AvHOverviewMap& theOverviewMap = gHUD.GetOverviewMap();

	float hudMinimap=CVAR_GET_FLOAT(kvHudMapZoom);
	hudMinimap=min(3, max(1, hudMinimap));

	float zoomScale=(3.0f-hudMinimap);

	AvHOverviewMap::DrawInfo theDrawInfo;
    
    theDrawInfo.mX          = inX;
    theDrawInfo.mY          = inY;
    theDrawInfo.mWidth      = inWidth;
    theDrawInfo.mHeight     = inHeight;
    theDrawInfo.mFullScreen = false;

    float worldViewWidth = 800 + 400.0f*zoomScale;

	theDrawInfo.mZoomScale  = 1-(0.25f * zoomScale );

	
	float aspectRatio = (float)(theDrawInfo.mHeight) / theDrawInfo.mWidth;

    float thePlayerX;
    float thePlayerY;

    theOverviewMap.GetWorldPosition(thePlayerX, thePlayerY);

    theDrawInfo.mViewWorldMinX = thePlayerX - worldViewWidth;
    theDrawInfo.mViewWorldMinY = thePlayerY - worldViewWidth * aspectRatio;
    theDrawInfo.mViewWorldMaxX = thePlayerX + worldViewWidth;
    theDrawInfo.mViewWorldMaxY = thePlayerY + worldViewWidth * aspectRatio;

    theOverviewMap.Draw(theDrawInfo);

}

void AvHHud::RenderMarineUI()
{

	int theR, theG, theB;
	this->GetPrimaryHudColor(theR, theG, theB, false, false);


    if (mMarineTopSprite && (this->mMapMode == MAP_MODE_NS))
    {
	    
        float theX;
        float theY;
        float theWidth  = .75f*ScreenWidth();
	    float theHeight = ((float)256/768)*ScreenHeight();

        AvHSpriteSetRenderMode(kRenderTransAdd);
        AvHSpriteSetColor(1,1,1);
		int hudMinimap=CVAR_GET_FLOAT(kvHudMapZoom);
		for ( int i=0; i<3; i++ ) {
			float width=theWidth/3.0f;
			int frame=i;
			if ( hudMinimap == 0 && i == 2 ) frame=i+1;
			
			AvHSpriteDraw(mMarineTopSprite, frame, mViewport[2] - width*(3-i) + mViewport[0],
				mViewport[1], mViewport[2] - width*(2-i) + mViewport[0], mViewport[1] + theHeight, 0, 0, 1, 1);

		}


        // Draw the minimap.
		if ( hudMinimap > 0 ) {
			int theMiniMapX = mViewport[2] - 0.21f * ScreenWidth() + mViewport[0];
			int theMiniMapY = mViewport[1] + 0.013 * ScreenHeight();
			int theMiniMapWidth =  0.200 * ScreenWidth();
			int theMiniMapHeight = 0.202 * ScreenHeight();

			RenderMiniMap(theMiniMapX, theMiniMapY, theMiniMapWidth, theMiniMapHeight);
		}
        // Draw the resource label.
		
        theHeight = ScreenHeight() * 0.038;

        std::string theResourceText;
        char theResourceBuffer[64];
        
        LocalizeString(kMarineResourcePrefix, theResourceText);
		_snprintf(theResourceBuffer, 64, "%s %d", theResourceText.c_str(), this->mVisualResources);
        
        theX = mViewport[0] + 0.3 * ScreenWidth();
        theY = mViewport[1] + 0.007 * ScreenHeight() + (theHeight - GetSmallFont().GetStringHeight()) / 2;
        
        GetSmallFont().DrawString(theX, theY, theResourceBuffer, theR, theG, theB);

        // Draw the commander status label.

        std::string theCommanderName;
        bool theCommanderNameVisible = true;
        
        if (!GetCommanderLabelText(theCommanderName))
        {
            // Blink if there is no commander.
		    if ((int)this->mTimeOfLastUpdate % 2)
		    {
			    theCommanderNameVisible = false;
		    }
        }

        if (theCommanderNameVisible)
        {
        
            theX = mViewport[0] + 0.50 * ScreenWidth();
            theY = mViewport[1] + 0.006 * ScreenHeight() + (theHeight - GetSmallFont().GetStringHeight()) / 2;
        
            GetSmallFont().DrawString(theX, theY, theCommanderName.c_str(), theR, theG, theB);
        
        }

    }

    AvHSpriteEnableClippingRect(false);

    // TODO: Draw various marine upgrades (assumes 256x256 sprite, with each upgrade being 64x64, listed right to left, top to bottom, armor upgrades first, 
	// weapon upgrades next, then motion-tracking, then jetpacks, then exoskeleton)
	
	// Do we have a jetpack?
	if(GetHasUpgrade(this->GetHUDUpgrades(), MASK_UPGRADE_7))
	{
		// Draw jetpack energy indicator
		if(this->mMarineUIJetpackSprite)
		{
			float theFactor = 1 - pmove->fuser3/kNormalizationNetworkFactor;
			int theFrame = 0;
	
			string theOutputMessage;
			if(LocalizeString(kJetpackEnergyHUDText, theOutputMessage))
			{
			
                float theX = .9f*ScreenWidth();
                float theY = .8f*ScreenHeight();
                float theWidth = .02f*ScreenWidth();
                float theHeight = .1f*ScreenHeight();

                AvHSpriteSetRenderMode(kRenderTransAdd);
                AvHSpriteSetColor(1,1,1);
                                
                AvHSpriteDraw(mMarineUIJetpackSprite, 0, theX, theY, theX + theWidth, theY + theHeight * theFactor, 0, 0, 1, theFactor);    
                AvHSpriteDraw(mMarineUIJetpackSprite, 1, theX, theY + theHeight * theFactor, theX + theWidth, theY + theHeight, 0, theFactor, 1, 1);

            }
		}
	}


	const int kNumUpgradesToDraw = 4;
	AvHUpgradeMask kUpgradeToMaskTable[kNumUpgradesToDraw] = {MASK_UPGRADE_4, MASK_UPGRADE_1, MASK_UPGRADE_8, MASK_UPGRADE_7};

    const float kAspectRatio = ScreenWidth()/ScreenHeight();
	
	const float kNormalizedSpacing = 0.01f;
	const int kBaseRightOffset = kNormalizedSpacing*ScreenWidth();
	const int kVerticalUpgradeSpacing = kNormalizedSpacing*kAspectRatio*ScreenHeight();
	int theUpgradeVar = this->GetHUDUpgrades();
	const int kUpgradeFrame = 0;
	const float kUpgradeSize = 0.05;
	int theUpgradeWidth = kUpgradeSize*ScreenWidth();
	int theUpgradeHeight = kUpgradeSize*kAspectRatio*ScreenHeight();
    
    for(int i = 0; i < kNumUpgradesToDraw; i++)
	{
		AvHUpgradeMask theUpgradeMask = kUpgradeToMaskTable[i];

		// Draw highest ammo upgrade level
		if(theUpgradeMask == MASK_UPGRADE_4)
		{
			if(GetHasUpgrade(theUpgradeVar, MASK_UPGRADE_5))
			{
				theUpgradeMask = MASK_UPGRADE_5;
				
				if(GetHasUpgrade(theUpgradeVar, MASK_UPGRADE_6))
				{
					theUpgradeMask = MASK_UPGRADE_6;
				}
			}
		}

		// Draw highest armor level
		if(theUpgradeMask == MASK_UPGRADE_1)
		{
			if(GetHasUpgrade(theUpgradeVar, MASK_UPGRADE_2))
			{
				theUpgradeMask = MASK_UPGRADE_2;
				
				if(GetHasUpgrade(theUpgradeVar, MASK_UPGRADE_3))
				{
					theUpgradeMask = MASK_UPGRADE_3;
				}
			}
		}

		// Draw heavy armor instead of jetpacks if they have it
		if(theUpgradeMask == MASK_UPGRADE_7)
		{
			if(GetHasUpgrade(theUpgradeVar, MASK_UPGRADE_13))
			{
				theUpgradeMask = MASK_UPGRADE_13;
			}
		}
		
		if(GetHasUpgrade(theUpgradeVar, theUpgradeMask))
		{
			int theFrame = 0;

			switch(theUpgradeMask)
			{
			case MASK_UPGRADE_4:
				theFrame = 0;
				break;
			case MASK_UPGRADE_5:
				theFrame = 1;
				break;
			case MASK_UPGRADE_6:
				theFrame = 2;
				break;
			case MASK_UPGRADE_1:
				theFrame = 3;
				break;
			case MASK_UPGRADE_2:
				theFrame = 4;
				break;
			case MASK_UPGRADE_3:
				theFrame = 5;
				break;
			case MASK_UPGRADE_8:
				theFrame = 6;
				break;
			case MASK_UPGRADE_7:
				theFrame = 7;
				break;
			case MASK_UPGRADE_13:
				theFrame = 8;
				break;
			}

			float theStartU = (theFrame % 4)*.25f;
			float theStartV = (theFrame / 4)*.333f;
			float theEndU = theStartU + .25f;
			float theEndV = theStartV + .333f;
			
			const int kBaseX = ScreenWidth() - .05*ScreenWidth();
			const int kBaseY = .75*ScreenHeight();
			const int kIconWidth = .05*ScreenWidth();
			const int kIconHeight = .05*ScreenHeight();
			
            float x1 = kBaseX;
            float y1 = kBaseY + i*kIconHeight;
            float x2 = x1 + kIconWidth;
            float y2 = y1 + kIconHeight;
            
			AvHSpriteSetRenderMode(kRenderTransAdd);
			AvHSpriteEnableClippingRect(false);
			AvHSpriteSetColor(1, 1, 1);
            AvHSpriteDraw(mMarineUpgradesSprite, 0, x1, y1, x2, y2, theStartU, theStartV, theEndU, theEndV);
        
        }
	}
	
	bool frames[3] = { false, false, false};
	if ( this->mHasGrenades ) frames[0]=true;
	if ( this->mHasMines ) frames[1]=true;
	if ( this->mHasWelder ) frames[2]=true;
   
    for(int i = 0; i < 3; i++)
	{
		int theFrame=i+9;
		if ( frames[i] == true ) {
			const int kIconWidth = .05*ScreenWidth();
			const int kIconHeight = .05*ScreenHeight();
			const int kBaseX = ScreenWidth() - .05*ScreenWidth();
			const int kBaseY = .75*ScreenHeight();

			float theStartU = (theFrame % 4)*.25f;
			float theStartV = (theFrame / 4)*.333f;
			float theEndU = theStartU + .25f;
			float theEndV = theStartV + .333f;

			float x1 = kBaseX;
			float y1 = kBaseY - (i+1)*kIconHeight;
			float x2 = x1 + kIconWidth;
			float y2 = y1 + kIconHeight;
	            
			AvHSpriteSetRenderMode(kRenderTransAdd);
			AvHSpriteEnableClippingRect(false);
			AvHSpriteSetColor(1, 1, 1);
			AvHSpriteDraw(mMarineUpgradesSprite, theFrame, x1, y1, x2, y2, theStartU, theStartV, theEndU, theEndV);
		}
	}
}

void AvHHud::RenderCommanderUI()
{

    RenderStructureRanges();

	// Draw command button
	if(this->mCommandButtonSprite)
	{
		// Animate the button unless the mouse is over it (?)
		int kNumFrames = 16; // Last frame is highlighted, first frame seems off
		const float kFramesPerSecond = 10;
		const float kCycleTime = (1.0f/kFramesPerSecond)*kNumFrames;

		float theNumCycles =  this->mTimeOfLastUpdate/kCycleTime;
		int theFrame = 1 + (theNumCycles - (int)theNumCycles)*kNumFrames;

		const int kStartX = 890;
		int theX = (kStartX/1024.0f)*ScreenWidth();
		int theY = (525/768.0f)*ScreenHeight();
		
		float theWidth = ((1024 - kStartX)/1024.0f)*ScreenWidth();
		float theHeight = (38/768.0f)*ScreenHeight();

        float x1 = theX;
        float y1 = theY;
        float x2 = x1 + theWidth;
        float y2 = y1 + theHeight;

        AvHSpriteSetRenderMode(kRenderTransAlpha);
        AvHSpriteDraw(mCommandButtonSprite, theFrame, x1, y2, x2, y2, 0, 0, 1, 1);
    
    }

	// Draw "select all" button
	if(this->mSelectAllSprite)
	{
		AvHTechImpulsePanel* theTechImpulsePanel = NULL;
		if(this->GetManager().GetVGUIComponentNamed(kSelectAllImpulsePanel, theTechImpulsePanel))
		{
			if(theTechImpulsePanel->isVisible())
			{
				int theX, theY;
				theTechImpulsePanel->getPos(theX, theY);
				
				int theWidth, theHeight;
				theTechImpulsePanel->getSize(theWidth, theHeight);
				
				// Draw unhighlighted, highlighted, or active
				int theFrame = 0;
				
				// If mouse is over, use frame 1
				if(this->GetIsMouseInRegion(theX, theY, theWidth, theHeight))
				{
					theFrame = 1;
				}
				
				// If all troops selected, use frame 2
				if((this->mSelected.size()) > 0 && (this->mSelected == this->mSelectAllGroup))
				{
					theFrame = 2;
				}
				
                AvHSpriteSetRenderMode(kRenderTransAlpha);
			    AvHSpriteDraw(mSelectAllSprite, theFrame, theX, theY, theX + theWidth, theY + theHeight, 0, 0, 1, 1);
            
            }
		}
	}
	
	// Draw blinking "under attack" indicator
	if(this->mCommandStatusSprite)
	{
		int theFrame = 0;
		
		int theX = (725/1024.0f)*ScreenWidth();
		int theY = (702/768.0f)*ScreenHeight();
		
		float theWidth = (61.5f/1024.0f)*ScreenWidth();
		float theHeight = (66/768.0f)*ScreenHeight();
		
		// Blink button when an alert is set!
		if(this->mBlinkingAlertType)
		{
			int theSecondOfLastUpdate = (int)this->mTimeOfLastUpdate;
			if(theSecondOfLastUpdate % 2)
			{
				theFrame = this->mBlinkingAlertType;
			}
		}

        AvHSpriteSetRenderMode(kRenderTransAlpha);
	    AvHSpriteDraw(mCommandStatusSprite, theFrame, theX, theY, theX + theWidth, theY + theHeight, 0, 0, 1, 1);
    
    }
	
	if(this->mTopDownTopSprite)
	{
		float theHeight = ((float)128/768)*ScreenHeight();
        AvHSpriteDrawTiles(mTopDownTopSprite, 4, 1, 0, 0, ScreenWidth(), theHeight, 0, 0, 1, 1);
	}

	if(this->mTopDownBottomSprite)
	{
		// The artwork is three 128s high
		float theHeight = ((float)256/768)*ScreenHeight();
        AvHSpriteSetRenderMode(kRenderTransAlpha);
        AvHSpriteDrawTiles(mTopDownBottomSprite, 4, 1, 0, ScreenHeight()-theHeight, ScreenWidth(), ScreenHeight(), 0, 0, 1, 1);
	}

	// Draw scaled sprites for all ActionButtons
	this->DrawActionButtons();

	// Draw hotgroups
	this->DrawHotgroups();

	// Draw pending requests
	this->DrawPendingRequests();
	
    // Draw logout button
	if(this->mLogoutSprite)
	{
		int theFrame = 0;

		int theX = .785*ScreenWidth();
		int theY = .013*ScreenHeight();
		
		float theWidth = .197f*ScreenWidth();
		float theHeight = .083f*ScreenHeight();

		// Detect mouseover
		if(this->GetIsMouseInRegion(theX, theY, theWidth, theHeight))
		{
			theFrame = 1;
		}

        AvHSpriteSetRenderMode(kRenderTransAlpha);
        AvHSpriteDraw(mLogoutSprite, theFrame, theX, theY, theX + theWidth, theY + theHeight, 0, 0, 1, 1);
    
    }

}

void AvHHud::RenderStructureRanges()
{

    float theRangeR = 0;
    float theRangeG = 1;
    float theRangeB = 0;
    float theRangeA = 0.2f;

	// Draw range for current ghost building, if applicable
	EntityListType theEntities;
	IntList theDistanceRequirements;
	float theZAdjustment = 0.0f;
	bool theSnapToLocation = false;
	bool theDrewGhostRegions = false;
	
	AvHSHUGetBuildRegions(this->mGhostBuilding, theEntities, theDistanceRequirements, theZAdjustment, theSnapToLocation);

	// For each entity to draw
	int theDistanceCounter = 0;
	for(EntityListType::iterator theRangeIter = theEntities.begin(); theRangeIter != theEntities.end(); theRangeIter++, theDistanceCounter++)
	{
		//cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(*theRangeIter);
		physent_t* theEntity = gEngfuncs.pEventAPI->EV_GetPhysent(*theRangeIter);
		if(theEntity)
		{
			vec3_t thePosition;
			thePosition = AvHSHUGetRealLocation(theEntity->origin, theEntity->mins, theEntity->maxs);

			//int theSprite = (theEntity->iuser3 == AVH_USER3_SIEGETURRET) ? this->mSiegeTurretSprite : this->mBuildCircleSprite;
			int theSprite = this->mBuildCircleSprite;

			int theDistanceRequirement = theDistanceRequirements[theDistanceCounter];
			RenderStructureRange(thePosition, theDistanceRequirement, theSprite, kRenderTransAdd, 0, theRangeR, theRangeG, theRangeB, theRangeA);

			theDrewGhostRegions = true;
		}
	}

    // Draw selection as well
	for(EntityListType::iterator theSelectedIter = this->mSelected.begin(); theSelectedIter != this->mSelected.end(); theSelectedIter++)
	{
		cl_entity_s* theEntity = gEngfuncs.GetEntityByIndex(*theSelectedIter);
		//physent_t* theEntity = gEngfuncs.pEventAPI->EV_GetPhysent(*theSelectedIter);
		if(theEntity)
		{
			vec3_t thePosition;
			thePosition = AvHSHUGetRealLocation(theEntity->curstate.origin, theEntity->curstate.mins, theEntity->curstate.maxs);
			
			AvHUser3 theUser3 = (AvHUser3)(theEntity->curstate.iuser3);
			int theRange = AvHSHUGetDrawRangeForUser3(theUser3);
			if(theRange > 0)
			{
				// Don't draw structures that are recycling and therefore inactive
				if(!GetHasUpgrade(theEntity->curstate.iuser4, MASK_RECYCLING))
				{
					//int theSprite = (theEntity->curstate.iuser3 == AVH_USER3_SIEGETURRET) ? this->mSiegeTurretSprite : this->mBuildCircleSprite;
					int theSprite = this->mBuildCircleSprite;
					RenderStructureRange(thePosition, theRange, theSprite, kRenderTransAdd, 0, theRangeR, theRangeG, theRangeB, theRangeA);
				}
			}
		}
	}

    // Draw the minimum build radius violations.

    EntityListType theBuildViolations;
    AvHSHUGetMinBuildRadiusViolations(mGhostBuilding, mGhostWorldLocation, theBuildViolations);
   


	for (EntityListType::iterator theBuildViolationsIter = theBuildViolations.begin(); theBuildViolationsIter != theBuildViolations.end(); ++theBuildViolationsIter)
	{	
		gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
		
		// Store off the old count
		gEngfuncs.pEventAPI->EV_PushPMStates();
		
		// Now add in all of the players.
		gEngfuncs.pEventAPI->EV_SetSolidPlayers (-1);

		physent_t* theEntity = gEngfuncs.pEventAPI->EV_GetPhysent(*theBuildViolationsIter);

		gEngfuncs.pEventAPI->EV_PopPMStates();
			
		if(theEntity)
		{
			vec3_t thePosition;
			thePosition = AvHSHUGetRealLocation(theEntity->origin, theEntity->mins, theEntity->maxs);
			
			AvHUser3 theUser3 = (AvHUser3)(theEntity->iuser3);

            vec3_t theMinSize, theMaxSize;

            AvHSHUGetSizeForUser3(theUser3, theMinSize, theMaxSize);
            float theMaxRadius2 = max(max(theMinSize.x, theMaxSize.x), max(theMinSize.y, theMaxSize.y));

            int theSprite = this->mBuildCircleSprite;
			// : 0000291 
			// It's possible to place "on" marines if you're offset a little from center. This code and 
			// associated changes above and in AvHSharedUtil.cpp is to enforce a build distance around marines,
			// in the same way as structures, to prevent this exploit.
 			float theMinMarineBuildDistance;
            if (theUser3 == AVH_USER3_MARINE_PLAYER) {
				theMinMarineBuildDistance = BALANCE_VAR(kMinMarinePlayerBuildDistance);
			}
			else 
			{
				theMinMarineBuildDistance = BALANCE_VAR(kMinMarineBuildDistance);
			}
			// :
			RenderStructureRange(thePosition, theMinMarineBuildDistance + theMaxRadius2, theSprite, kRenderTransAdd, 0, 1, 0, 0, 0.3f);

		}
	}
}

void AvHHud::RenderStructureRange(vec3_t inOrigin, int inRadius, HSPRITE inSprite, int inRenderMode, int inFrame, float inR, float inG, float inB, float inAlpha)
{

    vec3_t w1;
    
    w1[0] = inOrigin[0] - inRadius;
    w1[1] = inOrigin[1] - inRadius;
    w1[2] = inOrigin[2];
    
    vec3_t s1;
    gEngfuncs.pTriAPI->WorldToScreen(w1, s1);

    vec3_t w2;
    
    w2[0] = inOrigin[0] + inRadius;
    w2[1] = inOrigin[1] + inRadius;
    w2[2] = inOrigin[2];
    
    vec3_t s2;
    gEngfuncs.pTriAPI->WorldToScreen(w2, s2);

    float x1 = XPROJECT(s1[0]);
    float y1 = YPROJECT(s1[1]);
    float x2 = XPROJECT(s2[0]);
    float y2 = YPROJECT(s2[1]);

    AvHSpriteSetColor(inR, inG, inB, inAlpha);
    AvHSpriteSetRenderMode(inRenderMode);
    AvHSpriteDraw(inSprite, inFrame, x1, y1, x2, y2, 0, 0, 1, 1);
    AvHSpriteSetColor(1, 1, 1, 1);

}

void AvHHud::RenderAlienUI()
{

	const float kAspectRatio = ScreenWidth()/ScreenHeight();

    // Draw the embryo overlay if the player is gestating.

    if (GetHUDUser3() == AVH_USER3_ALIEN_EMBRYO)
    {
		if (mMembraneSprite)
		{
            AvHSpriteSetRenderMode(kRenderTransAdd);
            AvHSpriteSetColor(1,1,1);
            DrawWarpedOverlaySprite(mMembraneSprite, 4, 3, .007 * 2, .002 * 2, .15, .3);
		}
        return;
    }

    AvHSpriteSetRenderMode(kRenderTransAlpha);

    int theWidth = kResourceEnergyBarWidth*ScreenWidth();
    int theHeight = kResourceEnergyBarHeight*ScreenHeight();

    int theX = mViewport[0];
    int theY = mViewport[3] - theHeight + mViewport[1];

	// Draw resource text label.

    if (!mSteamUIActive)
    {

	    const float kTextInset = kResourceEnergyBarWidth*.5f;
	    const int kNumericYOffset = 1.5*this->GetHudStringHeight();

	    // : 0000989
		// moved resource label a bit down
        //int theResourceLabelX = mViewport[0] + kTextInset*ScreenWidth();
		//int theResourceLabelY = theY -  + .05f * ScreenHeight();
        int theResourceLabelX = 10;
		int theResourceLabelY = .68f * ScreenHeight();
		// :
	    
		if(this->mMapMode == MAP_MODE_NS)
		{
			string theLocalizedResourceDescription;
			if(LocalizeString(kAlienResourcePrefix, theLocalizedResourceDescription))
			{
				int theStringWidth = this->GetHudStringWidth(theLocalizedResourceDescription.c_str());
				int theR, theG, theB;
				this->GetPrimaryHudColor(theR, theG, theB, false, false);
				this->DrawHudString(theResourceLabelX, theResourceLabelY, ScreenWidth(), theLocalizedResourceDescription.c_str(), theR, theG, theB);
			}

			// Draw amount we have
			char theBuffer[128];
			sprintf(theBuffer, "(%d/%d)", this->mVisualResources, this->GetMaxAlienResources());
			this->DrawTranslatedString(theResourceLabelX, theResourceLabelY + kNumericYOffset, theBuffer);
		}
    }
    
    // Draw energy bar.

    if (mAlienUIEnergySprite)
    {
        theX = mViewport[2] - theWidth + mViewport[0];

        float theFactor = 1 - this->mVisualEnergyLevel;
        
        AvHSpriteSetColor(1,1,1);
        AvHSpriteSetRenderMode(kRenderTransTexture);

        AvHSpriteDraw(mAlienUIEnergySprite, 0, theX, theY, theX + theWidth, theY + theHeight * theFactor, 0, 0, 1, theFactor);    
        AvHSpriteDraw(mAlienUIEnergySprite, 1, theX, theY + theHeight * theFactor, theX + theWidth, theY + theHeight, 0, theFactor, 1, 1);

    }

	if (mAlienUICloakSprite )
    {
		cl_entity_s* theLocalPlayer = GetVisiblePlayer();
		if(theLocalPlayer ) {
				theX = mViewport[2] - theWidth + mViewport[0];
			
				int amount=0;
				int range=255-kAlienSelfCloakingBaseOpacity;
				if ( theLocalPlayer->curstate.renderamt > 0 ) {
					amount=theLocalPlayer->curstate.renderamt-kAlienSelfCloakingBaseOpacity;
					amount=max(0, min(range, amount));
				}
				float theFactor = 1; 
				if ( theLocalPlayer->curstate.rendermode == kRenderTransTexture )
					theFactor=(float)amount/(float)range;
					

				AvHSpriteSetColor(1,1,1);
				AvHSpriteSetRenderMode(kRenderTransTexture);

				AvHSpriteDraw(mAlienUICloakSprite, 0, theX, theY, theX + theWidth, theY + theHeight * theFactor, 0, 0, 1, theFactor);    
				AvHSpriteDraw(mAlienUICloakSprite, 1, theX, theY + theHeight * theFactor, theX + theWidth, theY + theHeight, 0, theFactor, 1, 1);
//			}
//			else {
//				int a=0;
//			}
		}
    }

	// Draw hive indicators.

    if (mHiveInfoSprite) //  && (this->mMapMode == MAP_MODE_NS)) // removed check to enable hive with health to be shown in combat
    {

	    int theHiveIndex = 0;

	    for (HiveInfoListType::iterator theIter = this->mHiveInfoList.begin(); theIter != this->mHiveInfoList.end(); theIter++, theHiveIndex++)
        {

	        // For this hive, figure out which frame to draw

		    int theFrame = theIter->mStatus;
		    
		    // If under attack, make it blink red
		    int theSecondOfLastUpdate = (int)this->mTimeOfLastUpdate;
		    if(theIter->mUnderAttack && (theSecondOfLastUpdate % 2))
		    {
			    theFrame = kHiveInfoStatusUnderAttack;
		    }

            int theHiveWidth = kHiveNormScreenWidth*ScreenWidth();

		    // Draw hive
		    //float theNormX = theHiveWidth;
		    float theNormY = kHiveNormScreenY + kHiveNormScreenVerticalSpacing*theHiveIndex;
		    
            //DrawScaledHUDSprite(this->mHiveInfoSprite, kRenderTransAlpha, 1, theNormX*ScreenWidth, theNormY*ScreenHeight(), kHiveNormScreenWidth*ScreenWidth, kHiveNormScreenHeight*ScreenHeight(), theFrame);
            
            float x1 = mViewport[0] + mViewport[2] - theHiveWidth;
            float y1 = mViewport[1] + theNormY*ScreenHeight();
            float x2 = x1 + theHiveWidth;
            float y2 = y1 + kHiveNormScreenHeight*ScreenHeight();
            
            AvHSpriteSetColor(1,1,1);
            AvHSpriteSetRenderMode(kRenderTransTexture);
            AvHSpriteDraw(mHiveInfoSprite, theFrame, x1, y1, x2, y2, 0, 0, 1, 1);

//            AvHSpriteSetColor(1,1,1);
//            AvHSpriteSetRenderMode(kRenderTransTexture);
//			AvHSpriteDraw(mHiveHealthSprite, 0, x1, y1, x1+100, y1+100,  0, 0, 1, 1);

			
			// use the hive sprite height for the hivehealth sprite height and width
			float theHealthSpriteHeight = (float)(kHiveNormScreenHeight) *ScreenHeight();
			float theHealthSpriteWidth = theHealthSpriteHeight;
			
			AvHSpriteSetColor(1,1,1);
            AvHSpriteSetRenderMode(kRenderTransTexture);

			// adjust the position for the sprite -- to the left
			float w1 = mViewport[0] + mViewport[2] - (kHiveNormScreenWidth + 0.02f)*ScreenWidth();

			AvHSpriteDraw(mHiveHealthSprite, 0, w1, y1, w1 + theHealthSpriteWidth, y1 + theHealthSpriteHeight,  0, 0, 1, 1);
			
			if (theIter->mStatus > 0) {
				if (theIter->mHealthPercentage < 100) {
					AvHSpriteDraw(mHiveHealthSprite, 0, w1, y1, w1 + theHealthSpriteWidth, y1 + theHealthSpriteHeight,  0, 0, 1, 1);
					AvHSpriteDraw(mHiveHealthSprite, 1, 
						w1, y1 + theHealthSpriteHeight - theHealthSpriteHeight * ((float)(theIter->mHealthPercentage) * 0.92f / 100),
						w1 + theHealthSpriteWidth, y1 + theHealthSpriteHeight,  
						0, 1 - ((float)(theIter->mHealthPercentage) * 0.92f / 100), 
						1, 1);
				} else {
					AvHSpriteDraw(mHiveHealthSprite, 1, w1, y1, w1 + theHealthSpriteWidth, y1 + theHealthSpriteHeight,  0, 0, 1, 1);
				}
			}



		    // Draw technology it's supporting
		    AvHMessageID theTechnology = (AvHMessageID)theIter->mTechnology;
		    theFrame = -1;
		    switch(theTechnology)
		    {
		    case ALIEN_BUILD_DEFENSE_CHAMBER:
			    theFrame = 0;
			    break;
		    case ALIEN_BUILD_SENSORY_CHAMBER:
			    theFrame = 3;
			    break;
		    case ALIEN_BUILD_MOVEMENT_CHAMBER:
			    theFrame = 2;
			    break;
		    }

		    // Draw it inset
		    if(theFrame >= 0)
		    {
			    
                float x1 = mViewport[0] + mViewport[2] - theHiveWidth + (kHiveTechnologyInsetXScalar*kHiveNormScreenWidth)*ScreenWidth();
                float y1 = mViewport[1] + (theNormY + kHiveTechnologyInsetYScalar*kHiveNormScreenHeight)*ScreenHeight();
                float x2 = x1 + kHiveTechnologyInsetWidthScalar*kHiveNormScreenWidth*ScreenWidth();
                float y2 = y1 + kHiveTechnologyInsetHeightScalar*kHiveNormScreenHeight*ScreenHeight();
                
                AvHSpriteDraw(mAlienUIUpgradeCategories, theFrame, x1, y1, x2, y2, 0, 0, 1, 1);    

		    }

        }
    
    }

    // Draw the upgrades.

	const int kNumberXInset = 2;
	const int kNumberYInset = 2;
	
	const int kBaseYOffset = .1*ScreenHeight();
	
	// Now draw alien upgrades
	const float kNormalizedSpacing = 0.01f;
	const int kBaseRightOffset = kNormalizedSpacing*ScreenWidth();
	const int kVerticalUpgradeSpacing = kNormalizedSpacing*kAspectRatio*ScreenHeight();
	int theUpgradeVar = this->GetHUDUpgrades();
	const int kUpgradeFrame = 0;
	const float kUpgradeSize = 0.04;
	int theUpgradeWidth = kUpgradeSize*ScreenWidth();
	int theUpgradeHeight = kUpgradeSize*kAspectRatio*ScreenHeight();
	
	// In Combat mode, we can have multiple upgrades in each category
	int theNumDrawnInCategory[ALIEN_UPGRADE_CATEGORY_MAX_PLUS_ONE + 1];
	memset(theNumDrawnInCategory, 0, sizeof(int)*(ALIEN_UPGRADE_CATEGORY_MAX_PLUS_ONE + 1));
	
	AvHUpgradeMask theUpgradeMasks[kNumAlienUpgrades] = {MASK_UPGRADE_1, MASK_UPGRADE_2, MASK_UPGRADE_3, MASK_NONE, MASK_NONE, MASK_NONE, MASK_UPGRADE_4, MASK_UPGRADE_5, MASK_UPGRADE_6, MASK_UPGRADE_7, MASK_UPGRADE_8, MASK_UPGRADE_9};
	for(int i = 0; i < kNumAlienUpgrades; i++)
	{
		AvHUpgradeMask theUpgradeMask = theUpgradeMasks[i];
		AvHAlienUpgradeCategory theCategory = ALIEN_UPGRADE_CATEGORY_INVALID;
		AvHGetAlienUpgradeCategoryFromMask(theUpgradeMask, theCategory);
		
		int theCategoryNumber = (int)theCategory;
		//int theRowNumber = i % (kNumAlienUpgrades/kNumAlienUpgradeCategories);

		// Quick hack: treat defense as offense for drawing purposes, because offense has been (temporarily?) removed
		int theCategoryNumberOffset = (int)(((theCategory == ALIEN_UPGRADE_CATEGORY_DEFENSE) ?  ALIEN_UPGRADE_CATEGORY_OFFENSE : theCategory));
		int theX = ScreenWidth() - theUpgradeWidth - kBaseRightOffset;

		// Inset drawing of multiple upgrades in same category (needed for Combat mode)
		theX -= (theNumDrawnInCategory[theCategory]*theUpgradeWidth);
		
		int theEnergyHeight = kResourceEnergyBarHeight*ScreenHeight();

		int theY = (3*kHiveNormScreenVerticalSpacing)*ScreenHeight() + kVerticalUpgradeSpacing + theCategoryNumberOffset*(kVerticalUpgradeSpacing + theUpgradeHeight);
		
		if(GetHasUpgrade(theUpgradeVar, theUpgradeMask))
		{
			theNumDrawnInCategory[theCategory]++;

			int theLevelOfUpgrade = AvHGetAlienUpgradeLevel(theUpgradeVar, theUpgradeMask);
			for(int theLevel = theLevelOfUpgrade; theLevel > 0; theLevel--)
			{
				// Draw them slightly overlapping
				const float kOffset = .01f;
			
                float x1 = theX - theLevel*(kOffset*ScreenWidth());
                float y1 = theY - theLevel*(kOffset*ScreenHeight());
                float x2 = x1 + theUpgradeWidth;
                float y2 = y1 + theUpgradeHeight;

                AvHSpriteDraw(mAlienUIUpgrades, i, x1, y1, x2, y2, 0, 0, 1, 1);
            }
		}
		else
		{
			if(this->GetGameStarted() && !this->GetIsCombatMode())
			{
				// If upgrade is pending, draw it blinking
				for(int i = 0; i < this->mNumUpgradesAvailable; i++)
				{
					if(this->mCurrentUpgradeCategory[i] == theCategory)
					{
						int theSecondOfLastUpdate = (int)this->mTimeOfLastUpdate;
						if(theSecondOfLastUpdate % 2)
						{
							int numSprites=1;
							switch ( theCategory ) {
								case ALIEN_UPGRADE_CATEGORY_DEFENSE:
									numSprites=this->mNumDefense;
									break;
								case ALIEN_UPGRADE_CATEGORY_SENSORY:
									numSprites=this->mNumSensory;
									break;
								case ALIEN_UPGRADE_CATEGORY_MOVEMENT:
									numSprites=this->mNumMovement;
									break;
							}
							for ( int j = numSprites; j > 0; j-- ) {
								const float kOffset = .01f;
								int theFrame = theCategory-1;
							
								float x1 = theX - j*(kOffset*ScreenWidth());
								float y1 = theY - j*(kOffset*ScreenHeight());
								float x2 = x1 + theUpgradeWidth;
								float y2 = y1 + theUpgradeHeight;

								AvHSpriteDraw(mAlienUIUpgradeCategories, theFrame, x1, y1, x2, y2, 0, 0, 1, 1);
							}
                    	}
						break;
					}
				}
			}
		}
	}


    if(this->GetIsAlien() && (this->GetHUDTeam() != TEAM_IND))
	{
		bool theIsGestating = (this->GetHUDUser3() == AVH_USER3_ALIEN_EMBRYO);
		bool theIsBeingDigested = this->GetIsBeingDigested();

		this->mSelectedNodeResourceCost = -1;
		
		// Find the blip nearest our view reticle
		int theNearestBlip = -1;
		float theDotProductOfClosestBlip = -1;
		
		// Get view vector
		Vector theViewVector;
		vec3_t theForward, theRight, theUp;
		AngleVectors(v_angles, theForward, theRight, theUp);
		VectorNormalize(theForward);

		for(int theBlip = 0; theBlip < this->mFriendlyBlips.mNumBlips; theBlip++)
		{
			// Get vector to current blip
			Vector theVectorToBlip;
			VectorSubtract(this->mFriendlyBlips.mBlipPositions[theBlip], gPredictedPlayerOrigin, theVectorToBlip);
			VectorNormalize(theVectorToBlip);

			// Dot them
			float theDotProduct = DotProduct(theForward, theVectorToBlip);

			// Is dot product closer to 1 then best?
			if(theDotProduct > theDotProductOfClosestBlip)
			{
				// Save new blip and dot product
				theNearestBlip = theBlip;
				theDotProductOfClosestBlip = theDotProduct;
			}
		}
			
		// Draw information about our friendly blips
		theBlip = theNearestBlip;
		if(theNearestBlip >= 0)
		{
			ASSERT(theNearestBlip < this->mFriendlyBlips.mNumBlips);

			// Draw location when player under attack
			int theBlipStatus = this->mFriendlyBlips.mBlipStatus[theBlip];
			//if((theBlipStatus == kVulnerableFriendlyBlipStatus) || ((theBlipStatus >= kSayingOne) && (theBlipStatus <= kSayingSix)))
			//{
				// Get location
				string theLocationName = this->GetNameOfLocation(this->mFriendlyBlips.mBlipPositions[theBlip]);
				
				// Get name of entity
				string theBlipName;
				
				int theBlipInfo = this->mFriendlyBlips.mBlipInfo[theBlip];
				if(theBlipInfo <= kBaseBlipInfoOffset)
				{
					hud_player_info_t thePlayerInfo;
					gEngfuncs.pfnGetPlayerInfo(theBlipInfo, &thePlayerInfo);
					if(thePlayerInfo.name)
					{
						theBlipName = thePlayerInfo.name;
					}
				}
				else
				{
					AvHUser3 theUser3 = AvHUser3(theBlipInfo - kBaseBlipInfoOffset);
					this->GetTranslatedUser3Name(theUser3, theBlipName);
				}
				
				string theBlipStatusText;
				char theBlipStatusString[512];
				sprintf(theBlipStatusString, "#%s%d", kBlipStatusPrefix, theBlipStatus);
				LocalizeString(theBlipStatusString, theBlipStatusText);
				
				Vector theScreenPos;
				Vector theMessageWorldPos = this->mFriendlyBlips.mBlipPositions[theBlip];
				const float kSpacingScalar = 1.2f;
				theMessageWorldPos.z -= (kWorldBlipScale/2.0f)*kSpacingScalar;

				if(AvHCUWorldToScreen(theMessageWorldPos, (float*)&theScreenPos))
				{
					if((theBlipName != "") && (theBlipStatusText != "") && (theLocationName != "") && (CVAR_GET_FLOAT(kvLabelHivesight) == 1))
					{
						// Find alpha for the blip-text based on position on the screen
						float screenWidth = ScreenWidth();
						float screenHeight = ScreenHeight();
						float xdiff = fabs(theScreenPos[0] - screenWidth/2);
						float ydiff = fabs(theScreenPos[1] - screenHeight/2);
						float quadrance = xdiff * xdiff + ydiff * ydiff;
						float alpha = max(0.0f, 0.9f - quadrance / (screenHeight * screenHeight));
						alpha *= alpha * alpha * alpha;

						// "MonsieurEvil is under attack"
						// "Resource tower is under attack"
						char theFirstLine[512];
						sprintf(theFirstLine, "%s %s\n", theBlipName.c_str(), theBlipStatusText.c_str());
						this->DrawTranslatedString(theScreenPos[0], theScreenPos[1], theFirstLine, true, true, false, alpha);
						
						char theLocationNameCStr[512];
						strcpy(theLocationNameCStr, theLocationName.c_str());
						this->DrawTranslatedString(theScreenPos[0], theScreenPos[1] + .022f*ScreenHeight(), theLocationNameCStr, true, true, true, alpha);
					}
				}
			
            //}
		}

		// Draw hive locations under each hive indicator
		if(!theIsGestating && !theIsBeingDigested && (this->mMapMode == MAP_MODE_NS))
		{
			int theHiveIndex = 0;
			for(HiveInfoListType::iterator theIter = this->mHiveInfoList.begin(); theIter != this->mHiveInfoList.end(); theIter++, theHiveIndex++)
			{
				vec3_t theHivePosition(theIter->mPosX, theIter->mPosY, theIter->mPosZ);
				string theLocationName = this->GetNameOfLocation(theHivePosition);
			
                int theHiveWidth = kHiveNormScreenWidth*ScreenWidth();

                int theScreenPosX = mViewport[0] + mViewport[2] - theHiveWidth;
				int theScreenPosY = mViewport[1] + (kHiveNormScreenY + theHiveIndex*kHiveNormScreenVerticalSpacing + kHiveNormScreenHeight)*ScreenHeight();

				string theTranslatedLocationName;
				LocalizeString(theLocationName.c_str(), theTranslatedLocationName);
			
				AvHCUTrimExtraneousLocationText(theTranslatedLocationName);
			
				int theR, theG, theB;
				this->GetPrimaryHudColor(theR, theG, theB, false, false);
			
				// Draw text in greyscale if hive isn't building or active
				if(theIter->mStatus == kHiveInfoStatusUnbuilt)
				{
					theR = theG = theB = 100;
				}
			
				this->DrawHudStringReverse(mViewport[0] + mViewport[2] - 5, theScreenPosY, ScreenWidth(), (char*)theTranslatedLocationName.c_str(), theR, theG, theB);
			}
		}
	}


}

void AvHHud::DrawWarpedOverlaySprite(int spriteHandle, int numXFrames, int numYFrames, float inWarpXAmount, float inWarpYAmount, float inWarpXSpeed, float inWarpYSpeed)
{
    
    float dx = ScreenWidth();
    float dy = ScreenHeight();

	float theCurrentTime = gHUD.GetTimeOfLastUpdate();
	float theNormXAmount = theCurrentTime*inWarpXSpeed - (int)(theCurrentTime*inWarpXSpeed); // Get fractional part of second
	float theNormYAmount = theCurrentTime*inWarpYSpeed - (int)(theCurrentTime*inWarpYSpeed);
	float theSinusoidalNormXAmount = cos(theNormXAmount*2.0f*M_PI);
	float theSinusoidalNormYAmount = sin(theNormYAmount*2.0f*M_PI);
	float theXAmount = theSinusoidalNormXAmount*inWarpXAmount;
	float theYAmount = theSinusoidalNormYAmount*inWarpYAmount;

    for (int frameY = 0; frameY < numYFrames; ++frameY)
    {
        for (int frameX = 0; frameX < numXFrames; ++frameX)
        {

            float theWarpXStartAmount = 0;
		    float theWarpYStartAmount = 0;
		    float theWarpXEndAmount = 0;
		    float theWarpYEndAmount = 0;
            
            int frame = frameX + frameY * numXFrames;

            float pw = SPR_Width(spriteHandle, frame);
            float ph = SPR_Height(spriteHandle, frame);
            
            float fx1 = ((float)(frameX)) / numXFrames;
            float fy1 = ((float)(frameY)) / numYFrames;
            
            float fx2 = ((float)(frameX + 1)) / numXFrames;
            float fy2 = ((float)(frameY + 1)) / numYFrames;

			if(frameX > 0)
			{
				theWarpXStartAmount = theXAmount*ScreenWidth();
			}
			if(frameX < numXFrames - 1)
			{
				theWarpXEndAmount = theXAmount*ScreenWidth();
			}
			if(frameY > 0)
			{
				theWarpYStartAmount = theYAmount*ScreenHeight();
			}
			if(frameY < numYFrames - 1)
			{
				theWarpYEndAmount = theYAmount*ScreenHeight();
			}
            
            AvHSpriteDraw(spriteHandle, frame,
                 dx * fx1 + theWarpXStartAmount,
                 dy * fy1 + theWarpYStartAmount,
                 dx * fx2 + theWarpXEndAmount,
                 dy * fy2 + theWarpYEndAmount,
                 1 / pw, 1 / ph, 1 - 1 / pw, 1 - 1 / ph);

        }
    }

}

void AvHHud::RenderNoZBuffering()
{

    if (mSteamUIActive)
    {
        // Hack since the HUD doesn't get drawn while the Steam UI is open.
        Render();
    }

	// Don't draw this stuff while being digested
	if(this->GetIsBeingDigested() || gParticleEditorHandler.GetInEditMode())
	{
		return;
	}

	// Temporary, so we can make movies without having the UI flit around (known HL playback bug)
	if(/*gEngfuncs.pDemoAPI->IsPlayingback() ||*/ gViewPort->IsOptionsMenuVisible())
	{
		return;
	}

	const int theDistance = 50;
	
	const float kAspectRatio = ScreenWidth()/ScreenHeight();
	
	float m_vRenderOrigin[3];
	pVector m_vUp;
	pVector m_vRight;
	pVector m_vNormal;
	
	//vec3_t theViewAngles;
	//gEngfuncs.GetViewAngles((float*)theViewAngles)
	//gEngfuncs.GetView
	
	IEngineStudio.GetViewInfo( m_vRenderOrigin, (float*)&m_vUp, (float*)&m_vRight, (float*)&m_vNormal );
	
	//float m_fSoftwareXScale, m_fSoftwareYScale;
	//IEngineStudio.GetAliasScale( &m_fSoftwareXScale, &m_fSoftwareYScale );
	
	//vec3_t theViewHeight;
	//gEngfuncs.pEventAPI->EV_LocalPlayerViewheight(theViewHeight);
	
	//	pVector p;
	//	p.x = m_vRenderOrigin[0] + m_vNormal.x*theDistance;
	//	p.y = m_vRenderOrigin[1] + m_vNormal.y*theDistance;
	//	p.z = m_vRenderOrigin[2] + m_vNormal.z*theDistance;
	
	//	pVector p;
	//	p.x = m_vRenderOrigin[0];
	//	p.y = m_vRenderOrigin[1];
	//	p.z = m_vRenderOrigin[2];
	//	p.x = v_origin[0];
	//	p.y = v_origin[1];
	//	p.z = v_origin[2];
	
	// Allow scripts to perform render
	AvHScriptManager::Instance()->DrawNoZBuffering();
	
//	if(cl_particleinfo->value)
//	{
//		// Draw particles here for debugging
//		pVector theRealView;
//		vec3_t angles, up, right, forward;
//		gEngfuncs.pfnAngleVectors(pmove->angles, forward, right, up);
//		
//		theRealView.x = forward[0];
//		theRealView.y = forward[1];
//		theRealView.z = forward[2];
//
//		AvHParticleSystemManager::Instance()->Draw(theRealView);
//	}
	
	//if(this->mMappingTechSprite && !this->mIsRenderingSelectionView && !this->mGameStarted)
	//{
	//	float theXPercentage = 256/1024.0f;
	//	float theYPercentage = 64/768.0f;
	//	DrawScaledHUDSprite(this->mMappingTechSprite, kRenderTransColor, 1, 0, ScreenHeight/2, theXPercentage*ScreenWidth, theYPercentage*ScreenHeight());
	//}

	// Draw blips, orders, selection and build effects for spectators and demo watchers too
	int theUpgradeVar = this->GetHUDUpgrades();
	bool theHasHiveSightUpgrade = true;//GetHasUpgrade(theUpgradeVar, MASK_ALIEN_UPGRADE_11);

	if(this->GetIsAlien())
	{
		if(theHasHiveSightUpgrade)
		{
			// Draw friendly blips
			this->mFriendlyBlips.Draw(m_vNormal, kFriendlyBlipStatus);
		}
	}
	
	// Draw enemy blips
	this->mEnemyBlips.Draw(m_vNormal, kEnemyBlipStatus);
	
	//this->DrawOrders(true);
	this->DrawSelectionAndBuildEffects();
	//this->DrawHelpIcons();

}

void AvHHud::InitHUDData( void )
{
	this->ResetGame(true);
}

void AvHHud::VidInit(void)
{
	UIHud::VidInit();

    mOverviewMap.VidInit();

	int theScreenWidth = ScreenWidth();
	string theSpriteName;
	
	//	theSpriteName = UINameToSprite(kEggSprite, theScreenWidth);
	//	this->mAlienUIEggSprite = Safe_SPR_Load(theSpriteName.c_str());
	
	//	theSpriteName = UINameToSprite(kHiveSprite, theScreenWidth);
	//	this->mAlienUIHiveSprite = Safe_SPR_Load(theSpriteName.c_str());
	
	int i = 0;
	//	for(i = 0; i < kNumAlienLifeforms; i++)
	//	{
	//		char theBaseName[128];
	//		sprintf(theBaseName, "%s%d", kLifeformSprite, i+1);
	//		string theSpriteName = "sprites/level1_hud.spr";//UINameToSprite(theBaseName, theScreenWidth, true);
	//		this->mAlienUILifeforms[i] = Safe_SPR_Load(theSpriteName.c_str());
	//	}
	
	char theBaseName[128];
	sprintf(theBaseName, "%s", kAlienUpgradeSprite);
	theSpriteName = UINameToSprite(theBaseName, theScreenWidth);
	this->mAlienUIUpgrades = Safe_SPR_Load(theSpriteName.c_str());
	
	sprintf(theBaseName, "%s", kAlienUpgradeCategory);
	theSpriteName = UINameToSprite(theBaseName, theScreenWidth);
	this->mAlienUIUpgradeCategories = Safe_SPR_Load(theSpriteName.c_str());
	
	// Load jetpack sprite
	theSpriteName = UINameToSprite(kJetpackSprite, theScreenWidth);
	this->mMarineUIJetpackSprite = Safe_SPR_Load(theSpriteName.c_str());

	// Load alien energy sprite
	theSpriteName = UINameToSprite(kAlienEnergySprite, theScreenWidth);
	this->mAlienUIEnergySprite = Safe_SPR_Load(theSpriteName.c_str());
	theSpriteName = UINameToSprite(kAlienCloakSprite, theScreenWidth);
	this->mAlienUICloakSprite = Safe_SPR_Load(theSpriteName.c_str());

	// Load background for topdown mode
	this->mBackgroundSprite = Safe_SPR_Load(kTopDownBGSprite);

	// Load HUD
	this->mTopDownTopSprite = Safe_SPR_Load(kTopDownTopHUDSprite);
	this->mTopDownBottomSprite = Safe_SPR_Load(kTopDownBottomHUDSprite);
	this->mMarineTopSprite = Safe_SPR_Load(kMarineTopHUDSprite);

	this->mLogoutSprite = Safe_SPR_Load(kLogoutSprite);
	this->mCommandButtonSprite = Safe_SPR_Load(kCommandButtonSprite);
	this->mCommandStatusSprite = Safe_SPR_Load(kCommandStatusSprite);
	this->mSelectAllSprite = Safe_SPR_Load(kSelectAllSprite);

	//this->mTopDownBottomSprite = Safe_SPR_Load("sprites/distorttest.spr");
	//this->mTopDownBottomSprite = Safe_SPR_Load("sprites/ns.spr");
	//this->mTopDownBottomSprite = Safe_SPR_Load("sprites/distorttest.spr");

	// Load overlays
	this->mMembraneSprite = Safe_SPR_Load(kMembraneSprite);
	this->mDigestingSprite = Safe_SPR_Load(kDigestingSprite);
	
	// Load order sprite
	theSpriteName = UINameToSprite(kOrdersSprite, theScreenWidth);
	this->mOrderSprite = Safe_SPR_Load(theSpriteName.c_str());
	this->mHiveInfoSprite = Safe_SPR_Load(kHiveInfoSprite);
	this->mHiveHealthSprite = Safe_SPR_Load(kHiveHealthSprite);

	// Load cursor sprite
	this->mMarineCursor = Safe_SPR_Load(kCursorsSprite);
	this->mAlienCursor = Safe_SPR_Load(kAlienCursorSprite);
	this->mMarineOrderIndicator = Safe_SPR_Load(kMarineOrderSprite);
	this->mMarineUpgradesSprite = Safe_SPR_Load(kMarineUpgradesSprite);
	//this->mMappingTechSprite = Safe_SPR_Load("sprites/ns.spr");

	this->mAlienBuildSprite = Safe_SPR_Load(kAlienBuildSprite);
	this->mMarineBuildSprite = Safe_SPR_Load(kMarineBuildSprite);

	this->mAlienHealthSprite = Safe_SPR_Load(kAlienHealthSprite);
	this->mMarineHealthSprite = Safe_SPR_Load(kMarineHealthSprite);

	this->mBuildCircleSprite = Safe_SPR_Load(kBuildCircleSprite);
	//this->mSiegeTurretSprite = Safe_SPR_Load(kSiegeTurretSprite);

	this->mActionButtonSprites.clear();
	//this->mHelpSprites.clear();

	string theIconName = string(kHelpIconPrefix) + ".spr";
	this->mHelpSprite = Safe_SPR_Load(theIconName.c_str());

	// : 0000971
	this->mTeammateOrderSprite = Safe_SPR_Load(kTeammateOrderSprite);
	// :

	this->mExperienceBarSprite = Safe_SPR_Load(kExperienceBarSprite);
	this->mProgressBarSprite = Safe_SPR_Load(kProgressBarSprite);

	this->mEnemyBlips.VidInit();
	this->mFriendlyBlips.VidInit();
}
