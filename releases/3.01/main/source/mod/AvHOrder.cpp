//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHOrder.cpp $
// $Date: 2002/10/16 01:02:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHOrder.cpp,v $
// Revision 1.17  2002/10/16 01:02:16  Flayra
// - Added commander hacking particle effect, but it doesn't seem to play or be visible
//
// Revision 1.16  2002/10/03 18:59:46  Flayra
// - Reworked orders
//
// Revision 1.15  2002/09/25 20:49:04  Flayra
// - Removed use order
//
// Revision 1.14  2002/08/02 21:53:18  Flayra
// - Added type to the order, so help can be displayed on the client
//
// Revision 1.13  2002/07/24 18:45:42  Flayra
// - Linux and scripting changes
//
// Revision 1.12  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "mod/AvHOrder.h"

#ifdef AVH_CLIENT
#include "cl_dll/cl_util.h"
#include "cl_dll/util_vector.h"
#include "common/const.h"
#include "engine/progdefs.h"
#include "cl_dll/ev_hldm.h"
#include "cl_dll/parsemsg.h"
#include "common/vector_util.h"
#include "common/r_efx.h"
#endif

#ifdef AVH_SERVER
#include "extdll.h"
#include "util.h"
#include "common/vector_util.h"
//#include "mod/AvHSelection.h"
#include "mod/AvHSelectionHelper.h"
#include "dlls/cbase.h"
#include "mod/AvHPlayer.h"
#include "mod/AvHWeldable.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHParticleConstants.h"
#include "mod/AvHMarineEquipment.h"
#include "mod/AvHPlayerUpgrade.h"
#endif

#include "pm_defs.h"
#include "pm_shared.h"
#include "pm_movevars.h"
#include "pm_debug.h"
#include <stdio.h>  // NULL
#include <math.h>   // sqrt
#include <string.h> // strcpy
#include <stdlib.h> // atoi
#include <ctype.h>  // isspace
#include "mod/AvHSpecials.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHMessage.h"
#include "util/MathUtil.h"
extern playermove_t *pmove;
#include "mod/AvHSelectionHelper.h"
#include "mod/AvHSharedUtil.h"

// Allow assignment within conditional
#pragma warning (disable: 4706)

#ifdef AVH_CLIENT
int	AvHOrder::ReceiveFromNetworkStream()
{
	int theBytesReceived = 0;

	// Receive player

	this->mPlayer = READ_BYTE();
	theBytesReceived += 1;


	// Get order type
	AvHOrderType theOrder = (AvHOrderType)(READ_BYTE());
	theBytesReceived += 1;

	ASSERT(theOrder > ORDERTYPE_UNDEFINED);
	ASSERT(theOrder < ORDERTYPE_MAX);
	this->mOrderType = theOrder;
	
	// Get order target type
	AvHOrderTargetType theOrderTargetType = (AvHOrderTargetType)READ_BYTE();
	theBytesReceived += 1;
	
	ASSERT(theOrderTargetType > ORDERTARGETTYPE_UNDEFINED);
	ASSERT(theOrderTargetType <= ORDERTARGETTYPE_TARGET);
	this->mOrderTargetType = theOrderTargetType;

	// Read target location, if appropriate
	if(this->mOrderTargetType == ORDERTARGETTYPE_LOCATION)
	{
		this->mLocation.x = READ_COORD();
		this->mLocation.y = READ_COORD();
		this->mLocation.z = READ_COORD();
		theBytesReceived += 6;
	}
	
	// Read target index, if appropriate
	if(this->mOrderTargetType == ORDERTARGETTYPE_TARGET)
	{
		this->mTargetIndex = READ_SHORT();
		theBytesReceived += 2;
	}

	this->mOrderTargetUser3 = (AvHUser3)READ_BYTE();
	theBytesReceived += 1;

	this->mOrderStatus = READ_BYTE();
	theBytesReceived++;
	
	return theBytesReceived;
}
#endif

// Server			
#ifdef AVH_SERVER

