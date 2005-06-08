#if !defined( DEMOH )
#define DEMOH
#pragma once

// Types of demo messages we can write/parse
enum
{
	TYPE_SNIPERDOT = 0,
	TYPE_ZOOM,

	TYPE_BASESTATE,
	TYPE_MOUSEVIS,
	TYPE_MOUSEX,
	TYPE_MOUSEY,
	TYPE_VIEWORIGIN,
	TYPE_VIEWANGLES,
	TYPE_PARTICLES,

	// More base state that wasn't originally saved (made a separate type for backwards-compatibility)
	TYPE_BASESTATE2,
	TYPE_PLAYERINFO,
	TYPE_WEAPONINFO,
};

void Demo_WriteVector(int inType, vec3_t inVector);
void Demo_WriteByte(int inType, unsigned char inByte);
void Demo_WriteFloat(int inType, float inFloat);
void Demo_WriteInt(int inType, int inInt);
void Demo_WriteBuffer( int type, int size, unsigned char *buffer );
#define LOAD_DATA(x) LoadData(&x, inBuffer, sizeof(x), theBytesRead);
#define SAVE_DATA(x) SaveData(theCharArray, &x, sizeof(x), theCounter);

int GetDataSize(const string& inString);

void LoadData(void* inBuffer, const unsigned char* inData, int inSizeToCopy, int& inSizeVariable);
void LoadStringData(string& outString, const unsigned char* inData, int& inBytesRead);
void LoadVectorData(float* outData, const unsigned char* inData, int& inBytesRead);

void SaveData(unsigned char* inBuffer, const void* inData, int inSizeToCopy, int& inSizeVariable);
void SaveStringData(unsigned char* inBuffer, const string& inString, int& inSizeVariable);
void SaveVectorData(unsigned char* inBuffer, float* inData, int& inSizeVariable);

extern int g_demosniper;
extern int g_demosniperdamage;
extern float g_demosniperorg[3];
extern float g_demosniperangles[3];
extern float g_demozoom;

extern float gNormMouseX;
extern float gNormMouseY;

extern int gVisibleMouse;
extern vec3_t v_angles;
extern vec3_t v_origin;

#endif