#include "cl_dll/hud.h"
#include "cl_dll/cl_util.h"
#include "common/const.h"
#include "common/entity_state.h"
#include "common/cl_entity.h"
#include "ui/UITags.h"
#include "mod/AvHOverviewMap.h"
#include "mod/AvHSharedUtil.h"
#include "mod/AvHSpecials.h"
#include "mod/AvHMiniMap.h"
#include "ui/UIUtil.h"
#include "mod/AvHPlayerUpgrade.h"
#include "mod/AvHSpriteAPI.h"
#include "mod/AvHSprites.h"
#include "mod/AvHClientVariables.h"

using std::string;


class DrawingOrderSort
{

public:
	
	bool operator()(const DrawableEntity& entity1, const DrawableEntity& entity2)
	{
		
        // Draw resource nodes all the way on the bottom.

		if (entity1.mUser3 == AVH_USER3_FUNC_RESOURCE)
        {
            if (entity2.mUser3 == AVH_USER3_FUNC_RESOURCE)
            {
                return entity1.mEntityNumber > entity2.mEntityNumber;
            }
            else
            {
                return true;
            }
        }
		else if (entity2.mUser3 == AVH_USER3_FUNC_RESOURCE)
        {
            return false;
        }

        // Draw the local player on top of everything.

		if (entity1.mIsLocalPlayer)
		{
			return false;
		}
		else if (entity2.mIsLocalPlayer)
		{
			return true;
        }
		
        // Draw players on top of structures.

        return (entity1.mEntityNumber > entity2.mEntityNumber);
		
	}
	
};

AvHOverviewMap::AvHOverviewMap()
{
	this->Init();
}

void AvHOverviewMap::Init()
{
	this->mUser3 = AVH_USER3_NONE;
    this->mTeam  = TEAM_IND;

	this->mDrawableEntityList.clear();

	// Approximately 1/max world dimension
	this->mMapExtents.ResetMapExtents();

	this->mMiniMapSprite = 0;
	this->mReticleSprite = 0;

	mLastUpdateTime = 0;
}

void AvHOverviewMap::Clear()
{
	this->Init();
}

void AvHOverviewMap::GetSpriteForEntity(const DrawableEntity& entity, int& outSprite, int& outFrame, int& outRenderMode)
{
    
    outRenderMode = kRenderTransTexture;
    
    if ((this->mUser3 == AVH_USER3_COMMANDER_PLAYER) || (entity.mUser3 == AVH_USER3_UNKNOWN))
    {
        outSprite = Safe_SPR_Load(kCommBlipSprite);
        outFrame  = 0;
    }
    else
    {
        gHUD.GetSpriteForUser3(entity.mUser3, outSprite, outFrame, outRenderMode);
    }

}

