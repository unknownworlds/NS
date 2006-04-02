//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHSpawn.cpp $
// $Date: $
//
//-------------------------------------------------------------------------------
// $Log: $
//===============================================================================
#include "mod/AvHSpawn.h"

AvHSpawn::AvHSpawn(const string& inClassName, const Vector& inOrigin, const Vector& inAngles, const AvHTeamNumber inTeamNumber) : mClassName(inClassName), mOrigin(inOrigin), mAngles(inAngles), mTeamNumber(inTeamNumber)
{
}

string AvHSpawn::GetClassName() const
{
	return this->mClassName;
}

Vector AvHSpawn::GetOrigin() const
{
	return this->mOrigin;
}

Vector AvHSpawn::GetAngles() const
{
	return this->mAngles;
}

AvHTeamNumber AvHSpawn::GetTeamNumber() const
{
	return this->mTeamNumber;
}

