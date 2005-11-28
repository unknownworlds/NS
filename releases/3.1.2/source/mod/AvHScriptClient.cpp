//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHScriptClient.cpp $
// $Date: 2002/06/10 20:01:24 $
//
//-------------------------------------------------------------------------------
// $Log: AvHScriptClient.cpp,v $
// Revision 1.2  2002/06/10 20:01:24  Flayra
// - Updated extern references to drawing code (ugh)
//
// Revision 1.1  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
// Revision 1.1  2002/05/14 18:54:48  Charlie
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

extern "C" {
	#include <lua.h>
}

extern void DrawScaledHUDSprite(int inSpriteHandle, int inMode, int inRowsInSprite = 1, int inX = 0, int inY = 0, int inWidth = ScreenWidth(), int inHeight = ScreenHeight(), int inForceSpriteFrame = -1, float inStartU = 0.0f, float inStartV = 0.0f, float inEndU = 1.0f, float inEndV = 1.0f, float inRotateUVRadians = 0.0f, bool inUVWrapsOverFrames = false);
extern vec3_t v_origin;

static int errormessage(lua_State* inState)
{
	const char *s = lua_tostring(inState, 1);
	if (s == NULL) s = "(no message)";
	char theErrorMessage[2048];
	sprintf(theErrorMessage, "Script error: %s\n", s);
	
	// Print the message					
	//gEngfuncs.Con_Printf("%s", theErrorMessage);
	
	return 0;
}

static int print(lua_State* inState)
{
	int n = lua_gettop(inState);
	int i;
	string theString;
	
	for (i=1; i<=n; i++)
	{
		if (i>1) theString = "\t";
		if (lua_isstring(inState, i))
			theString += string(lua_tostring(inState, i));
		else
		{
			char theBuffer[512];
			sprintf(theBuffer, "%s:%p",lua_typename(inState, lua_type(inState, i)),lua_topointer(inState,i));
			theString += string(theBuffer);
		}
	}
	//theString += string("\n");

	// TODO: Print message
	//UTIL_ClientPrintAll(HUD_PRINTNOTIFY, theString.c_str());
	gEngfuncs.Con_Printf("%s", theString.c_str());
	
	return 0;
}

// Runs a client command, returns success
static int clientCommand(lua_State* inState)
{
	bool theSuccess = false;

	int theNumArgs = lua_gettop(inState);
	if(theNumArgs >= 1)
	{
		string theConsoleCommandString = lua_tostring(inState, 1);

		char theConsoleCommand[1024];
		strcpy(theConsoleCommand, theConsoleCommandString.c_str());
		ClientCmd(theConsoleCommand);

		theSuccess = true;
	}

	lua_pushnumber(inState, theSuccess);

	return 1;
}

// Returns xyz
static int getViewOrigin(lua_State* inState)
{
	lua_pushnumber(inState, v_origin.x);
	lua_pushnumber(inState, v_origin.y);
	lua_pushnumber(inState, v_origin.z);

	return 3;
}

static int getScreenWidth(lua_State* inState)
{
	int theScreenWidth = ScreenWidth();
	lua_pushnumber(inState, theScreenWidth);
	return 1;
}

static int getScreenHeight(lua_State* inState)
{
	int theScreenHeight = ScreenHeight();
	lua_pushnumber(inState, theScreenHeight);
	return 1;
}

// string spriteName, int inMode, int x0, int y0, int theWidth, int theHeight, optional frame
static int drawScaledHUDSprite(lua_State* inState)
{
	bool theSuccess = false;

	int theNumArgs = lua_gettop(inState);
	if(theNumArgs >= 6)
	{
		string theSpriteName = lua_tostring(inState, 1);
		int theRenderMode = lua_tonumber(inState, 2);
		int theStartX = lua_tonumber(inState, 3);
		int theStartY = lua_tonumber(inState, 4);
		int theWidth = lua_tonumber(inState, 5);
		int theHeight = lua_tonumber(inState, 6);
		int theFrame = 0;
		
		// Read optional frame
		if(theNumArgs >= 7)
		{
			theFrame = lua_tonumber(inState, 7);
		}
		
		int theSpriteHandle = Safe_SPR_Load(theSpriteName.c_str());
		if(theSpriteHandle)
		{
			DrawScaledHUDSprite(theSpriteHandle, theRenderMode, 1, theStartX, theStartY, theWidth, theHeight, theFrame);
			theSuccess = true;
		}
	}
	
	lua_pushnumber(inState, theSuccess);
	
	return 1;
}

