/***
*
*	Copyright (c) 1999, Valve LLC. All rights reserved.
*	
*	This product contains software technology licensed from Id 
*	Software, Inc. ("Id Technology").  Id Technology (c) 1996 Id Software, Inc. 
*	All Rights Reserved.
*
*   Use, distribution, and modification of this source code and/or resulting
*   object code is restricted to non-commercial enhancements to products from
*   Valve LLC.  All other use, distribution, or modification is prohibited
*   without written permission from Valve LLC.
*
****/
//
// util.cpp
//
// implementation of class-less helper functions
//

#include "STDIO.H"
#include "STDLIB.H"
#include "MATH.H"

#include "hud.h"
#include "cl_util.h"
#include "common/cl_entity.h"
#include <string.h>
#ifndef M_PI
#define M_PI		3.14159265358979323846	// matches value in gcc v2 math.h
#endif

#include "pm_shared/pm_defs.h"
#include "pm_shared/pm_shared.h"
#include "pm_shared/pm_movevars.h"
extern playermove_t *pmove;

extern float HUD_GetFOV( void );
vec3_t vec3_origin( 0, 0, 0 );
extern vec3_t v_angles;
//double sqrt(double x);
//
//float Length(const float *v)
//{
//	int		i;
//	float	length;
//	
//	length = 0;
//	for (i=0 ; i< 3 ; i++)
//		length += v[i]*v[i];
//	length = sqrt (length);		// FIXME
//
//	return length;
//}
//
//void VectorAngles( const float *forward, float *angles )
//{
//	float	tmp, yaw, pitch;
//	
//	if (forward[1] == 0 && forward[0] == 0)
//	{
//		yaw = 0;
//		if (forward[2] > 0)
//			pitch = 90;
//		else
//			pitch = 270;
//	}
//	else
//	{
//		yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
//		if (yaw < 0)
//			yaw += 360;
//
//		tmp = sqrt (forward[0]*forward[0] + forward[1]*forward[1]);
//		pitch = (atan2(forward[2], tmp) * 180 / M_PI);
//		if (pitch < 0)
//			pitch += 360;
//	}
//	
//	angles[0] = pitch;
//	angles[1] = yaw;
//	angles[2] = 0;
//}
//
//float VectorNormalize (float *v)
//{
//	float	length, ilength;
//
//	length = v[0]*v[0] + v[1]*v[1] + v[2]*v[2];
//	length = sqrt (length);		// FIXME
//
//	if (length)
//	{
//		ilength = 1/length;
//		v[0] *= ilength;
//		v[1] *= ilength;
//		v[2] *= ilength;
//	}
//		
//	return length;
//
//}
//
//void VectorInverse ( float *v )
//{
//	v[0] = -v[0];
//	v[1] = -v[1];
//	v[2] = -v[2];
//}
//
//void VectorScale (const float *in, float scale, float *out)
//{
//	out[0] = in[0]*scale;
//	out[1] = in[1]*scale;
//	out[2] = in[2]*scale;
//}
//
//void VectorMA (const float *veca, float scale, const float *vecb, float *vecc)
//{
//	vecc[0] = veca[0] + scale*vecb[0];
//	vecc[1] = veca[1] + scale*vecb[1];
//	vecc[2] = veca[2] + scale*vecb[2];
//}

int ScreenHeight()
{
	// CGC: Replace code when ready to fix overview map
	//int theViewport[4];
	//gHUD.GetViewport(theViewport);
	//return theViewport[3];
	return gHUD.m_scrinfo.iHeight;
}

// ScreenWidth returns the width of the screen, in pixels
//#define ScreenWidth ( int theViewport[4];  gHUD.GetViewport(theViewport);  return theViewport[2]; )
int ScreenWidth()
{
	//int theViewport[4];
	//gHUD.GetViewport(theViewport);
	//return theViewport[2];
	return gHUD.m_scrinfo.iWidth;
}


HSPRITE Safe_SPR_Load(const char* inSpriteName)
{
	HSPRITE theSpriteHandle = gEngfuncs.pfnSPR_Load(inSpriteName);
	
	// Check for "Can't allocate 128 HUD sprites" crash
	ASSERT(theSpriteHandle < 128);
	
	return theSpriteHandle;
}

