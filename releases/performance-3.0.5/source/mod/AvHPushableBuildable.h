#ifndef AVH_PUSHABLEBUILDABLE_H
#define AVH_PUSHABLEBUILDABLE_H

#include "mod/AvHBaseBuildable.h"

class AvHPushableBuildable : public AvHBaseBuildable
{
public:
	AvHPushableBuildable(AvHTechID inTechID, AvHMessageID inMessageID, char* inClassName, int inUser3);

	void	Precache( void );
	void	Touch ( CBaseEntity *pOther );
	void	Move( CBaseEntity *pMover, int push );
	void	KeyValue( KeyValueData *pkvd );
	void	Use( CBaseEntity *pActivator, CBaseEntity *pCaller, USE_TYPE useType, float value );
	void	EXPORT StopSound( void );
//	virtual void	SetActivator( CBaseEntity *pActivator ) { m_pPusher = pActivator; }

	//virtual int	ObjectCaps( void ) { return (CBaseEntity :: ObjectCaps() & ~FCAP_ACROSS_TRANSITION) | FCAP_CONTINUOUS_USE; }

	inline float GetMaxSpeed(void);

	virtual void SetConstructionComplete();
	
	static	TYPEDESCRIPTION m_SaveData[];

	static char *m_soundNames[3];
	int		m_lastSound;	// no need to save/restore, just keeps the same sound from playing twice in a row
	float	m_maxSpeed;
	float	m_soundTime;


};


#endif