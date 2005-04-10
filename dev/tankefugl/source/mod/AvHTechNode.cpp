//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHTechNode.cpp $
// $Date: 2002/09/23 22:36:08 $
//
//===============================================================================

#include "mod/AvHTechNode.h"

//for use in operator==
#define CHECK_EQUAL(x) (this->##x == other.##x)

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// AvHTechNode
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

AvHTechNode::AvHTechNode(const AvHMessageID id) : mMessageID(id), mCost(-1), mBuildTime(-1), mResearchable(true),
	mResearched(false), mAllowMultiples(false), mTechID(TECH_NULL), mPrereqID1(TECH_NULL), mPrereqID2(TECH_NULL)
{}

AvHTechNode::AvHTechNode(const AvHTechNode& other) : mMessageID(other.mMessageID), mTechID(other.mTechID), 
	mCost(other.mCost), mPrereqID1(other.mPrereqID1), mPrereqID2(other.mPrereqID2), mBuildTime(other.mBuildTime),
	mResearchable(other.mResearchable), mResearched(other.mResearched), mAllowMultiples(other.mAllowMultiples)
{}

AvHTechNode::AvHTechNode(AvHMessageID inMessageID, AvHTechID inID, AvHTechID inPrereq1, AvHTechID inPrereq2, int inCost, int inBuildTime, bool inResearched) :
	mMessageID(inMessageID), mTechID(inID), mPrereqID1(inPrereq1), mPrereqID2(inPrereq2), mCost(inCost), 
	mBuildTime(inBuildTime), mResearchable(true), mResearched(inResearched), mAllowMultiples(false)
{}

AvHTechNode::~AvHTechNode(void) {}

AvHTechNode* AvHTechNode::clone(void) const	{ return new AvHTechNode(*this); }

void AvHTechNode::swap(AvHTechNode& other)
{
	AvHTechID tempTech = mTechID; mTechID = other.mTechID; other.mTechID = tempTech;
	tempTech = mPrereqID1; mPrereqID1 = other.mPrereqID1; other.mPrereqID1 = tempTech;
	tempTech = mPrereqID2; mPrereqID2 = other.mPrereqID2; other.mPrereqID2 = tempTech;
	int temp = mCost; mCost = other.mCost; other.mCost = temp;
	temp = mBuildTime; mBuildTime = other.mBuildTime; other.mBuildTime = temp;
	bool btemp = mResearchable; mResearchable = other.mResearchable; other.mResearchable = btemp;
	btemp = mResearched; mResearched = other.mResearched; other.mResearched = btemp;
	btemp = mAllowMultiples; mAllowMultiples = other.mAllowMultiples; other.mAllowMultiples = btemp;
}

AvHMessageID AvHTechNode::getMessageID(void) const { return mMessageID; }
AvHTechID	AvHTechNode::getTechID(void) const { return mTechID; }
AvHTechID	AvHTechNode::getPrereqTechID1(void) const { return mPrereqID1; }
AvHTechID	AvHTechNode::getPrereqTechID2(void) const { return mPrereqID2; }
int			AvHTechNode::getBuildTime(void) const { return mBuildTime; }
int			AvHTechNode::getCost(void) const { return mCost; }
bool		AvHTechNode::getIsResearchable(void) const { return mResearchable; }
bool		AvHTechNode::getIsResearched(void) const { return mResearched; }
bool		AvHTechNode::getAllowMultiples(void) const	{ return mAllowMultiples; }

void AvHTechNode::setTechID(const AvHTechID inTechID) { mTechID = inTechID; }
void AvHTechNode::setPrereqTechID1(const AvHTechID inTechID) { mPrereqID1 = inTechID; }
void AvHTechNode::setPrereqTechID2(const AvHTechID inTechID) { mPrereqID2 = inTechID; }
void AvHTechNode::setBuildTime(const int inBuildTime) { mBuildTime = inBuildTime; }
void AvHTechNode::setCost(const int inCost) { mCost = inCost; }
void AvHTechNode::setResearchable(bool inState) { mResearchable = inState; }
void AvHTechNode::setResearchState(bool inState)
{
	mResearched = inState;
	if(!mAllowMultiples)
	{ mResearchable = !mResearched; }
}

bool AvHTechNode::operator==(const AvHTechNode& other) const
{
	bool theIsEqual = CHECK_EQUAL(mMessageID) && CHECK_EQUAL(mTechID) && CHECK_EQUAL(mPrereqID1)
		&& CHECK_EQUAL(mPrereqID2) && CHECK_EQUAL(mCost) && CHECK_EQUAL(mBuildTime) 
		&& CHECK_EQUAL(mResearchable) && CHECK_EQUAL(mResearched) && CHECK_EQUAL(mAllowMultiples);
	return theIsEqual;
}

bool AvHTechNode::operator!=(const AvHTechNode& other) const
{
	return !this->operator==(other);
}

AvHTechNode& AvHTechNode::operator=(const AvHTechNode& inTechNode)
{
	AvHTechNode node(inTechNode);
	swap(node);
	return *this;
}

void AvHTechNode::setAllowMultiples(const bool inAllow){ mAllowMultiples = inAllow; }