//-----------------------------------------------------------------------------
// Purpose: 
// Given a field of view and mouse/screen positions as well as the current
//  render origin and angles, returns a unit vector through the mouse position
//  that can be used to trace into the world under the mouse click pixel.
// Input  : fov - 
//                      mousex - 
//                      mousey - 
//                      screenwidth - 
//                      screenheight - 
//                      vecRenderAngles - 
//                      c_x - 
//                      vpn - 
//                      vup - 
//                      360.0 - 
//-----------------------------------------------------------------------------
void CreatePickingRay( int mousex, int mousey, Vector& outVecPickingRay )
{
	float dx, dy;
	float c_x, c_y;
	float dist;
	Vector vpn, vup, vright;

//	char gDebugMessage[256];

	float fovDegrees = gHUD.m_iFOV;

	//cl_entity_s* theLocalEntity = gEngfuncs.GetLocalPlayer();
	//Vector vecRenderOrigin = theLocalEntity->origin;
	//Vector vecRenderAngles = theLocalEntity->angles;
	//Vector vecRenderAngles;
	vec3_t vecRenderAngles;
	//gEngfuncs.GetViewAngles((float*)vecRenderAngles);
	VectorCopy(v_angles, vecRenderAngles);

	//vec3_t theForward, theRight, theUp;
	//AngleVectors(v_angles, theForward, theRight, theUp);

	//ASSERT(v_angles.x == vecRenderAngles.x);
	//ASSERT(v_angles.y == vecRenderAngles.y);
	//ASSERT(v_angles.z == vecRenderAngles.z);
	
	c_x = ScreenWidth() / 2;
	c_y = ScreenHeight() / 2;
	
	
	dx = (float)mousex - c_x;
	// Invert Y
	dy = c_y - (float)mousey;

//	sprintf(gDebugMessage, "inMouseX/Y: %d/%d, dx/dy = %d/%d", (int)mousex, (int)mousey, (int)dx, (int)dy);
//	CenterPrint(gDebugMessage);
	
	// Convert view plane distance
	dist = c_x / tan( M_PI * fovDegrees / 360.0 );
	
	
	// Decompose view angles
	AngleVectors( vecRenderAngles, vpn, vright, vup );
	
	
	// Offset forward by view plane distance, and then by pixel offsets
	outVecPickingRay = vpn * dist + vright * ( dx ) + vup * ( dy );

	//sprintf(gDebugMessage, "outVecPickingRay: %.0f, %.0f, %.0f", outVecPickingRay.x, outVecPickingRay.y, outVecPickingRay.z);
	//CenterPrint(gDebugMessage);
	
	// Convert to unit vector
	VectorNormalize( outVecPickingRay );
}

void FillRGBAClipped(vgui::Panel* inPanel, int inStartX, int inStartY, int inWidth, int inHeight, int r, int g, int b, int a)
{
	int thePanelXPos, thePanelYPos;
	inPanel->getPos(thePanelXPos, thePanelYPos);

	int thePanelWidth, thePanelHeight;
	inPanel->getSize(thePanelWidth, thePanelHeight);

	// Clip starting point
	inStartX = min(max(0, inStartX), thePanelWidth-1);
	inStartY = min(max(0, inStartY), thePanelHeight-1);

	// Clip width if it goes too far
	ASSERT(inWidth >= 0);
	ASSERT(inHeight >= 0);

	if(inStartX + inWidth >= thePanelWidth)
	{
		inWidth = max(0, thePanelWidth - inStartX);
	}
	if(inStartY + inHeight >= thePanelHeight)
	{
		inHeight = max(0, thePanelHeight - inStartY);
	}

	// Now we can draw
	FillRGBA(inStartX, inStartY, inWidth, inHeight, r, g, b, a);
	
}


HSPRITE LoadSprite(const char *pszName)
{
	int i;
	char sz[256]; 

	if (ScreenWidth() < 640)
		i = 320;
	else
		i = 640;

	sprintf(sz, pszName, i);

	return Safe_SPR_Load(sz);
}

bool LocalizeString(const char* inMessage, string& outputString)
{
	#define kMaxLocalizedStringLength 1024

	bool theSuccess = false;
	char theInputString[kMaxLocalizedStringLength];
	char theOutputString[kMaxLocalizedStringLength];

	// Don't localize empty strings
	if(strcmp(inMessage, ""))
	{
		if(*inMessage != '#')
		{
			sprintf(theInputString, "#%s", inMessage);
		}
		else
		{
			sprintf(theInputString, "%s", inMessage);
		}
		
		if((CHudTextMessage::LocaliseTextString(theInputString, theOutputString, kMaxLocalizedStringLength) != NULL))
		{
			outputString = theOutputString;

			if(theOutputString[0] != '#')
			{
				theSuccess = true;
			}
			else
			{
				string theTempString = theOutputString;
				theTempString = theTempString.substr(1, theTempString.length());
				outputString = theTempString;
			}
		}
		else
		{
			outputString = string("err: ") + theInputString;
		}
	}
	
	return theSuccess;
}