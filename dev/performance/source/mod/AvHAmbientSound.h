//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHAmbientSound.h $
// $Date: 2002/07/10 14:38:51 $
//
//-------------------------------------------------------------------------------
// $Log: AvHAmbientSound.h,v $
// Revision 1.5  2002/07/10 14:38:51  Flayra
// - Fixed bug where sound volume wasn't being set for omnipresent sounds, added document headers
//
//===============================================================================
#ifndef AVH_AMBIENT_SOUND_H
#define AVH_AMBIENT_SOUND_H

#include "types.h"
#include "fmod.h"
#include "ui/UIHud.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHSharedTypes.h"
#include "mod/AvHConstants.h"
#include "mod/AvHParticleSystem.h"
#include "common/entity_state.h"
#include "VGUI_ProgressBar.h"
#include "ui/MarqueeComponent.h"
#include "mod/AvHOrder.h"
#include "mod/AvHMessage.h"

class AvHAmbientSound
{
public:
						AvHAmbientSound(const string& inRelativeSoundName, int inVolume, int inFadeDistance, bool inLooping, const Vector& inPosition, int inEntIndex, float inTimeElapsed = -1);
						AvHAmbientSound(const AvHAmbientSound& inSource);

	void				ClearData();

	int					GetEntityIndex() const;

	void				SetPosition(const Vector& inPosition);

	void				StartPlayingIfNot();

	void				UpdateVolume(const Vector& inListenerPosition);

private:
	// Changing or adding data?  Don't forget to put it in the copy constructor!
	FSOUND_STREAM*		mStream;
	int					mChannel;
	bool				mInitialized;

	string				mSoundName;
	int					mVolume;
	int					mFadeDistance;
	bool				mLooping;
	Vector				mPosition;
	int					mEntityIndex;
	float				mTimeElapsed;
	
};

#endif