#include "util/nowarnings.h"

#ifdef AVH_SERVER
#include "extdll.h"
#include "util.h"
#include "types.h"
#endif

#ifdef AVH_CLIENT
#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#endif

#include "mod/AvHSelectionHelper.h"
#include "mod/AvHConstants.h"

#ifdef AVH_SERVER
#include "mod/AvHPlayer.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHEntities.h"
#include "mod/AvHGamerules.h"
#endif

#include "pm_shared/pm_defs.h"
#include "pm_shared/pm_shared.h"

#ifdef AVH_CLIENT
#include "pm_shared/pm_debug.h"
extern DebugPointListType				gTriDebugLocations;
#endif

#include "util/MathUtil.h"

extern playermove_t *pmove;
#include "mod/AvHSharedUtil.h"

#include "common/vector_util.h"

#ifdef AVH_CLIENT
#include "cl_dll/eventscripts.h"

#include "common/r_efx.h"
#include "common/event_api.h"
#include "common/event_args.h"
#include "cl_dll/in_defs.h"
#endif

#include "mod/AvHSpecials.h"

AvHSelectionHelper::AvHSelectionHelper()
{
	this->mQueuedTeamNumber = TEAM_IND;
	this->mQueuedSelectionWaiting = false;
	this->mSelectionResultsWaiting = false;
}

void AvHSelectionHelper::ClearSelection()
{
	ASSERT(this->mSelectionResults.size() == 0);
	this->mSelectionResults.clear();
	this->mSelectionResultsWaiting = true;
}

void AvHSelectionHelper::GetAndClearSelection(EntityListType& outSelectedList)
{
	ASSERT(this->mSelectionResultsWaiting);
	outSelectedList = this->mSelectionResults;
	this->mSelectionResults.clear();
	this->mSelectionResultsWaiting = false;
}

void AvHSelectionHelper::ProcessPendingSelections()
{
	if(this->mQueuedSelectionWaiting)
	{
		this->mSelectionResultsWaiting = this->SelectUnits(this->mQueuedPointOfView, this->mQueuedRayOne, this->mQueuedRayTwo, this->mQueuedTeamNumber, this->mSelectionResults);
		this->mQueuedSelectionWaiting = false;
	}
}

void AvHSelectionHelper::QueueSelection(const Vector& inPointOfView, const Vector& inStartRay, const Vector& inEndRay, AvHTeamNumber inTeamNumber)
{
	this->mQueuedPointOfView = inPointOfView;
	this->mQueuedRayOne = inStartRay;
	this->mQueuedRayTwo = inEndRay;
	this->mQueuedTeamNumber = inTeamNumber;

	this->mQueuedSelectionWaiting = true;
}

bool AvHSelectionHelper::SelectionResultsWaiting()
{
	return this->mSelectionResultsWaiting;
}

//#ifdef AVH_SERVER
//void AvHSelectionHelper::SelectLocation(const Vector& inPointOfView, const Vector& inNormRay, Vector& outVector)
//{
//	TraceResult		tr;
//	Vector			theStartPos;
//	Vector			theEndPos;
//	bool			theSuccess = false;
//	bool			theDone = false;
//	
//	VectorMA(inPointOfView, kSelectionStartRange, inNormRay, theStartPos);
//	VectorMA(inPointOfView, kSelectionEndRange, inNormRay, theEndPos);
//	
//	CBaseEntity* theEntityHit = NULL;
//	edict_t* theEdictToIgnore = NULL;
//	
//	do
//	{
//		UTIL_TraceLine(theStartPos, theEndPos, ignore_monsters, theEdictToIgnore, &tr);
//		//UTIL_TraceLine(theStartPos, theEndPos, dont_ignore_monsters, dont_ignore_glass, theEdictToIgnore, &tr);
//		
////		theEntityHit = CBaseEntity::Instance(tr.pHit);
////		AvHWaypoint* theGround = dynamic_cast<AvHWaypoint*>(theEntityHit);
////		if(theGround)
////		{
////			VectorCopy(tr.vecEndPos, outVector);
////			theSuccess = true;
////			theDone = true;
////		}
//
//		if((tr.flFraction >= 1.0f) || (tr.flFraction < kFloatTolerance))
//		{
//			theDone = true;
//		}
//		else
//		{
//			if(theEntityHit)
//			{
//				theEdictToIgnore = ENT(theEntityHit->pev);
//			}
//			VectorCopy(tr.vecEndPos, theStartPos);
//		}
//	} while(!theDone);
//}
//#endif	

