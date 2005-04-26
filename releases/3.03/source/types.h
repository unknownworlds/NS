//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: types.h $
// $Date: 2002/07/25 18:05:28 $
//
//-------------------------------------------------------------------------------
// $Log: types.h,v $
// Revision 1.10  2002/07/25 18:05:28  flayra
// - Remove unneeded type for Linux compatibility
//
// Revision 1.9  2002/05/23 02:41:53  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef BASIC_H
#define BASIC_H

#pragma warning (disable: 4786)

// #ifdef this the right way for Linux
//#include <crtdbg.h>
//#define ASSERT _ASSERT
#include "localassert.h"
#include "build.h"

#include <string>
#include <vector>
#include <map>
#include <iostream>
#include <fstream>
#include <algorithm>

using namespace std;

// data types
typedef signed char			int8;
typedef signed short		int16;
typedef signed long			int32;
							
typedef unsigned char		uint8;
typedef unsigned short		uint16;
typedef unsigned long		uint32;

#ifndef max
#define max(a, b)  (((a) > (b)) ? (a) : (b))
#endif

#ifndef min
#define min(a, b)  (((a) < (b)) ? (a) : (b))
#endif

typedef vector<string>					StringList;
//typedef vector<pair<string, string>>	StringPairList;

#endif
