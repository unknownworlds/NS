//======== (C) Copyright 2002 Charles G. Cleveland All rights reserved. =========
//
// The copyright to the contents herein is the property of Charles G. Cleveland.
// The contents may be used and/or copied only with the written permission of
// Charles G. Cleveland, or in accordance with the terms and conditions stipulated in
// the agreement/contract under which the contents have been supplied.
//
// Purpose: 
//
// $Workfile: AvHParticleSystemEntity.cpp $
// $Date: 2002/11/22 21:28:16 $
//
//-------------------------------------------------------------------------------
// $Log: AvHParticleSystemEntity.cpp,v $
// Revision 1.13  2002/11/22 21:28:16  Flayra
// - mp_consistency changes
//
// Revision 1.12  2002/05/23 02:33:20  Flayra
// - Post-crash checkin.  Restored @Backup from around 4/16.  Contains changes for last four weeks of development.
//
//===============================================================================
#include "util/nowarnings.h"
#include "mod/AvHParticleSystemEntity.h"
#include "mod/AvHParticleTemplateServer.h"
#include "mod/AvHParticleSystemManager.h"
#include "mod/AvHConstants.h"
#include "mod/AvHMarineEquipmentConstants.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHParticleTemplate.h"
#include "mod/AvHParticleConstants.h"

extern AvHParticleTemplateListServer	gParticleTemplateList;

uint32 AvHParticleSystemEntity::sCurrentHandle = 1;
const float kDefaultParticleSystemThinkRate = 0.05f;

LINK_ENTITY_TO_CLASS( keParticles, AvHParticleSystemEntity );
LINK_ENTITY_TO_CLASS( keParticlesCustom, AvHCustomParticleSystemEntity );

AvHParticleSystemEntity::AvHParticleSystemEntity()
{
	this->mTemplateIndex = -1;
	this->mIsOn = false;
	//this->mClientIsOn = false;
	this->mUseState = false;
	this->mHandle = 0;
	this->mCreatedTemplate = false;
	this->mTimeParticlesCreated = 0;
	this->mCustomData = 0;
}

void AvHParticleSystemEntity::SetTemplateIndex(int inTemplateIndex)
{
	this->mTemplateIndex = inTemplateIndex;
}

uint16 AvHParticleSystemEntity::GetCustomData() const
{
	return this->mCustomData;
}

void AvHParticleSystemEntity::SetCustomData(uint16 inCustomData)
{
	this->mCustomData = inCustomData;
	this->pev->weaponmodel = this->mCustomData;
}


// Fetch template if we have a valid template index, create a new custom template if we don't
AvHParticleTemplate*
AvHParticleSystemEntity::GetCustomTemplate()
{
	AvHParticleTemplate* theTemplate = NULL;

	if(!this->mCreatedTemplate)
	{
		if(this->mTemplateIndex == -1)
		{
			// Create a new template, none was specified in the name field
			ASSERT(!this->mCreatedTemplate);
			this->mTemplateIndex = gParticleTemplateList.CreateTemplateFromIndex();
		}
		//else
		//{
		//	// Create a template from our existing base template
		//	this->mTemplateIndex = gParticleTemplateList.CreateTemplateFromIndex(this->mTemplateIndex);
		//}
		
		// Set the name of our new custom particle system
		theTemplate = gParticleTemplateList.GetTemplateAtIndex(this->mTemplateIndex);
		ASSERT(theTemplate);
		string theName = STRING(this->pev->targetname);
		theTemplate->SetName(theName);
		
		this->mCreatedTemplate = true;
	}
	
	ASSERT(this->mTemplateIndex != -1);
	if(!theTemplate)
		theTemplate = gParticleTemplateList.GetTemplateAtIndex(this->mTemplateIndex);
	
	return theTemplate;
}

void
AvHParticleSystemEntity::KeyValue( KeyValueData* inPkvd )
{
	if(FStrEq(inPkvd->szKeyName, kpscSystemName))
	{
		uint32	theIndex;
		if(gParticleTemplateList.GetTemplateIndexWithName(inPkvd->szValue, theIndex))
		{
			this->mTemplateIndex = theIndex;
			inPkvd->fHandled = TRUE;
		}
	}
	else
	{
		// Call down to base class
		CBaseEntity::KeyValue(inPkvd);
	}
}

