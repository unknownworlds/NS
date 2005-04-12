#include "AvHConstants.h"
#include <cstring>

const int slashchr =  '\\';
#define kAvHModDir					((const char*)("ns"))

const char* getModDirectory(void)
{
//#ifdef __linux__
//	return kAvHModDir;
//#else
//	static char theModDirectory[512];
//	static bool theIsComputed = false;
//	#define thisFileString __FILE__

//	if(!theIsComputed)
//	{
//		const char* thisFileName = thisFileString;
//		strcpy(theModDirectory,thisFileName);
//		//theModDirectory = <blah>/[mod_directory]/source/mod/AvHConstants.cpp
//		char* pos = strrchr(theModDirectory,slashchr);
//		for(int counter = 0; counter < 3; ++counter) //remove three slahses and everything that comes after
//		{
//			*pos = '\0';
//			pos = strrchr(theModDirectory,slashchr); //point to next slash
//		}
		//theModDirectory = <blah>/[mod_directory]
		//pos+1 = [mod_directory]
//		char temp[512];
//		strcpy(temp,pos+1); //use temp so we don't overwrite ourselves on copy.
//		strcpy(theModDirectory,temp); //theModDirectory now holds the correct directory name
//		theIsComputed = true; //don't compute this again
//	}
//	return theModDirectory;
//#endif
	return "ns";
}

const char* getModName(void)
{
	return kAvHGameName;
}