//#ifdef AVH_CLIENT
//void AvHSelectionHelper::SelectLocation(const Vector& inPointOfView, const Vector& inNormRay, Vector& outVector)
//{
//	// TODO: Change this to only return location when proper entity hit
//	pmtrace_t tr;
//	Vector theStartPos = inPointOfView;
//	Vector theEndPos = theStartPos + kSelectionEndRange*inNormRay;
//	Vector theResult;
//
//	tr = *pmove->PM_TraceLine( (float *)&theStartPos, (float *)&theEndPos, PM_TRACELINE_ANYVISIBLE, 2 /*point sized hull*/, -1 );
//	outVector = tr.endpos;
//}	
//#endif


bool AvHSelectionHelper::IsPositionInRegion(const Vector& inPosition, const Vector& inPointOfView, const Vector& inNormRayOne, const Vector& inNormRayTwo)
{
	bool theSuccess = false;

	// Build normalized vector from eye to entity
	Vector theEyeToEntity;
	VectorSubtract(inPosition, inPointOfView, theEyeToEntity);
	VectorNormalize(theEyeToEntity);

	// Is vector between two other vectors?
	//if(IsVectorBetweenBoundingVectors(theEyeToEntity, inNormRayOne, inNormRayTwo))
	//if(IsVectorBetweenBoundingVectors(inPosition, theEyeToEntity, inNormRayOne, inNormRayTwo, thePlaneABCD))
	if(IsVectorBetweenBoundingVectors(inPosition, theEyeToEntity, inNormRayOne, inNormRayTwo))
	{
		theSuccess = true;
	}
	
	return theSuccess;
}

void AvHSelectionHelper::ProcessEntityForSelection(const Vector& inOrigin, const Vector& inPointOfView, const Vector& inNormRayOne, const Vector& inNormRayTwo, int inIndex, bool inIsPlayer, bool inIsMarkedSelectable, bool inSameTeam, bool inIsVisible)
{
	if(this->IsPositionInRegion(inOrigin, inPointOfView, inNormRayOne, inNormRayTwo))
	{
		if(inIsPlayer || inIsMarkedSelectable)
		{
			bool theIsFriendly = inSameTeam;
			
			if(inIsPlayer)
			{
				if(theIsFriendly)
				{
					this->mFriendlyPlayers.push_back(inIndex);
				}
				else if(inIsVisible)
				{
					//this->mNonFriendlyPlayers.push_back(inIndex);
				}
			}
			else
			{
				if(theIsFriendly)
				{
					this->mFriendlyBuildings.push_back(inIndex);
				}
				else if(inIsVisible)
				{
					//this->mNonFriendlyBuildings.push_back(inIndex);
				}
			}
		}
		//		else if(inIsSelectableWorldObject)
		//		{
		//			this->mWorldObjects.push_back(inIndex);
		//		}
	}
}

