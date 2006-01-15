#include "mod/AvHReinforceable.h"
#include "mod/AvHServerUtil.h"
#include "mod/AvHSharedUtil.h"

AvHReinforceable::AvHReinforceable()
{
	this->mReinforcingPlayer = -1;
	this->mLastTimeReinforced = -1;
}

void AvHReinforceable::CueRespawnEffect(AvHPlayer* inPlayer)
{
	// Nothing by default, but children can override
}

void AvHReinforceable::ResetEntity(void)
{
	this->mReinforcingPlayer = -1;
	this->mLastTimeReinforced = 0;
}

void AvHReinforceable::UpdateReinforcements()
{
	if(this->GetCanReinforce())
	{
		// If this portal isn't reinforcing a player
		if(this->mReinforcingPlayer == -1)
		{
			// if there are any reinforcements
			//if(this->mNumReinforcements > 0)
			//{
				// Find player on this team that's been waiting the longest
				AvHPlayer* thePlayerToSpawn = NULL;
		
				FOR_ALL_ENTITIES(kAvHPlayerClassName, AvHPlayer*);
				if(theEntity->GetTeam() == this->GetReinforceTeamNumber())
				{
					if(theEntity->GetPlayMode() == PLAYMODE_AWAITINGREINFORCEMENT)
					{
						if(!thePlayerToSpawn || (theEntity->GetTimeLastPlaying() < thePlayerToSpawn->GetTimeLastPlaying()))
						{
							thePlayerToSpawn = theEntity;
						}
					}
				}
				END_FOR_ALL_ENTITIES(kAvHPlayerClassName);
				
				if(thePlayerToSpawn)
				{
					// Decrement reinforcements
					//					this->mNumReinforcements--;
					
					// Pay for points if possible
					//					int theSpawnCost = GetGameRules()->GetGameplay().GetMarineRespawnCost();
					//					int theNewResources = max(0, (thePlayerToSpawn->GetResources() - theSpawnCost));
					//					thePlayerToSpawn->SetResources(theNewResources);
					
					// Set the player's mode to PLAYMODE_REINFORCING
					this->mReinforcingPlayer = thePlayerToSpawn->entindex();
					
					thePlayerToSpawn->SetPlayMode(PLAYMODE_REINFORCING);
				}
				//}
				// else, if we're not in tourny mode
				//else if(!GetGameRules()->GetIsTournamentMode())
				//{
				// TODO: research reinforcements on their own when there are people waiting
				//}
		}
		else
		{
			// Is player still valid, or has he left the server/team?
			bool thePlayerHasLeft = true;
			AvHPlayer* theReinforcingPlayer = this->GetReinforcingPlayer();
			if(theReinforcingPlayer && (theReinforcingPlayer->GetTeam() == this->GetReinforceTeamNumber()))
			{
				if(theReinforcingPlayer->GetPlayMode() == PLAYMODE_REINFORCING)
				{
					float theRespawnTime = this->GetReinforceTime();

					// Has enough time passed to bring the player in?
					if((gpGlobals->time > (theReinforcingPlayer->GetTimeLastPlaying() + theRespawnTime)) && (gpGlobals->time > (this->mLastTimeReinforced + theRespawnTime)))
					{
						this->ResetReinforcingPlayer(true);
					}

					thePlayerHasLeft = false;
				}
			}

			if(thePlayerHasLeft)
			{
				this->ResetReinforcingPlayer(false);
			}
		}
	}
}

AvHPlayer* AvHReinforceable::GetReinforcingPlayer()
{
	AvHPlayer* thePlayer = NULL;
	if(this->mReinforcingPlayer != -1)
	{
		thePlayer = dynamic_cast<AvHPlayer*>(CBaseEntity::Instance(g_engfuncs.pfnPEntityOfEntIndex(this->mReinforcingPlayer)));
	}
	
	return thePlayer;
}

float AvHReinforceable::GetReinforceTime() const
{
	return 15.0f;
}

void AvHReinforceable::ResetReinforcingPlayer(bool inSuccess)
{
	if(this->mReinforcingPlayer != -1)
	{
		AvHPlayer* thePlayer = this->GetReinforcingPlayer();
		if(thePlayer)
		{
			// If inSuccess is true, spawn player into the world
			if(inSuccess)
			{
				Vector theLocation;
				if(this->GetSpawnLocationForPlayer(thePlayer, theLocation))
				{
					// set the playmode and reset internal state to reinforce again
					thePlayer->SetPlayMode(PLAYMODE_PLAYING);
					
					VectorCopy(theLocation, thePlayer->pev->origin);
			
					this->CueRespawnEffect(thePlayer);

					this->mLastTimeReinforced = gpGlobals->time;
				}
			}
			// else, set player as reinforcement (hive was set inactive before reinforcement finished)
			else
			{
				AvHPlayMode thePlayMode = thePlayer->GetPlayMode();
				if(thePlayMode == PLAYMODE_REINFORCING)
				{
					thePlayer->SetPlayMode(PLAYMODE_AWAITINGREINFORCEMENT);
				}
			}
		}
		
		this->mReinforcingPlayer = -1;
	}
}
