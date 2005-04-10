//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHScriptServer.cpp $
// $Date: 2002/07/24 18:45:42 $
//
//-------------------------------------------------------------------------------
// $Log: AvHScriptServer.cpp,v $
// Revision 1.3  2002/07/24 18:45:42  Flayra
// - Linux and scripting changes
//
// Revision 1.2  2002/06/25 18:15:52  Flayra
// - Some enhancements and bugfixes for tutorial
//
// Revision 1.1  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "util/nowarnings.h"
#include "dlls/extdll.h"
#include "dlls/util.h"
#include "dlls/cbase.h"
#include "dlls/player.h"
#include "dlls/weapons.h"
#include "mod/AvHScriptManager.h"
#include "mod/AvHGamerules.h"
#include "util/Checksum.h"
#include "util/STLUtil.h"

extern "C" {
	#include <lua.h>
}

static int errormessage(lua_State* inState)
{
	const char *s = lua_tostring(inState, 1);
	if (s == NULL) s = "(no message)";
	char theErrorMessage[2048];
	sprintf(theErrorMessage, "Script error: %s\n", s);

	//fprintf(stderr, "error: %s\n", s);
	UTIL_ClientPrintAll(HUD_PRINTNOTIFY, theErrorMessage);
	
	return 0;
}

//static int execute(lua_State* inState)
//{
//	// Lame hook to call into server tick
//	int theNumServerTicks = GetGameRules()->GetServerTick();
//	
//	lua_getglobal(inState, "serverTick");
//	lua_pushnumber(inState, theNumServerTicks);
//	lua_call(inState, 1, 0);
//	
//	return 0;
//}


/* a simple "print". based on the code in lbaselib.c */
static int print(lua_State* inState)
{
	int n = lua_gettop(inState);
	int i;
	string theString;
	
	for (i=1; i<=n; i++)
	{
		if (i>1) theString = "\t";
		if (lua_isstring(inState, i))
			theString += string(lua_tostring(inState, i));
		else
		{
			char theBuffer[512];
			sprintf(theBuffer, "%s:%p",lua_typename(inState, lua_type(inState, i)),lua_topointer(inState,i));
			theString += string(theBuffer);
		}
	}
	//theString += string("\n");
	
	UTIL_ClientPrintAll(HUD_PRINTNOTIFY, theString.c_str());
	
	return 0;
}

// inEntityIndex, returns x, y, z
static int getPos(lua_State* inState)
{
	int theNumReturned = 0;
	
	// Get entity index
	int theIndex = lua_tonumber(inState, 1);
	if(theIndex > 0)
	{
		// Look it up
		CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theIndex));
		if(theEntity)
		{
			// Return it's angle
			lua_pushnumber(inState, theEntity->pev->origin.x);
			lua_pushnumber(inState, theEntity->pev->origin.y);
			lua_pushnumber(inState, theEntity->pev->origin.z);
			theNumReturned = 3;
		}
	}
	
	return theNumReturned;
}

// inEntityIndex, x, y, z
static int setPos(lua_State* inState)
{
	// Get entity index
	int theIndex = lua_tonumber(inState, 1);
	if(theIndex > 0)
	{
		// Look it up
		CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theIndex));
		if(theEntity)
		{
			// Set it's position
			float theX = lua_tonumber(inState, 2);
			float theY = lua_tonumber(inState, 3);
			float theZ = lua_tonumber(inState, 4);
			theEntity->pev->origin = vec3_t(theX, theY, theZ);
		}
	}
	
	// Return true or false
	return 0;
}

// inEntityIndex, returns yaw, pitch, roll
static int getAngles(lua_State* inState)
{
	int theNumReturned = 0;

	// Get entity index
	int theIndex = lua_tonumber(inState, 1);
	if(theIndex > 0)
	{
		// Look it up
		CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theIndex));
		if(theEntity)
		{
			// Return it's angle
			lua_pushnumber(inState, theEntity->pev->angles.x);
			lua_pushnumber(inState, theEntity->pev->angles.y);
			lua_pushnumber(inState, theEntity->pev->angles.z);
			theNumReturned = 3;
		}
	}
	
	return theNumReturned;
}

// string inEntityName
static int getEntityIndexWithName(lua_State* inState)
{
	// Get entity name
	int theEntityIndex = -1;

	const char* theEntityName = lua_tostring(inState, 1);
	if(theEntityName)
	{
		CBaseEntity* theEntity = UTIL_FindEntityByTargetname(NULL, theEntityName);
		if(theEntity)
		{
			theEntityIndex = theEntity->entindex();
		}
	}

	lua_pushnumber(inState, theEntityIndex);

	return 1;
}