bool AvHOrder::operator==(const AvHOrder& inOrder) const
{
	bool theAreEqual = false;

	if(this->mPlayer == inOrder.mPlayer)
	{
		if(this->mOrderType == inOrder.mOrderType)
		{
			if(this->mOrderTargetType == inOrder.mOrderTargetType)
			{
				if(this->mOrderTargetUser3 == inOrder.mOrderTargetUser3)
				{
					if(this->mLocation == inOrder.mLocation)
					{
						if(this->mTargetIndex == inOrder.mTargetIndex)
						{
							if(this->mOrderStatus == inOrder.mOrderStatus)
							{
								#ifdef AVH_SERVER
								{
									if(this->mTimeOrderCompleted == inOrder.mTimeOrderCompleted)
									{
										if(this->mOrderID == inOrder.mOrderID)
										{
								#endif
											theAreEqual = true;
								#ifdef AVH_SERVER
										}
									}
								}
								#endif
							}
						}
					}
				}
			}
		}
	}


	return theAreEqual;
}

void AvHOrder::operator=(const AvHOrder& inOrder)
{
	this->mPlayer = inOrder.mPlayer;
	this->mOrderType = inOrder.mOrderType;
	this->mOrderTargetType = inOrder.mOrderTargetType;
	this->mOrderTargetUser3 = inOrder.mOrderTargetUser3;
	this->mLocation = inOrder.mLocation;
	this->mTargetIndex = inOrder.mTargetIndex;
	this->mOrderStatus = inOrder.mOrderStatus;
#ifdef AVH_SERVER
	this->mTimeOrderCompleted = inOrder.mTimeOrderCompleted;
	this->mOrderID = inOrder.mOrderID;
#endif
	return;
}

bool AvHOrder::operator!=(const AvHOrder& inOrder) const
{
	return !this->operator==(inOrder);
}

bool AvHOrder::Update()
{
	bool theOrderJustCompleted = false;

	ASSERT(this->GetReceiver() != -1 );
	//ALERT(at_console, "AvHOrder Update %d : %d \n", this->mOrderType, this->mOrderStatus);
	if(this->GetOrderActive())
	{
		bool theOrderIsComplete = false;
		AvHPlayer* thePlayer = NULL;
		vec3_t theOrderLocation;
		this->GetLocation(theOrderLocation);
		
		EntityInfo theReceiver = this->GetReceiver();
		float theDistance;
		const float kMoveToDistance = 90;
		const float kPickupDistance = 20;
		
		CBaseEntity* theTargetEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(this->mTargetIndex));
		AvHBaseBuildable* theTargetBuildable = dynamic_cast<AvHBaseBuildable*>(theTargetEntity);
		AvHPlayer* theTargetPlayer = dynamic_cast<AvHPlayer*>(theTargetEntity);
		AvHWeldable* theWeldable = dynamic_cast<AvHWeldable*>(theTargetEntity);
		switch(this->mOrderType)
		{
		case ORDERTYPE_UNDEFINED:
		default:
			break;
		
		case ORDERTYPEL_MOVE:
			// set true if all receivers are within a certain distance of move to order
			theTargetPlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theReceiver)));
			if(theTargetPlayer)
			{
				theOrderIsComplete = true;
				theDistance = VectorDistance(theTargetPlayer->pev->origin, theOrderLocation);
				if(!theTargetPlayer->GetIsRelevant() || (theDistance > kMoveToDistance))
				{
					theOrderIsComplete = false;
				}
			}

			if(theOrderIsComplete)
			{
				this->mOrderStatus = kOrderStatusComplete;
			}
			break;
		
		case ORDERTYPET_GET:
			// set true if all receivers are within a certain distance of item
			theTargetPlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theReceiver)));
			if(theTargetPlayer)
			{
				// If one of the players in the group is near enough to pick it up
				theDistance = VectorDistance(theTargetPlayer->pev->origin, theOrderLocation);
				if(theTargetPlayer->GetIsRelevant() && (theDistance < kPickupDistance))
				{
					theOrderIsComplete = true;
				}
			}

		
			// If the item is gone, the order is done
			if(!theTargetEntity)
			{
				this->mOrderStatus = kOrderStatusCancelled;
			}
			break;
		
		case ORDERTYPET_ATTACK:
			// set true if target is dead or not relevant
			if(!theTargetEntity || (theTargetPlayer && !theTargetPlayer->GetIsRelevant()))
			{
				this->mOrderStatus = kOrderStatusCancelled;
				theOrderIsComplete = true;
			}
			else if(theTargetEntity && !theTargetEntity->IsAlive())
			{
				this->mOrderStatus = kOrderStatusComplete;
				theOrderIsComplete = true;
			}
			break;
		
		case ORDERTYPET_BUILD:
			if(!theTargetEntity || !theTargetEntity->IsAlive())
			{
				this->mOrderStatus = kOrderStatusCancelled;
				theOrderIsComplete = true;
			}
			else if(theTargetBuildable && theTargetBuildable->GetIsBuilt())
			{
				this->mOrderStatus = kOrderStatusComplete;
				theOrderIsComplete = true;
			}
			else
			{
				if(theTargetEntity)
				{
					bool theIsBuilding;
					bool theIsResearching;
					float thePercentage;
		
					AvHSHUGetBuildResearchState(theTargetEntity->pev->iuser3, theTargetEntity->pev->iuser4, theTargetEntity->pev->fuser1, theIsBuilding, theIsResearching, thePercentage);
					if(!theIsBuilding && (thePercentage == 1.0f))
					{
						this->mOrderStatus = kOrderStatusComplete;
						theOrderIsComplete = true;
					}
				}
			}
			break;
		
		case ORDERTYPET_GUARD:
			theOrderIsComplete = false;
		
			if(!theTargetEntity ||!theTargetEntity->IsAlive())
			{
				this->mOrderStatus = kOrderStatusCancelled;
				theOrderIsComplete = true;
			}
			break;
		
		case ORDERTYPET_WELD:
			//ALERT(at_console, "Checking weldables ");
			// set true when target is fully welded
			if(!theTargetEntity ||!theTargetEntity->IsAlive())
			{
				this->mOrderStatus = kOrderStatusCancelled;
				theOrderIsComplete = true;
			}
			if(theWeldable && theWeldable->GetIsWelded())
			{
				this->mOrderStatus = kOrderStatusComplete;
				theOrderIsComplete = true;
			} 
