
//new collision replacemnet

#include <vector>
#include <memory>

#include "common/mathlib.h"
#include "common/const.h"
#include "common/com_model.h"
#include "pm_defs.h"
#include "common/vector_util.h"
#include "mod/AvHHulls.h"
#include "types.h"
#include "util/MathUtil.h"

#include "CollisionChecker.h"

using std::auto_ptr;

//-----------------------------------------------------------------------------
// PlaneTest classes - used by CollisionChecker for testing against hulls
//-----------------------------------------------------------------------------

class CollisionTest 
{
public:
	virtual ~CollisionTest(void) {}
	virtual int GetPlanarIntersectionType(const mplane_t* plane) const = 0;
	virtual bool GetAABBIntersection(const nspoint_t& mins, const nspoint_t& maxs) const = 0;
	virtual auto_ptr<CollisionTest> RotateAndShift(const nspoint_t& forward, const nspoint_t& up, const nspoint_t& right, const nspoint_t& origin) const = 0;
	virtual auto_ptr<CollisionTest> Shift(const nspoint_t& origin) const = 0;
};

class PointCollisionTest : public CollisionTest
{
public:
	PointCollisionTest(const nspoint_t& point) 
	{ SetPoint(point); }

	void SetPoint(const nspoint_t& point) 
	{ VectorCopy(point,point_); }

	int GetPlanarIntersectionType(const mplane_t* plane) const
	{
		float d;
		if(plane->type < 3) // first three plane types are axial by index
		{
			d = point_[plane->type]*plane->normal[plane->type] - plane->dist;
		}
		else
		{
			d = DotProduct(plane->normal,point_) - plane->dist;
		}
		return ((d < 0) ? CollisionChecker::INTERSECTION_RELATION_BACK : CollisionChecker::INTERSECTION_RELATION_FRONT);
	}

	bool GetAABBIntersection(const nspoint_t& mins, const nspoint_t& maxs) const
	{
		return (
			(point_[0] >= mins[0]) &&
			(point_[0] <= maxs[0]) &&
			(point_[1] >= mins[1]) &&
			(point_[1] <= maxs[1]) &&
			(point_[2] >= mins[2]) &&
			(point_[2] <= maxs[1])
			);
	}

	auto_ptr<CollisionTest> RotateAndShift(const nspoint_t& forward, const nspoint_t& right, const nspoint_t& up, const nspoint_t& origin) const
	{
		return Shift(origin);
	}

	auto_ptr<CollisionTest> Shift(const nspoint_t& origin) const
	{
		nspoint_t new_origin;
		VectorAdd(origin,point_,new_origin);
		return auto_ptr<CollisionTest>(new PointCollisionTest(new_origin));
	}

private:
	nspoint_t point_;
};

//-----------------------------------------------------------------------------

class CylinderCollisionTest : public CollisionTest
{
public:
	CylinderCollisionTest(const nspoint_t& up, const nspoint_t& base, const float radius, const float height) : point_test_(base)
	{ 
		VectorCopy(up,up_);
		VectorNormalize(up_);
		VectorCopy(base,base_);
		radius_ = radius;
		height_ = height;
	}

	int GetPlanarIntersectionType(const mplane_t* plane) const
	{
		nspoint_t in_plane,out_of_plane,point_to_check;
		CrossProduct(up_,plane->normal,in_plane);
		CrossProduct(up_,in_plane,out_of_plane); //perp to up and pointing directly out of plane
		VectorNormalize(out_of_plane);

		VectorMA(base_,radius_,out_of_plane,point_to_check);
		point_test_.SetPoint(point_to_check);
		int intersect = point_test_.GetPlanarIntersectionType(plane);

		VectorMA(point_to_check,height_,up_,point_to_check);
		point_test_.SetPoint(point_to_check);
		intersect |= point_test_.GetPlanarIntersectionType(plane);

		if(intersect != CollisionChecker::INTERSECTION_RELATION_BOTH)
		{
			VectorMA(base_,-radius_,out_of_plane,point_to_check);
			point_test_.SetPoint(point_to_check);
			intersect |= point_test_.GetPlanarIntersectionType(plane);
		}

		if(intersect != CollisionChecker::INTERSECTION_RELATION_BOTH)
		{
			VectorMA(point_to_check,height_,up_,point_to_check);
			point_test_.SetPoint(point_to_check);
			intersect |= point_test_.GetPlanarIntersectionType(plane);
		}

		return intersect;
	}