// Returns success if a valid mode is passed in
static int triRenderMode(lua_State* inState)
{
	bool theSuccess = false;
	
	int theNumArgs = lua_gettop(inState);
	if(theNumArgs >= 1)
	{
		int theRenderMode = lua_tonumber(inState, 1);
		if((theRenderMode >= kRenderNormal) && (theRenderMode <= kRenderTransAdd))
		{
			gEngfuncs.pTriAPI->RenderMode(theRenderMode);
			theSuccess = true;
		}
	}
	
	lua_pushnumber(inState, theSuccess);
	
	return 1;
}

// Pass in primitive code, returns success
static int triBegin(lua_State* inState)
{
	bool theSuccess = false;
	
	int theNumArgs = lua_gettop(inState);
	if(theNumArgs >= 1)
	{
		int thePrimitiveCode = lua_tonumber(inState, 1);
		if((thePrimitiveCode >= TRI_TRIANGLES) && (thePrimitiveCode <= TRI_QUAD_STRIP))
		{
			gEngfuncs.pTriAPI->Begin(thePrimitiveCode);
			theSuccess = true;
		}
	}
	
	lua_pushnumber(inState, theSuccess);
	
	return 1;
}

// Takes and returns nothing
static int triEnd(lua_State* inState)
{
	gEngfuncs.pTriAPI->End();
	
	return 0;
}

// Takes four floats (rgba), returns success
static int triColor4f(lua_State* inState)
{
	bool theSuccess = false;
	
	int theNumArgs = lua_gettop(inState);
	if(theNumArgs >= 4)
	{
		float theR = lua_tonumber(inState, 1);
		float theG = lua_tonumber(inState, 2);
		float theB = lua_tonumber(inState, 3);
		float theA = lua_tonumber(inState, 4);
		
		gEngfuncs.pTriAPI->Color4f(theR, theG, theB, theA);
		theSuccess = true;
	}
	
	lua_pushnumber(inState, theSuccess);
	
	return 1;
}

// Takes four ints (rgba), returns success
static int triColor4ub(lua_State* inState)
{
	bool theSuccess = false;
	
	int theNumArgs = lua_gettop(inState);
	if(theNumArgs >= 4)
	{
		int theR = lua_tonumber(inState, 1);
		int theG = lua_tonumber(inState, 2);
		int theB = lua_tonumber(inState, 3);
		int theA = lua_tonumber(inState, 4);
		
		gEngfuncs.pTriAPI->Color4ub(theR, theG, theB, theA);
		
		theSuccess = true;
	}
	
	lua_pushnumber(inState, theSuccess);
	
	return 1;
}

// Takes two floats (0 to 1), returns success
static int triTexCoord2f(lua_State* inState)
{
	bool theSuccess = false;
	
	int theNumArgs = lua_gettop(inState);
	if(theNumArgs >= 2)
	{
		float theU = lua_tonumber(inState, 1);
		float theV = lua_tonumber(inState, 2);
		
		gEngfuncs.pTriAPI->TexCoord2f(theU, theV);
		
		theSuccess = true;
	}
	
	lua_pushnumber(inState, theSuccess);
	
	return 1;
}

// Takes three floats, returns success
static int triVertex3f(lua_State* inState)
{
	bool theSuccess = false;
	
	int theNumArgs = lua_gettop(inState);
	if(theNumArgs >= 3)
	{
		float theX = lua_tonumber(inState, 1);
		float theY = lua_tonumber(inState, 2);
		float theZ = lua_tonumber(inState, 3);
		
		gEngfuncs.pTriAPI->Vertex3f(theX, theY, theZ);
		
		theSuccess = true;
	}
	
	lua_pushnumber(inState, theSuccess);
	
	return 1;
}

// Takes float (0-1), returns success
static int triBrightness(lua_State* inState)
{
	bool theSuccess = false;
	
	int theNumArgs = lua_gettop(inState);
	if(theNumArgs >= 1)
	{
		float theBrightness = lua_tonumber(inState, 1);
		
		gEngfuncs.pTriAPI->Brightness(theBrightness);
		theSuccess = true;
	}
	
	lua_pushnumber(inState, theSuccess);
	
	return 1;
}

// Takes cullstyle int (0 or 1), returns success
static int triCullFace(lua_State* inState)
{
	bool theSuccess = false;
	
	int theNumArgs = lua_gettop(inState);
	if(theNumArgs >= 1)
	{
		int theCullStyleInt = lua_tonumber(inState, 1);
		TRICULLSTYLE theCullStyle = TRICULLSTYLE(theCullStyleInt);
		if((theCullStyle == TRI_FRONT) || (theCullStyle == TRI_NONE))
		{
			gEngfuncs.pTriAPI->CullFace(theCullStyle);
			theSuccess = true;
		}
	}
	
	lua_pushnumber(inState, theSuccess);
	
	return 1;
}