void AvHOverviewMap::GetColorForEntity(const DrawableEntity& entity, float& outR, float& outG, float& outB)
{

    if (entity.mUser3 == AVH_USER3_WAYPOINT)
    {
        outR = 0.1;
	    outG = 0.8;
		outB = 1.0;
    }
    else if (entity.mTeam == TEAM_IND)
    {
        
        if (entity.mUser3 == AVH_USER3_WELD)
        {
            outR = 1.0;
            outG = 0.7;
            outB = 0.0;
        }
        else
        {
            outR = 0.5;
            outG = 0.5;
            outB = 0.5;
        }
    }
    else
    {
        if (entity.mTeam == mTeam)
        {

            if (entity.mUser3 == AVH_USER3_HIVE ||
                entity.mUser3 == AVH_USER3_COMMANDER_STATION ||
                entity.mUser3 == AVH_USER3_TURRET_FACTORY ||
                entity.mUser3 == AVH_USER3_ARMORY ||
                entity.mUser3 == AVH_USER3_ADVANCED_ARMORY ||
                entity.mUser3 == AVH_USER3_ARMSLAB ||
                entity.mUser3 == AVH_USER3_PROTOTYPE_LAB ||
                entity.mUser3 == AVH_USER3_OBSERVATORY ||
                entity.mUser3 == AVH_USER3_TURRET ||
                entity.mUser3 == AVH_USER3_SIEGETURRET ||
                entity.mUser3 == AVH_USER3_RESTOWER ||
                entity.mUser3 == AVH_USER3_INFANTRYPORTAL ||
                entity.mUser3 == AVH_USER3_PHASEGATE ||
	            entity.mUser3 == AVH_USER3_DEFENSE_CHAMBER ||
	            entity.mUser3 == AVH_USER3_MOVEMENT_CHAMBER  ||
	            entity.mUser3 == AVH_USER3_OFFENSE_CHAMBER  ||
	            entity.mUser3 == AVH_USER3_SENSORY_CHAMBER ||
	            entity.mUser3 == AVH_USER3_ALIENRESTOWER ||
	            entity.mUser3 == AVH_USER3_ADVANCED_TURRET_FACTORY)
            {
                outR = 0.5;
	    	    outG = 0.8;
		        outB = 1.0;
            }
            else
            {

    		    outR = 1.0;
	    	    outG = 1.0;
		        outB = 1.0;

                // Color squads.

                int localPlayerSquad;
                
                if (g_iUser1 == OBS_NONE)
                {
                    localPlayerSquad = gHUD.GetCurrentSquad();
                }
                else
                {
                    // We don't have access to the squad information for player's
                    // we're spectating.
                    localPlayerSquad = 0;
                }

                if (mUser3 != AVH_USER3_COMMANDER_PLAYER)
                {
                    if (entity.mIsLocalPlayer || 
                        (entity.mSquadNumber != 0 &&
                         entity.mSquadNumber == localPlayerSquad))
                    {
    		            outR = 0.5;
	    	            outG = 1.0;
		                outB = 0.5;
                    }
                }

            }

        }
        else
        {
		    outR = 1.0;
		    outG = 0.1;
		    outB = 0.0;
        }
    }

}