// string targetName, int entityIndexActivator, int entityIndexCaller, useType, value
static int fireTargets(lua_State* inState)
{
	int theNumArgs = lua_gettop(inState);
	if(theNumArgs == 5)
	{
		const char* theTargetName = lua_tostring(inState, 1);
		if(theTargetName)
		{
			int theActivatorIndex = lua_tonumber(inState, 2);
			int theCallerIndex = lua_tonumber(inState, 3);
			int theIntUseType = lua_tonumber(inState, 4);
			float theValue = lua_tonumber(inState, 5);
			
			// Trim to valid use types
			theIntUseType = min(max(theIntUseType, 0), 3);
			
			// Lookup entity indices (it's OK if either of these return NULL)
			CBaseEntity* theActivatorEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theActivatorIndex));
			CBaseEntity* theCallerEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theCallerIndex));
			USE_TYPE theUseType = USE_TYPE(theIntUseType);
			
			FireTargets(theTargetName, theActivatorEntity, theCallerEntity, theUseType, theValue);
		}
	}
	
	return 0;
}

// inEntityIndex, yaw, pitch, roll
static int setAngles(lua_State* inState)
{
	bool theSuccess = false;
	
	int theIndex = lua_tonumber(inState, 1);
	if(theIndex > 0)
	{
		// Look it up
		CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theIndex));
		if(theEntity)
		{
			// Return it's angle
			theEntity->pev->angles.x = lua_tonumber(inState, 2);
			theEntity->pev->angles.y = lua_tonumber(inState, 3);
			theEntity->pev->angles.z = lua_tonumber(inState, 4);
			theEntity->pev->fixangle = TRUE;
			theSuccess = true;
		}
	}
	
	lua_pushnumber(inState, theSuccess);
	
	return 1;
}

// int inEntityIndex, returns success, health
static int getHealth(lua_State* inState)
{
	bool theSuccess = false;
	float theHealth = -1;	

	int theIndex = lua_tonumber(inState, 1);
	if(theIndex > 0)
	{
		// Look it up
		CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theIndex));
		if(theEntity)
		{
			theHealth = theEntity->pev->health;
			theSuccess = true;
		}
	}
	
	lua_pushnumber(inState, theSuccess);
	lua_pushnumber(inState, theHealth);
	
	return 2;
}

// int inEntityIndex, int inHealth
static int setHealth(lua_State* inState)
{
	bool theSuccess = false;

	int theIndex = lua_tonumber(inState, 1);
	if(theIndex > 0)
	{
		// Look it up
		CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theIndex));
		if(theEntity)
		{
			int theHealth = lua_tonumber(inState, 2);
			theEntity->pev->health = theHealth;
			theSuccess = true;
		}
	}

	lua_pushnumber(inState, theSuccess);

	return 1;
}


// int inEntityIndex, returns success, team
static int getTeam(lua_State* inState)
{
	bool theSuccess = false;
	int theTeam = 0;

	int theIndex = lua_tonumber(inState, 1);
	if(theIndex > 0)
	{
		// Look it up
		CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theIndex));
		if(theEntity)
		{
			theTeam = theEntity->pev->team;
			theSuccess = true;
		}
	}
	
	lua_pushnumber(inState, theSuccess);
	lua_pushnumber(inState, theTeam);
	
	return 2;
}



// takes filename, returns whether errors were encountered, returns whether checksums are identical or not
static int compareWorldChecksum(lua_State* inState)
{
	bool theSuccess = false;
	bool theChecksumsAreEqual = false;
	
	const char* theFileName = lua_tostring(inState, 1);
	Checksum theOldChecksum;
	theSuccess = theOldChecksum.ReadFromFile(theFileName);

	Checksum theNewChecksum;
	GetGameRules()->ComputeWorldChecksum(theNewChecksum);
	if(theSuccess)
	{
		StringList theErrors;
		theChecksumsAreEqual = theNewChecksum.Compare(theOldChecksum, theErrors);
	}
	
	// Return function success
	lua_pushnumber(inState, theSuccess);

	// Return checksums are equal
	lua_pushnumber(inState, theChecksumsAreEqual);
	
	return 2;
}

// takes filename, returns success
static int saveWorldChecksum(lua_State* inState)
{
	bool theSuccess = false;

	Checksum theChecksum;
	GetGameRules()->ComputeWorldChecksum(theChecksum);

	const char* theFileName = lua_tostring(inState, 1);
	theSuccess = theChecksum.SaveToFile(theFileName);

	// Return success
	lua_pushnumber(inState, theSuccess);

	return 1;
}

// int inEntityID, string itemName
static int giveNamedItem(lua_State* inState)
{
	// Get entity index
	int theIndex = lua_tonumber(inState, 1);
	if(theIndex > 0)
	{
		// Look it up
		CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theIndex));
		if(theEntity)
		{
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
			if(thePlayer)
			{
				const char* theItemName = lua_tostring(inState, 2);
				if(theItemName != NULL)
				{
					thePlayer->GiveNamedItem(theItemName);
				}
			}
		}
	}
	
	return 0;
}