	bool GetAABBIntersection(const nspoint_t& mins, const nspoint_t& maxs) const
	{
		//special case of unrotated cylinder
		if(up_[2] == 1.0f)
		{
			if( maxs[2] < base_[2] || 
				mins[2] > base_[2] + height_ ||
				maxs[0] < base_[0] - radius_ || 
				mins[0] > base_[0] + radius_ ||
				maxs[1] < base_[1] - radius_ ||
				maxs[1] > base_[1] + radius_
				) { return false; }
			//TODO: tighten definition to use overlap of circle instead of box with circle inscribed
			return true;
		}

		//TODO: handle general case by turning bbox into hull and testing against that hull
		ASSERT(0 && "Need to handle case of rotated cylinder & AABB collision");
		return false;
	}

	auto_ptr<CollisionTest> RotateAndShift(const nspoint_t& forward, const nspoint_t& right, const nspoint_t& up, const nspoint_t& origin) const
	{
		nspoint_t new_up, new_base;
		TransformVector(up_,forward,right,up,new_up);
		VectorAdd(base_,origin,new_base);
		return auto_ptr<CollisionTest>(new CylinderCollisionTest(new_up,new_base,radius_,height_));
	}

	auto_ptr<CollisionTest> Shift(const nspoint_t& origin) const
	{
		nspoint_t new_base;
		VectorAdd(base_,origin,new_base);
		return auto_ptr<CollisionTest>(new CylinderCollisionTest(up_,new_base,radius_,height_));
	}

private:
	mutable PointCollisionTest point_test_;
	nspoint_t up_;
	nspoint_t base_;
	float radius_;
	float height_;
};

//-----------------------------------------------------------------------------

//TODO: determine if there's a faster way than scanning all 8 corners
class OBBCollisionTest : public CollisionTest
{
public:
	OBBCollisionTest(const OBBox& box) : point_test_(box.mOrigin)
	{
		box_ = box;
		nspoint_t offset;
		//calculate eight corners of the box given input angles:
		for(int index = 0; index < 8; ++index)
		{
			offset[0] = box.mExtents[0] * box.mAxis[0][0];
			offset[0] += box.mExtents[1] * box.mAxis[1][0];
			offset[0] += box.mExtents[2] * box.mAxis[2][0];
			if(index % 2) //every other item
			{ offset[0] *= -1; }

			offset[1] = box.mExtents[0] * box.mAxis[0][1];
			offset[1] += box.mExtents[1] * box.mAxis[1][1];
			offset[1] += box.mExtents[2] * box.mAxis[2][1];
			if((index / 2) % 2) //every other pair of items
			{ offset[1] *= -1; }

			offset[2] = box.mExtents[0] * box.mAxis[0][2];
			offset[2] += box.mExtents[1] * box.mAxis[1][2];
			offset[2] += box.mExtents[2] * box.mAxis[2][2];
			if(index < 4) //4 false, then 4 true
			{ offset[2] *= -1; }

			VectorAdd(box.mOrigin,offset,corners[index]);
		}
	}

	int GetPlanarIntersectionType(const mplane_t* plane) const
	{
		int intersection_type = CollisionChecker::INTERSECTION_RELATION_NONE;
		for(int index = 0; index < 8; ++index)
		{
			point_test_.SetPoint(corners[index]);
			intersection_type |= point_test_.GetPlanarIntersectionType(plane);
			if(intersection_type == CollisionChecker::INTERSECTION_RELATION_BOTH)
			{ break; }
		}
		return intersection_type;
	}

