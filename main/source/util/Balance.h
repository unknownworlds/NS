//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//===============================================================================

#ifndef BALANCE_H
#define BALANCE_H

#include <string>
#include <map>
#include <memory>
#include "../Balance.txt"	//default balancing source - this used to be ../Balance.txt

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BalanceValueContainerFactory -- facade that creates, stores, and 
// maintains BalanceValueContainer objects.  Multiple calls with the 
// same parameter will return the same object.  The (void) function 
// call is equivalent to calling for a filename with an empty string.
// Invalid filenames will still return an usable BalanceValueContainer,
// but that container will not be able to save later.
//
// The objects that this function creates should not be manually
// deleted; it is responsible for cleanup of the objects.
//
// TODO: Add another constructor for UPP::ProductInfo objects
// TODO: Make retrieval/creation of objects thread safe (it's not)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class BalanceValueContainer;

namespace BalanceValueContainerFactory
{
	BalanceValueContainer* get(const std::string& filename);
	BalanceValueContainer* get(void);
	const std::string& getDefaultFilename(void);
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BalanceChangeListener -- abstract class that allows arbitrary
// items to listen for changes to the balancing system.  Items can
// register for individual values or for the entire balance variable
// set.  Listeners are notified in advance of each change, but don't
// have the authority to override.
//
// The shouldNotify function is a filter for items registered for
// entire sets, allowing them to select items they are actually
// interested in receiving.  Items that are registered for specific
// variables skip the shouldNotify check.
//
// When a BalanceValueContainer is wiped, the balanceCleared function
// is called rather than iterating over all of the values for all of
// the attached listeners.  It should be considered a signal that
// all items the listener is interested in have been removed.
//
// The BalanceStartCompoundChange and BalanceEndCompoundChange
// functions provide rough grain functionality for listeners that
// want to only operate once on a large change (such as a complete
// reset) instead of once per change.  By combining these functions
// with an internal state variable, they can know when to ignore
// individual signals.  The compound change functions will not be 
// called for individual balance changes, so it's important to track
// these as well if you want a catch-all method of detecting changes.
//
// The compound change functions will only be called for universally
// registered listeners.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

enum BalanceValueType {
	BALANCE_TYPE_FLOAT = 1,
	BALANCE_TYPE_INT = 2,
	BALANCE_TYPE_STRING = 3
};

class BalanceChangeListener
{
public:
	unsigned int getBalanceChangeListenerID(void) const;
	virtual ~BalanceChangeListener(void);

	virtual bool shouldNotify(const std::string& name, const BalanceValueType type) const;
	virtual void balanceCleared(void) const;

	virtual void balanceValueInserted(const std::string& name, const float value) const;
	virtual void balanceValueInserted(const std::string& name, const int value) const;
	virtual void balanceValueInserted(const std::string& name, const std::string& value) const;

	virtual void balanceValueChanged(const std::string& name, const float old_value, const float new_value) const;
	virtual void balanceValueChanged(const std::string& name, const int old_value, const int new_value) const;
	virtual void balanceValueChanged(const std::string& name, const std::string& old_value, const std::string& default_value) const;

	virtual void balanceValueRemoved(const std::string& name, const float old_value) const;
	virtual void balanceValueRemoved(const std::string& name, const int old_value) const;
	virtual void balanceValueRemoved(const std::string& name, const std::string& old_value) const;

	virtual void balanceStartCompoundChange(void) const;
	virtual void balanceEndCompoundChange(void) const;
protected:
	BalanceChangeListener(void);
private:
	const unsigned int BCL_ID; //handle for tracking registrations; internal use only
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BalanceValueContainer -- abstract parent class for balance variable
// storage and retrieval with change notification.
//
// It is possible to actually use the same name for multiple variable
// types at runtime, but the results are undefined if the conflict
// is saved or reloaded, so it's important to be careful when using
// value types.
//
// When calling addListener, multiple registrations for the same
// variable name or multiple global registrations have no added
// effect.  If a listener is registered globally and for a specific
// variable, it will still only be called once per event.
//
// When calling removeListener without a name argument, only the
// universal listener location is checked for the matching pointer.
// It is safe to leave listeners registered with a container even
// after the listeners have been deconstructed, so it is not required
// to unregister a listener from many different items.
//
// The equality and inequality checks test the equality/inequality
// of the value maps.  The assignment operator performs the minimum
// number of changes required for the value maps to become equal.
// This is much more complex than simple reassignment, but it allows
// proper notification of BalanceChangeListeners.
//
// TODO: Make addition/removal of listeners thread safe (it's not)
// TODO: Make assignment thread safe (it's not)
// TODO: Make insertion/retrieval/removal of values thread safe
// TODO: Make load a minimal change operation (like assignment)
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef std::map<std::string,float>			BalanceFloatCollection;
typedef std::map<std::string,int>			BalanceIntCollection;
typedef std::map<std::string,std::string>	BalanceStringCollection;

class BalanceValueContainer
{
public:
	virtual ~BalanceValueContainer(void);
	virtual BalanceValueContainer& operator=(const BalanceValueContainer& other) = 0;
	virtual bool operator!=(const BalanceValueContainer& other) const = 0;
	virtual bool operator==(const BalanceValueContainer& other) const = 0;