//			else if(!theWeldable || !theWeldable->GetCanBeWelded())
//			{
//				this->mOrderStatus = kOrderStatusCancelled;
//				theOrderIsComplete = true;
//			}
			else if ( !theWeldable ) 
			{
				if ( theTargetEntity->pev->iuser3 == AVH_USER3_MARINE_PLAYER )
				{
					// Players are welded if they have full armour
					if ( theTargetEntity->pev->armorvalue == AvHPlayerUpgrade::GetMaxArmorLevel(theTargetEntity->pev->iuser4, (AvHUser3)theTargetEntity->pev->iuser3)) 
					{
						this->mOrderStatus = kOrderStatusComplete;
						theOrderIsComplete = true;
					}
				}
				else 
				{
					// Structures are welded if they have full health
					if ( theTargetEntity->pev->health == theTargetEntity->pev->max_health ) 
					{
						this->mOrderStatus = kOrderStatusComplete;
						theOrderIsComplete = true;
					}
				}
			}
			break;
		}

		if(theOrderIsComplete)
		{
			this->SetOrderCompleted();
			theOrderJustCompleted = true;
		}
	}

	return theOrderJustCompleted;
}

int	AvHOrder::SendToNetworkStream()
{
	int theBytesSent = 0;

	WRITE_BYTE(this->mPlayer);
	
	// Send order type
	WRITE_BYTE(this->mOrderType);
	theBytesSent += 1;
	
	// Send order target type
	WRITE_BYTE(this->mOrderTargetType);
	theBytesSent += 1;
	
	// Send location, if appropriate
	if(this->mOrderTargetType == ORDERTARGETTYPE_LOCATION)
	{
		WRITE_COORD(this->mLocation.x);
		WRITE_COORD(this->mLocation.y);
		WRITE_COORD(this->mLocation.z);
		
		theBytesSent += 6;
	}

	// Send target index, if appropriate
	if(this->mOrderTargetType == ORDERTARGETTYPE_TARGET)
	{
		WRITE_SHORT(this->mTargetIndex);
		theBytesSent += 2;
	}

	WRITE_BYTE(this->mOrderTargetUser3);
	theBytesSent += 1;
	
	// Write completed or not
	WRITE_BYTE(this->mOrderStatus);

	return theBytesSent;
}

bool AvHOrder::SetReceiver(const EntityInfo& inPlayer)
{
	this->mPlayer = inPlayer;
	return true;
}
#endif

