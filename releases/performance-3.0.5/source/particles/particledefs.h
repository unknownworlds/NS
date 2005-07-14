// general.h
//
// Copyright 1998 by David K. McAllister.
//
// This file implements the API calls that are not particle actions.

#ifndef particledefs_h
#define particledefs_h

#include "p_vector.h"

#ifdef WIN32
#include <windows.h>

// removed <<< cgc >>>
//#ifdef PARTICLEDLL_EXPORTS
//#define PARTICLEDLL_API __declspec(dllexport)
//#else
//#define PARTICLEDLL_API __declspec(dllimport)
//#endif
#define PARTICLEDLL_API extern "C"

// removed <<< cgc >>>
#else
#define PARTICLEDLL_API
#endif


#ifdef _WIN32
#pragma warning (disable:4244)
#endif

// Actually this must be < sqrt(MAXFLOAT) since we store this value squared.
#define P_MAXFLOAT 1.0e16f

#ifdef MAXINT
#define P_MAXINT MAXINT
#else
#define P_MAXINT 0x7fffffff
#endif

#define P_EPS 1e-3f

//////////////////////////////////////////////////////////////////////
// Type codes for domains
PARTICLEDLL_API enum PDomainEnum
{
	PDPoint = 0, // Single point
	PDLine = 1, // Line segment
	PDTriangle = 2, // Triangle
	PDPlane = 3, // Arbitrarily-oriented plane
	PDBox = 4, // Axis-aligned box
	PDSphere = 5, // Sphere
	PDCylinder = 6, // Cylinder
	PDCone = 7, // Cone
	PDBlob = 8, // Gaussian blob
	PDDisc = 9, // Arbitrarily-oriented disc
	PDRectangle = 10 // Rhombus-shaped planar region
};

// A single particle
struct Particle
{
	pVector pos;
	pVector posB;
	pVector size;
	pVector vel;
	pVector velB;	// Used to compute binormal, normal, etc.
	pVector color;	// Color must be next to alpha so glColor4fv works.
	float alpha;	// This is both cunning and scary.
	float age;
};

// A group of particles - Info and an array of Particles
struct ParticleGroup
{
	int p_count;		// Number of particles currently existing.
	int max_particles;	// Max particles allowed in group.
	int particles_allocated; // Actual allocated size.
	Particle list[1];	// Actually, num_particles in size
	
	inline void Remove(int i)
	{
		list[i] = list[--p_count];
	}
	
	inline bool Add(const pVector &pos, const pVector &posB,
		const pVector &size, const pVector &vel, const pVector &color,
		const float alpha = 1.0f,
		const float age = 0.0f)
	{
		if(p_count >= max_particles)
			return false;
		else
		{
			list[p_count].pos = pos;
			list[p_count].posB = posB;
			list[p_count].size = size;
			list[p_count].vel = vel;
			list[p_count].velB = vel;	// XXX This should be fixed.
			list[p_count].color = color;
			list[p_count].alpha = alpha;
			list[p_count].age = age;
			p_count++;
			return true;
		}
	}
};


#endif
