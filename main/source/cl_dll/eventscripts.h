// eventscripts.h
#if !defined ( EVENTSCRIPTSH )
#define EVENTSCRIPTSH

#include "common/const.h"

// defaults for clientinfo messages
#define	DEFAULT_VIEWHEIGHT	28

#ifndef VEC_DUCK_VIEW
#define VEC_DUCK_VIEW 12
#endif

#define FTENT_FADEOUT			0x00000080

#include "common/damagetypes.h"
#include "pm_shared/pm_defs.h"

// Some of these are HL/TFC specific?
void EV_EjectBrass( float *origin, float *velocity, float rotation, int model, int soundtype );
void EV_GetGunPosition( struct event_args_s *args, float *pos, float *origin );
void EV_GetDefaultShellInfo( struct event_args_s *args, float *origin, float *velocity, float *ShellVelocity, float *ShellOrigin, float *forward, float *right, float *up, float forwardScale, float upScale, float rightScale );
qboolean EV_IsLocal( int idx );
qboolean EV_IsSpec( int idx );
qboolean EV_IsPlayer( int idx );
void EV_CreateTracer( float *start, float *end );

struct cl_entity_s *GetEntity( int idx );
struct cl_entity_s *GetViewEntity( void );
physent_t* GetPhysEntity(int inPhysIndex);
void DoCenterPrint(char* inString);
void EV_MuzzleFlash( void );

#endif // EVENTSCRIPTSH
