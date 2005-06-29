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
#ifndef AVH_ALERT_H
#define AVH_ALERT_H

#include "types.h"
#include "mod/AvHConstants.h"

// Lookup: entity index -> time of alert, type of alert
class AvHAlert 
{
public:
					AvHAlert() : mAlertType(ALERT_NONE), mEntityIndex(-1), mAlertTime(-1), mPlayedAudio(false) {}

					AvHAlert(AvHAlertType inAlertType, float inTime) : mAlertType(inAlertType), mAlertTime(inTime)
					{
						mEntityIndex = -1;
						mPlayedAudio = false;
					}

					AvHAlert(AvHAlertType inAlertType, float inTime, EntityInfo inEntityIndex) : mAlertType(inAlertType), mAlertTime(inTime), mEntityIndex(inEntityIndex)
					{
						mPlayedAudio = false;
					}

	AvHAlertType	GetAlertType() const { return this->mAlertType; }
	EntityInfo		GetEntityIndex() const {	return this->mEntityIndex; }
	float			GetTime() const { return this->mAlertTime; }
	bool			GetPlayedAudio() const { return this->mPlayedAudio; }
	void			SetPlayedAudio(bool inPlayedAudio) { this->mPlayedAudio = inPlayedAudio; }
	
private:
	AvHAlertType	mAlertType;
	EntityInfo		mEntityIndex;
	float			mAlertTime;
	bool			mPlayedAudio;
};

typedef	vector<AvHAlert> AlertListType;

#endif