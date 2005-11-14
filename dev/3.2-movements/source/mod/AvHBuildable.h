#ifndef AVH_BUILDABLE_H
#define AVH_BUILDABLE_H

#include "mod/AvHTechID.h"
#include "mod/AvHConstants.h"

// Interface type class, don't inherit off anything
class AvHBuildable
{
public:
							AvHBuildable(AvHTechID inTechID);

	virtual int				GetBuilder() const;

	virtual bool			GetHasBeenBuilt() const;

	virtual bool			GetIsTechActive() const = 0;

	virtual bool			GetSupportsTechID(AvHTechID	inTechID) const;
	
	virtual AvHTeamNumber	GetTeamNumber() const = 0;

	virtual AvHTechID		GetTechID() const;

	virtual void			SetTechID(AvHTechID inTechID);

	virtual void			SetConstructionComplete(bool inForce = false) = 0;

	virtual void			SetHasBeenKilled();

	virtual void			SetBuilder(int inEntIndex);
	
	virtual void			SetHasBeenBuilt();

	virtual void			SetResearching(bool inState);
				
	virtual void			TriggerAddTech() const;

	virtual void			TriggerRemoveTech() const;

private:
	AvHTechID				mTechID;

	int						mBuilder;

	bool					mHasBeenBuilt;
};

#endif