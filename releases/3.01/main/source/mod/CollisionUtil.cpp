#include "common/mathlib.h"
#include "common/const.h"
#include "common/com_model.h"

#include "common/vector_util.h"
#include "engine/studio.h"

#include "CollisionUtil.h"

#include <memory.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include <queue>
#include "localassert.h"
#include "engine/progdefs.h"

#ifdef AVH_SERVER
#include "engine/edict.h"
#include "engine/eiface.h"
#include "dlls/enginecallback.h"
#endif

#ifdef AVH_CLIENT
#include "common/cl_entity.h"
#endif

#include "AnimationUtil.h"

#include "pm_defs.h"

const float AVH_INFINITY = -logf(0);
const float AVH_EPSILON  = 0.001f;

#define PITCH   0
#define YAW     1
#define ROLL    2 

// These definitions are taken from the QuakeWorld source code.

#define	SURF_PLANEBACK		2
#define	SURF_DRAWSKY		4
#define SURF_DRAWSPRITE		8
#define SURF_DRAWTURB		0x10
#define SURF_DRAWTILED		0x20
#define SURF_DRAWBACKGROUND	0x40
#define SURF_UNDERWATER		0x80
#define SURF_DONTWARP		0x100

#define	VERTEXSIZE	7

typedef struct glpoly_s
{
	struct	glpoly_s	*next;
	struct	glpoly_s	*chain;
	int		numverts;
	int		flags;			// for SURF_UNDERWATER
	float	verts[4][VERTEXSIZE];	// variable sized (xyz s1t1 s2t2)
} glpoly_t;

typedef struct glmnode_s
{

// common with leaf
	int				contents;		// 0, to differentiate from leafs
	int				visframe;		// node needs to be traversed if current

	float   		minmaxs[6];		// for bounding box culling

	struct mnode_s	*parent;

// node specific
	mplane_t		*plane;
	struct mnode_s	*children[2];

	unsigned short		firstsurface;
	unsigned short		numsurfaces;

} glmnode_t;

typedef struct glmleaf_s
{

// common with node
	int			contents;		// wil be a negative contents number
	int			visframe;		// node needs to be traversed if current

	float		minmaxs[6];		// for bounding box culling

	struct mnode_s	*parent;

// leaf specific
	byte		*compressed_vis;
	struct efrag_s	*efrags;

	msurface_t	**firstmarksurface;
	int			nummarksurfaces;
	int			key;			// BSP sequence number for leaf's contents
	byte		ambient_sound_level[NUM_AMBIENTS];

} glmleaf_t;

typedef struct glmsurface_s
{
	int			visframe;		// should be drawn when node is crossed

	mplane_t	*plane;
	int			flags;

	int			firstedge;	// look up in model->surfedges[], negative numbers
	int			numedges;	// are backwards edges

	short		texturemins[2];
	short		extents[2];

	int			light_s, light_t;			// gl lightmap coordinates

	glpoly_t	*polys;						// multiple if warped
	struct msurface_s	*texturechain;

	mtexinfo_t	*texinfo;

// lighting info
	int			dlightframe;
	int			dlightbits;

	int			lightmaptexturenum;
	byte		styles[MAXLIGHTMAPS];
	int			cached_light[MAXLIGHTMAPS];	// values currently used in lightmap
	qboolean	cached_dlight;				// true if dynamic light in cache

//	byte		*samples;					// [numstyles*surfsize]
	color24		*samples;					// note: this is the actual lightmap data for this surface
	decal_t		*pdecals;

} glmsurface_t;


#define NODE		glmnode_t
#define SURFACE		glmsurface_t


/**
 * Returns the distance between a point and a plane.
 */
float NS_PointToPlaneDistance(const vec3_t point, const mplane_t* plane)
{

	if (plane->type < 3)
	{
		return point[plane->type] - plane->dist; 
	}
	else
	{
		return DotProduct(point, plane->normal) - plane->dist;
	}

}


/**
 * Returns the square of the distance between two points.
 */
