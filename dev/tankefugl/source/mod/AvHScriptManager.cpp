//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: Contains all script bindings for testing functionality
//
// $Workfile: AvHAvHScriptManager.cpp $
// $Date: 2002/11/22 21:24:27 $
//
//-------------------------------------------------------------------------------
// $Log: AvHScriptManager.cpp,v $
// Revision 1.3  2002/11/22 21:24:27  Flayra
// - Changed AVH_DEVELOPER_BUILD to DEBUG
//
// Revision 1.2  2002/06/25 18:15:52  Flayra
// - Some enhancements and bugfixes for tutorial
//
// Revision 1.1  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "util/nowarnings.h"
#include "mod/AvHScriptManager.h"
#include "util/Checksum.h"
#include "util/STLUtil.h"
#include "mod/AvHConstants.h"

extern "C" {
	#include <lua.h>
	#include <lualib.h>
	#include <lauxlib.h>
}

AvHScriptInstance* gRunningScript = NULL;

AvHScriptInstance::AvHScriptInstance(string inScriptName)
{
	this->mState = NULL;
	
	this->Init();

	// Assumes that filenames are relative to the ns/scripts directory
	//this->mScriptName = kModDirectory + string("/") + kScriptsDirectory + string("/") + inScriptName;
	this->mScriptName = inScriptName;
}

void AvHScriptInstance::AddCallback(string& inCallbackName, float inTime)
{
	CallbackType theCallback(inCallbackName, inTime);

	this->mCallbacksQueuedForAdd.push_back(theCallback);
}

bool AvHScriptInstance::CallbacksPending() const
{
	bool theCallbacksPending = false;

	if((this->mCallbackList.size() > 0) || (this->mCallbacksQueuedForAdd.size() > 0))
	{
		theCallbacksPending = true;
	}

	return theCallbacksPending;
}

void AvHScriptInstance::CallSimpleFunction(const string& inFunctionName)
{
	gRunningScript = this;
	
	// Execute callback
	lua_getglobal(this->mState, inFunctionName.c_str());
	//lua_pushstring(this->mState, inFunctionName.c_str());
	lua_call(this->mState, 0, 0);
	
	gRunningScript = NULL;
}

void AvHScriptInstance::Cleanup()
{
	ASSERT(!this->CallbacksPending());
	ASSERT(this->mState);

	lua_close(this->mState);
}

lua_State* AvHScriptInstance::GetState()
{
	return this->mState;
}

void AvHScriptInstance::Init()
{
	this->mState = lua_open();
	
	lua_baselibopen(this->mState);
	lua_strlibopen(this->mState);
	lua_mathlibopen(this->mState);
	//lua_iolibopen(this->mState);

	this->InitShared();

	#ifdef AVH_SERVER
	this->InitServer();
	#else
	this->InitClient();
	#endif
}

void AvHScriptInstance::Reset()
{
	// Delete callbacks
	this->mCallbackList.clear();
	this->mCallbacksQueuedForAdd.clear();
		
	// Cleanup
	this->Cleanup();
}

void AvHScriptInstance::Run()
{
	ASSERT(this->mState);
	ASSERT(this->mScriptName != "");

	// Set global current script so it's accessible statically (needed for setting callbacks)
	gRunningScript = this;

	lua_dofile(this->mState, this->mScriptName.c_str());

	gRunningScript = NULL;
}


void AvHScriptInstance::Update(float inTime)
{
	// First add any queued callback onto our list
	for(CallbackListType::iterator theQueuedIter = this->mCallbacksQueuedForAdd.begin(); theQueuedIter != this->mCallbacksQueuedForAdd.end(); theQueuedIter++)
	{
		this->mCallbackList.push_back(*theQueuedIter);
	}

	// Clear queued list
	this->mCallbacksQueuedForAdd.clear();

	// Update callbacks
	for(CallbackListType::iterator theIter = this->mCallbackList.begin(); theIter != this->mCallbackList.end(); /* no increment*/)
	{
		CallbackType& theCallback = *theIter;
		if(inTime >= theCallback.second)
		{
			this->CallSimpleFunction(theCallback.first.c_str());

			// Remove callback from list 
			theIter = this->mCallbackList.erase(theIter);
		}
		else
		{
			theIter++;
		}
	}
}



AvHScriptManager* AvHScriptManager::sSingleton = NULL;

AvHScriptManager* AvHScriptManager::Instance()
{
	if(!sSingleton)
	{
		sSingleton = new AvHScriptManager();
	}
	
	ASSERT(sSingleton);
	
	return sSingleton;
}

