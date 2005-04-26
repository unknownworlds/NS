#ifndef ROACH_H
#define ROACH_H

#include	"cbase.h"

#define kRoachClassName "Roach"

//=========================================================
// Monster's Anim Events Go Here
//=========================================================
class CRoach : public CBaseMonster
{
public:
	void Spawn( void );
	void Precache( void );
	void SetYawSpeed( void );
	void EXPORT MonsterThink ( void );
	void Move ( float flInterval );
	void PickNewDest ( int iCondition );
	void EXPORT Touch ( CBaseEntity *pOther );
	void Killed( entvars_t *pevAttacker, int iGib );

	float	m_flLastLightLevel;
	float	m_flNextSmellTime;
	int		Classify ( void );
	void	Look ( int iDistance );
	int		ISoundMask ( void );
	
	// UNDONE: These don't necessarily need to be save/restored, but if we add more data, it may
	BOOL	m_fLightHacked;
	int		m_iMode;
	// -----------------------------
};

#endif