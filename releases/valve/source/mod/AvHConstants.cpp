#include "AvHConstants.h"
#include <cstring>

const int slashchr =  '\\';
#define kAvHModDir					((const char*)("ns"))

#ifdef AVH_SERVER
#include "common/mathlib.h"
#include "common/const.h"
#include "engine/edict.h"
#include "engine/eiface.h"
#include "dlls/enginecallback.h"
#else
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#include "dlls/extdll.h"
#endif


const char* getModDirectory(void)
{
	static char theModDirectory[512];
	static bool theIsComputed = false;

	if(!theIsComputed)
	{
		strcpy(theModDirectory, "ns305");
#ifdef AVH_SERVER
		GET_GAME_DIR(theModDirectory);
#else
		const char *pchGameDirT = gEngfuncs.pfnGetGameDirectory();
		strcpy(theModDirectory, pchGameDirT);
#endif

		theIsComputed = true; //don't compute this again
	}
	return theModDirectory;
}

const char* getModName(void)
{
	return kAvHGameName;
}
