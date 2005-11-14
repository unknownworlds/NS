#ifndef AVHOVERVIEWMAP_H
#define AVHOVERVIEWMAP_H

#include "mod/AvHEntityHierarchy.h"
#include "mod/AvHMapExtents.h"
#include "mod/AvHOrder.h"

class DrawableEntity
{
public:
	DrawableEntity() : mUser3(AVH_USER3_NONE), mIsAlive(true), mX(0), mY(0), mAngleRadians(0), mIsLocalPlayer(false), mEntityNumber(0), mTeam(TEAM_IND), mSquadNumber(0)
	{}
	
	AvHUser3	    mUser3;
	bool		    mIsAlive;
	int			    mX;
	int			    mY;
    AvHTeamNumber   mTeam;
	float	  	    mAngleRadians;
	bool		    mIsLocalPlayer;
	int			    mEntityNumber;
    int             mSquadNumber;
};

class AvHOverviewMap
{										                                                                                                            
public:

    struct DrawInfo
    {

        int mX;
        int mY;
        int mWidth;
        int mHeight;
    
        float mViewWorldMinX;
        float mViewWorldMinY;
        float mViewWorldMaxX;
        float mViewWorldMaxY;

        bool mFullScreen;
   
    };
                                    AvHOverviewMap();

	void							Clear();

	bool							GetHasData() const;
	void							GetWorldPosFromMouse(const DrawInfo& inDrawInfo, int inMouseX, int inMouseY, float& outWorldX, float& outWorldY);
	
    void							SetMapExtents(const string& inMapName, const AvHMapExtents& inMapExtents);
	void                            GetMapExtents(AvHMapExtents& outMapExtents);
    
    void							SetUser3(AvHUser3 inUser3);
	void							SetWorldPosition(float inWorldX, float inWorldY);
    void                            GetWorldPosition(float& outWorldX, float& outWorldY);
	
	void							Update(float inCurrentTime);
	void                            UpdateOrders(const OrderListType& inOrderList, const EntityListType& inDrawPlayers);
    void							VidInit(void);

    void                            AddAlert(float x, float y);

    void                            Draw(const DrawInfo& inDrawInfo);

    int                             GetEntityAtWorldPosition(float inWorldX, float inWorldY, float inRadius) const;

protected:
	void							KillOldAlerts(float inCurrentTime);
	
    void							DrawMiniMap(const DrawInfo& inDrawInfo);
	void							DrawMiniMapEntity(const DrawInfo& inDrawInfo, const DrawableEntity& inEntity);
    void                            DrawAlerts(const DrawInfo& inDrawInfo);

	AvHUser3						mUser3;
    AvHTeamNumber                   mTeam;
	// puzl: 1066 the name of the last minimap we loaded
	string							mLastMinimapName;	
private:
	
    void                            WorldToMiniMapCoords(const DrawInfo& inDrawInfo, float& x, float& y);

	void							Init();
    void                            GetSpriteForEntity(const DrawableEntity& entity, int& outSprite, int& outFrame, int& outRenderMode);
    void                            GetColorForEntity(const DrawableEntity& entity, float& outR, float& outG, float& outB);
    
	void							UpdateDrawData(float inCurrentTime);

	float							mWorldPlayerX;
	float							mWorldPlayerY;

	AvHMapExtents					mMapExtents;

	string							mMapName;
	int								mMiniMapSprite;

	HSPRITE							mReticleSprite;
	
	typedef vector<DrawableEntity>	DrawableEntityListType;
	DrawableEntityListType			mDrawableEntityList;

    struct MapAlert
    {
        
        float mStartTime;
        float mExpireTime;
        
        float mX; // World space.
        float mY;

    };

    typedef std::vector<MapAlert>	MapAlertListType;
    MapAlertListType				mAlertList;

    struct MapOrder
    {
        float mX; // World space.
        float mY;
    };

    typedef std::vector<MapOrder>	MapOrderListType;
    MapOrderListType				mMapOrderList;

    float                           mLastUpdateTime;

};

#endif