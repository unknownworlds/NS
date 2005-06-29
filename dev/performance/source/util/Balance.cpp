#ifdef BALANCE_ENABLED
	#pragma message( "[CONFIG] Balance system enabled" )
#else
	#pragma message( "[CONFIG] Balance system disabled" )
#endif

#include <fstream>
#include <map>
#include <memory>
#include <set>
#include <vector>
#include "Balance.h"

using std::auto_ptr;
using std::ifstream;
using std::map;
using std::ofstream;
using std::set;
using std::string;
using std::vector;

//CONSIDER : use boost shared pointers for BalanceValueContainer map
//CONSIDER : put wrapper around std::map to include notification 
//	functionality and minimal change assignment operator; this would
//  eliminate some heavy redundancy in code below.

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BalanceListenerSort - functor that allows BalanceChangeListeners to be
// placed into sorted sets.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

struct BalanceListenerSort
{
	bool operator()(const BalanceChangeListener* lhs, const BalanceChangeListener* rhs)
	{ return lhs->getBalanceChangeListenerID() < rhs->getBalanceChangeListenerID(); }
	bool operator()(const BalanceChangeListener* lhs, const unsigned int rhs)
	{ return lhs->getBalanceChangeListenerID() < rhs; }
	bool operator()(const unsigned int lhs, const BalanceChangeListener* rhs)
	{ return lhs < rhs->getBalanceChangeListenerID(); }
};

typedef set<const BalanceChangeListener*,BalanceListenerSort> BalanceListenerSet;	//storage

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BalanceSystem - singleton that forms foundation of 
// BalanceValueCollectionFactory functionality and tracks BalanceValueListener
// IDs, which allow deletion of BalanceChangeListeners without decoupling from
// each BalanceValueContainer they listen to.
//
// This is the only code in the new balance system that is sensitive to 
// AVH_PLAYTEST_BUILD; it will return a dummy BalanceValueContainer and ignore
// BalanceChangeListener registration if AVH_PLAYTEST_BUILD is not #define'd.
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

#define g_BalanceSystem BalanceSystem::getInstance()	//shorthand for convenience

class BalanceSystem
{
public:
	static BalanceSystem& getInstance(void);
	~BalanceSystem(void);

	const string& getDefaultFilename(void);
	unsigned int getNextListenerID(void);

	BalanceValueContainer* getContainer(const string& filename);

	void registerListener(const BalanceChangeListener* listener);
	void unregisterListener(const BalanceChangeListener* listener);
	const BalanceChangeListener* getListener(const unsigned int id);

private:
	BalanceSystem(void);

