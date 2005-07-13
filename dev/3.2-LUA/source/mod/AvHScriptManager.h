//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHScriptManager.h $
// $Date: 2002/05/23 02:33:20 $
//
//-------------------------------------------------------------------------------
// $Log: AvHScriptManager.h,v $
// Revision 1.1  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#ifndef AVH_SCRIPTMANAGER_H
#define AVH_SCRIPTMANAGER_H

#include "util/nowarnings.h"
#include "util/Checksum.h"
#include "util/STLUtil.h"

extern "C" struct lua_State;

class AvHScriptInstance
{
public:
				AvHScriptInstance(string inScriptName);

	void		AddCallback(string& inCallbackName, float inTime);

	bool		CallbacksPending() const;

	void		CallSimpleFunction(const string& inFunctionName);

	void		Cleanup();

	lua_State*	GetState();

	void		Reset();
	
	void		Run();

	void		Update(float inTime);

private:
	void		Init();

	void		InitShared();

	#ifdef AVH_SERVER
	void		InitServer();
	#else
	void		InitClient();
	#endif

	// This gets copied around, so make sure elements can be shallow-copied or write a copy constructor
	lua_State*	mState;
	string		mScriptName;

	typedef pair<string, float>			CallbackType;
	typedef vector< CallbackType >		CallbackListType;

	CallbackListType					mCallbackList;
	CallbackListType					mCallbacksQueuedForAdd;
	
};

class AvHScriptManager
{
public:
	static AvHScriptManager*	Instance();

	void					Reset();

	void					RunScript(const string& inScriptName);

	void					Update(float inTime);

	#ifdef AVH_CLIENT
	void					ClientUpdate(float inTimePassed);
	void					DrawNormal();
	void					DrawTransparent();
	void					DrawNoZBuffering();
	bool					GetClientMove(int& outButtonBits, int& outImpulse);
	#endif	

private:
	static AvHScriptManager*			sSingleton;

	typedef vector<AvHScriptInstance>	AvHScriptInstanceListType;
	AvHScriptInstanceListType			mScriptList;

};

#endif