void
AvHParticleSystemEntity::ParticleThink()
{
	this->UpdateClientData();
	
	// Look up particle system
	AvHParticleSystem* theParticleSystem = AvHParticleSystemManager::Instance()->GetParticleSystem(this->mHandle);
	if(theParticleSystem)
	{
		// Call UpdatePhysics()
		theParticleSystem->UpdatePhysics(this->pev);

		// The actual entity's position is the regular position of the entity, unless it's using a generaton entity, then use it instead
		//theParticleSystem->GetEffectiveOrigin(this->pev->origin);
		////UTIL_SetOrigin(this->pev, theGenerationEntityOrigin);
	}

	this->pev->nextthink = gpGlobals->time + kDefaultParticleSystemThinkRate;
}

void
AvHParticleSystemEntity::ParticleTouch( CBaseEntity* /*pOther*/ )
{
	//EMIT_SOUND(ENT(pev), CHAN_VOICE, "buttons/spark1.wav", 1.0f, ATTN_NORM);
}

void
AvHParticleSystemEntity::ParticleUse( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value )
{
	switch(useType)
	{
	case USE_OFF:
		this->mUseState = false;
		break;

	case USE_ON:
		this->mUseState = true;
		break;

	case USE_SET:
		// Handle this?
		break;

	case USE_TOGGLE:
		this->mUseState = !this->mUseState;
		break;
	}
}

void 
AvHParticleSystemEntity::Precache(void)
{
	CBaseEntity::Precache();
	PRECACHE_UNMODIFIED_MODEL(kNullModel);
}

void AvHParticleSystemEntity::SetUseState(USE_TYPE inUseType)
{
	this->ParticleUse(NULL, NULL, inUseType, 0.0f);
}

void
AvHParticleSystemEntity::Spawn( void )
{
	// Just in case our class derives off of something else in the future
	CBaseEntity::Spawn();
	
	this->Precache();
	
	this->pev->classname = MAKE_STRING(kesParticlesCustom);
	this->pev->solid = SOLID_NOT;
	this->pev->movetype = MOVETYPE_NONE;
	//this->pev->movetype = MOVETYPE_PUSH;
	//this->pev->effects = EF_BRIGHTLIGHT;
	
	this->SetTouch(&AvHParticleSystemEntity::ParticleTouch);
	this->SetUse(&AvHParticleSystemEntity::ParticleUse);
	
	SET_MODEL(ENT(this->pev), kNullModel);
	UTIL_SetOrigin(this->pev, this->pev->origin);
	
	// Set the other flags
	AvHParticleTemplate* theTemplate = gParticleTemplateList.GetTemplateAtIndex(this->mTemplateIndex);
	//ASSERT(theTemplate);
	if(theTemplate)
	{
		//theTemplate->SetFlags(theSpawnFlags);
		
		string theTargetName = STRING(this->pev->targetname);
		if(!FStrEq(theTargetName.c_str(), ""))
		{
			theTemplate->SetName(theTargetName);
		}
		
		this->SetThink(&AvHParticleSystemEntity::ParticleThink);
		pev->nextthink = gpGlobals->time + kDefaultParticleSystemThinkRate;
		
		// The spawn flags field is the only one that doesn't get set through KeyValue(), so
		// we set it in the template this way
		int theSpawnFlags = this->pev->spawnflags;
		if(!theSpawnFlags)
		{
			theSpawnFlags = theTemplate->GetFlags();
		}

		// Did they check the "start on" flag?
		if(theSpawnFlags & 1)
		{
			this->mUseState = true;
		}

		theTemplate->SetFlags(theSpawnFlags);
	}
	else
	{
		ALERT(at_logged, "Couldn't find particle system template: %d\n", this->mTemplateIndex);
		UTIL_Remove(this);
	}
}