float NS_PointToPointDistanceSquared(const vec3_t p1, const vec3_t p2)
{
	vec3_t v;
	VectorSubtract(p2, p1, v);
	return DotProduct(v, v);
}


/**
 * Returns the minimum distance from a point to the perimeter of a surface
 * of a model.
 */
float NS_PointToSurfaceEdgeDistance(const model_t* model, const SURFACE* surface, const vec3_t point)
{
	
	float minDistance = FLT_MAX;

	int lastEdge = surface->firstedge + surface->numedges;

	for (int i = surface->firstedge; i < lastEdge; ++i)
	{
		
		ASSERT(i >= 0 && i < model->numsurfedges);
	
		int edge = model->surfedges[i];

		const mvertex_t* v1;
		const mvertex_t* v2;

		if (edge >= 0)
		{
			v1 = &model->vertexes[model->edges[edge].v[0]];
			v2 = &model->vertexes[model->edges[edge].v[1]];
		}
		else
		{
			v1 = &model->vertexes[model->edges[-edge].v[1]];	
			v2 = &model->vertexes[model->edges[-edge].v[0]];
		}

		// Compute the closest point on the line to the point.
		
		vec3_t a;
		vec3_t b;

		VectorSubtract(point, v1->position, a);
		VectorSubtract(v2->position, v1->position, b);

		float t = DotProduct(a, b) / DotProduct(b, b);

		// Restrict the point to being on the line segment.

		if (t < 0) t = 0;
		if (t > 1) t = 1;

		vec3_t closestPoint;

		VectorScale(b, t, closestPoint);
		VectorAdd(closestPoint, v1->position, closestPoint);

		// Compute the distance.

		float distance = NS_PointToPointDistanceSquared(point, closestPoint);
		
		if (distance < minDistance)
		{
			minDistance = distance;
		}
	
	}

	return sqrtf(minDistance);

}


/**
 * Tests if a point lies on a surface of a model. The point must lie on the
 * plane of the surface.
 */
bool NS_PointOnSurface(const model_t* model, const SURFACE* surface, const vec3_t point)
{
	
	int lastEdge = surface->firstedge + surface->numedges;

	int x = (surface->plane->type + 1) % 3;
	int y = (surface->plane->type + 2) % 3; 

	int crossings = 0;

	for (int i = surface->firstedge; i < lastEdge; ++i)
	{
		
		ASSERT(i >= 0 && i < model->numsurfedges);
	
		int edge = model->surfedges[i];

		const mvertex_t* v1;
		const mvertex_t* v2;

		if (edge >= 0)
		{
			v1 = &model->vertexes[model->edges[edge].v[0]];
			v2 = &model->vertexes[model->edges[edge].v[1]];
		}
		else
		{
			v1 = &model->vertexes[model->edges[-edge].v[1]];	
			v2 = &model->vertexes[model->edges[-edge].v[0]];
		}

		// Check for a crossing.

		if ((v1->position[y] - point[y] >= 0) !=
			(v2->position[y] - point[y] >= 0))
		{

			float dx = v1->position[x] - v2->position[x];
			float dy = v1->position[y] - v2->position[y];

			if (v2->position[x] - (v1->position[y] - point[y]) * dx / dy >= point[x])
			{

				++crossings;

				// For a convex polygon, a straight line can only intersect the
				// perimeter a maximum of two times.
			
				if (crossings == 2)
				{
					return false;
				}
			
			}
		}
	
	}

	return (crossings % 2) == 1;

}


/**
 * Computes the area of intersection between a circle and a surface. The
 * circle and the surface must intersect.
 */