	virtual bool load(void) = 0;
	virtual bool save(void) = 0;

	virtual const float			get(const std::string& name, const float default_value) const = 0;
	virtual const int			get(const std::string& name, const int default_value) const = 0;
	virtual const std::string	get(const std::string& name, const std::string& default_value) const = 0;

	virtual const BalanceFloatCollection*	getFloatMap(void) const = 0;
	virtual const BalanceIntCollection*		getIntMap(void) const = 0;
	virtual const BalanceStringCollection*	getStringMap(void) const = 0;

	virtual void insert(const std::string& name, const float value) = 0;
	virtual void insert(const std::string& name, const int value) = 0;
	virtual void insert(const std::string& name, const std::string& value) = 0;

	virtual void remove(const std::string& name) = 0;
	virtual void clear(void) = 0;

	virtual void addListener(const BalanceChangeListener* listener) = 0;
	virtual void addListener(const std::string& name, const BalanceChangeListener* listener) = 0;
	virtual void removeListener(const BalanceChangeListener* listener) = 0;
	virtual void removeListener(const std::string& name, const BalanceChangeListener* listener) = 0;
protected:
	BalanceValueContainer(void);
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BALANCE_VAR(x) macro - the heart of the balance system, ties value
// to default balance source if PLAYTEST_BUILD is enabled, uses 
// hardcoded value otherwise.  Place the name of the varaible #define
// in Balance.txt as the value in the macro.
//
// BALANCE_LISTENER(x) macro - for registering global
// BalanceChangeListeners (see below), reverts to no operation 
// if AVH_PLAYTEST_BUILD isn't enabled.
//
// BALANCE_FIELD_LISTENER(x,y) macro - for registering field-specific
// BalanceChangeListeners, reverts to no operation if 
// BALANCE_ENABLED isn't defined
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#ifdef BALANCE_ENABLED //use Balance.txt values on server, no-source/explicitly set values for client
	#ifdef SERVER
		#define BALANCE_DEFNAME BalanceValueContainerFactory::getDefaultFilename()
	#else
		#define BALANCE_DEFNAME ""
	#endif

	inline void BALANCE_LISTENER(const BalanceChangeListener* object) { BalanceValueContainerFactory::get(BALANCE_DEFNAME)->addListener(object); }
	inline void BALANCE_FIELD_LISTENER(const BalanceChangeListener* object, const char* field) { BalanceValueContainerFactory::get(BALANCE_DEFNAME)->addListener(field,object); }

	#define BALANCE_VAR(name) GetBalanceVar(#name,name)	//requires macro for string-izing of name
	inline int GetBalanceVar(const char* name, const int value)				{ return BalanceValueContainerFactory::get(BALANCE_DEFNAME)->get(name,value); }
	inline float GetBalanceVar(const char* name, const float value)			{ return BalanceValueContainerFactory::get(BALANCE_DEFNAME)->get(name,value); }
	inline float GetBalanceVar(const char* name, const double value)		{ return BalanceValueContainerFactory::get(BALANCE_DEFNAME)->get(name,(float)value); }
	inline std::string GetBalanceVar(const char* name, const char* value)	{ return BalanceValueContainerFactory::get(BALANCE_DEFNAME)->get(name,value); }
#else
	#define BALANCE_VAR(name) name						//hardcodes the value at compile time
	#define BALANCE_LISTENER(object)
	#define BALANCE_FIELD_LISTENER(object,name)
#endif //BALANCE_ENABLED

#endif //BALANCE_H