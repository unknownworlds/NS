//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Utility functions loosely wrapping the STL
//
// $Workfile: STLUtil.cpp$
// $Date: 2002/07/26 21:43:04 $
//
//-------------------------------------------------------------------------------
// $Log: STLUtil.cpp,v $
// Revision 1.6  2002/07/26 21:43:04  flayra
// - Updates to get particles and sounds working under Linux
//
// Revision 1.5  2002/07/26 01:51:23  Flayra
// - Linux support for FindFirst/FindNext
//
// Revision 1.4  2002/05/23 04:03:11  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
// Revision 1.5  2002/05/14 18:48:16  Charlie
// - More utility functions
//
// Revision 1.4  2002/05/01 00:52:34  Charlie
// - Added BuildAbridgedString
//
//===============================================================================
#include "util/STLUtil.h"
#include <stdio.h>
#include "stdarg.h"


// For FindFirst/FindNext functionality in BuildFileList
#ifdef WIN32
	#include "windows.h"
#else
	#include "util/LinuxSupport.h"
#endif

string LowercaseString(const string& ioString)
{
	string theReturnString = ioString;
	LowercaseString(theReturnString);
	return theReturnString;
}

string LowercaseString(string& ioString)
{
	size_t theLength = ioString.length();
	for(size_t i = 0; i < theLength; i++)
	{
		char theCurrentChar = ioString[i];
		ioString[i] = tolower(theCurrentChar);
	}
	return ioString;
}

string UppercaseString(const string& ioString)
{
    string theReturnString = ioString;
    UppercaseString(theReturnString);
    return theReturnString;
}

string UppercaseString(string& ioString)
{
    size_t theLength = ioString.length();
    for(size_t i = 0; i < theLength; i++)
    {
        char theCurrentChar = ioString[i];
        ioString[i] = toupper(theCurrentChar);
    }
    return ioString;
}

int MakeIntFromString(string& inString)
{
	int theInt = 0;
	if(inString.length() > 0)
	{
		sscanf(inString.c_str(), "%d", &theInt);
	}
	return theInt;
}

float MakeFloatFromString(string& inString)
{
	float theFloat = 0;
	if(inString.length() > 0)
	{
		sscanf(inString.c_str(), "%f", &theFloat);
	}

	return theFloat;
}

bool MakeBytesFromHexPairs(const string& inHex, unsigned char* ioBytes, int numBytes)
{
	bool theSuccess = true;
	if(inHex.size() != numBytes*2)
	{ 
		theSuccess = false; 
	}
	else
	{
		for(int index = 0; index < numBytes; ++index)
		{
			int theByte = 0;
			for(int strpos = index*2; strpos < (index+1)*2; ++strpos)
			{
				theByte *= 16;
				switch(inHex[strpos])
				{
				case '0': case '1': case '2': case '3': case '4': 
				case '5': case '6': case '7': case '8': case '9': 
					theByte += inHex[strpos] - '0';
					break;
				case 'A': case 'B': case 'C':
				case 'D': case 'E': case 'F':
					theByte += inHex[strpos] - 'A' + 10;
					break;
				default:
					theSuccess = false;
					break;
				}
			}
			if(theSuccess)
			{
				ioBytes[index] = theByte;
			}
			else
			{ 
				break; 
			}
		}
	}
	return theSuccess;
}

string MakeStringFromInt(int inNumber)
{
	char theBuffer[kMaxStrLen];
	sprintf(theBuffer, "%d", inNumber);
	return string(theBuffer);
}

string MakeStringFromFloat(float inNumber, int inNumDecimals)
{
	// Build format specfier like "%.2f" (where 2 is inNumDecimals)
	string theFormatSpecfier("%.");
	theFormatSpecfier += MakeStringFromInt(inNumDecimals);
	theFormatSpecfier += string("f");

	char theBuffer[kMaxStrLen];
	sprintf(theBuffer, theFormatSpecfier.c_str(), inNumber);

	return string(theBuffer);
}

bool MakeHexPairsFromBytes(const unsigned char* inBytes, string& ioHex, int numBytes)
{
	bool theSuccess = true;
	string theString;
	int theNumber;
	for(int index = 0; index < numBytes*2; ++index)
	{
		theNumber = inBytes[index/2];
		if(index % 2)
		{
			theNumber %= 16;
		}
		else
		{
			theNumber /= 16;
		}
		if(theNumber > 15 || theNumber < 0)
		{ 
			theSuccess = false; 
			break;
		}
		switch(theNumber)
		{
		case 0: case 1: case 2: case 3: case 4:
		case 5: case 6: case 7: case 8: case 9:
			theString += ('0' + theNumber);
			break;
		case 10: case 11: case 12:
		case 13: case 14: case 15:
			theString += ('A' + theNumber - 10);
			break;
		}
	}
	if(theSuccess)
	{
		ioHex.assign(theString);
	}
	return theSuccess;
}


string BuildAbridgedString(const string& inString, int inMaxLen)
{
	ASSERT(inMaxLen > 0);

	string theAbridgedString = inString;
	
	// If inString is longer then inMaxLen
	if(inString.length() > (unsigned int)inMaxLen)
	{
		// For short max strings, just truncate past inMaxLen (it would be ridiculous to have more then half the string be "...")
		if(inMaxLen <= 7)
		{
			theAbridgedString = inString.substr(0, inMaxLen-1);
		}
		// else
		else
		{
			// Cut off all characters pass inMaxLen - 3
			theAbridgedString = inString.substr(0, inMaxLen-4);

			// Tack on "..."
			theAbridgedString += "...";
		}
	}

	return theAbridgedString;
}

