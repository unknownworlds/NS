#ifndef AVH_MAP_EXTENTS_H
#define AVH_MAP_EXTENTS_H

class AvHMapExtents
{
public:
						AvHMapExtents();

	#ifdef AVH_SERVER
	void				CalculateMapExtents();
	#endif

	void				ResetMapExtents();

	float				GetMinViewHeight() const;
	float				GetMaxViewHeight() const;

	void				SetMinViewHeight(float inViewHeight);
	void				SetMaxViewHeight(float inViewHeight);

	float				GetMinMapX() const;
	float				GetMaxMapX() const;

	void				SetMinMapX(float inMapX);
	void				SetMaxMapX(float inMapX);
	
	float				GetMinMapY() const;
	float				GetMaxMapY() const;

	void				SetMinMapY(float inMapY);
	void				SetMaxMapY(float inMapY);

	bool				GetDrawMapBG() const;
	void				SetDrawMapBG(bool inDrawMapBG);
	
	#ifdef AVH_SERVER
	float				GetTopDownCullDistance() const;
	void				SetTopDownCullDistance(float inCullDistance);
	#endif

private:
	bool				mCalculatedMapExtents;

	float				mMinViewHeight;
	float				mMaxViewHeight;
	float				mMinMapX;
	float				mMinMapY;
	float				mMaxMapX;
	float				mMaxMapY;
	bool				mDrawMapBG;

	#ifdef AVH_SERVER
	float				mTopDownCullDistance;
	#endif
};

#endif