// Takes texture name, and optional frame, sets the current texture.  Returns true or false.
static int triSpriteTexture(lua_State* inState)
{
	bool theSuccess = false;
	
	int theNumArgs = lua_gettop(inState);
	if(theNumArgs >= 1)
	{
		string theSpriteName = lua_tostring(inState, 1);
		int theSpriteHandle = Safe_SPR_Load(theSpriteName.c_str());
		if(theSpriteHandle)
		{
			int theSpriteFrame = 0;
			if(theNumArgs >= 2)
			{
				theSpriteFrame = lua_tonumber(inState, 2);
			}
			
			if(gEngfuncs.pTriAPI->SpriteTexture((struct model_s *)gEngfuncs.GetSpritePointer(theSpriteHandle), theSpriteFrame))
			{
				theSuccess = true;
			}
		}
	}
	
	lua_pushnumber(inState, theSuccess);
	
	return 1;
}

// Takes xyz, returns success, xy
static int triWorldToScreen(lua_State* inState)
{
	bool theSuccess = false;
	
	float theScreenPos[3];
	memset(theScreenPos, 0, sizeof(float)*3);
	
	int theNumArgs = lua_gettop(inState);
	if(theNumArgs >= 3)
	{
		float theWorldPos[3];
		theWorldPos[0] = lua_tonumber(inState, 1);
		theWorldPos[1] = lua_tonumber(inState, 2);
		theWorldPos[2] = lua_tonumber(inState, 3);
		
		gEngfuncs.pTriAPI->WorldToScreen(theWorldPos, theScreenPos);
		
		theSuccess = true;
	}
	
	lua_pushnumber(inState, theSuccess);
	lua_pushnumber(inState, theScreenPos[0]);
	lua_pushnumber(inState, theScreenPos[1]);
	
	return 3;
}

// 3 floats for fog color, float fogStart, float fogEnd, int 0/1 for fog on or off.  Returns true if correct parameters were passed in
static int triFog(lua_State* inState)
{
	bool theSuccess = false;
	
	int theNumArgs = lua_gettop(inState);
	if(theNumArgs >= 6)
	{
		float theFogColor[3];
		theFogColor[0] = lua_tonumber(inState, 1);
		theFogColor[1] = lua_tonumber(inState, 2);
		theFogColor[2] = lua_tonumber(inState, 3);

		float theFogStart = lua_tonumber(inState, 4);
		float theFogEnd = lua_tonumber(inState, 5);
		int theFogOn = lua_tonumber(inState, 6);
		
		gEngfuncs.pTriAPI->Fog(theFogColor, theFogStart, theFogEnd, theFogOn);

		theSuccess = true;
	}
	
	lua_pushnumber(inState, theSuccess);
	
	return 1;
}


void AvHScriptInstance::InitClient()
{
	//lua_register(this->mState, LUA_ERRORMESSAGE, errormessage);
	lua_register(this->mState, "print", print);
	lua_register(this->mState, "clientCommand", clientCommand);
	lua_register(this->mState, "getViewOrigin", getViewOrigin);

	// Drawing utility functions
	lua_register(this->mState, "getScreenWidth", getScreenWidth);
	lua_register(this->mState, "getScreenHeight", getScreenHeight);
	lua_register(this->mState, "drawScaledHUDSprite", drawScaledHUDSprite);

	// Tri API hooks
	lua_register(this->mState, "triRenderMode", triRenderMode);
	lua_register(this->mState, "triBegin", triBegin);
	lua_register(this->mState, "triEnd", triEnd);
	lua_register(this->mState, "triColor4f", triColor4f);
	lua_register(this->mState, "triColor4ub", triColor4ub);
	lua_register(this->mState, "triTexCoord2f", triTexCoord2f);
	lua_register(this->mState, "triVertex3f", triVertex3f);
	lua_register(this->mState, "triBrightness", triBrightness);
	lua_register(this->mState, "triCullFace", triCullFace);
	lua_register(this->mState, "triSpriteTexture", triSpriteTexture);
	lua_register(this->mState, "triWorldToScreen", triWorldToScreen);
	lua_register(this->mState, "triFog", triFog);

	// Input hooks

	//lua_register(this->mState, "drawScaledHUDSprite", drawScaledHUDSpriteUV);
	//lua_register(this->mState, "drawScaledTiledHUDSprite", drawScaledTiledHUDSprite);
}