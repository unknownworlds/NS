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
#ifndef AVH_SPAWN_H
#define AVH_SPAWN_H

#include "util/nowarnings.h"
#include "extdll.h"
#include "util.h"
#include "mod/AvHConstants.h"
#include <vector>

class AvHSpawn
{
public:
	AvHSpawn(const string& inClassName, const Vector& inOrigin, const Vector& inAngles, const AvHTeamNumber inTeamNumber);

	string				GetClassName() const;
	Vector				GetOrigin() const;
	Vector				GetAngles() const;
	AvHTeamNumber		GetTeamNumber() const;

private:
	string				mClassName;
	Vector				mOrigin;
	Vector				mAngles;
	AvHTeamNumber		mTeamNumber;
};

typedef vector<AvHSpawn>	SpawnListType;

#endif