float NS_CircleSurfaceIntersectionArea(const model_t* model, const SURFACE* surface, const vec3_t point, float radius)
{

	float area = 0;

	int lastEdge = surface->firstedge + surface->numedges;

	int x = (surface->plane->type + 1) % 3;
	int y = (surface->plane->type + 2) % 3; 

	float radiusSquared = radius * radius;

	for (int i = surface->firstedge; i < lastEdge; ++i)
	{

		ASSERT(i >= 0 && i < model->numsurfedges);
	
		int edge = model->surfedges[i];

		const mvertex_t* v1;
		const mvertex_t* v2;

		if (edge >= 0)
		{
			v1 = &model->vertexes[model->edges[edge].v[0]];
			v2 = &model->vertexes[model->edges[edge].v[1]];
		}
		else
		{
			v1 = &model->vertexes[model->edges[-edge].v[1]];	
			v2 = &model->vertexes[model->edges[-edge].v[0]];
		}

		bool v1Inside = NS_PointToPointDistanceSquared(v1->position, point) < radiusSquared;
		bool v2Inside = NS_PointToPointDistanceSquared(v2->position, point) < radiusSquared;

		if (v1Inside && v2Inside)
		{
			// Add the signed area of triangle v1, v2, origin.
		}
		else if (v1Inside && !v2Inside)
		{
			// Find the interection of v1, v2 with the circle.
			// Add the signed area of triangle v1, intersection, origin
			// chordStart = v1
		}
		else if (!v1Inside && v2Inside)
		{
			// Find the interection of v1, v2 with the circle.
			// Add the signed area of triangle intersection, v2, origin
			// Add the area partitioned by the chord chordStart, intersection
		}
		else if (!v1Inside && !v2Inside)
		{
			// Check if v1, v2 intersects the circle.
		}
		
	}
	
	return area;

}


/**
 * Tests if the model structure is a software rendering model structure.
 */
bool NS_IsSoftwareModel(const model_t* model)
{

	if (model->nodes[0].parent != NULL)
	{
		return false;
	}

	const mnode_t* child = model->nodes[0].children[0];

	if (child < model->nodes || child > model->nodes + model->numnodes)
	{
		return false;
	}

	if (child->parent != &model->nodes[0])
	{
		return false;
	}

	return true;

}


int NS_GetAverageNormalInsideSphere(const model_t* model, const NODE* node, const vec3_t point, float radius, vec3_t normal)
{

	// Check for a leaf node.

	if (node->contents < 0)
	{
		return 0;
	}
	
	float distance = NS_PointToPlaneDistance(point, node->plane);

	int numSurfaces = 0;

	if (distance > radius)
	{
		
		// The sphere is completely in front of the plane.

		numSurfaces = NS_GetAverageNormalInsideSphere(model,
			(const NODE*)node->children[0], point, radius, normal);	

	}
	else if (distance < -radius)
	{
		
		// The sphere is completely behind the plane.
	
		numSurfaces = NS_GetAverageNormalInsideSphere(model,
			(const NODE*)node->children[1], point, radius, normal);

	}
	else
	{

		// Compute the projection of the point onto the plane.

		vec3_t projection;

		VectorScale(node->plane->normal, distance, projection);
		VectorAdd(point, projection, projection);

		// Test if the sphere intersects any of the surfaces in this node.

		unsigned short lastSurface = node->firstsurface + node->numsurfaces;
		
		for (unsigned short i = node->firstsurface; i < lastSurface; ++i)
		{
		
			ASSERT(i < model->numsurfaces);
			
			const SURFACE* surface = &((const SURFACE*)model->surfaces)[i];
			ASSERT(surface->plane == node->plane);

			if (NS_PointOnSurface(model, surface, projection) ||
				NS_PointToSurfaceEdgeDistance(model, surface, point) < radius)
			{

				// Compute a weighting factor for the normal based on the
				// intersection area of the sphere and plane.

				float weight = radius * radius - distance * distance;

				vec3_t surfaceNormal;
				
				if (surface->flags & SURF_PLANEBACK)
				{
					VectorScale(node->plane->normal, -weight, surfaceNormal);
				}
				else
				{
					VectorScale(node->plane->normal, weight, surfaceNormal);
				}

				VectorAdd(normal, surfaceNormal, normal);

				++numSurfaces;
			}

		}

		// Recursively test the child nodes.

		numSurfaces += NS_GetAverageNormalInsideSphere(model,
			(const NODE*)node->children[0], point, radius, normal);

		numSurfaces += NS_GetAverageNormalInsideSphere(model,
			(const NODE*)node->children[1], point, radius, normal);

	}

	return numSurfaces;

}


