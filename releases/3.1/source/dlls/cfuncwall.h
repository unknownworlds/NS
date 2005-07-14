#ifndef CFUNC_WALL_H
#define CFUNC_WALL_H

#include "extdll.h"
#include "util.h"
#include "cbase.h"
#include "doors.h"

/*QUAKED func_wall (0 .5 .8) ?
This is just a solid wall if not inhibited
*/
class CFuncWall : public CBaseEntity
{
public:
	void	Spawn( void );
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );

	// Bmodels don't go across transitions
	virtual int	ObjectCaps( void ) { return CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION; }
};

#endif