//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHTechNodes.h $
// $Date: 2002/09/23 22:36:08 $
//
//-------------------------------------------------------------------------------
// $Log: AvHTechNodes.h,v $
// Revision 1.7  2002/09/23 22:36:08  Flayra
// - Slight refactoring
//
// Revision 1.6  2002/07/08 17:20:32  Flayra
// - Added hooks to support disallowing actions when a building is "busy"
//
//===============================================================================
#ifndef AVHTECHNODE_H
#define AVHTECHNODE_H

#include "types.h"
#include "mod/AvHMessage.h"
#include "mod/AvHTechID.h"

//NOTE (KGP): I've laid the groundwork for AvHTechNode to potentially become an abstract
// interface so that it can be subclassed separately for Client and Server instead of having
// #defines on the client side.  The AvHMessageID is now a static field meant to uniquely
// identify the node within the tree; this greatly simplifies the AvHTechTree class.  This
// has a side effect--assignment from another AvHTechNode doesn't change the AvHMessageID.
// When you want to create a copy of an AvHTechNode, use clone() instead of equality--this
// also eliminates the problem of slicing from the copy.
//
// The ability to subclass AvHTechNode also allows for a playtest configuration subclass
// that automatically self-updates when the relevant fields are updated; this is something
// I think is defintely worth doing for a couple days' work; it would completely replace
// the BalanceChangeListener functions of AvHTechTree below with a system that only changes 
// components as necessary, which is much less CPU intensive.

class AvHTechNode
{
public:
	AvHTechNode(const AvHMessageID inMessageID);
	AvHTechNode(const AvHTechNode& other);
	AvHTechNode(AvHMessageID inMessageID, AvHTechID inID, AvHTechID inPrereq1, AvHTechID inPrereq2, int inCost, int inBuildTime, bool inResearched = false);
	virtual ~AvHTechNode(void);

	virtual AvHTechNode*	clone(void) const;			//deleting this pointer is the responsibility of the caller.
	virtual void			swap(AvHTechNode& other);	//assignment by swap - see Scott Meyers, Effective C++
	AvHMessageID	getMessageID(void) const;
	AvHTechID		getTechID(void) const;
	void			setTechID(const AvHTechID inMessageID);
	AvHTechID		getPrereqTechID1(void) const;
	void			setPrereqTechID1(const AvHTechID inMessageID);
	AvHTechID		getPrereqTechID2(void) const;
	void			setPrereqTechID2(const AvHTechID inMessageID);
	int				getCost(void) const;
	void			setCost(const int inCost);
	int				getBuildTime(void) const;
	void			setBuildTime(const int inBuildTime);

	bool			getIsResearchable(void) const;
	bool			getIsResearched(void) const;
	bool			getAllowMultiples(void) const;
	void			setAllowMultiples(const bool inAllow = true);
	void			setResearchable(bool inState = true);
	void			setResearchState(bool inState);

	virtual bool	operator==(const AvHTechNode& inTechNode) const;
	bool			operator!=(const AvHTechNode& inTechNode) const;

	AvHTechNode&	operator=(const AvHTechNode& inTechNode);

private:
	// Adding a member?  Change operator== and the copy contructor, too
	const AvHMessageID	mMessageID;
	AvHTechID		mTechID;
	AvHTechID		mPrereqID1;
	AvHTechID		mPrereqID2;
	int				mCost;
	int				mBuildTime;

	bool			mResearchable;
	bool			mResearched;
	bool			mAllowMultiples;
};

#endif