void AvHOverviewMap::DrawMiniMapEntity(const DrawInfo& inDrawInfo, const DrawableEntity& inEntity)
{
	
    if (!GetHasData())
	{
        return;
    }

	float theEntityPosX = inEntity.mX;
	float theEntityPosY = inEntity.mY;

	// Draw local player smoothly and predicted.

	if (inEntity.mIsLocalPlayer)
	{
		theEntityPosX = this->mWorldPlayerX;
		theEntityPosY = this->mWorldPlayerY;
	}

	int theSprite = 0;
    int theFrame = 0;
    int theRenderMode;

	GetSpriteForEntity(inEntity, theSprite, theFrame, theRenderMode);
	
    if (theSprite > 0)
	{
        
        int theSprWidth = SPR_Width(theSprite, theFrame);
		int theSprHeight = SPR_Height(theSprite, theFrame);

        int theX = inDrawInfo.mX;
		int theY = inDrawInfo.mY;

        int theWidth  = inDrawInfo.mWidth;
		int theHeight = inDrawInfo.mHeight;

        float viewWorldXSize = inDrawInfo.mViewWorldMaxX - inDrawInfo.mViewWorldMinX;
        float viewWorldYSize = inDrawInfo.mViewWorldMaxY - inDrawInfo.mViewWorldMinY;

        float scale = 0.75; // How much to scale the sprite.

        bool isPlayer = inEntity.mUser3 == AVH_USER3_MARINE_PLAYER || inEntity.mUser3 == AVH_USER3_HEAVY; //heavy used for player in minimap system
		bool theIsWaypoint = inEntity.mUser3 == AVH_USER3_WAYPOINT;
	
        float w = theSprWidth * scale;
        float h = theSprHeight * scale;

        float entityMiniMapX = theX + ((theEntityPosX - inDrawInfo.mViewWorldMinX) / viewWorldXSize) * theWidth;
        float entityMiniMapY = theY + ((inDrawInfo.mViewWorldMaxY - theEntityPosY) / viewWorldYSize) * theHeight;

        float x = entityMiniMapX - w / 2.0f;
        float y = entityMiniMapY - h / 2.0f;
        
        if (theIsWaypoint)
        {
                
            float theFractionalLastUpdate = mLastUpdateTime - (int)mLastUpdateTime;
        
            if (theFractionalLastUpdate < .25f) 
            {
    	        return;
            }

        }

        // Perform gross culling of sprites.
        if (x + w >= theX && y + h >= theY && x < theX + theWidth && y < theY + theHeight)
        {

            float r, g, b;
            GetColorForEntity(inEntity, r, g, b);

            AvHSpriteSetColor(r, g, b);
            
            // If it's the local player, draw the FOV.

            if (inEntity.mIsLocalPlayer && mUser3 != AVH_USER3_COMMANDER_PLAYER)
            {
                
                int theSprite = Safe_SPR_Load("sprites/fov.spr");
                int theFrame  = 0;

                int theSprWidth = SPR_Width(theSprite, theFrame);
		        int theSprHeight = SPR_Height(theSprite, theFrame);

                float w2 = theSprWidth * scale;
                float h2 = theSprHeight * scale;

                float x2 = entityMiniMapX;
                float y2 = entityMiniMapY - h2 / 2;

                AvHSpriteSetRotation(-inEntity.mAngleRadians * 180 / M_PI, x2, y2 + h2 / 2);
                
                AvHSpriteSetColor(1, 1, 1);
                AvHSpriteSetRenderMode(kRenderTransAdd);
                AvHSpriteDraw(theSprite, theFrame, x2, y2, x2 + w2, y2 + h2, 0, 0, 1, 1);

            }

		    if (mUser3 != AVH_USER3_COMMANDER_PLAYER)
			{
				AvHSpriteSetRotation(-inEntity.mAngleRadians * 180 / M_PI, x + w / 2, y + h / 2);
			}
			else
			{
				AvHSpriteSetRotation(0, 0, 0);
			}

            AvHSpriteSetColor(r, g, b);
            AvHSpriteSetRenderMode(theRenderMode);
            AvHSpriteDraw(theSprite, theFrame, x, y, x + w, y + h, 0, 0, 1, 1);

        }

        if (isPlayer || theIsWaypoint)
        {

            const float border = 2;

            if (!(x + w / 2 >= theX && y + h / 2 >= theY && x + w / 2 < theX + theWidth && y + h / 2 < theY + theHeight))
            {

                // Draw friendly players as little arrows on the edge of the minimap.

                int theSprite = Safe_SPR_Load(kMarinePlayersSprite);
				int theFrame  = theIsWaypoint ? 4 : 3;

                ASSERT(theSprite != 0);

                int theSprWidth = SPR_Width(theSprite, theFrame);
			    int theSprHeight = SPR_Height(theSprite, theFrame);

                float tipX = entityMiniMapX;
                float tipY = entityMiniMapY;

                if (tipX < theX + border) tipX = theX + border;
                if (tipY < theY + border) tipY = theY + border;
                if (tipX > theX + theWidth - border)  tipX = theX + theWidth - border;
                if (tipY > theY + theHeight - border) tipY = theY + theHeight - border;

                float dx = tipX - entityMiniMapX;
                float dy = tipY - entityMiniMapY;

                float angle = atan2(dy, dx) * 180 / M_PI;

                w = theSprWidth;
                h = theSprHeight;

                int renderMode = kRenderTransTexture;

                if (theIsWaypoint)
                {
                    renderMode = kRenderTransAdd;
                }

                AvHSpriteSetRenderMode(renderMode);
                
                float r, g, b;
                GetColorForEntity(inEntity, r, g, b);

                AvHSpriteSetColor(r, g, b);
                AvHSpriteSetRotation(angle, tipX, tipY);
                AvHSpriteDraw(theSprite, theFrame, tipX, tipY - h / 2 , tipX + w, tipY + h / 2, 0, 0, 1, 1);

            }

        }
	
    }

}

bool AvHOverviewMap::GetHasData() const
{
	return this->mDrawableEntityList.size() > 0;
}


void AvHOverviewMap::GetWorldPosFromMouse(const DrawInfo& inDrawInfo, int inMouseX, int inMouseY, float& outWorldX, float& outWorldY)
{

    float viewWorldXSize = inDrawInfo.mViewWorldMaxX - inDrawInfo.mViewWorldMinX;
    float viewWorldYSize = inDrawInfo.mViewWorldMaxY - inDrawInfo.mViewWorldMinY;

    outWorldX = ((float)(inMouseX) / inDrawInfo.mWidth) * viewWorldXSize + inDrawInfo.mViewWorldMinX;
    outWorldY =  inDrawInfo.mViewWorldMaxY - ((float)(inMouseY) / inDrawInfo.mHeight) * viewWorldYSize;

}