int NS_GetAverageNormalInsideSphere(const model_t* model, const vec3_t point, float radius, vec3_t normal)
{
	
	ASSERT(!NS_IsSoftwareModel(model));

	normal[0] = 0;
	normal[1] = 0;
	normal[2] = 0;

	int numSurfaces = NS_GetAverageNormalInsideSphere(model, (const NODE*)(model->nodes), point, radius, normal);

	if (numSurfaces > 0)
	{
		VectorNormalize(normal);
	}

	return numSurfaces;

}

int NS_UpdateNodeContent(int old_content, int new_content)
{
	if(new_content == CONTENTS_EMPTY)
	{
		return old_content;
	}
	if(old_content == CONTENTS_EMPTY)
	{
		return new_content;
	}
	return max(old_content,new_content);
}

/**
 * Returns world content value in a sphere - solid beats liquid beats empty
 */
int NS_SphereContents(const hull_t *hull, int num, float origin[3], float radius)
{
	ASSERT(radius >= 0);

	int contents = CONTENTS_EMPTY;

	if(hull->firstclipnode <= hull->lastclipnode)
	{
		float d;
		dclipnode_t	*node;
		mplane_t	*plane;
		std::queue<int> nodelist; //list of clipnodes to check
		nodelist.push(num);
		while(!nodelist.empty())
		{
			ASSERT(num >= hull->firstclipnode && num <= hull->lastclipnode);
			num = nodelist.front();
			nodelist.pop();

			node = hull->clipnodes + num;
			plane = hull->planes + node->planenum;
			
			d = NS_PointToPlaneDistance(origin,plane);

			if(d < radius)
			{
				ASSERT(node->children[1] != num);
				if(node->children[1] < 0)
				{
					contents = NS_UpdateNodeContent(contents,node->children[1]);
				}
				else
				{
					nodelist.push(node->children[1]);
				}
			}
			if(d > -radius)
			{
				ASSERT(node->children[0] != num);
				if(node->children[0] < 0)
				{
					contents = NS_UpdateNodeContent(contents,node->children[0]);
				}
				else
				{
					nodelist.push(node->children[0]);
				}
			}
		}
	}

	return contents;
}

/**
 * Returns whether a box is in front, behind, or intersecting a plane
 */
int NS_BoxPlaneIntersectionType(float mins[3], float maxs[3], mplane_t* plane)
{
	int intersect_type = INTERSECT_NONE;
	float orientedMins[3] = { 
		plane->normal[0] < 0 ? maxs[0] : mins[0], 
		plane->normal[1] < 0 ? maxs[1] : mins[1], 
		plane->normal[2] < 0 ? maxs[2] : mins[2]
	};
	float orientedMaxes[3] = { 
		plane->normal[0] < 0 ? mins[0] : maxs[0], 
		plane->normal[1] < 0 ? mins[1] : maxs[1], 
		plane->normal[2] < 0 ? mins[2] : maxs[2]
	};
	intersect_type |= (NS_PointToPlaneDistance(orientedMins,plane) < 0) ? INTERSECT_BACK : INTERSECT_FRONT;
	intersect_type |= (NS_PointToPlaneDistance(orientedMaxes,plane) < 0) ? INTERSECT_BACK : INTERSECT_FRONT;
	return intersect_type;
}

/**
 * Returns world content value in a box - solid beats liquid beats empty
 */
