#ifndef AVH_BASEINFO_LOCATION_H
#define AVH_BASEINFO_LOCATION_H

#include "util/nowarnings.h"
#include "types.h"
#include "mod/AvHConstants.h"

#ifdef AVH_CLIENT
#include "common/triangleapi.h"
#include "cl_dll/wrect.h"
#include "cl_dll/cl_dll.h"
#endif

#ifdef AVH_SERVER
#include "extdll.h"
class CBaseEntity;
#endif

#include "types.h"

class AvHBaseInfoLocation
{
public:
					AvHBaseInfoLocation();

					AvHBaseInfoLocation(const string& inLocationName, const vec3_t& inMaxExtent, const vec3_t& inMinExtent);

	bool			GetIsPointInRegion(const vec3_t& inPoint) const;

	string			GetLocationName() const;

	vec3_t			GetMaxExtent() const;

	vec3_t			GetMinExtent() const;

protected:
	string			mLocationName;
	vec3_t			mMinExtent;
	vec3_t			mMaxExtent;
};

typedef vector<AvHBaseInfoLocation>	AvHBaseInfoLocationListType;

#endif