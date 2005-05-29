//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHSoundListManager.h $
// $Date: 2002/09/09 20:06:56 $
//
//-------------------------------------------------------------------------------
// $Log: AvHSoundListManager.h,v $
// Revision 1.5  2002/09/09 20:06:56  Flayra
// - Added custom attention parameter
//
// Revision 1.4  2002/07/26 01:51:57  Flayra
// - Linux support for FindFirst/FindNext
//
// Revision 1.3  2002/07/01 21:48:11  Flayra
// - Added debug code, added document headers
//
//===============================================================================
#ifndef AVHSOUNDLISTMANAGER_H
#define AVHSOUNDLISTMANAGER_H

#include "types.h"
#include "util/CString.h"
class CBaseEntity;

typedef vector< pair<string, CStringList> >	SoundListType;

class AvHSoundListManager
{
public:
	AvHSoundListManager();

	void Clear();

	bool PrecacheSoundList(const string& inDirName);

	// Channel is CHAN_AUTO just didn't want to include the world
	bool PlaySoundInList(const string& inDirName, CBaseEntity* inEntity, int inChannel = 0, float inVolume = 1.0f, float inAttenuation = .8f);

private:
	bool BuildSoundList(const string& inDirName, CStringList& outList);

	SoundListType::iterator		GetSoundList(const string& inKey);

	SoundListType				mSoundList;
};

#endif