//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAmbientSound.cpp $
// $Date: 2002/07/10 14:38:50 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAmbientSound.cpp,v $
// Revision 1.5  2002/07/10 14:38:50  Flayra
// - Fixed bug where sound volume wasn't being set for omnipresent sounds, added document headers
//
//===============================================================================
#include "mod/AvHAmbientSound.h"
#include "cl_dll/hud.h"
#include "common/vec_op.h"
#include <stdlib.h>
#pragma warning(push)
#pragma warning(disable: 311)
#include <fmoddyn.h>
#pragma warning(pop)
#include "cl_dll/cl_util.h"

AvHAmbientSound::AvHAmbientSound(const string& inRelativeSoundName, int inVolume, int inFadeDistance, bool inLooping, const Vector& inPosition, int inEntIndex, float inTimeElapsed)
{
	this->mInitialized = false;
	this->mStream = NULL;
	this->mChannel = 0;
	this->mVolume = inVolume;
	this->mLooping = inLooping;
	this->mFadeDistance = inFadeDistance;
	VectorCopy(inPosition, this->mPosition);
	this->mEntityIndex = inEntIndex;
	this->mTimeElapsed = inTimeElapsed;
	
	// Start playing the song
	string theSoundName = string(getModDirectory()) + "/" + string(inRelativeSoundName);
	this->mSoundName = theSoundName;
}

AvHAmbientSound::AvHAmbientSound(const AvHAmbientSound& inSource)
{
	// Don't re-init, just copy values
	this->mInitialized = inSource.mInitialized;
	this->mStream = inSource.mStream;
	this->mChannel = inSource.mChannel;
	this->mVolume = inSource.mVolume;
	this->mLooping = inSource.mLooping;
	this->mSoundName = inSource.mSoundName;
	this->mFadeDistance = inSource.mFadeDistance;
	VectorCopy(inSource.mPosition, this->mPosition);
	this->mEntityIndex = inSource.mEntityIndex;
}

void AvHAmbientSound::ClearData()
{
	if(this->mInitialized)
	{
		gHUD.ClearStream(this->mStream);
	}
}

int	AvHAmbientSound::GetEntityIndex() const
{
	return this->mEntityIndex;
}

void AvHAmbientSound::SetPosition(const Vector& inPosition)
{
	VectorCopy(inPosition, this->mPosition);
}

void AvHAmbientSound::StartPlayingIfNot()
{
	if(!this->mInitialized)
	{
		int theBytesInSong;
		this->mInitialized = gHUD.PlaySong(this->mSoundName, this->mVolume, this->mLooping, this->mStream, this->mChannel, theBytesInSong, this->mTimeElapsed);
	}
}

void AvHAmbientSound::UpdateVolume(const Vector& inListenerPosition)
{
	if(this->mInitialized)
	{
		Vector theDistanceVector = inListenerPosition - this->mPosition;
		float theDistance = sqrt(theDistanceVector[0]*theDistanceVector[0] + theDistanceVector[1]*theDistanceVector[1] + theDistanceVector[2]*theDistanceVector[2]);

		//FSOUND_SetPan(this->mChannel, FSOUND_STEREOPAN);
		int theVolume = this->mVolume;

		if(this->mFadeDistance > 0)
		{
			theVolume = this->mVolume - this->mVolume*(theDistance/(float)this->mFadeDistance);
		}

		theVolume = min(max(0, theVolume), 255);

        FMOD_INSTANCE* theFMOD = gHUD.GetFMOD();

        if (theFMOD)
        {
    		theFMOD->FSOUND_SetVolume(this->mChannel, theVolume);
        }

	}
}