// Shared			
AvHOrder::AvHOrder()
{
	this->mPlayer = -1;
	this->mOrderType = ORDERTYPEL_DEFAULT;
	this->mOrderTargetType = ORDERTARGETTYPE_GLOBAL;
	this->mOrderTargetUser3 = AVH_USER3_NONE;
	this->mTargetIndex = -1;
	this->mOrderStatus = kOrderStatusActive;
	
	#ifdef AVH_SERVER
	this->mTimeOrderCompleted = -1;
	this->mOrderID = -1;
	#endif
}

void AvHOrder::ClearReceiver()
{
	this->mPlayer = -1;
}

int	AvHOrder::GetTargetIndex() const
{
	return this->mTargetIndex;
}


AvHOrderType AvHOrder::GetOrderType() const
{
	return this->mOrderType;
}

AvHOrderTargetType AvHOrder::GetOrderTargetType() const
{
	return this->mOrderTargetType;
}

void AvHOrder::SetOrderTargetType(AvHOrderTargetType inTargetType)
{
	this->mOrderTargetType=inTargetType;
}


AvHUser3 AvHOrder::GetTargetUser3Type() const
{
	return this->mOrderTargetUser3;
}

bool AvHOrder::GetHasReceiver(int inIndex) const
{
	return ( inIndex == this->mPlayer );
}

EntityInfo AvHOrder::GetReceiver() const
{
	return this->mPlayer;
}

void AvHOrder::GetLocation(vec3_t& outLocation) const
{
	if(this->mTargetIndex != -1)
	{
		#ifdef AVH_SERVER
		CBaseEntity* theTargetEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(this->mTargetIndex));
		if(theTargetEntity)
		{
			VectorCopy(theTargetEntity->pev->origin, outLocation);
		}
		#endif

		#ifdef AVH_CLIENT
		cl_entity_s* theTargetEntity = gEngfuncs.GetEntityByIndex(this->mTargetIndex);
		if(theTargetEntity)
		{
			VectorCopy(theTargetEntity->curstate.origin, outLocation);
		}
		#endif
	}
	else
	{
		VectorCopy(this->mLocation, outLocation);
	}
}

void AvHOrder::GetOrderColor(float& outR, float& outG, float& outB, float& outA) const
{
	switch(this->mOrderType)
	{
	case ORDERTYPE_UNDEFINED:
	default:
		outR = outG = outB = outA = 1.0f;
		break;
	case ORDERTYPEL_MOVE:
		outR = 0.0f; outG = 1.0f; outB = 0.0f; outA = .5f;
		break;
	case ORDERTYPET_ATTACK:
		outR = 1.0f; outG = 0.0f; outB = 0.0f; outA = .5f;
		break;
	case ORDERTYPET_GUARD:
		outR = 1.0f; outG = 1.0f; outB = 0.0f; outA = .5f;
		break;
	case ORDERTYPET_WELD:
		outR = 0.0f; outG = 0.0f; outB = 1.0f; outA = .5f;
		break;
	}
}


bool AvHOrder::RemovePlayerFromReceivers(int inIndex)
{
	bool theSuccess = false;
	
	if ( inIndex == this->mPlayer ) 
	{
		this->mPlayer = -1;
		theSuccess=true;
	}

	return theSuccess;
}

void AvHOrder::SetTargetIndex(int inTargetIndex)
{
	this->mTargetIndex = inTargetIndex;
}

void AvHOrder::SetOrderType(AvHOrderType inType)
{
	this->mOrderType = inType;

	// TODO: Set target type from order type
	switch(this->mOrderType)
	{
	case ORDERTYPEL_MOVE:
	case ORDERTYPET_WELD:
	default:
		this->mOrderTargetType = ORDERTARGETTYPE_LOCATION;
		break;
	case ORDERTYPET_ATTACK:
	case ORDERTYPET_GUARD:
		this->mOrderTargetType = ORDERTARGETTYPE_TARGET;
		break;
	}
}

void AvHOrder::SetUser3TargetType(AvHUser3 inUser3)
{
	this->mOrderTargetUser3 = inUser3;
}


void AvHOrder::SetLocation(const vec3_t& inPosition)
{
	VectorCopy(inPosition, this->mLocation);
}

bool AvHOrder::GetOrderActive() const
{
	return (this->mOrderStatus == kOrderStatusActive);
}