	bool GetAABBIntersection(const nspoint_t& mins, const nspoint_t& maxs) const
	{
		bool intersection = false;
		for(int index = 0; index < 8; ++index)
		{
			point_test_.SetPoint(corners[index]);
			intersection = point_test_.GetAABBIntersection(mins,maxs);
			if(intersection)
			{ break; }
		}
		return intersection;
	}

	auto_ptr<CollisionTest> RotateAndShift(const nspoint_t& forward, const nspoint_t& right, const nspoint_t& up, const nspoint_t& origin) const
	{
		//make this an OBB by rotation around the ent angles - not a cheap operation.
		OBBox box = box_;

		VectorAdd(box_.mOrigin,origin,box.mOrigin);
		TransformVector(box_.mAxis[0],forward,right,up,box.mAxis[0]);
		TransformVector(box_.mAxis[1],forward,right,up,box.mAxis[1]);
		TransformVector(box_.mAxis[2],forward,right,up,box.mAxis[2]);

		//rotate axes!
		return auto_ptr<CollisionTest>(new OBBCollisionTest(box));
	}

	auto_ptr<CollisionTest> Shift(const nspoint_t& origin) const
	{
		//make this an OBB by rotation around the ent angles - not a cheap operation.
		OBBox box = box_;
		VectorAdd(box_.mOrigin,origin,box.mOrigin);
		return auto_ptr<CollisionTest>(new OBBCollisionTest(box));
	}

private:
	OBBox box_;
	nspoint_t corners[8];
	mutable PointCollisionTest point_test_;
};

//-----------------------------------------------------------------------------

class AABBCollisionTest : public CollisionTest
{
public:
	AABBCollisionTest(const nspoint_t& mins, const nspoint_t& maxs) : point_test_(mins)
	{
		VectorCopy(maxs,maxs_);
		VectorCopy(mins,mins_);
	}

	int GetPlanarIntersectionType(const mplane_t* plane) const
	{
		nspoint_t orientedMins = { 
			plane->normal[0] < 0 ? maxs_[0] : mins_[0], 
			plane->normal[1] < 0 ? maxs_[1] : mins_[1], 
			plane->normal[2] < 0 ? maxs_[2] : mins_[2]
		};
		point_test_.SetPoint(orientedMins);
		int intersect_type = point_test_.GetPlanarIntersectionType(plane);

		nspoint_t orientedMaxes = { 
			plane->normal[0] < 0 ? mins_[0] : maxs_[0], 
			plane->normal[1] < 0 ? mins_[1] : maxs_[1], 
			plane->normal[2] < 0 ? mins_[2] : maxs_[2]
		};
		point_test_.SetPoint(orientedMaxes);
		intersect_type |= point_test_.GetPlanarIntersectionType(plane);

		return intersect_type;	
	}

	bool GetAABBIntersection(const nspoint_t& mins, const nspoint_t& maxs) const
	{
		return (
			(maxs_[0] >= mins[0]) &&
			(mins_[0] <= maxs[0]) &&
			(maxs_[1] >= mins[1]) &&
			(mins_[1] <= maxs[1]) &&
			(maxs_[2] >= mins[2]) &&
			(mins_[2] <= maxs[2])
			);
	}

