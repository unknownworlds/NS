#ifndef AVH_SHARED_TYPES_H
#define	AVH_SHARED_TYPES_H
#include "types.h"

const int kHiveInfoStatusUnbuilt = 0;
const int kHiveInfoStatusBuildingStage1 = 1;
const int kHiveInfoStatusBuildingStage2 = 2;
const int kHiveInfoStatusBuildingStage3 = 3;
const int kHiveInfoStatusBuildingStage4 = 4;
const int kHiveInfoStatusBuildingStage5 = 5;
const int kHiveInfoStatusBuilt = 6;
const int kHiveInfoStatusUnderAttack = 7;

class AvHHiveInfo
{
public:
	bool	operator==(const AvHHiveInfo& inHiveInfo) const
	{
		bool theAreEqual = false;

		if(this->mHealthPercentage == inHiveInfo.mHealthPercentage)
		{
			if(this->mPosX == inHiveInfo.mPosX)
			{
				if(this->mPosY == inHiveInfo.mPosY)
				{
					if(this->mPosZ == inHiveInfo.mPosZ)
					{
						if(this->mStatus == inHiveInfo.mStatus)
						{
							if(this->mUnderAttack == inHiveInfo.mUnderAttack)
							{
								if(this->mTechnology == inHiveInfo.mTechnology)
								{
									theAreEqual = true;
								}
							}
						}
					}
				}
			}
		}
		return theAreEqual;
	}

	bool	operator!=(const AvHHiveInfo& inHiveInfo) const
	{
		return !operator==(inHiveInfo);
	}

	float	mPosX;
	float	mPosY;
	float	mPosZ;

	int		mStatus;
	bool	mUnderAttack;
	int		mTechnology;

	int		mHealthPercentage;
};

typedef vector<AvHHiveInfo>		HiveInfoListType;

#endif
