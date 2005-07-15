#include "mod/AvHObjective.h"
#include <vector>
#include <string>

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AvHObjective::AvHObjective(const char *inName, const char *inTitle, const char *inDescription)
{
	this->mName = inName;
	this->mTitle = inTitle;
	this->mDescription = inDescription;
	this->mState = OBJECTIVE_INDETERMINED;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AvHObjectiveState AvHObjective::GetState()
{
	return this->mState;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AvHObjective::SetState(AvHObjectiveState inState)
{
	this->mState = inState;
}


// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AvHObjectiveGroup::AvHObjectiveGroup()
{
	this->mTriggerOnComplete = false;
	this->mTriggerOnFailed = false;
	this->mState = OBJECTIVE_INDETERMINED;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AvHObjectiveGroup::~AvHObjectiveGroup()
{
	this->Clear();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
void AvHObjectiveGroup::Clear()
{
	AvHObjectiveList::iterator iter, end;
	iter = this->mObjectives.begin();
	end = this->mObjectives.end();
	for(; iter < end; iter++)
	{
		delete((AvHObjective *)(iter));
	}
	this->mObjectives.clear();

	this->mName.clear();
	this->mTriggerOnComplete = false;
	this->mTriggerOnFailed = false;
	this->mState = OBJECTIVE_INDETERMINED;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AvHObjectiveState AvHObjectiveGroup::GetState()
{
	return this->mState;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AvHObjectiveState AvHObjectiveGroup::UpdateState()
{
	// iterate through the vector this->mObjectives
	AvHObjectiveList::iterator iter, end;
	iter = this->mObjectives.begin();
	end = this->mObjectives.end();

	bool failed = true;
	bool completed = true;
	for(; iter < end; iter++)
	{
		AvHObjectiveState state = ((AvHObjective *)(iter))->mState;
		if (state == OBJECTIVE_FAILED)
			completed = false;
		else if (state == OBJECTIVE_COMPLETED)
			failed = false;
		else if (state == OBJECTIVE_INDETERMINED)
			completed = failed = false;
	}
	if (completed)
		this->mState = OBJECTIVE_COMPLETED;
	else if (failed)
		this->mState = OBJECTIVE_FAILED;

	return this->mState;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
AvHObjective *AvHObjectiveGroup::AddObjective(const char *inName, const char *inTitle, const char *inDescription)
{
	AvHObjective *theObjective = new AvHObjective(inName, inTitle, inDescription);
	this->mObjectives.push_back(theObjective);
	return theObjective;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
AvHObjectiveManager::AvHObjectiveManager()
{
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
AvHObjectiveManager::~AvHObjectiveManager()
{
	this->Clear();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
void AvHObjectiveManager::Clear()
{
	AvHObjectiveGroupList::iterator iter, end;
	iter = this->mObjectiveGroups.begin();
	end = this->mObjectiveGroups.end();
	for(; iter != end; iter++)
	{
		delete((AvHObjectiveGroup *)(iter->second));
	}
	this->mObjectiveGroups.clear();
	this->mObjectives.clear();
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
AvHObjectiveGroup *AvHObjectiveManager::FindGroup(const char *inGroup)
{
	AvHObjectiveGroupList::iterator iter = this->mObjectiveGroups.begin();
	if (iter != this->mObjectiveGroups.end())
	{
		return ((AvHObjectiveGroup *)(iter->second));
	}
	return NULL;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
AvHObjective *AvHObjectiveManager::FindObjective(const char *inObjectivename)
{
	AvHObjectiveMapList::iterator iter = this->mObjectives.find(inObjectivename);
	if (iter != this->mObjectives.end())
	{
		return ((AvHObjective *)(iter->second));
	}
	return NULL;
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
void AvHObjectiveManager::AddObjective(const char *inGroup, const char *inName, const char *inTitle, const char *inDescription)
{
	AvHObjectiveGroup *theObjectiveGroup = this->FindGroup(inGroup);
	if (!theObjectiveGroup)
	{
		theObjectiveGroup = new AvHObjectiveGroup();
		this->mObjectiveGroups.insert(std::make_pair(inGroup, theObjectiveGroup));
	}
	AvHObjective *theObjective = theObjectiveGroup->AddObjective(inName, inTitle, inDescription);
	this->mObjectives.insert(std::make_pair(inName, theObjective));
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
void AvHObjectiveManager::SetObjectiveGroupTriggerState(const char *inGroup, AvHObjectiveState inState)
{
	AvHObjectiveGroup *theObjectiveGroup = this->FindGroup(inGroup);
	if (theObjectiveGroup)
	{
		if (inState == OBJECTIVE_COMPLETED)
			theObjectiveGroup->mTriggerOnComplete = true;
		else if (inState == OBJECTIVE_FAILED)
			theObjectiveGroup->mTriggerOnFailed = true;
	}
}

// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
AvHObjectiveState AvHObjectiveManager::GetObjectivesState()
{
	AvHObjectiveGroupList::iterator iter, end;
	iter = this->mObjectiveGroups.begin();
	end = this->mObjectiveGroups.end();
	for(; iter != end; iter++)
	{
		bool triggerOnComplete = ((AvHObjectiveGroup *)(iter->second))->mTriggerOnComplete;
		bool triggerOnFailed = ((AvHObjectiveGroup *)(iter->second))->mTriggerOnFailed;
		if (triggerOnComplete || triggerOnFailed)
		{
			AvHObjectiveState state = ((AvHObjectiveGroup *)(iter->second))->UpdateState();

			if (triggerOnComplete && state == OBJECTIVE_COMPLETED)
				return OBJECTIVE_COMPLETED;
			else if (triggerOnFailed && state == OBJECTIVE_FAILED)
				return OBJECTIVE_FAILED;
		}
	}
	return OBJECTIVE_INDETERMINED;
}