	auto_ptr<CollisionTest> RotateAndShift(const nspoint_t& forward, const nspoint_t& right, const nspoint_t& up, const nspoint_t& origin) const
	{
		//make this an OBB by rotation around the ent angles - not a cheap operation.
		OBBox box;

		//set up origin = ((mins + maxs) / 2) - ent origin
		VectorCopy(mins_,box.mOrigin);
		VectorAdd(maxs_,box.mOrigin,box.mOrigin);
		VectorScale(box.mOrigin,0.5f,box.mOrigin);
		VectorSubtract(box.mOrigin,origin,box.mOrigin);

		//set up extents = ((maxs - mins) / 2)
		VectorCopy(maxs_,box.mExtents);
		VectorSubtract(box.mExtents,mins_,box.mExtents);
		VectorScale(box.mExtents,0.5f,box.mExtents);

		//set up axis = axis defined by ent angles
		nspoint_t axis[3] = {{1,0,0},{0,1,0},{0,0,1}};
		VectorCopy(forward,box.mAxis[0]);
		VectorCopy(up,box.mAxis[1]);
		VectorCopy(right,box.mAxis[2]);

		return auto_ptr<CollisionTest>(new OBBCollisionTest(box));
	}

	auto_ptr<CollisionTest> Shift(const nspoint_t& origin) const
	{
		nspoint_t new_mins, new_maxs;
		VectorAdd(origin,mins_,new_mins);
		VectorAdd(origin,maxs_,new_maxs);
		return auto_ptr<CollisionTest>(new AABBCollisionTest(new_mins,new_maxs));
	}

private:
	nspoint_t mins_, maxs_;
	mutable PointCollisionTest point_test_;
};

//-----------------------------------------------------------------------------
// CollisionChecker class - making a fresh start on collision checking code
// that runs properly on both the server and client; requires that pmove is
// filled to function -- won't function fully for initial frames
//
// TODO: write quick functions for when pmove isn't available
// KGP - 4/25/04
//-----------------------------------------------------------------------------

const int CollisionChecker::NO_ENTITY = -1;
const int CollisionChecker::WORLD_ENTITY = 0;

const bool CollisionChecker::IGNORE_PLAYERS_FALSE = false;
const bool CollisionChecker::IGNORE_PLAYERS_TRUE = true;

const int CollisionChecker::HULL_TYPE_BBOX = 1;
const int CollisionChecker::HULL_TYPE_BSP = 2;
const int CollisionChecker::HULL_TYPE_ALL = 3;

const int CollisionChecker::INTERSECTION_RELATION_NONE = 0;
const int CollisionChecker::INTERSECTION_RELATION_FRONT = 1;
const int CollisionChecker::INTERSECTION_RELATION_BACK = 2;
const int CollisionChecker::INTERSECTION_RELATION_BOTH = 3;

CollisionChecker::CollisionChecker(const playermove_t* pmove) : pmove_(pmove) 
{
	SetHullNumber(0);
	SetHullTypeMask(HULL_TYPE_ALL);
	SetIgnorePlayers(false);
	SetIgnoreEntityClass(IGNORE_NONE);
	ClearEntityToIgnore();
}

CollisionChecker::CollisionChecker(const playermove_t* pmove, int ns_hull_number, int hull_type_mask, bool ignore_players, int ignore_entity_class, int entity_index_to_ignore) : pmove_(pmove)
{
	SetHullNumber(ns_hull_number);
	SetHullTypeMask(hull_type_mask);
	SetIgnorePlayers(ignore_players);
	SetIgnoreEntityClass(ignore_entity_class);
	SetEntityToIgnore(entity_index_to_ignore);
}

//-------------------------------------------------------------------
// Configuration
//-------------------------------------------------------------------

void CollisionChecker::SetHullNumber(int ns_hull_number)
{
	switch(ns_hull_number)
	{
	case kHLPointHullIndex:
		valve_hull_number_ = 0;
		break;
	case kHLCrouchHullIndex:
		valve_hull_number_ = 1;
		break;
	case kHLStandHullIndex:
		valve_hull_number_ = 2;
		break;
	case kNSHugeHullIndex:
		valve_hull_number_ = 3;
		break;
	default:
		ASSERT(0 && "unknown hull index passed to CollisionChecker::SetHullNumber");
		valve_hull_number_ = 0;
		break;
	}
}

