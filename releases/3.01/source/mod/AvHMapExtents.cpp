#include "util/nowarnings.h"
#include "types.h"
#include "mod/AvHMapExtents.h"
#include "mod/AvHConstants.h"

#ifdef AVH_SERVER
#include "dlls/extdll.h"
#include "util.h"
#include "dlls/cbase.h"
#include "engine/shake.h"
#include "dlls/decals.h"
#include "dlls/gamerules.h"
#include "mod/AvHServerUtil.h"
#endif

#ifdef AVH_CLIENT
#include "cl_dll/parsemsg.h"
#endif

AvHMapExtents::AvHMapExtents()
{
	this->ResetMapExtents();
}

#ifdef AVH_SERVER
void AvHMapExtents::CalculateMapExtents()
{
	if(!this->mCalculatedMapExtents)
	{
		// Fetch from map extents entity if the map has one
		FOR_ALL_ENTITIES(kwsMapInfoClassName, AvHMapInfo*)
			*this = theEntity->GetMapExtents();
		END_FOR_ALL_ENTITIES(kwsMapInfoClassName)

		this->mCalculatedMapExtents = true;
	}
}
#endif

void AvHMapExtents::ResetMapExtents()
{
	// Set defaults
	this->mMaxViewHeight = kDefaultViewHeight;
	this->mMinViewHeight = kDefaultMinMapExtent;
	this->mMinMapX = this->mMinMapY = kDefaultMinMapExtent;
	this->mMaxMapX = this->mMaxMapY = kDefaultMaxMapExtent;
	this->mDrawMapBG = true;
	this->mCalculatedMapExtents = false;
	
	#ifdef AVH_SERVER
	this->mTopDownCullDistance = kMaxRelevantCullDistance;
	#endif
}

float AvHMapExtents::GetMinViewHeight() const
{
	return this->mMinViewHeight;
}

float AvHMapExtents::GetMaxViewHeight() const
{
	return this->mMaxViewHeight;
}

void AvHMapExtents::SetMinViewHeight(float inViewHeight)
{
	this->mMinViewHeight = inViewHeight;
}

void AvHMapExtents::SetMaxViewHeight(float inViewHeight)
{
	this->mMaxViewHeight = inViewHeight;
}



float AvHMapExtents::GetMinMapX() const
{
	return this->mMinMapX;
}

float AvHMapExtents::GetMaxMapX() const
{
	return this->mMaxMapX;
}

void AvHMapExtents::SetMinMapX(float inMapX)
{
	this->mMinMapX = inMapX;
	if(this->mMinMapX < -kMaxMapDimension)
	{
		this->mMinMapX = -kMaxMapDimension;
	}
}

void AvHMapExtents::SetMaxMapX(float inMapX)
{
	this->mMaxMapX = inMapX;

	if(this->mMaxMapX > kMaxMapDimension)
	{
		this->mMaxMapX = kMaxMapDimension;
	}
}


float AvHMapExtents::GetMinMapY() const
{
	return this->mMinMapY;
}

float AvHMapExtents::GetMaxMapY() const
{
	return this->mMaxMapY;
}

void AvHMapExtents::SetMinMapY(float inMapY)
{
	this->mMinMapY = inMapY;
	if(this->mMinMapY < -kMaxMapDimension)
	{
		this->mMinMapY = -kMaxMapDimension;
	}
}

void AvHMapExtents::SetMaxMapY(float inMapY)
{
	this->mMaxMapY = inMapY;
	if(this->mMaxMapY > kMaxMapDimension)
	{
		this->mMaxMapY = kMaxMapDimension;
	}
}


bool AvHMapExtents::GetDrawMapBG() const
{
	return this->mDrawMapBG;
}

void AvHMapExtents::SetDrawMapBG(bool inDrawMapBG)
{
	this->mDrawMapBG = inDrawMapBG;
}

#ifdef AVH_SERVER
float AvHMapExtents::GetTopDownCullDistance() const
{
	return this->mTopDownCullDistance;
}

void AvHMapExtents::SetTopDownCullDistance(float inCullDistance)
{
	ASSERT(inCullDistance > 0);
	this->mTopDownCullDistance = inCullDistance;
}


void AvHMapExtents::SendToNetworkStream() const
{
	WRITE_COORD(this->mMinViewHeight);
	WRITE_COORD(this->mMaxViewHeight);
	
	WRITE_COORD(this->mMinMapX);
	WRITE_COORD(this->mMinMapY);
	WRITE_COORD(this->mMaxMapX);
	WRITE_COORD(this->mMaxMapY);
	
	WRITE_BYTE(this->mDrawMapBG);
}
#endif

#ifdef AVH_CLIENT
int	AvHMapExtents::ReceiveFromNetworkStream()
{
	int theBytesRead = 0;

	this->mMinViewHeight = READ_COORD();
	this->mMaxViewHeight = READ_COORD();

	this->mMinMapX = READ_COORD();
	this->mMinMapY = READ_COORD();

	this->mMaxMapX = READ_COORD();
	this->mMaxMapY = READ_COORD();
	theBytesRead += 2*6;
	
	this->mDrawMapBG = READ_BYTE();
	theBytesRead++;
	
	return theBytesRead;
}
#endif