void AvHOverviewMap::KillOldAlerts(float inCurrentTime)
{
	for(MapAlertListType::iterator theIter = this->mAlertList.begin(); theIter != this->mAlertList.end(); /* no inc */)
	{
		if(inCurrentTime > theIter->mExpireTime)
		{
			theIter = this->mAlertList.erase(theIter);
		}
		else
		{
			++theIter;
		}
	}
}

void AvHOverviewMap::DrawMiniMap(const DrawInfo& inDrawInfo)
{

	// puzl: 1064
	// Use labelled minimaps if cl_labelmaps is 1

    // Load the mini-map sprite if it's not already loaded.
	static string lastMiniMapName="";
	if ( mMapName != "") {
		int drawLabels=CVAR_GET_FLOAT(kvLabelMaps);
		string theMiniMapName = AvHMiniMap::GetSpriteNameFromMap(ScreenWidth(), mMapName, drawLabels);
		if ( lastMiniMapName != theMiniMapName )
		{
			mMiniMapSprite = Safe_SPR_Load(theMiniMapName.c_str());

			// We want to preserve the last minimap even if we fail.  There's no point in failing again until the player
			// changes the value of the cvar.
			lastMiniMapName=theMiniMapName;

			// Draw normal minimap if no labelled map exists ( for custom maps )
			if ( !mMiniMapSprite && drawLabels ) {
				theMiniMapName = AvHMiniMap::GetSpriteNameFromMap(ScreenWidth(), mMapName, 0);
				mMiniMapSprite = Safe_SPR_Load(theMiniMapName.c_str());
			}
		}
	}
	// :puzl
    if (!mMiniMapSprite)
    {
        return;
    }

    float mapXSize = mMapExtents.GetMaxMapX() - mMapExtents.GetMinMapX();
    float mapYSize = mMapExtents.GetMaxMapY() - mMapExtents.GetMinMapY();

    float mapXCenter = (mMapExtents.GetMaxMapX() + mMapExtents.GetMinMapX()) / 2;
    float mapYCenter = (mMapExtents.GetMaxMapY() + mMapExtents.GetMinMapY()) / 2;

	float aspectRatio = mapXSize / mapYSize;
	
    float xScale;
    float yScale;

    if(mapXSize > mapYSize)
	{
		xScale = 1.0f;
		yScale = mapYSize / mapXSize;	
	}
	else
	{
		xScale = mapYSize / mapXSize;
		yScale = 1.0f;
	}


    float x1 = mapXCenter - mapXSize * xScale / 2;
    float y1 = mapYCenter + mapYSize * yScale / 2;

    WorldToMiniMapCoords(inDrawInfo, x1, y1);

    float x2 = mapXCenter + mapXSize * xScale / 2;
    float y2 = mapYCenter - mapYSize * yScale / 2;

    WorldToMiniMapCoords(inDrawInfo, x2, y2);

    AvHSpriteSetRenderMode(kRenderTransTexture);
    AvHSpriteSetRotation(0, 0, 0);

    // TODO this should be based on a flag not the user3
    
    if (mUser3 == AVH_USER3_COMMANDER_PLAYER)
    {
        // Use the small map if it's the commander view.
        AvHSpriteDraw(mMiniMapSprite, 4, x1, y1, x2, y2, 0, 0, 1, 1);
    }
    else
    {
        AvHSpriteDrawTiles(mMiniMapSprite, 2, 2, x1, y1, x2, y2,
            0, 0, 1, 1);
    }

	// Reset orientation after potentially orienting above to make sure it doesn't affect subsequent code
	AvHSpriteSetRotation(0, 0, 0);
}

void AvHOverviewMap::WorldToMiniMapCoords(const DrawInfo& inDrawInfo, float& x, float& y)
{

    float viewWorldXSize = inDrawInfo.mViewWorldMaxX - inDrawInfo.mViewWorldMinX;
    float viewWorldYSize = inDrawInfo.mViewWorldMaxY - inDrawInfo.mViewWorldMinY;

    x = inDrawInfo.mX + ((x - inDrawInfo.mViewWorldMinX) / viewWorldXSize) * inDrawInfo.mWidth;
    y = inDrawInfo.mY + ((inDrawInfo.mViewWorldMaxY - y) / viewWorldYSize) * inDrawInfo.mHeight;

}