bool AvHOrder::GetOrderCancelled() const
{
	return (this->mOrderStatus == kOrderStatusCancelled);
}

bool AvHOrder::GetOrderCompleted() const
{
	return (this->mOrderStatus == kOrderStatusComplete);
}

void AvHOrder::SetOrderCompleted()
{
#ifdef AVH_SERVER
	this->mTimeOrderCompleted = gpGlobals->time;
#endif
}

#ifdef AVH_SERVER
int AvHOrder::GetOrderID() const
{
	return this->mOrderID;
}


void AvHOrder::SetOrderID()
{
	static int sOrderID=0;
	this->mOrderID = ++sOrderID;
}

float AvHOrder::GetTimeOrderCompleted() const
{
	return this->mTimeOrderCompleted;
}

void AvHOrder::SetTimeOrderCompleted(float inTime)
{
	this->mTimeOrderCompleted = inTime;
}
#endif

void AvHChangeOrder(OrderListType& inList, const AvHOrder& inOrder)
{
	EntityInfo theReceiver = inOrder.GetReceiver();
	
	// Run through list
	ASSERT( theReceiver != -1 );
	for(OrderListType::iterator theOrderListIter = inList.begin(); theOrderListIter != inList.end(); /* no increment */)
	{
		if ( theOrderListIter->GetHasReceiver(theReceiver) )
		{
			theOrderListIter = inList.erase(theOrderListIter);
		}
		else
		{
			theOrderListIter++;
		}
	}

	// If the order has any receivers, add it (but we could be deleting it)
	if(!inOrder.GetOrderCompleted())
	{
		// Add the order on to the end of the list
		inList.push_back(inOrder);
	}
}

//void AvHRemovePlayerFromOrders(OrderListType& inList, int inPlayerIndex)
//{
//	// Run through list
//	for(OrderListType::iterator theOrderListIter = inList.begin(); theOrderListIter != inList.end(); /* no increment */)
//	{
//		theOrderListIter->RemovePlayerFromReceivers(inPlayerIndex);
//		
//		// If the order has no receivers, remove the order
//		int theNumReceivers = theOrderListIter->GetReceivers().size();
//		if(theNumReceivers == 0)
//		{
//			theOrderListIter = inList.erase(theOrderListIter);
//		}
//		else
//		{
//			theOrderListIter++;
//		}
//	}
//}


//bool AvHOrderTraceNonPlayers(AvHTeamNumber inTeam, const vec3_t& inOrigin, const vec3_t& inNormRay, AvHOrderType& outOrderType, int& outTargetIndex, vec3_t& outTargetPoint)
//{
//	vec3_t			theTraceStart;
//	vec3_t			theTraceEnd;
//	vec3_t			theForward;
//	pmtrace_t*		theTrace = NULL;
//	physent_t*		theTarget = NULL;
//	float			theHeight = 175;
//	qboolean		theDone = false;
//	int				theEntityHit = -1;
//	const int		kHitOffsetAmount = 10;
//	bool			theSuccess = false;
//	
//	// Trace to get target enemy, target friendly and target location
//	// Offset starting position a little so we don't select ourselves
//	VectorCopy(inOrigin, theTraceStart);
//	VectorMA(inOrigin, kSelectionEndRange, inNormRay, theTraceEnd);
//	
//	// While we haven't hit the floor
//	do
//	{
//		//struct pmtrace_s *(*PM_TraceLine)( float *start, float *end, int flags, int usehull, int ignore_pe );
//		theTrace = pmove->PM_TraceLine(theTraceStart, theTraceEnd, PM_TRACELINE_ANYVISIBLE, 2,  -1);
//		
//		// Find out if there are any view height entities at our x,y
//		theEntityHit = theTrace->ent;
//		
//		theTarget = AvHSUGetEntity(theEntityHit);
//		
//		if(theTarget)
//		{
//			// Hit the ground?  Move to, we're done
//			if(theTarget->iuser3 == AVH_USER3_WAYPOINT)
//			{
//				// Use it's center for the height
//				VectorCopy(theTarget->origin, outTargetPoint);
//				
//				// We're done
//				outOrderType = ORDERTYPEL_MOVE;
//				theSuccess = true;
//				theDone = true;
//			}
//		}
//		
//		if(!theTarget)
//		{
//			theDone = true;
//		}
//		
//		// Do the trace again but starting at this point
//		//VectorCopy(, theTraceStart);
//		VectorMA(theTrace->endpos, kHitOffsetAmount, inNormRay, theTraceStart);
//	}
//	// While the end point is not a view entity and not below
//	while((theTrace->fraction >= 0) && (theTrace->fraction <= 1.0f) && !theDone);
//	
//	return theSuccess;
//}

