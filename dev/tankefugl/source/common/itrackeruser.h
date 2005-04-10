//========= Copyright © 1996-2001, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef ITRACKERUSER_H
#define ITRACKERUSER_H
#ifdef _WIN32
#pragma once
#endif

#include "interface.h"

//-----------------------------------------------------------------------------
// Purpose: Interface to accessing information about tracker users
//-----------------------------------------------------------------------------
class ITrackerUser : public IBaseInterface
{
public:
	// returns true if the interface is ready for use
	virtual bool IsValid() = 0;

	// returns the tracker ID of the current user
	virtual int GetTrackerID() = 0;

	// returns information about a user
	// information may not be known about some users, "" will be returned
	virtual const char *GetUserName(int trackerID) = 0;
	virtual const char *GetFirstName(int trackerID) = 0;
	virtual const char *GetLastName(int trackerID) = 0;
	virtual const char *GetEmail(int trackerID) = 0;

	// returns true if friendID is a friend of the current user 
	// ie. the current is authorized to see when the friend is online
	virtual bool IsFriend(int friendID) = 0;

	// requests authorization from a user
	virtual void RequestAuthorizationFromUser(int potentialFriendID) = 0;
};

#define TRACKERUSER_INTERFACE_VERSION "TrackerUser001"


#endif // ITRACKERUSER_H