void
AvHParticleSystemEntity::UpdateClientData()
{
	// Turn the system on or off
	if(this->mIsOn != this->mUseState)
	{
		this->mIsOn = this->mUseState;
		
		// set up entity so it's propagated correctly
		if(this->mIsOn)
		{
			// TODO: Reset sCurrentHandle to 1 every time a new game starts?
			this->mHandle = sCurrentHandle++;
			
			// Create server side system for collision only
			int theEntIndex = this->entindex();
			AvHParticleSystemManager::Instance()->CreateParticleSystemIfNotCreated(theEntIndex, this->mTemplateIndex, this->mHandle);
			
			// Replicate action to everyone
			this->pev->iuser3 = AVH_USER3_PARTICLE_ON;
			const AvHParticleTemplate* theTemplate = gParticleTemplateList.GetTemplateAtIndex(this->mTemplateIndex);
			ASSERT(theTemplate);
				
			int theGenEntityIndex = theTemplate->GetGenerationEntityIndex();
			if(theGenEntityIndex == -1)
			{
				theGenEntityIndex = 0;
			}

			//this->pev->fuser1 = /*this->mTemplateIndex;*/ (theGenEntityIndex << 16) | this->mTemplateIndex;

			ASSERT(this->mTemplateIndex < 256);
			this->pev->fuser1 = (theGenEntityIndex << 16) | ((this->mTemplateIndex & 0xFF) << 8);
			this->pev->fuser2 = this->mHandle;

			// Store our custom data
			this->SetCustomData(this->mCustomData);
			
			this->mTimeParticlesCreated = gpGlobals->time;
		}
		else
		{
			// Destroy server side particle system
			//AvHParticleSystemManager::Instance()->DestroyParticleSystemIfNotDestroyed(this->entindex(), this->mHandle);
			AvHParticleSystemManager::Instance()->MarkParticleSystemForDeletion(this->entindex(), this->mHandle);
			
			// Replicate action to everyone
			this->pev->iuser3 = AVH_USER3_PARTICLE_OFF;
			this->pev->fuser1 = this->mHandle;
		}
	}
	
	if(this->mIsOn)
	{
		AvHParticleTemplate* theTemplate = gParticleTemplateList.GetTemplateAtIndex(this->mTemplateIndex);
		ASSERT(theTemplate);
		
		// If particle system was set to expire, make sure to realize when it turns off
		int theParticleSystemLifetime = theTemplate->GetParticleSystemLifetime();
		if(theParticleSystemLifetime != -1)
		{
			if(gpGlobals->time - this->mTimeParticlesCreated >= theParticleSystemLifetime)
			{
				//this->mIsOn = false;
				this->mUseState = false;
			}
		}
	}
}




