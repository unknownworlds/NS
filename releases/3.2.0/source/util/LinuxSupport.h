#ifdef LINUX

#ifndef LINUX_SUPPORT_H
#define LINUX_SUPPORT_H


// Code from VALVe for cross-platform FindFirst/FindNext
//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose:
// 
// $NoKeywords: $
//=============================================================================
#include <ctype.h> // tolower()
#include <limits.h> // PATH_MAX define 
#include <string.h> //strcmp, strcpy

#include <sys/stat.h> // stat()
#include <unistd.h> 
#include <dirent.h> // scandir()
#include <stdlib.h>
#include <stdio.h>

#define FILE_ATTRIBUTE_DIRECTORY S_IFDIR

typedef struct 
{
	// public data
	int dwFileAttributes;
	char cFileName[PATH_MAX]; // the file name returned from the call
	
	int numMatches;
	struct dirent **namelist;  
} FIND_DATA;

int strlwr(char *src);
int FindFirstFile(const char *findName, FIND_DATA *dat);
bool FindNextFile(int handle, FIND_DATA *dat);
bool FindClose(int handle);

#endif

#endif
