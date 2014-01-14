//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHSoundListManager.cpp $
// $Date: 2002/11/22 21:28:17 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSoundListManager.cpp,v $
// Revision 1.10  2002/11/22 21:28:17  Flayra
// - mp_consistency changes
//
// Revision 1.9  2002/09/09 20:06:56  Flayra
// - Added custom attention parameter
//
// Revision 1.8  2002/07/26 01:51:57  Flayra
// - Linux support for FindFirst/FindNext
//
// Revision 1.7  2002/07/25 16:58:00  flayra
// - Linux changes
//
// Revision 1.6  2002/07/01 21:48:10  Flayra
// - Added debug code, added document headers
//
//===============================================================================
#include "mod/AvHSoundListManager.h"
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "mod/AvHConstants.h"
#include "util/STLUtil.h"

#define kMaxNumberLists 200

AvHSoundListManager::AvHSoundListManager()
{
	// Needed so strings are allocated and moved around after being precached
	this->mSoundList.resize(kMaxNumberLists);
}

bool AvHSoundListManager::BuildSoundList(const string& inDirName, CStringList& outList)
{
	bool theSuccess = false;

	string theBaseDirectoryName = string(getModDirectory()) + string("/") + string(kSoundDirectory) + string("/");
	string theFullDirName = inDirName;
	if(BuildFileList(theBaseDirectoryName, theFullDirName, "*.wav", outList))
	{
	    theSuccess = true;
	}
	
	return theSuccess;
}

void AvHSoundListManager::Clear()
{
	this->mSoundList.clear();
}

SoundListType::iterator	AvHSoundListManager::GetSoundList(const string& inKey)
{
	// TODO: lower case inDirName and save as theKey
	string theKey = inKey;
	
	// Check if we have a sound list with this name
	SoundListType::iterator	theIterator;
	for(theIterator = this->mSoundList.begin(); theIterator != this->mSoundList.end(); theIterator++)
	{
		if(theIterator->first == theKey)
		{
			break;
		}
	}

	return theIterator;
}

bool AvHSoundListManager::PrecacheSoundList(const string& inDirName)
{
	bool theReturnValue = false;

	// Check if we have a sound list with this name
	SoundListType::iterator theIter = this->GetSoundList(inDirName);

	// If so, return false
	if(theIter == this->mSoundList.end())
	{
		// If not, read list of all sound files in this directory, add this to the list with theKey as a key
		CStringList theNewSoundList;
		if(this->BuildSoundList(inDirName, theNewSoundList))
		{
			// Save entry
			this->mSoundList.push_back(make_pair(inDirName, theNewSoundList));

			// NOW, precache each sound because it remembers the pointer, so it has to be the final version <sigh>
			SoundListType::iterator theIter = this->GetSoundList(inDirName);
			ASSERT(theIter != this->mSoundList.end());
			
			// Get number of entries
			CStringList& theList = theIter->second;
			int theNumEntries = theList.size();
			for(int i = 0; i < theNumEntries; i++)
			{
				CString& theSoundToPrecache = theIter->second[i];

				int iString = ALLOC_STRING((char*)theSoundToPrecache);//: We cant do "(char*)theSoundToPrecache" directly cause it causes some wierd problems.
				PRECACHE_UNMODIFIED_SOUND((char*)STRING(iString));

				// Success
				theReturnValue = true;
			}
		}
		else
		{
			int a = 0;
		}
	}
	return theReturnValue;
}

bool AvHSoundListManager::PlaySoundInList(const string& inDirName, CBaseEntity* inEntity, int inChannel, float inVolume, float inAttenuation)
{
	bool theReturnValue = false;

	// Check if we have a sound list with this name
	SoundListType::iterator theIter = this->GetSoundList(inDirName);

	// If not, return false
	if(theIter != this->mSoundList.end())
	{
		// Get number of entries
		CStringList& theList = theIter->second;
		int theNumEntries = theList.size();
		if(theNumEntries > 0)
		{
			// Pick one at random
			int theOneToPlay = RANDOM_LONG(0, theNumEntries - 1);
			
			// Play it!
			CString& theSoundToPlay = theIter->second[theOneToPlay];
			char* theCStrToPlay = (char*)theSoundToPlay;
			if(theCStrToPlay)
			{
				//UTIL_LogPrintf("AvHSoundListManager::Playing sound: %s\n", theCStrToPlay);

				EMIT_SOUND(ENT(inEntity->pev), inChannel, theCStrToPlay, inVolume, inAttenuation);
				//int theRandomChannel = RANDOM_LONG(0, 6);
				//EMIT_SOUND_DYN(ENT(inEntity->pev), , (char*)theSoundToPlay, 1.0, ATTN_NORM, 0, 95);
				
				theReturnValue = true;
			}
			else
			{
				int a = 0;
			}
		}
	}

	return theReturnValue;
}