void AvHOverviewMap::DrawAlerts(const DrawInfo& inDrawInfo)
{

    int theX = inDrawInfo.mX;
    int theY = inDrawInfo.mY;

    int theWidth  = inDrawInfo.mWidth;
    int theHeight = inDrawInfo.mHeight;
    
    AvHSpriteEnableClippingRect(true);
    AvHSpriteSetClippingRect(theX, theY, theX + theWidth, theY + theHeight);

    int theSprite = Safe_SPR_Load(kAlertSprite);
	int theFrame  = 0;

    ASSERT(theSprite != 0);

    int theSprWidth = SPR_Width(theSprite, theFrame);
	int theSprHeight = SPR_Height(theSprite, theFrame);

    float viewWorldXSize = inDrawInfo.mViewWorldMaxX - inDrawInfo.mViewWorldMinX;
    float viewWorldYSize = inDrawInfo.mViewWorldMaxY - inDrawInfo.mViewWorldMinY;

    for (unsigned int i = 0; i < mAlertList.size(); ++i)
    {
        
        float maxAlertSize = 5;
		float minAlertSize = 0.4;

		float transitionTime = 1;

        float dt = (mLastUpdateTime - mAlertList[i].mStartTime) / transitionTime;

        if (dt > 1) dt = 1;

        float scale = (1 - sqrt(dt)) * (maxAlertSize - minAlertSize) + minAlertSize;

        float w = theSprWidth * scale;
        float h = theSprHeight * scale;
        
        float cx = mAlertList[i].mX;
        float cy = mAlertList[i].mY;
        
        WorldToMiniMapCoords(inDrawInfo, cx, cy); 

        float angle = dt * 180;

		float fadeOutStartTime = mAlertList[i].mExpireTime - (transitionTime / 2);
		float alpha = 1 - (mLastUpdateTime - fadeOutStartTime) / (transitionTime / 2);

		if (alpha < 0)
		{
			alpha = 0;
		}

        AvHSpriteSetRenderMode(kRenderTransAdd);
        AvHSpriteSetColor(1, 1, 1, alpha * 0.1);
        AvHSpriteSetRotation(angle, cx, cy);
        AvHSpriteDraw(theSprite, theFrame, cx - w / 2, cy - h / 2, cx + w / 2, cy + h / 2, 0, 0, 1, 1);
    }
    
}

void AvHOverviewMap::AddAlert(float x, float y)
{
    
    MapAlert alert;

    alert.mStartTime  = mLastUpdateTime;
    alert.mExpireTime = mLastUpdateTime + BALANCE_VAR(kAlertExpireTime) / 5;
    
    alert.mX = x;
    alert.mY = y;
    
    mAlertList.push_back(alert);

}

void AvHOverviewMap::Draw(const DrawInfo& inDrawInfo)
{

    int theX = inDrawInfo.mX;
    int theY = inDrawInfo.mY;
    int theCompWidth  = inDrawInfo.mWidth;
    int theCompHeight = inDrawInfo.mHeight;

    AvHSpriteBeginFrame();

    AvHSpriteEnableClippingRect(true);
    AvHSpriteSetClippingRect(theX, theY, theX + theCompWidth, theY + theCompHeight);
    
    // Draw the minimap background.

    DrawMiniMap(inDrawInfo);
    
    // Draw the entities on the minimap.

    if (mUser3 == AVH_USER3_COMMANDER_PLAYER)
    {
        AvHSpriteEnableClippingRect(false);
    }

    for (DrawableEntityListType::const_iterator theIter = this->mDrawableEntityList.begin(); theIter != this->mDrawableEntityList.end(); theIter++)
	{
        DrawMiniMapEntity(inDrawInfo, *theIter);
	}

    // Draw the way points as entities.
    
    {
    
        for (MapOrderListType::const_iterator theIter = mMapOrderList.begin(); theIter != mMapOrderList.end(); theIter++)
	    {
            DrawableEntity drawableEntity;
        
            drawableEntity.mUser3 = AVH_USER3_WAYPOINT;
            drawableEntity.mX = theIter->mX;
            drawableEntity.mY = theIter->mY;
        
            DrawMiniMapEntity(inDrawInfo, drawableEntity);
	    }
    
    }

    // Draw the alerts.

    DrawAlerts(inDrawInfo);
  
    // Draw the reticle.

	if(this->mUser3 == AVH_USER3_COMMANDER_PLAYER)
	{
		
        int theFrame = 0;
		
        if (!this->mReticleSprite)
		{
			this->mReticleSprite = Safe_SPR_Load(kReticleSprite);
		}

		if (this->mReticleSprite)
		{

            float x = mWorldPlayerX;
            float y = mWorldPlayerY;

            WorldToMiniMapCoords(inDrawInfo, x, y);

            float w = SPR_Width(this->mReticleSprite, theFrame);
            float h = SPR_Height(this->mReticleSprite, theFrame);

            AvHSpriteSetRenderMode(kRenderTransAdd);
            AvHSpriteSetColor(1, 1, 1);
			AvHSpriteSetRotation(0, 0, 0);
            AvHSpriteDraw(mReticleSprite, theFrame, x - w / 2, y - h / 2, x + w / 2, y + h / 2, 0, 0, 1, 1);

		}
    }

	// Reset orientation after potentially orienting above to make sure it doesn't affect subsequent code
	AvHSpriteSetRotation(0, 0, 0);

    AvHSpriteEndFrame();

}

