#include "AvHConstants.h"
#include <cstring>

const int slashchr =  '\\';
#define kAvHModDir					((const char*)("nsp"))

const char* getModDirectory(void)
{
	return kAvHModDir;
}

const char* getModName(void)
{
	return kAvHGameName;
}