	unsigned int nextID;
	map<string,BalanceValueContainer*> containersByFilename;
	map<unsigned int,const BalanceChangeListener*> listeners;
	string defaultFilename;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// NullValueContainer - no-op implmentation of BalanceValueCollection used
// when playtest code is disabled (note that the macros never call into this, 
// but it provides a sink for directly written calls
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class NullValueContainer : public BalanceValueContainer
{
public:
	NullValueContainer(void) {}
	virtual ~NullValueContainer(void) {}
	virtual BalanceValueContainer& operator=(const BalanceValueContainer& other) { return *this; }
	virtual bool operator!=(const BalanceValueContainer& other) const { return !operator==(other); }
	virtual bool operator==(const BalanceValueContainer& other) const { return (dynamic_cast<const NullValueContainer*>(&other) != NULL); }
	virtual bool load(void) { return false; }
	virtual bool save(void) { return false; }
	virtual const float get(const string& name, const float default_value) const { return default_value; }
	virtual const int get(const string& name, const int default_value) const { return default_value; }
	virtual const string get(const string& name, const string& default_value) const { return default_value; }
	virtual const BalanceFloatCollection* getFloatMap(void) const { return &null_float_map; }
	virtual const BalanceIntCollection* getIntMap(void) const { return &null_int_map; }
	virtual const BalanceStringCollection* getStringMap(void) const { return &null_string_map; }
	virtual void insert(const string& name, const float value) {}
	virtual void insert(const string& name, const int value) {}
	virtual void insert(const string& name, const string& value) {}
	virtual void remove(const string& name) {}
	virtual void clear(void) {}
	virtual void addListener(const BalanceChangeListener* listener) {}
	virtual void addListener(const string& name, const BalanceChangeListener* listener) {}
	virtual void removeListener(const BalanceChangeListener* listener) {}
	virtual void removeListener(const string& name, const BalanceChangeListener* listener) {}
private:
	const static BalanceFloatCollection null_float_map;
	const static BalanceIntCollection null_int_map;
	const static BalanceStringCollection null_string_map;
};

const BalanceFloatCollection NullValueContainer::null_float_map;
const BalanceIntCollection NullValueContainer::null_int_map;
const BalanceStringCollection NullValueContainer::null_string_map;

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// StandardValueContainer - basic implementation of BalanceValueCollection
// that forms concrete base for other subclasses
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

typedef set<unsigned int> BalanceListenerIDSet;					//handle collection

class StandardValueContainer : public BalanceValueContainer
{
public:
	StandardValueContainer(void);
	virtual ~StandardValueContainer(void);
	virtual BalanceValueContainer& operator=(const BalanceValueContainer& other);
	virtual bool operator!=(const BalanceValueContainer& other) const;
	virtual bool operator==(const BalanceValueContainer& other) const;

	virtual void init(void);
	virtual void reset(void);

	virtual bool load(void);
	virtual bool save(void);

	virtual const float		get(const string& name, const float default_value) const;
	virtual const int		get(const string& name, const int default_value) const;
	virtual const string	get(const string& name, const string& default_value) const;

	virtual const BalanceFloatCollection*	getFloatMap(void) const;
	virtual const BalanceIntCollection*		getIntMap(void) const;
	virtual const BalanceStringCollection*	getStringMap(void) const;

	virtual void insert(const string& name, const float value);
	virtual void insert(const string& name, const int value);
	virtual void insert(const string& name, const string& value);

	virtual void remove(const string& name);
	virtual void clear(void);

	virtual void addListener(const BalanceChangeListener* listener);
	virtual void addListener(const string& name, const BalanceChangeListener* listener);
	virtual void removeListener(const BalanceChangeListener* listener);
	virtual void removeListener(const string& name, const BalanceChangeListener* listener);

protected:
	virtual BalanceListenerSet getListeners(const string& name, const BalanceValueType type);
	virtual BalanceListenerSet getAllListeners(void);
	virtual BalanceListenerSet getUniversalListeners(void);

	BalanceFloatCollection float_values;
	BalanceIntCollection int_values;
	BalanceStringCollection string_values;
	BalanceListenerIDSet universal_listeners;
	map<string,BalanceListenerIDSet*> specific_field_listeners;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FileValueContainer - a value container that reads/writes from/to a text file
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

class FileValueContainer : public StandardValueContainer
{
public:
	FileValueContainer(const string& filename);
	virtual ~FileValueContainer(void);

	virtual void init(void);
	virtual void reset(void);