static char *ignoreSounds[] = {
	"vox/ssay82.wav",
	"vox/ssay83.wav",
	0
};

// Pass in relative path, do search on path including mod directory, return files relative to mod directory
bool BuildFileList(const string& inBaseDirectoryName, const string& inDirectoryName, const string& inFileExtension, CStringList& outList)
{
	#ifdef WIN32
	const string kDelimiter("\\");
	#else
	const string kDelimiter("/");
	#endif

	bool theSuccess = false;

	string theBaseDirectoryName = inBaseDirectoryName;
	string theDirectoryName = inDirectoryName;

	#ifdef WIN32
	// Replace all forward slashes with \\'s if needed
	std::replace(theBaseDirectoryName.begin(), theBaseDirectoryName.end(), '/', '\\');
	std::replace(theDirectoryName.begin(), theDirectoryName.end(), '/', '\\');
	#endif
	
	string theFullDirName = theBaseDirectoryName + theDirectoryName;
	
	size_t theEndOffset = theDirectoryName.find_last_of(kDelimiter);
	string theBaseDirName = theDirectoryName.substr(0, theEndOffset);

	theFullDirName += inFileExtension;

	#ifdef WIN32
	WIN32_FIND_DATA		theFindData;
	HANDLE				theFileHandle;
	
	theFileHandle = FindFirstFile(theFullDirName.c_str(), &theFindData);
	if (theFileHandle != INVALID_HANDLE_VALUE)
	{
		do
		{
			string theFoundFilename = string(theFindData.cFileName);


	#else

	string theFoundFilename;
	FIND_DATA theFindData;

	const char* theFullDirNameCStr = theFullDirName.c_str();
	int theRC = FindFirstFile(theFullDirNameCStr, &theFindData);
	if(theRC != -1)
	{
		do
		{
			string theFoundFilename = string(theFindData.cFileName);
	#endif
			CString theCString;
			string theFullFileName = theBaseDirName + string("/") + theFoundFilename;

			// Precache requires / in the filename
			std::replace(theFullFileName.begin(), theFullFileName.end(), '\\', '/');

			theCString = theFullFileName;
#ifdef AVH_SERVER
			// <HACK> Ignore ssay82 and ssay83 till we can client patch this ..
			int i=0;
			bool found = false;
			while ( ignoreSounds[i] != 0 ) {
				if ( !strcmp(ignoreSounds[i], theCString))
				{
					found = true;
				}
				i++;
			}
			if (found == false)
			{
#endif
				outList.push_back(theCString);
				theSuccess = true;
#ifdef AVH_SERVER
			}
			// </HACK>
#endif

	#ifdef WIN32
		} 
		while(FindNextFile(theFileHandle, &theFindData));
	}
	#else
		}
		while(FindNextFile(0, &theFindData));
	}
	#endif

	//DIR theDirp = opendir(theDirName.c_str());
	//	while(theDirp) 
	//	{
	//		int theErrno = 0;
	//		     if ((dp = readdir(theDirp)) != NULL) {
	//			         if (strcmp(dp->d_name, name) == 0) {
	//				             closedir(theDirp);
	//				             return FOUND;
	//				         }
	//			     } else {
	//				         if (theErrno == 0) {
	//					             closedir(theDirp);
	//					             return NOT_FOUND;
	//					         }
	//				         closedir(theDirp);
	//				         return READ_ERROR;
	//				     }
	//			 }
	//	return OPEN_ERROR;

	return theSuccess;
}

int32 sprintf(string& outString, const char* inPattern, ...)
{
	va_list theParameters;
	int32 theResult;
	
	va_start(theParameters, inPattern);
	
	char theCharArray[kMaxStrLen];
	
	theResult = ::vsprintf(theCharArray, inPattern, theParameters);
	outString = theCharArray;
	
	va_end(theParameters);
	
	return theResult;
}

int SafeStrcmp(const char* inStringOne, const char* inStringTwo)
{
	int theReturnValue = -1;

	if(inStringOne && inStringTwo)
	{
		theReturnValue = strcmp(inStringOne, inStringTwo);
	}

	return theReturnValue;
}

// Remove leading and trailing spaces.
// Remove trailing \r\n
void TrimString(string& ioString)
{
	int theStartChopIndex = 0;
	int theEndChopIndex = (int)ioString.length();

	// Now remove any leading spaces
	while((theStartChopIndex < (signed)ioString.length()) && (ioString[theStartChopIndex] == ' '))
	{
		theStartChopIndex++;
	}
	
	// Remove trailing newlines, carriage returns, or spaces
	while((theEndChopIndex > 0) && ((ioString[theEndChopIndex-1] == '\r') || (ioString[theEndChopIndex-1] == '\n') || (ioString[theEndChopIndex-1] == ' ')))
	{
		theEndChopIndex--;
	}

	int theLength = (int)(theEndChopIndex - theStartChopIndex);
	if(theLength > 0)
	{
		string theTrimmedString = ioString.substr(theStartChopIndex, theLength);
		ioString = theTrimmedString;
	}
}
