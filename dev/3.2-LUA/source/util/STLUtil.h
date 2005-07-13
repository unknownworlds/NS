//======== (C) Copyright 2001 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Utility functions loosely wrapping the STL
//
// $Workfile: STLUtil.h$
// $Date: 2002/07/26 01:51:23 $
//
//-------------------------------------------------------------------------------
// $Log: STLUtil.h,v $
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

#ifndef STL_UTIL_H
#define STL_UTIL_H

#include "util/nowarnings.h"
#include "util/CString.h"
#include "types.h"

const int kMaxStrLen = 2056;

string LowercaseString(const string& ioString);
string LowercaseString(string& ioString);
string UppercaseString(const string& ioString);
string UppercaseString(string& ioString);
int MakeIntFromString(string& inString);
float MakeFloatFromString(string& inString);
string MakeStringFromInt(int inNumber);
string MakeStringFromFloat(float inNumber, int inNumDecimals = 1);
string BuildAbridgedString(const string& inString, int inMaxLen);
bool BuildFileList(const string& inBaseDirectoryName, const string& inDirectoryName, const string& inFileExtension, CStringList& outList);
int SafeStrcmp(const char* inStringOne, const char* inStringTwo);
void TrimString(string& ioString);

bool MakeBytesFromHexPairs(const string& inHex, unsigned char* ioBytes, int numBytes);
bool MakeHexPairsFromBytes(const unsigned char* inBytes, string& ioHex, int numBytes);

extern int32 sprintf(string& inOutput, const char* inPattern, ...);

#endif