int AvHOverviewMap::GetEntityAtWorldPosition(float inWorldX, float inWorldY, float inRadius) const
{

	for (int i = 0; i < (int)mDrawableEntityList.size(); ++i)
	{
                
        float dx = mDrawableEntityList[i].mX - inWorldX;
        float dy = mDrawableEntityList[i].mY - inWorldY;
        
        if (dx * dx + dy * dy < inRadius * inRadius)
        {
            return mDrawableEntityList[i].mEntityNumber;
        }
	
	}    

    return 0;

}

void AvHOverviewMap::SetMapExtents(const string& inMapName, const AvHMapExtents& inMapExtents)
{
	this->mMapName = inMapName;
	this->mMapExtents = inMapExtents;
}

void AvHOverviewMap::GetMapExtents(AvHMapExtents& outMapExtents)
{
    outMapExtents = mMapExtents;
}

void AvHOverviewMap::SetUser3(AvHUser3 inUser3)
{
	this->mUser3 = inUser3;
}

void AvHOverviewMap::SetWorldPosition(float inPlayerX, float inPlayerY)
{
	mWorldPlayerX = inPlayerX;
	mWorldPlayerY = inPlayerY;
}

void AvHOverviewMap::GetWorldPosition(float& outWorldX, float& outWorldY)
{
    outWorldX = mWorldPlayerX;
    outWorldY = mWorldPlayerY;
}


void AvHOverviewMap::Update(float inCurrentTime)
{

    mLastUpdateTime = inCurrentTime;

//	if(gHUD.GetGameStarted())
//	{
//		this->mMap.Update(inCurrentTime, this->mMapExtents.GetMinMapX(), this->mMapExtents.GetMinMapY(), this->mMapExtents.GetMaxMapX(), this->mMapExtents.GetMaxMapY(), this->mViewHeight);
//	}

	// Get player data from engine and store for use during draw
	this->UpdateDrawData(inCurrentTime);

	// Kill off any old alerts
	this->KillOldAlerts(inCurrentTime);
}

const float kPositionNetworkConstant = 2.0f;