int NS_BoxContents(const hull_t *hull, int num, float mins[3], float maxs[3])
{
	ASSERT(mins[0] <= maxs[0]);
	ASSERT(mins[1] <= maxs[1]);
	ASSERT(mins[2] <= maxs[2]);

	int contents = CONTENTS_EMPTY;

	if(hull->firstclipnode <= hull->lastclipnode)
	{
		int intersection_type;
		dclipnode_t	*node;
		mplane_t	*plane;
		std::queue<int> nodelist; //list of clipnodes to check
		nodelist.push(num);
		while(!nodelist.empty())
		{
			ASSERT(num >= hull->firstclipnode && num <= hull->lastclipnode);
			num = nodelist.front();
			nodelist.pop();

			node = hull->clipnodes + num;
			plane = hull->planes + node->planenum;

			intersection_type = NS_BoxPlaneIntersectionType(mins,maxs,plane);

			if(intersection_type & INTERSECT_BACK)
			{
				ASSERT(node->children[1] != num);
				if(node->children[1] < 0)
				{
					contents = NS_UpdateNodeContent(contents,node->children[1]);
				}
				else
				{
					nodelist.push(node->children[1]);
				}
			}
			if(intersection_type & INTERSECT_FRONT)
			{
				ASSERT(node->children[0] != num);
				if(node->children[0] < 0)
				{
					contents = NS_UpdateNodeContent(contents,node->children[0]);
				}
				else
				{
					nodelist.push(node->children[0]);
				}
			}
		}
	}
	return contents;
}


int NS_PointContents(const hull_t *hull, int num, float p[3])
{
	float		d;
	dclipnode_t	*node;
	mplane_t	*plane;

    if (hull->firstclipnode > hull->lastclipnode)
    {
        return CONTENTS_EMPTY;
    }

	while (num >= 0)
	{
		ASSERT(num >= hull->firstclipnode && num <= hull->lastclipnode);
	
		node = hull->clipnodes + num;
		plane = hull->planes + node->planenum;
		
		if (plane->type < 3)
			d = p[plane->type] - plane->dist;
		else
			d = DotProduct (plane->normal, p) - plane->dist;

		if (d < 0)
		{
			ASSERT(node->children[1] != num);
			num = node->children[1];
		}
		else
		{
			ASSERT(node->children[0] != num);
			num = node->children[0];
		}
	}
	
	return num;
}

bool NS_TraceLine(const hull_t* hull, int num, float p1f, float p2f, vec3_t p1, vec3_t p2, trace_t* trace)
{

	//const float DIST_EPSILON = 0.03125f;
    const float DIST_EPSILON = 0.1f;

	dclipnode_t	*node;
	mplane_t	*plane;
	float		t1, t2;
	float		frac;
	int			i;
	vec3_t		mid;
	int			side;
	float		midf;

// check for empty
	if (num < 0)
	{
		if (num != CONTENTS_SOLID)
		{
			trace->allsolid = false;
			if (num == CONTENTS_EMPTY)
				trace->inopen = true;
			else
				trace->inwater = true;
		}
		else
			trace->startsolid = true;
		return true;		// empty
	}

	ASSERT(num >= hull->firstclipnode && num <= hull->lastclipnode);

//
// find the point distances
//
	node = hull->clipnodes + num;
	plane = hull->planes + node->planenum;

	if (plane->type < 3)
	{
		t1 = p1[plane->type] - plane->dist;
		t2 = p2[plane->type] - plane->dist;
	}
	else
	{
		t1 = DotProduct (plane->normal, p1) - plane->dist;
		t2 = DotProduct (plane->normal, p2) - plane->dist;
	}
	
	if (t1 >= 0 && t2 >= 0)
		return NS_TraceLine(hull, node->children[0], p1f, p2f, p1, p2, trace);
	if (t1 < 0 && t2 < 0)
		return NS_TraceLine(hull, node->children[1], p1f, p2f, p1, p2, trace);

// put the crosspoint DIST_EPSILON pixels on the near side
	if (t1 < 0)
		frac = (t1 + DIST_EPSILON)/(t1-t2);
	else
		frac = (t1 - DIST_EPSILON)/(t1-t2);
	if (frac < 0)
		frac = 0;
	if (frac > 1)
		frac = 1;
		
	midf = p1f + (p2f - p1f)*frac;
	for (i=0 ; i<3 ; i++)
		mid[i] = p1[i] + frac*(p2[i] - p1[i]);

	side = (t1 < 0);

// move up to the node
	if (!NS_TraceLine(hull, node->children[side], p1f, midf, p1, mid, trace) )
		return false;
	
	if (NS_PointContents(hull, node->children[side^1], mid) != CONTENTS_SOLID)
// go past the node
		return NS_TraceLine(hull, node->children[side^1], midf, p2f, mid, p2, trace);
	
	if (trace->allsolid)
		return false;		// never got out of the solid area
		
//==================
// the other side of the node is solid, this is the impact point
//==================
	if (!side)
	{
		VectorCopy (plane->normal, trace->plane.normal);
		trace->plane.dist = plane->dist;
	}
	else
	{
		VectorSubtract (vec3_origin, plane->normal, trace->plane.normal);
		trace->plane.dist = -plane->dist;
	}

	while (NS_PointContents(hull, hull->firstclipnode, mid) == CONTENTS_SOLID)
	{ // shouldn't really happen, but does occasionally
		frac -= 0.1f;
		if (frac < 0)
		{
			trace->fraction = midf;
			VectorCopy (mid, trace->endpos);
			// backup past 0
			return false;
		}
		midf = p1f + (p2f - p1f)*frac;
		for (i=0 ; i<3 ; i++)
			mid[i] = p1[i] + frac*(p2[i] - p1[i]);
	}

	trace->fraction = midf;
	VectorCopy (mid, trace->endpos);

	return false;

}


