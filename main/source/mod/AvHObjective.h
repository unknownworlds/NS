#ifndef AVH_OBJECTIVE_H
#define AVH_OBJECTIVE_H

#include <vector>
#include <string>
#include <map>

enum AvHObjectiveState
{
	OBJECTIVE_INDETERMINED,
	OBJECTIVE_FAILED,
	OBJECTIVE_COMPLETED
};

class AvHObjective
{
public:
								AvHObjective(const char *inName, const char *inTitle, const char *inDescription);
	AvHObjectiveState			GetState();
	void						SetState(AvHObjectiveState inState);
	std::string					mName;
	std::string					mTitle;
	std::string					mDescription;
	AvHObjectiveState			mState;
};

typedef std::vector<AvHObjective *>	AvHObjectiveList;

class AvHObjectiveGroup
{
public:
								AvHObjectiveGroup();
								~AvHObjectiveGroup();
	void						Clear();
	AvHObjectiveState			GetState();
	AvHObjectiveState			UpdateState();
	AvHObjective				*AddObjective(const char *inName, const char *inTitle, const char *inDescription);
	AvHObjectiveList			mObjectives;
	std::string					mName;
	bool						mTriggerOnFailed;
	bool						mTriggerOnComplete;
	AvHObjectiveState			mState;
};

typedef std::map<const char *, AvHObjectiveGroup *>		AvHObjectiveGroupList;
typedef std::map<const char *, AvHObjective *>			AvHObjectiveMapList;

class AvHObjectiveManager
{
public:
							AvHObjectiveManager();
							~AvHObjectiveManager();
	void					Clear();
	AvHObjectiveGroup		*FindGroup(const char *inGroup);
	AvHObjective			*FindObjective(const char *inObjectivename);
	void					AddObjective(const char *inGroup, const char *inName, const char *inTitle, const char *inDescription);
	void					AddObjectiveGroup(const char *inGroup);
	void					SetObjectiveGroupTriggerState(const char *inGroup, AvHObjectiveState inState);
	AvHObjectiveState		GetObjectivesState();
	AvHObjectiveGroupList	mObjectiveGroups;
	AvHObjectiveMapList		mObjectives;

};

#endif