void AvHOverviewMap::UpdateDrawData(float inCurrentTime)
{
    
    int theLocalPlayerIndex;
    
    if (g_iUser1 == OBS_NONE)
    {
	    cl_entity_s* thePlayer = gEngfuncs.GetLocalPlayer();
	    theLocalPlayerIndex = thePlayer->index;    
    }
    else
    {
        theLocalPlayerIndex = g_iUser2;
    }
    
    cl_entity_s* thePlayer = gEngfuncs.GetEntityByIndex(theLocalPlayerIndex);
    mTeam = (AvHTeamNumber)(thePlayer->curstate.team);

	// Clear list of drawable entities 
	this->mDrawableEntityList.clear();
	
	// Get all entities
	MapEntityMap theEntityList;
	gHUD.GetEntityHierarchy().GetEntityInfoList(theEntityList);

	// For each entity
	for(MapEntityMap::iterator theIter = theEntityList.begin(); theIter != theEntityList.end(); theIter++)
	{
		// If the player has no leader, then he IS a leader
		int theCurrentPlayerIndex = theIter->first;
		bool theIsLocalPlayer = (theLocalPlayerIndex == theCurrentPlayerIndex);

		DrawableEntity theDrawableEntity;
		theDrawableEntity.mEntityNumber = theIter->first;

        theDrawableEntity.mX = theIter->second.mX;
        theDrawableEntity.mY = theIter->second.mY;
        theDrawableEntity.mUser3  = theIter->second.mUser3;
        theDrawableEntity.mTeam   = theIter->second.mTeam;
        theDrawableEntity.mAngleRadians = theIter->second.mAngle * M_PI / 180;
        theDrawableEntity.mSquadNumber  = theIter->second.mSquadNumber;

		// Returns position relative to minimap, so add it back in
//				commented this out here, commented out corresponding shift in AvHEntityHierarchy::BuildFromTeam at line 234
//		theDrawableEntity.mX += this->mMapExtents.GetMinMapX();
//		theDrawableEntity.mY += this->mMapExtents.GetMinMapY();
		theDrawableEntity.mIsLocalPlayer = theIsLocalPlayer;

        // Get additional information about the entity from the client state.

		cl_entity_t* clientEntity = gEngfuncs.GetEntityByIndex(theDrawableEntity.mEntityNumber);        
        
        if(clientEntity)
		{
		
			if (clientEntity->index >= 32)
			{
				theDrawableEntity.mAngleRadians = clientEntity->angles[1] * M_PI / 180;
			}

			// Update the information for this entity from the client information
			// if they're in the local player's PVS.
        
			// We really want to check if the client data is more recent than the
			// minimap data, but I don't know how to get the timestamp on the minimap
			// data.
			
			if (clientEntity->curstate.messagenum >= thePlayer->curstate.messagenum)
			{
			
				//theDrawableEntity.mUser3 = (AvHUser3)(clientEntity->curstate.iuser3);

				// Brush entities don't have the correct position information, so
				// don't update them from the client data.
            

				if (theDrawableEntity.mUser3 != AVH_USER3_WELD)
				{
					theDrawableEntity.mX = clientEntity->origin.x;
					theDrawableEntity.mY = clientEntity->origin.y;
					theDrawableEntity.mAngleRadians = clientEntity->angles[1] * M_PI / 180;
				}

			}
			else
			{
			
				// If the difference between the minimap position and the client data
				// position is less than the minimap quantization error, then use
				// the client position to avoid popping when the entity goes out of the
				// PVS.
			
				float dx = fabs(theDrawableEntity.mX - clientEntity->origin.x);
				float dy = fabs(theDrawableEntity.mY - clientEntity->origin.y);
			
				if (dx < kPositionNetworkConstant && dy < kPositionNetworkConstant)
				{
					theDrawableEntity.mX = clientEntity->origin.x;
					theDrawableEntity.mY = clientEntity->origin.y;
				}
			
			}

			if (theDrawableEntity.mUser3 != AVH_USER3_COMMANDER_PLAYER)
			{
    			this->mDrawableEntityList.push_back(theDrawableEntity);
			}
		
		}
	}

	std::sort(mDrawableEntityList.begin(), mDrawableEntityList.end(), DrawingOrderSort());
}

void AvHOverviewMap::UpdateOrders(const OrderListType& inOrderList, const EntityListType& inDrawPlayers)
{

    // Look for orders which apply to the players in the draw players list.

    mMapOrderList.clear();

    if (mUser3 == AVH_USER3_COMMANDER_PLAYER)
    {
        return;
    }
    
    for (OrderListType::const_iterator theIter = inOrderList.begin(); theIter != inOrderList.end(); ++theIter)
    {

	    // Draw the order if the order is for any plays that are in our draw player list
	    bool theDrawWaypoint = false;
	    EntityInfo theReceiverPlayer = theIter->GetReceiver();
	    EntityListType::const_iterator theSearchIter = std::find(inDrawPlayers.begin(), inDrawPlayers.end(), theReceiverPlayer);
	    if(theSearchIter != inDrawPlayers.end())
		{
		    theDrawWaypoint = true;
	    }
    
        if (theDrawWaypoint)
        {

            vec3_t position;
            theIter->GetLocation(position);
            
            MapOrder mapOrder;

            mapOrder.mX = position[0];
            mapOrder.mY = position[1];
            
            mMapOrderList.push_back(mapOrder);

        }

    }

}

void AvHOverviewMap::VidInit(void)
{
	this->mMiniMapSprite = 0;
	this->mReticleSprite = 0;
	this->mMapName = "";
}