void NS_TraceLine(const hull_t* hull, vec3_t srcPoint, vec3_t dstPoint, trace_t* trace)
{

	memset(trace, 0, sizeof(trace_t));
	
	trace->fraction = 1;
	trace->allsolid = true;
	VectorCopy(dstPoint, trace->endpos);

    if (hull->firstclipnode <= hull->lastclipnode)
    {
    	NS_TraceLine(hull, hull->firstclipnode, 0, 1, srcPoint, dstPoint, trace);
    }

	if (trace->allsolid)
    {
		trace->startsolid = true;
    }

	if (trace->startsolid)
    {
		trace->fraction = 0;
    }

}


void NS_TraceLine(vec3_t min, vec3_t max, vec3_t srcPoint, vec3_t dstPoint, trace_t* trace)
{

	memset(trace, 0, sizeof(trace_t));

    vec3_t direction;
    VectorSubtract(dstPoint, srcPoint, direction);
    
    vec3_t tMax;

    tMax[0] = -1;
    tMax[1] = -1;
    tMax[2] = -1;

    vec3_t result;
    bool inside = true;

    // Find candidate planes.

    for (int i = 0; i < 3; ++i)
    {

        if (srcPoint[i] <= min[i])
        {
            
            result[i] = min[i];
            inside = false;

            // Calculate the distances to the candidate planes.
            
            if (direction[i] != 0)
            {
                tMax[i] = (min[i] - srcPoint[i]) / direction[i];
            }
        
        
        }
        else if (srcPoint[i] >= max[i])
        {

            result[i] = max[i];
            inside = false;

            // Calculate the distances to the candidate planes.

            if (direction[i] != 0)
            {
                tMax[i] = (max[i] - srcPoint[i]) / direction[i];
            }

        }
    
    }

    // Check if the trace origin is inside the box.

    if (inside)
    {
        
        trace->startsolid = true;
        trace->fraction   = 0;
	    VectorCopy(srcPoint, trace->endpos);

        return;

    }

    // Get the largest of the tMax's for the final choice of intersection.

    int axis = 0;

    if (tMax[1] > tMax[axis]) axis = 1;
    if (tMax[2] > tMax[axis]) axis = 2;

    // Check that the intersection is actually inside the line segment.

    trace->fraction = 1;
    VectorCopy(dstPoint, trace->endpos);

    if (tMax[axis] <= 0 || tMax[axis] > 1)
    {
        return;
    }

    for (int j = 0; j < 3; ++j)
    {
        if (j != axis)
        {

            result[j] = srcPoint[j] + tMax[axis] * direction[j];

            if (result[j] < min[j] || result[j] > max[j])
            {
                return;
            }
        
        }
    }

    trace->fraction = tMax[axis];
    VectorCopy(result, trace->endpos);

}


