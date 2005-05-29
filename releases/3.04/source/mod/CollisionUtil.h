#ifndef COLLISION_UTIL_H
#define COLLISION_UTIL_H

extern const float AVH_INFINITY;
extern const float AVH_EPSILON;

#include <vector>
#include "pm_defs.h"
#include "CollisionChecker.h"

/**
 * Computes the average of all the surface normals inside a sphere. Returns the
 * number of surfaces that were tested in the process of computing the normal.
 */
int NS_GetAverageNormalInsideSphere(const model_t* model, const vec3_t point, float radius, vec3_t normal);

/**
 * Traces a line segment against the specified hull.
 */
void NS_TraceLine(const hull_t* hull, float srcPoint[3], float dstPoint[3], trace_t* trace);

/**
 * Traces a line segment against an AABB.
 */
void NS_TraceLine(vec3_t min, vec3_t max, vec3_t srcPoint, vec3_t dstPoint, trace_t* trace);

/**
 * Tests if two axis-aligned bounding boxes overlap.
 */
bool NS_BoxesOverlap(float origin1[3], float size1[3], float origin2[3], float size2[3]);

/**
 * Returns world content type at given point
 */
int NS_PointContents(const hull_t *hull, int num, float p[3]);

/**
 * Returns minimum world content type inside sphere -- precedence: solid, then liquid types, then empty
 */
int NS_SphereContents(const hull_t *hull, int num, float origin[3], float radius);

const static int INTERSECT_NONE = 0;
const static int INTERSECT_FRONT = 1;
const static int INTERSECT_BACK = 2;
const static int INTERSECT_BOTH = 3;

/**
 * Returns box / plane intersection type - see constants above
 */
int NS_BoxPlaneIntersectionType(float mins[3], float maxs[3], mplane_t* plane);

/**
 * Returns minimum world content type inside box -- precedence: solid, then liquid types, then empty
 */
int NS_BoxContents(const hull_t *hull, int num, float mins[3], float maxs[3]);

/**
 * inEntityIndex is the index of the entity in edicts in the server or
 * cl_entities in the client.
 */
void NS_GetHitBoxesForEntity(int inEntityIndex, int inMaxNumBoxes, OBBox outBoxes[], int& outNumBoxes, float time);

/**
 * Retuns the distance along a ray at which it intersects an oriented bounding
 * box. If there is no intersection the function returns AVH_INFINITY.
 */
float NS_GetIntersection(const OBBox& inBox, const vec3_t inRayOrigin, const vec3_t inRayDirection);

/**
 * Retuns the distance along a ray at which it intersects an entity.
 * If there is no intersection the function returns AVH_INFINITY.
 */
float NS_TraceLineAgainstEntity(int inEntityIndex, float inTime, const vec3_t inRayOrigin, const vec3_t inRayDirection);

/**
 * Convert "our" hull into VALVe hull
 */
int NS_GetValveHull(int inHull);

#endif