//bool AvHOrderTracePlayers(AvHTeamNumber inTeam, const vec3_t& inOrigin, const vec3_t& inNormRay, AvHOrderType& outOrderType, int& outTargetIndex)
//{
//	int theEntIndex = 0;
//	bool theSuccess = false;
//
//	if(AvHSHUGetEntityAtRay(inOrigin, inNormRay, theEntIndex))
//	{
//		// TODO: Fetch team for player hit
//		//AvHTeamNumber theTeamOfPlayerHit = TEAM_IND;
//
//		//#ifdef AVH_SERVER
//		//CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(*theIterator));
//		//theTeamOfPlayerHit = (theEntity ? (AvHTeamNumber)(theEntity->pev->team) : TEAM_IND);
//		//#endif
//		physent_t* theEntity = AvHSUGetEntity(theEntIndex);
//		if(theEntity)
//		{
//			AvHTeamNumber theTeamOfPlayerHit = AvHTeamNumber(theEntity->team);
//			
//			// Did we hit an enemy?  If so, issue an attack order on him, then we're done, it's highest priority
////			if((theTeamOfPlayerHit != inTeam) && (theTeamOfPlayerHit != TEAM_IND))
////			{
//				outOrderType = ORDERTYPET_ATTACK;
//				outTargetIndex = theEntIndex;
//				theSuccess = true;
////			}
////			// Did we hit an entity on our team?  Repair/guard it, we're done
////			else if((theTeamOfPlayerHit == inTeam) && (theTeamOfPlayerHit != TEAM_IND))
////			{
////				outOrderType = ORDERTYPET_GUARD;
////				outTargetIndex = theEntIndex;
////				theSuccess = true;
////			}
//		}
//	}
//	
//	return theSuccess;
//}

