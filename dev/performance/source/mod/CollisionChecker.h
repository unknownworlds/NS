#ifndef COLLISIONCHECKER_H
#define COLLISIONCHECKER_H

//-----------------------------------------------------------------------------
// Begin NS collision detection replacement
//
// Class is not reentrant, declare 1 per thread if necessary
// Implementation is split into three files -- [x]Shared, [x]Client, and [x]Server
// Client and Server code provides identical functionality under the same function
// names, but are written to address the peculiarities of each environment
//-----------------------------------------------------------------------------

typedef float nspoint_t[3]; //do this because vec3_t is inconsistantly defined.

struct OBBox
{
    nspoint_t mExtents;
    nspoint_t mOrigin;
    nspoint_t mAxis[3];
};

class CollisionTest; //implemented internally for each collision hull type/shape

class CollisionChecker
{
public:
	CollisionChecker(const playermove_t* pmove);
	CollisionChecker(const playermove_t* pmove, int ns_hull_number, int hull_type_mask, bool ignore_players, int ignore_entity_class, int entity_index_to_ignore);

	void SetIgnorePlayers(bool ignore_players);
	void SetIgnoreEntityClass(int ignore_entity_class);
	void SetEntityToIgnore(int entity_index_to_ignore);
	void ClearEntityToIgnore(void);

	void SetHullNumber(int ns_hull_number);
	void SetHullTypeMask(int hull_type_mask);

	int GetContentsAtPoint(const nspoint_t& point) const;
	int GetSingleEntityContentsAtPoint(const nspoint_t& point, int pmove_index) const;
	int GetWorldContentsAtPoint(const nspoint_t& point) const;
	int GetAllEntityContentsAtPoint(const nspoint_t& point) const;

	int GetContentsInAABB(const nspoint_t& mins, const nspoint_t& maxs) const;
	int GetSingleEntityContentsInAABB(const nspoint_t& mins, const nspoint_t& maxs, int entity_index) const;
	int GetWorldContentsInAABB(const nspoint_t& mins, const nspoint_t& maxs) const;
	int GetAllEntityContentsInAABB(const nspoint_t& mins, const nspoint_t& maxs) const;

	int GetContentsInOBB(const OBBox& bounding_box) const;
	int GetSingleEntityConentsInOBB(const OBBox& bounding_box, int entity_index) const;
	int GetWorldContentsInOBB(const OBBox& bounding_box) const;
	int GetAllEntityContentsInOBB(const OBBox& bounding_box) const;

	int GetContentsInSphere(const nspoint_t& origin, const float radius) const;
	int GetSingleEntityContentsInSphere(const nspoint_t& origin, const float radius, int entity_index) const;
	int GetWorldContentsInSphere(const nspoint_t& origin, const float radius) const;
	int GetAllEntityContentsInSphere(const nspoint_t& origin, const float radius) const;

	int GetContentsInCylinder(const nspoint_t& base, const float radius, const float height) const;
	int GetSingleEntityContentsInCylinder(const nspoint_t& base, const float radius, const float height, int entity_index) const;
	int GetWorldContentsInCylinder(const nspoint_t& base, const float radius, const float height) const;
	int GetAllEntityContentsInCylinder(const nspoint_t& base, const float radius, const float height) const;

	//for use in passing values into the system
	const static int NO_ENTITY;
	const static int WORLD_ENTITY;
	const static int IGNORE_NONE;
	const static int IGNORE_INVISIBLE;
	const static int IGNORE_INTANGIBLE;
	const static bool IGNORE_PLAYERS_FALSE;
	const static bool IGNORE_PLAYERS_TRUE;

	//for use in internal tests
	const static int INTERSECTION_RELATION_NONE;
	const static int INTERSECTION_RELATION_FRONT;
	const static int INTERSECTION_RELATION_BACK;
	const static int INTERSECTION_RELATION_BOTH;
	const static int HULL_TYPE_BSP;
	const static int HULL_TYPE_BBOX;
	const static int HULL_TYPE_ALL;

private:
	int CombineContents(const int old_contents, const int new_contents) const;
	int GetHullContent(const hull_t* hull, int current_node, const CollisionTest* test) const;

	int GetContents(const CollisionTest* test) const;
	int GetSingleEntityContents(const CollisionTest* test, int entity_index) const;
	int GetAllEntityContents(const CollisionTest* test) const;

	//client/server specific
	void UpdateNumEntities() const; //called once per GetAllEntityContents, sets internal entity counter
	const physent_t* GetPhysentForIndex(int entity_index) const; //called by GetSingleEntityContents

	const playermove_t* pmove_;
	mutable physent_t local_ent;
	bool ignore_players_;
	int entity_index_to_ignore_;
	int valve_hull_number_;
	int ignore_entity_class_;
	int hull_type_mask_;
	mutable int entity_count_;
	mutable std::vector<int> intersected_nodes_; //used for DFS hull intersect code
};

#endif