//-------------------------------------------------------------------

void CollisionChecker::SetHullTypeMask(int hull_type_mask)
{
	hull_type_mask_ = hull_type_mask;
}

//-------------------------------------------------------------------

void CollisionChecker::SetEntityToIgnore(int entity_index_to_ignore)
{
	ASSERT(entity_index_to_ignore > 0 || entity_index_to_ignore == NO_ENTITY);
	entity_index_to_ignore_ = entity_index_to_ignore;
}

//-------------------------------------------------------------------

void CollisionChecker::ClearEntityToIgnore(void)
{
	entity_index_to_ignore_ = NO_ENTITY;
}

//-------------------------------------------------------------------

void CollisionChecker::SetIgnorePlayers(bool ignore_players)
{
	ignore_players_ = ignore_players;
}

//-------------------------------------------------------------------

void CollisionChecker::SetIgnoreEntityClass(int ignore_entity_class)
{
	ignore_entity_class_ = ignore_entity_class;
}

//-------------------------------------------------------------------
// Public point routines
//-------------------------------------------------------------------

int CollisionChecker::GetContentsAtPoint(const nspoint_t& point) const
{
	return GetContents(&PointCollisionTest(point));
}

//-------------------------------------------------------------------

int CollisionChecker::GetWorldContentsAtPoint(const nspoint_t& point) const
{
	return GetSingleEntityContentsAtPoint(point, WORLD_ENTITY);
}

//-------------------------------------------------------------------

int CollisionChecker::GetAllEntityContentsAtPoint(const nspoint_t& point) const
{
	return GetAllEntityContents(&PointCollisionTest(point));
}

//-------------------------------------------------------------------

int CollisionChecker::GetSingleEntityContentsAtPoint(const nspoint_t& point, int entity_index) const
{
	return GetSingleEntityContents(&PointCollisionTest(point),entity_index);
}

//-------------------------------------------------------------------
// Public Cylinder routines
//-------------------------------------------------------------------

const static nspoint_t CYLINDER_UP_DEFAULT = {0,0,1.0f};

int CollisionChecker::GetContentsInCylinder(const nspoint_t& base, float radius, float height) const
{
	return GetContents(&CylinderCollisionTest(CYLINDER_UP_DEFAULT,base,radius,height));
}

//-------------------------------------------------------------------

int CollisionChecker::GetWorldContentsInCylinder(const nspoint_t& base, float radius, float height) const
{
	return GetSingleEntityContents(&CylinderCollisionTest(CYLINDER_UP_DEFAULT,base,radius,height),WORLD_ENTITY);
}

//-------------------------------------------------------------------

int CollisionChecker::GetAllEntityContentsInCylinder(const nspoint_t& base, float radius, float height) const
{
	return GetAllEntityContents(&CylinderCollisionTest(CYLINDER_UP_DEFAULT,base,radius,height));
}

//-------------------------------------------------------------------

int CollisionChecker::GetSingleEntityContentsInCylinder(const nspoint_t& base, float radius, float height, int entity_index) const
{
	return GetSingleEntityContents(&CylinderCollisionTest(CYLINDER_UP_DEFAULT,base,radius,height),entity_index);
}

//-------------------------------------------------------------------
// Public AABB routines
//-------------------------------------------------------------------

int CollisionChecker::GetContentsInAABB(const nspoint_t& mins, const nspoint_t& maxs) const
{
	return GetContents(&AABBCollisionTest(mins,maxs));
}

//-------------------------------------------------------------------

int CollisionChecker::GetWorldContentsInAABB(const nspoint_t& mins, const nspoint_t& maxs) const
{
	return GetSingleEntityContents(&AABBCollisionTest(mins,maxs),WORLD_ENTITY);
}

//-------------------------------------------------------------------

int CollisionChecker::GetAllEntityContentsInAABB(const nspoint_t& mins, const nspoint_t& maxs) const
{
	return GetAllEntityContents(&AABBCollisionTest(mins,maxs));
}