// int destination, string formattedString, returns success
static int printMessage(lua_State* inState)
{
	bool theSuccess = false;

	int theNumArgs = lua_gettop(inState);
	if(theNumArgs == 2)
	{
		int theMessageDest = lua_tonumber(inState, 1);
		ALERT_TYPE theAlertType = ALERT_TYPE(min(max(theMessageDest, 0), 5));
		
		const char* theFormattedCString = lua_tostring(inState, 2);
		if(theFormattedCString)
		{
			char theCharArray[kMaxStrLen];
			strcpy(theCharArray, theFormattedCString);

			ALERT(theAlertType, theCharArray);

			theSuccess = true;
		}
	}
	
	lua_pushnumber(inState, theSuccess);

	return 1;
}

// int thePlayerIndex, string string (in titles.txt), returns success
static int sendMessage(lua_State* inState)
{
	bool theSuccess = false;

	int theNumArgs = lua_gettop(inState);
	if(theNumArgs == 2)
	{
		int thePlayerIndex = lua_tonumber(inState, 1);
		
		const char* theCString = lua_tostring(inState, 2);
		if(theCString)
		{
			char theCharArray[kMaxStrLen];
			strcpy(theCharArray, theCString);

			CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(thePlayerIndex));
			if(theEntity)
			{
				AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
				if(thePlayer)
				{
					thePlayer->SendMessage(theCString, true);
					theSuccess = true;
				}
			}
		}
	}

	lua_pushnumber(inState, theSuccess);
	
	return 1;
}

// nothing passed in, nothing returned
static int resetWorld(lua_State* inState)
{
	GetGameRules()->DeleteAndResetEntities();
	return 0;
}

// plays a precached sound
static int playSound(lua_State* inState)
{
	int theNumArgs = lua_gettop(inState);
	if(theNumArgs >= 2)
	{
		int theEntIndex = lua_tonumber(inState, 1);
		
		const char* theSoundName = lua_tostring(inState, 2);
		
		CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theEntIndex));
		if(theEntity)
		{
			float theVolume = 1.0f;
			if(theNumArgs >= 3)
			{
				theVolume = lua_tonumber(inState, 3);
			}
			
			float theAttenuation = ATTN_NORM;
			if(theNumArgs >= 4)
			{
				theAttenuation = lua_tonumber(inState, 4);
			}
			
			EMIT_SOUND(theEntity->edict(), CHAN_AUTO, theSoundName, theVolume, theAttenuation);
		}
	}
	
	return 0;
}

// int playerIndex, string command
static int serverCommand(lua_State* inState)
{
	// Get entity index
	int theIndex = lua_tonumber(inState, 1);
	if(theIndex > 0)
	{
		// Look it up
		CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theIndex));
		if(theEntity)
		{
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
			if(thePlayer)
			{
				const char* theServerCommand = lua_tostring(inState, 2);
				if(theServerCommand != NULL)
				{
					GetGameRules()->ClientCommand(thePlayer, theServerCommand);
				}
			}
		}
	}
	
	return 0;
}

static int runClientScript(lua_State* inState)
{
	// Get entity index
	int theIndex = lua_tonumber(inState, 1);
	if(theIndex > 0)
	{
		// Look it up
		CBaseEntity* theEntity = CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(theIndex));
		if(theEntity)
		{
			AvHPlayer* thePlayer = dynamic_cast<AvHPlayer*>(theEntity);
			if(thePlayer)
			{
				const char* theScript = lua_tostring(inState, 2);
				if(theScript != NULL)
				{
					thePlayer->RunClientScript(theScript);
				}
			}
		}
	}
	
	return 0;
}

static int setServerVariable(lua_State* inState)
{
	// Get variable name
	const char* theVariableName = lua_tostring(inState, 1);
	if(theVariableName)
	{
		float theValue = lua_tonumber(inState, 2);
		CVAR_SET_FLOAT(theVariableName, theValue);
	}

	return 0;
}

void AvHScriptInstance::InitServer()
{
	//lua_register(this->mState, LUA_ERRORMESSAGE, errormessage);
	
	//lua_register(this->mState, "execute", execute);
	lua_register(this->mState, "print", print);
	
	lua_register(this->mState, "getAngles", getAngles);
	lua_register(this->mState, "setAngles", setAngles);
	
	lua_register(this->mState, "getEntityIndexWithName", getEntityIndexWithName);
	lua_register(this->mState, "fireTargets", fireTargets);
	
	lua_register(this->mState, "getPos", getPos);
	lua_register(this->mState, "setPos", setPos);
	
	lua_register(this->mState, "getHealth", getHealth);
	lua_register(this->mState, "setHealth", setHealth);

	lua_register(this->mState, "getTeam", getTeam);
	
	lua_register(this->mState, "compareWorldChecksum", compareWorldChecksum);
	lua_register(this->mState, "saveWorldChecksum", saveWorldChecksum);
	
	lua_register(this->mState, "printMessage", printMessage);
	lua_register(this->mState, "sendMessage", sendMessage);
	
	lua_register(this->mState, "giveNamedItem", giveNamedItem);
	lua_register(this->mState, "playSound", playSound);
	lua_register(this->mState, "resetWorld", resetWorld);
	lua_register(this->mState, "serverCommand", serverCommand);
	lua_register(this->mState, "runClientScript", runClientScript);

	lua_register(this->mState, "setServerVariable", setServerVariable);
}