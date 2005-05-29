//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile:  $
// $Date: $
//
//-------------------------------------------------------------------------------
// $Log: $
//===============================================================================
#ifndef AVH_BALANCE_H
#define AVH_BALANCE_H

// Header to wrap #defines in AvHBalance.txt
#include "build.h"
#include <string>

	// If we're not playtesting, use #defines for variable names
	#ifndef AVH_PLAYTEST_BUILD 
		#include "../Balance.txt"
		#define BALANCE_IVAR(inName) inName
		#define BALANCE_FVAR(inName) inName
	#endif	

	// If we're playtesting, lookup dynamic value of variable
	#ifdef AVH_PLAYTEST_BUILD 
		
		int GetBalanceInt(const string& inName);
		float GetBalanceFloat(const string& inName);

		#define BALANCE_IVAR(inName) GetBalanceInt(#inName)
		#define BALANCE_FVAR(inName) GetBalanceFloat(#inName)
		
	#endif

	#ifdef AVH_PLAYTEST_BUILD
	typedef map<string, int>	BalanceIntListType;
	typedef map<string, float>	BalanceFloatListType;
	#endif

#endif