bool AvHSelectionHelper::SelectUnitsInRegion(const Vector& inPointOfView, const Vector& inNormRayOne, const Vector& inNormRayTwo, AvHTeamNumber inTeamNumber, EntityListType& outEntIndexList)
{
#ifdef AVH_SERVER
	// Assumes that entities won't be too far away
	float theRadius = GetGameRules()->GetMapExtents().GetTopDownCullDistance()*2;

	CBaseEntity* theBaseEntity = NULL;
	while((theBaseEntity = UTIL_FindEntityInSphere(theBaseEntity, inPointOfView, theRadius)) != NULL)
	{
		const char* theClassName = STRING(theBaseEntity->pev->classname);
		if(!AvHSUGetIsExternalClassName(theClassName))
		{
			
            // Check for EF_NODRAW so that recycled command stations cannot be selected.
            
            if(!GetHasUpgrade(theBaseEntity->pev->iuser4, MASK_TOPDOWN) && !(theBaseEntity->pev->effects & EF_NODRAW) )
			{
				AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theBaseEntity);
				bool theIsPlayer = (thePlayer && thePlayer->GetIsRelevant() && (thePlayer->GetUser3() != AVH_USER3_COMMANDER_PLAYER));
				bool theIsMarkedSelectable = GetHasUpgrade(theBaseEntity->pev->iuser4, MASK_SELECTABLE);
				bool theSameTeam = (theBaseEntity->pev->team == inTeamNumber);
				bool theIsVisible = (theSameTeam || GetHasUpgrade(theBaseEntity->pev->iuser4, MASK_VIS_SIGHTED));
				
				Vector thePosition = theBaseEntity->pev->origin;
				if((thePosition.x == thePosition.y) && (thePosition.y == thePosition.z) && (thePosition.z == 0.0f))
				{
					thePosition.x = (theBaseEntity->pev->mins.x + theBaseEntity->pev->maxs.x)/2.0f;
					thePosition.y = (theBaseEntity->pev->mins.y + theBaseEntity->pev->maxs.y)/2.0f;
					thePosition.z = (theBaseEntity->pev->mins.z + theBaseEntity->pev->maxs.z)/2.0f;
				}
				
				int theEntityIndex = theBaseEntity->entindex();
				AvHSHUGetEntityLocation(theEntityIndex, thePosition);
				
				this->ProcessEntityForSelection(thePosition, inPointOfView, inNormRayOne, inNormRayTwo, theEntityIndex, theIsPlayer, theIsMarkedSelectable, theSameTeam, theIsVisible);
			}
		}
	}
#endif
	
#ifdef AVH_CLIENT
	gEngfuncs.pEventAPI->EV_SetUpPlayerPrediction( false, true );
	
	// Store off the old count
	gEngfuncs.pEventAPI->EV_PushPMStates();
	
	// Now add in all of the players.
	gEngfuncs.pEventAPI->EV_SetSolidPlayers (-1);
	
	physent_t* theEntity = NULL;
	int theNumEnts = pmove->numphysent;
	for (int i = 0; i < theNumEnts; i++)
	{
		theEntity = gEngfuncs.pEventAPI->EV_GetPhysent(i);
		if(theEntity && !GetHasUpgrade(theEntity->iuser4, MASK_TOPDOWN))
		{
			int theEntityIndex = theEntity->info;
			bool theIsPlayer = ((theEntityIndex >= 1) && (theEntityIndex <= gEngfuncs.GetMaxClients()));
			bool theIsMarkedSelectable = GetHasUpgrade(theEntity->iuser4, MASK_SELECTABLE);
			bool theSameTeam = (theEntity->team == inTeamNumber);
			bool theIsVisible = (theSameTeam || GetHasUpgrade(theEntity->iuser4, MASK_VIS_SIGHTED));
			
			if(theIsVisible || theIsPlayer || theIsMarkedSelectable)
			{
				int a = 0;
			}
			
			Vector thePosition = theEntity->origin;
			if((thePosition.x == thePosition.y) && (thePosition.y == thePosition.z) && (thePosition.z == 0.0f))
			{
				thePosition.x = (theEntity->mins.x + theEntity->maxs.x)/2.0f;
				thePosition.y = (theEntity->mins.y + theEntity->maxs.y)/2.0f;
				thePosition.z = (theEntity->mins.z + theEntity->maxs.z)/2.0f;
			}
			
			this->ProcessEntityForSelection(thePosition, inPointOfView, inNormRayOne, inNormRayTwo, theEntityIndex, theIsPlayer, theIsMarkedSelectable, theSameTeam, theIsVisible);
		}
	}

	gEngfuncs.pEventAPI->EV_PopPMStates();
	
	//	gTriDebugLocations.clear();
	//	
	//	DebugPoint thePoint;
	//	thePoint.x = theResultOne.x;
	//	thePoint.y = theResultOne.y;
	//	thePoint.z = theResultOne.z;
	//	gTriDebugLocations.push_back(thePoint);
	//
	//	thePoint.x = theResultTwo.x;
	//	thePoint.y = theResultTwo.y;
	//	thePoint.z = theResultTwo.z;
	//	gTriDebugLocations.push_back(thePoint);
