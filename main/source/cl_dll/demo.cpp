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
#include "hud.h"
#include "cl_util.h"
#include "demo.h"
#include "common/demo_api.h"
#include <memory.h>

#include "engine/APIProxy.h"
#include "Exports.h"
#include "mod/AvHParticleTemplateClient.h"
#include "cl_dll/view.h"

extern AvHParticleTemplateListClient	gParticleTemplateList;

int g_demosniper = 0;
int g_demosniperdamage = 0;
float g_demosniperorg[3];
float g_demosniperangles[3];
float g_demozoom;
vec3_t gPlaybackViewOrigin;

int	ScorePanel_InitializeDemoPlayback(int inSize, unsigned char* inBuffer);
void ScorePanel_InitializeDemoRecording();

float gNormMouseX = 0;
float gNormMouseY = 0;

// FIXME:  There should be buffer helper functions to avoid all of the *(int *)& crap.

void Demo_WriteByte(int inType, unsigned char inByte)
{
	Demo_WriteBuffer(inType, sizeof(unsigned char), &inByte);
}

void Demo_WriteVector(int inType, vec3_t inVector)
{
	float theFloatArray[3];

	theFloatArray[0] = inVector.x;
	theFloatArray[1] = inVector.y;
	theFloatArray[2] = inVector.z;

	Demo_WriteBuffer(inType, 3*sizeof(float), (unsigned char*)theFloatArray);
}

void Demo_WriteFloat(int inType, float inFloat)
{
	Demo_WriteBuffer(inType, sizeof(float), (unsigned char*)&inFloat);
}

void Demo_WriteInt(int inType, int inInt)
{
	Demo_WriteBuffer(inType, sizeof(int), (unsigned char*)&inInt);
}

/*
=====================
Demo_WriteBuffer

Write some data to the demo stream
=====================
*/
void Demo_WriteBuffer( int type, int size, unsigned char *buffer )
{
	int pos = 0;
	unsigned char buf[ 32 * 1024 ];
	*( int * )&buf[pos] = type;
	pos+=sizeof( int );

	memcpy( &buf[pos], buffer, size );

	// Write full buffer out
	gEngfuncs.pDemoAPI->WriteBuffer( size + sizeof( int ), buf );
}

/*
=====================
Demo_ReadBuffer

Engine wants us to parse some data from the demo stream
=====================
*/
void CL_DLLEXPORT Demo_ReadBuffer( int size, unsigned char *buffer )
{
	RecClReadDemoBuffer(size, buffer);
	
	int type;
	int i = 0;
	bool theMouseVisibility = false;

	type = *( int * )buffer;
	i += sizeof( int );
	switch ( type )
	{
	case TYPE_SNIPERDOT:
		g_demosniper = *(int * )&buffer[ i ];
		i += sizeof( int );
		
		if ( g_demosniper )
		{
			g_demosniperdamage = *( int * )&buffer[ i ];
			i += sizeof( int );

			g_demosniperangles[ 0 ] = *(float *)&buffer[i];
			i += sizeof( float );
			g_demosniperangles[ 1 ] = *(float *)&buffer[i];
			i += sizeof( float );
			g_demosniperangles[ 2 ] = *(float *)&buffer[i];
			i += sizeof( float );
			g_demosniperorg[ 0 ] = *(float *)&buffer[i];
			i += sizeof( float );
			g_demosniperorg[ 1 ] = *(float *)&buffer[i];
			i += sizeof( float );
			g_demosniperorg[ 2 ] = *(float *)&buffer[i];
			i += sizeof( float );
		}
		break;
	case TYPE_ZOOM:
		g_demozoom = *(float * )&buffer[ i ];
		i += sizeof( float );
		break;

	case TYPE_BASESTATE:
		i += gHUD.InitializeDemoPlayback(size, (unsigned char *)&buffer[i]);
		break;

	case TYPE_MOUSEVIS:
		//theMouseVisibility = *(unsigned char * )&buffer[ i ];
		//gHUD.GetManager().SetMouseVisibility(theMouseVisibility);
		i += sizeof(unsigned char);
		break;

	case TYPE_MOUSEX:
		//gNormMouseX = *(float * )&buffer[ i ];
		i += sizeof( float );
		break;
		
	case TYPE_MOUSEY:
		//gNormMouseY = *(float * )&buffer[ i ];
		i += sizeof( float );
		break;

	case TYPE_VIEWANGLES:
		v_angles[0] = *(float * )&buffer[ i ];
		i += sizeof( float );
		
		v_angles[1] = *(float * )&buffer[ i ];
		i += sizeof( float );
		
		v_angles[2] = *(float * )&buffer[ i ];
		i += sizeof( float );
		break;

	case TYPE_VIEWORIGIN:
		v_origin[0] = *(float * )&buffer[ i ];
		i += sizeof( float );
		
		v_origin[1] = *(float * )&buffer[ i ];
		i += sizeof( float );

		v_origin[2] = *(float * )&buffer[ i ];
		i += sizeof( float );
		break;

	case TYPE_PARTICLES:
		i += gParticleTemplateList.InitializeDemoPlayback(size, (unsigned char*)&buffer[i]);
		break;

	case TYPE_BASESTATE2:
		i += gHUD.InitializeDemoPlayback2(size, (unsigned char *)&buffer[i]);
		break;

	case TYPE_PLAYERINFO:
		i += ScorePanel_InitializeDemoPlayback(size, (unsigned char *)&buffer[i]);
		break;

	case TYPE_WEAPONINFO:
		i += gHUD.InitializeWeaponInfoPlayback(size, (unsigned char *)&buffer[i]);
		break;

	default:
		gEngfuncs.Con_DPrintf( "Unknown demo buffer type, skipping.\n" );
		break;
	}
}

