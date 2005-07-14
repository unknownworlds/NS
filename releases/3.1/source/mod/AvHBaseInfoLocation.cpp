#include "mod/AvHBaseInfoLocation.h"

AvHBaseInfoLocation::AvHBaseInfoLocation()
{
	this->mMinExtent = this->mMaxExtent = vec3_t(0, 0, 0);
}

AvHBaseInfoLocation::AvHBaseInfoLocation(const string& inLocationName, const vec3_t& inMaxExtent, const vec3_t& inMinExtent)
{
	this->mLocationName = inLocationName;
	this->mMaxExtent = inMaxExtent;
	this->mMinExtent = inMinExtent;
}

bool AvHBaseInfoLocation::GetIsPointInRegion(const vec3_t& inPoint) const
{
	bool thePointIsWithin = false;

	if((inPoint.x > this->mMinExtent.x) && (inPoint.y > this->mMinExtent.y) /*&& (inPoint.z > this->mMinExtent.z)*/)
	{
		if((inPoint.x < this->mMaxExtent.x) && (inPoint.y < this->mMaxExtent.y) /*&& (inPoint.z < this->mMaxExtent.z)*/)
		{
			thePointIsWithin = true;
		}
	}

	return thePointIsWithin;
}

string AvHBaseInfoLocation::GetLocationName() const
{
	return this->mLocationName;
}

vec3_t AvHBaseInfoLocation::GetMaxExtent() const
{
	return this->mMaxExtent;
}

vec3_t AvHBaseInfoLocation::GetMinExtent() const
{
	return this->mMinExtent;
}