#endif

	bool theSuccess = false;

	// Our own players
	if(this->mFriendlyPlayers.size() > 0)
	{
		outEntIndexList = this->mFriendlyPlayers;
	}
	// Our own buildings only one
	else if(this->mFriendlyBuildings.size() > 0)
	{
		outEntIndexList.push_back(*mFriendlyBuildings.begin());
	}
	// Enemy players (only one)
	else if(this->mNonFriendlyPlayers.size() > 0)
	{
		outEntIndexList.push_back(*this->mNonFriendlyPlayers.begin());
	}
	// Enemy buildings (only one)
	else if(this->mNonFriendlyBuildings.size() > 0)
	{
		outEntIndexList.push_back(*this->mNonFriendlyBuildings.begin());
	}
	// World objects (only one)
//	else if(this->mWorldObjects.size() > 0)
//	{
//		outEntIndexList.push_back(*this->mWorldObjects.begin());
//	}
	if(outEntIndexList.size() > 0)
	{
		theSuccess = true;
	}

	this->mFriendlyBuildings.clear();
	this->mFriendlyPlayers.clear();
	this->mNonFriendlyBuildings.clear();
	this->mNonFriendlyPlayers.clear();
//	this->mWorldObjects.clear();
		
	return theSuccess;
}

bool AvHSelectionHelper::SelectUnits(const Vector& inPointOfView, const Vector& inStartRay, const Vector& inEndRay, AvHTeamNumber inTeamNumber, EntityListType& outEntIndexList)
{
	bool theSuccess = false;

	// Select into new list
	EntityListType theNewSelection;

	Vector theStartRay = inStartRay;
	Vector theEndRay = inEndRay;
	
	// If inNormRayOne and inNormRayTwo are sufficiently close, just do a ray test
	const float theXTolerance = .1f;
	const float theYTolerance = .1f;
	if((fabs(theStartRay.x - theEndRay.x) < theXTolerance) && (fabs(theStartRay.y - theEndRay.y) < theYTolerance))
	{
//		// Ignore team here, we're allowed to click select units on either team
//		int theEntIndex;
//		if(AvHSHUGetEntityAtRay(inPointOfView, inStartRay, theEntIndex))
//		{
//			theNewSelection.push_back(theEntIndex);
//			theSuccess = true;
//		}

		// Select minimum around center
		Vector theCenter;
		theCenter.x = (inStartRay.x + inEndRay.x)/2.0f;
		theCenter.y = (inStartRay.y + inEndRay.y)/2.0f;
//		theCenter.z = (inStartRay.z + inEndRay.z)/2.0f;

		// Not perfect, but good enough
		theStartRay.x = theCenter.x - theXTolerance/2.0f;
		theStartRay.y = theCenter.y - theYTolerance/2.0f;
		VectorNormalize(theStartRay);

		theEndRay.x = theCenter.x + theXTolerance/2.0f;
		theEndRay.y = theCenter.y + theYTolerance/2.0f;
		VectorNormalize(theEndRay);
	}
//	else
//	{
		theSuccess = SelectUnitsInRegion(inPointOfView, theStartRay, theEndRay, inTeamNumber, theNewSelection);
//	}

	if(theSuccess)
	{
		// Set new selection
		outEntIndexList = theNewSelection;
	}

	return theSuccess;
}