// Used for context sensitive mouse and for processing right-click
// Must be shared, uses prediction code
// Fill in target index or target point, depending on type of order decided upon
AvHOrderType AvHGetDefaultOrderType(AvHTeamNumber inTeam, const vec3_t& inOrigin, const vec3_t& inNormRay, int& outTargetIndex, vec3_t& outTargetPoint, AvHUser3& outUser3, AvHOrderTargetType& outTargetType)
{
	vec3_t			theTraceStart;
	vec3_t			theTraceEnd;
	AvHOrderType	theOrderType = ORDERTYPE_UNDEFINED;

//	// Look for a player order
//	if(!AvHOrderTracePlayers(inTeam, inOrigin, inNormRay, theOrderType, outTargetIndex))
//	{
//		// If couldn't find one, create a non-player order if possible
//		//AvHOrderTraceNonPlayers(inTeam, inOrigin, inNormRay, theOrderType, outTargetIndex, outTargetPoint);
//	}

	
	// Offset a little so we don't hit the commander
	VectorMA(inOrigin, kSelectionStartRange, inNormRay, theTraceStart);
	VectorMA(inOrigin, kSelectionEndRange, inNormRay, theTraceEnd);
	int theFoundIndex = -1;
	vec3_t theFoundLocation;
	AvHTeamNumber theTeamOfThingHit;
	bool thePlayerHit = false;
	int theUserThree = 0;
	int theUserFour = 0;
	
	if(AvHSHUTraceTangible(theTraceStart, theTraceEnd, theFoundIndex, theFoundLocation, theTeamOfThingHit, thePlayerHit, theUserThree, theUserFour))
	{
		float theHealthPercentage=100.0f;
		float theArmorPercentage=100.0f;
		bool theSighted=false;
#ifdef AVH_SERVER
		CBaseEntity *theEntity=AvHSUGetEntityFromIndex(theFoundIndex);
		if ( theEntity ) 
		{
			theHealthPercentage=theEntity->pev->health/theEntity->pev->max_health;
		}
		else 
		{
			ALERT(at_console, "Not a buildable\n");
		}
		
        theArmorPercentage = theEntity->pev->armorvalue/AvHPlayerUpgrade::GetMaxArmorLevel(theEntity->pev->iuser4, (AvHUser3)theEntity->pev->iuser3);
		theSighted=GetHasUpgrade(theEntity->pev->iuser4, MASK_VIS_SIGHTED);
#endif
		if(theUserThree == AVH_USER3_MARINEITEM)
		{
			int a = 0;
		}

		// Did we hit an enemy?  If so, issue an attack order on him, then we're done, it's highest priority
		if(thePlayerHit ) 
		{
//			if ( theTeamOfThingHit != inTeam && theSighted )
//			{
//				theOrderType = ORDERTYPET_ATTACK;
//				outTargetIndex = theFoundIndex;
//				outUser3 = (AvHUser3)theUserThree;
//			}
			// Did we hit a player on our team?  If so check for welding, if not guard
			if((theTeamOfThingHit == inTeam ) && (theTeamOfThingHit != TEAM_IND) )
			{
				theOrderType = ORDERTYPET_GUARD;
				if (  theArmorPercentage < 0.90f ) {
					theOrderType = ORDERTYPET_WELD;
					outTargetType = ORDERTARGETTYPE_TARGET;
				}
				outTargetIndex = theFoundIndex;
				outUser3 = (AvHUser3)theUserThree;
			}
		}
		// Something to pick up?
		if(!thePlayerHit ) 
		{
//			if ( ( theTeamOfThingHit ) != inTeam && (theTeamOfThingHit != TEAM_IND) && theSighted )
//			{
//				// Use it's center for the height
//				VectorCopy(theFoundLocation, outTargetPoint);
//				theOrderType = ORDERTYPET_ATTACK;
//				outTargetIndex = theFoundIndex;
//				outUser3 = (AvHUser3)theUserThree;
//				outTargetType = ORDERTARGETTYPE_LOCATION;
//			}
//			else 
			if ( theUserThree == AVH_USER3_MARINEITEM) 
			{
				// Use it's center for the height
				VectorCopy(theFoundLocation, outTargetPoint);
				outTargetIndex = theFoundIndex;
				outUser3 = (AvHUser3)theUserThree;
				
				// We're done
				theOrderType = ORDERTYPET_GET;
			}
			// Buildable?
			else if(GetHasUpgrade(theUserFour, MASK_BUILDABLE) && (theTeamOfThingHit == inTeam) && (theTeamOfThingHit != TEAM_IND))
			{
				// Use it's center for the height
				VectorCopy(theFoundLocation, outTargetPoint);
				outTargetIndex = theFoundIndex;
				outUser3 = (AvHUser3)theUserThree;
				
				// We're done
				theOrderType = ORDERTYPET_BUILD;
			}
			// Weldable?
			else if(theUserThree == AVH_USER3_WELD )
			{
				// Use it's center for the height
				VectorCopy(theFoundLocation, outTargetPoint);
				outTargetIndex = theFoundIndex;
				
				// We're done
				theOrderType = ORDERTYPET_WELD;
			}
			// Hit the ground?  Move to, we're done
			else if(theUserThree == AVH_USER3_WAYPOINT || (( theTeamOfThingHit != inTeam) && !theSighted ))
			{
				// Use it's center for the height
				VectorCopy(theFoundLocation, outTargetPoint);
			
				// We're done
				theOrderType = ORDERTYPEL_MOVE;
			}
			// Did we hit an entity on our team?  Repair/guard it, we're done
			else if((theTeamOfThingHit == inTeam) && (theTeamOfThingHit != TEAM_IND))
			{
				theOrderType = ORDERTYPET_GUARD;
				if (  theHealthPercentage < 0.90f ) {
					theOrderType = ORDERTYPET_WELD;
					VectorCopy(theFoundLocation, outTargetPoint);
					outTargetType = ORDERTARGETTYPE_LOCATION;
				}
				outTargetIndex = theFoundIndex;
				outUser3 = (AvHUser3)theUserThree;
			}
		}
//		else if(!thePlayerHit && (theUserThree == AVH_USER3_USEABLE))
//		{
//			// Use it's center for the height
//			VectorCopy(theFoundLocation, outTargetPoint);
//			outTargetIndex = theFoundIndex;
//			
//			// We're done
//			theOrderType = ORDERTYPEL_USE;
//		}
	}
	
	return theOrderType;
}