	virtual bool load(void);
	virtual bool save(void);

protected:
	string m_Filename;
};

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BalanceChangeListener implementation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BalanceChangeListener::BalanceChangeListener(void) : BCL_ID(g_BalanceSystem.getNextListenerID()) { g_BalanceSystem.registerListener(this); }
BalanceChangeListener::~BalanceChangeListener(void) { g_BalanceSystem.unregisterListener(this); }
unsigned int BalanceChangeListener::getBalanceChangeListenerID(void) const { return BCL_ID; }
bool BalanceChangeListener::shouldNotify(const string& name, const BalanceValueType type) const { return true; }
void BalanceChangeListener::balanceCleared(void) const {}
void BalanceChangeListener::balanceValueInserted(const string& name, const float value) const {}
void BalanceChangeListener::balanceValueInserted(const string& name, const int value) const {}
void BalanceChangeListener::balanceValueInserted(const string& name, const string& value) const {}
void BalanceChangeListener::balanceValueChanged(const string& name, const float old_value, const float new_value) const {}
void BalanceChangeListener::balanceValueChanged(const string& name, const int old_value, const int new_value) const {}
void BalanceChangeListener::balanceValueChanged(const string& name, const string& old_value, const string& default_value) const {}
void BalanceChangeListener::balanceValueRemoved(const string& name, const float old_value) const {}
void BalanceChangeListener::balanceValueRemoved(const string& name, const int old_value) const {}
void BalanceChangeListener::balanceValueRemoved(const string& name, const string& old_value) const {}
void BalanceChangeListener::balanceStartCompoundChange(void) const {}
void BalanceChangeListener::balanceEndCompoundChange(void) const {}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BalanceValueContainer implementation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BalanceValueContainer::BalanceValueContainer(void) {}
BalanceValueContainer::~BalanceValueContainer(void) {}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BalanceValueContainerFactory implementation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BalanceValueContainer* BalanceValueContainerFactory::get(void) { return g_BalanceSystem.getContainer(""); }
BalanceValueContainer* BalanceValueContainerFactory::get(const string& filename) { return g_BalanceSystem.getContainer(filename); }
const string& BalanceValueContainerFactory::getDefaultFilename(void) { return g_BalanceSystem.getDefaultFilename(); }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// BalanceSystem implementation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

extern const char* getModDirectory(void);

BalanceSystem::BalanceSystem(void) : nextID(0) 
{
	defaultFilename = getModDirectory();
	defaultFilename += "/Balance.txt";
}
const string& BalanceSystem::getDefaultFilename(void) { return defaultFilename; }
unsigned int BalanceSystem::getNextListenerID(void) { return nextID++; }
void BalanceSystem::unregisterListener(const BalanceChangeListener* listener) { listeners.erase(listener->getBalanceChangeListenerID()); }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void BalanceSystem::registerListener(const BalanceChangeListener* listener) 
{ 
	listeners.insert(map<unsigned int, const BalanceChangeListener*>::value_type(listener->getBalanceChangeListenerID(),listener)); 
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BalanceSystem::~BalanceSystem(void) 
{
	map<string,BalanceValueContainer*>::iterator current, end = containersByFilename.end();
	for( current = containersByFilename.begin(); current != end; ++current )
	{
		delete current->second;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BalanceSystem& BalanceSystem::getInstance(void)
{ 
	static BalanceSystem system;
	return system;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BalanceValueContainer* BalanceSystem::getContainer(const string& filename)
{
	BalanceValueContainer* returnVal = NULL;
#ifdef PLAYTEST_BUILD
	map<string,BalanceValueContainer*>::iterator item = containersByFilename.find(filename);
	if( item != containersByFilename.end() )
	{
		returnVal = item->second;
	}
	else if(filename.empty())
	{
		returnVal = new StandardValueContainer();
		containersByFilename[filename] = returnVal;
	}
	else
	{
		returnVal = new FileValueContainer(filename);
		containersByFilename[filename] = returnVal;
	}
#else
	map<string,BalanceValueContainer*>::iterator item = containersByFilename.find("");
	if( item != containersByFilename.end() )
	{
		returnVal = item->second;
	}
	else
	{
		returnVal = new NullValueContainer();
		containersByFilename[""] = returnVal;
	}
#endif
		
	return returnVal;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const BalanceChangeListener* BalanceSystem::getListener(const unsigned int id) 
{ 
	const BalanceChangeListener* listener = NULL;
	map<unsigned int, const BalanceChangeListener*>::iterator item = listeners.find(id);
	if( item != listeners.end() )
	{ listener = item->second; }
	return listener;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// StandardBalanceVarCollection implementation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

StandardValueContainer::StandardValueContainer(void) { init(); }
StandardValueContainer::~StandardValueContainer(void) 
{
	map<string,BalanceListenerIDSet*>::iterator current, end = specific_field_listeners.end();
	for(current = specific_field_listeners.begin(); current != end; ++current)
	{
		delete current->second;
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StandardValueContainer::init(void) {}
void StandardValueContainer::reset(void) { clear(); }
bool StandardValueContainer::load(void) { return false; }
bool StandardValueContainer::save(void) { return false; }
const BalanceFloatCollection*	StandardValueContainer::getFloatMap(void) const { return &float_values; }
const BalanceIntCollection*		StandardValueContainer::getIntMap(void) const { return &int_values; }
const BalanceStringCollection*	StandardValueContainer::getStringMap(void) const { return &string_values; }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BalanceValueContainer& StandardValueContainer::operator=(const BalanceValueContainer& other)
{
	if(&other == this) //shortcut if we're self-assigned
	{ return *this; }

	BalanceListenerSet listeners = getUniversalListeners();

	{	//START COMPOUND OPERATION
		BalanceListenerSet::iterator current, end = listeners.end();
		for( current = listeners.begin(); current != end; ++current )
		{
			(*current)->balanceStartCompoundChange();
		}
	}

	{	//FLOAT VALUES
		const BalanceFloatCollection* other_collection = other.getFloatMap();
		BalanceFloatCollection::const_iterator current = float_values.begin(), end = float_values.end();
		BalanceFloatCollection::const_iterator other_current = other_collection->begin(), other_end = other_collection->end();
		vector<string> names_to_insert, names_to_remove;
		while( current != end && other_current != other_end )
		{
			if( current->first < other_current->first )
			{
				names_to_remove.push_back(current->first);
				++current;
				continue;
			}
			if( other_current->first < current->first )
			{
				names_to_insert.push_back(other_current->first);
				++other_current;
				continue;
			}
			if( current->second != other_current->second )
			{
				names_to_insert.push_back(current->first);
			}
			++current;
			++other_current;
		}
		while( current != end )
		{
			names_to_remove.push_back(current->first);
			++current;
		}
		while( other_current != other_end )
		{
			names_to_insert.push_back(other_current->first);
			++other_current;
		}
		vector<string>::iterator index, last = names_to_remove.end();
		for( index = names_to_remove.begin(); index != last; ++index )
		{
			remove(*index);
		}
		last = names_to_insert.end();
		for( index = names_to_insert.begin(); index != last; ++index )
		{
			insert(*index,other.get(*index,0.0f));
		}
	}

	{	//INTEGER VALUES
		const BalanceIntCollection* other_collection = other.getIntMap();
		BalanceIntCollection::const_iterator current = int_values.begin(), end = int_values.end();
		BalanceIntCollection::const_iterator other_current = other_collection->begin(), other_end = other_collection->end();
		vector<string> names_to_insert, names_to_remove;
		while( current != end && other_current != other_end )
		{
			if( current->first < other_current->first )
			{
				names_to_remove.push_back(current->first);
				++current;
				continue;
			}
			if( other_current->first < current->first )
			{
				names_to_insert.push_back(other_current->first);
				++other_current;
				continue;
			}
			if( current->second != other_current->second )
			{
				names_to_insert.push_back(current->first);
			}
			++current;
			++other_current;
		}
		while( current != end )
		{
			names_to_remove.push_back(current->first);
			++current;
		}
		while( other_current != other_end )
		{
			names_to_insert.push_back(other_current->first);
			++other_current;
		}
		vector<string>::iterator index, last = names_to_remove.end();
		for( index = names_to_remove.begin(); index != last; ++index )
		{
			remove(*index);
		}
		last = names_to_insert.end();
		for( index = names_to_insert.begin(); index != last; ++index )
		{
			insert(*index,other.get(*index,0));
		}
	}

	{	//STRING VALUES
		const BalanceStringCollection* other_collection = other.getStringMap();
		BalanceStringCollection::const_iterator current = string_values.begin(), end = string_values.end();
		BalanceStringCollection::const_iterator other_current = other_collection->begin(), other_end = other_collection->end();
		vector<string> names_to_insert, names_to_remove;
		while( current != end && other_current != other_end )
		{
			if( current->first < other_current->first )
			{
				names_to_remove.push_back(current->first);
				++current;
				continue;
			}
			if( other_current->first < current->first )
			{
				names_to_insert.push_back(other_current->first);
				++other_current;
				continue;
			}
			if( current->second != other_current->second )
			{
				names_to_insert.push_back(current->first);
			}
			++current;
			++other_current;
		}
		while( current != end )
		{
			names_to_remove.push_back(current->first);
			++current;
		}
		while( other_current != other_end )
		{
			names_to_insert.push_back(other_current->first);
			++other_current;
		}
		vector<string>::iterator index, last = names_to_remove.end();
		for( index = names_to_remove.begin(); index != last; ++index )
		{
			remove(*index);
		}
		last = names_to_insert.end();
		for( index = names_to_insert.begin(); index != last; ++index )
		{
			insert(*index,other.get(*index,""));
		}
	}

	{	//END COMPOUND OPERATION
		BalanceListenerSet::iterator current, end = listeners.end();
		for( current = listeners.begin(); current != end; ++current )
		{
			(*current)->balanceEndCompoundChange();
		}
	}

	return *this;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool StandardValueContainer::operator!=(const BalanceValueContainer& other) const
{
	return !operator==(other);
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool StandardValueContainer::operator==(const BalanceValueContainer& other) const
{
	return float_values == *other.getFloatMap() 
		&& int_values == *other.getIntMap() 
		&& string_values == *other.getStringMap();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StandardValueContainer::insert(const string& name, const int value) 
{
	BalanceListenerSet listeners = getListeners(name,BALANCE_TYPE_INT);

	if( !listeners.empty() )
	{
		BalanceIntCollection::iterator item = int_values.find(name);
		if( item != int_values.end() )
		{
			if( item->second == value )
			{ return; }

			BalanceListenerSet::iterator current, end = listeners.end();
			for( current = listeners.begin(); current != end; ++current )
			{
				(*current)->balanceValueChanged(name,item->second,value);
			}
		}
		else
		{
			BalanceListenerSet::iterator current, end = listeners.end();
			for( current = listeners.begin(); current != end; ++current )
			{
				(*current)->balanceValueInserted(name,value);
			}
		}
	}
	int_values[name] = value; 
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StandardValueContainer::insert(const string& name, const float value) 
{ 
	BalanceListenerSet listeners = getListeners(name,BALANCE_TYPE_FLOAT);

	if( !listeners.empty() )
	{
		BalanceFloatCollection::iterator item = float_values.find(name);
		if( item != float_values.end() )
		{
			if( item->second == value )
			{ return; }

			BalanceListenerSet::iterator current, end = listeners.end();
			for( current = listeners.begin(); current != end; ++current )
			{
				(*current)->balanceValueChanged(name,item->second,value);
			}
		}
		else
		{
			BalanceListenerSet::iterator current, end = listeners.end();
			for( current = listeners.begin(); current != end; ++current )
			{
				(*current)->balanceValueInserted(name,value);
			}
		}
	}
	float_values[name] = value; 
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StandardValueContainer::insert(const string& name, const string& value) 
{ 
	BalanceListenerSet listeners = getListeners(name,BALANCE_TYPE_STRING);

	if( !listeners.empty() )
	{
		BalanceStringCollection::iterator item = string_values.find(name);
		if( item != string_values.end() )
		{
			if( item->second == value )
			{ return; }

			BalanceListenerSet::iterator current, end = listeners.end();
			for( current = listeners.begin(); current != end; ++current )
			{
				(*current)->balanceValueChanged(name,item->second,value);
			}
		}
		else
		{
			BalanceListenerSet::iterator current, end = listeners.end();
			for( current = listeners.begin(); current != end; ++current )
			{
				(*current)->balanceValueInserted(name,value);
			}
		}
	}
	string_values[name] = value; 
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StandardValueContainer::remove(const string& name) 
{
	BalanceIntCollection::iterator item = int_values.find(name);
	if( item != int_values.end() )
	{
		BalanceListenerSet listeners = getListeners(name,BALANCE_TYPE_INT);
		if( !listeners.empty() )
		{
			BalanceListenerSet::iterator current, end = listeners.end();
			for( current = listeners.begin(); current != end; ++current )
			{
				(*current)->balanceValueRemoved(name,item->second);
			}
		}
		int_values.erase(item); 
	}

	BalanceFloatCollection::iterator fitem = float_values.find(name);
	if( fitem != float_values.end() )
	{
		BalanceListenerSet listeners = getListeners(name,BALANCE_TYPE_FLOAT);
		if( !listeners.empty() )
		{
			BalanceListenerSet::iterator current, end = listeners.end();
			for( current = listeners.begin(); current != end; ++current )
			{
				(*current)->balanceValueRemoved(name,fitem->second);
			}
		}
		float_values.erase(fitem);
	}

	BalanceStringCollection::iterator sitem = string_values.find(name);
	if( sitem != string_values.end() )
	{
		BalanceListenerSet listeners = getListeners(name,BALANCE_TYPE_STRING);
		if( !listeners.empty() )
		{
			BalanceListenerSet::iterator current, end = listeners.end();
			for( current = listeners.begin(); current != end; ++current )
			{
				(*current)->balanceValueRemoved(name,sitem->second);
			}
		}
		string_values.erase(sitem);
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StandardValueContainer::clear(void) 
{ 
	BalanceListenerSet listeners = getAllListeners();
	if( !listeners.empty() )
	{
		BalanceListenerSet::iterator current, end = listeners.end();
		for( current = listeners.begin(); current != end; ++current )
		{
			(*current)->balanceCleared();
		}
	}

	int_values.clear();
	float_values.clear();
	string_values.clear();
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const int StandardValueContainer::get(const string& name, const int default_value) const
{ 
	int value = default_value; 
	BalanceIntCollection::const_iterator item = int_values.find(name);
	if( item != int_values.end() )
	{ value = item->second; }
	return value;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const float StandardValueContainer::get(const string& name, const float default_value) const
{ 
	float value = default_value; 
	BalanceFloatCollection::const_iterator item = float_values.find(name);
	if( item != float_values.end() )
	{ value = item->second; }
	return value;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

const string StandardValueContainer::get(const string& name, const string& default_value) const
{ 
	string value = default_value; 
	BalanceStringCollection::const_iterator item = string_values.find(name);
	if( item != string_values.end() )
	{ value = item->second; }
	return value;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StandardValueContainer::addListener(const BalanceChangeListener* listener)
{
	universal_listeners.insert(listener->getBalanceChangeListenerID());
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StandardValueContainer::addListener(const string& name, const BalanceChangeListener* listener)
{
	BalanceListenerIDSet* chain = NULL;
	map<string,BalanceListenerIDSet*>::iterator item = specific_field_listeners.find(name);
	if( item == specific_field_listeners.end() )
	{
		chain = new BalanceListenerIDSet();
		specific_field_listeners[name] = chain;
	}
	else
	{
		chain = item->second;
	}
	chain->insert(listener->getBalanceChangeListenerID());
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StandardValueContainer::removeListener(const BalanceChangeListener* listener)
{
	universal_listeners.erase(listener->getBalanceChangeListenerID());
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

void StandardValueContainer::removeListener(const string& name, const BalanceChangeListener* listener)
{
	map<string,BalanceListenerIDSet*>::iterator item = specific_field_listeners.find(name);
	if( item != specific_field_listeners.end() )
	{
		item->second->erase(listener->getBalanceChangeListenerID());
		if(item->second->empty())
		{
			delete item->second;
			specific_field_listeners.erase(item);
		}
	}
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BalanceListenerSet StandardValueContainer::getListeners(const string& name, const BalanceValueType type)
{
	BalanceListenerSet chain;
	const BalanceChangeListener* listener;

	if( !specific_field_listeners.empty() )
	{
		map<string,BalanceListenerIDSet*>::iterator item = specific_field_listeners.find(name);
		if( item != specific_field_listeners.end() )
		{
			BalanceListenerIDSet::iterator litem, current = item->second->begin(), end = item->second->end();
			while( current != end )
			{
				listener = g_BalanceSystem.getListener(*current);
				if( !listener )
				{
					litem = current;
					++current;
					item->second->erase(litem);
					continue;
				}

				chain.insert(listener);
				++current;
			}
			if( item->second->empty() )
			{
				delete item->second;
				specific_field_listeners.erase(item);
			}
		}
	}
	if( !universal_listeners.empty() )
	{
		BalanceListenerIDSet::iterator item, current = universal_listeners.begin(), end = universal_listeners.end();
		while( current != end )
		{
			listener = g_BalanceSystem.getListener(*current);
			if( !listener )
			{
				item = current;
				++current;
				universal_listeners.erase(item);
				continue;
			}
			if( listener->shouldNotify(name,type) )
			{
				chain.insert(listener);
			}
			++current;
		}
	}
	return chain;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BalanceListenerSet StandardValueContainer::getUniversalListeners(void)
{
	BalanceListenerSet chain;
	const BalanceChangeListener* listener;

	if( !universal_listeners.empty() )
	{
		BalanceListenerIDSet::iterator item, current = universal_listeners.begin(), end = universal_listeners.end();
		while( current != end )
		{
			listener = g_BalanceSystem.getListener(*current);
			if( !listener )
			{
				item = current;
				++current;
				universal_listeners.erase(item);
				continue;
			}
			chain.insert(listener);
			++current;
		}
	}
	return chain;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

BalanceListenerSet StandardValueContainer::getAllListeners(void)
{
	BalanceListenerSet chain = getUniversalListeners();
	const BalanceChangeListener* listener;

	if( !specific_field_listeners.empty() )
	{
		map<string,BalanceListenerIDSet*>::iterator item, current = specific_field_listeners.begin();
		map<string,BalanceListenerIDSet*>::iterator end = specific_field_listeners.end();
		BalanceListenerIDSet::iterator item_id, current_id, end_id;
		while( current != end )
		{
			current_id = current->second->begin();
			end_id = current->second->end();
			while( current_id != end_id )
			{
				listener = g_BalanceSystem.getListener(*current_id);
				if( !listener )
				{
					item_id = current_id;
					++current_id;
					current->second->erase(item_id);
					continue;
				}
				chain.insert(listener);
				++current_id;
			}
			if( current->second->empty() )
			{
				item = current;
				++current;
				delete item->second;
				specific_field_listeners.erase(item);
				continue;
			}
			++current;
		}
	}
	return chain;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// FileBalanceVarCollection implementation
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

FileValueContainer::FileValueContainer(const string& filename) : m_Filename(filename) {}
FileValueContainer::~FileValueContainer(void) {}

void FileValueContainer::init(void) { load(); }
void FileValueContainer::reset(void) { load(); }

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//CONSIDER: output bad lines to error
bool FileValueContainer::load(void)
{
	string content;

	ifstream file;
	file.open(m_Filename.c_str());
	if( !file.is_open() )
	{ return false; }

	BalanceListenerSet listeners = getUniversalListeners();

	{	//START COMPOUND OPERATION
		BalanceListenerSet::iterator current, end = listeners.end();
		for( current = listeners.begin(); current != end; ++current )
		{
			(*current)->balanceStartCompoundChange();
		}
	}

	clear();

	const static int LINE_MAX_LENGTH = 1024;
	char line_src[LINE_MAX_LENGTH];
	string line;
	string name, string_value;
	float float_value;
	int int_value;
	size_t index, end_index, dot_index;
	while( !file.eof() )
	{
		file.getline(line_src,LINE_MAX_LENGTH);

		//ignore comments and garbage
		if( strncmp(line_src,"#define",7) ) { continue; }
		line = string(&line_src[7]);

		//get the name start
		index = line.find_first_not_of(" \t",7);
		if( index == string::npos ) { continue; }

		//get the name end (whitepsace before value)
		end_index = line.find_first_of(" \t",index);
		if( end_index == string::npos ) { continue; }

		name = line.substr(index,end_index);

		//find beginning of the value
		index = line.find_first_not_of(" \t",end_index);
		if( index == string::npos ) { continue; }

		//trim the end of the line
		end_index = line.find_last_of(" \t");
		if( end_index == string::npos || end_index < line.find_last_not_of(" \t") ) //cover cases like "this is a string with whitespace"\n
		{ end_index = line.length(); }

		//detect type and insert value
		switch(line.at(end_index-1))
		{
		case '"': // string values
			//skip a string mismatch
			if(line.at(index) != '\"' || index == end_index) { continue; }
			string_value = line.substr(index+1,end_index-1);
			insert(name,string_value);
			break;
		case '0': case '1': case '2': case '3': case '4':
		case '5': case '6': case '7': case '8': case '9': //float or integer values
			//detect and process integer values
			dot_index = line.find('.',index);
			if( dot_index == string::npos )
			{
				int_value = atoi(line.substr(index,end_index).c_str());
				insert(name,int_value);
				break;
			}
		case 'f': //float values
			float_value = (float)atof(line.substr(index,end_index).c_str());
			insert(name,float_value);
			break;
		}
	}

	file.close();

	{	//END COMPOUND OPERATION
		BalanceListenerSet::iterator current, end = listeners.end();
		for( current = listeners.begin(); current != end; ++current )
		{
			(*current)->balanceEndCompoundChange();
		}
	}

	return true;
}

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

bool FileValueContainer::save(void)
{
	ofstream file;
	file.open(m_Filename.c_str());
	if( !file.is_open() )
	{ return false; }

	file << "// Balance.txt - automatically generated balance data" << std::endl;
	if( !int_values.empty() )
	{
		file << std::endl << "// INTEGER VALUES" << std::endl;

		BalanceIntCollection::const_iterator icurrent, iend = int_values.end();
		for( icurrent = int_values.begin(); icurrent != iend; ++icurrent )
		{
			file << "#define " << icurrent->first << " " << icurrent->second << std::endl;
		}
	}

	if( !float_values.empty() )
	{
		file << std::endl << "// FLOAT VALUES" << std::endl;
		file.precision(3);
		file << std::ios::fixed;
		BalanceFloatCollection::const_iterator fcurrent, fend = float_values.end();
		for( fcurrent = float_values.begin(); fcurrent != fend; ++fcurrent )
		{
			file << "#define " << fcurrent->first << " " << fcurrent->second << "f" << std::endl;
		}
	}

	if( !string_values.empty() )
	{
		file << std::endl << "// STRING VALUES" << std::endl;
		BalanceStringCollection::const_iterator scurrent, send = string_values.end();
		for( scurrent = string_values.begin(); scurrent != send; ++scurrent )
		{
			file << "#define " << scurrent->first << " \"" << scurrent->second << "\"" << std::endl;
		}
	}

	file.close();
	return true;
}