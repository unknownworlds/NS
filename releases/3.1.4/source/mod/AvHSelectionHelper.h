#ifndef AVH_SELECTION_HELPER_H
#define AVH_SELECTION_HELPER_H

#include "mod/AvHConstants.h"

// Selection stuff shared between client and server
// Queues up selection requests, executes them after prediction, once all values are set up
// Used by AvHHud and AvHPlayer to make sure selection is predicted as accurately as possible
class AvHSelectionHelper
{
public:
				AvHSelectionHelper();
	//void		CategorizeSelection(const Vector& inPointOfView, const Vector& inStartRay, SelectionInfo& outInfo);
	void		ClearSelection();
	void		GetAndClearSelection(EntityListType& outSelectedList);
	void		ProcessPendingSelections();
	void		QueueSelection(const Vector& inPointOfView, const Vector& inStartRay, const Vector& inEndRay, AvHTeamNumber inTeamNumber);
	bool		SelectionResultsWaiting();
				
private:		
	bool		IsPositionInRegion(const Vector& inPosition, const Vector& inPointOfView, const Vector& inNormRayOne, const Vector& inNormRayTwo);
	void		ProcessEntityForSelection(const Vector& inOrigin, const Vector& inPointOfView, const Vector& inNormRayOne, const Vector& inNormRayTwo, int inIndex, bool inIsPlayer, bool inIsMarkedSelectable, bool inSameTeam, bool inIsVisible);
	//void		SelectLocation(const Vector& inPointOfView, const Vector& inNormRay, Vector& outVector);
	bool		SelectUnitsInRegion(const Vector& inPointOfView, const Vector& inNormRayOne, const Vector& inNormRayTwo, AvHTeamNumber inTeamNumber, EntityListType& outEntIndexList);
	bool		SelectUnits(const Vector& inPointOfView, const Vector& inStartRay, const Vector& inEndRay, AvHTeamNumber inTeamNumber, EntityListType& outEntIndexList);

	Vector		mQueuedPointOfView;
	Vector		mQueuedRayOne;
	Vector		mQueuedRayTwo;
	bool		mQueuedSelectionWaiting;
	AvHTeamNumber	mQueuedTeamNumber;
	
	EntityListType	mSelectionResults;
	bool		mSelectionResultsWaiting;

	EntityListType	mFriendlyPlayers;
	EntityListType	mFriendlyBuildings;
	EntityListType	mNonFriendlyPlayers;
	EntityListType	mNonFriendlyBuildings;
	//EntityList	mWorldObjects;
	
};

#endif