bool NS_BoxesOverlap(float origin1[3], float size1[3], float origin2[3], float size2[3])
{

	for (int i = 0; i < 3; ++i)
	{

		float distance = (float)fabs(origin2[i] - origin1[i]);
		
		if (distance > size1[i] + size2[i])
		{
			return false;
		}

	}

	return true;

}

//-----------------------------------------------------------------------------

void NS_GetHitBoxesForEntity(int inEntityIndex, int inMaxNumBoxes, OBBox outBoxes[], int& outNumBoxes, float time)
{

    outNumBoxes = 0;

    NS_AnimationData theAnimationData;
    
    if (!NS_GetEntityAnimationData(inEntityIndex, theAnimationData))
    {
        return;
    }

    studiohdr_t* theModelHeader = theAnimationData.mModelHeader;

    if (theAnimationData.mModelHeader == NULL)
    {
        return;
    }

    if (theModelHeader->numhitboxes > inMaxNumBoxes)
    {
        outNumBoxes = inMaxNumBoxes;
    }
    else
    {
        outNumBoxes = theModelHeader->numhitboxes;
    }

    mstudiobbox_t* theHitBoxes = (mstudiobbox_t*)((byte*)theModelHeader + theModelHeader->hitboxindex);

    NS_Matrix3x4 theBoneMatrix[MAXSTUDIOBONES];
    NS_GetBoneMatrices(theAnimationData, time, theBoneMatrix);

    for (int i = 0; i < outNumBoxes; ++i)
    {
        
        int theBone = theHitBoxes[i].bone;

        for (int r = 0; r < 3; ++r)
        {
            for (int c = 0; c < 3; ++c)
            {
                outBoxes[i].mAxis[c][r] = theBoneMatrix[theBone][r][c];
            }
        }

        outBoxes[i].mOrigin[0] = theBoneMatrix[theBone][0][3];
        outBoxes[i].mOrigin[1] = theBoneMatrix[theBone][1][3];
        outBoxes[i].mOrigin[2] = theBoneMatrix[theBone][2][3];

        vec3_t temp;
        VectorSubtract(theHitBoxes[i].bbmax, theHitBoxes[i].bbmin, temp);
        VectorScale(temp, 0.5, outBoxes[i].mExtents);

        VectorAdd(theHitBoxes[i].bbmax, theHitBoxes[i].bbmin, temp);
        VectorScale(temp, 0.5, temp);
        
        VectorMA(outBoxes[i].mOrigin, temp[0], outBoxes[i].mAxis[0], outBoxes[i].mOrigin);
        VectorMA(outBoxes[i].mOrigin, temp[1], outBoxes[i].mAxis[1], outBoxes[i].mOrigin);
        VectorMA(outBoxes[i].mOrigin, temp[2], outBoxes[i].mAxis[2], outBoxes[i].mOrigin);

    }

}

//-----------------------------------------------------------------------------

