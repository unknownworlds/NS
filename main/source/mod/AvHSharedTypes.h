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

#define CHECK_EQUAL(x) (this->x == inHiveInfo.x )
class AvHHiveInfo
{
public:
	bool	operator==(const AvHHiveInfo& inHiveInfo) const
	{
		bool theAreEqual = CHECK_EQUAL(mHealthPercentage) && CHECK_EQUAL(mPosX) 
			&& CHECK_EQUAL(mPosY) && CHECK_EQUAL(mPosZ) && CHECK_EQUAL(mStatus) 
			&& CHECK_EQUAL(mUnderAttack) && CHECK_EQUAL(mTechnology);
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
#undef CHECK_EQUAL

typedef vector<AvHHiveInfo>		HiveInfoListType;
typedef int						EntityInfo;
typedef vector<EntityInfo>		EntityListType;

typedef struct Selection_s
{
	int group_number;
	EntityListType selected_entities;
	AvHUser3 group_type;
	AvHAlertType group_alert;
	int tracking_entity;
} Selection;

typedef struct ScoreInfo_s
{
	int player_index;
	int score;
	int frags;
	int deaths;
	int player_class;
	int team;
} ScoreInfo;

typedef struct WeaponList_s
{
	string weapon_name;
	int ammo1_type;
	int ammo1_max_amnt;
	int ammo2_type;
	int ammo2_max_amnt;
	int bucket;
	int bucket_pos;
	int bit_index; //pev->weapons bit index
	int flags;
} WeaponList;

#endif
