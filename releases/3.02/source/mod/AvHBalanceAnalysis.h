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
#ifndef AVH_BALANCE_ANALYSIS_H
#define AVH_BALANCE_ANALYSIS_H

#include "util/STLUtil.h"
#include "mod/AvHTechNodes.h"

void AvHBAComputeAnalysis(StringList& outList);

typedef vector<AvHMessageID> MessageIDListType;

class AvHEconomy
{
public:
	AvHEconomy(const AvHTechNodes& inTechNodes, int inInitialResources, int inInitialTowers, bool inMarine, MessageIDListType inTargetTechList);
	
	// Finds the fastest way to get to goal
	void			CalculateFastestTime();
	void			Simulate(int inBuildXNodes);
	
	int				GetBuildNodeCost() const;
	int				GetResultTime() const;
	int				GetResultTowers() const;
	int				GetResultResources() const;
	
private:
		
	AvHTechNodes	mTechNodes;
	int				mInitialResources;
	int				mInitialTowers;
	bool			mMarine;

	MessageIDListType				mTargetTech;
	
	int				mResultTime;
	int				mResultTowers;
	int				mResultResources;
};

#endif