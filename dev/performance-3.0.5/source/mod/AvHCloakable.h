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
#ifndef AVH_CLOAKABLE_H
#define AVH_CLOAKABLE_H

class AvHCloakable
{
public:
						AvHCloakable();

	virtual bool		GetCanCloak() const;
	virtual float		GetCloakTime() const;
	virtual bool		GetIsCloaked() const;
	bool				GetIsPartiallyCloaked() const;
	virtual float		GetUncloakTime() const;
	virtual float		GetOpacity() const;
	void				SetSpeeds(float inCurrentSpeed, float inMaxSpeed, float inMaxWalkSpeed);
	virtual void		Cloak(bool inNoFade = false);
	virtual void		Uncloak(bool inNoFade = false);

	virtual void		ResetCloaking();
	virtual void		Update();

private:
	float				GetTime() const;
	void				Init();
	float				mCurrentSpeed;
	float				mMaxSpeed;
	float				mMaxWalkSpeed;
	float				mTimeOfLastCloak;
	float				mTimeOfLastUncloak;
	float				mOpacity;
	float				mTimeOfLastUpdate;
};

#endif