//-------------------------------------------------------------------

int CollisionChecker::GetSingleEntityContentsInAABB(const nspoint_t& mins, const nspoint_t& maxs, int entity_index) const
{
	return GetSingleEntityContents(&AABBCollisionTest(mins,maxs),entity_index);
}

//-------------------------------------------------------------------
// Private utility routines
//-------------------------------------------------------------------

int CollisionChecker::CombineContents(const int old_contents, const int new_contents) const
{
	if(new_contents == CONTENTS_EMPTY)
	{ return old_contents; }
	if(old_contents == CONTENT_EMPTY)
	{ return new_contents; }
	return max(old_contents,new_contents);
}

//-------------------------------------------------------------------
// Private content check routines
//-------------------------------------------------------------------

int CollisionChecker::GetContents(const CollisionTest* test) const
{
	int total_contents = GetSingleEntityContents(test,WORLD_ENTITY);
	if(total_contents != CONTENTS_SOLID)
	{
		total_contents = CombineContents(total_contents,GetAllEntityContents(test));
	}
	return total_contents;
}

//-------------------------------------------------------------------

int CollisionChecker::GetAllEntityContents(const CollisionTest* test) const
{
	int total_contents = CONTENTS_EMPTY;

	UpdateNumEntities();
	for(int counter = WORLD_ENTITY+1; counter < entity_count_; ++counter)
	{
		total_contents = CombineContents(total_contents,GetSingleEntityContents(test,counter));
		if(total_contents == CONTENTS_SOLID)
		{ break; }
	}

	return total_contents;
}

//-------------------------------------------------------------------

int CollisionChecker::GetSingleEntityContents(const CollisionTest* test, int entity_index) const
{
	int total_contents = CONTENTS_EMPTY;
	const physent_t* ent = GetPhysentForIndex(entity_index);
	if(ent && ent->info != entity_index_to_ignore_ && (!ignore_players_ || ent->info == WORLD_ENTITY || ent->info > 32))
	{
		if(ent->solid == SOLID_BBOX)
		{
			nspoint_t bbox_mins, bbox_maxs;
			VectorSubtract(ent->mins,ent->origin,bbox_mins);
			VectorSubtract(ent->maxs,ent->origin,bbox_maxs);

			if(valve_hull_number_) //adjust for input hull
			{
				const hull_t* hull = &(&pmove_->physents[WORLD_ENTITY])->model->hulls[valve_hull_number_];
				VectorAdd(bbox_mins,hull->clip_mins,bbox_mins);
				VectorAdd(bbox_mins,hull->clip_maxs,bbox_maxs);
			}

			if(test->GetAABBIntersection(bbox_mins,bbox_maxs))
			{
				total_contents = CONTENTS_SOLID;
			}
		}
		else if(ent->model && ent->model->type == mod_brush)
		{
			const hull_t* hull = &ent->model->hulls[valve_hull_number_];

			if(ent->angles[0] || ent->angles[1] || ent->angles[2])
			{
				float axes[3][3];
				AngleVectors(ent->angles,axes[0],axes[1],axes[2]);
				auto_ptr<CollisionTest> rotated_test = test->RotateAndShift(axes[0],axes[1],axes[2],ent->origin);
				total_contents = GetHullContent(hull,hull->firstclipnode,rotated_test.get());
			}
			else
			{
				auto_ptr<CollisionTest> shifted_test = test->Shift(ent->origin);
				total_contents = GetHullContent(hull,hull->firstclipnode,shifted_test.get());
			}
		}
		else
		{
			int a = 0;	//breakpoint to be used as catch-all
		}
	}
	return total_contents;
}

//-------------------------------------------------------------------