float NS_GetIntersection(const OBBox& inBox, const vec3_t inRayOrigin, const vec3_t inRayDirection)
{

    // Ray/Box intersection test from "Real-Time Rendering" by Tomas Moller
    // and Eric Haines.

	float tMin = -AVH_INFINITY;
	float tMax =  AVH_INFINITY;
	
	vec3_t p;
    VectorSubtract(inBox.mOrigin, inRayOrigin, p);
	
	for (int i = 0; i < 3; i++)
    {
		
		float e = DotProduct(inBox.mAxis[i], p);
		float f = DotProduct(inBox.mAxis[i], inRayDirection);
	
		// Check that the ray and the slab are not parallel

		if (fabs(f) > AVH_EPSILON)
        {
			
			float t1 = (e + inBox.mExtents[i]) / f;
			float t2 = (e - inBox.mExtents[i]) / f;
			
			if (t1 > t2)
            {
                std::swap(t1, t2);
			}
			
			if (t1 > tMin)
            {
				tMin = t1;
			}
			
			if (t2 < tMax)
            {
				tMax = t2;
			}
			
			if (tMin > tMax)
            {
				return AVH_INFINITY;
			}

			if (tMax < 0)
            {
				return AVH_INFINITY;
			}

		}
        else if (-e - inBox.mExtents[i] > 0 || -e + inBox.mExtents[i] < 0)
        {
			return AVH_INFINITY;		
		}

	}
	
	if (tMin > 0)
    {
		return tMin;
	}
    else
    {
		return tMax;
	}

}

//-----------------------------------------------------------------------------

float NS_TraceLineAgainstEntity(int inEntityIndex, float inTime, const vec3_t inRayOrigin, const vec3_t inRayDirection)
{

    /*
    // Do an early out test to see if the ray collides with the bounding box.

    NS_AnimationData theAnimationData;
    
    if (!NS_GetEntityAnimationData(inEntityIndex, theAnimationData))
    {
        return AVH_INFINITY;
    }

    OBBox theBoundingBox;

    theBoundingBox.mAxis[0][0] = theAnimationData.mMatrix[0][0];
    theBoundingBox.mAxis[0][1] = theAnimationData.mMatrix[1][0];
    theBoundingBox.mAxis[0][2] = theAnimationData.mMatrix[2][0];

    theBoundingBox.mAxis[1][0] = theAnimationData.mMatrix[0][1];
    theBoundingBox.mAxis[1][1] = theAnimationData.mMatrix[1][1];
    theBoundingBox.mAxis[1][2] = theAnimationData.mMatrix[2][1];

    theBoundingBox.mAxis[2][0] = theAnimationData.mMatrix[0][2];
    theBoundingBox.mAxis[2][1] = theAnimationData.mMatrix[1][2];
    theBoundingBox.mAxis[2][2] = theAnimationData.mMatrix[2][2];
    
    theBoundingBox.mOrigin[0] = theAnimationData.mMatrix[0][3];
    theBoundingBox.mOrigin[1] = theAnimationData.mMatrix[1][3];
    theBoundingBox.mOrigin[2] = theAnimationData.mMatrix[2][3];
    
    vec3_t temp;
    VectorSubtract(theAnimationData.mMins, theAnimationData.mMaxs, temp);
    VectorScale(temp, 0.5, theBoundingBox.mExtents);
    
    VectorAdd(theAnimationData.mMins, theAnimationData.mMaxs, temp);
    VectorScale(temp, 0.5, theBoundingBox.mOrigin); // Wrong space, but probably good enough.
    
    if (NS_GetIntersection(theBoundingBox, inRayOrigin, inRayDirection) == AVH_INFINITY)    
    {
        return AVH_INFINITY;
    }
    */

    // Do the full hit box test.

    const int kMaxNumBoxes = 255;
    OBBox theBoxes[kMaxNumBoxes];
    int   theNumBoxes;

    NS_GetHitBoxesForEntity(inEntityIndex, kMaxNumBoxes, theBoxes, theNumBoxes, inTime);

    float tMin = AVH_INFINITY;
    
    for (int i = 0; i < theNumBoxes; ++i)
    {
        
        float t = NS_GetIntersection(theBoxes[i], inRayOrigin, inRayDirection);
    
        if (t < tMin)
        {
            tMin = t;
        }
    
    }

    return tMin;

}

//-----------------------------------------------------------------------------

int NS_GetValveHull(int inHull)
{

    int theHull = inHull;
    switch(inHull)
    {
    case 0:
        theHull = 1;
        break;
    case 1:
        theHull = 3;
        break;
    case 2:
        theHull = 0;
        break;
    case 3:
        theHull = 2;
        break;
    default:
        ASSERT(false);
        break;
    }

    return theHull;
}