#ifdef AVH_SERVER
bool AvHCreateSpecificOrder(AvHTeamNumber inTeam, const vec3_t& inOrigin, AvHOrderType inOrder, const vec3_t& inNormRay, AvHOrder& outOrder)
{
	bool theSuccess = false;
	int theTargetIndex = -1;
	vec3_t theTargetLocation;
	AvHOrderType theOrderType = inOrder;
	AvHUser3 theUser3 = AVH_USER3_NONE;

	vec3_t theStartPoint;
	VectorMA(inOrigin, kSelectionStartRange, inNormRay, theStartPoint);
	
	vec3_t theEndPoint;
	VectorMA(inOrigin, kSelectionEndRange, inNormRay, theEndPoint);

	vec3_t theValidOrigin;
	AvHSHUServerGetFirstNonSolidPoint(theStartPoint, theEndPoint, theValidOrigin);

	AvHOrderTargetType theTargetType=ORDERTARGETTYPE_UNDEFINED;
	// Create default order if passed in
	if(inOrder == ORDERTYPEL_DEFAULT)
	{
		theOrderType = AvHGetDefaultOrderType(inTeam, theValidOrigin, inNormRay, theTargetIndex, theTargetLocation, theUser3, theTargetType);
	}

	if(theOrderType != ORDERTYPE_UNDEFINED)
	{
		// Init order with values it found
		outOrder.SetOrderType(theOrderType);
		outOrder.SetLocation(theTargetLocation);
		outOrder.SetTargetIndex(theTargetIndex);
		outOrder.SetUser3TargetType(theUser3);
		if ( theTargetType != ORDERTARGETTYPE_UNDEFINED ) 
			outOrder.SetOrderTargetType(theTargetType);
		theSuccess = true;
	}

	outOrder.SetOrderID();
	
	return theSuccess;
}

bool AvHToggleUseable(CBaseEntity* inUser, const vec3_t& inOrigin, const vec3_t& inNormRay)
{
	bool	theSuccess = false;
	vec3_t	theTraceStart;
	vec3_t	theTraceEnd;
	
	// Offset a little so we don't hit the commander
	VectorMA(inOrigin, 100, inNormRay, theTraceStart);
	
	VectorMA(inOrigin, kSelectionEndRange, inNormRay, theTraceEnd);
	int theFoundIndex = -1;
	vec3_t theFoundLocation;
	AvHTeamNumber theTeamOfThingHit;
	bool thePlayerHit = false;
	int theUserThree = 0;
	int theUserFour = 0;
	
	if(AvHSHUTraceTangible(theTraceStart, theTraceEnd, theFoundIndex, theFoundLocation, theTeamOfThingHit, thePlayerHit, theUserThree, theUserFour))
	{
		if(!thePlayerHit && (theUserThree == AVH_USER3_USEABLE))
		{
			// Find entity we clicked on, use it
			//CBaseEntity* theEntity = CBaseEntity::Instance(ENT(theFoundIndex));
			CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theFoundIndex));
			if(theEntity)
			{
				// For each entity with this target name (including this one), use it
				if(theEntity->pev->targetname)
				{
					CBaseEntity* theTarget = NULL;
					while(theTarget = UTIL_FindEntityByTargetname(theTarget, STRING(theEntity->pev->targetname)))
					{
						int	theObjectCaps = theTarget->ObjectCaps();
						if((theObjectCaps & FCAP_IMPULSE_USE) || (FCAP_ONOFF_USE))
						{
							theTarget->Use(inUser, inUser, USE_TOGGLE, 0);
						}
					}
				}
				else if(FClassnameIs(theEntity->edict(), "func_button") && theEntity->pev->target) ////voogru: Its probably a button!, classname check to prevent any possible exploits
				{
					CBaseEntity* theTarget = NULL;
					while(theTarget = UTIL_FindEntityByTargetname(theTarget, STRING(theEntity->pev->target)))
					{
						int	theObjectCaps = theTarget->ObjectCaps();
						if((theObjectCaps & FCAP_IMPULSE_USE) || (FCAP_ONOFF_USE))
						{
							theTarget->Use(inUser, inUser, USE_TOGGLE, 0);
						}
					}
				}
				theSuccess = true;
			}
		}
	}
	return theSuccess;	
}
#endif