void AvHScriptManager::Reset()
{
	for(AvHScriptInstanceListType::iterator theIter = this->mScriptList.begin(); theIter != this->mScriptList.end(); theIter++)
	{
		theIter->Reset();
	}
	this->mScriptList.clear();
}

void AvHScriptManager::RunScript(const string& inScriptName)
{
	// Create new AvHScriptInstance, and add it
	AvHScriptInstance theAvHScriptInstance(inScriptName);
	
	// Run it
	theAvHScriptInstance.Run();

	bool theIsRunningOnClient = false;

	#ifdef AVH_CLIENT
	theIsRunningOnClient = true;
	#endif

	// If it's still running, add it to the list
	if(theAvHScriptInstance.CallbacksPending() || theIsRunningOnClient)
	{
		this->mScriptList.push_back(theAvHScriptInstance);
	}
}

void AvHScriptManager::Update(float inTime)
{
	// Run through list of scripts
	for(AvHScriptInstanceListType::iterator theIter = this->mScriptList.begin(); theIter != this->mScriptList.end(); /* no increment */)
	{
		// If callback is pending
		if(theIter->CallbacksPending())
		{
			// Is it time to run any of our callbacks?
			theIter->Update(inTime);
			
			// Always increment
			theIter++;
		}
		// else
		else
		{
			// Remove it from the list
			theIter = this->mScriptList.erase(theIter);
		}
	}
}


#ifdef AVH_CLIENT
void AvHScriptManager::ClientUpdate(float inTimePassed)
{
	// For all scripts
	for(AvHScriptInstanceListType::iterator theIter = this->mScriptList.begin(); theIter != this->mScriptList.end(); /* no increment */)
	{
		// Call "clientUpdate(inTimePassed)" function (push function then args)
		lua_getglobal(theIter->GetState(), "clientUpdate");

		// Push time passed
		lua_pushnumber(theIter->GetState(), inTimePassed);

		// Push num args and num return
		lua_call(theIter->GetState(), 1, 1);
		
		// If function returns false, delete it
		bool theKeepRunning = true;
		int theNumReturned = lua_gettop(theIter->GetState());
		if(theNumReturned > 0)
		{
			string theString = lua_tostring(theIter->GetState(), 1);
			theKeepRunning = lua_tonumber(theIter->GetState(), 1);
			lua_pop(theIter->GetState(), 1);
		}
		
		if(!theKeepRunning)
		{
			theIter->Reset();
			
			theIter = this->mScriptList.erase(theIter);
		}
		else
		{
			// else increment
			theIter++;
		}
	}
}

void AvHScriptManager::DrawNormal()
{
	// For all scripts
	for(AvHScriptInstanceListType::iterator theIter = this->mScriptList.begin(); theIter != this->mScriptList.end(); theIter++)
	{
		theIter->CallSimpleFunction("drawNormal");
	}
}

void AvHScriptManager::DrawTransparent()
{
	// For all scripts
	for(AvHScriptInstanceListType::iterator theIter = this->mScriptList.begin(); theIter != this->mScriptList.end(); theIter++)
	{
		theIter->CallSimpleFunction("drawTransparent");
	}
}

void AvHScriptManager::DrawNoZBuffering()
{
	// For all scripts
	for(AvHScriptInstanceListType::iterator theIter = this->mScriptList.begin(); theIter != this->mScriptList.end(); theIter++)
	{
		theIter->CallSimpleFunction("drawNoZBuffering");
	}
}

bool AvHScriptManager::GetClientMove(int& outButtonBits, int& outImpulse)
{
	bool theSuccess = false;

	#ifdef DEBUG

	// For all scripts
	for(AvHScriptInstanceListType::iterator theIter = this->mScriptList.begin(); theIter != this->mScriptList.end(); theIter++)
	{
		// Call getClientMove() on script.  If it is successful, stop processing.  Only one script can control movement at a time
		const int kNumExpectedReturnValues = 2;

		// Execute callback
		lua_getglobal(theIter->GetState(), "getClientMove");
		lua_call(theIter->GetState(), 0, kNumExpectedReturnValues);

		// Check for success
		int theNumReturned = lua_gettop(theIter->GetState());
		if(theNumReturned >= kNumExpectedReturnValues)
		{
			// Populating move structure if possible
			outButtonBits = (int)lua_tonumber(theIter->GetState(), 1);
			outImpulse = (int)lua_tonumber(theIter->GetState(), 2);

			lua_pop(theIter->GetState(), kNumExpectedReturnValues);

			theSuccess = true;
		}

		if(theSuccess)
		{
			break;
		}
	}

	#endif

	return theSuccess;
}

#endif	