// Returns num bytes needed to save/restore a string
int GetDataSize(const string& inString)
{
	// String length
	int theSize = sizeof(int);

	// String data
	theSize += (int)inString.length();

	return theSize;
}

void LoadData(void* inBuffer, const unsigned char* inData, int inSizeToCopy, int& inSizeVariable)
{
	memcpy(inBuffer, inData + inSizeVariable, inSizeToCopy);
	inSizeVariable += inSizeToCopy;
}

void LoadStringData(string& outString, const unsigned char* inData, int& inBytesRead)
{
	// Read in string length (could be 0)
	int theStringLength = 0;
	memcpy(&theStringLength, inData + inBytesRead, sizeof(int));
	inBytesRead += sizeof(int);

	// Read in string, if greater then 0
	if(theStringLength > 0)
	{
		char* theStringData = new char[theStringLength+1];
		memcpy(theStringData, inData + inBytesRead, theStringLength);
		theStringData[theStringLength] = '\0';
		inBytesRead += theStringLength;
		
		outString = string(theStringData);

		delete [] theStringData;
		theStringData = NULL;
	}
}

void LoadVectorData(float* outData, const unsigned char* inData, int& inBytesRead)
{
	int theSize = 3*sizeof(float);
	memcpy(outData, inData + inBytesRead, theSize);
	inBytesRead += theSize;
}

void SaveData(unsigned char* inBuffer, const void* inData, int inSizeToCopy, int& inSizeVariable)
{
	memcpy(inBuffer + inSizeVariable, inData, inSizeToCopy);
	inSizeVariable += inSizeToCopy;
}

// Save out string, works for empty strings
void SaveStringData(unsigned char* inBuffer, const string& inString, int& inSizeVariable)
{
	int theStringLength = (int)inString.length();

	// Write out string length
	memcpy(inBuffer + inSizeVariable, &theStringLength, sizeof(int));
	inSizeVariable += sizeof(int);

	// Write out string data, if any
	memcpy(inBuffer + inSizeVariable, inString.c_str(), theStringLength);
	inSizeVariable += theStringLength;
}

void SaveVectorData(unsigned char* inBuffer, float* inData, int& inSizeVariable)
{
	int theLength = 3*sizeof(float);

	memcpy(inBuffer + inSizeVariable, inData, theLength);
	inSizeVariable += theLength;
}