//DFS search for solid hull leaf with passed in intersection test and short circuit on solid leaf found
int CollisionChecker::GetHullContent(const hull_t* hull, int current_node, const CollisionTest* test) const
{
	ASSERT(current_node >= hull->firstclipnode);
	ASSERT(current_node <= hull->lastclipnode);
	int total_contents = CONTENTS_EMPTY;

	if(current_node < 0)
	{ total_contents = current_node; }
	else
	{
		dclipnode_t	*node = &hull->clipnodes[current_node];
		mplane_t	*plane;
		int intersection;
		while(true)
		{
			ASSERT(total_contents < 0); // 0 + is a node number and means we've had a bad assignment

			//short circuit on solid found
			if(total_contents == CONTENTS_SOLID)
			{ 
				this->intersected_nodes_.clear();
				break; 
			}

			plane = &hull->planes[node->planenum];

			intersection = test->GetPlanarIntersectionType(&hull->planes[node->planenum]);

			switch(intersection)
			{
			case INTERSECTION_RELATION_BOTH:
				if(node->children[0] > CONTENTS_EMPTY) //non-leaf
				{
					if(node->children[1] > CONTENTS_EMPTY) //non-leaf
					{
						this->intersected_nodes_.push_back(node->children[0]); //store one path for later
						node = &hull->clipnodes[node->children[1]]; //follow other path
						continue;
					}
				
					total_contents = CombineContents(total_contents,node->children[1]);
					node = &hull->clipnodes[node->children[0]];
					continue;
				}
				else
				{
					total_contents = CombineContents(total_contents,node->children[0]);

					if(node->children[1] > CONTENTS_EMPTY) //non-leaf
					{
						node = &hull->clipnodes[node->children[1]];
						continue;
					}

					total_contents = CombineContents(total_contents,node->children[1]);
				}
				break;

			case INTERSECTION_RELATION_FRONT:
				if(node->children[0] > CONTENTS_EMPTY)
				{ 
					node = &hull->clipnodes[node->children[0]];
					continue;
				}

				total_contents = CombineContents(total_contents,node->children[0]);
				break;

			case INTERSECTION_RELATION_BACK:
				if(node->children[1] > CONTENTS_EMPTY)
				{ 
					node = &hull->clipnodes[node->children[1]]; 
					continue;
				}

				total_contents = CombineContents(total_contents,node->children[1]);
				break;
			}

			//check for other subtrees to process
			if(this->intersected_nodes_.empty())
			{ break; }

			node = &hull->clipnodes[this->intersected_nodes_.back()];
			this->intersected_nodes_.pop_back();
		}
	}
	return total_contents;
}

//------------------------------------------------------------------------------
// Client/server specific code :( ... client version, server version in own file
//------------------------------------------------------------------------------
#ifdef AVH_CLIENT
const int CollisionChecker::IGNORE_NONE = 0;
const int CollisionChecker::IGNORE_INVISIBLE = 1;
const int CollisionChecker::IGNORE_INTANGIBLE = 2;

void CollisionChecker::UpdateNumEntities() const
{
	switch(ignore_entity_class_)
	{
	case IGNORE_NONE:
	case IGNORE_INVISIBLE:
		entity_count_ = pmove_ ? pmove_->numvisent : 0;
		break;
	case IGNORE_INTANGIBLE:
		entity_count_ = pmove_ ? pmove_->numphysent : 0;
		break;
	default:
		ASSERT(0 && "Unknown ignore entity class");
	};
}

const physent_t* CollisionChecker::GetPhysentForIndex(int entity_index) const
{
	switch(ignore_entity_class_)
	{
	case IGNORE_NONE:
	case IGNORE_INVISIBLE:
		return pmove_ ? &pmove_->visents[entity_index] : (physent_t*)NULL;
	case IGNORE_INTANGIBLE:
		return pmove_ ? &pmove_->physents[entity_index] : (physent_t*)NULL;
	}
	ASSERT(0 && "Unknown ignore entity class");
	return NULL;
}

#endif