// Create a custom particle system.  Create a new template instead of reading
// one off disk.
void
AvHCustomParticleSystemEntity::KeyValue( KeyValueData* inPkvd )
{
	// Read tag with ps name and create a new template using it
	if(FStrEq(inPkvd->szKeyName, kpscSystemName))
	{
		// Custom particle systems shouldn't be specifying a system in the .ps to use
		ASSERT(false);
	}
	else if(FStrEq(inPkvd->szKeyName, kpscGenSource) || FStrEq(inPkvd->szKeyName, "particleGenerationSource"))
	{
		char* theEntityName = inPkvd->szValue;
		this->GetCustomTemplate()->SetGenerationEntityName(theEntityName);
		inPkvd->fHandled = TRUE;
	}
	else if(FStrEq(inPkvd->szKeyName, kpscGenShape) || FStrEq(inPkvd->szKeyName, "particleGenerationShape"))
	{
		int theGenerationShape = 0;
		if(sscanf(inPkvd->szValue, "%d", &theGenerationShape))
		{
			ShapeType theShape = PS_Point;
			switch(theGenerationShape)
			{
			case 4:
				theShape = PS_Box;
				break;
			case 5:
				theShape = PS_Sphere;
				break;
			case 8:
				theShape = PS_Blob;
				break;
			}
			this->GetCustomTemplate()->SetGenerationShape(theShape);
			inPkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(inPkvd->szKeyName, kpscSprite) || FStrEq(inPkvd->szKeyName, "particleSprite"))
	{								   
		// relative path to sprite
		string theSpriteName(inPkvd->szValue);
		this->GetCustomTemplate()->SetSprite(theSpriteName);
		inPkvd->fHandled = TRUE;
	}
	else if(FStrEq(inPkvd->szKeyName, kpscSpriteNumFrames) || FStrEq(inPkvd->szKeyName, "particleSpriteNumFrames"))
	{
		// int number of frames
		int theNumFrames = 0;
		if(sscanf(inPkvd->szValue, "%d", &theNumFrames) == 1)
		{
			this->GetCustomTemplate()->SetNumSpriteFrames(theNumFrames);
			inPkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(inPkvd->szKeyName, kpscGenShapeParams) || FStrEq(inPkvd->szKeyName, "particleGenerationShapeParams"))
	{
		float theParameter = 0;
		if(sscanf(inPkvd->szValue, "%f", &theParameter) == 1)
		{
			this->GetCustomTemplate()->SetGenerationEntityParameter(theParameter);
			inPkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(inPkvd->szKeyName, kpscNumParticles) || FStrEq(inPkvd->szKeyName, "particleNumParticles"))
	{
		// max particles, or density
		int theNumParticles = 0;
		if(sscanf(inPkvd->szValue, "%d", &theNumParticles) == 1)
		{
		 	this->GetCustomTemplate()->SetMaxParticles(theNumParticles);
			inPkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(inPkvd->szKeyName, kpscGenRate) || FStrEq(inPkvd->szKeyName, "particleGenerationRate"))
	{
		// num particles per second
		int theNumParticles = 0;
		if(sscanf(inPkvd->szValue, "%d", &theNumParticles) == 1)
		{
			this->GetCustomTemplate()->SetGenerationRate(theNumParticles);
			inPkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(inPkvd->szKeyName, kpscSize) || FStrEq(inPkvd->szKeyName, "particleSize"))
	{
		// float particle size
		float theParticleSize = 0;
		if(sscanf(inPkvd->szValue, "%f", &theParticleSize) == 1)
		{
			this->GetCustomTemplate()->SetParticleSize(theParticleSize);
			inPkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(inPkvd->szKeyName, kpscSystemLifetime) || FStrEq(inPkvd->szKeyName, "particleSystemLifetime"))
	{
		// string system lifetime
		float theLifetime = -1;
		if(sscanf(inPkvd->szValue, "%f", &theLifetime) == 1)
		{
			this->GetCustomTemplate()->SetParticleSystemLifetime(theLifetime);
			inPkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(inPkvd->szKeyName, kpscAnimationSpeed) || FStrEq(inPkvd->szKeyName, "particleAnimationSpeed"))
	{
		// Sprite animation speed
		float theAnimSpeed = 1.0f;
		if(sscanf(inPkvd->szValue, "%f", &theAnimSpeed) == 1)
		{
			this->GetCustomTemplate()->SetAnimationSpeed(theAnimSpeed);
			inPkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(inPkvd->szKeyName, kpscParticleLifetime) || FStrEq(inPkvd->szKeyName, "particleLifetime"))
	{
		// string particle lifetime
		float theLifetime = -1;
		if(sscanf(inPkvd->szValue, "%f", &theLifetime) == 1)
		{
			this->GetCustomTemplate()->SetParticleLifetime(theLifetime);
			inPkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(inPkvd->szKeyName, kpscVelocityShape) || FStrEq(inPkvd->szKeyName, "particleStartingVelocityShape"))
	{
		int theVelocityShape = 0;
		if(sscanf(inPkvd->szValue, "%d", &theVelocityShape))
		{
			ShapeType theShape = PS_Point;
			switch(theVelocityShape)
			{
			case 1:
				theShape = PS_Point;
				break;
			case 2:
				theShape = PS_Box;
				break;
			case 3:
				theShape = PS_Sphere;
				break;
			case 4:
				theShape = PS_Blob;
				break;
			}
			this->GetCustomTemplate()->SetStartingVelocityShape(theShape);
			inPkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(inPkvd->szKeyName, kpscVelocityParams) || FStrEq(inPkvd->szKeyName, "particleStartingVelParams"))
	{
		// string, 8 comma-delimited parms
		ParticleParams theVelParms;
		if(sscanf(inPkvd->szValue, "%d,%d,%d,%d,%d,%d,%d,%d", theVelParms + 0, theVelParms + 1, theVelParms + 2, theVelParms + 3, theVelParms + 4, theVelParms + 5, theVelParms + 6, theVelParms + 7) == 8)
		{
			this->GetCustomTemplate()->SetStartingVelocityParams(theVelParms);
			inPkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(inPkvd->szKeyName, kpscScale) || FStrEq(inPkvd->szKeyName, "particleScaling"))
	{
		// float/string particle scaling
		float theScaling = 1.0f;
		if(sscanf(inPkvd->szValue, "%f", &theScaling) == 1)
		{
			this->GetCustomTemplate()->SetParticleScaling(theScaling);
			inPkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(inPkvd->szKeyName, kpscRendermode) || FStrEq(inPkvd->szKeyName, "particleRenderMode"))
	{
		// 0-5 render mode
		int theRenderMode = 0;
		if(sscanf(inPkvd->szValue, "%d", &theRenderMode) == 1)
		{
			this->GetCustomTemplate()->SetRenderMode(theRenderMode);
			inPkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(inPkvd->szKeyName, kpscMaxAlpha) || FStrEq(inPkvd->szKeyName, "particleMaxAlpha"))
	{
		float theMaxAlpha = 1.0f;
		if(sscanf(inPkvd->szValue, "%f", &theMaxAlpha) == 1)
		{
			this->GetCustomTemplate()->SetMaxAlpha(theMaxAlpha);
			inPkvd->fHandled = TRUE;
		}
	}
	else if(FStrEq(inPkvd->szKeyName, kpscSystemToGen) || FStrEq(inPkvd->szKeyName, "particleSystemToGenerate"))
	{
		string theSystemToGenerate = inPkvd->szValue;
		this->GetCustomTemplate()->SetParticleSystemToGenerate(theSystemToGenerate);
		inPkvd->fHandled = TRUE;
	}
	else
	{
		// Call down to base class
		AvHParticleSystemEntity::KeyValue(inPkvd);
	}
}

