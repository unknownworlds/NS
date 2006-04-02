#ifndef TURRET_CONST_H
#define TURRET_CONST_H

#define TURRET_SHOTS	2
#define TURRET_RANGE	(100 * 12)
#define TURRET_SPREAD	Vector( 0, 0, 0 )
#define TURRET_TURNRATE	30		//angles per 0.1 second
#define TURRET_MAXWAIT	15		// seconds turret will stay active w/o a target
#define TURRET_MAXSPIN	5		// seconds turret barrel will spin w/o a target
#define TURRET_MACHINE_VOLUME	0.5

typedef enum
{
	TURRET_ANIM_NONE = 0,
		TURRET_ANIM_FIRE,
		TURRET_ANIM_SPIN,
		TURRET_ANIM_DEPLOY,
		TURRET_ANIM_RETIRE,
		TURRET_ANIM_DIE,
} TURRET_ANIM;

#define SF_MONSTER_TURRET_AUTOACTIVATE	32
#define SF_MONSTER_TURRET_STARTINACTIVE	64

#endif